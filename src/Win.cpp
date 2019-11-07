//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TWin (TWindow).
//
//----------------------------------------------------------------------------
#include <time.h>
#include <owl/pch.h>
#include <owl/inputdia.h>
#include <owl/statusba.h>
#include "app.h"

#include "win.h"
#include "resltswn.h"
#include "layoutwn.h"
#include <stdio.h>

MethodA *methodA = NULL;
MethodB *methodB = NULL;
char formula[512];

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TWin, TWindow)
//{{TWinRSP_TBL_BEGIN}}
  EV_COMMAND(CM_METHODA, CmMethoda),
  EV_COMMAND(CM_METHODB, CmMethodb),
  EV_COMMAND(CM_RUN, CmRun),
//{{TWinRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TWin Implementation}}

const int XUnits = 10;
const int YUnits = 15;
const int XRange = 80;
const int YRange = 500;

//--------------------------------------------------------
// TWin
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TWin::TWin(TWindow* parent, const char far* title, TModule* module)
:
  TWindow(parent, title, module)
{
	method = METHOD_A;
    formula[0] = '\0';
    elapsed = 0;
    no_paint = true;
    isComputing = false;

  	Attr.Style |= WS_VSCROLL | WS_HSCROLL;
	Scroller = new TScroller(this, XUnits, YUnits, XRange, YRange);

    textFont = new TFont("Times Roman", 20, 9, 0,0, FW_BOLD);
}


TWin::~TWin()
{
  Destroy();

  //delete memDC;
  //delete dc1;

  if(methodA)
	delete methodA;
  if(methodB)
    delete methodB;

  delete textFont;
}

void TWin::CmMethoda()
{
	if(!CheckAndConfirmOperation())
        return;

	TMenu(*GetApplication()->GetMainWindow()).ModifyMenu(CM_METHODA, MF_BYCOMMAND | MF_CHECKED, CM_METHODA, "Method &A");
	TMenu(*GetApplication()->GetMainWindow()).ModifyMenu(CM_METHODB, MF_BYCOMMAND | MF_UNCHECKED, CM_METHODB, "Method &B");
	GetApplication()->GetMainWindow()->DrawMenuBar();
	method = METHOD_A;
}

void TWin::CmMethodb()
{
	if(!CheckAndConfirmOperation())
        return;

	TMenu(*GetApplication()->GetMainWindow()).ModifyMenu(CM_METHODA, MF_BYCOMMAND | MF_UNCHECKED, CM_METHODA, "Method &A");
	TMenu(*GetApplication()->GetMainWindow()).ModifyMenu(CM_METHODB, MF_BYCOMMAND | MF_CHECKED, CM_METHODB, "Method &B");
	GetApplication()->GetMainWindow()->DrawMenuBar();

    method = METHOD_B;
}

bool TWin::CanClose()
{
  bool result;

  if(!CheckAndConfirmOperation())
  	return false;

  result = TWindow::CanClose();

  return result;
}

bool TWin::CheckAndConfirmOperation()
{
	if(isComputing) //also means that a computation is in progress
    {
		MessageBox("A computation is in progress. You must wait for it to finish.", 0, MB_OK);
      	return false;
    }

    return true;
}

void TWin::CmRun()
{
	if(!CheckAndConfirmOperation())
        return;

	TResultsWin *resw = TYPESAFE_DOWNCAST(Parent, TLayoutWin)->res_win;
    resw->validList->DeleteAllItems();
    resw->badList->DeleteAllItems();    

    bool res = false;
	time_t start, end;

    percent = 0;

    no_paint = true;
    if(methodA)
    {
        delete methodA;
    	methodA = NULL;
    }

    if(methodB)
    {
        delete methodB;
    	methodB = NULL;
    }

    Invalidate(true);


    if(method == METHOD_A)
    {
    	/*if(methodA)
        	delete methodA;

        if(methodB)
        {
        	delete methodB;
            methodB = NULL;
        } */

    	methodA = new MethodA();

        if(!example)
        {
		    formula[0] = '\0';
			TInputDialog *dia = new TInputDialog(this, "Self-Assembly for SAT", "Enter a formula in 3-CNF form:", formula, 512);
	    	if(dia->Execute() == IDCANCEL)
			{
		    	delete dia;
                return;
            }
            delete dia;
        }
        example = false;

    	TYPESAFE_DOWNCAST(GetApplication()->GetMainWindow()->ChildWithId(IDW_STATUSBAR), TStatusBar)->SetText("Processing......");

	    if(!methodA->ParseFormula(formula))
    	 	MessageBox("Invalid input format.", "Self-Assembly for SAT", MB_ICONSTOP | MB_OK);

	    Invalidate(true);

        tile_count = 0;
        start = time(NULL);
        no_paint = false;
        isComputing = true;
		res = methodA->RunComputation(this);
        end = time(NULL);
        elapsed = difftime(end, start);
        isComputing = false;
    }
    else if(method == METHOD_B)
    {
    	/*if(methodB)
        	delete methodB;

        if(methodA)
        {
        	delete methodA;
            methodA = NULL;
        }*/

    	methodB = new MethodB();

        if(!example)
        {
		    formula[0] = '\0';
			TInputDialog *dia = new TInputDialog(this, "Self-Assembly for SAT", "Enter a formula in 3-CNF form:", formula, 512);
	    	dia->Execute();
	    	delete dia;
        }

        example = false;

       	TYPESAFE_DOWNCAST(GetApplication()->GetMainWindow()->ChildWithId(IDW_STATUSBAR), TStatusBar)->SetText("Processing......");

	    if(!methodB->ParseFormula(formula))
    	 	MessageBox("Invalid input format.", "Self-Assembly for SAT", MB_ICONSTOP | MB_OK);

	    Invalidate(true);
        no_paint = false;
        tile_count = 0;
        isComputing = true;
        start = time(NULL);
		res = methodB->RunComputation(this);
        end = time(NULL);
        elapsed = difftime(end, start);
        isComputing = false;
    }

    if(!res) //error
	    TYPESAFE_DOWNCAST(GetApplication()->GetMainWindow()->ChildWithId(IDW_STATUSBAR), TStatusBar)->SetText("Not Satisfiable");
    else
	    TYPESAFE_DOWNCAST(GetApplication()->GetMainWindow()->ChildWithId(IDW_STATUSBAR), TStatusBar)->SetText("Satisfiable");

    Parent->Invalidate(true);
}


