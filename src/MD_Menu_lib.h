#pragma once
// Library definitions for MD_Menu library
//
// See the main header file MD_Menu.h for more information

/**
 * \file
 * \brief Contains internal library definitions
 */

#define MD_DEBUG 1  ///< set to 1 to enable library debugging mode

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

const char INP_BOOL_T[] = "Y";   ///< Boolean input True display value. Length should be same as INP_BOOL_F.
const char INP_BOOL_F[] = "N";   ///< Boolean input False display value. Length should be same as INP_BOOL_T.
