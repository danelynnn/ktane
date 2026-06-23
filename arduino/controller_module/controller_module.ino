// controller module!
// by Allie
//
// contains "heart" of the module, mostly just info:
// serial number, batteries, current timer, number of strikes

#define I2C_ADDRESS 8
#include <Wire.h>

enum OutputType
{
  ACTIVATE,
  SERIALODD,
  SERIALVOWEL,
  BATTERY,
  TIMER,
  STRIKES,
  RESET
};

enum Phase
{
  SETUP,
  READY,
  ACTIVE,
  SOLVED
};

char serial[] = "DL5QF3";
int batteries = 3;
int strikes = 0;

int numModules = 0;
int addresses[12];
int states[12];

bool bombStatus = false;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  // scan addresses for connected modules
  for (int i = 1; i <= 56; i++)
  {
    Wire.beginTransmission(I2C_ADDRESS + i);
    Wire.write(RESET);
    if (Wire.endTransmission() == 0)
    {
      Serial.println("done sending");
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

  printState();
}

void tryStart()
{
  bool ready = true;

  for (int i = 0; i < numModules; i++)
    if (states[i] != READY)
      ready = false;

  // if ready, activate all modules
  if (ready)
  {
    for (int i = 0; i < numModules; i++)
    {
      Serial.print("activating ");
      Serial.println(I2C_ADDRESS + addresses[i]);

      Wire.beginTransmission(I2C_ADDRESS + addresses[i]);
      Wire.write(RESET);
      Wire.endTransmission();
    }
  }
}

void loop()
{
  // put your main code here, to run repeatedly:

  // Wire.requestFrom(4, 16);
  // while (Wire.available()) { // peripheral may send less than requested
  //   char c = Wire.read(); // receive a byte as character
  //   Serial.print(c);         // print the character
  // }
  delay(1000);
}

byte createPacket(OutputType type)
{
  byte data;

  switch (type)
  {
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

void receiveData(int bytes)
{
  if (bytes == 2)
  {
    int address = Wire.read();
    int module = -1;
    for (int i = 0; i < numModules; i++)
    {
      if (address == addresses[i])
      {
        module = i;
        break;
      }
    }

    if (module == -1)
    {
      addresses[numModules] = address;
      module = numModules;
      numModules++;
    }

    int data = Wire.read();
    Serial.print("signal from ");
    Serial.print(address);
    Serial.print(" data ");
    Serial.println(data);

    if (data == 255)
    {
      strikes++;
    }
    else
    {
      states[module] = data;
    }
  }

  bool done = true;
  for (int i = 0; i < numModules; i++)
  {
    if (states[i] != SOLVED)
    {
      done = false;
    }
  }
  if (done)
  {
    bombStatus = true;
  }

  printState();
}

void sendData()
{
  // Serial.println("sending");
  Wire.write(SERIALODD);
  Wire.write(createPacket(SERIALODD));
}

void printArray(int arr[], int len)
{
  for (int i = 0; i < len; i++)
  {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}

void printState()
{
  Serial.print("module states: ");
  printArray(states, numModules);
  Serial.print(", strikes: ");
  Serial.print(strikes);
  Serial.print(", remaining time: ");
  Serial.print(", bomb is ");
  Serial.println(bombStatus ? "SOLVED" : "pending");
}

bool isSerialOdd()
{
  return (serial[5] - '0') % 2 == 1;
}

bool hasSerialVowel()
{
  for (int i = 0; i < 6; i++)
  {
    if (serial[i] == 'A' || serial[i] == 'E' || serial[i] == 'I' || serial[i] == 'O' || serial[i] == 'U')
      return true;
  }

  return false;
}