void TWin::SetupWindow()
{
  TWindow::SetupWindow();

  //TClientDC dc(*this);

  //memDC = new TMemoryDC(dc);
}


void TWin::drawTiles(int w, int h)
{
    width = w;
    height = h;

    Invalidate(true);
    Parent->SendMessage(WM_PAINT, true, 0);
}

void TWin::Paint(TDC& dc, bool erase, TRect& rect)
{
  	TWindow::Paint(dc, erase, rect);

    if(no_paint)
    	return;

    if(isComputing)
    {
  		TRect r;

        TFont * font = new TFont("Arial Bold Italic", 20);
  		dc.SelectObject(*font);
  		r.top = 175;
  		r.left = 50;
        char buf[255];
        sprintf(buf, "Computing...Please Wait...%0.f%%", percent);
  		dc.DrawText(buf, -1, r, DT_CALCRECT | DT_CENTER);
  		dc.DrawText(buf, -1, r, DT_CENTER);
  		dc.RestoreObjects();
        delete font;

		/* --- */
    	dc.DrawText(formula, -1, r, DT_CALCRECT);
    	r.top = 0;
    	r.left = 0;
    	dc.DrawText(formula, -1, r);

    	if((methodA == NULL  && methodB == NULL) || formula[0] == '\0')
    		return;

		if(methodA)
        {
	    	if(methodA->current_input == NULL || methodA->tiles == NULL)
    			return;

    		int xx = 0, yy = 25;
	    	char eq1[32], eq2[90], buf[128];
    		char * str, *str2;

	    	for(int i = 0; i < methodA->num_variables; i++)
		    {
    			TRect rr = TRect(xx,yy, xx+10, yy+15);
        		char * tmp = &methodA->variable_list[i];

		        if(i == 0)
    		    {
	    		    sprintf(eq1, "(%c", tmp[0]);
		    		if(methodA->current_input[i])
					    sprintf(eq2, "(1");
	        	    else
					    sprintf(eq2, "(0");
	    	    }
    	    	else
	        	{
    	    		char b[16];
	    	        sprintf(b, ", %c", tmp[0]);
    	    	    str = eq1;
	    	    	str = strcat(str, b);

		    		if(methodA->current_input[i])
	    	        {
		    	        sprintf(b, ", 1");
        	    	    str2 = eq2;
		    	    	str2 = strcat(str2, b);
		            }
    		        else
        		    {
	        		    sprintf(b, ", 0");
                		str2 = eq2;
			        	str2 = strcat(str2, b);
	    	        }
    	    	}
	    	    xx+=15;
	    	}

			if(methodA->num_variables == 1)
    		{
    			str = eq1;
		        str2 = eq2;
    		}

	    	str = strcat(str, ")");
	    	str2 = strcat(str2, ")");
		    sprintf(buf, "%s = %s", str, str2);

    		r.top = 20;
		    r.left = 0;
    		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    	dc.DrawText(buf, -1, r);
	    }
        else if(methodB)
        {
	    	if(methodB->current_input == NULL || methodB->tiles == NULL)
    			return;

    		int xx = 0, yy = 25;
	    	char eq1[32], eq2[90], buf[128];
    		char * str, *str2;

	    	for(int i = 0; i < methodB->num_variables; i++)
		    {
    			TRect rr = TRect(xx,yy, xx+10, yy+15);
        		char * tmp = &methodB->variable_list[i];

		        if(i == 0)
    		    {
	    		    sprintf(eq1, "(%c", tmp[0]);
		    		if(methodB->current_input[i])
					    sprintf(eq2, "(1");
	        	    else
					    sprintf(eq2, "(0");
	    	    }
    	    	else
	        	{
    	    		char b[16];
	    	        sprintf(b, ", %c", tmp[0]);
    	    	    str = eq1;
	    	    	str = strcat(str, b);

		    		if(methodB->current_input[i])
	    	        {
		    	        sprintf(b, ", 1");
        	    	    str2 = eq2;
		    	    	str2 = strcat(str2, b);
		            }
    		        else
        		    {
	        		    sprintf(b, ", 0");
                		str2 = eq2;
			        	str2 = strcat(str2, b);
	    	        }
    	    	}
	    	    xx+=15;
	    	}

			if(methodB->num_variables == 1)
    		{
    			str = eq1;
		        str2 = eq2;
    		}

	    	str = strcat(str, ")");
	    	str2 = strcat(str2, ")");
		    sprintf(buf, "%s = %s", str, str2);

    		r.top = 20;
		    r.left = 0;
    		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    	dc.DrawText(buf, -1, r);
	    }
    }
    else
    {
	   	if(method == METHOD_A)
    		drawMethodA(dc);
	    else
    		drawMethodB(dc);
    }

 /* hbmMem = ::CreateCompatibleBitmap(dc.GetHDC(), 100 + (width*64), 100 + (height*64));
  memDC->SelectObject(hbmMem);
  dc1 = new TDC(memDC->GetHDC());
  dc1->BitBlt(0,0,100 + (width*64), 100 + (height*64), *memDC, 0,0, WHITENESS);  */
    //dc.BitBlt(0,0,100 + (width*64), 100 + (height*64), *memDC, 0,0, SRCCOPY);
}

