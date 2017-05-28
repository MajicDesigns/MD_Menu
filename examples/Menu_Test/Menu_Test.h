#pragma once
#include <MD_Menu.h>

// Select the User Input/Output hardware components that will be used
#define DISPLAY_SERIAL  1     // Serial Monitor display (useful for testing)
#define DISPLAY_LCD     0     // Two line LCD using LiquidCrystal library
#define DISPLAY_PAROLA  0     // One line LED matrix display using MD_Parola library

#define INPUT_SWITCH    0     // Use discrete switches for input
#define INPUT_LCDSWITCH 0     // Use analog based switches on LCD shield
#define INPUT_RENCODER  1     // Use rotary encoder with built in push switch

#define BAUD_RATE 57600