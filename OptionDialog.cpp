// OptionDialog.cpp
//{{{  includes
#include "pch.h"
#include "OptionDialog.h"

#include "Option.h"
#include "FedDoc.h"
//}}}
//{{{  static strings
static const char szSettings[] = "Settings";

static const char szSingleInstance[]            = "SingleInstance";
static const char szShowEditMarks[]             = "ShowEditMarks";
static const char szShowLeftMargin[]            = "ShowLeftMargin";
static const char szLineWrapLength[]            = "LineWrapLength";
static const char szLineWrapShade[]             = "LineWrapShade";
static const char szShowWrapMark[]              = "ShowWrapMark";
static const char szShowDebug[]                 = "ShowDebug";
static const char szSaveWindowPlacement[]       = "SaveWindowPlacement";
static const char szSaveOptions[]               = "SaveOptions";
static const char szSuppressBeeps[]             = "SuppressBeeps";
static const char szShowClosedFoldComments[]    = "ShowClosedFoldComments";

static const char szCheckFileIsNewer[]          = "CheckFileIsNewer";
static const char szFileNewerDelta[]            = "FileNewerDelta";
static const char szProtectReadOnly[]           = "ProtectReadOnly";
static const char szAllowBackspAtSol[]          = "AllowBackspAtSol";
static const char szAllowLeftAtSol[]            = "AllowLeftAtSol";
static const char szSetCursorAtEndOfPaste[]     = "SetCursorAtEndOfPaste";
static const char szCreateBackupFirstSave[]     = "CreateBackupFirstSave";
static const char szCreateBackupEverySave[]     = "CreateBackupEverySave";
static const char szEnableUndo[]                = "EnableUndo";
//}}}

// cOption Class
//{{{  static cOption variables
BOOL cOption::SingleInstance          = TRUE;
BOOL cOption::ShowEditMarks           = TRUE;
BOOL cOption::ShowLeftMargin          = FALSE;
int cOption::LineWrapLength           = 80;
int cOption::LineWrapShade            = 8;
BOOL cOption::ShowWrapMark            = FALSE;
BOOL cOption::ShowDebug               = FALSE;
BOOL cOption::SaveWindowPlacement     = TRUE;
BOOL cOption::SaveOptions             = TRUE;
BOOL cOption::SuppressBeeps           = FALSE;
BOOL cOption::ShowClosedFoldComments  = TRUE;

BOOL cOption::CheckFileIsNewer      = TRUE;
int cOption::FileNewerDelta         = 2;
BOOL cOption::ProtectReadOnly       = TRUE;
BOOL cOption::AllowBackspAtSol      = TRUE;
BOOL cOption::AllowLeftAtSol        = FALSE;
BOOL cOption::SetCursorAtEndOfPaste = FALSE;
BOOL cOption::CreateBackupFirstSave = TRUE;
BOOL cOption::CreateBackupEverySave = TRUE;
BOOL cOption::EnableUndo            = FALSE;
//}}}

