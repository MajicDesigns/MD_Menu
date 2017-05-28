// Example program for the MD_Menu library
//
// Test functionality of the library
//
#include "Menu_Test.h"

// Global menu data and definitions
uint8_t listValue = 2;
bool bValue = true;
int8_t  int8Value = 99;
int16_t int16Value = 999;
int32_t int32Value = 9999;

// Menu Headers --------
const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { 100, "MD_Menu",    100, 103, 0 },
  { 101, "Input Data", 200, 206, 0 },
  { 102, "More Menus", 100, 103, 0 },
};

// Menu Items ----------
const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  // Staring (Root) menu and More Menu
  { 100, "Input Test", MD_Menu::MNU_MENU, 101 },
  { 101, "Submenu 1",  MD_Menu::MNU_MENU, 102 },
  { 102, "Submenu 2",  MD_Menu::MNU_MENU, 102 },
  { 103, "Submenu 3",  MD_Menu::MNU_MENU, 102 },
  // Input Data submenu
  { 200, "List Select",MD_Menu::MNU_INPUT, 100 },
  { 201, "Boolean",    MD_Menu::MNU_INPUT, 101 },
  { 202, "Integer 8",  MD_Menu::MNU_INPUT, 102 },
  { 203, "Integer 16", MD_Menu::MNU_INPUT, 103 },
  { 204, "Integer 32", MD_Menu::MNU_INPUT, 104 },
  { 205, "Hex 16",     MD_Menu::MNU_INPUT, 105 },
  { 206, "Reset Menu", MD_Menu::MNU_INPUT, 106 },
};

// Input Items ---------
const PROGMEM char inList[] = "Apple\0Pear\0Orange\0Banana\0Pineapple\0Peach\0";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { 100, 0, "List",    MD_Menu::INP_LIST,  mnuLValueRqst,  6,      0,      0,  0, inList },
  { 101, 0, "Bool",    MD_Menu::INP_BOOL,  mnuBValueRqst,  1,      0,      0,  0, nullptr },
  { 102, 0, "Int8",    MD_Menu::INP_INT8,  mnuIValueRqst,  4,   -128,    127, 10, nullptr },
  { 103, 0, "Int16",   MD_Menu::INP_INT16, mnuIValueRqst,  4, -32768,  32767, 10, nullptr },
  { 104, 0, "Int32",   MD_Menu::INP_INT32, mnuIValueRqst,  6, -66636,  65535, 10, nullptr },
  { 105, 0, "Hex16",   MD_Menu::INP_INT16, mnuIValueRqst,  4, 0x0000, 0xffff, 16, nullptr },
  { 106, 0, "Confirm", MD_Menu::INP_RUN,   myCode,         0,      0,      0, 10, nullptr },
};

// bring it all together in the global menu object
MD_Menu M(navigation, display,        // user navigation and display
          mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
          mnuItm, ARRAY_SIZE(mnuItm), // menu item data
          mnuInp, ARRAY_SIZE(mnuInp));// menu input data

// Callback code for menu set/get input values
void *mnuLValueRqst(uint8_t id, MD_Menu::mnuId_t idx, bool bGet)
{
  if (id == 100)
  {
    if (bGet)
      return((void *)&listValue);
    else
    {
      Serial.print(F("\nList index changed to "));
      Serial.print(listValue);
    }
  }
}

void *mnuBValueRqst(uint8_t id, MD_Menu::mnuId_t idx, bool bGet)
{
  if (id == 101)
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

void *mnuIValueRqst(uint8_t id, MD_Menu::mnuId_t idx, bool bGet)
{
  switch (id)
  {
  case 102:
    if (bGet)
      return((void *)&int8Value);
    else
    {
      Serial.print(F("\nInt8 value changed to "));
      Serial.print(int8Value);
    }
    break;

  case 103:
  case 105:
    if (bGet)
      return((void *)&int16Value);
    else
    {
      Serial.print(F("\nInt16 value changed to "));
      Serial.print(int16Value);
    }
    break;

  case 104:
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

void *myCode(uint8_t id, uint8_t idx, bool bGet)
{
  Serial.print(F("\nmyCode called id="));
  Serial.print(id);
  Serial.print(F(" idx="));
  Serial.print(idx);
  Serial.print(F(" to "));
  Serial.print(bGet ? F("GET") : F("SET - reset menu"));

  if (!bGet) M.reset();
}

// Menu timer data and functions
uint32_t timeStart;

void timerStart(void)
{
  timeStart = millis();
}

void timerCheck(void)
{
  if (millis() - timeStart >= MENU_TIMEOUT)
  {
    Serial.print("\n-> Menu timeout");
    M.reset();
  }
}

// Standard setup() and loop()
void setup(void)
{
  Serial.begin(BAUD_RATE);
  Serial.print("\n[Menu_Test Debug]");

  setupDisp();
  setupNav();

  M.begin();
  M.setMenuWrap(true);
}

void loop(void)
{
  static bool prevMenuRun = true;
  uint16_t incDelta = 1;

  // Detect is we need to initiate running normal user code
  if (prevMenuRun && !M.isInMenu())
    Serial.print("\n\nRUNNING USER CODE\n");
  prevMenuRun = M.isInMenu();

  // If we are not running, check if there is a reason to start the menu
  // otherwise just check for a menu timeout
  if (!M.isInMenu())
  {
    if (navigation(incDelta) == MD_Menu::NAV_SEL)
      M.runMenu(true);
  }
  else
    timerCheck();

  // just run the menu if it needs to run
  M.runMenu();
}
