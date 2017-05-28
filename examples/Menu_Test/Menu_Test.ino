// Example program for the MD_Menu library
//
// Test functionality of the library
//
#include "Menu_Test.h"

// Function prototypes
void setupDisp(void);
void setupNav(void);
bool display(MD_Menu::userDisplayAction_t, char*);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);

// Global menu data and definitions
uint8_t listValue = 2;
bool bValue = true;
int8_t  int8Value = 99;
int16_t int16Value = 999;
int32_t int32Value = 9999;

const PROGMEM char inList[] = "Apple\0Pear\0Orange\0Banana\0Pineapple\0Peach\0";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { 100, 0, "List", MD_Menu::INP_LIST, mnuLValueRqst, 6, 0, 0, inList },
  { 101, 0, "Bool", MD_Menu::INP_BOOL, mnuBValueRqst, 1, 0, 0, nullptr },
  { 102, 0, "Int8", MD_Menu::INP_INT8, mnuIValueRqst, 4, -127, 128, nullptr },
  { 103, 0, "Int16", MD_Menu::INP_INT16, mnuIValueRqst, 6, -32767, 32768, nullptr },
  { 104, 0, "Int32", MD_Menu::INP_INT32, mnuIValueRqst, 10, -66635, 65536, nullptr },
  { 105, 0, "Confirm", MD_Menu::INP_RUN, myCode, 0, 0, 0, nullptr },
};

const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  { 100, "Submenu 0", MD_Menu::MNU_MENU, 101 },
  { 101, "Submenu 1", MD_Menu::MNU_MENU, 102 },
  { 102, "Submenu 2", MD_Menu::MNU_MENU, 101 },
  { 103, "Submenu 3", MD_Menu::MNU_MENU, 102 },
  { 104, "Input List", MD_Menu::MNU_INPUT, 100 },
  { 105, "Input Bool", MD_Menu::MNU_INPUT, 101 },
  { 106, "Input I8", MD_Menu::MNU_INPUT, 102 },
  { 107, "Input I16", MD_Menu::MNU_INPUT, 103 },
  { 108, "Input I32", MD_Menu::MNU_INPUT, 104 },
  { 109, "Reset Menu", MD_Menu::MNU_INPUT, 105 },
};

const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { 100, "Root",   100, 103, 0 },
  { 101, "Input", 104, 109, 0 },
  { 102, "More Menu", 100, 103, 0 },
};

// bring it all together in the global menu object
MD_Menu M(navigation, display,        // user navigation and display
          mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
          mnuItm, ARRAY_SIZE(mnuItm), // menu item data
          mnuInp, ARRAY_SIZE(mnuInp));// menu input data

// Callback code for menu set/get
void *mnuLValueRqst(uint8_t id, uint8_t idx, bool bGet)
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

void *mnuBValueRqst(uint8_t id, uint8_t idx, bool bGet)
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

void *mnuIValueRqst(uint8_t id, uint8_t idx, bool bGet)
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
  Serial.print(F("\nMyCode called id="));
  Serial.print(id);
  Serial.print(F(" idx="));
  Serial.print(idx);
  Serial.print(F(" to "));
  Serial.print(bGet ? F("GET") : F("SET - reset menu"));

  if (!bGet) M.reset();
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
  uint16_t incDelta = 1;

  // if we are not running, check if there is a reason to start the menu
  if (!M.isInMenu())
  {
    if (navigation(incDelta) == MD_Menu::NAV_SEL)
      M.runMenu(true);
  }
  
  M.runMenu();  // just run the menu
}