void TWin::drawMethodA(TDC &dc)
{
	int w = 64, h = 64, x= 0, y = 50;

    TRect r;
    dc.DrawText(formula, -1, r, DT_CALCRECT);

    r.top = 0;
    r.left = 0;
    dc.DrawText(formula, -1, r);

    if(methodA == NULL || formula[0] == '\0')
    	return;

    if(methodA->current_input == NULL || methodA->tiles == NULL)
    	return;

    int xx = 0, yy = 25;
    char eq1[32], eq2[90], buf[128];
    char * str, *str2;

    for(int i = 0; i < methodA->num_variables; i++)
    {
    	TRect rr = TRect(xx,yy, xx+10, yy+15);

        char * tmp = &methodA->variable_list[i];

        if(i == 0)
        {
	        sprintf(eq1, "(%c", tmp[0]);
	    	if(methodA->current_input[i])
			    sprintf(eq2, "(1");
            else
			    sprintf(eq2, "(0");
        }
        else
        {
        	char b[16];
            sprintf(b, ", %c", tmp[0]);
            str = eq1;
	        str = strcat(str, b);

	    	if(methodA->current_input[i])
            {
	            sprintf(b, ", 1");
                str2 = eq2;
		        str2 = strcat(str2, b);
            }
            else
            {
	            sprintf(b, ", 0");
                str2 = eq2;
		        str2 = strcat(str2, b);
            }
        }

        xx+=15;
    }

	if(methodA->num_variables == 1)
    {
    	str = eq1;
        str2 = eq2;
    }

    dc.SelectObject(*textFont);

    str = strcat(str, ")");
    str2 = strcat(str2, ")");
    sprintf(buf, "%s = %s", str, str2);

    r.top = 20;
    r.left = 0;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
    dc.DrawText(buf, -1, r);

  	if(methodA->tiles == NULL || isComputing)
    	return;

    for(int j = 0; j < height; j++)
    {
    	for(int i = 0; i < width; i++)
        {
		  	if(methodA == NULL || methodA->tiles == NULL)
		    	return;

            if(methodA->tiles[j] == NULL)
               return;

        	if(methodA->tiles[j][i].inUse != true)
            	continue;

            switch(methodA->tiles[j][i].tileType)
            {
            	case InputBoundary:
                {
                    PrintInputTiles(methodA->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case VariableTile:
                {
                 	PrintInputTiles(methodA->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case ClauseTile:
                {
					PrintInputTiles(methodA->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case Assignment:
                {
                	PrintAssignmentTiles(methodA->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case ComputationBoundary:
                {
                	PrintCompBoundaryTiles(methodA->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case Computation:
                {
                	PrintComputationTiles(methodA->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case FinalCheck:
                {
                	PrintFinalTiles(methodA->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            }

            x+=w;
        }

        y += h;
        x = 0;
    }
}

void TWin::PrintInputTiles(Tile tile, TDC &dc, int x, int y, int w, int h)
{
	TRect r;

    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE1));
    memDC.SelectObject(bmp);

	if(tile.topRight.type == Input_Begin)
    {
    	char buf[10];
        strcpy(buf, "Begin");

        r.top = y+1;
        r.left = x+2;
    	dc.DrawText(buf, -1, r, DT_CALCRECT);
        int tw = r.right - r.left;
        r.left = r.left + (tw / 4);
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    }
	else if(tile.bottomRight.type == Input_Variable || tile.bottomRight.type == Input_C)
    {
    	char buf[2];
        char var = '\0';

        if(tile.bottomRight.data_offset != -1)
        	var = methodA->variable_list[tile.bottomRight.data_offset];

        if(tile.bottomRight.type == Input_C)
	        buf[0] = 'C';
        else
        	buf[0] = var;

        buf[1] = '\0';

    	TRect r;
	    int th, tw;
   		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    th = r.bottom-r.top;
	    tw = r.right-r.left;

        r.left = x + w - (tw + 2);
        r.top = y + h - (th + 2);
        r.bottom = r.top + th;
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    }
	else if(tile.topRight.type == Input_V || tile.topRight.type == Input_Clause || tile.topRight.type == Input_E)
    {
    	char buf[20];
        Clause *c = NULL;

        if(tile.bottomRight.data_offset != -1)
        	c = (Clause *)&methodA->clauses[tile.bottomRight.data_offset];

        buf[1] = '\0';
        if(tile.topRight.type == Input_V)
	        buf[0] = 'V';
        else if(tile.topRight.type == Input_E)
        	buf[0] = 'E';
        else
            sprintf(buf, "C%d", c->number);


    	TRect r;
	    int th, tw;
   		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    th = r.bottom-r.top;
	    tw = r.right-r.left;

        r.left = x + w - (tw + 2);
        r.top = y + 2;
        r.bottom = r.top + th;
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    }

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintAssignmentTiles(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE2));
    memDC.SelectObject(bmp);

    if(tile.topLeft.data_offset == -1)
		return; //some sort of error

    TRect r;
    int th, tw;
   	dc.DrawText("x", -1,  r, DT_CALCRECT); //dummy values
    PrintAssignmentCorner(tile.topLeft, dc, x + 5,y + 2);

   	dc.DrawText("V", -1,  r, DT_CALCRECT); //dummy values
    tw = r.right-r.left;
    PrintAssignmentCorner(tile.topRight, dc, x + w - 2-tw,y + 2);

   	dc.DrawText("x+", -1,  r, DT_CALCRECT); //dummy values
    th = r.bottom-r.top;
    tw = r.right-r.left;
    PrintAssignmentCorner(tile.bottomRight, dc,x + w - 2 - tw,y + h - 2 - th);

   	dc.DrawText("V", -1,  r, DT_CALCRECT); //dummy values
    th = r.bottom-r.top;
    PrintAssignmentCorner(tile.bottomLeft, dc, x + 5,y + h - 2 - th);

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintAssignmentCorner(TileCorner corner, TDC &dc, int x, int y)
{
	char buf[3];
    char var;

    buf[1] = buf[2] = '\0';

	if(corner.type == Assingment_V)
    	buf[0] = 'V';
    else if(corner.type ==  Assignment_Variable)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodA->variable_list[corner.data_offset];

    	buf[0] = var;
    }
    else if(corner.type ==  Assignment_Var1)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodA->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '+';
    }
    else if(corner.type ==  Assignment_Var0)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodA->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '-';
    }

    TRect r;
    r.top = y;
    r.left = x;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);
}

void TWin::PrintCompBoundaryTiles(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE3));
    memDC.SelectObject(bmp);

    TRect r;
   	dc.DrawText("x+", -1,  r, DT_CALCRECT); //dummy values
    int th = r.bottom-r.top;
    int tw = r.right-r.left;

    PrintCompBoundaryCorner(tile.topLeft, dc, x + 5, y+ 2);
    PrintCompBoundaryCorner(tile.topRight, dc, x + w - 2 - tw, y + 2);
    PrintCompBoundaryCorner(tile.bottomLeft, dc, x + 5, y + h - 2 - th);

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintCompBoundaryCorner(TileCorner corner, TDC &dc, int x, int y)
{
	char buf[3];
    char var;

    buf[1] = buf[2] = '\0';

    if(corner.type ==  ComputationBoundary_E)
    	buf[0] = 'E';
    else if(corner.type ==  ComputationBoundary_Var1)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodA->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '+';
    }
    else if(corner.type ==  ComputationBoundary_Var0)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodA->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '-';
    }

    TRect r;
    r.top = y;
    r.left = x;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);
}

void TWin::PrintComputationTiles(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE4));
    memDC.SelectObject(bmp);

    TRect r;
   	dc.DrawText("OK", -1,  r, DT_CALCRECT); //dummy values
    int th = r.bottom-r.top;
    int tw = r.right-r.left;

    PrintComputationCorner(tile.topLeft, dc, x + 5, y+ 2);
    PrintComputationCorner(tile.topRight, dc, x + w - 2 - tw, y + 2);
    PrintComputationCorner(tile.bottomLeft, dc, x + 5, y + h - 2 - th);
    PrintComputationCorner(tile.bottomRight, dc,x + w - 2 - tw,y + h - 2 - th);


	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintComputationCorner(TileCorner corner, TDC &dc, int x, int y)
{
 	char buf[6];
    char var;

    if(corner.type == Computation_OK)
    {
    	buf[0] = 'O';
        buf[1] = 'K';
        buf[3] = '\0';
    }
    else if(corner.type == Computation_Clause || corner.type == Input_Clause)
		sprintf(buf, "C%u", corner.data_offset);
    else if(corner.type == Computation_Var1 || corner.type == Assignment_Var1)
    {
    	var = methodA->variable_list[corner.data_offset];
        buf[0] = var;
        buf[1] = '+';
        buf[2] = '\0';
    }
    else if(corner.type == Computation_Var0 || corner.type == Assignment_Var0)
    {
    	var = methodA->variable_list[corner.data_offset];
        buf[0] = var;
        buf[1] = '-';
        buf[2] = '\0';
    }


    TRect r;
    r.top = y;
    r.left = x;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);
}

void TWin::PrintFinalTiles(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE5));
    memDC.SelectObject(bmp);

	if(tile.bottomRight.type == FinalCheck_Sucess)
    {
    	char buf[10];
        strcpy(buf, "Success");

        TRect r;
        r.top = y + (h / 4)+5;
        r.left = x+ (w / 5)-2;
    	dc.DrawText(buf, -1, r, DT_CALCRECT);
       	dc.DrawText(buf, -1, r);
    }

    TRect r;
    int th, tw;
   	dc.DrawText("X", -1, r, DT_CALCRECT); //dummy value
    th = r.bottom-r.top;
    tw = r.right-r.left;


	PrintFinalCorner(tile.topLeft, dc, x + 5,y + 2);

	if(tile.bottomRight.type != FinalCheck_Sucess)
    	PrintFinalCorner(tile.topRight, dc, x + w - 2-tw,y + 2);

    PrintFinalCorner(tile.bottomLeft, dc, x + 5,y + h - 2 - th);

	if(tile.bottomRight.type != FinalCheck_Sucess)
	    PrintFinalCorner(tile.bottomRight, dc, x + w - 2 - tw,y + h - 2 - th);

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintFinalCorner(TileCorner corner, TDC &dc, int x, int y)
{
	if(corner.type == Empty)
    	return;

    char buf[3];

    if(corner.type == FinalCheck_C)
    {
    	buf[0] = 'C';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_V)
    {
    	buf[0] = 'V';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_T)
    {
    	buf[0] = 'T';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_E)
    {
    	buf[0] = 'E';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_OK)
    {
    	buf[0] = 'O';
        buf[1] = 'K';
        buf[2] = '\0';
    }

    TRect r;
    r.top = y;
    r.left = x;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);

}

