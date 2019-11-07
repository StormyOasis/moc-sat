//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TSaAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(saabtdlg_h)              // Sentry, use file only if it's not already included.
#define saabtdlg_h

#include <owl/static.h>

#include "app.rh"                  // Definition of all resources.


//{{TDialog = TSaAboutDlg}}
class TSaAboutDlg : public TDialog {
  public:
    TSaAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TSaAboutDlg();

//{{TSaAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TSaAboutDlgVIRTUAL_END}}
};    //{{TSaAboutDlg}}


// Reading the VERSIONINFO resource.
//
class TProjectRCVersion {
  public:
    TProjectRCVersion(TModule* module);
    virtual ~TProjectRCVersion();

    bool GetProductName(LPSTR& prodName);
    bool GetProductVersion(LPSTR& prodVersion);
    bool GetCopyright(LPSTR& copyright);
    bool GetDebug(LPSTR& debug);

  protected:
    uint8 far*  TransBlock;
    void far*   FVData;

  private:
    // Don't allow this object to be copied.
    //
    TProjectRCVersion(const TProjectRCVersion&);
    TProjectRCVersion& operator = (const TProjectRCVersion&);
};


#endif  // saabtdlg_h sentry.
