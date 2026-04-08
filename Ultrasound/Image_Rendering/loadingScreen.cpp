// An adaption of the "UncannyEyes" sketch (see eye_functions tab)
// for the TFT_eSPI library. As written the sketch is for driving
// one (240x320 minimum) TFT display, showing 2 eyes. See example
// Animated_Eyes_2 for a dual 128x128 TFT display configured sketch.

// The size of the displayed eye is determined by the screen size and
// resolution. The eye image is 128 pixels wide. In humans the palpebral
// fissure (open eye) width is about 30mm so a low resolution, large
// pixel size display works best to show a scale eye image. Note that
// display manufacturers usually quote the diagonal measurement, so a
// 128 x 128 1.7" display or 128 x 160 2" display is about right.

// Configuration settings for the eye, eye style, display count,
// chip selects and x offsets can be defined in the sketch "config.h" tab.

// Performance (frames per second = fps) can be improved by using
// DMA (for SPI displays only) on ESP32 and STM32 processors. Use
// as high a SPI clock rate as is supported by the display. 27MHz
// minimum, some displays can be operated at higher clock rates in
// the range 40-80MHz.

// Single defaultEye performance for different processors
//                                  No DMA   With DMA
// ESP8266 (160MHz CPU) 40MHz SPI   36 fps
// ESP32 27MHz SPI                  53 fps     85 fps
// ESP32 40MHz SPI                  67 fps    102 fps
// ESP32 80MHz SPI                  82 fps    116 fps // Note: Few displays work reliably at 80MHz
// STM32F401 55MHz SPI              44 fps     90 fps
// STM32F446 55MHz SPI              83 fps    155 fps
// STM32F767 55MHz SPI             136 fps    197 fps

// DMA can be used with RP2040, STM32 and ESP32 processors when the interface
// is SPI, uncomment the next line:
//#define USE_DMA

// Load TFT driver library
#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft;           // A single instance is used for 1 or 2 displays

// A pixel buffer is used during eye rendering
#define BUFFER_SIZE 1024 // 128 to 1024 seems optimum

#ifdef USE_DMA
  #define BUFFERS 2      // 2 toggle buffers with DMA
#else
  #define BUFFERS 1      // 1 buffer for no DMA
#endif

uint16_t pbuffer[BUFFERS][BUFFER_SIZE]; // Pixel rendering buffer
bool     dmaBuf   = 0;                  // DMA buffer selection

// This struct is populated in config.h
typedef struct {        // Struct is defined before including config.h --
  int8_t  select;       // pin numbers for each eye's screen select line
  int8_t  wink;         // and wink button (or -1 if none) specified there,
  uint8_t rotation;     // also display rotation and the x offset
  int16_t xposition;    // position of eye on the screen
} eyeInfo_t;

#include "config.h"     // ****** CONFIGURATION IS DONE IN HERE ******

extern void user_setup(void); // Functions in the user*.cpp files
extern void user_loop(void);

#define SCREEN_X_START 0
#define SCREEN_X_END   SCREEN_WIDTH   // Badly named, actually the "eye" width!
#define SCREEN_Y_START 0
#define SCREEN_Y_END   SCREEN_HEIGHT  // Actually "eye" height

// A simple state machine is used to control eye blinks/winks:
#define NOBLINK 0       // Not currently engaged in a blink
#define ENBLINK 1       // Eyelid is currently closing
#define DEBLINK 2       // Eyelid is currently opening
typedef struct {
  uint8_t  state;       // NOBLINK/ENBLINK/DEBLINK
  uint32_t duration;    // Duration of blink state (micros)
  uint32_t startTime;   // Time (micros) of last state change
} eyeBlink;

struct {                // One-per-eye structure
  int16_t   tft_cs;     // Chip select pin for each display
  eyeBlink  blink;      // Current blink/wink state
  int16_t   xposition;  // x position of eye image
} eye[NUM_EYES];

uint32_t startTime;  // For FPS indicator

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

// INITIALIZATION -- runs once at startup ----------------------------------
void setup(void) {
  Serial.begin(115200);
  //while (!Serial);
  Serial.println("Starting");

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
  // Enable backlight pin, initially off
  Serial.println("Backlight turned off");
  pinMode(DISPLAY_BACKLIGHT, OUTPUT);
  digitalWrite(DISPLAY_BACKLIGHT, LOW);
#endif

  // User call for additional features
  user_setup();

  // Initialise the eye(s), this will set all chip selects low for the tft.init()
  initEyes();

  // Initialise TFT
  Serial.println("Initialising displays");
  tft.init();

#ifdef USE_DMA
  tft.initDMA();
#endif

  // Raise chip select(s) so that displays can be individually configured
  digitalWrite(eye[0].tft_cs, HIGH);
  if (NUM_EYES > 1) digitalWrite(eye[1].tft_cs, HIGH);

  for (uint8_t e = 0; e < NUM_EYES; e++) {
    digitalWrite(eye[e].tft_cs, LOW);
    tft.setRotation(eyeInfo[e].rotation);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(eye[e].tft_cs, HIGH);
  }

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
  Serial.println("Backlight now on!");
  analogWrite(DISPLAY_BACKLIGHT, BACKLIGHT_MAX);
#endif

  startTime = millis(); // For frame-rate calculation

    // Display cool startup screen text in the bottom left corner
    tft.setCursor(5, 300);  
    tft.setTextColor(TFT_WHITE);
    tft.setTextFont(2);
    tft.print("Ultrasound OS Loading...");

    // tft.setCursor(240, 180);
    // tft.print("By Jack Hickey and Ty Stauffer");

    updateEye(); // one full eye animation cycle, a little long might reduce later
    
    tft.fillScreen(TFT_BLACK);
    myGUI.begin();
}


// MAIN LOOP -- runs continuously after setup() ----------------------------
void loop() {
    delay(3000); // wait for 2 seconds before switching to sector scan mode
    myGUI.changeHighlightedMode(0);
    delay(3000); // wait for 2 seconds before switching to sector scan mode
    myGUI.changeHighlightedMode(1); // change to sector scan mode
    delay(3000); // wait for 2 seconds before switching to volume scan mode
    myGUI.changeHighlightedMode(2); // change to volume scan mode
    delay(3000); // wait for 2 seconds before switching to settings mode
    myGUI.changeHighlightedMode(3); // change to settings mode
}
