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

void setup(void) {
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
}

// Define the sensor outputs you want to receive
void setReports(void) {
    Serial.println("Setting desired reports");
    // Enable Game Rotation Vector (No Magnetometer interference)
    if (! bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
        Serial.println("Could not enable game vector");
    }
}

void loop() {
    delay(10);

    if (bno08x.wasReset()) {
        Serial.print("Sensor was reset ");
        setReports();
    }

    if (! bno08x.getSensorEvent(&sensorValue)) {
        return;
    }

    switch (sensorValue.sensorId) {
        case SH2_GAME_ROTATION_VECTOR:
            Serial.print("Game Rotation Vector - r: ");
            Serial.print(sensorValue.un.gameRotationVector.real);
            Serial.print(" i: ");
            Serial.print(sensorValue.un.gameRotationVector.i);
            Serial.print(" j: ");
            Serial.print(sensorValue.un.gameRotationVector.j);
            Serial.print(" k: ");
            Serial.println(sensorValue.un.gameRotationVector.k);
            break;
    }
}