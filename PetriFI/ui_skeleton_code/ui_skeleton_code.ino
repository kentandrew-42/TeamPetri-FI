//Sara Barker, modified July 5, 2021
//Team Petri-FI UI Code
  //This is the very rough and unfinished draft of the UI functionality code. 


//For right now, the variables are hardcoded into here so we don't have to set up the whole system to test.
int startingTemp = 30; //this is only used at the starting screen to show the inside temp of the container
int temp; //keeps track of temp inside container - will eventually be the thermistor reading
int runTime; //the time that is set at the beginning of the run
int elapsedTime; //the time that the device has been running for

void setup() {
  // put your setup code here, to run once:
//will fill this in later once we integrate the UI code into the main code
}

void loop() {
  /* This code works by checking the electrical state (i.e. time, temperature) and the button state (i.e. the value
   *  (0 or 1) of the three buttons).
   *  
  */

//********run startup screen first before entering into the switch statement

//*****check current temp, display, output OK button press
//startingTemp = read temp
//buttonState = startupScreen(startingTemp)

//now start the loop

  //check electrical state - gives temp, time

  //check button state


  //switch(elec state)
      //switch(button state)
          //use the minicubator code for detecting button presses
          

}

int startupScreen(startingTemp) {
/*The first screen - automatically starts up once the battery is connected.
 * Enter with battery connection, exit with OK
 * Inputs: currentTemp, outputs: OKbuttonState
 */

Serial.println("Current inside temperature: " + String( + "C");
Serial.print("Press OK to set new incubation");

//read button press
//return OK button value = 1


}

int setTemp(){
/*Setting temperature before beginning the incubation.
 *Enter with OK, exit with OK
 *Inputs: none, outputs: temp
 *
 *Note: setTemp goes directly into setTime
 */  

temp = currentTemp;
/*logic for buttons:
 * if up = 1 && down = 0
 *  temp = temp+1
 * if down = 1 && up = 0
 *  temp = temp-1
 *  loop until OK pressed
 */
Serial.print("Set temp: ")
Serial.print(temp) //put highlight on the number
Serial.println("C")
Serial.println("Set time: 48h")
Serial.print("Press OK to proceed")
//if OK button pressed, go to setTime(temp)
}

int setTime(temp){
  runTime = 48 //default = 48 hours

Serial.print("Set temp: " + String(temp) + "C")
Serial.print(temp) 
Serial.println("C")
Serial.print("Set time: ")
Serial.print(runTime)//put highlight on the number
Serial.println("h")
Serial.print("Press OK to proceed")
//return runTime
}


void summary(temp, runTime){
//Show summary screen with set temp/time, can start run or go back to setting screen
Serial.println("SUMMARY")
Serial.println("Temp: " + String(temp) + "C")
Serial.println("Time: " + String(runTime) + "h")
Serial.println("
}
