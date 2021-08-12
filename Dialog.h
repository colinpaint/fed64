#pragma once

#include "resource.h"

//{{{
class cGotoDialog : public CDialog {
public:
  cGotoDialog(CWnd* pParent = NULL);

  enum { IDD = IDD_GOTODIALOG };
  static UINT    m_lineNumber;
  static DWORD HelpLinks[];

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_MESSAGE_MAP()
  };
//}}}
//{{{
class cInputDialog : public CDialog {
public:
  cInputDialog (const char* name, CWnd* pParent = NULL);

  enum { IDD = IDD_INPUT_DIALOG };

  CString inputStr;
  CString m_title;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()
  };
//}}}
//{{{
class cMyFindReplaceDialog : public CFindReplaceDialog {
public:
  cMyFindReplaceDialog();

  static char* prev_search_str[5];
  static char* prev_replace_str[5];

  virtual ~cMyFindReplaceDialog() {}
  void SetIgnoreComments (BOOL ignoreComments) { m_ignoreComments = ignoreComments; }
  void SetSearchAll (BOOL searchAll) { m_searchAll = searchAll; }

  BOOL IgnoreComments() { return m_ignoreComments; }
  BOOL SearchAll() { return m_searchAll; }

  BOOL m_searchAll;
  BOOL m_ignoreComments;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnEditChange();

  DECLARE_MESSAGE_MAP()
  };
//}}}
