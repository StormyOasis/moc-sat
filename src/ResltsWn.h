//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TResultsWin (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(resltswn_h)              // Sentry, use file only if it's not already included.
#define resltswn_h

#include <time.h>
#include <owl/window.h>
#include <owl/listwind.h>
#include <owl/listbox.h>

#include "win.h"

#include "app.rh"            // Definition of all resources.


//{{TWindow = TResultsWin}}
class TResultsWin : public TWindow {
  public:
    TResultsWin(TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~TResultsWin();

    void ValidDblClick();
    void BadDblClick();

    void RunSingle(char * txt);

    TListWindow * validList;
    TListWindow * badList;    

//{{TResultsWinVIRTUAL_BEGIN}}
  public:
    virtual void Paint(TDC& dc, bool erase, TRect& rect);
    virtual void SetupWindow();
    virtual bool ShowWindow(int cmdShow);
    virtual void PerformCreate(int menuOrId);
//{{TResultsWinVIRTUAL_END}}

//{{TResultsWinRSP_TBL_BEGIN}}
  protected:
    void EvSize(uint sizeType, TSize& size);
//{{TResultsWinRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TResultsWin);
};    //{{TResultsWin}}


#endif  // resltswn_h sentry.

