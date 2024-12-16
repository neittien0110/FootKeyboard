/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
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

#include "ssd1306.h"



#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void main_screen(void) {
  display.clearDisplay();


  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F(" Foot "));

  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.println(F(" keyboard"));
  display.setCursor(75,12);             // Start at top-left corner
  display.println(F("//--\\\\"));

  display.setTextSize(1);             // Draw 2X-scale text

  display.print(F("Rua chan roi su dung"));

  display.display();

  /*
  void show_title(const char * title) {
  //Hiển thị trên oled
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(title);  //10 kí tự
  display.setTextSize(2);
  display.setCursor(0, ROW_OF_INFOR);  
  display.display();
  */
}

void setup_oled() {
  Wire.begin(PIN_SDA,PIN_SCL);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  // Xoay 180*
  //display.setRotation(0);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delayMicroseconds(200*1000);

  main_screen();    // Draw 'stylized' characters

  delayMicroseconds(200*1000);
}


void show_pedal(unsigned char no) {
  display.drawChar(95,0, (no+0x30), SSD1306_WHITE, SSD1306_BLACK,3);
  display.display();
}