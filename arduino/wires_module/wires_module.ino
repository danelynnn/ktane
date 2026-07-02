// wires module!
// by Allie
//
// contains the functionality for the simple wires module; references a
// controller module for data

#define CONTROLLER_ADDRESS 8
#define I2C_ADDRESS CONTROLLER_ADDRESS + 1 // address 1
#define WIRE_SUCCESS 2
#define WIRE_ONE 3 // Read pin

#include <Wire.h>

enum InputType {
  ACTIVATE,
  SERIALODD,
  SERIALVOWEL,
  BATTERY,
  TIMER,
  STRIKES,
  RESET
};

enum Color { NONE, BLUE, RED, YELLOW, PURPLE, WHITE, BLACK };

enum Phase { SETUP, READY, ACTIVE, SOLVED };

Phase phase = SETUP;
int states[] = {-1, -1, -1, -1, -1, -1};
int answer = -1;

// current game state, this is partially local and partially pulled from
// controller
Color colors[] = {RED, NONE, RED, YELLOW, BLUE, NONE};
bool isSerialOdd = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println();

  // address 1
  Wire.begin(I2C_ADDRESS + 1);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  pinMode(WIRE_SUCCESS, OUTPUT);
  // set all pins as input
  for (int i = 0; i < 6; i++)
    pinMode(WIRE_ONE + i, INPUT);

  // scan for controller
  Wire.beginTransmission(CONTROLLER_ADDRESS);
  sendData(); // send state to controller
  if (Wire.endTransmission() == 0) {
    // request starting variables from controller
    Wire.requestFrom(CONTROLLER_ADDRESS, 2);
    receiveData(2);
  }

  Serial.println("setup complete:");
  printState();
}

int readout[] = {0, 0, 0, 0, 0, 0};
int debounce[] = {0, 0, 0, 0, 0, 0};

void loop() {
  for (int i = 0; i < 6; i++)
    readout[i] = digitalRead(WIRE_ONE + i);

  for (int i = 0; i < 6; i++) {
    if (readout[i] != states[i]) {
      debounce[i]++;
      if (debounce[i] == 10000) {
        setPin(i, readout[i]);
        debounce[i] = 0;
      }
    } else {
      debounce[i] = 0;
    }
  }
}

void receiveData(int bytes) {
  InputType signal = Wire.read();
  int data = Wire.read();

  Serial.print("received data, type ");
  Serial.print(signal);
  Serial.print(" data ");
  Serial.println(data);

  switch (signal) {
  case ACTIVATE:
    phase = ACTIVE;
    answer = handleRules(colors);
    printState();
    break;
  case SERIALODD:
    isSerialOdd = data;
    answer = handleRules(colors);
    break;
  case RESET:
    phase = SETUP;
    update(-1);
    break;
  }
}

void sendData() {
  Serial.print("sending from ");
  Serial.print(I2C_ADDRESS);
  Wire.write(I2C_ADDRESS);
  Serial.print(" data ");
  Serial.println(phase);
  Wire.write(phase);
}

// returns whether should report to controller
bool update(int pin) {
  bool shouldReport = false;

  if (phase == SETUP) {
    bool ready = true;
    for (int i = 0; i < 6; i++) {
      if ((colors[i] != NONE && states[i] == 1) ||
          (colors[i] == NONE && states[i] == 0)) {
      } else {
        ready = false;
      }
    }

    if (ready) {
      phase = READY;
      shouldReport = true;
    }
  } else if (phase == READY || phase == ACTIVE) {
    if (pin == answer) {
      phase = SOLVED;
      shouldReport = true;
    } else {
      Serial.println("mistake");

      Wire.beginTransmission(8);
      Wire.write(9);
      Wire.write(-1);
      Wire.endTransmission();
    }
  }

  if (phase == SOLVED) {
    digitalWrite(WIRE_SUCCESS, true);
  } else {
    digitalWrite(WIRE_SUCCESS, false);
  }
  printState();

  return shouldReport;
}

void setPin(int pin, int state) {
  states[pin] = state;

  Serial.print("pin ");
  Serial.print(pin);
  Serial.print(" has been set to ");
  Serial.print(state);
  Serial.println();

  if (update(pin)) {
    Wire.beginTransmission(8);
    sendData();
    Wire.endTransmission();
  }
}

int handleRules(Color colors[]) {
  int connectedWires[6]; // list of connected *indices*
  int numWires = 0;

  for (int i = 0; i < 6; i++) {
    if (colors[i] != NONE) {
      connectedWires[numWires++] = i;
    }
  }

  switch (numWires) {
  case 3:
    if (countWires(RED) == 0) {
      return connectedWires[1];
    } else if (colors[connectedWires[numWires - 1]] == WHITE) {
      return connectedWires[numWires - 1];
    } else if (countWires(BLUE) > 1) {
      // last blue wire
      for (int i = numWires - 1; i >= 0; i--) {
        if (colors[connectedWires[i]] == BLUE) {
          return connectedWires[i];
        }
      }
    } else {
      return connectedWires[numWires - 1];
    }
    break;
  case 4:
    if (countWires(RED) > 1 && isSerialOdd) {
      // last red wire
      for (int i = numWires - 1; i >= 0; i--) {
        if (colors[connectedWires[i]] == RED) {
          return connectedWires[i];
        }
      }
    } else if (colors[connectedWires[numWires - 1]] == YELLOW &&
               countWires(RED) == 0) {
      return connectedWires[0];
    } else if (countWires(BLUE) == 1) {
      return connectedWires[0];
    } else if (countWires(YELLOW) > 1) {
      return connectedWires[numWires - 1];
    } else {
      return connectedWires[1];
    }
    break;
  case 5:
    if (colors[connectedWires[numWires - 1]] == BLACK && isSerialOdd) {
      return connectedWires[3];
    } else if (countWires(RED) == 1 && countWires(YELLOW) > 1) {
      return connectedWires[0];
    } else if (countWires(BLACK) == 0) {
      return connectedWires[1];
    } else {
      return connectedWires[0];
    }
    break;
  case 6:
    if (countWires(YELLOW) == 0 && isSerialOdd) {
      return connectedWires[2];
    } else if (countWires(YELLOW) == 1 && countWires[WHITE] > 1) {
      return connectedWires[3];
    } else if (countWires(RED) == 0) {
      return connectedWires[numWires - 1];
    } else {
      return connectedWires[3];
    }
    break;
  }
}

int countWires(Color color) {
  int count = 0;

  for (int i = 0; i < 6; i++) {
    if (colors[i] == color)
      count++;
  }

  return count;
}

void printArray(int arr[], int len) {
  Serial.print("[");
  for (int i = 0; i < len; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
  Serial.print("]");
}

void printState() {
  Serial.print("we are in phase ");
  Serial.print(phase);
  Serial.print(", current state is ");
  printArray(states, 6);

  if (phase == ACTIVE) {
    Serial.print(", correct answer is ");
    Serial.print(answer);
  }

  Serial.println();
}
