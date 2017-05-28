// Implementation file for MD_Menu library
//
// See the main header file MD_Menu.h for more information

#include <MD_Menu_lib.h>
#include <MD_Menu.h>

/**
 * \file
 * \brief Main code file for MD_Menu library
 */
MD_Menu::MD_Menu(cbUserNav cbNav, cbUserDisplay cbDisp,
                mnuHeader_t *mnuHdr, uint8_t mnuHdrCount,
                mnuItem_t *mnuItm, uint8_t mnuItmCount,
                mnuInput_t *mnuInp, uint8_t mnuInpCount) :
                _inMenu(false), _currMenu(0), _inEdit(false),
                _mnuHdr(mnuHdr), _mnuHdrCount(mnuHdrCount),
                _mnuItm(mnuItm), _mnuItmCount(mnuItmCount),
                _mnuInp(mnuInp), _mnuInpCount(mnuInpCount)
{
  setUserNavCallback(cbNav);
  setUserDisplayCallback(cbDisp);
  setMenuWrap(false);
}

void MD_Menu::loadMenu(int16_t id)
// Load a menu header definition to the current stack position
{
  uint8_t idx = 0;
  mnuHeader_t mh;

  if (id != -1)   // look for a menu with that id and load it up
  {
    for (uint8_t i = 0; i < _mnuHdrCount; i++)
    {
      memcpy_P(&mh, &_mnuHdr[i], sizeof(mnuHeader_t));
      if (mh.id == id)
      {
        idx = i;  // found it!
        break;
      }
    }
  }

  // we either found the item or we will load the first one by default
  memcpy_P(&_mnuStack[_currMenu], &_mnuHdr[idx], sizeof(mnuHeader_t));
}

MD_Menu::mnuItem_t* MD_Menu::loadItem(int16_t id)
// Find a copy the input item to the private class accessible buffer
{
  for (uint8_t i = 0; i < _mnuItmCount; i++)
  {
    memcpy_P(&_mnuBufItem, &_mnuItm[i], sizeof(mnuItem_t));
    if (_mnuBufItem.id == id)
      return(&_mnuBufItem);
  }

  return(nullptr);
}

MD_Menu::mnuInput_t* MD_Menu::loadInput(int16_t id)
// Find a copy the input item to the private class accessible buffer
{
  for (uint8_t i = 0; i < _mnuItmCount; i++)
  {
    memcpy_P(&_mnuBufInput, &_mnuInp[i], sizeof(mnuInput_t));
    if (_mnuBufInput.id == id)
      return(&_mnuBufInput);
  }

  return(nullptr);
}

uint8_t MD_Menu::listCount(PROGMEM char *p)
{
  uint8_t count = 0;

  if (p != nullptr)
  {
    uint8_t len;

    do
    {
      len = strlen_P(p);
      p += len + 1;
      if (len != 0) count++;
    } while (len != 0);
  }

  return(count);
}

char *MD_Menu::listItem(PROGMEM char *p, uint8_t idx, char *buf, uint8_t bufLen)
{
  memset(buf, '\0', bufLen);

  if (p != nullptr)
  {
    for (uint8_t i = 0; i < idx; i++)
      p += strlen_P(p) + 1;

    strncpy_P(buf, p, bufLen-1);
  }

  return(buf);
}

bool MD_Menu::processList(userNavAction_t nav, mnuInput_t *mInp)
{
  bool endFlag = false;
  bool update = false;
  uint8_t listSize;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the display
  {
    if (listCount(mInp->pList) == 0)
    {
      MD_PRINTS("\nEmpty list selection!");
      endFlag = true;
    }
    else
    {
      _pValue = mInp->cbVR(mInp->id, mInp->idx, true);

      if (_pValue == nullptr)
      {
        MD_PRINTS("\nList cbVR(GET) == NULL!");
        endFlag = true;
      }
      else
      {
        _iValue = *((uint8_t*)_pValue);
        update = true;
      }
    }
  }
  break;

  case NAV_DEC:
    listSize = listCount(mInp->pList);
    if (_iValue > 0)
    {
      _iValue--;
      update = true;
    }
    else if (_iValue == 0 && _wrapMenu)
    {
      _iValue = listSize-1;
      update = true;
    }
    break;

  case NAV_INC:
    listSize = listCount(mInp->pList);
    if (_iValue < listSize - 1)
    {
      _iValue++;
      update = true;
    }
    else if (_iValue == listSize - 1 && _wrapMenu)
    {
      _iValue = 0;
      update = true;
    }
    break;

  case NAV_SEL:
    *((uint8_t*)_pValue) = _iValue;
    mInp->cbVR(mInp->id, mInp->idx, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    // Kuldge a format string specifier as a variable string as the Arduino
    // functions don't allow the "%*ld" type format but require and actual
    // number instead of the * specifier.
    char szFmt[15];
    char szItem[mInp->fieldWidth + 1];
    char sz[strlen(mInp->label) + strlen(FLD_PROMPT) + strlen(FLD_DELIM_L) + sizeof(szItem) + strlen(FLD_DELIM_R) + 1];

    snprintf(szFmt, sizeof(szFmt), "%s%d%s", "%s%s%s%", mInp->fieldWidth, "s%s");
    snprintf(sz, sizeof(sz), szFmt, mInp->label, FLD_PROMPT, FLD_DELIM_L, listItem(mInp->pList, _iValue, szItem, sizeof(szItem)), FLD_DELIM_R);
    _cbDisp(DISP_L1, sz);
  }

  return(endFlag);
}