//{{{
bool cOption::LoadStartOptionsFromRegistry()
{
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  SingleInstance = pApp->GetProfileInt (szSettings, szSingleInstance, SingleInstance);

  return true;
}
//}}}
//{{{
bool cOption::LoadFromRegistry()
{
  UpdateRegistry_Display (false);
  UpdateRegistry_Editor (false);

  return true;
}
//}}}
//{{{
bool cOption::SaveToRegistry()
{
  bool ok = true;

  ok &= UpdateRegistry_Display (true);
  ok &= UpdateRegistry_Editor (true);
  return ok;
}
//}}}
//{{{
bool cOption::UpdateRegistry_Display (bool save)
{
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  BOOL ok = true;
  if (save) {
    ok &= pApp->WriteProfileInt (szSettings, szSingleInstance,      SingleInstance);
    ok &= pApp->WriteProfileInt (szSettings, szSaveWindowPlacement, SaveWindowPlacement);
    ok &= pApp->WriteProfileInt (szSettings, szShowEditMarks,       ShowEditMarks);
    ok &= pApp->WriteProfileInt (szSettings, szLineWrapLength,      LineWrapLength);
    ok &= pApp->WriteProfileInt (szSettings, szLineWrapShade,       LineWrapShade);
    ok &= pApp->WriteProfileInt (szSettings, szShowWrapMark,        ShowWrapMark);
    ok &= pApp->WriteProfileInt (szSettings, szShowLeftMargin,      ShowLeftMargin);
    ok &= pApp->WriteProfileInt (szSettings, szShowDebug,           ShowDebug);
    }

  else {
    SingleInstance      = pApp->GetProfileInt (szSettings, szSingleInstance,      SingleInstance);
    SaveWindowPlacement = pApp->GetProfileInt (szSettings, szSaveWindowPlacement, SaveWindowPlacement);
    ShowEditMarks       = pApp->GetProfileInt (szSettings, szShowEditMarks,       ShowEditMarks);
    LineWrapLength      = pApp->GetProfileInt (szSettings, szLineWrapLength,      LineWrapLength);
    LineWrapShade       = pApp->GetProfileInt (szSettings, szLineWrapShade,       LineWrapShade);
    ShowWrapMark        = pApp->GetProfileInt (szSettings, szShowWrapMark,        ShowWrapMark);
    ShowLeftMargin      = pApp->GetProfileInt (szSettings, szShowLeftMargin,      ShowLeftMargin);
    ShowDebug           = pApp->GetProfileInt (szSettings, szShowDebug,           ShowDebug);
    }

  return ok ? true : false;
}
//}}}
//{{{
bool cOption::UpdateRegistry_Editor (bool save)
{
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  BOOL ok = true;
  if (save) {
    ok &= pApp->WriteProfileInt (szSettings, szCheckFileIsNewer,      CheckFileIsNewer);
    ok &= pApp->WriteProfileInt (szSettings, szFileNewerDelta,        FileNewerDelta);
    ok &= pApp->WriteProfileInt (szSettings, szProtectReadOnly,       ProtectReadOnly);
    ok &= pApp->WriteProfileInt (szSettings, szAllowBackspAtSol,      AllowBackspAtSol);
    ok &= pApp->WriteProfileInt (szSettings, szAllowLeftAtSol,        AllowLeftAtSol);
    ok &= pApp->WriteProfileInt (szSettings, szSetCursorAtEndOfPaste, SetCursorAtEndOfPaste);
    ok &= pApp->WriteProfileInt (szSettings, szCreateBackupFirstSave, CreateBackupFirstSave);
    ok &= pApp->WriteProfileInt (szSettings, szCreateBackupEverySave, CreateBackupEverySave);
    ok &= pApp->WriteProfileInt (szSettings, szEnableUndo,            EnableUndo);
    ok &= pApp->WriteProfileInt (szSettings, szSuppressBeeps,       SuppressBeeps);
    }

  else {
    CheckFileIsNewer      = pApp->GetProfileInt (szSettings, szCheckFileIsNewer,      CheckFileIsNewer);
    FileNewerDelta        = pApp->GetProfileInt (szSettings, szFileNewerDelta,        FileNewerDelta);
    ProtectReadOnly       = pApp->GetProfileInt (szSettings, szProtectReadOnly,       ProtectReadOnly);
    AllowBackspAtSol      = pApp->GetProfileInt (szSettings, szAllowBackspAtSol,      AllowBackspAtSol);
    AllowLeftAtSol        = pApp->GetProfileInt (szSettings, szAllowLeftAtSol,        AllowLeftAtSol);
    SetCursorAtEndOfPaste = pApp->GetProfileInt (szSettings, szSetCursorAtEndOfPaste, SetCursorAtEndOfPaste);
    CreateBackupFirstSave = pApp->GetProfileInt (szSettings, szCreateBackupFirstSave, CreateBackupFirstSave);
    CreateBackupEverySave = pApp->GetProfileInt (szSettings, szCreateBackupEverySave, CreateBackupEverySave);
    EnableUndo            = pApp->GetProfileInt (szSettings, szEnableUndo,            EnableUndo);
    SuppressBeeps         = pApp->GetProfileInt (szSettings, szSuppressBeeps,         SuppressBeeps);
    }

  return ok ? true : false;
}
//}}}

