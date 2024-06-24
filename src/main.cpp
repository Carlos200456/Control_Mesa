#include <Arduino.h>
#include <U8g2lib.h>
#include "Functions.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// #define OLED          // Con Display OLED

#ifdef OLED
// OLED 0.96"
// U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
// OLED 1.3"                R0 = Rotate 0°,  R2 = Rotate 180°
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);
// Full Graphic Smart Controller
// U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ A5, /* data=*/ A4, /* CS=*/ 10, /* reset=*/ U8X8_PIN_NONE);
#endif

// Define the Output pins
#define G1 8  // Motor Direction Up
#define G2 9  // Motor Direction Down
#define G3 10 // Motor Speed
#define TR 13 // Tube Ready
bool MoveEnabled = false;

void setup() {
  // Set the pin as an input
  pinMode( 2, INPUT_PULLUP);  // Arduino Pin 2 = SW Limit Up.
  pinMode( 3, INPUT_PULLUP);  // Arduino Pin 3 = SW Pre Limit Up
  pinMode( 4, INPUT_PULLUP);  // Arduino Pin 4 = SW Pre Limit Down
  pinMode( 5, INPUT_PULLUP);  // Arduino Pin 5 = SW Limit Down
  pinMode( 6, INPUT_PULLUP);  // Arduino Pin 6 = SW Collision / Emergency
  pinMode( 7, INPUT_PULLUP);  // Arduino Pin 7 = SW Tube Position
  
  // Set the Gx pin at LOW state
  digitalWrite(G1,LOW);
  digitalWrite(G2,LOW);
  digitalWrite(G3,LOW);
  digitalWrite(TR,LOW);

  // Set the pin as an output
  pinMode(G1, OUTPUT);
  pinMode(G2, OUTPUT);
  pinMode(G3, OUTPUT);
  pinMode(TR, OUTPUT);  

  #ifdef OLED
  u8g2.begin();  // initialize with the I2C
  u8g2.setPowerSave(0);
  // init done
  // Set a small footprint font
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 10);
  u8g2.print("Pimax S.R.L.");
  u8g2.setCursor(0,20);             // Column, Row
  u8g2.print("Table Control");      // SOFTWARE NAME ---------------------------<<<<<<<<<<<<<<<<
  u8g2.setCursor(0,30);             // Column, Row
  u8g2.print("Version 1.Git");      // SOFTWARE VERSION ---------------------------<<<<<<<<<<<<<<<<
  u8g2.sendBuffer();
  delay(2000);
  u8g2.clearDisplay();
  #endif
}

void loop() {
  if (digitalRead(6) == HIGH) { // Emergency Stop
    MoveEnabled = false;
    Stop();
  } else {
    MoveEnabled = true;
  }

  if ((digitalRead(7) == HIGH) && MoveEnabled) { // Tube to Position Up
    if (digitalRead(2) == HIGH) {
      Stop();
    }
    if (digitalRead(3) == HIGH) {
      MoveUp(128);
    } else {
      MoveUp(255);
    } 
  }

  if ((digitalRead(7) == LOW) && MoveEnabled) { // Tube to Position Down
    if (digitalRead(5) == HIGH) {
      Stop();
    }
    if (digitalRead(4) == HIGH) {
      MoveDown(128);
    } else {
      MoveDown(255);
    }
  }
}

// Functions Declarations
void MoveUp(int speed) {
  digitalWrite(G1, HIGH);
  digitalWrite(G2, LOW);
  analogWrite(G3, speed);
} 

void MoveDown(int speed) {
  digitalWrite(G1, LOW);
  digitalWrite(G2, HIGH);
  analogWrite(G3, speed);
}

void Stop(void) {
  digitalWrite(G1, LOW);
  digitalWrite(G2, LOW);
  analogWrite(G3, 0);
  if ((digitalRead(2) && digitalRead(3)) || (digitalRead(4) && digitalRead(5))){
    digitalWrite(TR, HIGH);
  } else {
    digitalWrite(TR, LOW);
  }
}
