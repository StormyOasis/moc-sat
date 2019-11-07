//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TResultDia (TDialog).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "resultdi.h"


//{{TResultDia Implementation}}


TResultDia::TResultDia(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{

}


TResultDia::~TResultDia()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}

