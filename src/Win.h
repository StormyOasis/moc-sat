//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TWin (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(win_h)              // Sentry, use file only if it's not already included.
#define win_h

#include <time.h>
#include <owl/static.h>
#include <owl/scroller.h>
#include <owl/listwind.h>
#include <owl/listbox.h>

#include "methoda.h"
#include "methodb.h"

#include "app.rh"            // Definition of all resources.


//{{TWindow = TWin}}
class TWin : public TWindow {
  public:
    TWin(TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~TWin();

    void drawTiles(int, int);
    void drawMethodA(TDC &dc);
	void drawMethodB(TDC &dc);
    void AddStringToResultList(char * str, int);
    bool CheckAndConfirmOperation();

    int width, height;
    bool no_paint;
    int method;
    time_t elapsed;
    double tile_count;
    bool example;
    bool isComputing;
    double percent;

    TFont * textFont;

//{{TWinVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual void Paint(TDC& dc, bool erase, TRect& rect);
    virtual bool CanClose();
//{{TWinVIRTUAL_END}}

  private:
  	void PrintInputTiles(Tile t, TDC &dc, int, int, int, int);
  	void PrintAssignmentTiles(Tile t, TDC &dc, int, int, int, int);
    void PrintAssignmentCorner(TileCorner corner, TDC &dc, int x, int y);
  	void PrintCompBoundaryTiles(Tile t, TDC &dc, int, int, int, int);
    void PrintCompBoundaryCorner(TileCorner corner, TDC &dc, int x, int y);
  	void PrintComputationTiles(Tile t, TDC &dc, int, int, int, int);
  	void PrintComputationCorner(TileCorner t, TDC &dc, int, int );    
  	void PrintFinalTiles(Tile t, TDC &dc, int, int, int, int);
  	void PrintFinalCorner(TileCorner t, TDC &dc, int, int);


	void PrintInputTilesForB(Tile tile, TDC &dc, int x, int y, int w, int h);
	void PrintAssignmentTilesForB(Tile tile, TDC &dc, int x, int y, int w, int h);
	void PrintAssignmentCornerForB(TileCorner corner, TDC &dc, int x, int y);
  	void PrintFinalTilesForB(Tile t, TDC &dc, int, int, int, int);
  	void PrintFinalCornerForB(TileCorner t, TDC &dc, int, int);
  	void PrintComputationTilesForB(Tile t, TDC &dc, int, int, int, int);
  	void PrintComputationCornerForB(TileCorner t, TDC &dc, int, int );
  	void PrintCompBoundaryTilesForB(Tile t, TDC &dc, int, int, int, int);
    void PrintCompBoundaryCornerForB(TileCorner corner, TDC &dc, int x, int y);

//{{TWinRSP_TBL_BEGIN}}
  protected:
    void CmMethoda();
    void CmMethodb();
    void CmRun();
//{{TWinRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TWin);
};    //{{TWin}}


#endif  // win_h sentry.
