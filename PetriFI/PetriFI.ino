/*
  Team Petri-FI | SEED Internship & Rice360 Institute
  
  Contributors:
  Kenton Roberts
  Nancy Lindsey
  Teja Paturu
  Sara Barker
  
  Code Adapted From Previous Student Teams:
  Moonrat
  Minicubator
  and Contributions from the Public Invention Moonrat Team.
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
  tft_screen.fillRect(0, 0, 128, 128, BLACK);
  time = millis() - time;
}

void loop() {

  selectButtonState = digitalRead(selectButton);

  // rename temp_setting, it sounds so similar to time_setting. -kenton
  printBanner(temp_setting, temp_setting_prev, time_setting, time_setting_prev);
  
  if (selectButtonState == HIGH) { // pressing Down will increment menuopt by 1 
	 // TODO we should make a comment table explaining what menu options mean what -kenton.
    menuOpt = (menuOpt + 1) % 3; // increment the menu setting by 1
    //tft_screen.fillScreen(BLACK);
    tft_screen.fillRect(0, 40, 128, 60, BLACK);
  }
  if (menuOpt == 0) {
    setTemp(menuOpt);
  }
  else if(menuOpt == 1) {
    setTime(menuOpt);
  }
  else { // only entered if menuOpt is 2
    runningAvg[0] = temp_setting;
    runningAvg[1] = temp_setting;
    runningAvg[2] = temp_setting;
    runningAvg[3] = temp_setting;
    runningAvg[4] = temp_setting;

    startUp();
  }
}

void printBanner(int temp_setting, int temp_setting_prev, int time_setting, int time_setting_prev) {
//prints the yellow text at the top that shows the selected setting while powered on.
  tft_screen.setCursor(0,10);
  tft_screen.setTextColor(YELLOW);
  tft_screen.print("Curr Temp Set (C): "); // Temperature set prompt
  tft_screen.print(temp_setting);
  
  tft_screen.setCursor(0,20);
  tft_screen.setTextColor(YELLOW);
  tft_screen.print("Curr Time Set (h): "); // Time set prompt
  tft_screen.print(time_setting);
  
}

void setTemp(int m) { // menu option to set temperature
  /*
    Sets the temperature for the next incubation period.
  */

  // where the heck is "int m" used in this method? -kenton
  DownState = digitalRead(Down);
  UpState = digitalRead(Up);

  int temp_setting_prev = temp_setting;
  
  if ((DownState == HIGH) && (temp_setting <= MAX_TEMP_SET) && (temp_setting > MIN_TEMP_SET)) { 
    temp_setting--; // decrement the temperature setting by 1, given within the acceptable range

  }
  if ((UpState == HIGH) && (temp_setting < MAX_TEMP_SET) && (temp_setting >= MIN_TEMP_SET)) {
    temp_setting++; // increment the temperature setting by 1, given within the acceptable range
    
  }  

  if ((temp_setting != temp_setting_prev) || (firstTempSet == true)) {
    tft_screen.setCursor(0,10); // this code blots out the yellow print at the top while the temperature selection is occurring
    tft_screen.setTextColor(YELLOW);
    tft_screen.print("Curr Temp Set (C): ");
    tft_screen.setTextColor(BLACK);
    tft_screen.print(temp_setting_prev);
    tft_screen.setCursor(0,50); // display the selection in progress to the user.
    tft_screen.setTextColor(WHITE);
    tft_screen.println("Set temperature (C): ");
    tft_screen.setCursor(0,60);
    tft_screen.setTextColor(BLACK);
    tft_screen.print(temp_setting_prev);
    tft_screen.setCursor(0,60);
    tft_screen.setTextColor(MAGENTA);
    tft_screen.print(temp_setting);
  }

  //delay(250);
  firstTempSet = false;
}

void setTime(int m) {
  /*
    Activates the menu option to set the duration of incubation.
	 Modifies 

  */
  
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
    tft_screen.setCursor(0,20); // this code blots out the yellow print at the top while the time selection is occurring
    tft_screen.setTextColor(YELLOW);
    tft_screen.print("Curr Time Set (h): ");
    tft_screen.setTextColor(BLACK);
    tft_screen.print(time_setting_prev);
    tft_screen.setCursor(0,50); // display the selection in progress to the user.
    tft_screen.setTextColor(WHITE);
    tft_screen.println("Set time (hrs): ");
    tft_screen.setCursor(0,60);
    tft_screen.setTextColor(BLACK);
    tft_screen.print(time_setting_prev);
    tft_screen.setCursor(0,60);
    tft_screen.setTextColor(MAGENTA);
    tft_screen.print(time_setting);
  }

  firstTimeSet = false;
}


float readTemp() {
  /*
    Performs mathematical operations on the voltage reading from the thermistor pin.
    Calculates Kelvin temperature, then outputs as a Celsius reading.
    
    Returns: T - Celsius temperature reading from temperature sensor
  */

  Vo = analogRead(ThermistorPin); // reads the voltage of ThermistorPin. Resolution of only 4.9 mV (out of 5V)
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = 1.0 / (c1 + (c2 * logR2) + (c3 * pow(logR2, 3)));
  T = T - 273.15;
  return T;
}

