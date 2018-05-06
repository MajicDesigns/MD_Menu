// Example program for the MD_Menu library
//
// Test functionality of the library and demonstrate how a menu is structured.
// The code runs the menu and display debug information on the Serial Monitor.
//
// Different combinations of output display and navigation interfaces can be
// selected from the Menu_Test.h header file. Depending on the input and display
// options selected, one or more of the libraries listed below these may be
// required.
//
// The example UI callback routines cover the most common user input and display
// hardware:
//
// User Navigation - Menu_Test_Nav.cpp
// ---------------
// There are examples for user navigation for:
// - 3 separate momentary on switches for INC, DEC and ESC/SEL selections
// - Rotary encoder for INC/DEC (rotation) and momentary on switch for ESC/SEL
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
// Depending on the input or display options selected, one or more of these may be required:
// - MD_UISwitch library for digital and analog switches at https://github.com/MajicDesigns/MD_UISwitch
// - MD_REncoder library for rotary encoder input at https://github.com/MajicDesigns/MD_REncoder
// - MD_Parola library for LED matrix display at https://github.com/MajicDesigns/MD_Parola
// - MD_MAX72XX library to support MD_Parola library at https://github.com/MajicDesigns/MD_MAX72XX
// - LiquidCrystal library for LCD module is a standard Arduino library.
//
#include "Menu_Test.h"

// Global menu data and definitions
uint8_t fruit = 2;
bool bValue = true;
int8_t  int8Value = 99;
int16_t int16Value = 999;
int32_t int32Value = 9999;
float floatValue = 999.99;
MD_Menu::value_t engValue = { 999900, 3 };
char _txt[] = "192.168.1.101";

MD_Menu::value_t vBuf;  // interface buffer for values

// Menu Headers --------
const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { 10, "MD_Menu",      10, 16, 0 },
  { 11, "Input Data",   20, 27, 0 },
  { 12, "Serial Setup", 30, 33, 0 },
  { 13, "LED Menu",     40, 41, 0 },
  { 14, "FF Menu",      50, 51, 0 },
  { 15, "Realtime FB",  60, 64, 0 },
  { 16, "Output TXT",   70, 70, 0 },

};

// Menu Items ----------
const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  // Starting (Root) menu
  { 10, "Input Test",  MD_Menu::MNU_MENU, 11 },
  { 11, "Serial",      MD_Menu::MNU_MENU, 12 },
  { 12, "LED",         MD_Menu::MNU_MENU, 13 },
  { 13, "More Menu",   MD_Menu::MNU_MENU, 10 },
  { 14, "Flip-Flop",   MD_Menu::MNU_MENU, 14 },
  { 15, "Realtime FB", MD_Menu::MNU_MENU, 15 },
  { 16, "Output TXT",  MD_Menu::MNU_MENU, 16 },

  // Input Data submenu
  { 20, "Fruit List", MD_Menu::MNU_INPUT, 10 },
  { 21, "Boolean",    MD_Menu::MNU_INPUT, 11 },
  { 22, "Integer 8",  MD_Menu::MNU_INPUT, 12 },
  { 23, "Integer 16", MD_Menu::MNU_INPUT, 13 },
  { 24, "Integer 32", MD_Menu::MNU_INPUT, 14 },
  { 25, "Hex 16",     MD_Menu::MNU_INPUT, 15 },
  { 26, "Float",      MD_Menu::MNU_INPUT, 16 },
  { 27, "Eng Units",  MD_Menu::MNU_INPUT, 17 },
  { 28, "Reset Menu", MD_Menu::MNU_INPUT, 18 },
  // Serial Setup
  { 30, "COM Port",  MD_Menu::MNU_INPUT, 30 },
  { 31, "Speed",     MD_Menu::MNU_INPUT, 31 },
  { 32, "Parity",    MD_Menu::MNU_INPUT, 32 },
  { 33, "Stop Bits", MD_Menu::MNU_INPUT, 33 },
  // LED
  { 40, "Turn Off", MD_Menu::MNU_INPUT, 40 },
  { 41, "Turn On",  MD_Menu::MNU_INPUT, 41 },
  // Flip-flop - boolean controls variable edit
  { 50, "Flip", MD_Menu::MNU_INPUT, 50 },
  { 51, "Flop", MD_Menu::MNU_INPUT, 51 },
  // Realtime feedback variable edit
  { 60, "Fruit List", MD_Menu::MNU_INPUT_FB, 10 },
  { 61, "Boolean",    MD_Menu::MNU_INPUT_FB, 11 },
  { 62, "Integer 8",  MD_Menu::MNU_INPUT_FB, 12 },
  { 63, "Float",      MD_Menu::MNU_INPUT_FB, 16 },
  { 64, "Eng Units",  MD_Menu::MNU_INPUT_FB, 17 },

  // Output Data submenu
  { 70, "Output TXT", MD_Menu::MNU_INPUT, 60 },
};

