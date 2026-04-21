#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI(); 

#define ADC_SAMPLES 200
const int screenWidth = 480;
const int screenHeight = 320;

// Basic demo for readings from Adafruit BNO08x via SPI on Arduino Due
#include <Adafruit_BNO08x.h>

//   CUSTOMIZABLE PARAMETERS
const float orientationCutoff = 0.5; // Minimum angle change (in radians) to trigger a pulse
#define MAX_SAMPLES 50 // Adjust based on memory needs
#define ADC_SAMPLES 200 // Number of ADC samples to capture per pulse
const float angleCutoff = 1.0; // Minimum roll change in degrees to trigger a pulse
// -----------------------------

// --- PIN DEFINITIONS ---
#define BNO08X_CS  6    // Moved from 10
#define BNO08X_INT 9    
#define BNO08X_RST 8    // Moved from 5

// Pins used as 3.3V power for mode selection
#define P1_MODE_PIN 12 
#define P0_MODE_PIN 13

Adafruit_BNO08x bno08x(BNO08X_RST);
sh2_SensorValue_t sensorValue;

// Corrected Register Macros for Arduino Due
// Pin 5  = PC25
#define INA_HIGH    (REG_PIOC_SODR = (1 << 25))
#define INA_LOW     (REG_PIOC_CODR = (1 << 25))

const int pinENA = 7;   // Output Enable (OE)

// Pin 7  = PA16 ????? for some reason we need this
#define ENA_HIGH    (REG_PIOA_SODR = (1 << 16))
#define ENA_LOW     (REG_PIOA_CODR = (1 << 16))

// Pin 10 = PA28
#define INB_HIGH    (REG_PIOA_SODR = (1 << 28))
#define INB_LOW     (REG_PIOA_CODR = (1 << 28))

int analogPin = A0; // output from ultrasound sensor connected to A0

struct Scan1D {
    float roll, pitch, yaw; // BNO08x orientation data
    unsigned long total_time;
    uint16_t values[ADC_SAMPLES];
    float average_time_per_conversion;
};

struct EulerAngles {
    float roll, pitch, yaw;
};

unsigned int i; // index for storing ADC values

// Here is where you define the sensor outputs you want to receive
void setReports(void) {
    Serial.println("Setting desired reports");
    if (! bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
        Serial.println("Could not enable game vector");
    }
}

EulerAngles quaternionsToDegrees(float qr, float qi, float qj, float qk) {
    EulerAngles angles;

    // Roll (x-axis rotation)
    float sinr_cosp = 2 * (qr * qi + qj * qk);
    float cosr_cosp = 1 - 2 * (qi * qi + qj * qj);
    angles.roll = atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = 2 * (qr * qj - qk * qi);
    if (abs(sinp) >= 1)
        angles.pitch = copysign(M_PI / 2, sinp); // Use 90 degrees if out of range
    else
        angles.pitch = asin(sinp);

    // Yaw (z-axis rotation)
    float siny_cosp = 2 * (qr * qk + qi * qj);
    float cosy_cosp = 1 - 2 * (qj * qj + qk * qk);
    angles.yaw = atan2(siny_cosp, cosy_cosp);

    // Convert Radians to Degrees
    angles.roll  *= 180.0 / M_PI;
    angles.pitch *= 180.0 / M_PI;
    angles.yaw   *= 180.0 / M_PI;

    return angles;
}

class SectorScan {
    private:
        // Use 'static constexpr' to allow these to size the arrays below
        static constexpr int MaxScanSize = 200; 
        static constexpr int angleRange = 91; // Total slots for -45 to 45 (inclusive)

        const int angleTopBound = -30;
        const int angleBottomBound = 30;

        float rollOffset, pitchOffet, yawOffset;

        // Arrays and Buffers
        Scan1D scan2D[MaxScanSize]; 
        float lutSin[angleRange]; 
        float lutCos[angleRange];
        uint16_t colorLUT[1024];
        
        int currentScanIndex = 0;

