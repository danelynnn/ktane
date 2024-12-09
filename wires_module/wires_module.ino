#define RPin 2  // Read pin

enum Color {
  NONE,
  BLUE,
  RED,
  YELLOW,
  PURPLE,
  WHITE,
  BLACK
};

int wireState = 0;
int states[] = {0, 0, 0, 0, 0, 0};
int phase = 0; // 0 = setup, 1 = execution

// hardcoding things, this will eventually be randomised/sent from elsewhere
Color colors[] = {BLUE, RED, BLUE, NONE, NONE, NONE};
char serial[] = "AL5QF3";

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(RPin, INPUT);      // Separate pin for Q3
}

int countWires(Color color) {
  int count = 0;
  for (int i=0; i<6; i++) {
    if (colors[i] == color) {
      count++;
    }
  }
  return count;
}

void printArray(Color colors[], int len) {
  for (int i=0; i<len; i++) {
    Serial.print(colors[i]);
    Serial.print(" ");
  }
  Serial.println();
}

int handleRules() {
  Color connectedWires[6]; // list of connected *indices*
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
void loop() {
  wireState = digitalRead(RPin);

  if (phase == 0 && millis() >= 100) {
    Serial.println("starting setup phase");
    bool setupSuccessful = true;
    for (int i=0; i<6; i++) {
      
      states[i] = digitalRead(RPin + i);

      if (((states[i] == HIGH) && (colors[i] == NONE)) || ((states[i] == LOW) && (colors[i] != NONE))) {
        Serial.print("failed setup on ");
        Serial.print(i);
        Serial.println();
        setupSuccessful = false;
        break;
      }

      if (states[i]) {
        Serial.print(i);
        Serial.print(" ");
        Serial.println();
      }
    }
    if (setupSuccessful) {
      phase = 1;
    }
  } else if (phase == 1) {
    // Serial.println("onto execution phase");
    Serial.println(handleRules());
    for (int i=0; i<6; i++) {
      if (states[i] == HIGH && states[i] != digitalRead(RPin + i)) {
        Serial.println("found a cut");
        Serial.println(i);
        states[i] = LOW;
      }
    }
  }
}