// cOptionDialog
//{{{
cOptionDialog::cOptionDialog (CWnd* pParentWnd)
  //: CPropertySheet (IDS_SETTINGS_CAPTION, pParentWnd)
  : CPropertySheet ("Settings", pParentWnd)
{
  AddPage (&m_DisplayPage);
  AddPage (&m_EditorPage);
}
//}}}
//{{{
cOptionDialog::~cOptionDialog()
{
}
//}}}
IMPLEMENT_DYNAMIC(cOptionDialog, CPropertySheet)
//{{{
BEGIN_MESSAGE_MAP(cOptionDialog, CPropertySheet)
END_MESSAGE_MAP()
//}}}

// cDisplayPage property page
//{{{
cDisplayPage::cDisplayPage()
  : CPropertyPage (cDisplayPage::IDD)
{
  //{{AFX_DATA_INIT(cDisplayPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  SingleInstance         = cOption::SingleInstance;
  //HorizScrollBar         = cOption::HorizScrollBar;
  //VertScrollBar          = cOption::VertScrollBar;
  ShowEditMarks          = cOption::ShowEditMarks;
  ShowLeftMargin         = cOption::ShowLeftMargin;
  ShowWrapMark           = cOption::ShowWrapMark;
  LineWrapLength         = cOption::LineWrapLength;
  LineWrapShade          = cOption::LineWrapShade;
  SaveWindowPlacement    = cOption::SaveWindowPlacement;
  ShowClosedFoldComments = cOption::ShowClosedFoldComments;
  ShowDebug              = cOption::ShowDebug;
  SaveOptions            = cOption::SaveOptions;
}
//}}}
//{{{
cDisplayPage::~cDisplayPage()
{
}
//}}}
//{{{
void cDisplayPage::DoDataExchange (CDataExchange* pDX)
{
  DDX_Check (pDX, IDC_OPTION_SINGLEINSTANCE,   SingleInstance);
  //DDX_Check (pDX, IDC_OPTION_HORIZ,            HorizScrollBar);
  //DDX_Check (pDX, IDC_OPTION_VERT,             VertScrollBar);
  DDX_Check (pDX, IDC_OPTION_EDITMARKS,        ShowEditMarks);
  DDX_Check (pDX, IDC_OPTION_SHOWLEFTMARGIN,   ShowLeftMargin);
  DDX_Check (pDX, IDC_OPTION_SHOWWRAPMARK,     ShowWrapMark);
  DDX_Check (pDX, IDC_OPTION_SAVEWINDOW,       SaveWindowPlacement);
  DDX_Check (pDX, IDC_OPTION_SHOWCLOSEDFOLDCOMMENTS, ShowClosedFoldComments);
  DDX_Check (pDX, IDC_OPTION_SHOWDEBUG,        ShowDebug);
  DDX_Check (pDX, IDC_OPTION_SAVEOPTIONS,      SaveOptions);

  DDX_Text (pDX, IDC_OPTION_LINEWRAPLENGTH, LineWrapLength);
  DDX_Text (pDX, IDC_OPTION_LINEWRAPSHADE, LineWrapShade);
  DDV_MinMaxInt (pDX, LineWrapLength, 0, 1000);
  DDV_MinMaxInt (pDX, LineWrapShade, 0, 100);

  //CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(cDisplayPage)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP

}
//}}}
IMPLEMENT_DYNCREATE(cDisplayPage, CPropertyPage)
//{{{
BEGIN_MESSAGE_MAP(cDisplayPage, CPropertyPage)
  ON_BN_CLICKED (IDC_OPTION_SINGLEINSTANCE,         OnChanged)
  ON_BN_CLICKED (IDC_OPTION_EDITMARKS,              OnChanged)
  ON_BN_CLICKED (IDC_OPTION_SHOWLEFTMARGIN,         OnChanged)
  ON_BN_CLICKED (IDC_OPTION_SHOWWRAPMARK,           OnChangedWrapmark)
  ON_BN_CLICKED (IDC_OPTION_SAVEWINDOW,             OnChanged)
  ON_BN_CLICKED (IDC_OPTION_SHOWDEBUG,              OnChanged)
  ON_BN_CLICKED (IDC_OPTION_SHOWCLOSEDFOLDCOMMENTS, OnChanged)
  ON_BN_CLICKED (IDC_OPTION_SAVEOPTIONS,            OnChanged)

  ON_EN_CHANGE  (IDC_OPTION_LINEWRAPLENGTH,         OnChanged)
  ON_EN_CHANGE  (IDC_OPTION_LINEWRAPSHADE,          OnChanged)
