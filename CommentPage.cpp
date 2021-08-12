// CommentPage.cpp
//{{{  includes
#include "pch.h"
#include "CommentPage.h"

#include "resource.h"
#include "Language.h"
#include "LanguageSheet.h"
//}}}

//{{{
cCommentPage::cCommentPage()
  : CPropertyPage(cCommentPage::IDD),
    m_language (NULL),
    m_sheet (NULL)
{
}
//}}}
//{{{
cCommentPage::~cCommentPage() {}
//}}}

//{{{
static void DDV_NonAlphanumChar (CDataExchange* pDX, CString& str)
{
  DDV_MaxChars (pDX, str, 1);
  if (!str.IsEmpty()) {
    char ch = str [0];
    if (((ch >= 'a') && (ch <= 'z')) ||
        ((ch >= 'A') && (ch <= 'Z')) ||
        ((ch >= '0') && (ch <= '9')) ||
        (ch <= ' ') ) {

      if (!pDX->m_bSaveAndValidate)
        return;

      AfxMessageBox("Please enter a non-alphnumeric character or blank", MB_ICONEXCLAMATION);
      pDX->Fail();
      }
    }
}
//}}}

//{{{
void cCommentPage::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  if (!m_language) return;

  int tablen = m_language->Tablen(0);
  DDX_Text (pDX, IDC_INDENT_TAB_SIZE, tablen);
  DDV_MinMaxInt (pDX, tablen, 1, 100);
  m_language->set_tablen (tablen, 0);

  tablen = m_language->Tablen(1);
  DDX_Text (pDX, IDC_TEXT_TAB_SIZE, tablen);
  DDV_MinMaxInt (pDX, tablen, 1, 100);
  m_language->set_tablen (tablen, 1);

  CString str, str2;

  str = m_language->m_commentStart1;
  str2 = m_language->m_commentEnd1;
  DDX_Text (pDX, IDC_COMMENT1_START, str);
  DDX_Text (pDX, IDC_COMMENT1_END, str2);
  if (pDX->m_bSaveAndValidate)
    m_language->set_comment1 (str, str2);

  str = m_language->m_commentStart2;
  str2 = m_language->m_commentEnd2;
  DDX_Text (pDX, IDC_COMMENT2_START, str);
  DDX_Text (pDX, IDC_COMMENT2_END, str2);
  if (pDX->m_bSaveAndValidate)
    m_language->set_comment2 (str, str2);

  if (m_language->m_stringChar)
    str = m_language->m_stringChar;
  else
    str = "";
  DDX_Text (pDX, IDC_STRING_BOUNDS, str);
  DDV_NonAlphanumChar (pDX, str);
  if (pDX->m_bSaveAndValidate)
    m_language->set_string_char (str);

  if (m_language->m_characterChar)
    str = m_language->m_characterChar;
  else
    str.Empty();
  DDX_Text (pDX, IDC_CHARACTER_BOUNDS, str);
  DDV_NonAlphanumChar (pDX, str);
  if (pDX->m_bSaveAndValidate)
    m_language->set_character_char (str);

  if (m_language->m_escapeChar)
    str = m_language->m_escapeChar;
  else
    str.Empty();
  DDX_Text (pDX, IDC_STRING_ESCAPE, str);
  DDV_NonAlphanumChar (pDX, str);
  if (pDX->m_bSaveAndValidate)
    m_language->set_escape_char (str);

  CButton* button = (CButton*) GetDlgItem (IDC_USE_TAGGED_COLOUR);
  ASSERT_VALID(button);
  if (pDX->m_bSaveAndValidate)
    m_language->set_use_tagged_colour (button->GetCheck() ? true : false);
  else
    button->SetCheck (m_language->useTaggedColour());
  }
//}}}
//{{{
void cCommentPage::OnOK()
{
  m_sheet->SaveLanguage();
}
//}}}

IMPLEMENT_DYNCREATE(cCommentPage, CPropertyPage)
//{{{
BEGIN_MESSAGE_MAP(cCommentPage, CPropertyPage)
  ON_EN_CHANGE (IDC_COMMENT1_START, OnChanged)
  ON_EN_CHANGE (IDC_COMMENT1_END, OnChanged)
  ON_EN_CHANGE (IDC_COMMENT2_START, OnChanged)
  ON_EN_CHANGE (IDC_COMMENT2_END, OnChanged)
  ON_EN_CHANGE (IDC_STRING_BOUNDS, OnChanged)
  ON_EN_CHANGE (IDC_CHARACTER_BOUNDS, OnChanged)
  ON_EN_CHANGE (IDC_STRING_ESCAPE, OnChanged)
  ON_EN_CHANGE (IDC_INDENT_TAB_SIZE, OnChanged)
  ON_EN_CHANGE (IDC_TEXT_TAB_SIZE, OnChanged)
  ON_EN_CHANGE (IDC_USE_TAGGED_COLOUR, OnChanged)
END_MESSAGE_MAP()
//}}}
