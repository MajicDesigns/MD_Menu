// Implementation file for MD_Menu library
//
// See the main header file MD_Menu.h for more information

#include <MD_Menu.h>
#include <MD_Menu_lib.h>

/**
 * \file
 * \brief Main code file for MD_Menu library
 */
MD_Menu::MD_Menu(cbUserNav cbNav, cbUserDisplay cbDisp,
                const mnuHeader_t *mnuHdr, uint8_t mnuHdrCount,
                const mnuItem_t *mnuItm, uint8_t mnuItmCount,
                const mnuInput_t *mnuInp, uint8_t mnuInpCount) :
                _options(0), _timeout(0),
                _mnuHdr(mnuHdr), _mnuHdrCount(mnuHdrCount),
                _mnuItm(mnuItm), _mnuItmCount(mnuItmCount),
                _mnuInp(mnuInp), _mnuInpCount(mnuInpCount)
{
  setUserNavCallback(cbNav);
  setUserDisplayCallback(cbDisp);
}

void MD_Menu::reset(void)
{ 
  CLEAR_FLAG(F_INMENU); 
  CLEAR_FLAG(F_INEDIT); 
  _currMenu = 0; 
};

void MD_Menu::setUserNavCallback(cbUserNav cbNav) 
{ 
  if (cbNav != nullptr) 
    _cbNav = cbNav; 
};

void MD_Menu::setUserDisplayCallback(cbUserDisplay cbDisp) 
{ 
  if (cbDisp != nullptr) 
    _cbDisp = cbDisp; 
};

// Status and options
bool MD_Menu::isInMenu(void) { return(TEST_FLAG(F_INMENU)); };
bool MD_Menu::isInEdit(void) { return(TEST_FLAG(F_INEDIT)); };
void MD_Menu::setMenuWrap(bool bSet)  { if (bSet) { SET_FLAG(F_MENUWRAP); } else { CLEAR_FLAG(F_MENUWRAP); } };
void MD_Menu::setAutoStart(bool bSet) { if (bSet) { SET_FLAG(F_AUTOSTART); } else { CLEAR_FLAG(F_AUTOSTART); } };
void MD_Menu::setTimeout(uint32_t t) { _timeout = t; };

void MD_Menu::timerStart(void)
{
  _timeLastKey = millis();
}

void MD_Menu::timerCheck(void)
{
  if (_timeout == 0) return;    // not set

  if (millis() - _timeLastKey >= _timeout)
  {
    MD_PRINTS("\ntimerCheck: Menu timeout");
    reset();
  }
}