        int gridOffsetY = -10;

        int firstScan = 0;
    
    public:
        void begin(){
            initLUT(); // Precompute sine and cosine values for angles -90 to 90
            initColorConversionTable(); // Precompute color values for intensity mapping
        };

        void initLUT(){
            for (int a = angleTopBound; a <= angleBottomBound; a++) {
                int index = a - angleTopBound; // Maps -45 to 0, 45 to 90
                lutSin[index] = sinf(a * (M_PI / 180.0f));
                lutCos[index] = cosf(a * (M_PI / 180.0f));
            }
        }

        // Map 0 to black, middle values to shades of blue, and 1023 to white. Using a LUT makes this function a simple
        void initColorConversionTable(){
            // Black --> blue gradient for the lower half of the intensity range
             for (int i = 0; i <= 1022 / 2; i++) {
                uint8_t blue = (uint8_t)((i / 1023.0f) * 255);
                colorLUT[i] = tft.color565(0, 0, blue);
            }
            // Blue --> white gradient for the upper half of the intensity range
             for (int i = 1023 / 2 + 1; i <= 1023; i++) {
                uint8_t blue = (uint8_t)((i / 1023.0f) * 255);
                uint8_t white = (uint8_t)(((i - (1023 / 2)) / (1023 / 2.0f)) * 255);
                colorLUT[i] = tft.color565(white, white, blue);
             }
        }

        // Receives a new "ping" and stores it in a buffer.
        int registerEcho(Scan1D newScan){
            if(firstScan == 0){
                firstScan = 1;
                rollOffset = newScan.roll;
                pitchOffet = newScan.pitch;
                yawOffset = newScan.yaw;
            }

            // Store the new scan data in the buffer
            if (currentScanIndex < MaxScanSize) {
                // apply offset
                newScan.roll = newScan.roll - rollOffset;
                newScan.pitch = newScan.pitch - pitchOffet;
                newScan.yaw = newScan.yaw - yawOffset;

                // save scan
                scan2D[currentScanIndex] = newScan;
                currentScanIndex++;
            }
            return 0;
        }

        void polarToCartesian(float angle, int radius, int &x, int &y, int scanStartXPos, int scanStartYPos){
            // In polarToCartesian
            int lutIndex = (int)angle + angleBottomBound;
            // Ensure angle is constrained to between angleTopBound and angleBottomBound (e.g. -45 to 45)
            lutIndex = constrain(lutIndex, 0, angleBottomBound - angleTopBound);
            x = scanStartXPos + (radius * lutSin[lutIndex]);
            y = scanStartYPos + (radius * lutCos[lutIndex]);
        }

        // Adjusts intensity based on depth (r) to account for signal attenuation.
        int applyTimeGainCompensation(){
            for (int i = 0; i < ADC_SAMPLES; i++) {
                float gain = 1.0f + (i / (float)ADC_SAMPLES); // Simple linear TGC
                scan2D[currentScanIndex].values[i] = (uint16_t)(scan2D[currentScanIndex].values[i] * gain);
            }
            return 0;
        }

        // The Gradient: Map 0 to black, middle values to shades of blue, and 1023 to white. Using a LUT makes this function a simple, lightning-fast array index lookup.
        uint16_t convertToColor(uint16_t intensity){
            return colorLUT[intensity];
        }

        void displayIntensityGradient(int xPos, int yPos){
            // Black --> blue gradient using fillRectVGradient function
            tft.fillRectVGradient(xPos, yPos, 8, 100, tft.color565(0, 0, 0), tft.color565(0, 0, 255));
            // Blue --> white gradient using fillRectVGradient function
            tft.fillRectVGradient(xPos, yPos + 100, 8, 100, tft.color565(0, 0, 255), tft.color565(255, 255, 255));
        }

