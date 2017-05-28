#pragma once
/**
\mainpage MD_Menu Library
This library implements menus for display devices with up to 2 display lines. 

Features of this menu library include:

- Simplified menu definitions to allow minimised RAM footprint. Extensive use of PROGMEM for static structures.
- Input and display devices controller through user code and callbacks from the library.
- Input methods available for 
  + Boolean (Y/N) values
  + List selection
  + 8/16/32 bit signed integers

- \subpage pageRevisionHistory
- \subpage pageCopyright

\page pageCopyright Copyright
Copyright
---------
Copyright (C) 2017 Marco Colli. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\page pageRevisionHistory Revision History
Revision History
----------------
xxx 2017 version 1.0
- First implementation
*/
#include <Arduino.h>

/**
 * \file
 * \brief Main header file for class definition of the MD_Menu library
 */

// Label size definitions
const uint8_t HEADER_LABEL_SIZE = 16;   ///< Displayed length of a menu header label
const uint8_t ITEM_LABEL_SIZE = 16;     ///< Displayed length of a menu item label
const uint8_t INPUT_LABEL_SIZE = 16;    ///< Displayed length of an input item label

// Miscellaneous defines
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))  ///< Generic macro for obtaining number of elements of an array
const uint8_t MNU_STACK_SIZE = 4;       ///< Maximum menu 'depth'. Starting (root) menu occupies first level.

/**
 * Core object for the MD_Menu library
 */
class MD_Menu
{
public:
  //--------------------------------------------------------------
  /** \name Enumerated values and Typedefs.
  * @{
  */
  /** 
  * Common Action Id type
  * 
  * Record id numbers link the different parts of the menu together.
  * typedef this to make it easier to change to a different type in
  * future if required. Note that id -1 is used to indicate error or no
  * id, so value must be signed.
  */
  typedef int8_t mnuId_t;

  /**
  * Return values for the user input handler
  *
  * The menu navigation keys are implemented by user code that must
  * return one of these definesd types when it is invoked. The menu
  * navigation in the libray and data input is fully controlled by
  * this returned value.
  */
  enum userNavAction_t
  {
    NAV_NULL,  ///< There was no current selection to process
    NAV_INC,   ///< INCREMENT. Move to the next menu item or increment a value.
    NAV_DEC,   ///< DECREMENT. Move to the previous menu item or decrement a value.
    NAV_SEL,   ///< SELECT the current menu item or confirm a new value.
    NAV_ESC,   ///< ESCAPE from current menu or abandon editing a value (remains unchanged).
  };

  /**
  * User input function prototype
  *
  * The user input function must handle the physical user interface
  * (eg, switches, rotary encoder) and return one of the userNavAction_t
  * enumerated types to trigger the next menu action.
  * The user funtion can also specify the incremental change quantity that 
  * to be applied for INC and DEC actions when editing a numeric variable
  * (default is 1) by changing the incDelta variable.
  */
  typedef userNavAction_t(*cbUserNav)(uint16_t &incDelta);

  /**
  * Request values for user display handler
  *
  * The display handler will receive requests that tell it what
  * needs to be done. The display must implement appropriate actions
  * on the display device to carry out the request.
  */
  enum userDisplayAction_t
  {
    DISP_CLEAR, ///< Clear the display
    DISP_L0,    ///< Display the data provided in line 0 (first line). For single line displays, this should be ignored.
    DISP_L1,    ///< Display the data provided in line 1 (second line). This must always be implemented.
  };

  /**
  * User input function prototype
  *
  * The user input function must handle the physical user interface
  * (eg, switches, rotary encoder) and return one of the userNavAction_t
  * enumerated types to trigger the next menu action.
  */
  typedef bool(*cbUserDisplay)(userDisplayAction_t action, char *msg);

  /**
  * Menu input type enumerated type specification.
  *
  * Used to define the the type input action a for variable menu
  * item so that it can be appropriately processed by the library.
  */
  enum inputAction_t
  {
    INP_LIST,   ///< The item is for selection from a defined list of values
    INP_BOOL,   ///< The item is for input of a boolean variable (Y/N)
    INP_INT8,   ///< The item is for input of an 8 bit unsigned integer
    INP_INT16,  ///< The item is for input of an 16 bit unsigned integer
    INP_INT32,  ///< The item is for input of an 32 bit unsigned integer
    INP_RUN,    ///< The item will run a user function
  };

