// LanguageSheet.cpp
//{{{  includes
#include "pch.h"
#include "LanguageSheet.h"

#include "Language.h"
#include "Fed.h"
#include "FedDoc.h"
#include "KeyMap.h"
#include "TextProj.h"
//}}}

//{{{
cLanguageSheet::cLanguageSheet(CWnd* pWndParent)
  : CPropertySheet(IDS_LANGSHT_CAPTION, pWndParent),
    m_original_language (NULL),
    m_language (NULL)
{
  // Add all of the property pages here.  Note that
  // the order that they appear in here will be
  // the order they appear in on screen.  By default,
  // the first page of the set is the active one.
  // One way to make a different property page the
  // active one is to call SetActivePage().
  AddPage (&m_extensionPage);
  AddPage (&m_commentPage);
  AddPage (&m_keyWordPage);
  AddPage (&m_macroPage);

  m_extensionPage.m_sheet = this;
  m_commentPage.m_sheet = this;
  m_keyWordPage.m_sheet = this;
  m_macroPage.m_sheet = this;
}
//}}}
//{{{
cLanguageSheet::cLanguageSheet(cLanguage* lang, CWnd* pWndParent)
  : CPropertySheet(lang && lang->Name() ? lang->Name() : "Lang", pWndParent)
{
  // Add all of the property pages here.  Note that
  // the order that they appear in here will be
  // the order they appear in on screen.  By default,
  // the first page of the set is the active one.
  // One way to make a different property page the
  // active one is to call SetActivePage().

  m_original_language = lang;
  m_language = new cLanguage();
  m_language->Set (m_original_language);

  m_extensionPage.m_sheet = this;
  m_commentPage.m_sheet = this;
  m_keyWordPage.m_sheet = this;
  m_macroPage.m_sheet = this;

  m_extensionPage.m_language = m_language;
  m_commentPage.m_language = m_language;
  m_keyWordPage.m_language = m_language;
  m_macroPage.m_language = m_language;

  AddPage (&m_extensionPage);
  AddPage (&m_commentPage);
  AddPage (&m_keyWordPage);
  AddPage (&m_macroPage);
}
//}}}
//{{{
cLanguageSheet::~cLanguageSheet()
{
  if (m_language)
    delete m_language;
}
//}}}

IMPLEMENT_DYNAMIC(cLanguageSheet, CPropertySheet)
//{{{
BEGIN_MESSAGE_MAP (cLanguageSheet, CPropertySheet)
END_MESSAGE_MAP()
//}}}

//{{{
BOOL cLanguageSheet::OnInitDialog()
{
  //int rgiButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

  BOOL bResult = CPropertySheet::OnInitDialog();

  //BOOL bResult = (BOOL)Default();

  // center the property sheet relative to the parent window
  //if (!(GetStyle() & WS_CHILD))
  //  CenterWindow();

  return bResult;
}
//}}}

//{{{
void cLanguageSheet::SaveLanguage()
{
  m_original_language->Set (m_language);
  m_original_language->SaveToRegistry();
  cFedDoc::LanguageChanged (m_original_language);
}
//}}}
//{{{
BOOL cLanguageSheet::AddLanguage (const char* name)
{
  return FALSE;
}
//}}}
//{{{
BOOL cLanguageSheet::RemoveLanguage()
{
  if (m_original_language) {
    CString msg;
    if (cTextProj::isInUse (m_original_language)) {
      msg.Format ("Language '%s' is in use by a File.", m_original_language->Name());
      MessageBox (msg, "Remove Language", MB_OK);
      return FALSE;
      }

    msg.Format ("Remove language '%s'. Are you sure ?", m_original_language->Name());
    if (MessageBox (msg, "Remove Language", MB_OKCANCEL) == IDOK) {
      m_original_language->RemoveFromRegistry();
      cFedApp::RemoveLanguageFromMenu (m_original_language->Name());
      delete m_original_language;
      m_original_language = 0;
      PressButton (PSBTN_CANCEL);
      }
    }
  return FALSE;
}
//}}}