void MD_Menu::loadMenu(mnuId_t id)
// Load a menu header definition to the current stack position
{
  mnuId_t idx = 0;
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

MD_Menu::mnuItem_t* MD_Menu::loadItem(mnuId_t id)
// Find a copy the input item to the class private buffer
{
  for (uint8_t i = 0; i < _mnuItmCount; i++)
  {
    memcpy_P(&_mnuBufItem, &_mnuItm[i], sizeof(mnuItem_t));
    if (_mnuBufItem.id == id)
      return(&_mnuBufItem);
  }

  return(nullptr);
}

MD_Menu::mnuInput_t* MD_Menu::loadInput(mnuId_t id)
// Find a copy the input item to the class private buffer
{
  for (uint8_t i = 0; i < _mnuItmCount; i++)
  {
    memcpy_P(&_mnuBufInput, &_mnuInp[i], sizeof(mnuInput_t));
    if (_mnuBufInput.id == id)
      return(&_mnuBufInput);
  }

  return(nullptr);
}

uint8_t MD_Menu::getListCount(const char *p)
// Return a count of the items in the list
{
  uint8_t count = 0;
  char c;

  if (p != nullptr)
  {
    if (pgm_read_byte(p) != '\0')   // not empty list
    {
      do
      {
        c = pgm_read_byte(p++);
        if (c == LIST_SEPARATOR) count++;
      } while (c != '\0');

      // if the list is not empty, then the last element is 
      // terminated by '\0' and we have not counted it, so 
      // add it now
      count++;
    }
  }

  return(count);
}

char *MD_Menu::getListItem(const char *p, uint8_t idx, char *buf, uint8_t bufLen)
// Find the idx'th item in the list and return in fixed width, padded
// with trailing spaces. 
{
  // fill the buffer with '\0' so we know that string will
  // always be terminted within this buffer
  memset(buf, '\0', bufLen);

  if (p != nullptr)
  {
    char *psz;
    char c;

    // skip items before the one we want
    while (idx > 0)
    {
      do
        c = pgm_read_byte(p++);
      while (c != '\0' && c != LIST_SEPARATOR);
      idx--;
    }

    // copy the next item over
    psz = buf;
    for (uint8_t i = 0; i < bufLen - 1; psz++, i++)
    {
      *psz = pgm_read_byte(p++);
      if (*psz == LIST_SEPARATOR) *psz = '\0';
      if (*psz == '\0') break;
    }

    // Pad out any short string with trailing spaces
    // The trailing buffer is already filled with '\0'
    while (strlen(buf) < bufLen - 1)
      *psz++ = ' ';
  }

  return(buf);
}

void MD_Menu::strPreamble(char *psz, mnuInput_t *mInp)
// Create the start to a variable CB_DISP
{
  strcpy(psz, mInp->label);
  strcat(psz, FLD_PROMPT);
  strcat(psz, FLD_DELIM_L);
}

void MD_Menu::strPostamble(char *psz, mnuInput_t *mInp)
// Attach the tail of the variable CB_DISP
{
  strcat(psz, FLD_DELIM_R);
}

bool MD_Menu::processList(userNavAction_t nav, mnuInput_t *mInp, bool rtfb)
// Processing for List based input
// Return true when the edit cycle is completed
{
  bool endFlag = false;
  bool update = false;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the CB_DISP
  {
    uint8_t size = getListCount(mInp->pList);

    if (size == 0)
    {
      MD_PRINTS("\nEmpty list selection!");
      endFlag = true;
    }
    else
    {
      _pValue = mInp->cbVR(mInp->id, true);

      if (_pValue == nullptr)
      {
        MD_PRINTS("\nList cbVR(GET) == NULL!");
        endFlag = true;
      }
      else
      {
        _V.value = _pValue->value;
        if (_V.value >= size)   // index set incorrectly
          _V.value = 0;
        update = true;
      }
    }
  }
  break;

  case NAV_DEC:
    {
      uint8_t listSize = getListCount(mInp->pList);
      if (_V.value > 0)
      {
        _V.value--;
        update = true;
      }
      else if (_V.value == 0 && TEST_FLAG(F_MENUWRAP))
      {
        _V.value = listSize - 1;
        update = true;
      }
    }
    break;

  case NAV_INC:
    {
      uint8_t listSize = getListCount(mInp->pList);

      if (_V.value < listSize - 1)
      {
        _V.value++;
        update = true;
      }
      else if (_V.value == listSize - 1 && TEST_FLAG(F_MENUWRAP))
      {
        _V.value = 0;
        update = true;
      }
    }
    break;

  case NAV_SEL:
    _pValue->value = _V.value;
    mInp->cbVR(mInp->id, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    char szItem[mInp->fieldWidth + 1];
    char sz[INP_PRE_SIZE(mInp) + sizeof(szItem) + INP_POST_SIZE(mInp) + 1];

    strPreamble(sz, mInp);
    strcat(sz, getListItem(mInp->pList, _V.value, szItem, sizeof(szItem)));
    strPostamble(sz, mInp);

    _cbDisp(DISP_L1, sz);

    // real time feedback needed
    if (rtfb)
    {
      _pValue->value = _V.value;
      mInp->cbVR(mInp->id, false);
    }
  }

  return(endFlag);
}

bool MD_Menu::processBool(userNavAction_t nav, mnuInput_t *mInp, bool rtfb)
// Processing for Boolean (true/false) value input
// Return true when the edit cycle is completed
{
  bool endFlag = false;
  bool update = false;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the CB_DISP
    {
      _pValue = mInp->cbVR(mInp->id, true);

      if (_pValue == nullptr)
      {
        MD_PRINTS("\nBool cbVR(GET) == NULL!");
        endFlag = true;
      }
      else
      {
        _V.value = _pValue->value;
        update = true;
      }
    }
    break;

  case NAV_INC:
  case NAV_DEC:
    _V.value = !_V.value;
    update = true;
    break;

  case NAV_SEL:
    _pValue->value = _V.value;
    mInp->cbVR(mInp->id, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    char sz[INP_PRE_SIZE(mInp) + strlen(INP_BOOL_T) + INP_POST_SIZE(mInp) + 1];

    strPreamble(sz, mInp);
    strcat(sz, _V.value ? INP_BOOL_T : INP_BOOL_F);
    strPostamble(sz, mInp);

    _cbDisp(DISP_L1, sz);

    // real time feedback needed
    if (rtfb)
    {
      _pValue->value = _V.value;
      mInp->cbVR(mInp->id, false);
    }
  }

  return(endFlag);
}

char *ltostr(char *buf, uint8_t bufLen, int32_t v, uint8_t base, bool leadZero = false)
// Convert a long to a string right justified with leading spaces
// in the base specified (up to 16).
{
  char *ptr = buf + bufLen - 1; // the last element of the buffer
  bool sign = (v < 0);
  uint32_t t = 0, res = 0;
  uint32_t value = (sign ? -v : v);

  if (buf == nullptr) return(nullptr);

  *ptr = '\0'; // terminate the string as we will be moving backwards

  // now successively deal with the remainder digit 
  // until we have value == 0
  do
  {
    t = value / base;
    res = value - (base * t);
    if (res < 10)
      *--ptr = '0' + res;
    else if ((res >= 10) && (res < 16))
      *--ptr = 'A' - 10 + res;
    value = t;
  } while (value != 0 && ptr != buf);

  if (ptr != buf)      // if there is still space
  {
    if (sign) *--ptr = '-';  // put in the sign ...
    while (ptr != buf)       // ... and pad with leading character
      *--ptr = (leadZero ? '0' : ' ');
  }
  else if (value != 0 || sign) // insufficient space - show this
      *ptr = INP_NUMERIC_OFLOW;

  return(buf);
}

bool MD_Menu::processInt(userNavAction_t nav, mnuInput_t *mInp, bool rtfb, uint16_t incDelta)
// Processing for Integer (all sizes) value input
// Return true when the edit cycle is completed
{
  bool endFlag = false;
  bool update = false;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the CB_DISP
    {
      _pValue = mInp->cbVR(mInp->id, true);

      if (_pValue == nullptr)
      {
        MD_PRINTS("\nInt cbVR(GET) == NULL!");
        endFlag = true;
      }
      else
      {
        _V.value = _pValue->value;
        update = true;
      }
    }
    break;

  case NAV_INC:
    if (_V.value + incDelta < mInp->range[1].value)
      _V.value += incDelta;
    else
      _V.value = mInp->range[1].value;
    update = true;
    break;

  case NAV_DEC:
    if (_V.value - incDelta > mInp->range[0].value)
      _V.value -= incDelta;
    else
      _V.value = mInp->range[0].value;
    update = true;
    break;

  case NAV_SEL:
    _pValue->value = _V.value;
    mInp->cbVR(mInp->id, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    char sz[INP_PRE_SIZE(mInp) + mInp->fieldWidth + INP_POST_SIZE(mInp) + 1];

    strPreamble(sz, mInp);
    ltostr(sz + strlen(sz), mInp->fieldWidth + 1, _V.value, mInp->base);
    strPostamble(sz, mInp);

    _cbDisp(DISP_L1, sz);

    // real time feedback needed
    if (rtfb)
    {
      _pValue->value = _V.value;
      mInp->cbVR(mInp->id, false);
    }
  }

  return(endFlag);
}

bool MD_Menu::processFloat(userNavAction_t nav, mnuInput_t *mInp, bool rtfb, uint16_t incDelta)
// Processing for Floating number representation value input
// The number is actually a uint32, where the last FLOAT_DECIMALS digits are taken
// to be fractional part of the floating numer. For all purposes, this number is a long
// integer except when displayed. The base field is used as the increment for the decimal
// part in single fractional units of the decimal part.
// Return true when the edit cycle is completed
{
  bool endFlag = false;
  bool update = false;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the CB_DISP
  {
    _pValue = mInp->cbVR(mInp->id, true);

    if (_pValue == nullptr)
    {
      MD_PRINTS("\nFloat cbVR(GET) == NULL!");
      endFlag = true;
    }
    else
    {
      _V.value = _pValue->value;
      update = true;
    }
  }
  break;

  case NAV_INC:
    if (_V.value + (incDelta * mInp->base) < mInp->range[1].value)
      _V.value += (incDelta * mInp->base);
    else
      _V.value = mInp->range[1].value;
    update = true;
    break;

  case NAV_DEC:
    if (_V.value - (incDelta * mInp->base) > mInp->range[0].value)
      _V.value -= (incDelta * mInp->base);
    else
      _V.value = mInp->range[0].value;
    update = true;
    break;

  case NAV_SEL:
    _pValue->value = _V.value;
    mInp->cbVR(mInp->id, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    uint16_t divisor = 1;
    char sz[INP_PRE_SIZE(mInp) + mInp->fieldWidth + INP_POST_SIZE(mInp) + 1];

    for (uint8_t i = 0; i < FLOAT_DECIMALS; i++)
      divisor *= 10;

    strPreamble(sz, mInp);
    ltostr(sz + strlen(sz), mInp->fieldWidth - (FLOAT_DECIMALS + 1) + 1, _V.value / divisor, 10);
    sz[strlen(sz) + 1] = '\0';
    sz[strlen(sz)] = DECIMAL_POINT;
    ltostr(sz + strlen(sz), (FLOAT_DECIMALS + 1), abs(_V.value % divisor), 10, true);

    strPostamble(sz, mInp);

    _cbDisp(DISP_L1, sz);

    // real time feedback needed
    if (rtfb)
    {
      _pValue->value = _V.value;
      mInp->cbVR(mInp->id, false);
    }
  }

  return(endFlag);
}

bool MD_Menu::processEng(userNavAction_t nav, mnuInput_t *mInp, bool rtfb, uint16_t incDelta)
// Processing for Engineering Units number value input
// The number is actually a uint32, where the last ENGU_DECIMALS digits are taken
// to be fractional part of the floating numer. For all purposes, this number is a long
// integer except when displayed. The base field is used as the increment for the decimal
// part in single fractional units of the decimal part.
// Return true when the edit cycle is completed
{
  bool endFlag = false;
  bool update = false;

  switch (nav)
  {
  case NAV_NULL:    // this is to initialise the CB_DISP
  {
    _pValue = mInp->cbVR(mInp->id, true);

    if (_pValue == nullptr)
    {
      MD_PRINTS("\nEng cbVR(GET) == NULL!");
      endFlag = true;
    }
    else
    {
      _V.value = _pValue->value;
      _V.power = _pValue->power;
      if (_V.power < (-ENGU_RANGE)) _V.power = -ENGU_RANGE;
      if (_V.power > (ENGU_RANGE)) _V.power = ENGU_RANGE;
      update = true;
    }
  }
  break;

  case NAV_INC:
    if ((_V.value + (incDelta * mInp->base))/1000 < 1000)  // still within the same prefix range
    {
      if ((_V.power < mInp->range[1].power) ||
         (_V.power == mInp->range[1].power && _V.value + (incDelta * mInp->base) < mInp->range[1].value))
        _V.value += (incDelta * mInp->base);
      else
        _V.value = mInp->range[1].value;
    }
    else  // moved into the next range
    {
      _V.value += (incDelta * mInp->base);
      _V.value /= 1000;
      _V.power += 3;
      if ((_V.power > mInp->range[1].power) ||
         (_V.power == mInp->range[1].power && _V.value > mInp->range[1].value))
      {
        _V.value = mInp->range[1].value;
        _V.power = mInp->range[1].power;
      }
    }
    update = true;
    break;

  case NAV_DEC:
    if ((_V.value - (incDelta * mInp->base)) / 1000 > 0)  // still within the same prefix range
    {
      if ((_V.power > mInp->range[0].power) ||
         (_V.power == mInp->range[0].power && _V.value - (incDelta * mInp->base) > mInp->range[0].value))
        _V.value -= (incDelta * mInp->base);
      else
        _V.value = mInp->range[0].value;
    }
    else  // moved into the previous range
    {
      _V.value -= (incDelta * mInp->base);
      _V.value *= 1000;
      _V.power -= 3;
      if ((_V.power < mInp->range[0].power) ||
         (_V.power == mInp->range[0].power && _V.value < mInp->range[0].value))
      {
        _V.value = mInp->range[0].value;
        _V.power = mInp->range[0].power;
      }
    }
    update = true;
    break;

  case NAV_SEL:
    _pValue->value = _V.value;
    _pValue->power = _V.power;
    mInp->cbVR(mInp->id, false);
    endFlag = true;
    break;
  }

  if (update)
  {
    // The decimal units prefix are fitted to stay between 10^-ENGU_RANGE and 10^ENGU_RANGE 
    // as a symmetrical range. prefixes are atto(10^-18), femto(-15), pico(-12), nano(-9),
    // micro(-6), milli(-3), blank(0), kilo(3), Mega(6), Giga(9), Tera(12), Peta(15), Exa(18).
    static char unitsPrefix[] = { "afpnum kMGTPE" };

    uint16_t divisor = 1;
    char sz[INP_PRE_SIZE(mInp) + mInp->fieldWidth + INP_POST_SIZE(mInp) + 1 + strlen_P(mInp->pList) + 1];

    // work out the divisor we need
    for (uint8_t i = 0; i < ENGU_DECIMALS; i++)
      divisor *= 10;

    strPreamble(sz, mInp);
    ltostr(sz + strlen(sz), mInp->fieldWidth - (ENGU_DECIMALS + 1) + 1, _V.value / divisor, 10);
    sz[strlen(sz) + 1] = '\0';
    sz[strlen(sz)] = DECIMAL_POINT;
    ltostr(sz + strlen(sz), (ENGU_DECIMALS + 1), abs(_V.value % divisor), 10, true);

    strPostamble(sz, mInp);
    sz[strlen(sz) + 1] = '\0';
    sz[strlen(sz)] = unitsPrefix[(strlen(unitsPrefix) / 2) + (_V.power / 3)]; // milli, kilo, etc
    strcat_P(sz, mInp->pList);

    _cbDisp(DISP_L1, sz);

    // real time feedback needed
    if (rtfb)
    {
      _pValue->value = _V.value;
      mInp->cbVR(mInp->id, false);
    }
  }

  return(endFlag);
}

bool MD_Menu::processRun(userNavAction_t nav, mnuInput_t *mInp, bool rtfb)
// Processing for Run user code input field.
// When the field is selected, run the user variable code. For all other
// input do nothing. Return true when the element has run user code.
{
  if (nav == NAV_NULL)    // initialise the CB_DISP
  {
    _pValue = mInp->cbVR(mInp->id, true);

    if (_pValue == nullptr)
    {
      char sz[INP_PRE_SIZE(mInp) + INP_POST_SIZE(mInp) + 1];
      strcpy(sz, FLD_DELIM_L);
      strcat(sz, mInp->label);
      strcat(sz, FLD_DELIM_R);
      _cbDisp(DISP_L1, sz);
    }
    else
    {
      char sz[(strlen( ((char*)_pValue)) + strlen(FLD_PROMPT) + strlen(FLD_DELIM_L)) +  (strlen(FLD_DELIM_R)) + 1];
      strcpy(sz, FLD_DELIM_L);
      strcat(sz, ((char*)_pValue));
      strcat(sz, FLD_DELIM_R);
      _cbDisp(DISP_L1, sz);
    }
  }
  else if (nav == NAV_SEL)
  {
    mInp->cbVR(mInp->id, false);
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
    if ((me == nullptr) || (me->cbVR == nullptr))
      ended = true;
    else
    {
      SET_FLAG(F_INEDIT);
      timerStart();

      switch (me->action)
      {
      case INP_LIST:  ended = processList(NAV_NULL, me, mi->action == MNU_INPUT_FB);            break;
      case INP_BOOL:  ended = processBool(NAV_NULL, me, mi->action == MNU_INPUT_FB);            break;
      case INP_INT:   ended = processInt(NAV_NULL, me, mi->action == MNU_INPUT_FB, incDelta);   break;
      case INP_FLOAT: ended = processFloat(NAV_NULL, me, mi->action == MNU_INPUT_FB, incDelta); break;
      case INP_ENGU:  ended = processEng(NAV_NULL, me, mi->action == MNU_INPUT_FB, incDelta);   break;
      case INP_RUN:   ended = processRun(NAV_NULL, me, mi->action == MNU_INPUT_FB);             break;
      }
    }
  }
  else
  {
    userNavAction_t nav = _cbNav(incDelta);

    if (nav == NAV_ESC)
      ended = true;
    else if (nav != NAV_NULL)
    {
      timerStart();
      mi = loadItem(_mnuStack[_currMenu].idItmCurr);
      me = loadInput(mi->actionId);

      switch (me->action)
      {
      case INP_LIST:  ended = processList(nav, me, mi->action == MNU_INPUT_FB);            break;
      case INP_BOOL:  ended = processBool(nav, me, mi->action == MNU_INPUT_FB);            break;
      case INP_INT:   ended = processInt(nav, me, mi->action == MNU_INPUT_FB, incDelta);   break;
      case INP_FLOAT: ended = processFloat(nav, me, mi->action == MNU_INPUT_FB, incDelta); break;
      case INP_ENGU:  ended = processEng(nav, me, mi->action == MNU_INPUT_FB, incDelta);   break;
      case INP_RUN:   ended = processRun(nav, me, mi->action == MNU_INPUT_FB);             break;
      }
    }
  }

  if (ended)
  {
    CLEAR_FLAG(F_INEDIT);
    handleMenu(true);
  }
}

void MD_Menu::handleMenu(bool bNew)
{
  bool update = false;
  mnuItem_t *mi;

  if (bNew)
  {
    _cbDisp(DISP_CLEAR, nullptr);
    _cbDisp(DISP_L0, _mnuStack[_currMenu].label);
    if (_mnuStack[_currMenu].idItmCurr == 0)
      _mnuStack[_currMenu].idItmCurr = _mnuStack[_currMenu].idItmStart;
    SET_FLAG(F_INMENU);
    timerStart();
    update = true;
  }
  else
  {
    uint16_t incDelta = 1;
    userNavAction_t nav = _cbNav(incDelta);

    if (nav != NAV_NULL) timerStart();

    switch (nav)
    {
    case NAV_DEC:
      do
      {
        if (_mnuStack[_currMenu].idItmCurr > _mnuStack[_currMenu].idItmStart)
        {
          _mnuStack[_currMenu].idItmCurr--;
          update = true;
        }
        else if (TEST_FLAG(F_MENUWRAP))
        {
          _mnuStack[_currMenu].idItmCurr = _mnuStack[_currMenu].idItmEnd;
          update = true;
        }
      } while ((mi = loadItem(_mnuStack[_currMenu].idItmCurr)) == nullptr);
      break;

    case NAV_INC:
    do
    {
      if (_mnuStack[_currMenu].idItmCurr < _mnuStack[_currMenu].idItmEnd)
      {
        _mnuStack[_currMenu].idItmCurr++;
        update = true;
      }
      else if (TEST_FLAG(F_MENUWRAP))
      {
        _mnuStack[_currMenu].idItmCurr = _mnuStack[_currMenu].idItmStart;
        update = true;
      }
    } while ((mi = loadItem(_mnuStack[_currMenu].idItmCurr)) == nullptr);
    break;

    case NAV_SEL:
      {
        mi = loadItem(_mnuStack[_currMenu].idItmCurr);

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
        case MNU_INPUT_FB:
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
      {
        CLEAR_FLAG(F_INMENU);
      }
      else
      {
        _currMenu--;
        handleMenu(true);  // just one level of recursion;
      }
      break;
    }
  }

  if (update) // update L1 on the CB_DISP
  {
    mnuItem_t *mi = loadItem(_mnuStack[_currMenu].idItmCurr);

    if (mi != nullptr)
    {
      char sz[strlen(mi->label) + strlen(MNU_DELIM_L) + strlen(MNU_DELIM_R) + 1]; // temporary string

      strcpy(sz, MNU_DELIM_L);
      strcat(sz, mi->label);
      strcat(sz, MNU_DELIM_R);

      _cbDisp(DISP_L1, sz);
    }
  }
}

bool MD_Menu::runMenu(bool bStart)
{
  // check if we need to process anything
  if (!TEST_FLAG(F_INMENU) && !bStart)
  {
    uint16_t dummy;

    bStart = (TEST_FLAG(F_AUTOSTART) && _cbNav(dummy) == NAV_SEL);
    if (bStart) MD_PRINTS("\nrunMenu: Auto Start detected");
    if (!bStart) return(false);   // nothing to do
  }

  if (bStart)   // start the menu
  {
    MD_PRINTS("\nrunMenu: Starting menu");
    _currMenu = 0;
    loadMenu();
    handleMenu(true);
  }
  else    // keep running current menu
  {
    if (TEST_FLAG(F_INEDIT))
      handleInput();
    else
      handleMenu();

    timerCheck();  // check for timeout before we end here

    if (!TEST_FLAG(F_INMENU))
    {
      _cbDisp(DISP_CLEAR, nullptr);
      MD_PRINTS("\nrunMenu: Ending Menu");
    }
  }

  return(TEST_FLAG(F_INMENU));
}
