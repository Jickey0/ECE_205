int analogPin = A0; // Potentiometer connected to A0
int val = 0; // Variable to store reading
void setup() {
    Serial.begin(9600); // Start serial communication
}
void loop() {
    val = analogRead(analogPin); // Read analog value (0-1023)
    Serial.print("Raw Value: ");
    Serial.print(val);
    // Convert to voltage
    float voltage = (val / 1023.0) * 5.0;
    Serial.print(" | Voltage: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
    delay(500);
}


// Store intervals of ADC data
#define N 100

uint16_t buffer[N];

void capture() {
  for(int i=0;i<N;i++){
    buffer[i] = analogRead(analogPin);
  }
}