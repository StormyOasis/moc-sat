//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TLegend (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(legend_h)              // Sentry, use file only if it's not already included.
#define legend_h

#include <owl/window.h>

#include "app.rh"            // Definition of all resources.


//{{TWindow = TLegend}}
class TLegend : public TWindow {
  public:
    TLegend(TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~TLegend();

//{{TLegendVIRTUAL_BEGIN}}
  public:
    virtual void Paint(TDC& dc, bool erase, TRect& rect);
//{{TLegendVIRTUAL_END}}
};    //{{TLegend}}


#endif  // legend_h sentry.