bool MD_Menu::processBool(userNavAction_t nav, mnuInput_t *mInp)
{
  bool endFlag = false;
  bool update = false;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the display
    {
      _pValue = mInp->cbVR(mInp->id, mInp->idx, true);

      if (_pValue == nullptr)
      {
        MD_PRINTS("\nBool cbVR(GET) == NULL!");
        endFlag = true;
      }
      else
      {
        _bValue = *((bool *)_pValue);
        update = true;
      }
    }
    break;

  case NAV_INC:
  case NAV_DEC:
    _bValue = !_bValue;
    update = true;
    break;

  case NAV_SEL:
    *((bool *)_pValue) = _bValue;
    mInp->cbVR(mInp->id, mInp->idx, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    char sz[strlen(mInp->label) + strlen(FLD_PROMPT) + strlen(FLD_DELIM_L) + strlen(INP_BOOL_T) + strlen(FLD_DELIM_R) + 1];

    snprintf(sz, sizeof(sz), "%s%s%s%s%s", mInp->label, FLD_PROMPT, FLD_DELIM_L, _bValue ? INP_BOOL_T : INP_BOOL_F, FLD_DELIM_R);
    _cbDisp(DISP_L1, sz);
  }

  return(endFlag);
}

bool MD_Menu::processInt(userNavAction_t nav, mnuInput_t *mInp, uint16_t incDelta)
{
  bool endFlag = false;
  bool update = false;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the display
    {
      _pValue = mInp->cbVR(mInp->id, mInp->idx, true);

      if (_pValue == nullptr)
      {
        MD_PRINTS("\nInt cbVR(GET) == NULL!");
        endFlag = true;
      }
      else
      {
        switch (mInp->action)
        {
        case INP_INT8:  _iValue = *((int8_t*)_pValue);  break;
        case INP_INT16: _iValue = *((int16_t*)_pValue); break;
        case INP_INT32: _iValue = *((int32_t*)_pValue); break;
        }
        update = true;
      }
    }
    break;

  case NAV_INC:
    if (_iValue + incDelta < mInp->range[1])
      _iValue += incDelta;
    else
      _iValue = mInp->range[1];
    update = true;
    break;

  case NAV_DEC:
    if (_iValue - incDelta > mInp->range[0])
      _iValue -= incDelta;
    else
      _iValue = mInp->range[0];
    update = true;
    break;

  case NAV_SEL:
    switch (mInp->action)
    {
    case INP_INT8:  *((int8_t*)_pValue) = _iValue;  break;
    case INP_INT16: *((int16_t*)_pValue) = _iValue; break;
    case INP_INT32: *((int32_t*)_pValue) = _iValue; break;
    }
    mInp->cbVR(mInp->id, mInp->idx, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    // Kuldge a format string specifier as a variable string as the Arduino
    // functions don't allow the "%*ld" type format but require and actual
    // number instead of the * specifier.
    char szFmt[15];
    char sz[strlen(mInp->label) + strlen(FLD_PROMPT) + strlen(FLD_DELIM_L) + mInp->fieldWidth + strlen(FLD_DELIM_R) + 1];

    snprintf(szFmt, sizeof(szFmt), "%s%d%s", "%s%s%s%", mInp->fieldWidth, "ld%s");
    snprintf(sz, sizeof(sz), szFmt, mInp->label, FLD_PROMPT, FLD_DELIM_L, _iValue, FLD_DELIM_R);
    _cbDisp(DISP_L1, sz);
  }

  return(endFlag);
}

bool MD_Menu::processRun(userNavAction_t nav, mnuInput_t *mInp)
// Processing a Run user code input field.
// When the field is selected, run the user variable code. For all other
// input do nothing
// Return true when the element has run user code.
{
  if (nav == NAV_NULL)    // initialise the display
  {
    char sz[strlen(mInp->label) + strlen(FLD_DELIM_L) + strlen(FLD_DELIM_R) + 1];

    snprintf(sz, sizeof(sz), "%s%s%s", FLD_DELIM_L, mInp->label, FLD_DELIM_R);
    _cbDisp(DISP_L1, sz);
  }
  else if (nav == NAV_SEL)
  {
    mInp->cbVR(mInp->id, mInp->idx, false);
    return(true);
  }

  return(false);
}

