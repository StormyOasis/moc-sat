//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TLegend (TWindow).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/scroller.h>

#include "legend.h"


//{{TLegend Implementation}}


TLegend::TLegend(TWindow* parent, const char far* title, TModule* module)
:
    TWindow(parent, title, module)
{
  // Override the default window style for TWindow.
  //
      Attr.Style |= WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;

      Attr.Style |= WS_VSCROLL | WS_HSCROLL;
      Scroller = new TScroller(this, 10, 10, 20, 25);
}


TLegend::~TLegend()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void TLegend::Paint(TDC& dc, bool erase, TRect& rect)
{
  TWindow::Paint(dc, erase, rect);

  int y, h = 64;
  char buf[512];

  TRect r;
  dc.SelectObject(*new TFont("Arial Bold Italic", 25));
  r.top = 2;
  r.left = GetClientRect().Width() / 2.5;
  dc.DrawText("Legend", -1, r, DT_CALCRECT | DT_CENTER);
  dc.DrawText("Legend", -1, r, DT_CENTER);
  dc.RestoreObjects();

  y = r.bottom+5;

  TMemoryDC memDC(dc);

  TBitmap bmp(*GetApplication(), TResId(IDB_TILE1));
  memDC.SelectObject(bmp);
  dc.BitBlt(5,y,64,h, memDC, 0,0, SRCAND);
  memDC.RestoreObjects();

  strcpy(buf, " - Input Tiles\nThese tiles encode variables and clauses(literals for Method B) that are to be used in the calculation.");
  r.left = 5 + 64 + 10;
  r.right = GetClientRect().Width()-1;
  r.top = y;
  dc.DrawText(buf, -1, r, DT_CALCRECT | DT_WORDBREAK);
  dc.DrawText(buf, -1, r, DT_WORDBREAK);
  y+=(15 + h);

  TBitmap bmp2 = TBitmap(*GetApplication(), TResId(IDB_TILE2));
  memDC.SelectObject(bmp2);
  dc.BitBlt(5,y,64,h, memDC, 0,0, SRCAND);
  memDC.RestoreObjects();
  strcpy(buf, " - Assignment Tiles\nThese tiles encode values into a variable.  This value is either 1 or 0.");
  r.left = 5 + 64 + 10;
  r.right = GetClientRect().Width()-1;
  r.top = y;
  dc.DrawText(buf, -1, r, DT_CALCRECT | DT_WORDBREAK);
  dc.DrawText(buf, -1, r, DT_WORDBREAK);
  y+=(15 + h);

  TBitmap bmp3 = TBitmap(*GetApplication(), TResId(IDB_TILE4));
  memDC.SelectObject(bmp3);
  dc.BitBlt(5,y,64,h, memDC, 0,0, SRCAND);
  memDC.RestoreObjects();
  strcpy(buf, " - Computation Tiles\nThese tiles take in a variable assignment and clause(or literal) and determine if the clause(or literal) is satisfied.");
  r.left = 5 + 64 + 10;
  r.right = GetClientRect().Width()-1;
  r.top = y;
  dc.DrawText(buf, -1, r, DT_CALCRECT | DT_WORDBREAK);
  dc.DrawText(buf, -1, r, DT_WORDBREAK);
  y+=(15 + h);

  TBitmap bmp4 = TBitmap(*GetApplication(), TResId(IDB_TILE3));
  memDC.SelectObject(bmp4);
  dc.BitBlt(5,y,64,h, memDC, 0,0, SRCAND);
  memDC.RestoreObjects();
  strcpy(buf, " - Computation Boundary Tiles\nHelper tiles that are used to mark the end of a computation sequence.");
  r.left = 5 + 64 + 10;
  r.right = GetClientRect().Width()-1;
  r.top = y+10;
  dc.DrawText(buf, -1, r, DT_CALCRECT | DT_WORDBREAK);
  dc.DrawText(buf, -1, r, DT_WORDBREAK);
  y+=(15 + h);

  TBitmap bmp5 = TBitmap(*GetApplication(), TResId(IDB_TILE5));
  memDC.SelectObject(bmp5);
  dc.BitBlt(5,y,64,h, memDC, 0,0, SRCAND);
  memDC.RestoreObjects();
  strcpy(buf, " - Final Tiles\nTiles used to check if all clauses(or literals) are satisfied or not.");
  r.left = 5 + 64 + 10;
  r.right = GetClientRect().Width()-1;
  r.top = y+10;
  dc.DrawText(buf, -1, r, DT_CALCRECT | DT_WORDBREAK);
  dc.DrawText(buf, -1, r, DT_WORDBREAK);


}

