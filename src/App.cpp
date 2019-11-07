//----------------------------------------------------------------------------
//  Project SAT
// 
//  SUBSYSTEM:    Sa Application
//  YEAR:		  2006
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TApp (TApplication).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include <owl/statusba.h>
#include <stdio.h>

#include "app.h"
#include "layoutwn.h"                        // Definition of client class.


//{{TApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TApp, TApplication)
//{{TAppRSP_TBL_BEGIN}}
  EV_WM_CLOSE,
//{{TAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TApp
//
TApp::TApp() : TApplication("Self-assembly and SAT")
{

  // Common file file flags and filters for Open/Save As dialogs.  Filename and directory are
  // computed in the member functions CmFileOpen, and CmFileSaveAs.
  //
  FileData.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  FileData.SetFilter("All Files (*.*)|*.*|");
  FileData.DefExt = "txt";

  // INSERT>> Your constructor code here.
}


TApp::~TApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TApp
// ~~~~~
// Application intialization.
//
void TApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWMAXIMIZED : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, true);

  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  // Menu associated with window and accelerator table associated with table.
  //
  frame->AssignMenu(IDM_SDI);

  // Associate with the accelerator table.
  //
  frame->Attr.AccelTable = IDM_SDI;

  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed);
  sb->Attr.Id = IDW_STATUSBAR;
  frame->Insert(*sb, TDecoratedFrame::Bottom);

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_SDI));
}



//--------------------------------------------------------
// TApp
// ~~~~~~~~~~~
// Menu File New command
//
void TApp::CmFileNew()
{
}


//--------------------------------------------------------
// TApp
// ~~~~~~~~~~~
// Menu File Open command
//
void TApp::CmFileOpen()
{
  // Display standard Open dialog box to select a file name.
  //
  *FileData.FileName = 0;

  TWin* client = TYPESAFE_DOWNCAST(GetMainWindow()->GetClientWindow(), TWin);     // Client window for the frame.
  if (client->CanClose())
    if (TFileOpenDialog(GetMainWindow(), FileData).Execute() == IDOK)
      OpenFile();
}


void TApp::OpenFile(const char* fileName)
{
  if (fileName)
    strcpy(FileData.FileName, fileName);

}


//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, (!clientWnd ? new TLayoutWin(0) : clientWnd), trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TApp
// ~~~~~~~~~~~
// Menu Help About Sa command
//
void TApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
}



int OwlMain(int , char* [])
{
  TApp   app;
  return app.Run();
}

void TApp::EvClose()
{



}