void TWin::drawMethodB(TDC &dc)
{
	int w = 64, h = 64, x= 0, y = 50;

    TRect r;
    dc.DrawText(formula, -1, r, DT_CALCRECT);

    r.top = 0;
    r.left = 0;
    dc.DrawText(formula, -1, r);

    if(methodB == NULL || methodB->tiles == NULL)
    	return;

    if(methodB->current_input == NULL)
    	return;

    int xx = 0, yy = 25;
    char eq1[32], eq2[90], buf[128];
    char * str, *str2;

    for(int i = 0; i < methodB->num_variables; i++)
    {
    	TRect rr = TRect(xx,yy, xx+10, yy+15);

        char * tmp = &methodB->variable_list[i];

        if(i == 0)
        {
	        sprintf(eq1, "(%c", tmp[0]);
	    	if(methodB->current_input[i])
			    sprintf(eq2, "(1");
            else
			    sprintf(eq2, "(0");
        }
        else
        {
        	char b[16];
            sprintf(b, ", %c", tmp[0]);
            str = eq1;
	        str = strcat(str, b);

	    	if(methodB->current_input[i])
            {
	            sprintf(b, ", 1");
                str2 = eq2;
		        str2 = strcat(str2, b);
            }
            else
            {
	            sprintf(b, ", 0");
                str2 = eq2;
		        str2 = strcat(str2, b);
            }
        }

        xx+=15;
    }

	if(methodB->num_variables == 1)
    {
    	str = eq1;
        str2 = eq2;
    }

    dc.SelectObject(*textFont);

    str = strcat(str, ")");
    str2 = strcat(str2, ")");
    sprintf(buf, "%s = %s", str, str2);

    r.top = 20;
    r.left = 0;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
    dc.DrawText(buf, -1, r);

  	if(methodB->tiles == NULL || isComputing)
    	return;

    for(int j = 0; j < height; j++)
    {
    	for(int i = 0; i < width; i++)
        {
		  	if(methodB == NULL || methodB->tiles == NULL)
		    	return;

        	if(methodB->tiles[j][i].inUse != true)
            	continue;

            switch(methodB->tiles[j][i].tileType)
            {
            	case InputBoundary:
                {
                    PrintInputTilesForB(methodB->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case Assignment:
                {
                    PrintAssignmentTilesForB(methodB->tiles[j][i], dc, x,y, w,h);
                    break;
                }
                case Computation:
                {
                    PrintComputationTilesForB(methodB->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            	case ComputationBoundary:
                {
                	PrintCompBoundaryTilesForB(methodB->tiles[j][i], dc, x,y, w,h);
                    break;
                }
                case FinalCheck:
                {
	                PrintFinalTilesForB(methodB->tiles[j][i], dc, x,y, w,h);
                    break;
                }
            }

            x+=w;
        }

        y += h;
        x = 0;
    }

}

void TWin::PrintInputTilesForB(Tile tile, TDC &dc, int x, int y, int w, int h)
{
	TRect r;

    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE1));
    memDC.SelectObject(bmp);

	if(tile.topRight.type == Input_Begin)
    {
    	char buf[10];
        strcpy(buf, "Begin");

        r.top = y+1;
        r.left = x+2;
    	dc.DrawText(buf, -1, r, DT_CALCRECT);
        int tw = r.right - r.left;
        r.left = r.left + (tw / 4);
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    }
	else if(tile.bottomRight.type == Input_Variable || tile.bottomRight.type == Input_C)
    {
    	char buf[2];
        char var = '\0';

        if(tile.bottomRight.data_offset != -1)
        	var = methodB->variable_list[tile.bottomRight.data_offset];

        if(tile.bottomRight.type == Input_C)
	        buf[0] = 'C';
        else
        	buf[0] = var;

        buf[1] = '\0';

    	TRect r;
	    int th, tw;
   		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    th = r.bottom-r.top;
	    tw = r.right-r.left;

        r.left = x + w - (tw + 2);
        r.top = y + h - (th + 2);
        r.bottom = r.top + th;
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    }
	else if(tile.bottomRight.type == Input_S)
    {
    	char buf[2];

	    buf[0] = 'S';
        buf[1] = '\0';

    	TRect r;
	    int th, tw;
   		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    th = r.bottom-r.top;
	    tw = r.right-r.left;

        r.left = x + w - (tw + 2);
        r.top = y + 2;
        r.bottom = r.top + th;
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    }
	else if(tile.topRight.type == Input_V || tile.topRight.type == Input_E)
    {
    	char buf[20];
        //Clause *c = NULL;

        //if(tile.bottomRight.data_offset != -1)
         //	c = (Clause *)&methodB->clauses[tile.bottomRight.data_offset];

        buf[1] = '\0';
        if(tile.topRight.type == Input_V)
	        buf[0] = 'V';
        else if(tile.topRight.type == Input_E)
        	buf[0] = 'E';


    	TRect r;
	    int th, tw;
   		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    th = r.bottom-r.top;
	    tw = r.right-r.left;

        r.left = x + w - (tw + 2);
        r.top = y + 2;
        r.bottom = r.top + th;
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    }
    else
    {
    	char buf[20];
       	Clause * c = (Clause*)&methodB->clauses[tile.bottomRight.data_offset];
        char var = c->variables[tile.bottomRight.offset_for_b];

        if(tile.topRight.type == Computation_Literal0)
	       sprintf(buf, "%c-", var);
        else
	       sprintf(buf, "%c+", var);

    	TRect r;
	    int th, tw;
   		dc.DrawText(buf, -1, r, DT_CALCRECT);
	    th = r.bottom-r.top;
	    tw = r.right-r.left;

        r.left = x + w - (tw + 2);
        r.top = y + 2;
        r.bottom = r.top + th;
        r.right = r.left + tw;

       	dc.DrawText(buf, -1, r);
    } 

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND );

    memDC.RestoreObjects();
}

void TWin::PrintAssignmentTilesForB(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);
    TBitmap bmp(*GetApplication(), TResId(IDB_TILE2));
    memDC.SelectObject(bmp);

    if(tile.topLeft.data_offset == -1)
		return; //some sort of error

    TRect r;
    int th, tw;
   	dc.DrawText("x", -1,  r, DT_CALCRECT); //dummy values
    PrintAssignmentCornerForB(tile.topLeft, dc, x + 5,y + 2);

   	dc.DrawText("V", -1,  r, DT_CALCRECT); //dummy values
    tw = r.right-r.left;
    PrintAssignmentCornerForB(tile.topRight, dc, x + w - 2-tw,y + 2);

   	dc.DrawText("x+", -1,  r, DT_CALCRECT); //dummy values
    th = r.bottom-r.top;
    tw = r.right-r.left;
    PrintAssignmentCornerForB(tile.bottomRight, dc,x + w - 2 - tw,y + h - 2 - th);

   	dc.DrawText("V", -1,  r, DT_CALCRECT); //dummy values
    th = r.bottom-r.top;
    PrintAssignmentCornerForB(tile.bottomLeft, dc, x + 5,y + h - 2 - th);

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);
    memDC.RestoreObjects();
}

