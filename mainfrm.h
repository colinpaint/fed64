#pragma once

//{{{
class cFindBar : public CToolBar
{
public:
    CComboBox   m_findBox;
    CComboBox   m_replaceBox;
    CFont       m_font;
};
//}}}

class cMainFrame : public CFrameWnd {
protected:
  cMainFrame();
  DECLARE_DYNCREATE(cMainFrame)

  CSplitterWnd m_wndSplitter;
  CSplitterWnd m_wndSplitter1;

public:
  BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, BOOL initialDir  = FALSE);

  // Overrides
  public:
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  virtual ~cMainFrame();

  void OnLanguageNew();
  void OnLanguageLoad();
  void OnLanguage(UINT nID);
  void OnTool(UINT nID);
  void OnPreferenceSetup();
  void OnKeymapSetup();
  void OnFontSetup();
  void OnViewFindBar();

  void AddSearchStr(const char* str);
  void AddReplaceStr(const char* str);
  void GetSearchStr (CString& str);
  void GetReplaceStr (CString& str);
  void FillSearchBox (CComboBox& combo, bool replace);

protected:
  static UINT APIENTRY CFHookProc (HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

  void OnUpdateFindBarMenu(CCmdUI* pCmdUI);

  void OnUpdateSplitHoriz (CCmdUI* pCmdUI);
  void OnUpdateSplitVert (CCmdUI* pCmdUI);

  void OnSplitHoriz();
  void OnSplitVert();

  LRESULT OnOpenMsg (WPARAM wParam, LPARAM lParam);

  // Generated message map functions
  afx_msg void OnInitMenu (CMenu* pMenu);
  afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnClose();
  afx_msg void OnContextHelp();
  DECLARE_MESSAGE_MAP()

  CToolBar m_wndToolBar;
  cFindBar m_wndFindBar;

  BOOL CreateToolBar();
  BOOL CreateFindBar();
};