  /**
  * Data input/output function prototype
  *
  * This user function must handle the get/set of the inpout value
  * currently being handled by the menu.
  * When bGet is true, the function must return the pointer to the
  * data identified by the ID and index
  */
  typedef void*(*cbValueRequest)(mnuId_t id, uint8_t idx, bool bGet);

  /**
  * Input field defintion
  *
  * Defines the data value for input from the user. Each definition
  * contains enough data for the data collection to be managed by the
  * library.
  */
  typedef struct mnuInput_t
  {
    mnuId_t id;            ///< Identifier for this item
    uint8_t idx;           ///< index for this value - id and idx make this value unique
    char    label[INPUT_LABEL_SIZE + 1]; ///< Label for this menu item
    inputAction_t action;  ///< Type of action required for this value
    cbValueRequest cbVR;   ///< Callback function to get/set the value
    uint8_t fieldWidth;    ///< Width of the displayed field betwene delimiters
    int32_t range[2];      ///< min/max values an integer
    uint8_t base;          ///< number base for display (2 through 16)
    const char *pList;     ///< pointer to list string
  };

  /**
  * Menu input type enumerated type specification.
  *
  * Used to define the the type input action a for variable menu
  * item so that it can be appropriately processed by the library.
  */
  enum mnuAction_t
  {
    MNU_MENU,   ///< The item is for selection of a new menu
    MNU_INPUT,  ///< The item is for input of a value
  };

  /**
  * Menu item definition
  *
  * Defines the item data for the menu. The items are conceptually 'child'
  * records of a menu header item, where a continuous range form part
  * of the menu.
  */
  typedef struct mnuItem_t
  {
    mnuId_t id;            ///< Identifier for this item
    char    label[ITEM_LABEL_SIZE + 1]; ///< Label for this menu item
    mnuAction_t action;    ///< Selecting this item does this action
    mnuId_t actionId;      ///< Next menu or input field Id
  };

  /**
  * Menu header definition
  *
  * Defines the header data for a menu. The items are defined separately.
  * This data structure encodes the range of menu items that form part
  * of the menu.
  */
  typedef struct mnuHeader_t
  {
    mnuId_t id;          ///< Identifier for this item
    char    label[HEADER_LABEL_SIZE + 1]; ///< Label for this menu item
    mnuId_t idItmStart;  ///< Start item number for menu
    mnuId_t idItmEnd;    ///< End item number for the menu
    mnuId_t idItmCurr;   ///< Current item being processed
  };

  /** @} */
  //--------------------------------------------------------------
  /** \name Class constructor and destructor.
  * @{
  */
  /**
   * Class Constructor.
   *
   * Instantiate a new instance of the class. The parameters passed define the
   * data structures defining the menu items and function callbacks required for
   * the library to interact with user code.
   *
   * \param cbNav		navigation user callback function
   * \param cbDisp  display user callback function
   * \param mnuHdr  address of the menu headers data table
   * \param mnuHdrCount number of elements in the header table
   * \param mnuItm  address of the menu items data table
   * \param mnuItmCount number of elements in the item table
   * \param mnuInp  address of the input definitions data table
   * \param mnuInpCount number of elements in the input definitions table
   */
  MD_Menu(cbUserNav cbNav, cbUserDisplay cbDisp,
    mnuHeader_t *mnuHdr, uint8_t mnuHdrCount,
    mnuItem_t *mnuItm, uint8_t mnuItmCount,
    mnuInput_t *mnuInp, uint8_t mnuInpCount);

  /**
   * Class Destructor.
   *
   * Released allocated memory and does the necessary to clean up once the queue is
   * no longer required.
   */
  ~MD_Menu(void) {};

  /** @} */
  //--------------------------------------------------------------
  /** \name Methods for core object control.
  * @{
  */
  /**
  * Initialize the object.
  *
  * Initialise the object data. This needs to be called during setup() to initialise new
  * data for the class that cannot be done during the object creation.
  */
  void begin(void) {};

  /**
   * Run the menu.
   *
   * This should be called each time through the loop() function.
   * The optional parameter should be set to true when the menu display needs
   * to start (or restart) and false (or omitted) for normal running.
   * The function will coordinate the user callbacks to obtain input and
   * display the menu, as needed.
   *
   * \param bStart Set to true is the menu needs to be started; defaults to false if not specified
   * \return true if the menu is still running, false otherwise
   */
  bool runMenu(bool bStart = false);

  /**
  * Check if library is running a menu.
  *
  * Returns boolean with the running status.
  *
  * \return true if running menu, false otherwise
  */
  bool isInMenu(void) { return(_inMenu); };

