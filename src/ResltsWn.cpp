//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TResultsWin (TWindow).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <math.h>

#include "resltswn.h"
#include "layoutwn.h"

extern MethodA *methodA;
extern MethodB *methodB;
extern char formula[512];


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TResultsWin, TWindow)
//{{TResultsWinRSP_TBL_BEGIN}}
  EV_WM_SIZE,
  EV_NM_DBLCLK(26433, ValidDblClick),
  EV_NM_DBLCLK(26434, BadDblClick),
//{{TResultsWinRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TResultsWin Implementation}}


TResultsWin::TResultsWin(TWindow* parent, const char far* title,  TModule* module)
:
    TWindow(parent, title, module)
{
  // Override the default window style for TWindow.
  //
   // Attr.Style |= WS_VSCROLL | WS_HSCROLL;
    //  Scroller = new TScroller(this, 10, 10, 20, 500);

      Attr.Style |= WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;

	  validList = new TListWindow(this, 26433, 15, 40, 0, 0);
	  validList->Attr.Style |= LVS_REPORT;

	  badList = new TListWindow(this, 26434, 15, 40, 0, 0);
	  badList->Attr.Style |= LVS_REPORT;

	  //ListWin->SetBkgndColor( TColor::Sys3dFace );
}


TResultsWin::~TResultsWin()
{
  Destroy(IDCANCEL);

  delete validList;
  delete badList;
}

void TResultsWin::SetupWindow()
{
  TWindow::SetupWindow();

  TListWindColumn column("Satisfying Inputs", 165);
  validList->InsertColumn(0, column);
  TListWindColumn column2("Non-Satisfying Inputs", 165);
  badList->InsertColumn(0, column2);
}

bool TResultsWin::ShowWindow(int cmdShow)
{
  bool result;

  result = TWindow::ShowWindow(cmdShow);


  return result;
}

void TResultsWin::Paint(TDC& dc, bool erase, TRect& rect)
{
  TWindow::Paint(dc, erase, rect);

  TWin *win1 = TYPESAFE_DOWNCAST(Parent, TLayoutWin)->win;

  TRect r;
  //int xx, yy;


  TFont * font = new TFont("Arial Bold Italic", 30);
  dc.SelectObject(*font);
  r.top = 5;
  r.left = GetClientRect().Width() / 2.5;
  dc.DrawText("Results", -1, r, DT_CALCRECT | DT_CENTER);
  dc.DrawText("Results", -1, r, DT_CENTER);
  dc.RestoreObjects();
  delete font;

  if(formula[0] == '\0')
  	return;

  char buf[256];
  r.top = r.bottom + 5;
  r.left = 5;
  sprintf(buf, "Elapsed time: %u seconds, using %10.0f tiles.", win1->elapsed, win1->tile_count);
  dc.DrawText(buf, -1, r, DT_CALCRECT | DT_CENTER);
  dc.DrawText(buf, -1, r, DT_CENTER);

  /*dc.SelectObject(*new TFont("Arial Italic", 20));
  r.top = r.bottom + 5;
  r.left = 5;
  dc.DrawText("Satisfying Inputs:", -1, r, DT_CALCRECT | DT_CENTER);
  dc.DrawText("Satisfying Inputs:", -1, r, DT_CENTER);

  r.left = r.right+75;
  dc.DrawText("Bad Inputs:", -1, r, DT_CALCRECT | DT_CENTER);
  dc.DrawText("Bad Inputs:", -1, r, DT_CENTER);

  dc.RestoreObjects();

  if(win->method == METHOD_A)
  {
    int i,j,y = r.bottom + 5;
    char buf[128];
    memset(buf, 0, 128);
    buf[0] = '(';
    buf[1] = methodA->variable_list[0];

    for(j = 1, i = 2; j < methodA->num_variables; i+=2, j++)
    {
  		buf[i] = ',';
		buf[i+1] = methodA->variable_list[j];
    }

    buf[i] = ')';
    buf[i+1] = ' ';
    buf[i+2] = '=';
    buf[i + 3] = ' ';

	yy = r.top = y;
    r.left = 5;
    dc.DrawText(buf, -1, r, DT_CALCRECT | DT_CENTER);
    dc.DrawText(buf, -1, r, DT_CENTER);

    r.left = r.right + 30;
    dc.DrawText(buf, -1, r, DT_CALCRECT | DT_CENTER);
    dc.DrawText(buf, -1, r, DT_CENTER);

    int x = 25;
    y += 20;
    int k;

    for(i = 0; i < methodA->numValidInputs; i++)
    {
    	memset(buf, 0, 128);
        k = 0;
        buf[k] = '(';

    	for(j = 0; j < methodA->num_variables; j++)
        {
        	if(j != 0)
       	        buf[++k] = ',';

        	buf[++k] = methodA->valid_inputs[i][j];
        }
        buf[++k] = ')';
   		r.top = y;
	  	r.left = x;
    	dc.DrawText(buf, -1, r, DT_CALCRECT); //calculate rect here!
	  	dc.DrawText(buf, -1, r);
        y += 20;
    }

    yy +=20;
    xx = r.right + 15;
    for(i = 0; i < methodA->numBadInputs; i++)
    {
    	memset(buf, 0, 128);
        k = 0;
        buf[k] = '(';

    	for(j = 0; j < methodA->num_variables; j++)
        {
        	if(j != 0)
       	        buf[++k] = ',';

        	buf[++k] = methodA->bad_inputs[i][j];
        }
        buf[++k] = ')';
   		r.top = yy;
	  	r.left = xx;
    	dc.DrawText(buf, -1, r, DT_CALCRECT); //calculate rect here!
	   	dc.DrawText(buf, -1, r);
        yy += 20;
    }
  }
  else
  {
    int i,j,y = r.bottom + 5;
    char buf[128];
    memset(buf, 0, 128);
    buf[0] = '(';
    buf[1] = methodB->variable_list[0];

    for(j = 1, i = 2; j < methodB->num_variables; i+=2, j++)
    {
  		buf[i] = ',';
		buf[i+1] = methodB->variable_list[j];
    }

    buf[i] = ')';
    buf[i+1] = ' ';
    buf[i+2] = '=';
    buf[i + 3] = ' ';

	yy = r.top = y;
    r.left = 5;
    dc.DrawText(buf, -1, r, DT_CALCRECT | DT_CENTER);
    dc.DrawText(buf, -1, r, DT_CENTER);

    r.left = r.right + 30;
    dc.DrawText(buf, -1, r, DT_CALCRECT | DT_CENTER);
    dc.DrawText(buf, -1, r, DT_CENTER);

    int x = 25;
    y += 20;
    int k;

    for(i = 0; i < methodB->numValidInputs; i++)
    {
    	memset(buf, 0, 128);
        k = 0;
        buf[k] = '(';

    	for(j = 0; j < methodB->num_variables; j++)
        {
        	if(j != 0)
       	        buf[++k] = ',';

        	buf[++k] = methodB->valid_inputs[i][j];
        }
        buf[++k] = ')';
   		r.top = y;
	  	r.left = x;
    	dc.DrawText(buf, -1, r, DT_CALCRECT); //calculate rect here!
	  	dc.DrawText(buf, -1, r);
        y += 20;
    }

    yy +=20;
    xx = r.right + 15;
    for(i = 0; i < methodB->numBadInputs; i++)
    {
    	memset(buf, 0, 128);
        k = 0;
        buf[k] = '(';

    	for(j = 0; j < methodB->num_variables; j++)
        {
        	if(j != 0)
       	        buf[++k] = ',';

        	buf[++k] = methodB->bad_inputs[i][j];
        }
        buf[++k] = ')';
   		r.top = yy;
	  	r.left = xx;
    	dc.DrawText(buf, -1, r, DT_CALCRECT); //calculate rect here!
	   	dc.DrawText(buf, -1, r);
        yy += 20;
    }
  }*/
}



