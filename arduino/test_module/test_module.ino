#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin(4);
  Wire.onReceive(receiveData);
  Wire.onRequest(requestData);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}

void receiveData(int bits) {
  Serial.println("received data");
  int signal = Wire.read();
  int data = Wire.read();
  if (signal == 0) {
    Serial.println(data);
  }
}

void requestData() {
  // Serial.print("request data");
  // Wire.write("di");
}
