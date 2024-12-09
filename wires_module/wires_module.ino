#define RPin 2  // Read pin

enum Color {
  NONE,
  BLUE,
  RED,
  YELLOW,
  PURPLE,
  WHITE
};

int wireState = 0;
int states[] = {0, 0, 0, 0, 0, 0};
int phase = 0; // 0 = setup, 1 = execution

// hardcoding things, this will eventually be randomised/sent from elsewhere
Color colors[] = {YELLOW, BLUE, WHITE, NONE, RED, NONE};
char serial[] = "AL5QF2";

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

int handleRules() {
  Color connectedWires[6]; // list of connected *indices*
  int numWires = 0;
  for (int i=0; i<6; i++) {
    if (colors[i] != NONE) {
      connectedWires[numWires++] = i;
    }
  }

  if (wireCount(RED) == 0) {
    return connectedWires[1];
  } else if (colors[connectedWires[numWires-1]] == WHITE) {
    return connectedWires[numWires-1];
  } else if (wireCount(BLUE) > 1) {
    for (int i=numWires-1; i >= 0; i--) {
      if (colors[connectedWires[i]] == BLUE) {
        return connectedWires[i];
      }
    }
  } else {
    return connectedWires[numWires-1];
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
      }
      Serial.println();
    }
    if (setupSuccessful) {
      phase = 1;
    }
  } else if (phase == 1) {
    // Serial.println("onto execution phase");
    for (int i=0; i<6; i++) {
      if (states[i] == HIGH && states[i] != digitalRead(RPin + i)) {
        Serial.println("found a cut");
        Serial.println(i);
        states[i] = LOW;
      }
    }
  }
}
