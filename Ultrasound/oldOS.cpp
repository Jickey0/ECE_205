#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft;           // A single instance is used for 1 or 2 displays

// A pixel buffer is used during eye rendering
#define BUFFER_SIZE 1024 // 128 to 1024 seems optimum

#ifdef USE_DMA
  #define BUFFERS 2      // 2 toggle buffers with DMA
#else
  #define BUFFERS 1      // 1 buffer for no DMA
#endif

uint16_t pbuffer[BUFFERS][BUFFER_SIZE]; // Pixel rendering buffer
bool     dmaBuf   = 0;                  // DMA buffer selection

// This struct is populated in config.h
typedef struct {        // Struct is defined before including config.h --
  int8_t  select;       // pin numbers for each eye's screen select line
  int8_t  wink;         // and wink button (or -1 if none) specified there,
  uint8_t rotation;     // also display rotation and the x offset
  int16_t xposition;    // position of eye on the screen
} eyeInfo_t;

#include "config.h"     // ****** CONFIGURATION IS DONE IN HERE ******

extern void user_setup(void); // Functions in the user*.cpp files
extern void user_loop(void);

#define SCREEN_X_START 0
#define SCREEN_X_END   SCREEN_WIDTH   // Badly named, actually the "eye" width!
#define SCREEN_Y_START 0
#define SCREEN_Y_END   SCREEN_HEIGHT  // Actually "eye" height

// A simple state machine is used to control eye blinks/winks:
#define NOBLINK 0       // Not currently engaged in a blink
#define ENBLINK 1       // Eyelid is currently closing
#define DEBLINK 2       // Eyelid is currently opening
typedef struct {
  uint8_t  state;       // NOBLINK/ENBLINK/DEBLINK
  uint32_t duration;    // Duration of blink state (micros)
  uint32_t startTime;   // Time (micros) of last state change
} eyeBlink;

struct {                // One-per-eye structure
  int16_t   tft_cs;     // Chip select pin for each display
  eyeBlink  blink;      // Current blink/wink state
  int16_t   xposition;  // x position of eye image
} eye[NUM_EYES];

uint32_t startTime;  // For FPS indicator

#define ADC_SAMPLES 100
const int screenWidth = 480;
const int screenHeight = 320;

// GUI class for moving between the following modes: 1D scan, 2D sector scan, 3D volume scan, and device setting. Also displays system info (e.g. battery life, user name, etc.)
// We use the drawRoundRect function to draw buttons for each mode, and the user can click on the buttons to navigate between modes. 
// To show which mode is currently selected, we change the color from grey to white. 
class HomeGUI {
    private:
        int currentMode; // 0 = 1D scan, 1 = 2D sector scan, 2 = 3D volume scan, 3 = device settings

    public:
        void begin(){
            currentMode = 0; // start with 1D scan mode selected
            drawHomeScreen();
        }

        void drawHomeScreen(){
            tft.fillScreen(TFT_BLACK);
            drawButtons();
            changeHighlightedMode(currentMode);
            // displaySystemInfo(); 
        }

        void drawButtons(){
            // draw buttons for each mode at the top of the screen
            tft.drawRoundRect(10, 10, 100, 40, 5, TFT_LIGHTGREY);
            tft.setCursor(20, 30);
            tft.setTextColor(TFT_LIGHTGREY);
            tft.setTextFont(2);
            tft.print("1D Scan");

            tft.drawRoundRect(120, 10, 100, 40, 5, TFT_LIGHTGREY);
            tft.setCursor(130, 30);
            tft.setTextColor(TFT_LIGHTGREY);
            tft.setTextFont(2);
            tft.print("Sector Scan");

            tft.drawRoundRect(230, 10, 100, 40, 5, TFT_LIGHTGREY);
            tft.setCursor(240, 30);
            tft.setTextColor(TFT_LIGHTGREY);
            tft.setTextFont(2);
            tft.print("Volume Scan");

            tft.drawRoundRect(340, 10, 100, 40, 5, TFT_LIGHTGREY);
            tft.setCursor(350, 30);
            tft.setTextColor(TFT_LIGHTGREY);
            tft.setTextFont(2);
            tft.print("Settings");
        }

