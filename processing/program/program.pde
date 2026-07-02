/**
 * This is a recreation of KT&NE's first couple of modules in Processing.
 * @author Allie
 */

import java.util.Collections;
import java.util.Iterator;
import java.util.Date;
import java.util.Map;
import java.io.File;

HashMap<String, PImage> imageMap;

// GAME VARIABLES //
final float minutes = 3;
final int maxStrikes = 3;

// INITIALIZATION //
final String[][] keypadColumns = new String[][] {
  new String[]{"28-balloon", "13-at", "30-upsidedowny", "12-squigglyn", "7-squidknife", "9-hookn", "23-leftc"},
  new String[]{"16-euro", "28-balloon", "23-leftc", "26-cursive", "3-hollowstar", "9-hookn", "20-questionmark"},
  new String[]{"1-copyright", "8-pumpkin", "26-cursive", "5-doublek", "15-meltedthree", "30-upsidedowny", "3-hollowstar"},
  new String[]{"11-six", "21-paragraph", "31-bt", "7-squidknife", "5-doublek", "20-questionmark", "4-smileyface"},
  new String[]{"24-pitchfork", "4-smileyface", "31-bt", "22-rightc", "21-paragraph", "19-dragon", "2-filledstar"},
  new String[]{"11-six", "16-euro", "27-tracks", "14-ae", "24-pitchfork", "18-nwithhat", "6-omega"}
};

