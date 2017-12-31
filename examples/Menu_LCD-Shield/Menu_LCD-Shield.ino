// Example program for the MD_Menu library
//
// Run the menu on a LCD 2 line dispay to demonstrate how a menu is structured.
// The code runs the menu and display debugging information on the Serial Monitor.
// 
// The user input interface is the tact switches set up as analog and 'resistor
// ladder' common on LCD shields for INC, DEC, ESC and SEL.
//
// External Dependencies
// ---------------------
// - LiquidCrystal library for LCD module is a standard Arduino library
// - MD_UISwitch library for digital and analog switches available at 
//   https://github.com/MajicDesigns/MD_UISwitch.
//
#include <LiquidCrystal.h>
#include <MD_UISwitch.h>
#include <MD_Menu.h>

// LCD Display -----------------------------
// LCD display definitions
#define  LCD_ROWS  2
#define  LCD_COLS  16

// LCD pin definitions - change to suit your shield
#define  LCD_RS    8
#define  LCD_ENA   9
#define  LCD_D4    4
#define  LCD_D5    LCD_D4+1
#define  LCD_D6    LCD_D4+2
#define  LCD_D7    LCD_D4+3

static LiquidCrystal lcd(LCD_RS, LCD_ENA, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// LCD Shield keypad -----------------------

#define LCD_KEYS_PIN A0

// These key values work for most LCD shields
MD_UISwitch_Analog::uiAnalogKeys_t keyTable[] =
{
  {  10, 10, 'R' },  // Right
  { 140, 15, 'U' },  // Up
  { 315, 15, 'D' },  // Down
  { 490, 15, 'L' },  // Left
  { 725, 15, 'S' },  // Select
};

MD_UISwitch_Analog lcdKeys(LCD_KEYS_PIN, keyTable, ARRAY_SIZE(keyTable));

// Menu Setup -----------------------------
const bool AUTO_START = true; // auto start the menu, manual detect and start if false

const uint32_t BAUD_RATE = 57600;   // Serial Monitor speed setting 
const uint16_t MENU_TIMEOUT = 5000; // in milliseconds

const uint8_t LED_PIN = LED_BUILTIN;  // for myLEDCode function

// function prototypes for user nav and display callback
bool display(MD_Menu::userDisplayAction_t action, char *msg = nullptr);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);

// Function prototypes for variable get/set functions
MD_Menu::value_t *mnuLValueRqst(MD_Menu::mnuId_t id, bool bGet);
MD_Menu::value_t *mnuBValueRqst(MD_Menu::mnuId_t id, bool bGet);
MD_Menu::value_t *mnuIValueRqst(MD_Menu::mnuId_t id, bool bGet);
MD_Menu::value_t *mnuFValueRqst(MD_Menu::mnuId_t id, bool bGet);
MD_Menu::value_t *mnuEValueRqst(MD_Menu::mnuId_t id, bool bGet);
MD_Menu::value_t *myCode(MD_Menu::mnuId_t id, bool bGet);
MD_Menu::value_t *myLEDCode(MD_Menu::mnuId_t id, bool bGet);

// Global menu data and definitions
uint8_t fruit = 2;
bool bValue = true;
int8_t  int8Value = 99;
int16_t int16Value = 999;
int32_t int32Value = 9999;
float floatValue = 999.99;
MD_Menu::value_t engValue = { 999900, 3 };

MD_Menu::value_t vBuf;  // interface buffer for values

// Menu Headers --------
const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { 10, "MD_Menu", 10, 12, 0 },
  { 11, "Input Data", 20, 27, 0 },
  { 12, "LED Menu", 40, 41, 0 },
};

