// Basic demo for readings from Adafruit BNO08x via SPI on Arduino Due
#include <Adafruit_BNO08x.h>


//   CUSTOMIZABLE PARAMETERS
const float orientationCutoff = 0.5; // Minimum angle change (in radians) to trigger a pulse
#define MAX_SAMPLES 30 // Adjust based on memory needs
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

struct SensorData {
    float roll, pitch, yaw; // BNO08x orientation data
    unsigned long start_time;
    unsigned long stop_time;
    unsigned long values[ADC_SAMPLES];
    float average_time_per_conversion;
};

struct EulerAngles {
    float roll, pitch, yaw;
};

unsigned int i; // index for storing ADC values

// define an array to store multiple readings (e.g. for multiple pulses)
SensorData SensorDataArray[MAX_SAMPLES]; 
unsigned int SensorDataIndex = 0;


void setup(void) {
    // set all values in the first SensorData struct to 0 at the start
    SensorDataArray[SensorDataIndex].roll = 0;
    SensorDataArray[SensorDataIndex].pitch = 0;
    SensorDataArray[SensorDataIndex].yaw = 0;

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
        EulerAngles angles = quaternionsToDegrees(sensorValue.un.gameRotationVector.real, sensorValue.un.gameRotationVector.i, sensorValue.un.gameRotationVector.j, sensorValue.un.gameRotationVector.k);
        //float orientationChange = AngleRelativeChange(sensorValue.un.gameRotationVector.real, sensorValue.un.gameRotationVector.i, sensorValue.un.gameRotationVector.j, sensorValue.un.gameRotationVector.k);
        if (angles.roll - SensorDataArray[SensorDataIndex].roll > angleCutoff) {
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
            SensorDataArray[SensorDataIndex].start_time = micros(); 
            for(i=0;i<ADC_SAMPLES;i++)
            {
                while((ADC->ADC_ISR & 0x80)==0); // wait for conversion
                SensorDataArray[SensorDataIndex].values[i]=ADC->ADC_CDR[7]; //get values
            }
            SensorDataArray[SensorDataIndex].stop_time = micros();
            SensorDataArray[SensorDataIndex].average_time_per_conversion = (float)(SensorDataArray[SensorDataIndex].stop_time-SensorDataArray[SensorDataIndex].start_time)/100;

            // Store the BNO08x data as well
            SensorDataArray[SensorDataIndex].roll = angles.roll;
            SensorDataArray[SensorDataIndex].pitch = angles.pitch;
            SensorDataArray[SensorDataIndex].yaw = angles.yaw;

            // Print timing results for ADC readings --> only for debugging, not for real-time use
            Serial.print("Total time: ");
            Serial.println(SensorDataArray[SensorDataIndex].stop_time-SensorDataArray[SensorDataIndex].start_time); 
            Serial.print(" microseconds");

            Serial.print("number of samples: ");
            Serial.println(ADC_SAMPLES);

            Serial.print("Average time per conversion: ");
            Serial.println(SensorDataArray[SensorDataIndex].average_time_per_conversion);
            Serial.print(" microseconds");

            // Display the orientation data as well
            Serial.print("Orientation - Roll: ");
            Serial.print(angles.roll);
            Serial.print(" Pitch: ");
            Serial.print(angles.pitch);
            Serial.print(" Yaw: ");
            Serial.println(angles.yaw);

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