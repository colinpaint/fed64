// MainFrm.cpp
//{{{  includes
#include "pch.h"
#include "MainFrm.h"

#include <afxadv.h>      // for CDockState

#include "Fed.h"
#include "FedDoc.h"
#include "FedView.h"
#include "Option.h"
#include "OptionDialog.h"
#include "Dialog.h"
#include "LanguageSheet.h"
#include "KeymapPage.h"
//}}}
//{{{  static strings
static const char szSettings[]         = "Settings";
static const char szDockState[]        = "Settings\\Dock";
static const char szWindowPlacement[]  = "WindowPlacement";
static const char szDir[]              = "Language_Dir";
static const char szSearches[]         = "Searches";
static const char szFindFmt[]          = "Find%d";
static const char szReplaceFmt[]       = "Replace%d";
//}}}
//{{{
static UINT BASED_CODE findbar[] =
{
    // same order as in the bitmap 'findbar.bmp'
    ID_SEPARATOR,           // 0 for combo box (placeholder)
    ID_SEPARATOR,           // 1
    ID_EDIT_FINDDOWN,       // 2
    ID_EDIT_FINDUP,         // 3
    ID_EDIT_FIND,           // 4
    ID_SEPARATOR,           // 5
    ID_SEPARATOR,           // 6 for combo box (placeholder)
    ID_SEPARATOR,           // 7
    ID_EDIT_REPLACE,        // 8
};
//}}}

cMainFrame::cMainFrame() {}
cMainFrame::~cMainFrame() {}

//{{{
BOOL cMainFrame::OnCreateClient (LPCREATESTRUCT lpcs, CCreateContext* pContext) {

  return m_wndSplitter.Create (this, 2, 2, CSize(10, 10), pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);
  }
