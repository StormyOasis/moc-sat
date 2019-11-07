//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(app_h)              // Sentry, use file only if it's not already included.
#define app_h

#include <owl/opensave.h>


#include "app.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();
};    //{{TSDIDecFrame}}


//{{TApplication = TApp}}
class TApp : public TApplication {
  private:


  public:
    TApp();
    virtual ~TApp();

    TOpenSaveDialog::TData FileData;                    // Data to control open/saveas standard dialog.
    void OpenFile(const char* fileName = 0);

//{{TAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TAppVIRTUAL_END}}

//{{TAppRSP_TBL_BEGIN}}
  protected:
    void CmFileNew();
    void CmFileOpen();
    void CmHelpAbout();
    void EvClose();
//{{TAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TApp);
};    //{{TApp}}


#endif  // app_h sentry.
