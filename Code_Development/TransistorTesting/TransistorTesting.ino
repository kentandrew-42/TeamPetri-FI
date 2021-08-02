/*
  TransistorTesting

  Used to test the transistor control for turning on and off
  the heating pad.
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

// You can use any (4 or) 5 pins
#define SCLK_PIN 2
#define MOSI_PIN 3
#define DC_PIN   4
#define CS_PIN   5
#define RST_PIN  6

// define buttons
#define selectButton 10
#define Up 11
#define Down 9

// define transistor/power from battery to heating pad
#define transistor 12
#define led 13
#define piezo_alarm 15

// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

// Incubation Defaults
#define DEFAULT_TEMP 30 // degrees Celsius
#define MAX_TEMP_SET 42 // degrees Celsius; max temperature that the user can set during incubation
#define MIN_TEMP_SET 30 // degrees Celsius; minimum temperature that the user can set during incubation
#define DEFAULT_TIME 48 // hours

// Defines the parameters associated with the TFT screen.
Adafruit_SSD1351 tft_screen = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

bool firstTempSet = true;
bool firstTimeSet = true;
float p = 3.1415927;
int timenow = 0;
int ThermistorPin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; // What are these?
int finaltest = 0;
int menuButtonPreviousState = LOW;
int tempUpPreviousState = LOW;
int tempDownPreviousState = LOW;
int out_range_counter = 0;
int menuOpt = 0;
float runningAvg[] = {0, 0, 0, 0, 0};
float T_average;
//int range = map(T, tempMin, tempMax, 0, 3);

int temp_setting = DEFAULT_TEMP; // use button to increment or decrement as necessary
int temp_setting_prev = DEFAULT_TEMP;
int time_setting = DEFAULT_TIME; // default time setting is 48 hours, use button to increment or decrement as necessary
int time_setting_prev = DEFAULT_TIME; // default time setting is 48 hours, use button to increment or decrement as necessary

int DownState = 0;
int UpState = 0;
int selectButtonState = 0;

int onStatus = 0; // this is a placeholder for the transistor testing


void setup(void) {

  //battery
  pinMode(transistor, OUTPUT);
  pinMode(selectButton, INPUT);
  pinMode(Up, INPUT);
  pinMode(Down, INPUT);
  digitalWrite(transistor, LOW);
  
  Serial.begin(9600);
  Serial.println("Starting Up...");
  tft_screen.begin();
  tft_screen.fillScreen(BLACK);

  Serial.println("Initializing...");

  uint16_t time = millis();
  tft_screen.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
  time = millis() - time;
}


void loop() {
  /*
    This loop is designed to test the transistor by turning the heating
    pad on and off, and providing some feedback to the user. We will
    attempt to measure that the transistor is properly working.
  */
}