final String[] indicatorValues = new String[]{"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
final String[] portValues = new String[]{"DVI", "PS2", "Parallel", "RJ45", "Serial", "StereoRCA"};

int startingTime = (int)(minutes*60*100);
int timer = 0;
int strikes = 0;
Date start;
Module[] modules = new Module[11];
Batteries batteries;
StringList ports = new StringList();
HashMap<String, Boolean> indicators = new HashMap();

// HELPER FUNCTIONS //
IntList subtract(IntList alpha, IntList beta) {
  final IntList result = new IntList(alpha.array());
  for (int i : beta) {
    result.removeValue(i);
  }
  return result;
}

String stringifyTimer(int timer) {
  if (timer >= 60 * 100) {
    int seconds = timer / 100;
    return String.format("%02d:%02d", seconds / 60, seconds % 60);
  } else if (timer >= 0) {
    return String.format("%02d:%02d", timer / 100, timer % 100);
  } else {
    return "00:00";
  }
}

// OO SECTION //
class Batteries {
  int AA, D;
  
  Batteries() {
    this.AA = (int)random(4);
    this.D = (int)random(4);
  }
  int getBatteries() {
    return AA * 2 + D;
  }
}

interface Drawable {
  void pressed(int x, int y);
  void draw();
}

abstract class Module implements Drawable {
  boolean success;
  float x, y;
  
  private Module(int x, int y) {
    this.x = x;
    this.y = y;
    success = false;
  }
  
  void pressed(int x, int y) {}
  void released() {}
  void draw() {
    noFill();
    rect(this.x-10, this.y-10, 250, 200);
    
    if (success) {
      fill(0, 250, 0);
    } else {
      noFill();
    }
    
    circle(this.x + 220, this.y + 5, 20);
  }
}

class Keypad extends Module {
  int currentColumn;
  int[] characters;
  IntList charList;
  IntList currentPressed = new IntList();
  
  public Keypad(int x, int y) {
    super(x, y);
    
    this.currentColumn = (int)random(5);
    this.charList = new IntList();
    
    for (int i=0; i<4; i++) {
      int randomOffset;
      do {
        randomOffset = (int)random(7);
      } while (this.charList.hasValue(randomOffset));
      this.charList.append(randomOffset);
    }
    
    this.charList.shuffle();
  }
  
  private void press(int i) {
    try {
      if (i == subtract(charList, currentPressed).min()) {
        currentPressed.append(i);
        if (currentPressed.size() == 4) success = true;
      } else if (!currentPressed.hasValue(i)) {
        strikes++;
      }
    } catch (Exception e) {
    }
      
      
  }
  
  void pressed(int x, int y) {
    if (x > this.x && x < this.x + 50 &&
        y > this.y && y < this.y + 50) {
      press(this.charList.get(0));
    } else if (x > this.x + 50 && x < this.x + 100 &&
               y > this.y && y < this.y + 50) {
      press(this.charList.get(1));
    } else if (x > this.x && x < this.x + 50 &&
               y > this.y + 50 && y < this.y + 100) {
      press(this.charList.get(2));
    } else if (x > this.x + 50 && x < this.x + 100 &&
               y > this.y + 50 && y < this.y + 100) {
      press(this.charList.get(3));
    }
  }
  
  void draw() {
    super.draw();
    
    if (currentPressed.hasValue(charList.get(0)))
      fill(0, 100, 0);
    else
      fill(200, 200, 200);
    rect(this.x, this.y, 50, 50);
    image(imageMap.get(keypadColumns[currentColumn][charList.get(0)]), this.x, this.y, 50, 50);
    
    if (currentPressed.hasValue(charList.get(1)))
      fill(0, 100, 0);
    else
      fill(200, 200, 200);
    rect(this.x+50, this.y, 50, 50);
    image(imageMap.get(keypadColumns[currentColumn][charList.get(1)]), this.x+50, this.y, 50, 50);
    
    if (currentPressed.hasValue(charList.get(2)))
      fill(0, 100, 0);
    else
      fill(200, 200, 200);
    rect(this.x, this.y+50, 50, 50);
    image(imageMap.get(keypadColumns[currentColumn][charList.get(2)]), this.x, this.y+50, 50, 50);
    
    if (currentPressed.hasValue(charList.get(3)))
      fill(0, 100, 0);
    else
      fill(200, 200, 200);
    rect(this.x+50, this.y+50, 50, 50);
    image(imageMap.get(keypadColumns[currentColumn][charList.get(3)]), this.x+50, this.y+50, 50, 50);
  }
}

enum Color {
  BLUE(0), RED(1), WHITE(2), YELLOW(3), BLACK(4);
  
  int value;
  private static HashMap map = new HashMap();
  private Color(int value) {
    this.value = value;
  }
  static {
    for (Color pageType : Color.values()) {
      map.put(pageType.value, pageType);
    }
  }
  
  static Color valueOf(int value) {
    return (Color)map.get(value);
  }
  
  int getValue() {
    return value;
  }
  
};
enum Label {
  ABORT(0, "Abort"), DETONATE(1, "Detonate"), HOLD(2, "Hold"), PRESS(3, "Press");
  
  int value;
  String text;
  private static HashMap map = new HashMap();
  private Label(int value, String text) {
    this.value = value;
    this.text = text;
  }
  static {
    for (Label pageType : Label.values()) {
      map.put(pageType.value, pageType);
    }
  }
  
  static Label valueOf(int value) {
    return (Label)map.get(value);
  }
  int getValue() {
    return value;
  }
  //String toString() {
  //  return text;
  //}
};
class Button extends Module {
  Color colour;
  Color strip;
  Label label;
  
  boolean pressed;
  int pressDuration;
  
  Button(int x, int y) {
    super(x, y);
    
    this.colour = Color.valueOf((int)random(5));
    this.strip = Color.valueOf((int)random(4));
    this.label = Label.valueOf((int)random(4));
  }
  
  void pressed(int x, int y) {
    if (dist(mouseX, mouseY, this.x+75, this.y+75) < 75) {
      this.pressed = true;
    }
  }
  
  void released() {
    if (pressed) {
      if (pressDuration < 20) {
        test(true);
      } else {
        test(false);
      }
    }
    pressed = false;
    pressDuration = 0;
  }
  
  void test(boolean tapped) {
    boolean hold;
    if (colour == Color.BLUE && label == Label.ABORT) {
      hold = true;
    } else if (batteries.getBatteries() > 1 && label == Label.DETONATE) {
      hold = false;
    } else if (colour == Color.WHITE && indicators.containsKey("CAR") && indicators.get("CAR")) {
      hold = true;
    } else if (batteries.getBatteries() > 2 && indicators.containsKey("FRK") && indicators.get("FRK")) {
      hold = false;
    } else if (colour == Color.YELLOW) {
      hold = true;
    } else if (colour == Color.RED && label == Label.HOLD) {
      hold = false;
    } else {
      hold = true;
    }
    
    if (hold) {
      if (tapped) {
        strikes++;
      } else {
        String timerString = stringifyTimer(timer);
        if (colour == Color.BLUE && timerString.contains("4")) {
          success = true;
        } else if (colour == Color.WHITE && timerString.contains("5")) {
          success = true;
        } else if (timerString.contains("1")) {
          success = true;
        } else {
          strikes++;
        }
      }
    } else {
      if (tapped) {
        success = true;
      } else {
        strikes++;
      }
    }
  }
  
  void draw() {
    super.draw();
    
    if (this.pressed) {
      pressDuration += 1;
    }
    
    switch (this.colour) {
      case BLUE:
        fill(0, 0, 230);
        break;
      case RED:
        fill(230, 0, 0);
        break;
      case WHITE:
        fill(230, 230, 230);
        break;
      case YELLOW:
        fill(230, 230, 0);
        break;
      case BLACK:
        fill(30, 30, 30);
        break;
    }
    circle(this.x+75, this.y+75, 150);
    
    if (this.colour == Color.WHITE || this.colour == Color.YELLOW) {
      fill(0, 0, 0);
    } else {
      fill(255, 255, 255);
    }
    textAlign(CENTER, CENTER);
    textSize(24);
    text(this.label.toString(), this.x+75, this.y+75);
    
    if (this.pressDuration >= 20) {
      switch (this.strip) {
        case BLUE:
          fill(0, 0, 230);
          break;
        case RED:
          fill(230, 0, 0);
          break;
        case WHITE:
          fill(230, 230, 230);
          break;
        case YELLOW:
          fill(230, 230, 0);
          break;
        default:
          break;
      }
    } else
      noFill();
    rect(this.x+160, this.y+10, 30, 150);
  }
}

void setup() {
  start = new Date();
  File imageDir = new File(dataPath(""), "img");
  File[] images = imageDir.listFiles();
  println(imageDir);
  imageMap = new HashMap<String, PImage>();
  for (File file : images) {
    // this abomination taken from https://stackoverflow.com/questions/941272/how-do-i-trim-a-file-extension-from-a-string-in-java
    String[] split = file.getName().split("\\.");
    String ext = split[split.length - 1];
    String filename = file.getName().replace("." + ext, "");
    imageMap.put(filename, loadImage(String.format("img/%s.%s", filename, ext)));
  }
  
  size(1080, 720);
  
  batteries = new Batteries();
  
  for (int i=0; i<(int)random(4); i++) {
    indicators.put(indicatorValues[(int)random(10)], (random(1) > .5));
  }
  
  for (int i=0; i<(int)random(6); i++) {
    String port = portValues[(int)random(6)];
    
    if (!ports.hasValue(port)) {
      ports.append(port);
    }
  }
  
  modules[0] = new Keypad(10, 60);
  modules[1] = new Button(270, 60);
}

void draw() {
  clear();
  background(240, 240, 240);
  
  for (Module module : modules) {
    if (module != null)
      module.draw();
  }
  
  Date now = new Date();
  timer = startingTime - (int)(now.getTime() - start.getTime())/10;
  
  stroke(0);
  fill(0);
  textSize(12);
  text(strikes, 20, 30);
  text(stringifyTimer(timer), 100, 30);
  
  strokeWeight(1);
  for (int i=0, j=10; i<batteries.AA+batteries.D; i++) {
    if (i < batteries.AA) {
      image(imageMap.get("Battery-AA"), j, 400, 40, 80);
      image(imageMap.get("Battery-AA"), j+23, 400, 40, 80);
      j += 60;
    } else {
      image(imageMap.get("Battery-D"), j, 400, 40, 80);
      j += 50;
    }
  }
  
  int j = 10;
  for (Map.Entry<String, Boolean> e : indicators.entrySet()) {
    noFill();
    rect(j, 500, 185, 70);
    
    if (e.getValue()) {
      fill(230, 230, 0);
    } else {
      noFill();
    }
    circle(j+35, 535, 50);
    
    fill(0);
    rect(j+70, 510, 100, 50);
    fill(255);
    textSize(24);
    text(e.getKey(), j+120, 535);
    
    j += 200;
  }
  
  j = 10;
  for (String port : ports) {
    PImage img = imageMap.get(port);
    // intentional misspelling gotem
    int hight = 80;
    int with = (int)(((float)img.width/(float)img.height) * hight);
    
    image(img, j, 600, with, hight); //<>//
    
    j += with + 20;
  }
}

void mousePressed() {
  for (Module module : modules) {
    if (module != null)
      module.pressed(mouseX, mouseY);
  }
}

void mouseReleased() {
  for (Module module : modules) {
    if (module != null)
      module.released();
  }
}
