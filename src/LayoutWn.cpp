//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TLayoutWin (TLayoutWindow).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "layoutwn.h"

extern MethodA *methodA;
extern MethodB *methodB;
extern char formula[512];

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TLayoutWin, TLayoutWindow)
//{{TLayoutWinRSP_TBL_BEGIN}}
  EV_COMMAND(CM_METHODA, CmMethoda),
  EV_COMMAND(CM_METHODB, CmMethodb),
  EV_COMMAND(CM_RUN, CmRun),
  EV_COMMAND(CM_FORMULA1, CmFormula1),
  EV_COMMAND(CM_FORMULA2, CmFormula2),
  EV_COMMAND(CM_FORMULA3, CmFormula3),
//{{TLayoutWinRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TLayoutWin Implementation}}


TLayoutWin::TLayoutWin(TWindow* parent, const char far* title, TModule* module)
:
    TLayoutWindow(parent, title, module)
{
	Attr.Id = 8323;

	win = new TWin(this, "DNA Tiling");
    win->Attr.Style |= WS_CLIPSIBLINGS | WS_BORDER;
    TLayoutMetrics layoutW1;
    layoutW1.X.Absolute(lmLeft,0); // Attach to top corner
    layoutW1.Y.Absolute(lmTop,0);
    layoutW1.Width.PercentOf(lmParent,70);
    layoutW1.Height.PercentOf(lmParent,100);
    SetChildLayoutMetrics(*win,layoutW1);

	res_win = new TResultsWin(this, "");
    res_win->Attr.Style |= WS_CLIPSIBLINGS | WS_BORDER;
    layoutW1.X.RightOf(win);
    layoutW1.Y.Absolute(lmTop,0);
    layoutW1.Width.PercentOf(lmParent,30);
    layoutW1.Height.PercentOf(lmParent,50);
    SetChildLayoutMetrics(*res_win,layoutW1);

	leg_win = new TLegend(this, "");
    leg_win->Attr.Style |= WS_CLIPSIBLINGS | WS_BORDER;
    layoutW1.X.RightOf(win);
    layoutW1.Y.Below(res_win);
    layoutW1.Width.PercentOf(lmParent,30);
    layoutW1.Height.PercentOf(lmParent,50);
    SetChildLayoutMetrics(*leg_win,layoutW1);

}


TLayoutWin::~TLayoutWin()
{
  Destroy(IDCANCEL);

	delete win;
    delete leg_win;
    delete res_win;

}


void TLayoutWin::CmMethoda()
{
	win->SendMessage(WM_COMMAND, CM_METHODA);
}


void TLayoutWin::CmMethodb()
{
	win->SendMessage(WM_COMMAND, CM_METHODB);
}


void TLayoutWin::CmRun()
{
	win->SendMessage(WM_COMMAND, CM_RUN);
}


void TLayoutWin::Paint(TDC& dc, bool erase, TRect& rect)
{
  TLayoutWindow::Paint(dc, erase, rect);

  res_win->Invalidate(true);

}


void TLayoutWin::CmFormula1()
{
  win->example = true;
  sprintf(formula, "(!b + c + g)( c + !e + !h)(!a + b + d)(!c + e + f)(a + !f + e)(e + g + h)(!b + d + f)(c + !d + e)(a + d + !f)(b + !f + h)");
  win->SendMessage(WM_COMMAND, CM_RUN);
}


void TLayoutWin::CmFormula2()
{
  win->example = true;
  sprintf(formula, "(!x + !y + z)(x + !y + !z)(!x + y + z)");
  win->SendMessage(WM_COMMAND, CM_RUN);
}


void TLayoutWin::CmFormula3()
{
  win->example = true;
  sprintf(formula, "(x + y + z)(!t + y + !u)");
  win->SendMessage(WM_COMMAND, CM_RUN);

}


void TLayoutWin::SetupWindow()
{
  TLayoutWindow::SetupWindow();
}

