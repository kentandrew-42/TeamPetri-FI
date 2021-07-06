/*
  TemperatureRead

  Reads temperature from pin A0, based on analog thermistor input.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// old loop without the running average
//void loop() {
//  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
//  int R1 = 10000; // resistance of reference resistor
//  
//  // read the input on analog pin 0:
//  int Vo = analogRead(A0); // reads the voltage of ThermistorPin. Resolution of only 4.9 mV (out of 5V)
//  Serial.println("----------");
//  // Serial.print("Analog Value: ");
//  // Serial.println(Vo);
//  
//  float R2 = R1 / ((1023.0 / (float)Vo) - 1.0); // resistance of thermistor
//  // Serial.print("Thermistor Resistance:");
//  // Serial.println(R2);
//
//  // Minicubator's Model
//  float logR2 = log(R2);
//  float T = 1.0 / (c1 + (c2 * logR2) + (c3 * pow(logR2, 3)));
//  T = T - 273.15;
//  // print out the value you read:
//  // Serial.print("Old Temperature Model:");
//  // Serial.println(T);
//
//  // Adafruit Thermistor Lookup Table Curve Fit
//  float T_new = log(((R2/1000) / 28.7)) / (-0.0422);
//  Serial.print("Temperature (C): ");
//  Serial.println(T_new);
//
//  delay(1000); // delay in milliseconds between reads
//}

void loop() {
  float sum = 0; // running sum of temperature readings each loop.

  for (int i = 0; i < 5; ++i) {
    Serial.print("/ ");
    sum += readTemp();
    delay(1000); // delay is one sec between readings, 5 secs between displays
  }
  Serial.println();

  float averageTemp = sum / 5;
  Serial.print("Temperature (C): ");
  Serial.println(averageTemp); // displays the average over 5 readings
}

float readTemp() {
  /* Reads the temperature from the thermsitor, using information
   *  from Adafruit's thermistor lookup table of resistances.
   */

  int R1 = 10000; // resistance of reference resistor
  
  // read the input on analog pin 0:
  // TODO use variable for thermistor pin (don't specify A0)
  int Vo = analogRead(A0); // reads the voltage of ThermistorPin. Resolution of only 4.9 mV (out of 5V)

  float R2 = R1 / ((1023.0 / (float)Vo) - 1.0); // resistance of thermistor
  float T = log(((R2/1000) / 28.7)) / (-0.0422); // curve fit equation based on thermistor's lookup table

  return T;
}

float OLDreadTemp() {
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
