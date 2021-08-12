#pragma once
#include "resource.h"

//{{{
class cDisplayPage : public CPropertyPage {
  DECLARE_DYNCREATE(cDisplayPage)

public:
  cDisplayPage();
  ~cDisplayPage();

  enum { IDD = IDD_DISPLAY_PAGE };

public:
  virtual void OnOK();
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  afx_msg void OnChanged() { SetModified(); }
  afx_msg void OnChangedWrapmark();

  DECLARE_MESSAGE_MAP()

private:
  BOOL SingleInstance;
  BOOL ShowEditMarks;
  BOOL ShowLeftMargin;
  BOOL ShowWrapMark;
  int  LineWrapLength;
  int  LineWrapShade;
  BOOL SaveWindowPlacement;
  BOOL ShowClosedFoldComments;
  BOOL ShowDebug;
  BOOL SaveOptions;
  };
//}}}
//{{{
class cEditorPage : public CPropertyPage
{
  DECLARE_DYNCREATE(cEditorPage)

public:
  cEditorPage();
  ~cEditorPage();

  enum { IDD = IDD_EDITOR_PAGE };

public:
  virtual void OnOK();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnChanged() { SetModified(); }
  afx_msg void OnChangedCheckfile();

  DECLARE_MESSAGE_MAP()

private:
  BOOL CheckFileIsNewer;
  int  FileNewerDelta;
  BOOL ProtectReadOnly;
  BOOL AllowBackspAtSol;
  BOOL AllowLeftAtSol;
  BOOL SetCursorAtEndOfPaste;

  BOOL CreateBackupFirstSave;
  BOOL CreateBackupEverySave;
  BOOL SuppressBeeps;
  BOOL EnableUndo;
};
//}}}
//{{{
class cOptionDialog : public CPropertySheet
{
  DECLARE_DYNAMIC(cOptionDialog)

public:
  enum { IDD = IDD_OPTION_DIALOG };

  cOptionDialog (CWnd* pParentWnd);
  virtual ~cOptionDialog();

protected:
  DECLARE_MESSAGE_MAP()

private:
  cDisplayPage m_DisplayPage;
  cEditorPage m_EditorPage;
};
//}}}