//}}}
//{{{
BOOL cMainFrame::PreCreateWindow (CREATESTRUCT& cs) {

  BOOL bRes = CFrameWnd::PreCreateWindow (cs);
  HINSTANCE hInst = AfxGetInstanceHandle();

  // see if the class already exists
  WNDCLASS wndcls;
  if (!::GetClassInfo (hInst, szFedClass, &wndcls)) {
    // get default stuff
    ::GetClassInfo (hInst, cs.lpszClass, &wndcls);
    wndcls.style &= ~(CS_HREDRAW|CS_VREDRAW);

    // register a new class
    wndcls.lpszClassName = szFedClass;
    wndcls.hIcon = ::LoadIcon (hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
    ASSERT(wndcls.hIcon != NULL);
    if (!AfxRegisterClass (&wndcls))
      AfxThrowResourceException();
    }
  cs.lpszClass = szFedClass;

  // fetch window placement from the registry (if valid)
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID (pApp);
  CString str = pApp->GetProfileString (szSettings, szWindowPlacement);
  LPTSTR pStr = str.GetBuffer (0);

  RECT r, wr;
  if (sscanf (pStr, "%d,%d,%d,%d %d,%d,%d,%d",
              &r.left, &r.top, &r.right, &r.bottom, &wr.left, &wr.top, &wr.right, &wr.bottom) == 8)
    //{{{  new window placement (multiple monitors)
    {
    HMONITOR hMonitor = MonitorFromRect (&r, MONITOR_DEFAULTTONEAREST);

    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo (hMonitor, &mi);

    RECT& mr = mi.rcMonitor;
    if ((mr.left == wr.left) && (mr.top == wr.top) && (mr.right == wr.right) && (mr.bottom == wr.bottom)) {
      cs.x = r.left;
      cs.y = r.top;
      cs.cx = r.right - r.left;
      cs.cy = r.bottom - r.top;
      }
    }
    //}}}
  else
    //{{{  old window placement (single monitor)
    {
    int x1, y1, x2, y2, wx, wy;

    if (sscanf (pStr, "%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &wx, &wy) == 6) {
      int screenX = GetSystemMetrics (SM_CXSCREEN);
      int screenY = GetSystemMetrics (SM_CYSCREEN);
      if ((screenX == wx) && (screenY == wy)
          && (x1 >= 0) && (y1 >= 0)
          && (x2 <= screenX) && (y2 <= screenY)
          && (x2 - x1 >= 50) && (y2 - y1 >= 50)) {

        cs.x = x1;
        cs.y = y1;
        cs.cx = x2 - x1;
        cs.cy = y2 - y1;
        }
      }
    }
    //}}}

  return bRes;
  }
//}}}
//{{{
int cMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct) {

  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  EnableDocking (CBRS_ALIGN_ANY);

  if (!CreateToolBar())
    return -1;
  if (!CreateFindBar())
    return -1;

  CDockState dockstate;
  dockstate.LoadState (szDockState);
  SetDockState (dockstate);

  return 0;
  }
//}}}
//{{{
BOOL cMainFrame::CreateToolBar() {

  if (!m_wndToolBar.Create (this) || !m_wndToolBar.LoadToolBar (IDR_MAINFRAME))
    return -1;      // fail to create

  // Make the toolbar have tool tips and be resizeable
  m_wndToolBar.SetBarStyle (m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

  // Make the toolbar to be dockable
  m_wndToolBar.EnableDocking (CBRS_ALIGN_ANY);
  DockControlBar (&m_wndToolBar);

  return TRUE;
  }
//}}}
//{{{
BOOL cMainFrame::CreateFindBar() {

  const int nDropHeight = 250;
  const int nFindWidth  = 160;

  if (!m_wndFindBar.Create (this, WS_CHILD|WS_VISIBLE, IDB_FINDBAR) ||
      !m_wndFindBar.LoadBitmap (IDB_FINDBAR) ||
      !m_wndFindBar.SetButtons (findbar, sizeof(findbar)/sizeof(UINT)))
    return FALSE;

  // Make the toolbar have tool tips and be resizeable
  m_wndFindBar.SetWindowText ("Fed Search");
  m_wndFindBar.SetBarStyle (m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

  // Make the toolbar to be dockable
  m_wndFindBar.EnableDocking (CBRS_ALIGN_ANY);
  DockControlBar (&m_wndFindBar);

  // Create the combo boxes
  m_wndFindBar.SetButtonInfo (0, IDW_FINDCOMBO, TBBS_SEPARATOR, nFindWidth);
  m_wndFindBar.SetButtonInfo (6, IDW_REPLACECOMBO, TBBS_SEPARATOR, nFindWidth);

  CRect rect;
  m_wndFindBar.GetItemRect (0, &rect);
  rect.bottom = rect.top + nDropHeight;
  if (!m_wndFindBar.m_findBox.Create (CBS_DROPDOWN | CBS_AUTOHSCROLL | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL,
                                      rect, &m_wndFindBar, IDW_FINDCOMBO))
    return FALSE;

  m_wndFindBar.GetItemRect(6, &rect);
  rect.bottom = rect.top + nDropHeight;
  if (!m_wndFindBar.m_replaceBox.Create (CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL,
          rect, &m_wndFindBar, IDW_REPLACECOMBO))
    return FALSE;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID (pApp);

  // load find strings
  CString str, key;
  for (int n = 0; n < 10; n++) {
    str.Empty();
    key.Format (szFindFmt, n+1);
    str = pApp->GetProfileString (szSearches, key);
    if (!str.IsEmpty())
      m_wndFindBar.m_findBox.AddString (str);
    }
  m_wndFindBar.m_findBox.SetCurSel (0);

  // load replace strings
  for (int n = 0; n < 10; n++) {
    str.Empty();
    key.Format (szReplaceFmt, n+1);
    str = pApp->GetProfileString (szSearches, key);
    if (!str.IsEmpty())
      m_wndFindBar.m_replaceBox.AddString (str);
    }
  m_wndFindBar.m_replaceBox.SetCurSel (0);

  return TRUE;
  }
//}}}

IMPLEMENT_DYNCREATE(cMainFrame, CFrameWnd)
//{{{
BEGIN_MESSAGE_MAP(cMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_CLOSE()

  ON_COMMAND_RANGE(ID_LANGUAGE0, ID_LANGUAGE99, OnLanguage)
  ON_COMMAND(ID_PREFERENCE_SETUP, OnPreferenceSetup)
  ON_COMMAND(ID_KEYMAP_SETUP, OnKeymapSetup)
  ON_COMMAND(ID_FONT_SETUP, OnFontSetup)
  ON_COMMAND(ID_LANGUAGES_NEW, OnLanguageNew)
  ON_COMMAND(ID_LANGUAGES_LOAD, OnLanguageLoad)

  ON_COMMAND(ID_VIEW_FINDBAR, OnViewFindBar)
  ON_COMMAND_EX(ID_VIEW_FINDBAR, CFrameWnd::OnBarCheck)
  ON_UPDATE_COMMAND_UI(ID_VIEW_FINDBAR, OnUpdateFindBarMenu)

  ON_COMMAND(ID_SPLIT_HORIZ, OnSplitHoriz)
  ON_COMMAND(ID_SPLIT_VERT, OnSplitVert)
  ON_UPDATE_COMMAND_UI(ID_SPLIT_HORIZ, OnUpdateSplitHoriz)
  ON_UPDATE_COMMAND_UI(ID_SPLIT_VERT, OnUpdateSplitVert)

  ON_REGISTERED_MESSAGE(cFedApp::m_nOpenMsg, OnOpenMsg)

END_MESSAGE_MAP()
//}}}

//{{{
void cMainFrame::AddSearchStr (const char* str) {

  if (str && *str) {
    int existing = m_wndFindBar.m_findBox.FindStringExact(-1, str); // Note: case-independent match

    // always delete and re-insert to 1) bring to top, 2) change any upper/lower case
    if (existing != CB_ERR)
      m_wndFindBar.m_findBox.DeleteString(existing);

    m_wndFindBar.m_findBox.InsertString(0, str);  // re-insert at top = 0

    m_wndFindBar.m_findBox.SetCurSel(0);
    }
  }
//}}}
//{{{
void cMainFrame::GetSearchStr (CString& str) {

  m_wndFindBar.m_findBox.GetWindowText(str);
  }
//}}}
//{{{
void cMainFrame::FillSearchBox (CComboBox& combo, bool replace) {

  CComboBox& findBox = replace ? m_wndFindBar.m_replaceBox : m_wndFindBar.m_findBox;

  CString str;
  int count = findBox.GetCount();
  for (int index = 0; index < count; index++) {
    findBox.GetLBText (index, str);
    combo.AddString (str);
    }
  int cursel = findBox.GetCurSel();
  if (cursel > 0)
    combo.SetCurSel (cursel);
  else {
    findBox.GetWindowText (str);
    combo.SetWindowText (str);
    }
  }
//}}}
//{{{
void cMainFrame::AddReplaceStr (const char* str) {

  if (str) {
    int existing = m_wndFindBar.m_replaceBox.FindStringExact(-1, str); // Note: case-independent match

    // always delete and re-insert to 1) bring to top, 2) change any upper/lower case
    if (existing != CB_ERR)
      m_wndFindBar.m_replaceBox.DeleteString(existing);

    m_wndFindBar.m_replaceBox.InsertString(0, str);  // re-insert at top = 0

    m_wndFindBar.m_replaceBox.SetCurSel(0);
    }
  }
//}}}
//{{{
void cMainFrame::GetReplaceStr (CString& str) {

  m_wndFindBar.m_replaceBox.GetWindowText(str);
  }
//}}}

//{{{
BOOL cMainFrame::DoPromptFileName (CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, BOOL initialDir)
{
  CFileDialog dlgFile (bOpenFileDialog, "ini", 0);

  CString title;
  VERIFY (title.LoadString(nIDSTitle));

  dlgFile.m_ofn.Flags |= lFlags;

  CString strFilter;

  // append the "*.ini" project file filter
  strFilter += _T("Fed INI (*.ini)");
  strFilter += (TCHAR)'\0';   // next string please
  strFilter += _T("*.ini");
  strFilter += (TCHAR)'\0';   // next string please

  dlgFile.m_ofn.lpstrFilter = strFilter;
  dlgFile.m_ofn.lpstrTitle = title;

  CString dir;
  if (initialDir) {
    dir = fileName;
    fileName.Empty();
    dlgFile.m_ofn.lpstrInitialDir = dir.GetBuffer(_MAX_PATH);
    }

  dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
  dlgFile.m_ofn.nMaxFile = _MAX_PATH;

  BOOL bResult = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;
  fileName.ReleaseBuffer();
  if (initialDir)
    dir.ReleaseBuffer();

  return bResult;
  }
//}}}

//{{{
LRESULT cMainFrame::OnOpenMsg (WPARAM wParam, LPARAM lParam) {

  TCHAR szFilename[256];
  szFilename[0] = NULL;
  GlobalGetAtomName((ATOM)lParam, szFilename, 256);

  if (szFilename[0] != NULL) {
    // parse for switches remember we have to allow for spaces in the filename!
    TCHAR* s = szFilename;
    while (*s) {
      if (*s++ == '-') {
        s--;
        // terminate the filename
        TCHAR* ss = s - 1;
        while ((ss >= szFilename) && (*ss == ' '))  // remove trailing spaces
          *ss-- = 0;

        while (*s) {
          // parse the rest of line for switches
          if (*s == '-') {
            s++;  // skip the switch character
            if ((*s == 'r') || (*s == 'R'))
              cFedApp::ReadOnly = TRUE;
            else if ((*s == 'g') || (*s == 'G')) {
              s++;
              cFedApp::GotoLineNumber = 0;
              while ((*s >= '0') && (*s <= '9'))
                cFedApp::GotoLineNumber = cFedApp::GotoLineNumber * 10 + (*s++ - '0');
              }
            }
          while (*s && (*s != ' '))
            s++;    // skip non-spaces
          while (*s == ' ')
            s++;    // skip spaces
          }
        }
      }

    cFedDoc::AddDocument (szFilename, wParam ? true : false);
    return TRUE;
    }

  return FALSE;
  }
//}}}

//{{{
void cMainFrame::OnSplitHoriz() {

   CRect rect;
   m_wndSplitter.GetWindowRect (&rect);

   if (m_wndSplitter.GetColumnCount() > 1)
     m_wndSplitter.DeleteColumn (1);
   else
     m_wndSplitter.SplitColumn (rect.Width()/2);

   m_wndSplitter.RecalcLayout();
  }
//}}}
//{{{
void cMainFrame::OnSplitVert() {

   CRect rect;
   m_wndSplitter.GetWindowRect (&rect);

   if (m_wndSplitter.GetRowCount() > 1)
     m_wndSplitter.DeleteRow (1);
   else
     m_wndSplitter.SplitRow (rect.Height()/2);

   m_wndSplitter.RecalcLayout();
  }
//}}}
//{{{
void cMainFrame::OnUpdateSplitHoriz (CCmdUI* pCmdUI) {

  pCmdUI->SetCheck(m_wndSplitter.GetColumnCount() > 1);
  }
//}}}
//{{{
void cMainFrame::OnUpdateSplitVert (CCmdUI* pCmdUI) {

  pCmdUI->SetCheck(m_wndSplitter.GetRowCount() > 1);
  }
//}}}

//{{{
void cMainFrame::OnClose() {

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  if (cOption::SaveWindowPlacement) {
    // save window placement to the registry
    RECT r;
    GetWindowRect (&r);

    HMONITOR hMonitor = MonitorFromRect (&r, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    mi.cbSize = sizeof (mi);
    GetMonitorInfo (hMonitor, &mi);
    RECT& mr = mi.rcMonitor;

    if ((r.left >= mr.left) && (r.top >= mr.top) &&
        (r.right <= mr.right) && (r.bottom <= mr.bottom) &&
        (r.right - r.left >= 50) && (r.bottom - r.top >= 50) ) {
      CString str;
      str.Format ("%d,%d,%d,%d %d,%d,%d,%d", r.left, r.top, r.right, r.bottom, mr.left, mr.top, mr.right, mr.bottom);
      pApp->WriteProfileString (szSettings, szWindowPlacement, str);
      }
    }

  //  save toolbar docking state
  CDockState dockstate;
  GetDockState (dockstate);
  dockstate.SaveState (szDockState);

  //  save find strings
  CString str, key;
  int max = m_wndFindBar.m_findBox.GetCount();
  for (int n = 1; (n < 10) && (n <= max); n++) {
    m_wndFindBar.m_findBox.GetLBText (n-1, str);
    if (!str.IsEmpty()) {
      key.Format (szFindFmt, n);
      pApp->WriteProfileString (szSearches, key, str);
      }
    }

  //  save replace strings
  max = m_wndFindBar.m_replaceBox.GetCount();
  for (int n = 1; (n < 10) && (n <= max); n++) {
    m_wndFindBar.m_replaceBox.GetLBText (n-1, str);
    if (!str.IsEmpty()) {
      key.Format (szReplaceFmt, n);
      pApp->WriteProfileString (szSearches, key, str);
      }
    }

  CFrameWnd::OnClose();
  }
//}}}
//{{{
void cMainFrame::OnViewFindBar() {

  m_wndFindBar.ShowWindow (m_wndFindBar.IsWindowVisible() ? SW_HIDE : SW_SHOW);
  }
//}}}

//{{{
void cMainFrame::OnLanguage (UINT nID) {

  CMenu* menu = cFedApp::GetSubMenu ("&Options", "&Languages");
  if (!menu || !::IsMenu(menu->m_hMenu))
    return;

  CString name;
  menu->GetMenuString (nID, name, MF_BYCOMMAND);
  if (!name.IsEmpty()) {
    char* str = name.GetBuffer(0);
    char* s = strchr (str, '&');
    if (s)  // strip out the &
      memcpy (s, s+1, strlen (s));
    name.ReleaseBuffer();

    cLanguage* language = cLanguage::FindLanguage (name);
    if (language) {
      cLanguageSheet langsheet (language, this);
      langsheet.DoModal();
      }
    }
  }
//}}}
//{{{
void cMainFrame::OnLanguageNew() {

  cInputDialog dlg ("Enter new language name", this);

  if (dlg.DoModal() == IDOK) {
    if (!dlg.inputStr.IsEmpty()) {
      cLanguage* language = cLanguage::FindLanguage (dlg.inputStr);
      if (language)
        MessageBox ("Language already exists", MB_OK);
      else {
        //{{{  create a new language
        language = cLanguage::CreateLanguage (dlg.inputStr);
        if (!language || !language->Name())
          return;
        //}}}
        cFedApp::AddLanguageToMenu (language->Name());
        language->SaveToRegistry();
        }

      // now call the dialog for the language
      cLanguageSheet langsheet (language, this);
      langsheet.DoModal();
      }
    }
  }
//}}}
//{{{
void cMainFrame::OnLanguageLoad() {

  cFedApp* pApp = (cFedApp*) AfxGetApp();
  ASSERT_VALID(pApp);
  CString filename;
  filename = pApp->GetProfileString (szSettings, szDir, "C:");

  if (!DoPromptFileName (filename, AFX_IDS_OPENFILE, OFN_FILEMUSTEXIST, TRUE, TRUE))
    return; // open cancelled

  if (cLanguage::ReadIniFile (filename)) {
    cLanguage::SaveAllToRegistry();
    pApp->AddAllLanguagesToMenu();

    CString msg;
    msg.Format ("Loaded language from file '%s'", filename);
    MessageBox ("Load New Language from File", msg, MB_OK);

    LPTSTR name = filename.GetBuffer(0);
    char* dir = strrchr (name, '\\');
    if (dir && (dir > name)) {
      *dir = 0;
      pApp->WriteProfileString (szSettings, szDir, name);
      }
    filename.ReleaseBuffer(-1);

    cFedDoc::LanguageChanged();
    }
  }
//}}}

//{{{
void cMainFrame::OnKeymapSetup() {

  cKeyMapPage keymap;
  keymap.DoModal();
  }
//}}}
//{{{
void cMainFrame::OnPreferenceSetup() {

  cOptionDialog dlg (this);
  dlg.DoModal();
  }
//}}}
//{{{
void cMainFrame::OnFontSetup() {

  LOGFONT logFont;
  cFedView::GetFont (&logFont);

  CFontDialog dlg (&logFont, CF_APPLY | CF_ENABLEHOOK | CF_FIXEDPITCHONLY, 0, this);
  dlg.m_cf.lpfnHook = (LPCFHOOKPROC)CFHookProc;

  if (dlg.DoModal() == IDOK) {
    dlg.GetCurrentFont (&logFont);
    cFedView::SetFont (&logFont);
    }
  }
//}}}

//{{{
void cMainFrame::OnInitMenu (CMenu* pMenu) {

  CFrameWnd::OnInitMenu(pMenu);
  }
//}}}
//{{{
void cMainFrame::OnUpdateFindBarMenu (CCmdUI* pCmdUI) {

  pCmdUI->SetCheck (m_wndFindBar.IsWindowVisible());
  }
//}}}

//{{{
UINT APIENTRY cMainFrame::CFHookProc (HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) {

  int res = 0;
  if (uiMsg == WM_COMMAND) {
    int wNotifyCode = HIWORD (wParam);
    int wID = LOWORD (wParam);
    if ((wID == 0x0402) && (wNotifyCode == BN_CLICKED)) {
      // APPLY button
      LOGFONT logFont;
      ::SendMessage (hdlg, WM_CHOOSEFONT_GETLOGFONT, 0, (LPARAM) &logFont);
      cFedView::SetFont (&logFont);
      }
    }

  return 0;
  }
//}}}