void startUp() {
  T = readTemp();
  // while loop exits when temp gets up to adequate temperature, it appears -kenton
  while (T < temp_setting-0.5) {
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
    printBanner(temp_setting, temp_setting_prev, time_setting, time_setting_prev);
    tft_screen.setCursor(0, 43);
    tft_screen.setTextColor(BLACK);
    tft_screen.print(T);
    tft_screen.setCursor(30, 51);
    tft_screen.setTextColor(BLACK); //blacks out the screen from the previous temperature measurement
    tft_screen.print("IN RANGE");
    tft_screen.setCursor(30, 51);
    tft_screen.setTextColor(BLACK);
    tft_screen.print("OUT OF RANGE");
    tft_screen.setCursor(0, 59);
    tft_screen.setTextColor(WHITE);
    tft_screen.println("Finished Test was:"); // information on whether the internal temperature is at the correct temp over a trend of time
    if (finaltest == 1) { // build in the audio and visual alarms data here
      tft_screen.setCursor(0, 67); 
      tft_screen.setTextColor(BLACK);
      tft_screen.print("SUCCESSFUL"); // what does "successful" and "not successful" mean here? -kenton
      tft_screen.setCursor(0, 67);
      tft_screen.setTextColor(WHITE);
      tft_screen.print("NOT SUCCESSFUL");

      // trigger alarm by activating the LED and piezo
      digitalWrite(led, HIGH);
      //tone(piezo_alarm, 1000, 1000);
    }
    else {
      tft_screen.setCursor(0, 67);
      tft_screen.setTextColor(WHITE);
      tft_screen.print("SUCCESSFUL");
    }

    //reads current temperature
    T = readTemp();

    // tft_screen print function
    tftPrintTest();

    transistorControl();

    delay(500);

  }
  digitalWrite(transistor, LOW);
}

// This appears to be a general method that either turns on or off the transistor,
// depending on where in the duty cycle we are. I would prefer two separate methods,
// or at least a more descriptive title. I also can't tell what 0.3 and 0.5 mean. -kenton
void transistorControl(){
  /*
    Controls the transistor. When transistor is turned on (HIGH), it allows power flow from
	 the battery to the heaing pad. When the transistor is turned off (LOW), it stops this
	 flow and turns off the heating pad.
  */
  if (T > temp_setting-0.3){
    //turn off transistor, stop power flow from battery to the heating pad
    digitalWrite(transistor, LOW);
    if (onStatus == 1) {
      tft_screen.fillRect(67, 80, 128, 87, BLACK);
    }
    tft_screen.setCursor(0,80);
    tft_screen.setTextColor(WHITE);
    tft_screen.setTextSize(0);
    tft_screen.println("HEATING PAD OFF");
    onStatus = 0;
  } else if (T < temp_setting-0.5) {
    //turn on transistor, start power flow from battery to the heating pad if in range or below range
    digitalWrite(transistor, HIGH); // Cory's note: switched from high to low here
    if (onStatus == 0) {
      tft_screen.fillRect(67, 80, 128, 87, BLACK);
    }
    tft_screen.setCursor(0,80);
    tft_screen.setTextColor(WHITE);
    tft_screen.setTextSize(0);
    tft_screen.println("HEATING PAD ON");
    onStatus = 1;
  }
}

void tftPrintStartUp() {
  /*
    This method starts the tft_screen and prints the startup text.

	 Returns: none
  */

  tft_screen.setCursor(0, 35);
  tft_screen.setTextColor(WHITE);
  tft_screen.setTextSize(0);
  tft_screen.println("Current Temperature:");
  tft_screen.setTextColor(RED);
  tft_screen.println(T);
  tft_screen.setCursor(0, 51);
  tft_screen.setTextColor(WHITE);
  tft_screen.setCursor(0, 51);
  tft_screen.print("PRE-HEATING");
  tft_screen.setCursor(0, 61);
  tft_screen.print("Please wait");
}

void tftEraseStartUp() {
  /*
    Erases the start-up text by overwriting it with BLACK text.

	 Returns: none
  */
  tft_screen.setCursor(0, 35);
  tft_screen.setTextColor(BLACK);
  tft_screen.setTextSize(0);
  tft_screen.println("Current Temperature:");
  tft_screen.println(T);
  tft_screen.setCursor(0, 51);
  tft_screen.setCursor(0, 51);
  tft_screen.print("PRE-HEATING");
  tft_screen.setCursor(0, 61);
  tft_screen.print("Please wait");
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
  
  tft_screen.setCursor(0, 35);
  tft_screen.setTextColor(WHITE);
  tft_screen.setTextSize(0);
  tft_screen.println("Current Temperature:");
  tft_screen.setTextColor(RED);
  tft_screen.println(T);
  tft_screen.setCursor(0, 51);
  //reads stored temperature and determines if the temperature falls within range
  tft_screen.setTextColor(WHITE);
  tft_screen.print("Test:");
  if (T_average > temp_setting-2 && T_average < temp_setting+2) { // this is if the temperature is in range
    out_range_counter = 0;
    tft_screen.setCursor(30, 51);
    tft_screen.print("IN RANGE");
    // finaltest = 0;
  }
  else { // this is if the temperature is out of range
    out_range_counter++;
    tft_screen.setCursor(30, 51);
    tft_screen.print("OUT OF RANGE");
    if (out_range_counter >= 5) { // within 5 minutes, with a 15-second interval between measurements
      finaltest = 1; // in startRun(), this is meant to trigger the alarm/notification system -kenton
    }
  }
}
