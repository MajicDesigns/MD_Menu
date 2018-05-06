#include "Menu_Test.h"

// Routines for specific user navigation hardware, defining
// - Appropriate header files
// - Hardware definitions
// - Global library objects
// - Navigation callback function 

#if INPUT_SWITCH
// Implemented as momentary on 3 switches 
// one switch each for INC, DEC
// one switch for SEL (click) or ESC (long press)

#include <MD_UISwitch.h>

const uint8_t INC_PIN = 3;
const uint8_t DEC_PIN = 4;
const uint8_t CTL_PIN = 5;
uint8_t pins[] = { INC_PIN, DEC_PIN, CTL_PIN };

MD_UISwitch_Digital swNav(pins, ARRAY_SIZE(pins), LOW);

void setupNav(void)
{
  swNav.begin();
  swNav.enableRepeat(false);
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  MD_Menu::userNavAction_t nav = MD_Menu::NAV_NULL;

  switch (swNav.read())
  {
    case MD_UISwitch::KEY_PRESS:
    {
      Serial.print(swNav.getKey());
      switch (swNav.getKey())
      {
      case INC_PIN: nav = MD_Menu::NAV_INC; break;
      case DEC_PIN: nav = MD_Menu::NAV_DEC; break;
      case CTL_PIN: nav = MD_Menu::NAV_SEL; break;
      }
    }
    break;

    case MD_UISwitch::KEY_LONGPRESS:
    {
      if (swNav.getKey() == 2)
        nav = MD_Menu::NAV_ESC;
    }
    break;
  }

  incDelta = 1;
  
  return(nav);
}
#endif

#if INPUT_LCDSWITCH
// Using switches found on a LCD shield
// up and down map to INC and DEC
// Right and Left map to ESC
// Select maps to SEL

#include <MD_UISwitch.h>

#define LCD_KEYS_PIN A0

// These key values work for most LCD shields
MD_UISwitch_Analog::uiAnalogKeys_t kt[] =
{
  {  10, 10, 'R' },  // Right
  { 140, 15, 'U' },  // Up
  { 315, 15, 'D' },  // Down
  { 490, 15, 'L' },  // Left
  { 725, 15, 'S' },  // Select
};

MD_UISwitch_Analog lcdKeys(LCD_KEYS_PIN, kt, ARRAY_SIZE(kt));

void setupNav(void)
{
  lcdKeys.begin();
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
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
#endif

#if INPUT_RENCODER
// Implemented as a rotary encoder with integrated push switch.
// Rotate encoder clockwise for INC, anti-clockwise for DEC
// Switch for SEL (press) or ESC (long press)
//
// If a value is being edited, the rotary encoder will also apply 
// a factor for how fast the RE is being turned to increase the rate 
// of change of values. This will be applied by the menu library to 
// numeric input only.

#include <MD_REncoder.h>
#include <MD_UISwitch.h>

extern MD_Menu M;

const uint8_t RE_A_PIN = 2;
const uint8_t RE_B_PIN = 3;
const uint8_t CTL_PIN = 4;

MD_REncoder  RE(RE_A_PIN, RE_B_PIN);
MD_UISwitch_Digital swCtl(CTL_PIN);

void setupNav(void)
{
  RE.begin();
  swCtl.begin();
  swCtl.enableRepeat(false);
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  uint8_t re = RE.read();

  if (re != DIR_NONE)
  {
    incDelta = (M.isInEdit() ? (1 << abs(RE.speed() / 10)) : 1);
    return(re == DIR_CCW ? MD_Menu::NAV_DEC : MD_Menu::NAV_INC);
  }

  switch (swCtl.read())
  {
  case MD_UISwitch::KEY_PRESS:     return(MD_Menu::NAV_SEL);
  case MD_UISwitch::KEY_LONGPRESS: return(MD_Menu::NAV_ESC);
  }

  return(MD_Menu::NAV_NULL);
}
#endif

#if INPUT_SERIAL
// Using Serial as input
// 'U' and 'D' map to INC and DEC
// 'R' and 'L' map to ESC
// 'S' maps to SEL

void setupNav(void)
{
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  char c = 0;
  if(Serial.available()>0)
    c = Serial.read();

  incDelta = 1;
  switch (c)
  {
  case 'D':  return(MD_Menu::NAV_DEC);
  case 'U':  return(MD_Menu::NAV_INC);
  case 'S':  return(MD_Menu::NAV_SEL);
  case 'R':
  case 'L':  return(MD_Menu::NAV_ESC);
  }

  return(MD_Menu::NAV_NULL);
}
#endif