// Input Items ---------
const PROGMEM char listFruit[] = "Apple|Pear|Orange|Banana|Pineapple|Peach";
const PROGMEM char listCOM[] = "COM1|COM2|COM3|COM4";
const PROGMEM char listBaud[] = "9600|19200|57600|115200";
const PROGMEM char listParity[] = "O|E|N";
const PROGMEM char listStop[] = "0|1";
const PROGMEM char engUnit[] = "Hz";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { 10, "List",     MD_Menu::INP_LIST,  mnuLValueRqst, 6,       0, 0,      0, 0,  0, listFruit }, // shorter and longer list labels
  { 11, "Bool",     MD_Menu::INP_BOOL,  mnuBValueRqst, 1,       0, 0,      0, 0,  0, nullptr },
  { 12, "Int8",     MD_Menu::INP_INT,   mnuIValueRqst, 4,    -128, 0,    127, 0, 10, nullptr },
  { 13, "Int16",    MD_Menu::INP_INT,   mnuIValueRqst, 4,  -32768, 0,  32767, 0, 10, nullptr },  // test field too small
  { 14, "Int32",    MD_Menu::INP_INT,   mnuIValueRqst, 6,  -66636, 0,  65535, 0, 10, nullptr },
  { 15, "Hex16",    MD_Menu::INP_INT,   mnuIValueRqst, 4,  0x0000, 0, 0xffff, 0, 16, nullptr },  // test hex display
  { 16, "Float",    MD_Menu::INP_FLOAT, mnuFValueRqst, 7,  -10000, 0,  99950, 0, 10, nullptr },  // test float number
  { 17, "Eng Unit", MD_Menu::INP_ENGU,  mnuEValueRqst, 7,       0, 0, 999000, 3, 50, engUnit },  // test engineering units number
  { 18, "Confirm",  MD_Menu::INP_RUN,   myCode,        0,       0, 0,      0, 0, 10, nullptr },

  { 30, "Port",     MD_Menu::INP_LIST, mnuSerialValueRqst, 4, 0, 0, 0, 0, 0, listCOM },
  { 31, "Bits/s",   MD_Menu::INP_LIST, mnuSerialValueRqst, 6, 0, 0, 0, 0, 0, listBaud },
  { 32, "Parity",   MD_Menu::INP_LIST, mnuSerialValueRqst, 1, 0, 0, 0, 0, 0, listParity },
  { 33, "No. Bits", MD_Menu::INP_LIST, mnuSerialValueRqst, 1, 0, 0, 0, 0, 0, listStop },

  { 40, "Confirm", MD_Menu::INP_RUN, myLEDCode, 0, 0, 0, 0, 0, 0, nullptr },  // test using index in run code
  { 41, "Confirm", MD_Menu::INP_RUN, myLEDCode, 0, 0, 0, 0, 0, 0, nullptr },

  { 50, "Flip", MD_Menu::INP_INT, mnuFFValueRqst, 4, -128, 0, 127, 0, 10, nullptr },
  { 51, "Flop", MD_Menu::INP_INT, mnuFFValueRqst, 4, -128, 0, 127, 0, 16, nullptr },

  { 60, "TXT",  	MD_Menu::INP_RUN,   myCode,        0,       0, 0,      0, 0, 10, nullptr },  // test output TXT
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
  if (id == 10)
  {
    if (bGet)
    {
      vBuf.value = fruit;
      return(&vBuf);
    }
    else
    {
      fruit = vBuf.value;
      Serial.print(F("\nList index changed to "));
      Serial.print(fruit);
    }
  }
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

  return (r);
}

