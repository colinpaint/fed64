#pragma once

#include "resource.h"
class cLanguage;
class cLanguageSheet;

class cCommentPage : public CPropertyPage {
  DECLARE_DYNCREATE(cCommentPage)

public:
  cCommentPage();
  ~cCommentPage();

  cLanguage* m_language;
  cLanguageSheet* m_sheet;

  enum { IDD = IDD_COMMENT_PAGE };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnOK();

  void OnChanged() { SetModified(); }

  DECLARE_MESSAGE_MAP()
  };
