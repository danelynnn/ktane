// controller module!
// by Allie
//
// contains "heart" of the module, mostly just info:
// serial number, batteries, current timer, number of strikes

#define I2C_ADDRESS 8
#include <Wire.h>

enum RequestType {
  ACTIVATE, SERIALODD, SERIALVOWEL, BATTERY, TIMER, STRIKES
};

enum Phase {
  SETUP, PENDING, ACTIVE, SOLVED
};

char serial[] = "DL5QF3";
int batteries = 3;
int strikes = 0;

int numModules = 0;
int modules[12];
int phases[12];

bool bombStatus = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(I2C_ADDRESS);

  Wire.onReceive(receiveData);

  // scan addresses for connected modules
  for (int i=0; i<56; i++) {
    Wire.beginTransmission(I2C_ADDRESS + i);
    
    if (Wire.endTransmission() == 0) {
      modules[numModules] = i;
      numModules++;
    }
  }

  // send starting variables to all connected modules
  bool ready = true;
  for (int i=0; i<numModules; i++) {
    Serial.print("pinging ");
    Serial.println(I2C_ADDRESS + modules[i]);

    Wire.beginTransmission(I2C_ADDRESS + modules[i]);
    Wire.write(SERIALODD);
    Wire.write(createPacket(SERIALODD));
    Serial.println(Wire.endTransmission() == 0 ? "success" : "error");

    // request status from module
    Wire.requestFrom(I2C_ADDRESS + modules[i], 1);
    while (Wire.available()) {
      Phase phase = Wire.read();
      phases[i] = phase;

      if (phase != PENDING) {
        ready = false;
      }
    }
  }

  // if ready, activate all modules
  if (ready) {
    for (int i=0; i<numModules; i++) {
      Serial.print("activating ");
      Serial.println(I2C_ADDRESS + modules[i]);

      Wire.beginTransmission(I2C_ADDRESS + modules[i]);
      Wire.write(ACTIVATE);
      Wire.endTransmission();
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // Wire.requestFrom(4, 16);
  // while (Wire.available()) { // peripheral may send less than requested
  //   char c = Wire.read(); // receive a byte as character
  //   Serial.print(c);         // print the character
  // }
  printState();
  delay(1000);
}

byte createPacket(RequestType type) {
  byte data;

  switch (type) {
  case SERIALODD:
    data = isSerialOdd();
    break;
  case SERIALVOWEL:
    data = hasSerialVowel();
  case BATTERY:
    data = batteries ? batteries < 4 : 4;
    break;
  case TIMER:
    // timer i can't be fucked
    break;
  case STRIKES:
    data = strikes;
    break;
  default:
    break;
  }

  return data;
}

void receiveData(int bytes) {
  if (bytes == 2) {
    int module = Wire.read();
    for (int i=0; i<numModules; i++) {
      if (module == modules[i]) {
        module = i;
        break;
      }
    }

    int data = Wire.read();
    Serial.print("signal from ");
    Serial.println(module);
    
    if (data == 255) {
      strikes++;
    } else if (data == 69) {
      phases[module] = SOLVED;
    }
  }

  bool done = true;
  for (int i=0; i<numModules; i++) {
    if (phases[i] != SOLVED) {
      done = false;
    }
  }
  if (done) {
    bombStatus = true;
  }
}

void printArray(int arr[], int len) {
  for (int i=0; i<len; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}

void printState() {
  Serial.print("module phases: ");
  printArray(phases, numModules);
  Serial.print(", strikes: ");
  Serial.print(strikes);
  Serial.print(", remaining time: ");
  Serial.print(", bomb is ");
  Serial.println(bombStatus ? "SOLVED" : "pending");
}

bool isSerialOdd() {
  return (serial[5] - '0') % 2 == 1;
}

bool hasSerialVowel() {
  for (int i=0; i<6; i++) {
    if (serial[i] == 'A' || serial[i] == 'E' || serial[i] == 'I' || serial[i] == 'O' || serial[i] == 'U')
      return true;
  }

  return false;
}