        int displayMarkers(int scanStartXPos, int scanStartYPos){
            // draw line on top of the screen
            tft.drawLine(5, scanStartYPos + gridOffsetY, scanStartXPos * 2, scanStartYPos + gridOffsetY, tft.color565(255, 255, 255));

            // draw line on the left of the screen
            tft.drawLine(scanStartXPos * 2, scanStartYPos + gridOffsetY, scanStartXPos * 2, screenHeight - 5, tft.color565(255, 255, 255));

            // draw markers on the top line every 30 pixels starting from (5, scanStartYPos + gridOffsetY) to (scanStartXPos * 2, scanStartYPos + gridOffsetY)
            for (int i = 5; i <= scanStartXPos * 2; i=i+30) {
                // draw a small line going downwards from point x = i
                tft.drawLine(i, scanStartYPos + gridOffsetY, i, scanStartYPos + gridOffsetY + 5, tft.color565(255, 255, 255));
            }

            // draw markers on the left line every 30 pixels starting from (scanStartXPos * 2, scanStartYPos + gridOffsetY) to (scanStartXPos * 2, screenHeight - 5)
             for (int y = scanStartYPos + gridOffsetY; y <= screenHeight - 5; y=y+30) {
                // draw a small line going leftwards from point y = y
                tft.drawLine(scanStartXPos * 2, y, scanStartXPos * 2 - 5, y, tft.color565(255, 255, 255));
            }
            return 0;
        }

        // displays several useful metrics to the right of the scan, such as:
        // time per conversion, total scan time, probe frequency (5MHz), Specified medium (e.g. soft tissue), and depth (e.g. 10cm), battery life, Username, etc.
        int displayScanStats(int xPos, int yPos){
            // Example values for testing
            float timePerConversion = 5.0f; // microseconds
            float totalScanTime = 1000.0f; // microseconds
            float probeFrequency = 5.0f; // MHz
            const char* medium = "Soft Tissue";
            float depth = 10.0f; // cm
            int batteryLife = 80; // percent
            const char* username = "jph227";


            int lineHeight = 20; // Adjust as needed for spacing

            tft.setCursor(xPos * 2 + 10, yPos + gridOffsetY);
            tft.setTextColor(tft.color565(255, 255, 255));
            tft.setTextFont(2);
            tft.print("Sampling Rate: ");
            tft.print(scan2D[0].average_time_per_conversion); // TODO: fix accuracy
            tft.println(" us");

            tft.setCursor(xPos * 2 + 10, yPos + lineHeight + gridOffsetY);
            tft.print("Total Time: ");
            tft.print(scan2D[0].total_time);
            tft.println(" us");

            tft.setCursor(xPos * 2 + 10, yPos + lineHeight*2 + gridOffsetY);
            tft.print("FREQ: ");
            tft.print(probeFrequency);
            tft.println(" MHz");
            
            tft.setCursor(xPos * 2 + 10, yPos + lineHeight*3 + gridOffsetY);
            tft.print("Medium: ");
            tft.println(medium);

            tft.setCursor(xPos * 2 + 10, yPos + lineHeight*4 + gridOffsetY);
            tft.print("Depth: ");
            tft.print(depth);
            tft.println(" cm");

            tft.setCursor(xPos * 2 + 10, yPos + lineHeight*5 + gridOffsetY);
            tft.print("Battery: ");
            tft.print(batteryLife);
            tft.println("%");

            tft.setCursor(xPos * 2 + 10, yPos + lineHeight*6 + gridOffsetY);
            tft.print("Name: ");
            tft.println("Scan 321");

            tft.setCursor(xPos * 2 + 10, yPos + lineHeight*7 + gridOffsetY);
            tft.print("User: ");
            tft.println(username);

            // display scan name and number at (xPos, yPos - 40) in larger font
            tft.setCursor(xPos - 40, yPos + gridOffsetY - 20);
            tft.setTextFont(2);
            tft.print("Sector Scan");

            return 0;
        }