// Menu Items ----------
const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  // Starting (Root) menu
  { 10, "Input Test", MD_Menu::MNU_MENU, 11 },
  { 12, "LED", MD_Menu::MNU_MENU, 12 },
  // Input Data submenu
  { 20, "Fruit List", MD_Menu::MNU_INPUT, 10 },
  { 21, "Boolean", MD_Menu::MNU_INPUT, 11 },
  { 22, "Integer 8", MD_Menu::MNU_INPUT, 12 },
  { 23, "Integer 16", MD_Menu::MNU_INPUT, 13 },
  { 24, "Integer 32", MD_Menu::MNU_INPUT, 14 },
  { 25, "Hex 16", MD_Menu::MNU_INPUT, 15 },
  { 26, "Float", MD_Menu::MNU_INPUT, 16 },
  { 27, "Eng Unit", MD_Menu::MNU_INPUT, 17 },
  { 28, "Reset Menu", MD_Menu::MNU_INPUT, 18 },
  // LED  
  { 40, "Turn Off", MD_Menu::MNU_INPUT, 40 },
  { 41, "Turn On", MD_Menu::MNU_INPUT, 41 },
};

// Input Items ---------
const PROGMEM char listFruit[] = "Apple|Pear|Orange|Banana|Pineapple|Peach";
const PROGMEM char engUnit[] = "Hz";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { 10, "List", MD_Menu::INP_LIST, mnuLValueRqst, 6, 0, 0, 0, 0, 0, listFruit }, // shorter and longer list labels
  { 11, "Bool", MD_Menu::INP_BOOL, mnuBValueRqst, 1, 0, 0, 0, 0, 0, nullptr },
  { 12, "Int8", MD_Menu::INP_INT, mnuIValueRqst, 4, -128, 0, 127, 0, 10, nullptr },
  { 13, "Int16", MD_Menu::INP_INT, mnuIValueRqst, 4, -32768, 0, 32767, 0, 10, nullptr },  // test field too small
  { 14, "Int32", MD_Menu::INP_INT, mnuIValueRqst, 6, -66636, 0, 65535, 0, 10, nullptr },
  { 15, "Hex16", MD_Menu::INP_INT, mnuIValueRqst, 4, 0x0000, 0, 0xffff, 0, 16, nullptr },  // test hex display
  { 16, "Float", MD_Menu::INP_FLOAT, mnuFValueRqst, 7, -10000, 0, 99950, 0, 10, nullptr },  // test float number
  { 17, "Eng", MD_Menu::INP_ENGU, mnuEValueRqst, 7, 0, 0, 999000, 3, 50, engUnit },  // test engineering units number
  { 18, "Confirm", MD_Menu::INP_RUN, myCode, 0, 0, 0, 0, 0, 10, nullptr },

  { 40, "Confirm", MD_Menu::INP_RUN, myLEDCode, 0, 0, 0, 0, 0, 0, nullptr },  // test using index in run code
  { 41, "Confirm", MD_Menu::INP_RUN, myLEDCode, 0, 0, 0, 0, 0, 0, nullptr },
};

// bring it all together in the global menu object
MD_Menu M(navigation, display,        // user navigation and display
  mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
  mnuItm, ARRAY_SIZE(mnuItm), // menu item data
  mnuInp, ARRAY_SIZE(mnuInp));// menu input data

// Callback code for menu set/get input values
MD_Menu::value_t *mnuLValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for list selection variable
{
  MD_Menu::value_t *r = &vBuf;

  if (id == 10)
  {
    if (bGet)
      vBuf.value = fruit;
    else
    {
      fruit = vBuf.value;
      Serial.print(F("\nList index changed to "));
      Serial.print(fruit);
    }
  }
  else
    r = nullptr;

  return(r);
}

MD_Menu::value_t *mnuBValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for boolean variable
{
  MD_Menu::value_t *r = &vBuf;
    
  if (id == 11)
  {
    if (bGet)
      vBuf.value = bValue;
    else
    {
      bValue = vBuf.value;
      Serial.print(F("\nBoolean changed to "));
      Serial.print(bValue);
    }
  }
  else
    r = nullptr;

  return(r);
}

MD_Menu::value_t *mnuIValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for integers variables
{
  MD_Menu::value_t *r = &vBuf;

  switch (id)
  {
  case 12:
    if (bGet)
      vBuf.value = int8Value;
    else
    {
      int8Value = vBuf.value;
      Serial.print(F("\nInt8 value changed to "));
      Serial.print(int8Value);
    }
    break;

  case 13:
  case 15:
    if (bGet)
      vBuf.value = int16Value;
    else
    {
      int16Value = vBuf.value;
      Serial.print(F("\nInt16 value changed to "));
      Serial.print(int16Value);
    }
    break;

  case 14:
    if (bGet)
      vBuf.value = int32Value;
    else
    {
      int32Value = vBuf.value;
      Serial.print(F("\nInt32 value changed to "));
      Serial.print(int32Value);
    }
    break;

  default:
    r = nullptr;
    break;
  }

  return(r);
}