MD_Menu::value_t *mnuSerialValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for Serial parameters
{
  static uint8_t port = 0, speed = 0, parity = 0, stop = 0;
  MD_Menu::value_t *r = &vBuf;

  switch (id)
  {
    case 30:
      if (bGet)
        vBuf.value = port;
      else
      {
        port - vBuf.value;
        Serial.print(F("\nPort index="));
        Serial.print(port);
      }
      break;

    case 31:
      if (bGet)
        vBuf.value = speed;
      else
      {
        speed = vBuf.value;
        Serial.print(F("\nSpeed index="));
        Serial.print(speed);
      }
      break;

    case 32:
      if (bGet)
        vBuf.value = parity;
      else
      {
        parity = vBuf.value;
        Serial.print(F("\nParity index="));
        Serial.print(parity);
      }
      break;

    case 33:
      if (bGet)
        vBuf.value = stop;
      else
      {
        stop = vBuf.value;
        Serial.print(F("\nStop index="));
        Serial.print(stop);
      }
      break;

    default:
      r = nullptr;
      break;
  }

  return (r);
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
      return(&engValue);
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

MD_Menu::value_t *mnuFFValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value edit allowed request depends on another value
{
  static bool gateKeeper = false;
  static bool b;
  MD_Menu::value_t *r = &vBuf;

  switch (id)
  {
    case 50:
      if (bGet)
      {
        if (gateKeeper)
        {
          Serial.print(F("\nFlipFlop value blocked"));
          r = nullptr;
        }
        else
          vBuf.value = int8Value;
      }
      else
      {
        int8Value = vBuf.value;
        Serial.print(F("\nFlipFlop value changed to "));
        Serial.print(int8Value);
        gateKeeper = !gateKeeper;
      }
      break;

    case 51:
      if (bGet)
      {
        if (!gateKeeper)    // reverse the logic of above
        {
          Serial.print(F("\nFlipFlop value blocked"));
          r = nullptr;
        }
        else
          vBuf.value = int8Value;
      }
      else
      {
        int8Value = vBuf.value;
        Serial.print(F("\nFlipFlop value changed to "));
        Serial.print(int8Value);
        gateKeeper = !gateKeeper;
      }
      break;

    default:
      r = nullptr;
      break;
  }

  return(r);
}

MD_Menu::value_t *mnuFBValueRqst(MD_Menu::mnuId_t id, bool bGet)
// Value request callback using real time feedback selection variable
{
  MD_Menu::value_t *r = &vBuf;

  switch (id)
  {
  case 60:
    if (bGet)
      vBuf.value = uint8Value;
    else
    {
      uint8Value = vBuf.value;
      Serial.print(F("\nUint8 value changed to "));
      Serial.print(int8Value);
    }
    break;

  case 61:
    if (bGet)
    {
      vBuf.value = fruit;
      return(&vBuf);
    }
    else
    {
      fruit = vBuf.value;
      Serial.print(F("\nFruit index changed to "));
      Serial.print(fruit);
    }
    break;

  default:
    r = nullptr;
    break;
  }

  return(r);
}

MD_Menu::value_t *myCode(MD_Menu::mnuId_t id, bool bGet)
// Value request callback for run code input
{
  switch (id)
  {
    case 18:
      Serial.print(F("\nmyCode called id="));
      Serial.print(id);
      Serial.print(F(" to "));
      Serial.print(bGet ? F("GET") : F("SET - reset menu"));

      if (!bGet) M.reset();
      break;
  
    case 60:
      if (bGet) return ((MD_Menu::value_t *)&_txt);
      break;
  }
  return (nullptr);
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

// Standard setup() and loop()
void setup(void)
{
  Serial.begin(BAUD_RATE);
  Serial.print("\n[Menu_Test Debug]");

  pinMode(LED_PIN, OUTPUT);

  display(MD_Menu::DISP_INIT);
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

  M.runMenu();   // just run the menu code
}
