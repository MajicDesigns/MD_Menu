// Example program for the MD_Menu library
//
// Test functionality of the library and demonstrate how a menu is structured.
// The code runs the menu and display results on the Serial Monitor.
// 
// Different combinations of output display and navigation interfaces can be 
// selected from the Menu_Test.h header file. The example callback routines
// cover what would be the most common user input and display hardware:
//
// User Navigation - Menu_Test_Nav.cpp
// ---------------
// There are examples for user navigation for:
// - 3 switches for INC, DEC and ESC/SEL selections
// - Rotary encoder for INC/DEC and switch for ESC/SEL
// - Analog 'resistor ladder' switches common on LCD shields for INC, DEC, ESC and SEL
//
// User Display - Menu_Test_Disp.cpp
// ------------
// - Serial Monitor output (useful for debugging)
// - LCD module display (LiquidCrystal Library)
// - Parola library and LED matrix modules
//
// External Dependencies
// ---------------------
// - MD_KeySwitch library for digital switches at https://github.com/MajicDesigns/MD_KeySwitch
// - MD_AButton library for LCD shield switches at https://github.com/MajicDesigns/MD_AButton
// - MD_REncoder library for rotary encoder input at https://github.com/MajicDesigns/MD_REncoder
// - MD_Parola library for LED matrix display at https://github.com/MajicDesigns/MD_Parola
// - MD_MAX72XX library to support Parola library at https://github.com/MajicDesigns/MD_MAX72XX
// - LiquidCrystal libray for LCD module is a standard Arduino library
//
#include "Menu_Test.h"

// Global menu data and definitions
uint8_t fruit = 2;
bool bValue = true;
int8_t  int8Value = 99;
int16_t int16Value = 999;
int32_t int32Value = 9999;

// Menu Headers --------
const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { 10, "MD_Menu",      10, 13, 0 },
  { 11, "Input Data",   20, 26, 0 },
  { 12, "Serial Setup", 30, 33, 0 },
  { 13, "LED Menu",     40, 41, 0 },
};

// Menu Items ----------
const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  // Starting (Root) menu
  { 10, "Input Test", MD_Menu::MNU_MENU, 11 },
  { 11, "Serial",     MD_Menu::MNU_MENU, 12 },
  { 12, "LED",        MD_Menu::MNU_MENU, 13 },
  { 13, "More Menu",  MD_Menu::MNU_MENU, 10 },
  // Input Data submenu
  { 20, "Fruit List", MD_Menu::MNU_INPUT, 10 },
  { 21, "Boolean",    MD_Menu::MNU_INPUT, 11 },
  { 22, "Integer 8",  MD_Menu::MNU_INPUT, 12 },
  { 23, "Integer 16", MD_Menu::MNU_INPUT, 13 },
  { 24, "Integer 32", MD_Menu::MNU_INPUT, 14 },
  { 25, "Hex 16",     MD_Menu::MNU_INPUT, 15 },
  { 26, "Reset Menu", MD_Menu::MNU_INPUT, 16 },
  // Serial Setup
  { 30, "COM Port",  MD_Menu::MNU_INPUT, 30 },
  { 31, "Speed",     MD_Menu::MNU_INPUT, 31 },
  { 32, "Parity",    MD_Menu::MNU_INPUT, 32 },
  { 33, "Stop Bits", MD_Menu::MNU_INPUT, 33 },
  // LED  
  { 40, "Turn Off", MD_Menu::MNU_INPUT, 40 },
  { 41, "Turn On",  MD_Menu::MNU_INPUT, 41 },

};

// Input Items ---------
const PROGMEM char listFruit[] = "Apple|Pear|Orange|Banana|Pineapple|Peach";
const PROGMEM char listCOM[] = "COM1|COM2|COM3|COM4";
const PROGMEM char listBaud[] = "9600|19200|57600|115200";
const PROGMEM char listParity[] = "O|E|N";
const PROGMEM char listStop[] = "0|1";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { 10, 0, "List",    MD_Menu::INP_LIST,  mnuLValueRqst,  6,      0,      0,  0, listFruit }, // shorter and longer list labels
  { 11, 0, "Bool",    MD_Menu::INP_BOOL,  mnuBValueRqst,  1,      0,      0,  0, nullptr },
  { 12, 0, "Int8",    MD_Menu::INP_INT8,  mnuIValueRqst,  4,   -128,    127, 10, nullptr },
  { 13, 0, "Int16",   MD_Menu::INP_INT16, mnuIValueRqst,  4, -32768,  32767, 10, nullptr },  // test field too small
  { 14, 0, "Int32",   MD_Menu::INP_INT32, mnuIValueRqst,  6, -66636,  65535, 10, nullptr },
  { 15, 0, "Hex16",   MD_Menu::INP_INT16, mnuIValueRqst,  4, 0x0000, 0xffff, 16, nullptr },  // test hex display
  { 16, 0, "Confirm", MD_Menu::INP_RUN,   myCode,         0,      0,      0, 10, nullptr },

  { 30, 0, "Port",     MD_Menu::INP_LIST, mnuSerialValueRqst, 4, 0, 0, 0, listCOM },
  { 31, 0, "Bits/s",   MD_Menu::INP_LIST, mnuSerialValueRqst, 6, 0, 0, 0, listBaud },
  { 32, 0, "Parity",   MD_Menu::INP_LIST, mnuSerialValueRqst, 1, 0, 0, 0, listParity },
  { 33, 0, "No. Bits", MD_Menu::INP_LIST, mnuSerialValueRqst, 1, 0, 0, 0, listStop },

  { 40, 0, "Confirm", MD_Menu::INP_RUN, myLEDCode, 0, 0, 0, 0, nullptr },  // test using index in run code
  { 41, 1, "Confirm", MD_Menu::INP_RUN, myLEDCode, 0, 0, 0, 0, nullptr },

};