        void changeHighlightedMode(int newMode){
            // change the color of the button corresponding to newMode to white, and change the color of the button corresponding to currentMode to grey
            switch (currentMode) {
                case 0:
                    tft.drawRoundRect(10, 10, 100, 40, 5, TFT_LIGHTGREY);
                    tft.setCursor(20, 30);
                    tft.setTextColor(TFT_LIGHTGREY);
                    tft.setTextFont(2);
                    tft.print("1D Scan");
                    break;
                case 1:
                    tft.drawRoundRect(120, 10, 100, 40, 5, TFT_LIGHTGREY);
                    tft.setCursor(130, 30);
                    tft.setTextColor(TFT_LIGHTGREY);
                    tft.setTextFont(2);
                    tft.print("Sector Scan");
                    break;
                case 2:
                    tft.drawRoundRect(230, 10, 100, 40, 5, TFT_LIGHTGREY);
                    tft.setCursor(240, 30);
                    tft.setTextColor(TFT_LIGHTGREY);
                    tft.setTextFont(2);
                    tft.print("Volume Scan");
                    break;
                case 3:
                    tft.drawRoundRect(340, 10, 100, 40, 5, TFT_LIGHTGREY);
                    tft.setCursor(350, 30);
                    tft.setTextColor(TFT_LIGHTGREY);
                    tft.setTextFont(2);
                    tft.print("Settings");
                    break;
            }

            switch (newMode) {
                case 0:
                    tft.drawRoundRect(10, 10, 100, 40, 5, TFT_WHITE);
                    tft.setCursor(20, 30);
                    tft.setTextColor(TFT_WHITE);
                    tft.setTextFont(2);
                    tft.print("1D Scan");
                    break;
                case 1:
                    tft.drawRoundRect(120, 10, 100, 40, 5, TFT_WHITE);
                    tft.setCursor(130, 30);
                    tft.setTextColor(TFT_WHITE);
                    tft.setTextFont(2);
                    tft.print("Sector Scan");
                    break;
                case 2:
                    tft.drawRoundRect(230, 10, 100, 40, 5, TFT_WHITE);
                    tft.setCursor(240, 30);
                    tft.setTextColor(TFT_WHITE);
                    tft.setTextFont(2);
                    tft.print("Volume Scan");
                    break;
                case 3:
                    tft.drawRoundRect(340, 10, 100, 40, 5, TFT_WHITE);
                    tft.setCursor(350, 30);
                    tft.setTextColor(TFT_WHITE);
                    tft.setTextFont(2);
                    tft.print("Settings");
                    break;
            }

            currentMode = newMode;
        }
};

HomeGUI myGUI;

struct Scan1D {
    float roll, pitch, yaw; // BNO08x orientation data
    unsigned long total_time;
    uint16_t values[ADC_SAMPLES];
    float average_time_per_conversion;
};

class SectorScan {
    private:
        // Use 'static constexpr' to allow these to size the arrays below
        static constexpr int MaxScanSize = 100; 
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


class SingleScan {
    private:
        static constexpr int MaxScanSize = 100; 
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


// INITIALIZATION -- runs once at startup ----------------------------------
void setup(void) {
  Serial.begin(115200);
  //while (!Serial);
  Serial.println("Starting");

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
  // Enable backlight pin, initially off
  Serial.println("Backlight turned off");
  pinMode(DISPLAY_BACKLIGHT, OUTPUT);
  digitalWrite(DISPLAY_BACKLIGHT, LOW);
#endif

  // User call for additional features
  user_setup();

  // Initialise the eye(s), this will set all chip selects low for the tft.init()
  initEyes();

  // Initialise TFT
  Serial.println("Initialising displays");
  tft.init();

#ifdef USE_DMA
  tft.initDMA();
#endif

  // Raise chip select(s) so that displays can be individually configured
  digitalWrite(eye[0].tft_cs, HIGH);
  if (NUM_EYES > 1) digitalWrite(eye[1].tft_cs, HIGH);

  for (uint8_t e = 0; e < NUM_EYES; e++) {
    digitalWrite(eye[e].tft_cs, LOW);
    tft.setRotation(eyeInfo[e].rotation);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(eye[e].tft_cs, HIGH);
  }

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
  Serial.println("Backlight now on!");
  analogWrite(DISPLAY_BACKLIGHT, BACKLIGHT_MAX);
#endif

  startTime = millis(); // For frame-rate calculation

    // Display cool startup screen text in the bottom left corner
    tft.setCursor(5, 300);  
    tft.setTextColor(TFT_WHITE);
    tft.setTextFont(2);
    tft.print("Ultrasound OS Loading...");

    // tft.setCursor(240, 180);
    // tft.print("By Jack Hickey and Ty Stauffer");

    updateEye(); // one full eye animation cycle, a little long might reduce later
    
    tft.fillScreen(TFT_BLACK);
    myGUI.begin();
}

// MAIN LOOP -- runs continuously after setup() ----------------------------
void loop() {
    for (int i = 0; i <= 4; i++){
        delay(2000); // wait for 2 seconds before switching to sector scan mode
        myGUI.changeHighlightedMode(i);
    }

    tft.fillScreen(TFT_BLACK);

    // initialize the scan system
    myScan.begin();

    // generate example data and register it in the scan system
    for (int i = 0; i < 200; i++) { // Simulate receiving 100 scans
        myScan.registerEcho(generateExampleData());
    }

    // (scanStartXPos, scanStartYPos, gradientXPos, gradientYPos, scanRadius)
    myScan.scanConvert(160, 30, 5, 24, 300); // Display the scans

    displayButtons();

    delay(10000);
    tft.fillScreen(TFT_BLACK);
    myGUI.changeHighlightedMode(3);
    myGUI.changeHighlightedMode(2);
    myGUI.changeHighlightedMode(1);
    myGUI.changeHighlightedMode(0);
    delay(6000);

    tft.fillScreen(TFT_BLACK);

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