void TWin::PrintAssignmentCornerForB(TileCorner corner, TDC &dc, int x, int y)
{
	char buf[3];
    char var;

    buf[1] = buf[2] = '\0';

	if(corner.type == Assingment_V)
    	buf[0] = 'V';
    else if(corner.type ==  Assignment_Variable)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodB->variable_list[corner.data_offset];

    	buf[0] = var;
    }
    else if(corner.type ==  Assignment_Var1)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodB->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '+';
    }
    else if(corner.type ==  Assignment_Var0)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodB->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '-';
    }

    TRect r;
    r.top = y;
    r.left = x;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);
}

void TWin::PrintComputationTilesForB(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE4));
    memDC.SelectObject(bmp);

    TRect r;
   	dc.DrawText("OK", -1,  r, DT_CALCRECT); //dummy values
    int th = r.bottom-r.top;
    int tw = r.right-r.left;

    PrintComputationCornerForB(tile.topLeft, dc, x + 5, y+ 2);
    PrintComputationCornerForB(tile.topRight, dc, x + w - 2 - tw, y + 2);
    PrintComputationCornerForB(tile.bottomLeft, dc, x + 5, y + h - 2 - th);
    PrintComputationCornerForB(tile.bottomRight, dc,x + w - 2 - tw,y + h - 2 - th);

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintComputationCornerForB(TileCorner corner, TDC &dc, int x, int y)
{
 	char buf[6];
    char var;

    if(corner.type == Computation_OK)
    {
    	buf[0] = 'O';
        buf[1] = 'K';
        buf[2] = '\0';
    }
    else if(corner.type == Computation_Literal1)
    {
    	Clause * c = (Clause*)&methodB->clauses[corner.data_offset];
        var = c->variables[corner.offset_for_b];
        buf[0] = var;
        buf[1] = '+';
        buf[2] = ' ';
        buf[3] = '\0';
    }
    else if(corner.type == Computation_Literal0)
    {
    	Clause * c = (Clause*)&methodB->clauses[corner.data_offset];
        var = c->variables[corner.offset_for_b];
        buf[0] = var;
        buf[1] = '-';
        buf[2] = ' ';
        buf[3] = '\0';
    }
    else if(corner.type == Computation_Var1 || corner.type == Assignment_Var1)
    {
    	var = methodB->variable_list[corner.data_offset];
        buf[0] = var;
        buf[1] = '+';
        buf[2] = ' ';
        buf[3] = '\0';
    }
    else if(corner.type == Computation_Var0 || corner.type == Assignment_Var0)
    {
    	var = methodB->variable_list[corner.data_offset];
        buf[0] = var;
        buf[1] = '-';
        buf[2] = ' ';
        buf[3] = '\0';
    }
    else if(corner.type == Input_S || corner.type == Computation_S1 ||  corner.type == Computation_S0)
    {
    	buf[0] = 'S';
        buf[1] = '\0';
        buf[2] = '\0';
    }

    TRect r;
    r.top = y;
    r.left = x;

    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);
}

