char* receive(int address, int bytes) {
  Wire.requestFrom(address, bytes);

  char string[Wire.available()];
  int index = 0;
  while (Wire.available() > 1) {
    string[index++] = Wire.read();
  }
  return string;
}

void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
