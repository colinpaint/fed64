#pragma once

#include "resource.h"
class cLanguage;
class cLanguageSheet;

class cMacroPage : public CPropertyPage {

  DECLARE_DYNCREATE(cMacroPage)
public:
  cLanguage* m_language;
  cLanguageSheet* m_sheet;
  bool m_changed;
  int m_cursel;

  cMacroPage();
  ~cMacroPage();

  enum { IDC_LIST     = IDC_MACRO_LIST,
         IDC_NEW      = IDC_MACRO_NEW,
         IDC_REMOVE   = IDC_MACRO_REMOVE,
         IDC_GETLEARN = IDC_MACRO_GETLEARN
       };
  enum { IDD = IDD_MACRO_PAGE };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnApply();
  virtual void OnOK();

  afx_msg void OnRemove();
  afx_msg void OnNew();
  afx_msg void OnGetLearn();
  afx_msg void OnSelchangeList();
  afx_msg void OnSelcancelList();
  afx_msg void OnSelchangeKeysList();
  afx_msg void OnSelcancelKeysList();

  void OnChanged() { SetModified(); }
  void OnChangedMacroEdit();
  bool UpdateCurrentMacro();

  DECLARE_MESSAGE_MAP()
  };