void TWin::PrintCompBoundaryTilesForB(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE3));
    memDC.SelectObject(bmp);

    TRect r;
   	dc.DrawText("x+ ", -1,  r, DT_CALCRECT); //dummy values
    int th = r.bottom-r.top;
    int tw = r.right-r.left;

    PrintCompBoundaryCornerForB(tile.topLeft, dc, x + 5, y+ 2);
    PrintCompBoundaryCornerForB(tile.topRight, dc, x + w - 2 - tw, y + 2);
    PrintCompBoundaryCornerForB(tile.bottomLeft, dc, x + 5, y + h - 2 - th);

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintCompBoundaryCornerForB(TileCorner corner, TDC &dc, int x, int y)
{
	char buf[4];
    char var;

    buf[1] = buf[2] = '\0';
    buf[3] = '\0';

    if(corner.type ==  ComputationBoundary_E)
    	buf[0] = 'E';
    else if(corner.type ==  ComputationBoundary_Var1)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodB->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '+';
        buf[2] = ' ';
        buf[3] = '\0';
    }
    else if(corner.type ==  ComputationBoundary_Var0)
    {
    	var = '\0';
    	if(corner.data_offset != -1)
	    	var = methodB->variable_list[corner.data_offset];

    	buf[0] = var;
        buf[1] = '-';
        buf[2] = ' ';
        buf[3] = '\0';
    }

    TRect r;
    r.top = y;
    r.left = x;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);
}

