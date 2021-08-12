#pragma once

#include "resource.h"
class cLanguage;
class cLanguageSheet;

class cKeyWordPage : public CPropertyPage {
  DECLARE_DYNCREATE(cKeyWordPage)

public:
  cKeyWordPage();
  ~cKeyWordPage();

  enum { IDC_LIST   = IDC_KEYWORD_LIST,
         IDC_NEW    = IDC_KEYWORD_NEW,
         IDC_REMOVE = IDC_KEYWORD_REMOVE
       };
  enum { IDD = IDD_KEYWORD_PAGE };

  cLanguage* m_language;
  cLanguageSheet* m_sheet;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnOK();

  void OnChanged() { SetModified(); }

  afx_msg void OnRemove();
  afx_msg void OnNew();
  afx_msg void OnSelchangeList();
  afx_msg void OnSelcancelList();

  DECLARE_MESSAGE_MAP()
};
