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

// the loop routine runs over and over again forever:
void loop() {
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  int R1 = 10000; // resistance of reference resistor
  
  // read the input on analog pin 0:
  int Vo = analogRead(A0); // reads the voltage of ThermistorPin. Resolution of only 4.9 mV (out of 5V)
  Serial.print("----------");
  Serial.print("Analog Value: ");
  Serial.println(Vo);
  
  float R2 = R1 / ((1023.0 / (float)Vo) - 1.0); // resistance of thermistor
  Serial.print("Thermistor Resistance:");
  Serial.println(R2);

  // Minicubator's Model
  float logR2 = log(R2);
  float T = 1.0 / (c1 + (c2 * logR2) + (c3 * pow(logR2, 3)));
  T = T - 273.15;
  // print out the value you read:
  Serial.print("Old Temperature Model:");
  Serial.println(T);

  // Adafruit Thermistor Lookup Table Curve Fit
  float T_new = log(((R2/1000) / 28.7)) / (-0.0422);
  Serial.print("New Temperature Model:");
  Serial.println(T_new);

  delay(1000); // delay in milliseconds between reads
}