END_MESSAGE_MAP()
//}}}
//{{{
void cDisplayPage::OnChangedWrapmark()
{
  CButton* button = (CButton*) GetDlgItem (IDC_OPTION_SHOWWRAPMARK);
  if (button)
    {
    BOOL enable = button->GetCheck();
    CWnd* wnd = GetDlgItem (IDC_OPTION_LINEWRAPLENGTH);
    if (wnd)
      wnd->EnableWindow (enable);
    wnd = GetDlgItem (IDC_OPTION_LINEWRAPSHADE);
    if (wnd)
      wnd->EnableWindow (enable);
    }
  SetModified();
}
//}}}
//{{{
void cDisplayPage::OnOK()
{
  cOption::SingleInstance         = SingleInstance;
  //cOption::HorizScrollBar         = HorizScrollBar;
  //cOption::VertScrollBar          = VertScrollBar;
  cOption::ShowEditMarks          = ShowEditMarks;
  cOption::ShowLeftMargin         = ShowLeftMargin;
  cOption::LineWrapLength         = LineWrapLength;
  cOption::LineWrapShade          = LineWrapShade;
  cOption::ShowWrapMark           = ShowWrapMark;
  cOption::SaveWindowPlacement    = SaveWindowPlacement;
  cOption::ShowClosedFoldComments = ShowClosedFoldComments;
  cOption::ShowDebug              = ShowDebug;
  cOption::SaveOptions            = SaveOptions;

  if (cOption::SaveOptions)
    cOption::UpdateRegistry_Display (true);

  cFedDoc::UpdateViews();

  CPropertyPage::OnOK();
}
//}}}