void TWin::PrintFinalTilesForB(Tile tile, TDC &dc, int x, int y, int w, int h)
{
    TMemoryDC memDC(dc);

    TBitmap bmp(*GetApplication(), TResId(IDB_TILE5));
    memDC.SelectObject(bmp);

	if(tile.bottomRight.type == FinalCheck_Sucess)
    {
    	char buf[10];
        strcpy(buf, "Success");

        TRect r;
        r.top = y + (h / 4)+5;
        r.left = x+ (w / 5)-2;
    	dc.DrawText(buf, -1, r, DT_CALCRECT);
       	dc.DrawText(buf, -1, r);
    }

    TRect r;
    int th, tw;
   	dc.DrawText("X", -1, r, DT_CALCRECT); //dummy value
    th = r.bottom-r.top;
    tw = r.right-r.left;


	PrintFinalCornerForB(tile.topLeft, dc, x + 5,y + 2);

	if(tile.bottomRight.type != FinalCheck_Sucess)
    	PrintFinalCornerForB(tile.topRight, dc, x + w - 2-tw,y + 2);

    PrintFinalCornerForB(tile.bottomLeft, dc, x + 5,y + h - 2 - th);

	if(tile.bottomRight.type != FinalCheck_Sucess)
	    PrintFinalCornerForB(tile.bottomRight, dc, x + w - 2 - tw,y + h - 2 - th);

	dc.BitBlt(x,y,w,h, memDC, 0,0, SRCAND);

    memDC.RestoreObjects();
}

