// controller module!
// by Allie
//
// contains "heart" of the module, mostly just info:
// serial number, batteries, current timer, number of strikes

#include <Wire.h>

char serial[] = "DL5QF2";
int batteries = 3;
int strikes = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(1);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}

char* receive(int bytes) {
  char string[Wire.available()];
  int index = 0;
  while (Wire.available() > 1) {
    string[index++] = Wire.read();
  }
  return string;
}

void send(char stream[]) {
  Wire.write(stream);
}

bool hasSerialVowel() {
  for (int i=0; i<6; i++) {
    if (serial[i] == 'A' || serial[i] == 'E' || serial[i] == 'I' || serial[i] == 'O' || serial[i] == 'U')
      return true;
  }

  return false;
}

bool isSerialOdd() {
  return (serial[5] - '0') % 2 == 1;
}


// used to inform controller what data to send
void receiveEvent(int bytes) {
  Serial.print("got ");
  Serial.print(bytes);
  Serial.print(" bytes");
  char* data = receive(bytes);
  Serial.println(data);
}

void requestEvent() {
  Serial.println("asking for some");
  char data[] = "hi";
  send(data);
}
