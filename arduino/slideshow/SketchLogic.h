#include "pins_arduino.h"
/*
 * 8x8 LED and Driver parameters
 */
int DIN = 5;  // DIN pin of MAX7219 module
int CLK = 4;  // CLK pin of MAX7219 module
int CS = 6;   // CS pin of MAX7219 module
int maxInUse = 1;
MaxMatrix m(DIN, CS, CLK, maxInUse);

// sketch parameters
int currDeckSize = 0;
int maxDeckSize = 9;
char slideDeck[10][10] = { 0 };
char sketch[8][8] = { 0 };
int mode = 0;
const int modeLEDPin = LED_BUILTIN;
static int save_count = 0;
static int toggle_count = 0;

char arrTo8BitsChar(char a[]) {
  // Example: a={1,0,0,0,1,0,0,0} -> c='1000100'
  char c = 0;
  for (int i = 0; i < 8; i++) {
    if (a[i] == 1) {
      c = c | (1 << 7 - i);
    }
  }
  return c;
}

void rotateSketchBy90(char sketch[8][8]) {
  char tmp[8][8] = { 0 };
  // construct rotateSketchBy90; first row is last column
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      tmp[j][7 - i] = sketch[i][j];
    }
  }

  // copy the rotateSketchBy90 back to input arr
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      sketch[i][j] = tmp[i][j];
    }
  }
}

bool sketchIsNotBlank(char sketch[8][8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (sketch[i][j] == 1) {
        return true;
      }
    }
  }
  return false;
}

void saveSketchToASlide(char sketch[8][8], char slideFrame[10]) {
  rotateSketchBy90(sketch);

  // first 2 rows are static dimensions 8x8
  slideFrame[0] = 8;
  slideFrame[1] = 8;

  // write each row to frame
  for (int i = 0; i < 8; i++) {
    slideFrame[i + 2] = arrTo8BitsChar(sketch[i]);
  }
}

void printSketch(char sketch[8][8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (sketch[i][j] == 1) {
        Serial.print(1);
      } else {
        Serial.print(0);
      }
      Serial.print(" ");
    }
    Serial.print("\n");
  }
}

void updateCursorPosition(int &c, int &r) {
  // Right
  if (movedRight()) {
    Serial.println("Right (+X)");
    if (c < 7) {
      c++;
    }
  }

  // Left
  if (movedLeft()) {
    Serial.println("Left (-X)");
    if (c > 0) {
      c--;
    }
  }

  // Up
  if (movedUp()) {
    Serial.println("Down (-Y)");
    if (r < 7) {
      r++;
    }
  }

  // Down
  if (movedDown()) {
    Serial.println("Up (+Y)");
    if (r > 0) {
      r--;
    }
  }
}

void modeToggle() {
  mode ^= 1;
  digitalWrite(modeLEDPin, mode);

  // stdout
  Serial.print("Mode: ");
  Serial.println(mode);
}

void saveSketch() {
  if (currDeckSize < maxDeckSize && sketchIsNotBlank(sketch)) {
    currDeckSize++;
    saveSketchToASlide(sketch, slideDeck[currDeckSize - 1]);

    // clear sketch screen
    memset(sketch, 0, sizeof(sketch));

    // stdout
    Serial.print("Saved sketch. ");
    Serial.print("Slide deck: ");
    Serial.println(currDeckSize);
  }
}