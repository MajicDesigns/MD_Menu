#include "Menu_Test.h"

// Routines for specific display hardware, defining
// - Appropriate header files
// - Hardware definitions
// - Global library objects
// - Display callback function 

#if DISPLAY_SERIAL
// Display the output to the Serial Monitor
// This is useful for debugging and troubleshooting the structure of the 
// menu definitions without using the final output device.

bool display(MD_Menu::userDisplayAction_t action, char *msg)
{
  switch (action)
  {
  case MD_Menu::DISP_INIT:
    Serial.begin(BAUD_RATE);
    break;
    
  case MD_Menu::DISP_CLEAR:
    Serial.print("\n-> CLS");
    break;

  case MD_Menu::DISP_L0:
    Serial.print("\n0> ");
    Serial.print(msg);
    break;

  case MD_Menu::DISP_L1:
    Serial.print("\n1> ");
    Serial.print(msg);
    break;
  }

  return(true);
}
#endif

#if DISPLAY_LCD_SHIELD
// Output display to a 2 line LCD display. 
// For a one line display, comment out the L0 handling code.
// The output display line is cleared with spaces before the
// requested message is shown.
#include <LiquidCrystal.h>

// LCD display definitions
#define  LCD_ROWS  2
#define  LCD_COLS  16

// LCD pin definitions
#define  LCD_RS    8
#define  LCD_ENA   9
#define  LCD_D4    4
#define  LCD_D5    LCD_D4+1
#define  LCD_D6    LCD_D4+2
#define  LCD_D7    LCD_D4+3

static LiquidCrystal lcd(LCD_RS, LCD_ENA, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

bool display(MD_Menu::userDisplayAction_t action, char *msg)
{
  static char szLine[LCD_COLS + 1] = { '\0' };

  switch (action)
  {
  case MD_Menu::DISP_INIT:
    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.clear();
    lcd.noCursor();
    memset(szLine, ' ', LCD_COLS);
    break;
  
  case MD_Menu::DISP_CLEAR:
    lcd.clear();
    break;

  case MD_Menu::DISP_L0:
    lcd.setCursor(0, 0);
    lcd.print(szLine);
    lcd.setCursor(0, 0);
    lcd.print(msg);
    break;

  case MD_Menu::DISP_L1:
    lcd.setCursor(0, 1);
    lcd.print(szLine);
    lcd.setCursor(0, 1);
    lcd.print(msg);
    break;
  }

  return(true);
}
#endif

#if DISPLAY_LCD_I2C
// Output display to a of 2 line LCD display using I2C connections. 
// For a one line display, comment out the L0 handling code.
// The output display line is cleared with spaces before the
// requested message is shown.
//
// HD44780 librarty is available from the IDE Library Manager

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

// LCD display definitions
#define  LCD_ROWS  2
#define  LCD_COLS  16

// LCD pin definitions
static hd44780_I2Cexp lcd;

bool display(MD_Menu::userDisplayAction_t action, char* msg)
{
  bool b = true;
  static char szLine[LCD_COLS + 1] = { '\0' };

  switch (action)
  {
  case MD_Menu::DISP_INIT:
    b = (lcd.begin(LCD_COLS, LCD_ROWS) == hd44780::RV_ENOERR);
    if (!b) 
      Serial.print(F("\n!!! LCD initialization failed"));
    else
    {
      lcd.clear();
      lcd.noCursor();
    }
    memset(szLine, ' ', LCD_COLS);
    break;

  case MD_Menu::DISP_CLEAR:
    lcd.clear();
    break;

  case MD_Menu::DISP_L0:
    lcd.setCursor(0, 0);
    lcd.print(szLine);
    lcd.setCursor(0, 0);
    lcd.print(msg);
    break;

  case MD_Menu::DISP_L1:
    lcd.setCursor(0, 1);
    lcd.print(szLine);
    lcd.setCursor(0, 1);
    lcd.print(msg);
    break;
  }

  return(b);
}
#endif

#if DISPLAY_OLED_I2C
// Simple output to a one line LED Matrix display managed 
// by the Parola library.

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH    128   // OLED display width, in pixels
#define OLED_HEIGHT   64    // OLED display height, in pixels
#define OLED_I2C_ADDR 0x3c  // OLED I2C Address
#define OLED_RESET    -1    // OLED Reset pin # (-1 if not used or sharing Arduino reset pin)

#define FONT_SCALE 1    // OLED font scaling
#define LINE_HEIGHT (8 * FONT_SCALE)  // Height of text line
#define LINE1_X  0      // Line 1 coordinates for start of text
#define LINE1_Y  0
#define LINE2_X  0      // Line 2 coordinates for start of text
#define LINE2_Y  (LINE_HEIGHT + 2)

// Declaration a SSD1306 display connected using I2C
static Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

bool display(MD_Menu::userDisplayAction_t action, char *msg)
{
  bool b = true;

  switch (action)
  {
  case MD_Menu::DISP_INIT:
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    b = oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR);
    if (!b)
      Serial.println(F("\n!!! OLED initialize failed"));
    else
    {
      oled.clearDisplay();
      oled.setTextSize(FONT_SCALE);
      oled.setTextColor(SSD1306_WHITE);
      oled.display();
    }
    break;

  case MD_Menu::DISP_CLEAR:
    oled.clearDisplay();
    oled.display();
    break;

  case MD_Menu::DISP_L0:
    // clear the line, put cursor in position and print the message
    oled.fillRect(LINE1_X, LINE1_Y, OLED_WIDTH-LINE1_X, LINE_HEIGHT, SSD1306_BLACK);
    oled.setCursor(LINE1_X, LINE1_Y);
    oled.print(msg);
    oled.display();
    break;

  case MD_Menu::DISP_L1:
    // clear the line, put cursor in position and print the message
    oled.fillRect(LINE2_X, LINE2_Y, OLED_WIDTH - LINE2_X, LINE_HEIGHT, SSD1306_BLACK);
    oled.setCursor(LINE2_X, LINE2_Y);
    oled.print(msg);
    oled.display();
    break;
  }

  return(b);
}
#endif

#if DISPLAY_PAROLA
// Simple output to a one line LED Matrix display managed 
// by the Parola library.

#include <MD_MAX72XX.h>
#include <MD_Parola.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define MAX_DEVICES 8
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Hardware SPI connection
static MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

bool display(MD_Menu::userDisplayAction_t action, char* msg)
{
  switch (action)
  {
  case MD_Menu::DISP_INIT:
    P.begin();
    break;

  case MD_Menu::DISP_CLEAR:
    P.displayClear();
    break;

  case MD_Menu::DISP_L0:
    // P.print(msg);    // only one zone, no line 0
    break;

  case MD_Menu::DISP_L1:
    P.print(msg);
    break;
  }

  return(true);
}
#endif

