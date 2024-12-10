#define WIRE_ONE 2  // Read pin
#define WIRE_SUCCESS 8

enum Color {
  NONE,
  BLUE,
  RED,
  YELLOW,
  PURPLE,
  WHITE,
  BLACK
};

int phase = 0; // 0 = setup, 1 = execution
int state[] = {-1, -1, -1, -1, -1, -1};
int strikes = 0;
int answer = -1;
bool success = false;

// hardcoding things, this will eventually be randomised/sent from elsewhere
Color colors[] = {NONE, BLUE, YELLOW, RED, NONE, RED};
char serial[] = "AL5QF3";

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
  bool isSerialOdd = (serial[5] - '0') % 2 == 1;

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

  if (phase == 1 || phase == 2) {
    Serial.print(", strikes = ");
    Serial.print(strikes);
    Serial.print(", correct answer is ");
    Serial.print(answer);
    Serial.print(", module status is ");

    if (success) {
      Serial.println("defused");
    } else {
      Serial.println("pending");
    }
  } else {
    Serial.println();
  }
}

void onChange(int pin, int status) {
  Serial.print("pin ");
  Serial.print(pin);
  Serial.print(" has been set to ");
  Serial.print(status);
  Serial.println();

  if (phase == 0) {
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
  } else if (phase == 1) {
    if (pin == answer) {
      success = true;
      phase = 2;
      digitalWrite(WIRE_SUCCESS, true);
    } else {
      strikes++;
    }
  }

  printState();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  digitalWrite(WIRE_SUCCESS, success);

  // activate all pins as input
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