// bring it all together in the global menu object
MD_Menu M(navigation, display,        // user navigation and display
          mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
          mnuItm, ARRAY_SIZE(mnuItm), // menu item data
          mnuInp, ARRAY_SIZE(mnuInp));// menu input data

// Callback code for menu set/get input values
void *mnuLValueRqst(MD_Menu::mnuId_t id, uint8_t idx, bool bGet)
// Value request callback for list selection variable
{
  if (id == 10)
  {
    if (bGet)
      return((void *)&fruit);
    else
    {
      Serial.print(F("\nList index changed to "));
      Serial.print(fruit);
    }
  }
}

void *mnuBValueRqst(MD_Menu::mnuId_t id, uint8_t idx, bool bGet)
// Value request callback for boolean variable
{
  if (id == 11)
  {
    if (bGet)
      return((void *)&bValue);
    else
    {
      Serial.print(F("\nBoolean changed to "));
      Serial.print(bValue);
    }
  }
}

void *mnuIValueRqst(MD_Menu::mnuId_t id, uint8_t idx, bool bGet)
// Value request callback for integers variables
{
  switch (id)
  {
  case 12:
    if (bGet)
      return((void *)&int8Value);
    else
    {
      Serial.print(F("\nInt8 value changed to "));
      Serial.print(int8Value);
    }
    break;

  case 13:
  case 15:
    if (bGet)
      return((void *)&int16Value);
    else
    {
      Serial.print(F("\nInt16 value changed to "));
      Serial.print(int16Value);
    }
    break;

  case 14:
    if (bGet)
      return((void *)&int32Value);
    else
    {
      Serial.print(F("\nInt32 value changed to "));
      Serial.print(int32Value);
    }
    break;
  }

  return(nullptr);
}

void *mnuSerialValueRqst(MD_Menu::mnuId_t id, uint8_t idx, bool bGet)
// Value request callback for Serial parameters
{
  static uint8_t port = 0, speed = 0, parity = 0, stop = 0;

  switch (id)
  {
  case 30:
    if (bGet)
      return((void *)&port);
    else
    {
      Serial.print(F("\nPort index="));
      Serial.print(port);
    }
    break;

  case 31:
    if (bGet)
      return((void *)&speed);
    else
    {
      Serial.print(F("\nSpeed index="));
      Serial.print(speed);
    }
    break;

  case 32:
    if (bGet)
      return((void *)&parity);
    else
    {
      Serial.print(F("\nParity index="));
      Serial.print(parity);
    }
    break;

  case 33:
    if (bGet)
      return((void *)&stop);
    else
    {
      Serial.print(F("\nStop index="));
      Serial.print(stop);
    }
    break;
  }
  return(nullptr);
}

void *myCode(MD_Menu::mnuId_t id, uint8_t idx, bool bGet)
// Value request callback for run code input
{
  Serial.print(F("\nmyCode called id="));
  Serial.print(id);
  Serial.print(F(" idx="));
  Serial.print(idx);
  Serial.print(F(" to "));
  Serial.print(bGet ? F("GET") : F("SET - reset menu"));

  if (!bGet) M.reset();
}

void *myLEDCode(MD_Menu::mnuId_t id, uint8_t idx, bool bGet)
// Value request callback for run code input
// Only use the index here
{
  Serial.print(F("\nSwitchig LED "));
  Serial.print(idx == 0 ? F("off") : F("on"));
  digitalWrite(LED_PIN, idx == 0 ? LOW : HIGH);
}

// Standard setup() and loop()
void setup(void)
{
  Serial.begin(BAUD_RATE);
  Serial.print("\n[Menu_Test Debug]");

  pinMode(LED_PIN, OUTPUT);

  setupDisp();
  setupNav();

  M.begin();
  M.setMenuWrap(true);
  M.setAutoStart(AUTO_START);
  M.setTimeout(MENU_TIMEOUT);
}

void loop(void)
{
  static bool prevMenuRun = true;

  // Detect if we need to initiate running normal user code
  if (prevMenuRun && !M.isInMenu())
    Serial.print("\n\nRUNNING USER'S NORMAL OPERATION\n");
  prevMenuRun = M.isInMenu();

  // If we are not running andnot autostart
  // check if there is a reason to start the menu
  if (!M.isInMenu() && !AUTO_START)
  {
    uint16_t dummy;

    if (navigation(dummy) == MD_Menu::NAV_SEL)
      M.runMenu(true);
  }

  M.runMenu();   // just run the menu code

}
