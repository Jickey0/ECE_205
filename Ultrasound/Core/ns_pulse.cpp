// Corrected Register Macros for Arduino Due
// Pin 5  = PC25
#define INA_HIGH    (REG_PIOC_SODR = (1 << 25))
#define INA_LOW     (REG_PIOC_CODR = (1 << 25))

const int pinENA = 7;   // Output Enable (OE)

// Pin 7  = PA16 ????? 
#define ENA_HIGH    (REG_PIOA_SODR = (1 << 16))
#define ENA_LOW     (REG_PIOA_CODR = (1 << 16))

// Pin 10 = PA28
#define INB_HIGH    (REG_PIOA_SODR = (1 << 28))
#define INB_LOW     (REG_PIOA_CODR = (1 << 28))

void setup() {
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

  Serial.println("Initialization done. Beginning toggle test...");
}

void loop() {

  
  // Both HIGH, pulse active
  INA_HIGH;
  INB_HIGH;

  // Very short wait (NOPs instead of 1 µs)
  __asm__ __volatile__(
    "nop\n\t"
  );

  // outA LOW, outB LOW, current drains
  INA_LOW;
  INB_LOW;

      __asm__ __volatile__(
    "nop\n\t"
  );

  // OUTA = LOW, OUTB = HIGH, no drain and no pulse
  INA_LOW;
  INB_HIGH;

  delayMicroseconds(1000);
}