void TWin::PrintFinalCornerForB(TileCorner corner, TDC &dc, int x, int y)
{
	if(corner.type == Empty)
    	return;

    char buf[3];

    if(corner.type == FinalCheck_C)
    {
    	buf[0] = 'C';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_V)
    {
    	buf[0] = 'V';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_T)
    {
    	buf[0] = 'T';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_F)
    {
    	buf[0] = 'F';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_E)
    {
    	buf[0] = 'E';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_S0 || corner.type == FinalCheck_S1)
    {
    	buf[0] = 'S';
        buf[1] = '\0';
    }
    else if(corner.type == FinalCheck_Literal1)
    {
    	Clause * c = (Clause *)&methodB->clauses[corner.data_offset];
        char var = c->variables[corner.offset_for_b];
    	buf[0] = var;
        buf[1] = '+';
        buf[2] = '\0';
    }
    else if(corner.type == FinalCheck_Literal0)
    {
    	Clause * c = (Clause *)&methodB->clauses[corner.data_offset];
        char var = c->variables[corner.offset_for_b];
    	buf[0] = var;
        buf[1] = '-';
        buf[2] = '\0';
    }
    else if(corner.type == FinalCheck_OK)
    {
    	buf[0] = 'O';
        buf[1] = 'K';
        buf[2] = '\0';
    }

    TRect r;
    r.top = y;
    r.left = x;
    dc.DrawText(buf, -1, r, DT_CALCRECT);
	dc.DrawText(buf, -1, r);
}

void TWin::AddStringToResultList(char * str, int col)
{
	TResultsWin *res = TYPESAFE_DOWNCAST(Parent, TLayoutWin)->res_win;
	TListWindItem item(str, 0);
    if(col == 0)
	    res->validList->InsertItem(item);
    else
   	    res->badList->InsertItem(item);

    free(str);
}


/*
(!b + c + g)( c + !e + !h)(!a + b + d)(!c + e + f)(a + !f + e)(e + g + h)(!b + d + f)(c + !d + e)(a + d + !f)(b + !f + h)
(!x + !y + z)(x + !y + !z)(!x + y + z)
(x + y + z)(!t + y + !u)
(!b + c + g)( c + !e + !h)(!a + b + d)(!c + e + f)(a + !f + e)(!e + g + h)(!b + d + f)(c + !d + e)(a + d + !f)(b + !f + h)(x + y + !z)(!t + y + !u)(j + k + !l)(r + o + !p)(!m + n + s)( ! o + !l + n)(!q + !s + p)
(!b + c + g)( c + !e + !h)(!a + b + d)(!c + e + f)(a + !f + e)(e + g + h)(!b + d + f)(c + !d + e)(a + d + !f)(b + !f + h)(y + t + !u)(!a + !d + v)(!a + !c + v)(q + r + !b)(c + e + !f)
*/
