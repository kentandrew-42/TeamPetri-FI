/*
 * Team Petri-FI | SEED Internship & Rice360 Institute
 *
 * Contributors:
 * Kenton Roberts
 * Nancy Lindsey
 * Teja Paturu
 * Sara Barker
 * 
 * Code Adapted From Previous Student Teams:
 * Moonrat
 * Minicubator
 * and Contributions from the Public Invention Moonrat Team.
 *
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins
#define SCLK_PIN 2
#define MOSI_PIN 3
#define DC_PIN   4
#define CS_PIN   5
#define RST_PIN  6

//define buttons
#define Up 11
#define Down 9

//define transistor/power from battery to heating pad
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


Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

bool firstTempSet = true;
bool firstTimeSet = true;
float p = 3.1415927;
int timenow = 0;
int ThermistorPin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
int finaltest = 0;
int menuButtonPreviousState = LOW;
int tempUpPreviousState = LOW;
int tempDownPreviousState = LOW;
int out_range_counter = 0;
int menuOpt = 0;
float runningAvg[] = {0, 0, 0, 0, 0}; // keeps a running average, maybe for displaying the temp values? -kenton
float T_average;
//int range = map(T, tempMin, tempMax, 0, 3);

int T_setting = 30; // default temperature setting is 30C, use button to increment or decrement as necessary
int T_setting_prev = 30; // default temperature setting is 30C, use button to increment or decrement as necessary
int time_setting = 48; // default time setting is 48 hours, use button to increment or decrement as necessary
int time_setting_prev = 48; // default time setting is 48 hours, use button to increment or decrement as necessary

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
  Serial.println("hello!"); //FIXME improve intro text -kenton
  tft.begin();
  tft.fillScreen(BLACK);

  Serial.println("init"); //FIXME improve this, too -kenton

  uint16_t time = millis();
  tft.fillRect(0, 0, 128, 128, BLACK);
  time = millis() - time;
}

void printBanner(int T_setting, int T_setting_prev, int time_setting, int time_setting_prev) {
//prints the yellow text at the top that shows the selected setting while powered on.
  tft.setCursor(0,10);
  tft.setTextColor(YELLOW);
  tft.print("Curr Temp Set (C): "); // Thank you for using Celsius -kenton
  tft.print(T_setting);
  
  tft.setCursor(0,20);
  tft.setTextColor(YELLOW);
  tft.print("Curr Time Set (h): "); // Maybe add more clarity to units -kenton
  tft.print(time_setting);
  
}

void setTemp(int m) { // menu option to set temperature
  // where the heck is "int m" used in this method? -kenton
  DownState = digitalRead(Down);
  UpState = digitalRead(Up);

  int T_setting_prev = T_setting;
  
  if ((DownState == HIGH) && (T_setting <= 42) && (T_setting > 30)) { 
    T_setting--; // decrement the temperature setting by 1, given within the acceptable range

  }
  if ((UpState == HIGH) && (T_setting < 42) && (T_setting >= 30)) {
    T_setting++; // increment the temperature setting by 1, given within the acceptable range
    
  }  

  if ((T_setting != T_setting_prev) || (firstTempSet == true)) {
    tft.setCursor(0,10); // this code blots out the yellow print at the top while the temperature selection is occurring
    tft.setTextColor(YELLOW);
    tft.print("Curr Temp Set (C): ");
    tft.setTextColor(BLACK);
    tft.print(T_setting_prev);
    tft.setCursor(0,50); // display the selection in progress to the user.
    tft.setTextColor(WHITE);
    tft.println("Set temperature (C): ");
    tft.setCursor(0,60);
    tft.setTextColor(BLACK);
    tft.print(T_setting_prev);
    tft.setCursor(0,60);
    tft.setTextColor(MAGENTA);
    tft.print(T_setting);
  }

  //delay(250);
  firstTempSet = false;
}

void setTime(int m) { // menu option to set time
  
  DownState = digitalRead(Down);
  UpState = digitalRead(Up);

  int time_setting_prev = time_setting;
  
  if (DownState == HIGH) { 
    time_setting--; // decrement the time setting by 1 hour
  }
  if (UpState == HIGH) {
    time_setting++; // increment the time setting by 1 hour
  }  

  if ((time_setting != time_setting_prev) || (firstTimeSet == true)) {
    tft.setCursor(0,20); // this code blots out the yellow print at the top while the time selection is occurring
    tft.setTextColor(YELLOW);
    tft.print("Curr Time Set (h): ");
    tft.setTextColor(BLACK);
    tft.print(time_setting_prev);
    tft.setCursor(0,50); // display the selection in progress to the user.
    tft.setTextColor(WHITE);
    tft.println("Set time (hrs): ");
    tft.setCursor(0,60);
    tft.setTextColor(BLACK);
    tft.print(time_setting_prev);
    tft.setCursor(0,60);
    tft.setTextColor(MAGENTA);
    tft.print(time_setting);
  }

  firstTimeSet = false;
}


float readTemp() {
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  T = T - 273.15;
  return T;
}

void startUp() {
  T = readTemp();
  // while loop exits when temp gets up to adequate temperature, it appears -kenton
  while (T < T_setting-0.5) {
    tftPrintStartUp();
	 // TODO I don't think we want to hard-code the duty cycle, we instead want a response algorithm -kenton
	 // Oh, maybe this is JUST for getting the temperature up to the desired temp -kenton
    dutyCycle(20000, 0.3); //30% duty cycle found not to exceed 65oC surrounded by air at room temp, 20% duty cycle does not exceed 53oC
    tftEraseStartUp();
    T = readTemp();
  }
  tftEraseStartUp();
  startRun();
}

// Like I've said, I don't think we want to use a duty cycle like this. For real, I think this just turns
// on the transistor for a percentage of time, but that isn't useful if we start
// going out of temperature bounds. Right now I can't tell if this is being used, though. -kenton
void dutyCycle(float period, float onPercentage) {
  digitalWrite(transistor, HIGH);
  delay(int(period*onPercentage));
  digitalWrite(transistor, LOW);
  delay(int(period*(1-onPercentage)));
}

// ambiguous title: how is it different from startUp? -kenton
void startRun() { // ask user whether to start run (we are not doing this currently)
  int currentTime = -1;
  int previousTime;
  //for (int t = 0; t < time_setting*60*30*1000; t++) { // run for prescribed amount of time
  uint32_t period = time_setting*60*60000L; // 5 minutes
  // lmao is that a stackexchange link in that next comment :D -kenton
  for (uint32_t tStart = millis();  (millis()-tStart) < period;  ) { //https://arduino.stackexchange.com/questions/22272/how-do-i-run-a-loop-for-a-specific-amount-of-time

    // report temperature every hour to serial log
	 // I don't think every hour is enough to ensure that it was never out-of-bounds -kenton
    previousTime = currentTime;
    currentTime = (millis()-tStart)/3600000; //get current full hours elapsed
    if (currentTime != previousTime) {
      //Serial.print("Time (hours): ");
      Serial.print("Time (hours): ");
      Serial.println(currentTime); // time in hours
      Serial.print("Current Temperature: ");
      Serial.println(T);
      Serial.print("Running Average: ");
      Serial.println(T_average);
      Serial.print("");
      Serial.println("˚C");
    }
    
    delay(500);
    //resets screen words to black (clears characters)
    printBanner(T_setting, T_setting_prev, time_setting, time_setting_prev);
    tft.setCursor(0, 43);
    tft.setTextColor(BLACK);
    tft.print(T);
    tft.setCursor(30, 51);
    tft.setTextColor(BLACK); //blacks out the screen from the previous temperature measurement
    tft.print("IN RANGE");
    tft.setCursor(30, 51);
    tft.setTextColor(BLACK);
    tft.print("OUT OF RANGE");
    tft.setCursor(0, 59);
    tft.setTextColor(WHITE);
    tft.println("Finished Test was:"); // information on whether the internal temperature is at the correct temp over a trend of time
    if (finaltest == 1) { // build in the audio and visual alarms data here
      tft.setCursor(0, 67); 
      tft.setTextColor(BLACK);
      tft.print("SUCCESSFUL"); // what does "successful" and "not successful" mean here? -kenton
      tft.setCursor(0, 67);
      tft.setTextColor(WHITE);
      tft.print("NOT SUCCESSFUL");

      // trigger alarm by activating the LED and piezo
      digitalWrite(led, HIGH);
      //tone(piezo_alarm, 1000, 1000);
    }
    else {
      tft.setCursor(0, 67);
      tft.setTextColor(WHITE);
      tft.print("SUCCESSFUL");
    }

    //reads current temperature
    T = readTemp();

    // tft print function
    tftPrintTest();

    transistorControl();

    delay(500);

  }
  digitalWrite(transistor, LOW);
}

void loop() {

  selectButtonState = digitalRead(selectButton);

  // rename T_setting, it sounds so similar to time_setting. -kenton
  printBanner(T_setting, T_setting_prev, time_setting, time_setting_prev);
  
  if (selectButtonState == HIGH) { // pressing Down will increment menuopt by 1 
	 // TODO we should make a comment table explaining what menu options mean what -kenton.
    menuOpt = (menuOpt + 1) % 3; // increment the menu setting by 1
    //tft.fillScreen(BLACK);
    tft.fillRect(0, 40, 128, 60, BLACK);
  }
  if (menuOpt == 0) {
    setTemp(menuOpt);
  }
  else if(menuOpt == 1) {
    setTime(menuOpt);
  }
  else { // only entered if menuOpt is 2. -kenton
    runningAvg[0] = T_setting;
    runningAvg[1] = T_setting;
    runningAvg[2] = T_setting;
    runningAvg[3] = T_setting;
    runningAvg[4] = T_setting;

    startUp();
  }
}

// This appears to be a general method that either turns on or off the transistor,
// depending on where in the duty cycle we are. I would prefer two separate methods,
// or at least a more descriptive title. I also can't tell what 0.3 and 0.5 mean. -kenton
void transistorControl(){
  if (T > T_setting-0.3){
    //turn off transistor, stop power flow from battery to the heating pad
    digitalWrite(transistor, LOW);
    if (onStatus == 1) {
      tft.fillRect(67, 80, 128, 87, BLACK);
    }
    tft.setCursor(0,80);
    tft.setTextColor(WHITE);
    tft.setTextSize(0);
    tft.println("HEATING PAD OFF");
    onStatus = 0;
  } else if (T < T_setting-0.5) {
    //turn on transistor, start power flow from battery to the heating pad if in range or below range
    digitalWrite(transistor, HIGH); // Cory's note: switched from high to low here
    if (onStatus == 0) {
      tft.fillRect(67, 80, 128, 87, BLACK);
    }
    tft.setCursor(0,80);
    tft.setTextColor(WHITE);
    tft.setTextSize(0);
    tft.println("HEATING PAD ON");
    onStatus = 1;
  }
}

// used in the startUp() method to initialize the screen, it appears -kenton
void tftPrintStartUp() {
  tft.setCursor(0, 35);
  tft.setTextColor(WHITE);
  tft.setTextSize(0);
  tft.println("Current Temperature:");
  tft.setTextColor(RED);
  tft.println(T);
  tft.setCursor(0, 51);
  tft.setTextColor(WHITE);
  tft.setCursor(0, 51);
  tft.print("PRE-HEATING");
  tft.setCursor(0, 61);
  tft.print("Please wait");
}

// unclear how this is different from tftPrintStart(), but I'm assuming it erases something -kenton
void tftEraseStartUp() {
  tft.setCursor(0, 35);
  tft.setTextColor(BLACK);
  tft.setTextSize(0);
  tft.println("Current Temperature:");
  tft.println(T);
  tft.setCursor(0, 51);
  tft.setCursor(0, 51);
  tft.print("PRE-HEATING");
  tft.setCursor(0, 61);
  tft.print("Please wait");
}

void tftPrintTest() {
  // I think I understand why 4 is used, but can we not hard-code numbers into the program? -kenton
  for (int x = 4; x > 0; x--) // max number is number of sec b/t each measurement
  {
    runningAvg[x] = runningAvg[x-1];
  }
  runningAvg[0] = T;

  float sum = 0;
  for (int i = 0; i < 5; i++)
  {
    sum = sum + runningAvg[i];
  }
  T_average = sum / 5;
  
  tft.setCursor(0, 35);
  tft.setTextColor(WHITE);
  tft.setTextSize(0);
  tft.println("Current Temperature:");
  tft.setTextColor(RED);
  tft.println(T);
  tft.setCursor(0, 51);
  //reads stored temperature and determines if the temperature falls within range
  tft.setTextColor(WHITE);
  tft.print("Test:");
  if (T_average > T_setting-2 && T_average < T_setting+2) { // this is if the temperature is in range
    out_range_counter = 0;
    tft.setCursor(30, 51);
    tft.print("IN RANGE");
    // finaltest = 0;
  }
  else { // this is if the temperature is out of range
    out_range_counter++;
    tft.setCursor(30, 51);
    tft.print("OUT OF RANGE");
    if (out_range_counter >= 5) { // within 5 minutes, with a 15-second interval between measurements
      finaltest = 1; // in startRun(), this is meant to trigger the alarm/notification system -kenton
    }
  }
}