void TResultsWin::PerformCreate(int menuOrId)
{
  TWindow::PerformCreate(menuOrId);
}


void TResultsWin::EvSize(uint sizeType, TSize& size)
{
    TWindow::EvSize(sizeType, size);

    int x = 5;
  	int width = (GetClientRect().Width() / 2) - 10;
  	int height = GetClientRect().Height() - 80;
	validList->MoveWindow(x,75, width, height);
    x += width + 5;
	badList->MoveWindow(x, 75, width, height);
}

void TResultsWin::ValidDblClick()
{
  // find the first item that is selected
  //
  for (int i = 0, imax = validList->GetItemCount(); i < imax; i++)
  {
  	if (validList->GetItemState(i, TListWindItem::Selected))
  	{
    	TListWindItem item(*validList, i, 0);
    	RunSingle(item.GetText());
    	break;
  	}
  }
}

void TResultsWin::BadDblClick()
{
  // find the first item that is selected
  //
  for (int i = 0, imax = badList->GetItemCount(); i < imax; i++)
  {
  	if (badList->GetItemState(i, TListWindItem::Selected))
  	{
    	TListWindItem item(*badList, i, 0);
    	RunSingle(item.GetText());
    	break;
  	}
  }
}

void TResultsWin::RunSingle(char * txt)
{
	int num = 0;
    int pos = 0;

    for(unsigned i = strlen(txt); i >= 1; i--)
    {
		if(isalnum(txt[i]))
        {
        	if(txt[i] == '1')
            	num += pow(2, pos);

			pos++;
		}
    }

    TWin *win1 = TYPESAFE_DOWNCAST(Parent, TLayoutWin)->win;

	if(!win1->CheckAndConfirmOperation())
        return;

    win1->no_paint = false;
    win1->tile_count = 0;
    win1->isComputing = true;

    if(win1->method == METHOD_A)
		methodA->RunComputation(win1, true, num);
    else
    	methodB->RunComputation(win1, true, num);

	win1->isComputing = false;
    win1->Invalidate(true);
}


