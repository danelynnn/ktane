// controller module!
// by Allie
//
// contains "heart" of the module, mostly just info:
// serial number, batteries, current timer, number of strikes

#define I2C_ADDRESS 8

#include <Wire.h>

enum OutputType {
  ACTIVATE,
  SERIALODD,
  SERIALVOWEL,
  BATTERY,
  TIMER,
  STRIKES,
  RESET
};

enum Phase { SETUP, READY, ACTIVE, SOLVED };

char serial[] = "DL5QF3";
int batteries = 3;
int strikes = 0;

int numModules = 0;
int addresses[12];
int states[12];

bool bombStatus = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println();

  // address 0
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  // scan addresses for connected modules
  for (int i = 1; i <= 56; i++) {
    Wire.beginTransmission(I2C_ADDRESS + i);
    Wire.write(RESET);
    if (Wire.endTransmission() == 0) {
      // request state from module
      Wire.requestFrom(I2C_ADDRESS + i, 2);
      receiveData(2);

      // send starting variables to module
      Wire.beginTransmission(I2C_ADDRESS + i);
      Wire.write(SERIALODD);
      Wire.write(createPacket(SERIALODD));
      Wire.endTransmission();
    }
  }

  Serial.println("setup complete:");
  printState();
}

void loop() {
  // put your main code here, to run repeatedly:

  // Wire.requestFrom(4, 16);
  // while (Wire.available()) { // peripheral may send less than requested
  //   char c = Wire.read(); // receive a byte as character
  //   Serial.print(c);         // print the character
  // }
  delay(1000);
}

void receiveData(int bytes) {
  int index = getModuleIndex(Wire.read());
  int data = Wire.read();

  Serial.print("signal from module ");
  Serial.print(index);
  Serial.print(" data ");
  Serial.println(data);

  if (data == 255) // module sent -1, strike made
    strikes++;
  else // module sent status
    states[index] = data;

  bool solved = true;
  for (int i = 0; i < numModules; i++) {
    if (states[i] != SOLVED)
      solved = false;
  }
  if (solved)
    bombStatus = true;
  else
    bombStatus = false;

  printState();
}

void sendData() {
  Serial.print("sending type ");
  Serial.print(SERIALODD);
  Wire.write(SERIALODD);
  Serial.print(" data ");
  Serial.println(createPacket(SERIALODD));
  Wire.write(createPacket(SERIALODD));
}

void tryStart() {
  bool ready = true;

  for (int i = 0; i < numModules; i++)
    if (states[i] != READY)
      ready = false;

  // if ready, activate all modules
  if (ready) {
    for (int i = 0; i < numModules; i++) {
      Serial.print("activating ");
      Serial.println(I2C_ADDRESS + addresses[i]);

      Wire.beginTransmission(I2C_ADDRESS + addresses[i]);
      Wire.write(RESET);
      Wire.endTransmission();
    }
  }
}

int getModuleIndex(int address) {
  int index = -1;
  for (int i = 0; i < numModules; i++) {
    if (address == addresses[i]) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    addresses[numModules] = address;
    return numModules++;
  } else {
    return index;
  }
}

bool isSerialOdd() { return (serial[5] - '0') % 2 == 1; }

bool hasSerialVowel() {
  for (int i = 0; i < 6; i++) {
    if (serial[i] == 'A' || serial[i] == 'E' || serial[i] == 'I' ||
        serial[i] == 'O' || serial[i] == 'U')
      return true;
  }

  return false;
}

byte createPacket(OutputType type) {
  byte data;

  switch (type) {
  case SERIALODD:
    data = isSerialOdd();
    break;
  case SERIALVOWEL:
    data = hasSerialVowel();
    break;
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

void printArray(int arr[], int len) {
  for (int i = 0; i < len; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}

void printState() {
  Serial.print("module states: ");
  printArray(states, numModules);
  Serial.print(", strikes: ");
  Serial.print(strikes);
  Serial.print(", remaining time: ");
  Serial.print(", bomb is ");
  Serial.println(bombStatus ? "SOLVED" : "pending");
}
