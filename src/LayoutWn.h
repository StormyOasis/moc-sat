//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TLayoutWin (TLayoutWindow).
//
//----------------------------------------------------------------------------
#if !defined(layoutwn_h)              // Sentry, use file only if it's not already included.
#define layoutwn_h

#include <owl/layoutwi.h>

#include "win.h"
#include "legend.h"
#include "resltswn.h"


#include "app.rh"            // Definition of all resources.


//{{TLayoutWindow = TLayoutWin}}
class TLayoutWin : public TLayoutWindow {
  public:
    TLayoutWin(TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~TLayoutWin();

    TWin * win;
    TLegend *leg_win;
    TResultsWin * res_win;

//{{TLayoutWinVIRTUAL_BEGIN}}
  public:
    virtual void Paint(TDC& dc, bool erase, TRect& rect);
    virtual void SetupWindow();
//{{TLayoutWinVIRTUAL_END}}

//{{TLayoutWinRSP_TBL_BEGIN}}
  protected:
    void CmMethoda();
    void CmMethodb();
    void CmRun();
    void CmFormula1();
    void CmFormula2();
    void CmFormula3();
//{{TLayoutWinRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TLayoutWin);
};    //{{TLayoutWin}}


#endif  // layoutwn_h sentry.

