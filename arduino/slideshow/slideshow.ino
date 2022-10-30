#include <MaxMatrix.h>
#include "JoyStick.h"
#include "SketchLogic.h"

const int pin2Button = 2;  // The mode-toggle push button
const int pin3Button = 3;  // The save sketch push button

void refreshScreen(char sketch[8][8]) {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      if (sketch[r][c] == 1) {
        m.setDot(c, r, true);
      } else {
        m.setDot(c, r, false);
      }
    }
  }
}

void manage_display() {
  static int c = 4;
  static int r = 4;

  // blink cursor
  m.setDot(c, r, true);
  delay(50);
  m.setDot(c, r, false);
  delay(50);

  // user may have moved the cursor
  updateCursorPosition(c, r);

  // user may have saved a pixel
  if (joystick_button_is_released()) {
    if (sketch[r][c] == 0) {
      sketch[r][c] = 1;
    } else {
      sketch[r][c] = 0;
    }

    Serial.print("Saving pixels: ");
    Serial.print("R:");
    Serial.print(r);
    Serial.print(" C:");
    Serial.print(c);
    Serial.print("\n");
    printSketch(sketch);
  }

  // Maintain sketch on screen
  refreshScreen(sketch);
}

void scrollSprite(const byte *sprite) {
  m.clear();
  m.writeSprite(8, 0, sprite);
  for (int i = 0; i < 16; i++) {
    if (mode == 1) {
      m.shiftLeft(false, false);
      delay(readPot());
    }
  }
}

void scrollBuffer(char slideDeck[10][10]) {
  for (int n = 0; n < currDeckSize; n++) {
    scrollSprite(slideDeck[n]);
  }
}

void setup() {
  // Matrix
  m.init();            // MAX7219 initialization
  m.setIntensity(15);  // initial led matrix intensity, 0-15

  // JoyStick
  pinMode(SwitchButtonPin, INPUT);      // sets the SW switch as input
  digitalWrite(SwitchButtonPin, HIGH);  // sets the SW button HIGH
  Serial.begin(115200);                 // sets the baud rate

  // initialize the LED pin as an output:
  pinMode(modeLEDPin, OUTPUT);

  // Initialize the 2 push buttons to pin2 and pin3.
  // These 2 pins are the only interrupt ISR vector on UNO.
  pinMode(pin2Button, INPUT);
  pinMode(pin3Button, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin2Button), modeToggle, FALLING);
  attachInterrupt(digitalPinToInterrupt(pin3Button), saveSketch, FALLING);
}

void loop() {
  // mode 0 = sktech mode
  if (mode == 0) {
    manage_display();
  }

  // mode 1 = scroll mode
  else {
    scrollBuffer(slideDeck);
  }
}