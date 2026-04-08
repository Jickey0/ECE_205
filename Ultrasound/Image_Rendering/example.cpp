#include <TFT_eSPI.h>
#include <math.h>

TFT_eSPI tft = TFT_eSPI();

// --- Configuration ---
const int originX = 240;      // Horizontal center
const int originY = 28;      // Moved lower so the fan points UP into the screen
const int maxDepth = 200;     
const int sectorAngle = 90;   

// --- Look-Up Tables ---
float lutSin[181]; 
float lutCos[181]; 

// Data storage for interpolation (Changed to uint8_t for memory/logic)
uint8_t previousRayData[maxDepth];
int previousAngle = -1;

// Simulation settings
static int startAngle = 45;
static int finalAngle = 135;
const int AngleWidth = finalAngle - startAngle + 1; // 135 - 45 + 1

// Use uint8_t (0-255) to save 75% RAM vs float
uint8_t dummyData[AngleWidth][maxDepth];

void setup() {
    tft.init();
    tft.setRotation(1); // Landscape
    tft.fillScreen(TFT_BLACK);
    
    // 1. Precompute Sine and Cosine
    for (int i = 0; i <= 180; i++) {
        float rad = i * DEG_TO_RAD;
        lutSin[i] = sin(rad);
        lutCos[i] = cos(rad);
    }

    drawUIFrame();
    
    // Fill the dummyData buffer
    generateFakeData(); 
}

void drawUIFrame() {
    tft.drawArc(originX, originY, maxDepth + 2, maxDepth, startAngle, finalAngle, TFT_LIGHTGREY, TFT_BLACK);
    // Draw boundary lines
    tft.drawLine(originX, originY, originX + (maxDepth * lutSin[startAngle]), originY - (maxDepth * lutCos[startAngle]), TFT_LIGHTGREY);
    tft.drawLine(originX, originY, originX + (maxDepth * lutSin[finalAngle]), originY - (maxDepth * lutCos[finalAngle]), TFT_LIGHTGREY);
}

// 2. The Draw Function
void drawUltrasoundRay(int angle, uint8_t* currentData) {
    
    // Check for gaps (interpolation trigger)
    if (previousAngle != -1 && (angle - previousAngle) > 1) {
        fillMissingAngles(previousAngle, angle, previousRayData, currentData);
    }

    // Draw the actual ray
    for (int r = 0; r < maxDepth; r++) {
        // We subtract the Y offset to make the fan point UP
        int x = originX + (r * lutSin[angle]);
        int y = originY - (r * lutCos[angle]);
        
        uint16_t color = tft.color565(currentData[r], currentData[r], currentData[r]);
        tft.drawPixel(x, y, color);
    }

    // Store for next call
    memcpy(previousRayData, currentData, maxDepth);
    previousAngle = angle;
}

// 3. Interpolation Logic
void fillMissingAngles(int startA, int endA, uint8_t* startData, uint8_t* endData) {
    int gapSize = endA - startA;
    
    for (int a = startA + 1; a < endA; a++) {
        float weight = (float)(a - startA) / gapSize;
        
        for (int r = 0; r < maxDepth; r++) {
            uint8_t interpVal = (uint8_t)((1.0 - weight) * startData[r] + (weight * endData[r]));
            
            int x = originX + (r * lutSin[a]);
            int y = originY - (r * lutCos[a]);
            
            uint16_t color = tft.color565(interpVal, interpVal, interpVal);
            tft.drawPixel(x, y, color);
        }
    }
}

// Fixed function signature to work with the global array
void generateFakeData() {
    for (int a = 0; a < AngleWidth; a++) {
        for (int r = 0; r < maxDepth; r++) {
            // Generates a clinical-looking "cloud" pattern
            uint8_t val = (uint8_t)((sin((a * 0.1)) * cos((r * 0.05)) + 1) * 127);
            dummyData[a][r] = val;
        }
    }
}

void drawUltrasoundImage(){
    for (int i = 0; i < AngleWidth; i++) {
        // To simulate your "missing angles", we skip index 30 to 35
        if (i > 30 && i < 35) continue; 

        drawUltrasoundRay(startAngle + i, dummyData[i]);
    }
}

void loop() {
    drawUltrasoundImage(); 
    // Wait here to prevent flickering for the demo
    while(1); 
}