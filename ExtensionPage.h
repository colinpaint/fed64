#pragma once

#include "resource.h"
class cLanguage;
class cLanguageSheet;

class cExtensionPage : public CPropertyPage {
  DECLARE_DYNCREATE(cExtensionPage)

public:
  cExtensionPage();
  ~cExtensionPage();

  enum { IDC_LIST   = IDC_EXT_LIST,
         IDC_NEW    = IDC_EXT_NEW,
         IDC_REMOVE = IDC_EXT_REMOVE };

  enum { IDD = IDD_EXTENSION_PAGE };

  cLanguage* m_language;
  cLanguageSheet* m_sheet;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnOK();

  BOOL DoPromptFileName (CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog);

  afx_msg void OnLanguageRemove();
  afx_msg void OnLanguageRename();
  afx_msg void OnLanguageLoad();
  afx_msg void OnLanguageSave();
  afx_msg void OnRemove();
  afx_msg void OnNew();
  afx_msg void OnSelchangeList();
  afx_msg void OnSelcancelList();
  afx_msg void OnExtRegister();
  afx_msg void OnExtUnregister();
  afx_msg void OnExtRegisterAll();
  afx_msg void OnExtUnregisterAll();
  afx_msg void OnSelchangeExttypeList();
  afx_msg void OnSelcancelExttypeList();

  DECLARE_MESSAGE_MAP()

private:
  BOOL m_remove_opens;
};