  /**
  * Check if library is editing a field.
  *
  * Returns boolean with the edit status.
  *
  * \return true if editing field, false otherwise
  */
  bool isInEdit(void) { return(_inEdit); };

  /** @} */
  //--------------------------------------------------------------
  /** \name Support methods.
  * @{
  */
  /**
  * Reste the menu.
  *
  * Change the current menu state to be not running and reset all menu
  * conditions to start state.
  */
  void reset(void) { _inMenu = _inEdit = false; _currMenu = 0; };

  /**
  * Set the menu wrap option.
  *
  * Set the menu wrap option on or off. When set on, reaching the end 
  * of the menu will arap around to the start of the menu. Simialrly, 
  * reaching the end will restart from the beginning.
  * Default is set to no wrap.
  *
  * \param bSet true to set the option, false to un-set the option (default)
  */
  void setMenuWrap(bool bSet) { _wrapMenu = bSet; };

  /**
  * Set the user navigation callback function.
  *
  * Replace the current callback function with the new function.
  * Set to NULL to disable the functionality (not recommended)
  *
  * \param cbNav the callback function pointer.
  */
  void setUserNavCallback(cbUserNav cbNav) { if (cbNav != nullptr) _cbNav = cbNav; };

  /**
  * Set the user display callback function.
  *
  * Replace the current callback function with the new function.
  * Set to NULL to disable the functionality (not recommended)
  *
  * \param cbDisp the callback function pointer.
  */
  void setUserDisplayCallback(cbUserDisplay cbDisp) { if (cbDisp != nullptr) _cbDisp = cbDisp; };

  /** @} */
private:
  // initialisation parameters and data tables
  cbUserNav _cbNav;       ///< User navigation function
  cbUserDisplay _cbDisp;  ///< User display function

  mnuHeader_t *_mnuHdr;   ///< Menu header table
  uint8_t _mnuHdrCount;   ///< Number of items in the header table
  mnuItem_t *_mnuItm;     ///< Menu item table
  uint8_t _mnuItmCount;   ///< Number of items in the item table
  mnuInput_t *_mnuInp;    ///< Input item table
  uint8_t _mnuInpCount;   ///< Number of items in the input table

  // Status valuesand global flags
  bool _inMenu;           ///< Flag is true if the library is currently running a menu
  bool _inEdit;           ///< Flag is true if library is currently editing a value
  bool _wrapMenu;         ///< Flag is true to wrap around ends of menu and list selections

  // Input editing buffers and tracking
  void *_pValue;          ///< Pointer to the user value being edited
  bool  _bValue;          ///< Copy of boolean value being edited
  int32_t _iValue;        ///< Copy of the integer/list index value being edited

  // static buffers for find functions, keep accessible copies of data in PROGMEM
  uint8_t _currMenu;                   ///< Index of current menu displayed in the stack
  mnuHeader_t _mnuStack[MNU_STACK_SIZE];  ///< Stacked trail of menus being executed
  mnuInput_t _mnuBufInput;             ///< menu input buffer for load function
  mnuItem_t _mnuBufItem;               ///< menu item buffer for load function

  // Private functions
  void loadMenu(mnuId_t id = -1);      ///< find the menu header with the specified ID
  mnuItem_t *loadItem(mnuId_t id);     ///< find the menu item with the specified ID
  mnuInput_t *loadInput(mnuId_t id);   ///< find the input item with the specified ID
  uint8_t listCount(PROGMEM char *p);  ///< count the items in a list selection string 
  char *listItem(PROGMEM char *p, uint8_t idx, char *buf, uint8_t bufLen);  ///< extract the idx'th item from the list selection string
  void MD_Menu::strPreamble(char *psz, mnuInput_t *mInp);  ///< format a preamble to the a variable display
  void MD_Menu::strPostamble(char *psz, mnuInput_t *mInp); ///< attach a postamble to a variable display
  
  void handleMenu(bool bNew = false);  ///< handling display menu seitems and navigation
  void handleInput(bool bNew = false); ///< handling user input to edit values

  // Process the different types of input requests
  // All return true when edit changes are finished (SELECT or ESCAPE).
  bool processList(userNavAction_t nav, mnuInput_t *mInp);
  bool processBool(userNavAction_t nav, mnuInput_t *mInp);
  bool processInt(userNavAction_t nav, mnuInput_t *mInp, uint16_t incDelta);
  bool processRun(userNavAction_t nav, mnuInput_t *mInp);
};

