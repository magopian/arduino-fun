#include "LEDMatrix.h"

#define WIDTH   32
#define HEIGHT  16

LEDMatrix matrix(4, 5, 6, 7, 8, 9, 12, 13, 10, 11);     // LEDMatrix(a, b, c, d, oe, r1, g1, b1, stb, clk);
uint8_t displaybuf[WIDTH * HEIGHT];                     // Display Buffer
uint16_t index;
uint8_t temp;
uint32_t now;
uint32_t last_scrolled = 0;

void setup() {
  Serial.begin(9600);
  matrix.begin(displaybuf, WIDTH, HEIGHT);

  randomSeed(analogRead(0));  // pin 0 is unconnected, so random noise should be a good seed.
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      matrix.drawPoint(x, y, random(0, 8));
    }
  }
}

void loop() {
  matrix.scan();
  now = millis();
  
  if ((now - last_scrolled) < 100) return;
  
  last_scrolled = now;
  
  for (int y = 0; y < HEIGHT; y++) {
    temp = displaybuf[y * WIDTH]; // Remember the pixel from the first col of each row.
    for (int x = 0; x < WIDTH - 1; x++) { // Move each pixel "left" but the last one.
      index = x + y * WIDTH;
      displaybuf[index] = displaybuf[index + 1];
    }
    displaybuf[WIDTH - 1 + y * WIDTH] = temp; // Move the first pixel to the last col.
  }
}
