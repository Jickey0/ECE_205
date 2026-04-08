#include <Adafruit_GFX.h>
#include <Adafruit_ILI9488.h>
#include <SPI.h>

// LCD Pins
#define TFT_CS   11
#define TFT_DC    3
#define TFT_RST   4

// BNO085 Pins (Previously defined)
#define BNO_CS    6
#define BNO_INT   9
#define BNO_RST   8

Adafruit_ILI9488 tft = Adafruit_ILI9488(TFT_CS, TFT_DC, TFT_RST);

int currentColumn = 0;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);

  // Initialize Screen
  tft.begin();
  tft.setRotation(1); // Landscape (480x320)
  tft.fillScreen(ILI9488_BLACK);

  tft.setCursor(10, 10);
  tft.setTextColor(ILI9488_WHITE);
  tft.println("BNO085 Heatmap Display");
}

void drawNextColumn(unsigned long values[]) {
  // We simulate 300 data points (rows) for this column
  for (int i = 0; i < 300; i++) {
    // 1. Simulate or get your sensor value (0-4095)
    int rawValue = values[i]; // Use the provided values array

    // 2. Map to 8-bit grayscale (0-255)
    uint8_t gray = map(rawValue, 0, 4095, 0, 255);
    
    // 3. Convert to 16-bit color for the LCD
    uint16_t color = tft.color565(gray, gray, gray);
    
    // 4. Draw the 24x1 rectangle
    // fillRect(x, y, width, height)
    tft.fillRect(currentColumn * 24, 15 + i, 24, 1, color);
  }

  // Move to next column, wrap around if we hit the edge (480 pixels / 24 = 20 columns)
  currentColumn++;
  if (currentColumn >= 20) {
    currentColumn = 0;
    // Optional: clear screen or shift? 
    // tft.fillScreen(ILI9488_BLACK); 
  }
}

void drawUltrasoundSector(float bnoAngle) {
  // Convert BNO085 angle to radians for math functions
  float radians = bnoAngle * (PI / 180.0);
  
  int originX = 240; // Center of the screen
  int originY = 20;  // Top of the screen

  for (int i = 0; i < 300; i++) {
    // Calculate the X and Y coordinate for this specific data point
    int x = originX + (int)(i * sin(radians));
    int y = originY + (int)(i * cos(radians));

    // Get intensity from your array
    uint16_t rawValue = SensorDataArray[SensorDataIndex].values[i];
    uint8_t gray = map(rawValue, 0, 4095, 0, 255);
    uint16_t color = tft.color565(gray, gray, gray);

    // Draw a single pixel (or a small 2x2 block for thickness)
    if (x >= 0 && x < 480 && y >= 0 && y < 320) {
      tft.drawPixel(x, y, color);
    }
  }
}

void loop() {
    drawNextColumn(SensorDataArray[SensorDataIndex].values);
}