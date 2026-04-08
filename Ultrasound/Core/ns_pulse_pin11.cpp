// --- PIN DEFINITIONS & REGISTER MACROS ---

// Pin 5 = PC25
#define INA_HIGH    (REG_PIOC_SODR = (1 << 25))
#define INA_LOW     (REG_PIOC_CODR = (1 << 25))

// Pin 7 = PA16
#define ENA_HIGH    (REG_PIOA_SODR = (1 << 16))
#define ENA_LOW     (REG_PIOA_CODR = (1 << 16))

// Pin 11 = PD7 (Changed from Pin 10 / PA28)
#define INB_HIGH    (REG_PIOD_SODR = (1 << 7))
#define INB_LOW     (REG_PIOD_CODR = (1 << 7))

const int pinENA = 7; 

void setup() {
  Serial.begin(115200);

  // 1. Enable Peripheral Clocks for Ports A, C, and D
  pmc_enable_periph_clk(ID_PIOA);
  pmc_enable_periph_clk(ID_PIOC);
  pmc_enable_periph_clk(ID_PIOD); // Needed for Pin 11

  // 2. Give PIO Controller control of the pins
  REG_PIOA_PER = (1 << 16); // Pin 7
  REG_PIOC_PER = (1 << 25); // Pin 5
  REG_PIOD_PER = (1 << 7);  // Pin 11

  // 3. PRE-SET THE STATE TO HIGH 
  // We do this BEFORE enabling output to prevent the startup "dip"
  REG_PIOA_SODR = (1 << 16); 
  REG_PIOC_SODR = (1 << 25);
  REG_PIOD_SODR = (1 << 7); 

  // 4. Now enable the output drivers
  // Because SODR was already set, the pins will "wake up" HIGH
  REG_PIOA_OER = (1 << 16);
  REG_PIOC_OER = (1 << 25);
  REG_PIOD_OER = (1 << 7);

  Serial.println("Initialization done. Pin 11 (PD7) active. Beginning toggle test...");
}

void loop() {
  // Both HIGH, pulse active
  INA_HIGH;
  INB_HIGH;

  __asm__ __volatile__("nop\n\t");

  // outA LOW, outB LOW, current drains
  INA_LOW;
  INB_LOW;

  __asm__ __volatile__("nop\n\t");

  // OUTA = LOW, OUTB = HIGH, no drain and no pulse
  INA_LOW;
  INB_HIGH;

  delayMicroseconds(1000);
}