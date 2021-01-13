/**************************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x64 pixel display using I2C to communicate
  3 pins are required to interface (two I2C and one reset).

  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source
  hardware by purchasing products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries,
  with contributions from the open source community.
  BSD license, check license.txt for more information
  All text above, and the splash screen below must be
  included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RevEng_PAJ7620.h>

// Create gesture sensor driver object
RevEng_PAJ7620 sensor = RevEng_PAJ7620();

#define INTERRUPT_PIN 2                     // Interrupt capable Arduino pin

volatile bool isr = false;                  // isr == Interrupt Service Routine

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};


enum ARROW_DIRECTION
{
  UP_ARROW, DOWN_ARROW, LEFT_ARROW, RIGHT_ARROW,  
};


// ** *****************************************************************************************************
void setup() {
  Serial.begin(115200);

  if( !sensor.begin() )           // return value of 0 == success
    {    Serial.print("PAJ7620 I2C error - halting");  while(true) { }  }
  Serial.println("PAJ7620 init: OK");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { Serial.println(F("SSD1306 allocation failed"));  for (;;); }
  Serial.println("OLED init: OK");

  display.clearDisplay();
  display.display();

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), interruptRoutine, FALLING);


  // Start of animation code
  int x = 100;
  int arrowDelay = 1;
  for( int y = display.height() - 30; y > -46; y -= 4 ) {
    display.clearDisplay();
    displayArrow(x, y, LEFT_ARROW);
    display.display();
    delay(arrowDelay);
  }


  
  display.drawCircle(display.width() / 2, display.height() / 2, 30, SSD1306_WHITE);
  display.display();
  delay(10000);

  Serial.println("READY.");
}


// ** *****************************************************************************************************
void loop() {
  if (isr == true)                  // See interruptRoutine for where this is set to true
  {
    isr = false;                    // Reset ISR flag for next interrupt
    handleGestureInterrupt();
  }
}


// ** ********************************************************************
void handleGestureInterrupt()
{
  Gesture new_gesture = sensor.readGesture(); // Read back current gesture (if any) of type Gesture

  switch (new_gesture)
  {
    case GES_FORWARD:           Serial.print(" GES_FORWARD");       break;
    case GES_BACKWARD:          Serial.print(" GES_BACKWARD");      break;
    case GES_LEFT:              Serial.print(" GES_LEFT");          break;
    case GES_RIGHT:             Serial.print(" GES_RIGHT");         break;
    case GES_UP:                Serial.print(" GES_UP");            break;
    case GES_DOWN:              Serial.print(" GES_DOWN");          break;
    case GES_CLOCKWISE:         Serial.print(" GES_CLOCKWISE");     break;
    case GES_ANTICLOCKWISE:     Serial.print(" GES_ANTICLOCKWISE"); break;
    case GES_WAVE:              Serial.print(" GES_WAVE");          break;
    case GES_NONE:              Serial.print(" GES_NONE");          break;
  }
  Serial.println("");
}


// ** Called then interrupt pin falls  *****************************************
void interruptRoutine() {  
  //Serial.print("Int");
  isr = true;
}

// ** Rendering a straight arrow code ************************************************
void displayArrow(int x, int y, ARROW_DIRECTION dir) {
  int arrowShaftWidth = 11;
  int arrowShaftLength = 30;
  int arrowHeadHeight = 6;
  int arrowHeadWidth = 20;

  if( dir == UP_ARROW ) {
    display.fillTriangle(
      x, y,
      x - arrowHeadWidth / 2, y + arrowHeadHeight,
      x + arrowHeadWidth / 2, y + arrowHeadHeight, SSD1306_WHITE);
    display.fillRect(x - arrowShaftWidth / 2, y + arrowHeadHeight + 1, arrowShaftWidth, arrowShaftLength, SSD1306_WHITE);
  }
  else if( dir == DOWN_ARROW ) {
    display.fillTriangle(
      x, y,
      x - arrowHeadWidth / 2, y - arrowHeadHeight,
      x + arrowHeadWidth / 2, y - arrowHeadHeight, SSD1306_WHITE);
    display.fillRect(x - arrowShaftWidth / 2, y - arrowHeadHeight - arrowShaftLength, arrowShaftWidth, arrowShaftLength, SSD1306_WHITE);
  }
  else if( dir == RIGHT_ARROW ) {
    display.fillTriangle(
      x, y,
      x - arrowHeadHeight, y - arrowHeadWidth / 2,
      x - arrowHeadHeight, y + arrowHeadWidth / 2, SSD1306_WHITE);
    display.fillRect(x - (arrowHeadHeight + arrowShaftLength), y - (arrowShaftWidth / 2), arrowShaftLength, arrowShaftWidth, SSD1306_WHITE);
  }
  else if( dir == LEFT_ARROW ) {
    display.fillTriangle(
      x, y,
      x + arrowHeadHeight, y - arrowHeadWidth / 2,
      x + arrowHeadHeight, y + arrowHeadWidth / 2, SSD1306_WHITE);
    display.fillRect(x + (arrowHeadHeight), y - (arrowShaftWidth / 2), arrowShaftLength, arrowShaftWidth, SSD1306_WHITE);
  }
}





//************************************************************************************

void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for (i = 0; i < display.height(); i += 4) {
    display.drawLine(0, 0, display.width() - 1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(0, display.height() - 1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for (i = display.width() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for (i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testdrawcircle(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) {
  display.clearDisplay();

  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}


void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}