        // The calculates x,y and handles the Fill Holes logic
        int scanConvert(int scanStartXPos, int scanStartYPos, int gradientXPos, int gradientYPos, int scanRadius){
            // calculate constant to adjust the radius of the scan to fit within the display area
            float radiusScale = (float)scanRadius / ADC_SAMPLES;

            // for each scan in the buffer, convert polar to Cartesian and plot on the screen
            for (int s = 0; s < currentScanIndex; s++) {
                for (int i = 0; i < ADC_SAMPLES; i++) {
                    int x, y;
                    polarToCartesian(scan2D[s].roll, i * radiusScale, x, y, scanStartXPos, scanStartYPos); // roll corresponds to angle and i corresponds to radius
                    uint16_t color = convertToColor(scan2D[s].values[i]);
                    tft.drawPixel(x, y, color);
                }
            }

            // intesity gradient
            displayIntensityGradient(gradientXPos, gradientYPos); 

            // draw grid lines for reference
            displayMarkers(scanStartXPos, scanStartYPos); // Optional: draw grid lines for reference

            // display scan stats
            displayScanStats(scanStartXPos, scanStartYPos);

            return 0;
        }
    };


SectorScan myScan;

// Fill the scan2D buffer with example data for testing
Scan1D generateExampleData(){
    Scan1D newScan;
    newScan.roll = random(-30, 30); // Random angle between -30 and 30
    newScan.pitch = 0; // Not used
    newScan.yaw = 0; // Not used 
    newScan.total_time = 100; 
    newScan.average_time_per_conversion = (float)(newScan.total_time) / ADC_SAMPLES; 
    for (int i = 0; i < ADC_SAMPLES; i++) {
        newScan.values[i] = random(0, 1024); // Random intensity between 0 and 1023
    }
    return newScan;    
}

void displayButtons(){
    // back button in bottom right corner
    tft.fillRect(screenWidth - 60, screenHeight - 30, 50, 20, TFT_RED);
    tft.setCursor(screenWidth - 55, screenHeight - 28);
    tft.setTextColor(TFT_WHITE);
    tft.setTextFont(2);
    tft.print("BACK");

    // save button in bottom right conrner to the left of the back button
    tft.fillRect(screenWidth - 120, screenHeight - 30, 50, 20, TFT_MAGENTA);
    tft.setCursor(screenWidth - 115, screenHeight - 28);
    tft.setTextColor(TFT_WHITE);
    tft.setTextFont(2);
    tft.print("SAVE");
}


void setup(void) {
    // init LCD screen
    tft.init();
    tft.setRotation(1); // Landscape
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);

    // -- set up BNO08x for SPI communication --
    Serial.begin(115200);
    while (!Serial) delay(10); 
    Serial.println("Adafruit BNO08x SPI Test!");

    // 1. Power up the Mode Pins (P0 and P1) to enable SPI mode
    pinMode(P1_MODE_PIN, OUTPUT);
    pinMode(P0_MODE_PIN, OUTPUT);
    digitalWrite(P1_MODE_PIN, HIGH);
    digitalWrite(P0_MODE_PIN, HIGH);

    // 2. Small delay to let the voltage stabilize
    delay(10);

    // 3. Initialize SPI communication
    // Note: MISO, MOSI, and SCK are physically connected to the Due ICSP header
    if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
        Serial.println("Failed to find BNO08x chip. Check wiring to ICSP header!");
        while (1) { delay(10); }
    }
        
    Serial.println("BNO08x Found!");

    // Print Product IDs
    for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
        Serial.print("Part ");
        Serial.print(bno08x.prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
    }

    setReports();
    Serial.println("Reading events");
    delay(100);

    // ----- Set up the pulse pins -----
    // Enable Clocks for Port A and Port C (Pin 7 and 10 are on Port A)
    pmc_enable_periph_clk(ID_PIOA);
    pmc_enable_periph_clk(ID_PIOC);

    // Give PIO Controller control and set as Output
    REG_PIOA_PER = (1 << 16) | (1 << 28);
    REG_PIOA_OER = (1 << 16) | (1 << 28);

    REG_PIOC_PER = (1 << 25);
    REG_PIOC_OER = (1 << 25);

    INA_HIGH;
    INB_HIGH; // Start LOW so the first HIGH transition creates a pulse

    pinMode(pinENA, OUTPUT);
    digitalWrite(pinENA, HIGH);

    // -- Set up the analog pin for reading ultrasound sensor data --
    adc_init(ADC, SystemCoreClock, 21000000UL, ADC_STARTUP_FAST);
  
    ADC->ADC_MR |= 0x80;  //set free running mode on ADC 
    //ADC->ADC_CR=2;
    ADC->ADC_CHER = 0x80; //enable ADC on pin A0
}

