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

class SectorScan {
    private:
        // Use 'static constexpr' to allow these to size the arrays below
        static constexpr int MaxScanSize = 200; 
        static constexpr int angleRange = 91; // Total slots for -45 to 45 (inclusive)

        const int angleTopBound = -30;
        const int angleBottomBound = 30;

        // Arrays and Buffers
        Scan1D scan2D[MaxScanSize]; 
        float lutSin[angleRange]; 
        float lutCos[angleRange];
        uint16_t colorLUT[1024];
        
        int currentScanIndex = 0;

        int gridOffsetY = -10;
    
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
            // Store the new scan data in the buffer
            if (currentScanIndex < MaxScanSize) {
                scan2D[currentScanIndex] = newScan;
                currentScanIndex++;
            }
            return 0;
        }

        void polarToCartesian(float angle, int radius, int &x, int &y){
            // In polarToCartesian
            int lutIndex = (int)angle + angleBottomBound;
            // Ensure angle is constrained to between angleTopBound and angleBottomBound (e.g. -45 to 45)
            lutIndex = constrain(lutIndex, 0, angleBottomBound - angleTopBound);
            x = (radius * lutSin[lutIndex]);
            y = (radius * lutCos[lutIndex]);
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

        // Helper function to process one horizontal strip of the scan
        void renderStrip(int yStart, int yEnd, int scanWidth, int scanRadius, int startX, int startY) {
            int stripHeight = yEnd - yStart;
            
            // Local buffers for this chunk (Stored on the stack, cleared each time)
            uint16_t grid[scanWidth][stripHeight];
            uint8_t count[scanWidth][stripHeight];

            // 1. Initialize buffers to zero
            memset(grid, 0, sizeof(grid));
            memset(count, 0, sizeof(count));

            float radiusScale = (float)scanRadius / ADC_SAMPLES;

            // 2. Voxelize: Only store points that fall within this Y-strip
            for (int s = 0; s < currentScanIndex; s++) {
                for (int i = 0; i < ADC_SAMPLES; i++) {
                    int x, y;
                    polarToCartesian(scan2D[s].roll, i * radiusScale, x, y);
                    
                    // Map global Y to local strip Y
                    int localY = y - yStart;

                    // Check if point is within this horizontal chunk
                    if (localY >= 0 && localY < stripHeight) {
                        int gridX = x + (scanWidth / 2);
                        if (gridX >= 0 && gridX < scanWidth) {
                            grid[gridX][localY] += scan2D[s].values[i];
                            count[gridX][localY]++;
                        }
                    }
                }
            }

            // 3. Average and Hole-Fill
            for (int j = 0; j < stripHeight; j++) {
                for (int i = 0; i < scanWidth; i++) {
                    // Average pings that hit the same voxel
                    if (count[i][j] > 1) {
                        grid[i][j] /= count[i][j];
                    }

                    // Hole Filling: If empty, check neighbors (NN Average)
                    if (count[i][j] == 0) {
                        // Bounds check neighbors
                        if (i > 0 && i < scanWidth - 1 && j > 0 && j < stripHeight - 1) {
                            uint32_t sum = grid[i-1][j] + grid[i+1][j] + grid[i][j-1] + grid[i][j+1];
                            if (sum > 0) grid[i][j] = sum / 4;
                        }
                    }

                    // 4. Draw to Screen
                    if (grid[i][j] > 0) {
                        uint16_t color = convertToColor(grid[i][j]);
                        tft.drawPixel(i + startX - (scanWidth / 2), j + yStart + startY, color);
                    }
                }
            }
        }

        int scanConvert(int scanStartXPos, int scanStartYPos, int gradientXPos, int gradientYPos, int scanRadius) {
            // Calculate scanWidth based on max angle (approx 90 deg spread)
            int scanWidth = scanRadius * 1.5; // Safety margin for the fan
            int numChunks = 8;
            int rowsPerChunk = scanRadius / numChunks;

            // Process the scan in 8 horizontal chunks
            for (int chunk = 0; chunk < numChunks; chunk++) {
                int yStart = chunk * rowsPerChunk;
                int yEnd = (chunk == numChunks - 1) ? scanRadius : yStart + rowsPerChunk;
                
                renderStrip(yStart, yEnd, scanWidth, scanRadius, scanStartXPos, scanStartYPos);
            }

            displayIntensityGradient(gradientXPos, gradientYPos);
            displayMarkers(scanStartXPos, scanStartYPos);
            displayScanStats(scanStartXPos, scanStartYPos);

            return 0;
        }
    };

SectorScan myScan;

void setup(void) {
    tft.init();
    tft.setRotation(1); // Landscape
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
}

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

void loop() {
    // initialize the scan system
    myScan.begin();

    // generate example data and register it in the scan system
    for (int i = 0; i < 200; i++) { // Simulate receiving 100 scans
        myScan.registerEcho(generateExampleData());
    }

    // (scanStartXPos, scanStartYPos, gradientXPos, gradientYPos, scanRadius)
    myScan.scanConvert(160, 30, 5, 24, 300); // Display the scans

    displayButtons();

    while(1);
}