// cEditorPage property page
//{{{
cEditorPage::cEditorPage()
  : CPropertyPage (cEditorPage::IDD)
{
  //{{AFX_DATA_INIT(cEditorPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  CheckFileIsNewer      = cOption::CheckFileIsNewer;
  FileNewerDelta        = cOption::FileNewerDelta;
  ProtectReadOnly       = cOption::ProtectReadOnly;
  AllowBackspAtSol      = cOption::AllowBackspAtSol;
  AllowLeftAtSol        = cOption::AllowLeftAtSol;
  SetCursorAtEndOfPaste = cOption::SetCursorAtEndOfPaste;
  CreateBackupFirstSave = cOption::CreateBackupFirstSave;
  CreateBackupEverySave = cOption::CreateBackupEverySave;
  EnableUndo            = cOption::EnableUndo;
  SuppressBeeps         = cOption::SuppressBeeps;
}
//}}}
//{{{
cEditorPage::~cEditorPage()
{
}
//}}}
//{{{
void cEditorPage::DoDataExchange (CDataExchange* pDX)
{
  DDX_Check (pDX, IDC_OPTION_CHECKFILE,       CheckFileIsNewer);
  DDX_Check (pDX, IDC_OPTION_PROTREADONLY,    ProtectReadOnly);
  DDX_Check (pDX, IDC_OPTION_BACKSPACE,       AllowBackspAtSol);
  DDX_Check (pDX, IDC_OPTION_LEFTARROW,       AllowLeftAtSol);
  DDX_Check (pDX, IDC_OPTION_PASTECURSOR,     SetCursorAtEndOfPaste);
  DDX_Check (pDX, IDC_OPTION_BACKUPFIRSTSAVE, CreateBackupFirstSave);
  DDX_Check (pDX, IDC_OPTION_BACKUPEVERYSAVE, CreateBackupEverySave);
  DDX_Check (pDX, IDC_OPTION_ENABLEUNDO,      EnableUndo);
  DDX_Check (pDX, IDC_OPTION_SUPPRESSBEEPS,   SuppressBeeps);

  DDX_Text (pDX, IDC_OPTION_FILENEWERDELTA, FileNewerDelta);
  DDV_MinMaxInt (pDX, FileNewerDelta, 0, 59);
  //CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(cEditorPage)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP


  CButton* button = (CButton*) GetDlgItem (IDC_OPTION_ENABLEUNDO);
  if (button)
    button->EnableWindow (EnableUndo);
}
//}}}
IMPLEMENT_DYNCREATE(cEditorPage, CPropertyPage)
//{{{
BEGIN_MESSAGE_MAP(cEditorPage, CPropertyPage)
  ON_BN_CLICKED (IDC_OPTION_CHECKFILE,       OnChangedCheckfile)
  ON_BN_CLICKED (IDC_OPTION_PROTREADONLY,    OnChanged)
  ON_BN_CLICKED (IDC_OPTION_BACKSPACE,       OnChanged)
  ON_BN_CLICKED (IDC_OPTION_LEFTARROW,       OnChanged)
  ON_BN_CLICKED (IDC_OPTION_PASTECURSOR,     OnChanged)
  ON_BN_CLICKED (IDC_OPTION_BACKUPFIRSTSAVE, OnChanged)
  ON_BN_CLICKED (IDC_OPTION_BACKUPEVERYSAVE, OnChanged)
  ON_BN_CLICKED (IDC_OPTION_ENABLEUNDO,      OnChanged)
  ON_BN_CLICKED (IDC_OPTION_SUPPRESSBEEPS,   OnChanged)

  ON_EN_CHANGE  (IDC_OPTION_FILENEWERDELTA,  OnChanged)
END_MESSAGE_MAP()
//}}}
//{{{
void cEditorPage::OnChangedCheckfile()
{
  CButton* button = (CButton*) GetDlgItem (IDC_OPTION_CHECKFILE);
  CWnd* wnd = GetDlgItem (IDC_OPTION_FILENEWERDELTA);
  if (button && wnd)
    wnd->EnableWindow (button->GetCheck());
  SetModified();
}
//}}}
//{{{
void cEditorPage::OnOK()
{
  cOption::CheckFileIsNewer      = CheckFileIsNewer;
  cOption::FileNewerDelta        = FileNewerDelta;
  cOption::ProtectReadOnly       = ProtectReadOnly;
  cOption::AllowBackspAtSol      = AllowBackspAtSol;
  cOption::AllowLeftAtSol        = AllowLeftAtSol;
  cOption::SetCursorAtEndOfPaste = SetCursorAtEndOfPaste;
  cOption::CreateBackupFirstSave = CreateBackupFirstSave;
  cOption::CreateBackupEverySave = CreateBackupEverySave;
  cOption::EnableUndo            = EnableUndo;
  cOption::SuppressBeeps         = SuppressBeeps;

  if (cOption::SaveOptions)
    cOption::UpdateRegistry_Editor (true);

  CPropertyPage::OnOK();
}
//}}}
