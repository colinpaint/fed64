#pragma once

#include "resource.h"
#include "ExtensionPage.h"
#include "CommentPage.h"
#include "KeyWordPage.h"
#include "MacroPage.h"

class cLanguageSheet : public CPropertySheet {

  DECLARE_DYNAMIC (cLanguageSheet)

public:
  cLanguageSheet (cLanguage* lang, CWnd* pWndParent = NULL);
  cLanguageSheet (CWnd* pWndParent = NULL);
  virtual ~cLanguageSheet();

  virtual BOOL OnInitDialog();

  void SaveLanguage();
  BOOL AddLanguage (const char* name);
  BOOL RemoveLanguage();

  cLanguage* m_original_language;
  cLanguage* m_language;
  cExtensionPage m_extensionPage;
  cCommentPage   m_commentPage;
  cKeyWordPage   m_keyWordPage;
  cMacroPage     m_macroPage;

protected:
  DECLARE_MESSAGE_MAP()
  };
