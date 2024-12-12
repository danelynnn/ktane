#define WIRE_SUCCESS 2
#define COLOR_ROOT 3
#define BUTTON_ROOT 8

enum Color {
  YELLOW, GREEN, BLUE, RED
};

int phase = 2;
int colors[] = {YELLOW, GREEN, GREEN, RED};
const int lenColors = 4;
int strikes = 0;
bool success = false;

long start;

// hardcoding things, this will eventually be randomised/sent from elsewhere
char serial[] = "DL5QF2";
char *strings[] = {"Y", "G", "B", "R"};

void printArray(int arr[], int len) {
  for (int i=0; i<len; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}
void printArray(Color arr[], int len) {
  for (int i=0; i<len; i++) {
    Serial.print(strings[arr[i]]);
    Serial.print(" ");
  }
}

Color currentColor;
void glowColor(Color color) {
  if (currentColor != color) {
    // Serial.print("glowing ");
    // Serial.println(strings[color]);
    for (int c=YELLOW; c<=RED; c++) {
      if (c == color) digitalWrite(COLOR_ROOT + c, 1);
      else digitalWrite(COLOR_ROOT + c, 0);
    }

    currentColor = color;
  }
}

bool hasSerialVowel() {
  for (int i=0; i<6; i++) {
    if (serial[i] == 'A' || serial[i] == 'E' || serial[i] == 'I' || serial[i] == 'O' || serial[i] == 'U')
      return true;
  }

  return false;
}
const Color vowel[][4] = {
  {GREEN, YELLOW, RED, BLUE},
  {RED, BLUE, GREEN, YELLOW},
  {BLUE, YELLOW, RED, GREEN}
};
const Color nowel[][4] = {
  {RED, GREEN, YELLOW, BLUE},
  {GREEN, YELLOW, BLUE, RED},
  {RED, BLUE, GREEN, YELLOW}
};
Color answer[lenColors];
void handleRules() {
  if (hasSerialVowel()) {
    for (int i=0; i<lenColors; i++)
      answer[i] = vowel[strikes][colors[i]];
  } else {
    for (int i=0; i<lenColors; i++)
      answer[i] = nowel[strikes][colors[i]];
  }

  return answer;
}

Color currentAnswer[lenColors];
int lenCurrentAnswer = 0;
void onPress(Color button) {
  Serial.print("pressed ");
  Serial.println(button);

  if (button == answer[lenCurrentAnswer]) {
    currentAnswer[lenCurrentAnswer] = button;
    lenCurrentAnswer++;
  } else {
    strikes++;
    lenCurrentAnswer = 0;
    handleRules();
  }

  if (lenCurrentAnswer == phase+1) {
    lenCurrentAnswer = 0;
    if (phase+1 == lenColors) {
      success = true;
      digitalWrite(WIRE_SUCCESS, true);
    } else
      phase++;
  }

  printState();
}

void printState() {
  Serial.print("current string: ");
  printArray(colors, phase+1);
  Serial.print(", strikes = ");
  Serial.print(strikes);
  Serial.print(", correct answer is ");
  printArray(answer, phase+1);
  Serial.print(", current answer is ");
  printArray(currentAnswer, lenCurrentAnswer);
  Serial.print(", module status is ");

  if (success) {
    Serial.println("defused");
  } else {
    Serial.println("pending");
  }
}

void setup() {
  Serial.begin(9600);
  start = millis();
  handleRules();

  delay(1000);
  printState();

  pinMode(WIRE_SUCCESS, OUTPUT);
  for (int c=YELLOW; c<=RED; c++) {
    pinMode(COLOR_ROOT + c, OUTPUT);
  }
  for (int c=YELLOW; c<=RED; c++) {
    pinMode(BUTTON_ROOT + c, INPUT);
  }
}

int readout[] = {0, 0, 0, 0};
int debounce[] = {0, 0, 0, 0};
int state[] = {0, 0, 0, 0};
void loop() {
  long time = (millis() - start) % (1500 * (phase + 2));
  
  // christmas lights
  // switch ((time / 1000) % 4) {
  //   case 0:
  //     glowColor(YELLOW);
  //     break;
  //   case 1:
  //     glowColor(GREEN);
  //     break;
  //   case 2:
  //     glowColor(BLUE);
  //     break;
  //   case 3:
  //     glowColor(RED);
  //     break;
  // }

  for (int c=YELLOW; c<=RED; c++)
    readout[c] = digitalRead(BUTTON_ROOT+c);
  bool pressed = false;
  for (int c=YELLOW; c<=RED; c++) {
    if (readout[c] == 1) {
      glowColor(c);
      pressed = true;
      start = millis();

      debounce[c]++;
      if (debounce[c] == 1000) {
        if (state[c] == 0) {
          state[c] = 1;
          onPress(c);
        }

        debounce[c] = 0;
      } 
    } else {
      state[c] = 0;
      debounce[c] = 0;
    }
  }

  if (!pressed) {
    // say colors is [red, yellow, yellow]
    // cut time into 1000ms long segments
    // assign first 1000ms segment to nothing, 2nd segment to red, 3rd segment to yellow, 4rd segment to yellow
    // if within first half of segment, shine associated color
    // if within the rest, shine nothing
    int segment = (time / 1500) % (phase+2);
    if (segment == 0) {
      // Serial.println("holla");
    }
    if (segment == 0)
      glowColor(-1);
    for (int i=0; i<=phase; i++) {
      if (segment == i+1) {
        if (time % 1500 < 700)
          glowColor(colors[i]);
        else
          glowColor(-1);
      }
    }
  }
}