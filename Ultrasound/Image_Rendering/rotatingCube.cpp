#include <TFT_eSPI.h> 
TFT_eSPI tft = TFT_eSPI(); 

#define Width 480
#define Height 320

struct Point3D { float x; float y; float z; };
struct Point2D { float x; float y; };

void drawPoint(Point2D p, uint16_t color) {
    int size = 10;
    tft.fillRect(p.x - size/2, p.y - size/2, size, size, color);
}

Point2D convertToScreen(Point2D p) {
    float screenX = (p.x + 1.0) / 2.0 * Width;
    float screenY = (1.0 - (p.y + 1.0) / 2.0) * Height;
    return { screenX, screenY };
}

Point2D project(Point3D p) {
    if (p.z == 0) p.z = 0.01; // Avoid divide by zero
    return { p.x / p.z, p.y / p.z };
}

int FPS = 60; // 90 is very high for SPI screens; 30-60 is more realistic
float dz = 1;  // CHANGED TO FLOAT
float dt = 1.0 / FPS;
int angle = 0;
Point3D offset = {0.0f, 0.0f, 1.0f}; // Start with the cube 3 units away from the camera

// Use & to pass by reference, so the actual array data is modified
Point3D translate_z(Point3D p, float dz_val) {
    p.z += dz_val;
    return p;
}

Point3D rotate_xz(Point3D p, float angle_val) {
    float cosA = cos(angle_val);
    float sinA = sin(angle_val);
    float x_new = p.x * cosA - p.z * sinA;
    float z_new = p.x * sinA + p.z * cosA;
    p.x = x_new;
    p.z = z_new;
    return p;
}

Point3D rotate_yz(Point3D p, float angle_val) {
    float cosA = cos(angle_val);
    float sinA = sin(angle_val);
    
    // X remains unchanged
    float y_new = p.y * cosA - p.z * sinA;
    float z_new = p.y * sinA + p.z * cosA;
    
    p.y = y_new;
    p.z = z_new;
    return p;
}

Point3D rotate_xy(Point3D p, float angle_val) {
    float cosA = cos(angle_val);
    float sinA = sin(angle_val);
    
    float x_new = p.x * cosA - p.y * sinA;
    float y_new = p.x * sinA + p.y * cosA;
    
    p.x = x_new;
    p.y = y_new;
    return p;
}

// Keep the original points constant

Point3D vertices[8] = {
    {0.25, 0.25, 0.25}, {-0.25, 0.25, 0.25}, {0.25, -0.25, 0.25}, {-0.25, -0.25, 0.25},
    {0.25, 0.25, -0.25}, {-0.25, 0.25, -0.25}, {0.25, -0.25, -0.25}, {-0.25, -0.25, -0.25}
};

// Store the previous 2D screen positions to erase them efficiently
Point2D lastPositions[sizeof(vertices) / sizeof(vertices[0])];
Point2D currentPositions[sizeof(vertices) / sizeof(vertices[0])]; 
float currentAngle = 0;

int fs[6][4] = {
    {0, 1, 3, 2},
    {4, 5, 7, 6},
    {0, 1, 5, 4},
    {2, 3, 7, 6},
    {0, 2, 6, 4},
    {1, 3, 7, 5}
};


void drawLine(Point2D p1, Point2D p2, uint16_t color) {
    tft.drawLine(p1.x, p1.y, p2.x, p2.y, color);
}

void frame() {
    float dt = 1.0 / FPS;
    dz += 1.0 * dt; 
    currentAngle += 1.0 * dt;

    // We need a temporary array to store the projected points for THIS frame
    Point2D currentProjected[sizeof(vertices) / sizeof(vertices[0])];

    // Phase 1: Calculate and Erase
    for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        // Erase old points
        drawPoint(lastPositions[i], TFT_DARKGREY);

        // Transform and project
        Point3D p = vertices[i];
        p = rotate_xz(p, currentAngle); // Rotate at different speeds for a more dynamic effect
        p = translate_z(p, dz);
        currentProjected[i] = convertToScreen(project(p));
    }

    // Phase 2: Erase old lines and draw new lines
    for (int i = 0; i < sizeof(fs) / sizeof(fs[0]); i++) {
        for (int j = 0; j < sizeof(fs[0]) / sizeof(fs[0][0]); j++) {
            int v1 = fs[i][j];
            int v2 = fs[i][(j + 1) % 4];

            // Erase the old line using the positions saved in the LAST frame
            drawLine(lastPositions[v1], lastPositions[v2], TFT_DARKGREY);
            
            // Draw the new line using the positions we JUST calculated
            drawLine(currentProjected[v1], currentProjected[v2], TFT_ORANGE);
        }
    }

    // Phase 3: Update lastPositions for the next frame
    for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        lastPositions[i] = currentProjected[i];
    }

    delay(1000 / FPS); 
}

Point3D addOffset(Point3D p, Point3D offset) {
    p.x += offset.x;
    p.y += offset.y;
    p.z += offset.z;
    return p;
}

void setup(void) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_DARKGREY);

  for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        vertices[i] = addOffset(vertices[i], offset);
  }
}

void loop() {
    frame();
}