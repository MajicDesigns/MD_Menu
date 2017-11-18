#pragma once

#include <MD_Menu.h>

const bool AUTO_START = true; // auto start the menu, manual detect and start if false

// Select the User Input/Output hardware components that will be used
#define DISPLAY_SERIAL  1  // Serial Monitor display (useful for testing)
#define DISPLAY_LCD     0  // Two line LCD using LiquidCrystal library
#define DISPLAY_PAROLA  0  // One line LED matrix display using MD_Parola library

#define INPUT_SWITCH    1  // Use discrete switches for input
#define INPUT_LCDSWITCH 0  // Use analog based switches on LCD shield
#define INPUT_RENCODER  0  // Use rotary encoder with built in push switch

const uint32_t BAUD_RATE = 57600;   // Serial Monitor speed setting 
const uint16_t MENU_TIMEOUT = 5000; // in milliseconds

const uint8_t LED_PIN = 13;  // for myLEDCode function

// Function prototypes for Navigation/Display
void setupDisp(void);
void setupNav(void);
bool display(MD_Menu::userDisplayAction_t, char*);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);

// Function prototypes for variable get/set functions
void *mnuLValueRqst(MD_Menu::mnuId_t id, bool bGet);
void *mnuBValueRqst(MD_Menu::mnuId_t id, bool bGet);
void *mnuIValueRqst(MD_Menu::mnuId_t id, bool bGet);
void *mnuFValueRqst(MD_Menu::mnuId_t id, bool bGet);
void *mnuFFValueRqst(MD_Menu::mnuId_t id, bool bGet);
void *mnuSerialValueRqst(MD_Menu::mnuId_t id, bool bGet);
void *myCode(MD_Menu::mnuId_t id, bool bGet);
void *myLEDCode(MD_Menu::mnuId_t id, bool bGet);

