#define WIRE_SUCCESS 2
#define WIRE_ONE 3  // Read pin
#include <Wire.h>

enum Color {
  NONE,
  BLUE,
  RED,
  YELLOW,
  PURPLE,
  WHITE,
  BLACK
};

enum RequestType {
  ACTIVATE, SERIALODD, SERIALVOWEL, BATTERY, TIMER, STRIKES
};

enum Phase {
  SETUP, PENDING, ACTIVE, SOLVED
};

Phase phase = SETUP;
int state[] = {-1, -1, -1, -1, -1, -1};
int answer = -1;

// hardcoding things, this will eventually be randomised/sent from elsewhere
Color colors[] = {RED, NONE, RED, YELLOW, BLUE, NONE};
bool isSerialOdd = true;

int countWires(Color color) {
  int count = 0;
  for (int i=0; i<6; i++) {
    if (colors[i] == color) {
      count++;
    }
  }
  return count;
}

void printArray(int arr[], int len) {
  for (int i=0; i<len; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}

int handleRules(Color colors[]) {
  int connectedWires[6]; // list of connected *indices*
  int numWires = 0;

  for (int i=0; i<6; i++) {
    if (colors[i] != NONE) {
      connectedWires[numWires++] = i;
    }
  }

  switch(numWires) {
    case 3:
      if (countWires(RED) == 0) {
        return connectedWires[1];
      } else if (colors[connectedWires[numWires-1]] == WHITE) {
        return connectedWires[numWires-1];
      } else if (countWires(BLUE) > 1) {
        // last blue wire
        for (int i=numWires-1; i >= 0; i--) {
          if (colors[connectedWires[i]] == BLUE) {
            return connectedWires[i];
          }
        }
      } else {
        return connectedWires[numWires-1];
      } 
      break;
    case 4:
      if (countWires(RED) > 1 && isSerialOdd) {
        // last red wire
        for (int i=numWires-1; i >= 0; i--) {
          if (colors[connectedWires[i]] == RED) {
            return connectedWires[i];
          }
        }
      } else if (colors[connectedWires[numWires-1]] == YELLOW && countWires(RED) == 0) {
        return connectedWires[0];
      } else if (countWires(BLUE) == 1) {
        return connectedWires[0];
      } else if (countWires(YELLOW) > 1) {
        return connectedWires[numWires-1];
      } else {
        return connectedWires[1];
      }
      break;
    case 5:
      if (colors[connectedWires[numWires-1]] == BLACK && isSerialOdd) {
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

void printState() {
  Serial.print("we are in phase ");
  Serial.print(phase);
  Serial.print(", current state is ");
  printArray(state, 6);

  if (phase == ACTIVE) {
    Serial.print(", correct answer is ");
    Serial.print(answer);
  }
  
  Serial.println();
}

void onChange(int pin, int status) {
  Serial.print("pin ");
  Serial.print(pin);
  Serial.print(" has been set to ");
  Serial.print(status);
  Serial.println();

  if (phase == SETUP) {
    bool ready = true;
    for (int i=0; i<6; i++) {
      if ((colors[i] != NONE && state[i] == 1) || (colors[i] == NONE && state[i] == 0)) {
      } else {
        ready = false;
      }
    }

    if (ready) {
      phase = 1;
      answer = handleRules(colors);
    }
  } else if (phase == ACTIVE) {
    if (pin == answer) {
      phase = SOLVED;
      digitalWrite(WIRE_SUCCESS, true);

      Wire.beginTransmission(8);
      Wire.write(1);
      Wire.write(69);
      Wire.endTransmission();
    } else {
      Serial.println("mistake");

      Wire.beginTransmission(8);
      Wire.write(1);
      Wire.write(-1);
      Wire.endTransmission();
    }
  }

  printState();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Wire.begin(9);
  Wire.onReceive(receiveData);
  Wire.onRequest(requestData);

  pinMode(WIRE_SUCCESS, OUTPUT);
  // set all pins as input
  for (int i=0; i<6; i++)
    pinMode(WIRE_ONE+i, INPUT);
  
  Serial.println("setup complete, starting state");
  printState();
}

int readout[] = {0, 0, 0, 0, 0, 0};
int debounce[] = {0, 0, 0, 0, 0, 0};
void loop() {
  for (int i=0; i<6; i++)
    readout[i] = digitalRead(WIRE_ONE+i);
  
  for (int i=0; i<6; i++) {
    if (readout[i] != state[i]) {
      // state[i] = readout[i];
      // onChange(i, state[i]);

      debounce[i]++;
      if (debounce[i] == 10000) {
        state[i] = readout[i];
        onChange(i, state[i]);

        debounce[i] = 0;
      }
    } else {
      debounce[i] = 0;
    }
  }
}

void receiveData(int bytes) {
  RequestType signal = Wire.read();
  Serial.print("received data, type ");
  Serial.println(signal);
  int data = Wire.read();

  switch (signal) {
  case ACTIVATE:
    Serial.println("activate received");
    phase = ACTIVE;
    printState();
  case SERIALODD:
    isSerialOdd = data;
  }
}

void requestData() {
  Serial.print("sending status ");
  Serial.println(phase);
  Wire.write(phase);
}