MD_Menu::value_t *mnuFValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for floating value
{
  static int32_t f;
  MD_Menu::value_t *r = &vBuf;

  if (id == 16)
  {
    if (bGet)
      vBuf.value = (uint32_t)(floatValue * 100.0);
    else
    {
      floatValue = (vBuf.value / 100.0);
      Serial.print(F("\nFloat changed to "));
      Serial.print(floatValue);
    }
  }
  else
    r = nullptr;

  return(r);
}

MD_Menu::value_t *mnuEValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for engineering value
{
  if (id == 17)
  {
    if (bGet)
    {
      return(&engValue);
    }
    else
    {
      float f = (engValue.value / 1000.0);
      Serial.print(F("\nEng Unit changed to "));
      Serial.print(f);
      Serial.print("x10^");
      Serial.print(engValue.power);
    }
  }

  return(nullptr);
}

MD_Menu::value_t *myCode(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for run code input
{
  Serial.print(F("\nmyCode called id="));
  Serial.print(id);
  Serial.print(F(" to "));
  Serial.print(bGet ? F("GET") : F("SET - reset menu"));

  if (!bGet) M.reset();

  return(nullptr);
}

MD_Menu::value_t *myLEDCode(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for run code input
// Only use the index here
{
  Serial.print(F("\nSwitching LED "));
  Serial.print(id == 40 ? F("off") : F("on"));
  digitalWrite(LED_PIN, id == 40 ? LOW : HIGH);

  return(nullptr);
}

bool display(MD_Menu::userDisplayAction_t action, char *msg)
// Output display to a one of 2 line LCD display. 
// For a one line display, comment out the L0 handling code.
// The output display line is cleared with spaces before the
// requested message is shown.
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

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
// Using switches found on a LCD shield
// up and down map to INC and DEC
// Right and Left map to ESC
// Select maps to SEL
{
  incDelta = 1;
  if (lcdKeys.read() == MD_UISwitch::KEY_PRESS)
  {
    switch (lcdKeys.getKey())
    {
    case 'D': return(MD_Menu::NAV_DEC);
    case 'U': return(MD_Menu::NAV_INC);
    case 'S': return(MD_Menu::NAV_SEL);
    case 'R':
    case 'L': return(MD_Menu::NAV_ESC);
    }
  }

  return(MD_Menu::NAV_NULL);
}

// Standard setup() and loop()
void setup(void)
{
  Serial.begin(BAUD_RATE);
  Serial.print("\n[Menu_LCD_Shield Output]");

  pinMode(LED_PIN, OUTPUT);

  display(MD_Menu::DISP_INIT);

  lcdKeys.begin();
  lcdKeys.enableRepeat(false);

  M.begin();
  M.setMenuWrap(true);
  M.setAutoStart(AUTO_START);
  M.setTimeout(MENU_TIMEOUT);
}

void loop(void)
{
  static bool prevMenuRun = true;
  static uint16_t count = 0;
  static uint32_t timeCount = 0;

  // Detect if we need to initiate running normal user code
  if (prevMenuRun && !M.isInMenu())
    Serial.print("\n\nSWITCHING TO USER'S NORMAL OPERATION\n");
  prevMenuRun = M.isInMenu();

  // If we are not running and not autostart
  // check if there is a reason to start the menu
  if (!M.isInMenu() && !AUTO_START)
  {
    uint16_t dummy;

    if (navigation(dummy) == MD_Menu::NAV_SEL)
      M.runMenu(true);
  }

  // if we are not running the menu, show a counter counting
  if (!M.isInMenu() && (millis() - timeCount >= 1000))
  {
    lcd.setCursor(0, 0);
    lcd.print(count++);
    timeCount = millis();
  }
  else
    M.runMenu();   // just run the menu code each loop
}
