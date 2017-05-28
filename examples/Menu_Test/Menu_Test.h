#pragma once
#include <MD_Menu.h>

// Select the User Input/Output hardware components that will be used
#define DISPLAY_SERIAL  1  // Serial Monitor display (useful for testing)
#define DISPLAY_LCD     0  // Two line LCD using LiquidCrystal library
#define DISPLAY_PAROLA  0  // One line LED matrix display using MD_Parola library

#define INPUT_SWITCH    0  // Use discrete switches for input
#define INPUT_LCDSWITCH 0  // Use analog based switches on LCD shield
#define INPUT_RENCODER  1  // Use rotary encoder with built in push switch

#define BAUD_RATE    57600 // Serial Monitor speed setting 
#define MENU_TIMEOUT 10000 // in milliseconds

// Function prototypes for Navigation/Display
void setupDisp(void);
void setupNav(void);
bool display(MD_Menu::userDisplayAction_t, char*);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);

// Function prototypes for variable get/set functions
void *mnuLValueRqst(uint8_t id, MD_Menu::mnuId_t idx, bool bGet);
void *mnuBValueRqst(uint8_t id, MD_Menu::mnuId_t idx, bool bGet);
void *mnuIValueRqst(uint8_t id, MD_Menu::mnuId_t idx, bool bGet);
void *myCode(uint8_t id, uint8_t idx, bool bGet);

// Menu timeout functions prototypes
void timerStart(void);
void timerCheck(void);
