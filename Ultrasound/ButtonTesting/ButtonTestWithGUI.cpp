#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI(); 

// Pins
const int selectButton = 2;   // Selection button
const int constantHighPin = 30; 
//const int constantHighPin = 31;
const int potPin = A0;        // Potentiometer

// GUI Logic
int highlightedMode = 0;
int lastHighlightedMode = -1;
const int totalModes = 4;

#define ADC_SAMPLES 200
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

        int getCurrentMode(){
            return currentMode;
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

void setup() {
  pinMode(constantHighPin, OUTPUT);
  digitalWrite(constantHighPin, HIGH);
  pinMode(selectButton, INPUT_PULLUP);

  Serial.begin(9600);
  
  tft.init(); // CRITICAL: Initialize the screen hardware
  tft.setRotation(1); // Adjust (0-3) depending on your physical mounting
  tft.fillScreen(TFT_BLACK);
  
  myGUI.begin(); // This calls drawHomeScreen internally
}

void loop() {
  readPotentiometer();
  checkSelectionButton();
}

const int modeThresholds[] = {600, 700, 800, 1000}; 

void readPotentiometer() {
  static int lastStableMode = -1;
  int rawVal = analogRead(potPin);
  int detectedMode = 0;

  // Determine mode based on thresholds instead of a linear map
  for (int i = 0; i < totalModes; i++) {
    if (rawVal < modeThresholds[i]) {
      detectedMode = i;
      break;
    }
  }

  // Only update the screen if the mode actually changed
  if (detectedMode != highlightedMode) {
    highlightedMode = detectedMode;
    myGUI.changeHighlightedMode(highlightedMode);
    
    Serial.print("Pot Value: ");
    Serial.print(rawVal);
    Serial.print(" | Highlighting Mode: ");
    Serial.println(highlightedMode);
  }
}

void checkSelectionButton() {
  // Check Pin 2 (Button to GND)
  if (digitalRead(selectButton) == LOW) {
    executeSelection(highlightedMode);
    
    // Debounce: Wait for release
    while(digitalRead(selectButton) == LOW); 
    delay(50); 
  }
}

void executeSelection(int mode) {
  Serial.print(">>> MODE ");
  Serial.print(mode);
  Serial.println(" SELECTED! <<<");

  // cause flicker
//   myGUI.changeHighlightedMode(0);
//   delay(200);
//   myGUI.changeHighlightedMode(highlightedMode);
  
  // Add your GUI confirmation logic here
  // myGUI.confirmSelection(mode);
}