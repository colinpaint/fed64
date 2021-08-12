// KeyWordPage.cpp
//{{{  includes
#include "pch.h"
#include "KeyWordPage.h"

#include "Dialog.h"
#include "Language.h"
#include "LanguageSheet.h"
//}}}

//{{{
cKeyWordPage::cKeyWordPage()
  : CPropertyPage(cKeyWordPage::IDD), m_language (NULL), m_sheet (NULL) {}
//}}}
//{{{
cKeyWordPage::~cKeyWordPage() {}
//}}}

//{{{
void cKeyWordPage::DoDataExchange(CDataExchange* pDX) {

  CPropertyPage::DoDataExchange(pDX);
  if (!m_language) return;

  CListBox* listbox = (CListBox*) GetDlgItem (IDC_KEYWORD_LIST);
  ASSERT_VALID(listbox);
  CButton* button = (CButton*) GetDlgItem (IDC_CASE_SENSITIVE);
  ASSERT_VALID(button);

  if (pDX->m_bSaveAndValidate) {
    CString list, str;
    //list.Empty();

    int count = listbox->GetCount();
    if (count >= 1) {
      listbox->GetText (0, list);
      for (int n = 1; n < count; n++) {
        listbox->GetText (n, str);
        list += " ";
        list += str;
        }
      }

    m_language->set_keyWords (list);
    m_language->set_case_sensitive (button->GetCheck() ? true : false);
    }
  else {
    const char* list = m_language->m_keyWords;
    listbox->ResetContent();
    char str [80];

    while (list && *list) {
      while (*list == ' ')  // skip leading spaces
        list++;
      char* s = str;
      while (*list > ' ')   // copy next word
        *s++ = *list++;
      if (s > str) {
        *s = 0;
        listbox->AddString (str);
        }
      }
    button->SetCheck (m_language->isCaseSensitive());
    }
  }
//}}}
//{{{
void cKeyWordPage::OnOK() {

  m_sheet->SaveLanguage();
  }
//}}}

IMPLEMENT_DYNCREATE(cKeyWordPage, CPropertyPage)
//{{{
BEGIN_MESSAGE_MAP(cKeyWordPage, CPropertyPage)
  ON_BN_CLICKED(IDC_KEYWORD_REMOVE, OnRemove)
  ON_BN_CLICKED(IDC_KEYWORD_NEW, OnNew)

  ON_LBN_SELCHANGE(IDC_KEYWORD_LIST, OnSelchangeList)
  ON_LBN_SELCANCEL(IDC_KEYWORD_LIST, OnSelcancelList)

  ON_BN_CLICKED(IDC_CASE_SENSITIVE, OnChanged)
END_MESSAGE_MAP()
//}}}

//{{{
void cKeyWordPage::OnRemove() {

  CListBox* list = (CListBox*) GetDlgItem (IDC_LIST);
  ASSERT_VALID(list);
  int index = list->GetCurSel();

  list->DeleteString (index);
  int max_index = list->GetCount() - 1;
  if (max_index < 0)
    GetDlgItem (IDC_REMOVE)->EnableWindow(FALSE);
  else {
    if (index > max_index)
      index = max_index;
    list->SetCurSel (index);
    OnSelchangeList();
    }

  SetModified();
  }
//}}}
//{{{
void cKeyWordPage::OnNew() {

  cInputDialog dlg ("Enter new Keyword", this);

  if (dlg.DoModal() == IDOK) {
    if (!dlg.inputStr.IsEmpty()) {

      CListBox* listbox = (CListBox*) GetDlgItem (IDC_LIST);
      ASSERT_VALID(listbox);

      CString ext;
      int curext = -1;
      const char* s1 = dlg.inputStr;
      while (s1 && *s1) {
        while (*s1 == ' ') s1++;          // skip leading spaces
        const char* s2 = s1;
        while (*s2 > ' ') s2++;           // skip current KeyWord
        if (s2 > s1) {
          ext = dlg.inputStr.Mid ((int) (s1 - dlg.inputStr), (int) (s2 - s1));
          if (listbox->FindStringExact (-1, ext) == LB_ERR)
            curext = listbox->AddString (ext);
          }
        s1 = s2;
        }

      if (curext) {
        SetModified();
        OnSelchangeList();
        }
      }
    }
  }
//}}}

//{{{
void cKeyWordPage::OnSelchangeList() {

  //enable delete button
  CWnd* item = GetDlgItem (IDC_REMOVE);
  ASSERT (item);
  item->EnableWindow(TRUE);
  }
//}}}
//{{{
void cKeyWordPage::OnSelcancelList() {

  //disable delete button
  CWnd* item = GetDlgItem (IDC_REMOVE);
  ASSERT (item);
  item->EnableWindow(FALSE);
  }
//}}}
