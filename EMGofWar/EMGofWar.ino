void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
}

void loop() {
  // send the value of analog input 0:
  Serial.print(analogRead(A0));
  Serial.print(',');
  Serial.println(analogRead(A5));
  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(2);
}

