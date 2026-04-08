#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#define ADC_SAMPLES 200
const int screenWidth = 480;
const int screenHeight = 320;

struct Scan1D {
    float roll, pitch, yaw; // BNO08x orientation data
    unsigned long total_time;
    uint16_t values[ADC_SAMPLES];
    float average_time_per_conversion;
};

class SingleScan {
    private:
        static constexpr int MaxScanSize = 200; 
        uint16_t colorLUT[1024];
        int gridOffsetY = -10;
        Scan1D scanArray[MaxScanSize]; // Buffer to hold up to 30 scans (adjust as needed)
        int currentScanIndex = 0;
    
    public:
        void begin(){
            initColorConversionTable(); // Precompute color values for intensity mapping
        };

        // Map 0 to black, middle values to shades of blue, and 1023 to white
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
            // Store the new scan data in the buffer
            if (currentScanIndex < MaxScanSize) {
                scanArray[currentScanIndex] = newScan;
                currentScanIndex++;
            }
            return 0;
        }

        // Adjusts intensity based on depth (r) to account for signal attenuation.
        int applyTimeGainCompensation(){
            for (int i = 0; i < ADC_SAMPLES; i++) {
                float gain = 1.0f + (i / (float)ADC_SAMPLES); // Simple linear TGC
                scanArray[currentScanIndex].values[i] = (uint16_t)(scanArray[currentScanIndex].values[i] * gain);
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
            // tft.drawLine(5, scanStartYPos + gridOffsetY, scanStartXPos * 2, scanStartYPos + gridOffsetY, tft.color565(255, 255, 255));

            // draw line on the left of the screen
            tft.drawLine(scanStartXPos * 2, scanStartYPos + gridOffsetY, scanStartXPos * 2, screenHeight - 5, tft.color565(255, 255, 255));

            // draw markers on the top line every 30 pixels starting from (5, scanStartYPos + gridOffsetY) to (scanStartXPos * 2, scanStartYPos + gridOffsetY)
            // for (int i = 5; i <= scanStartXPos * 2; i=i+30) {
            //     // draw a small line going downwards from point x = i
            //     tft.drawLine(i, scanStartYPos + gridOffsetY, i, scanStartYPos + gridOffsetY + 5, tft.color565(255, 255, 255));
            // }

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
            tft.print(scanArray[0].average_time_per_conversion); // TODO: fix accuracy
            tft.println(" us");

            tft.setCursor(xPos * 2 + 10, yPos + lineHeight + gridOffsetY);
            tft.print("Total Time: ");
            tft.print(scanArray[0].total_time);
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

            // display scan name and number at (xPos * 2 + 50, yPos - 20) in larger font
            tft.setCursor(xPos * 2 + 50, yPos + gridOffsetY - 20);
            tft.setTextFont(2);
            tft.print("Scan 321");

            return 0;
        }

        // display which scan we are using (e.g. max intensity, average, etc.) at the top of the screen
        void displayScanTypes(int xPos, int yPos, const char* scanType){
            tft.setCursor(xPos, yPos - 20);
            tft.setTextFont(2);
            tft.print(scanType);
        }

        // Average scan values across multiple pings, only average the intensity values
        Scan1D averageScan(){
            Scan1D averagedScan;
            averagedScan.roll = scanArray[0].roll;
            averagedScan.pitch = scanArray[0].pitch;
            averagedScan.yaw = scanArray[0].yaw;
            averagedScan.total_time = scanArray[0].total_time;
            averagedScan.average_time_per_conversion = scanArray[0].average_time_per_conversion; // Assuming this is the same across scans for simplicity

            for (int i = 0; i < ADC_SAMPLES; i++) {
                unsigned long sum = 0;
                for (int j = 0; j < currentScanIndex; j++) {
                    sum += scanArray[j].values[i];
                }
                averagedScan.values[i] = sum / currentScanIndex;
            }
            return averagedScan;
        }

        // calculate scan with maxiumum total intensity ignoring the first 10 samples to avoid near-field noise
        Scan1D maxIntensityScan(){
            Scan1D maxScan;
            unsigned long maxIntensity = 0;

            for (int j = 0; j < currentScanIndex; j++) {
                unsigned long totalIntensity = 0;
                for (int i = 10; i < ADC_SAMPLES; i++) { // Start from 10 to ignore near-field noise
                    totalIntensity += scanArray[j].values[i];
                }
                if (totalIntensity > maxIntensity) {
                    maxIntensity = totalIntensity;
                    maxScan = scanArray[j];
                }
            }
            return maxScan;
        }

        int scanConvert(int scanStartXPos, int scanStartYPos, int gradientXPos, int gradientYPos, int scanLength, int scanWidth){
            // calculate constant to adjust the radius of the scan to fit within the display area
            float lengthScale = (float)scanLength / ADC_SAMPLES;

            int scanSeperationDistance = 60; // Distance between each scan in pixels

            Scan1D averagedScan = averageScan();
            Scan1D maxScan = maxIntensityScan();
        
            // for each sample in the current scan, calculate x,y and display it on the screen with the appropriate color based on intensity
            for (int i = 0; i < ADC_SAMPLES; i++) {
                // Assign color based on intensity
                uint16_t color1 = convertToColor(averagedScan.values[i]);
                uint16_t color2 = convertToColor(maxScan.values[i]);

                // each scan is displayed as a vertical bar seperated by a small gap
                tft.fillRect(scanStartXPos - scanSeperationDistance - scanWidth/2, scanStartYPos + lengthScale * i, scanWidth, lengthScale, color1);
                tft.fillRect(scanStartXPos + scanSeperationDistance - scanWidth/2, scanStartYPos + lengthScale * i, scanWidth, lengthScale, color2);
            }
            
            // display scan types above each scan
            displayScanTypes(scanStartXPos - scanSeperationDistance - scanWidth/2, scanStartYPos, "Average");
            displayScanTypes(scanStartXPos + scanSeperationDistance - scanWidth/2, scanStartYPos, "Max Intensity");

            // intesity gradient
            displayIntensityGradient(gradientXPos, gradientYPos); 

            // draw grid lines for reference
            displayMarkers(scanStartXPos, scanStartYPos); // Optional: draw grid lines for reference

            // display scan stats
            displayScanStats(scanStartXPos, scanStartYPos);

            return 0;
        }
    };


SingleScan myScan1D;

void setup(void) {
    tft.init();
    tft.setRotation(1); // Landscape
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
}

// Fill the scanArray with example data for testing
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

void loop() {
    // initialize the scan system
    myScan1D.begin();

    // generate example data and register it in the scan system
    for (int i = 0; i < 10; i++) { // Simulate receiving 100 scans
        myScan1D.registerEcho(generateExampleData());
    }

    // define scan display parameters
    int scanStartXPos = 160;
    int scanStartYPos = 30;
    int gradientXPos = 5;
    int gradientYPos = 24;
    int scanLength = 300;
    int scanWidth = 100;

    myScan1D.scanConvert(scanStartXPos, scanStartYPos, gradientXPos, gradientYPos, scanLength, scanWidth);

    displayButtons();

    while(1);
}