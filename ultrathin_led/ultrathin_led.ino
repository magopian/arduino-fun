#include "LEDMatrix.h"

// Comment/uncomment the board you want to flash onto.
//#define ARDUINO
#define NODEMCU
#define WIDTH   32
#define HEIGHT  16

/*
 * RGB 08 Port (on the LED matrix):
 * GND   A
 * GND   B
 * GND   C
 * OE    D
 * R1    G1
 * B1    N/A
 * GND   STB
 * GND   CLK
 */

#ifdef ARDUINO // for Arduino Uno
  #define PIN_A 4
  #define PIN_B 5
  #define PIN_C 6
  #define PIN_D 7
  #define PIN_OE 8
  #define PIN_R1 9
  #define PIN_G1 12
  #define PIN_B1 13
  #define PIN_STB 10
  #define PIN_CLK 11
#endif

#ifdef NODEMCU // for nodeMCU
  #include <pins_arduino.h>
  #define PIN_A D0
  #define PIN_B D1
  #define PIN_C D2
  #define PIN_D D3
  #define PIN_OE D4
  #define PIN_R1 D5
  #define PIN_G1 D6
  #define PIN_B1 D7
  #define PIN_STB D8
  #define PIN_CLK D9
#endif

LEDMatrix matrix(PIN_A, PIN_B, PIN_C, PIN_D, PIN_OE, PIN_R1, PIN_G1, PIN_B1, PIN_STB, PIN_CLK);
uint8_t displaybuf[WIDTH * HEIGHT]; // Display Buffer
uint16_t buf_index;
uint8_t temp;
uint32_t now;
uint32_t last_scrolled = 0;

void setup() {
  Serial.begin(115200);
  matrix.begin(displaybuf, WIDTH, HEIGHT);

  randomSeed(analogRead(0));  // pin 0 is unconnected, so random noise should be a good seed.
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      matrix.drawPoint(x, y, random(0, 8));
    }
  }
  Serial.println("Started!");
}

void loop() {
  matrix.scan();
  now = millis();
  
  if ((now - last_scrolled) < 100) return;
  
  last_scrolled = now;
  
  for (int y = 0; y < HEIGHT; y++) {
    temp = displaybuf[y * WIDTH]; // Remember the pixel from the first col of each row.
    for (int x = 0; x < WIDTH - 1; x++) { // Move each pixel "left" but the last one.
      buf_index = x + y * WIDTH;
      displaybuf[buf_index] = displaybuf[buf_index + 1];
    }
    displaybuf[WIDTH - 1 + y * WIDTH] = temp; // Move the first pixel to the last col.
  }
}
