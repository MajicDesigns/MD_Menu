#pragma once
// Library definitions for MD_Menu library
//
// See the main header file MD_Menu.h for more information

/**
 * \file
 * \brief Contains internal library definitions
 */

#define MD_DEBUG 0  ///< set to 1 to enable library debugging mode

#if MD_DEBUG
#define MD_PRINTS(s)   { Serial.print(F(s)); }                  ///< Library debugging output macro
#define MD_PRINT(s, v) { Serial.print(F(s)); Serial.print(v); } ///< Library debugging output macro
#else
#define MD_PRINTS(s)      ///< Library debugging output macro
#define MD_PRINT(s, v)    ///< Library debugging output macro
#endif

const char FLD_PROMPT[] = ":";   ///< Prompt separateor between input field label and left delimiter
const char FLD_DELIM_L[] = "[";  ///< Left delimiter for variable field input
const char FLD_DELIM_R[] = "]";  ///< Right delimiter for variable field input
const char MNU_DELIM_L[] = "<";  ///< Left delimiter for menu option label
const char MNU_DELIM_R[] = ">";  ///< Right delimiter for menu option label

const char INP_BOOL_T[] = "Y";   ///< Boolean input True display value. Length should be same as INP_BOOL_F
const char INP_BOOL_F[] = "N";   ///< Boolean input False display value. Length should be same as INP_BOOL_T

const char INP_NUMERIC_OFLOW = '#';  ///< Indicator for numeric overflow

#define INP_PRE_SIZE(mi)  (strlen(mi->label) + strlen(FLD_PROMPT) + strlen(FLD_DELIM_L))  ///< Size of text pre variable display
#define INP_POST_SIZE(mi) (strlen(FLD_DELIM_R))  ///< Size of text after variable display

#define CB_DISP(l,m) if (_cbDisp != nullptr) _cbDisp(l, m);   ///< Display only if dispay callback available
