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
  R1 = 10000
  // read the input on analog pin 0:
  Vo = analogRead(A0); // reads the voltage of ThermistorPin. Resolution of only 4.9 mV (out of 5V)
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = 1.0 / (c1 + (c2 * logR2) + (c3 * pow(logR2, 3)));
  T = T - 273.15;
  // print out the value you read:
  Serial.println(T);
  delay(1); // delay in between reads for stability
}