void MD_Menu::handleInput(bool bNew)
{
  bool ended = false;
  uint16_t incDelta = 1;
  mnuItem_t *mi;
  mnuInput_t *me;

  if (bNew)
  {
    _cbDisp(DISP_CLEAR, nullptr);
    mi = loadItem(_mnuStack[_currMenu].idItmCurr);
    _cbDisp(DISP_L0, mi->label);
    me = loadInput(mi->actionId);

    switch (me->action)
    {
    case INP_LIST: ended = processList(NAV_NULL, me); break;
    case INP_BOOL: ended = processBool(NAV_NULL, me); break;
    case INP_INT8:
    case INP_INT16:
    case INP_INT32: ended = processInt(NAV_NULL, me, incDelta); break;
    case INP_RUN: ended = processRun(NAV_NULL, me); break;
    }

    _inEdit = true;
  }
  else
  {
    userNavAction_t nav = _cbNav(incDelta);

    if (nav == NAV_ESC)
      ended = true;
    else if (nav != NAV_NULL)
    {
      mi = loadItem(_mnuStack[_currMenu].idItmCurr);
      me = loadInput(mi->actionId);

      switch (me->action)
      {
      case INP_LIST: ended = processList(nav, me); break;
      case INP_BOOL: ended = processBool(nav, me); break;
      case INP_INT8:
      case INP_INT16:
      case INP_INT32: ended = processInt(nav, me, incDelta); break;
      case INP_RUN: ended = processRun(nav, me); break;
      }
    }
  }

  if (ended)
  {
    _inEdit = false;
    handleMenu(true);
  }
}

void MD_Menu::handleMenu(bool bNew)
{
  bool update = false;

  if (bNew)
  {
    _cbDisp(DISP_CLEAR, nullptr);
    _cbDisp(DISP_L0, _mnuStack[_currMenu].label);
    if (_mnuStack[_currMenu].idItmCurr == 0)
      _mnuStack[_currMenu].idItmCurr = _mnuStack[_currMenu].idItmStart;
    update = true;
  }
  else
  {
    uint16_t incDelta = 1;

    switch (_cbNav(incDelta))
    {
    case NAV_DEC:
      if (_mnuStack[_currMenu].idItmCurr != _mnuStack[_currMenu].idItmStart)
      {
        _mnuStack[_currMenu].idItmCurr--;
        update = true;
      }
      else if (_wrapMenu)
      {
        _mnuStack[_currMenu].idItmCurr = _mnuStack[_currMenu].idItmEnd;
        update = true;
      }
      break;

    case NAV_INC:
      if (_mnuStack[_currMenu].idItmCurr != _mnuStack[_currMenu].idItmEnd)
      {
        _mnuStack[_currMenu].idItmCurr++;
        update = true;
      }
      else if (_wrapMenu)
      {
        _mnuStack[_currMenu].idItmCurr = _mnuStack[_currMenu].idItmStart;
        update = true;
      }
      break;

    case NAV_SEL:
      {
        mnuItem_t *mi = loadItem(_mnuStack[_currMenu].idItmCurr);

        switch (mi->action)
        {
        case MNU_MENU:
          if (_currMenu < MNU_STACK_SIZE - 1)
          {
            _currMenu++;
            loadMenu(mi->actionId);
            handleMenu(true);
          }
          break;

        case MNU_INPUT:
          if (loadInput(mi->actionId) != nullptr)
            handleInput(true);
          else
            MD_PRINTS("\nInput definition not found");
          break;
        }
      }
      break;

    case NAV_ESC:
      if (_currMenu == 0)
        _inMenu = false;
      else
      {
        _currMenu--;
        handleMenu(true);  // just one level of recursion;
      }
      break;
    }
  }

  if (update) // update L1 on the display
  {
    mnuItem_t *mi = loadItem(_mnuStack[_currMenu].idItmCurr);

    if (mi != nullptr)
    {
      char sz[strlen(mi->label) + strlen(MNU_DELIM_L) + strlen(MNU_DELIM_R) + 1]; // temporary string

      snprintf(sz, sizeof(sz), "%s%s%s", MNU_DELIM_L, mi->label, MNU_DELIM_R);
      _cbDisp(DISP_L1, sz);
    }
  }
}

bool MD_Menu::runMenu(bool bStart)
{
  // check if we need to process anything
  if (!_inMenu && !bStart)
    return(false);

  _inMenu = true; // we now are running a menu!

  if (bStart)   // start the menu again
  {
    MD_PRINTS("\n-> Starting menu");
    _currMenu = 0;
    loadMenu();
    handleMenu(true);
  }
  else    // keep running current menu
  {
    if (_inEdit)
      handleInput();
    else
      handleMenu();

    if (!_inMenu)
    {
      _cbDisp(DISP_CLEAR, nullptr);
      MD_PRINTS("\n-> Ending Menu");
    }
  }

  return(_inMenu);
}
