// Basic demo for readings from Adafruit BNO08x via SPI on Arduino Due
#include <Adafruit_BNO08x.h>

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

struct SensorData {
    float real, i, j, k; // BNO08x orientation data
    unsigned long start_time;
    unsigned long stop_time;
    unsigned long values[100];
    float average_time_per_conversion;
};

unsigned int i; // index for storing ADC values

// -----------------------------
//   CUSTOMIZABLE PARAMETERS
const float orientationCutoff = 0.5; // Minimum angle change (in radians) to trigger a pulse
#define MAX_SAMPLES 30 // Adjust based on memory needs
#define ADC_SAMPLES 200 // Number of ADC samples to capture per pulse
// -----------------------------

// define an array to store multiple readings (e.g. for multiple pulses)
SensorData SensorDataArray[MAX_SAMPLES]; 
unsigned int SensorDataIndex = 1;

void setup(void) {
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

    INA_LOW;
    INB_HIGH;

    pinMode(pinENA, OUTPUT);
    digitalWrite(pinENA, HIGH);

    // -- Set up the analog pin for reading ultrasound sensor data --
    adc_init(ADC, SystemCoreClock, 21000000UL, ADC_STARTUP_FAST);
  
    ADC->ADC_MR |= 0x80;  //set free running mode on ADC 
    //ADC->ADC_CR=2;
    ADC->ADC_CHER = 0x80; //enable ADC on pin A0
}

// Here is where you define the sensor outputs you want to receive
void setReports(void) {
    Serial.println("Setting desired reports");
    if (! bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
        Serial.println("Could not enable game vector");
    }
}

float AngleRelativeChange(float newReal, float newI, float newJ, float newK) {
    // 1. Calculate Dot Product
    float dotProduct = (SensorDataArray[SensorDataIndex-1].real * newReal) +
                       (SensorDataArray[SensorDataIndex-1].i * newI) +
                       (SensorDataArray[SensorDataIndex-1].j * newJ) +
                       (SensorDataArray[SensorDataIndex-1].k * newK);

    // 2. Take absolute value to handle quaternion "double cover" (aliasing)
    dotProduct = fabs(dotProduct);

    // 3. Clamp the value to [0.0, 1.0] to prevent acos() from returning NaN
    if (dotProduct > 1.0f) dotProduct = 1.0f;

    // 4. Calculate the angle in radians
    // Formula: angle = 2 * acos(dotProduct)
    float angle = 2.0f * acos(dotProduct);

    return angle; 
}

bool convertQuaternionsToDegrees(float newReal) {
    return 0;
}

void loop() {
    delay(100);  // may comment out later for faster response, but helps with serial printing
    if (bno08x.wasReset()) {
        setReports();
    }

    // 2. Check for data (non-blocking)
    if (!bno08x.getSensorEvent(&sensorValue)) {
        return; 
    }

    if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR) {
        float orientationChange = AngleRelativeChange(sensorValue.un.gameRotationVector.real, sensorValue.un.gameRotationVector.i, sensorValue.un.gameRotationVector.j, sensorValue.un.gameRotationVector.k);
        if (orientationChange > orientationCutoff) { // Only trigger if there's a significant change in orientation
            Serial.print("Significant orientation change detected: ");
            Serial.println(orientationChange);

            // --- TRIGGER PULSE IMMEDIATELY ---
            // This minimizes the "latency" between data arrival and hardware trigger
            INA_HIGH;
            INB_HIGH;
            __asm__ __volatile__("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"); // 100ns-ish pulse
            INA_LOW;
            INB_LOW;
            __asm__ __volatile__("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t");
            INA_LOW;
            INB_HIGH; // Reset to idle state
            
            // --- LOG DATA AFTER TRIGGER ---
            // Store intervals of ADC data
            SensorDataArray[SensorDataIndex].start_time = micros(); 
            for(i=0;i<ADC_SAMPLES;i++)
            {
                while((ADC->ADC_ISR & 0x80)==0); // wait for conversion
                SensorDataArray[SensorDataIndex].values[i]=ADC->ADC_CDR[7]; //get values
            }
            SensorDataArray[SensorDataIndex].stop_time = micros();
            SensorDataArray[SensorDataIndex].average_time_per_conversion = (float)(SensorDataArray[SensorDataIndex].stop_time-SensorDataArray[SensorDataIndex].start_time)/ADC_SAMPLES;

            // Store the BNO08x data as well
            SensorDataArray[SensorDataIndex].real = sensorValue.un.gameRotationVector.real;
            SensorDataArray[SensorDataIndex].i = sensorValue.un.gameRotationVector.i;
            SensorDataArray[SensorDataIndex].j = sensorValue.un.gameRotationVector.j;
            SensorDataArray[SensorDataIndex].k = sensorValue.un.gameRotationVector.k;

            // Print timing results for ADC readings --> only for debugging, not for real-time use
            Serial.print("Total time: ");
            Serial.println(SensorDataArray[SensorDataIndex].stop_time-SensorDataArray[SensorDataIndex].start_time); 
            Serial.print(" microseconds");

            Serial.print("number of samples: ");
            Serial.println(ADC_SAMPLES);

            Serial.print("Average time per conversion: ");
            Serial.println(SensorDataArray[SensorDataIndex].average_time_per_conversion);
            Serial.print(" microseconds");

            // Now that the time-sensitive pulse is done, we can do the slow printing --> only for debugging, not for real-time use
            Serial.print("Fired! r: ");
            Serial.println(sensorValue.un.gameRotationVector.real);
            Serial.print(" i: ");
            Serial.println(sensorValue.un.gameRotationVector.i);  
            Serial.print(" j: ");
            Serial.println(sensorValue.un.gameRotationVector.j);
            Serial.print(" k: ");
            Serial.println(sensorValue.un.gameRotationVector.k);

            SensorDataIndex++; // Increment index for next set of readings

        } else {
            return; // Skip if change is not significant
        }
    }

    if (SensorDataIndex >= MAX_SAMPLES) {
        Serial.println("Max samples reached, stopping data collection.");
        while (1); // Stop further processing
    }
}