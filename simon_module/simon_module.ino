#define WIRE_SUCCESS 2
#define COLOR_ROOT 3
#define BUTTON_ROOT 8

enum ColorOffset {
  YELLOW, GREEN, BLUE, RED
};

int phase = 0;
int colors[] = {YELLOW, GREEN, GREEN, RED};
int strikes = 0;
bool success = false;

// hardcoding things, this will eventually be randomised/sent from elsewhere
char serial[] = "AL5QF2";

void glowColor(ColorOffset color) {
  for (int c=YELLOW; c<=RED; c++) {
    if (c == color) digitalWrite(COLOR_ROOT + c, 1);
    else digitalWrite(COLOR_ROOT + c, 0);
  }
}

void onPress(ColorOffset button) {
  Serial.print("pressed ");
  Serial.println(button);

  glowColor(3-button);
}

int start;
void setup() {
  Serial.begin(9600);
  start = millis();

  pinMode(WIRE_SUCCESS, OUTPUT);
  for (int c=YELLOW; c<=RED; c++) {
    pinMode(COLOR_ROOT + c, OUTPUT);
  }
  for (int c=YELLOW; c<=RED; c++) {
    pinMode(BUTTON_ROOT + c, INPUT);
  }
}

void printArray(int arr[], int len) {
  for (int i=0; i<len; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}

int readout[] = {0, 0, 0, 0};
int debounce[] = {0, 0, 0, 0};
int state[] = {0, 0, 0, 0};
void loop() {
  int time = millis() - start;
  
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

  // say colors is [red, yellow, yellow]
  // cut time into 1000ms long segments
  // assign first 1000ms segment to red, 2nd segment to yellow, 3rd segment to yellow, last segment to nothing
  // if within first 500ms of segment, shine associated color
  // if within the rest, shine nothing
  for (int i=0; i<=phase; i++) {
    if (((time / 1000) % (phase+2)) == i) {
      Serial.println(i);
      if (time % 1000 < 500)
        glowColor(colors[i]);
      else
        glowColor(-1);
    }
  }
  if ((time / 1000) % (phase+2) == (phase+1)) {
    glowColor(-1);
  }

  for (int c=YELLOW; c<=RED; c++)
    readout[c] = digitalRead(BUTTON_ROOT+c);
  
  for (int c=YELLOW; c<=RED; c++) {
    if (readout[c] == 1) {
      debounce[c]++;
      if (debounce[c] == 5000) {
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
}