//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TResultDia (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(resultdi_h)              // Sentry, use file only if it's not already included.
#define resultdi_h

#include <owl/dialog.h>

#include "app.rh"            // Definition of all resources.


//{{TDialog = TResultDia}}
class TResultDia : public TDialog {
  public:
    TResultDia(TWindow* parent, TResId resId = IDD_RESULTS, TModule* module = 0);
    virtual ~TResultDia();

    TResultDia * dia;
};    //{{TResultDia}}


#endif  // resultdi_h sentry.

