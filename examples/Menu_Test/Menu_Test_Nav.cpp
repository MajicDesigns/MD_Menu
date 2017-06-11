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

#include <MD_KeySwitch.h>

const uint8_t INC_PIN = 3;
const uint8_t DEC_PIN = 4;
const uint8_t CTL_PIN = 5;

MD_KeySwitch swInc(INC_PIN);
MD_KeySwitch swDec(DEC_PIN);
MD_KeySwitch swCtl(CTL_PIN);

void setupNav(void)
{
  swInc.begin();
  swDec.begin();
  swCtl.begin();
  swCtl.enableRepeat(false);
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  MD_Menu::userNavAction_t nav = MD_Menu::NAV_NULL;

  if (swInc.read() == MD_KeySwitch::KS_PRESS) nav = MD_Menu::NAV_INC;
  else if (swDec.read() == MD_KeySwitch::KS_PRESS) nav = MD_Menu::NAV_DEC;

  switch (swCtl.read())
  {
  case MD_KeySwitch::KS_PRESS: nav = MD_Menu::NAV_SEL; break;;
  case MD_KeySwitch::KS_LONGPRESS: nav = MD_Menu::NAV_ESC; break;
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

#include <MD_AButton.h>

#define LCD_KEYS KEY_ADC_PORT // A0 default

MD_AButton lcdKeys(LCD_KEYS);

void setupNav(void)
{
}

MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  char c = lcdKeys.getKey();

  incDelta = 1;
  switch (c)
  {
  case 'D': return(MD_Menu::NAV_DEC);
  case 'U': return(MD_Menu::NAV_INC);
  case 'S': return(MD_Menu::NAV_SEL);
  case 'R':
  case 'L': return(MD_Menu::NAV_ESC);
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
#include <MD_KeySwitch.h>

extern MD_Menu M;

const uint8_t RE_A_PIN = 2;
const uint8_t RE_B_PIN = 3;
const uint8_t CTL_PIN = 4;

MD_REncoder  RE(RE_A_PIN, RE_B_PIN);
MD_KeySwitch swCtl(CTL_PIN);

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
    incDelta = (M.isInEdit() ? (1 << abs(RE.speed() / 10) : 1);
    return(re == DIR_CCW ? MD_Menu::NAV_DEC : MD_Menu::NAV_INC);
  }

  switch (swCtl.read())
  {
  case MD_KeySwitch::KS_PRESS:     return(MD_Menu::NAV_SEL);
  case MD_KeySwitch::KS_LONGPRESS: return(MD_Menu::NAV_ESC);
  }

  return(MD_Menu::NAV_NULL);
}
#endif
