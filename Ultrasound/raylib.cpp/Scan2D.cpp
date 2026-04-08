#include "raylib.h"
#include <math.h>
#include <string.h>
#include <stdint.h>

// --- Configuration ---
const int screenWidth = 480;
const int screenHeight = 320;

const int originX = 240;      
const int originY = 280;      
const int maxDepth = 200;     

// --- Look-Up Tables ---
float lutSin[181]; 
float lutCos[181]; 

// Data storage for interpolation
uint8_t previousRayData[maxDepth];
int previousAngle = -1;

const int startAngle = 45;
const int finalAngle = 135;
const int AngleWidth = 91; 

uint8_t dummyData[AngleWidth][maxDepth];

// --- Functional Functions ---

void GenerateFakeData() {
    for (int a = 0; a < AngleWidth; a++) {
        for (int r = 0; r < maxDepth; r++) {
            uint8_t val = (uint8_t)((sin(a * 0.1) * cos(r * 0.05) + 1) * 127);
            dummyData[a][r] = val;
        }
    }
}

void DrawUIFrame() {
    // Raylib's DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color)
    // Note: Raylib angles start from 0 at the right, so we adjust to match your logic
    DrawRingLines((Vector2){(float)originX, (float)originY}, (float)maxDepth, (float)maxDepth + 2, -135, -45, 40, LIGHTGRAY);
    
    // Boundary lines
    DrawLine(originX, originY, originX + (maxDepth * lutSin[45]), originY - (maxDepth * lutCos[45]), LIGHTGRAY);
    DrawLine(originX, originY, originX + (maxDepth * lutSin[135]), originY - (maxDepth * lutCos[135]), LIGHTGRAY);
}

void FillMissingAngles(int startA, int endA, uint8_t* startData, uint8_t* endData) {
    int gapSize = endA - startA;
    for (int a = startA + 1; a < endA; a++) {
        float weight = (float)(a - startA) / gapSize;
        for (int r = 0; r < maxDepth; r++) {
            uint8_t interpVal = (uint8_t)((1.0f - weight) * startData[r] + (weight * endData[r]));
            int x = originX + (r * lutSin[a]);
            int y = originY - (r * lutCos[a]);
            DrawPixel(x, y, (Color){interpVal, interpVal, interpVal, 255});
        }
    }
}

void DrawUltrasoundRay(int angle, uint8_t* currentData) {
    if (previousAngle != -1 && (angle - previousAngle) > 1) {
        FillMissingAngles(previousAngle, angle, previousRayData, currentData);
    }

    for (int r = 0; r < maxDepth; r++) {
        int x = originX + (r * lutSin[angle]);
        int y = originY - (r * lutCos[angle]);
        // Raylib uses a Color struct: {R, G, B, A}
        DrawPixel(x, y, (Color){currentData[r], currentData[r], currentData[r], 255});
    }

    memcpy(previousRayData, currentData, maxDepth);
    previousAngle = angle;
}

int main() {
    // 1. Initialization
    InitWindow(screenWidth, screenHeight, "Ultrasound GUI Simulator");
    SetTargetFPS(60); 

    for (int i = 0; i <= 180; i++) {
        lutSin[i] = sinf(i * DEG2RAD);
        lutCos[i] = cosf(i * DEG2RAD);
    }

    GenerateFakeData();

    // 2. Main Game Loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawUIFrame();

        // Draw the image
        previousAngle = -1; // Reset for each frame redraw
        for (int i = 0; i < AngleWidth; i++) {
            // Simulated hardware skip for testing interpolation
            if (i > 30 && i < 35) continue; 
            DrawUltrasoundRay(startAngle + i, dummyData[i]);
        }

        DrawText("MODE: B-SCAN", 10, 10, 20, CYAN);
        DrawFPS(screenWidth - 80, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}