float lastAngle = 0;
Scan1D currentScan; 
int SensorDataIndex = 0;

void loop() {
    
}

void runSectorScan(){
    // initialize the scan system
    myScan.begin();

    delay(100);  // may comment out later for faster response, but helps with serial printing
    if (bno08x.wasReset()) {
        setReports();
    }

    // 2. Check for data (non-blocking)
    if (!bno08x.getSensorEvent(&sensorValue)) {
        return; 
    }

    if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR) {
        EulerAngles angles = quaternionsToDegrees(sensorValue.un.gameRotationVector.real, sensorValue.un.gameRotationVector.i, sensorValue.un.gameRotationVector.j, sensorValue.un.gameRotationVector.k);
        
        // Check against the last saved index (SensorDataIndex - 1)
        float lastRoll = (lastAngle == 0) ? 0 : lastAngle;

        if (abs(angles.roll - lastRoll) > angleCutoff) {
            Serial.print("Significant roll change detected: ");
            Serial.println(angles.roll);

            // --- TRIGGER PULSE IMMEDIATELY ---
            // This minimizes the "latency" between data arrival and hardware trigger
            INA_HIGH;
            INB_HIGH;
            __asm__ __volatile__("nop\n\t"); // 100ns-ish pulse
            INA_LOW;
            INB_LOW;
            __asm__ __volatile__("nop\n\t");
            INA_LOW;
            INB_HIGH; // Reset to idle state
            
            // --- LOG DATA AFTER TRIGGER ---
            // Store intervals of ADC data
            long start_time = micros(); 
            for(i=0;i<ADC_SAMPLES;i++)
            {
                while((ADC->ADC_ISR & 0x80)==0); // wait for conversion
                currentScan.values[i]=ADC->ADC_CDR[7]; //get values
            }
            currentScan.total_time = micros() - start_time;
            currentScan.average_time_per_conversion = (float)(currentScan.total_time)/ADC_SAMPLES;

            // Store the BNO08x data as well
            currentScan.roll = angles.roll;
            currentScan.pitch = angles.pitch;
            currentScan.yaw = angles.yaw;

            // Print timing results for ADC readings --> only for debugging, not for real-time use
            Serial.print("Total time: ");
            Serial.println(currentScan.total_time); 
            Serial.print(" microseconds");

            Serial.print("number of samples: ");
            Serial.println(ADC_SAMPLES);

            Serial.print("Average time per conversion: ");
            Serial.println(currentScan.average_time_per_conversion);
            Serial.print(" microseconds");

            // Display the orientation data as well
            Serial.print("Orientation - Roll: ");
            Serial.print(angles.roll);
            Serial.print(" Pitch: ");
            Serial.print(angles.pitch);
            Serial.print(" Yaw: ");
            Serial.println(angles.yaw);

            // save data into 2D scan image
            myScan.registerEcho(currentScan);

            // store angle for comparison later
            lastAngle = angles.roll;
            SensorDataIndex++;
        }
    }

    if (SensorDataIndex >= MAX_SAMPLES) {
        Serial.println("Max samples reached, stopping data collection.");
        // (scanStartXPos, scanStartYPos, gradientXPos, gradientYPos, scanRadius)
        myScan.scanConvert(160, 30, 5, 24, 280); // Display the scans

        displayButtons();

        return;
    }
}