// MacroPage.cpp
//{{{  includes
#include "pch.h"
#include "MacroPage.h"

#include "KeyMap.h"
#include "Language.h"
#include "LanguageSheet.h"
//}}}

//{{{
cMacroPage::cMacroPage()
    : CPropertyPage(cMacroPage::IDD), m_language (NULL), m_sheet (NULL), m_changed (false) {}
//}}}
cMacroPage::~cMacroPage() {}

//{{{
bool cMacroPage::UpdateCurrentMacro() {

  if (m_changed) {
    // get current macro name
    CListBox* listbox;
    listbox = (CListBox*) GetDlgItem (IDC_MACRO_LIST);
    ASSERT_VALID(listbox);

    CString macro_str;
    listbox->GetText (listbox->GetCurSel(), macro_str);

    if (m_language) {
      cMacro* macro = m_language->FindMacro (macro_str);
      if (macro) {
        CEdit* editbox = (CEdit*) GetDlgItem (IDC_MACRO_EDIT);
        ASSERT_VALID(editbox);

        editbox->GetWindowText (macro_str);
        int error_range = macro->SetMacro (macro_str);
        if (error_range) {
          editbox->SetSel (error_range, true);
          editbox->SetActiveWindow();
          return false;
          }
        }
      }
    }

  m_changed = false;
  return true;
  }
//}}}
//{{{
void cMacroPage::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  if (!m_language) return;

  if (pDX->m_bSaveAndValidate) {
    }
  else {
    CListBox* listbox = (CListBox*) GetDlgItem (IDC_MACRO_LIST);
    ASSERT_VALID(listbox);
    listbox->ResetContent();

    bool loaded = false;
    cMacro* macro = m_language->FirstMacro();
    while (macro) {
      if (macro->name) {
        listbox->AddString (macro->name);
        loaded = true;
        }
      macro = macro->next;
      }

    CWnd* item;

    //item = GetDlgItem (IDC_MACRO_NEW);
    //ASSERT (item);
    //item->EnableWindow(FALSE);

    item = GetDlgItem (IDC_MACRO_REMOVE);
    ASSERT (item);
    item->EnableWindow(TRUE);

    item = GetDlgItem (IDC_MACRO_LOAD);
    ASSERT (item);
    item->EnableWindow(FALSE);

    item = GetDlgItem (IDC_MACRO_SAVE);
    ASSERT (item);
    item->EnableWindow(FALSE);

    }
  //{{AFX_DATA_MAP(cMacroPage)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}
//}}}

//{{{
BOOL cMacroPage::OnApply() {

  return UpdateCurrentMacro();
  }
//}}}
//{{{
void cMacroPage::OnOK() {

  m_sheet->SaveLanguage();
  }
//}}}

IMPLEMENT_DYNCREATE(cMacroPage, CPropertyPage)
//{{{
BEGIN_MESSAGE_MAP(cMacroPage, CPropertyPage)
  ON_BN_CLICKED(IDC_REMOVE, OnRemove)
  ON_BN_CLICKED(IDC_GETLEARN, OnGetLearn)

  ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
  ON_LBN_SELCANCEL(IDC_LIST, OnSelcancelList)

  ON_LBN_SELCHANGE(IDC_MACROKEYS_LIST, OnSelchangeKeysList)
  ON_LBN_SELCANCEL(IDC_MACROKEYS_LIST, OnSelcancelKeysList)

  ON_EN_CHANGE(IDC_MACRO_EDIT, OnChangedMacroEdit)
END_MESSAGE_MAP()
//}}}

//{{{
void cMacroPage::OnRemove() {

  CString name;
  CListBox* list = (CListBox*) GetDlgItem (IDC_LIST);
  ASSERT_VALID(list);
  int index = list->GetCurSel();
  list->GetText (index, name);
  if (name .IsEmpty())
    return;

  CListBox* keylist = (CListBox*) GetDlgItem (IDC_MACROKEYS_LIST);
  ASSERT_VALID(keylist);
  int key_index = keylist->GetCurSel();

  if (key_index == LB_ERR) {
    // delete the macro
    m_language->delete_macro (name);

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
    }
  else {
    // delete a key from the the macro
    cMacro* macro = m_language->FindMacro (name);
    name.Empty();
    keylist->GetText (key_index, name);
    if (!macro || name.IsEmpty())
      return;

    int key = cKeyMap::FindKey (name);
    macro->DeleteKey (key);

    keylist->DeleteString (key_index);
    int max_index = keylist->GetCount() - 1;
    if (max_index < 0)
      GetDlgItem (IDC_REMOVE)->EnableWindow(FALSE);
    else {
      if (key_index > max_index)
        key_index = max_index;
      keylist->SetCurSel (key_index);
      OnSelchangeKeysList();
      }
    }

  m_changed = false;
  SetModified();
  }
//}}}
//{{{
void cMacroPage::OnGetLearn() {

  cLanguage* gen = cLanguage::m_genericLanguage;
  cMacro* macro = gen->FindMacro ("Temp");
  if (macro && m_language) {
    if (!UpdateCurrentMacro())
      return;

    m_language->add_macro (*macro);
    CListBox* listbox = (CListBox*) GetDlgItem (IDC_LIST);
    ASSERT_VALID(listbox);

    m_cursel = listbox->AddString (macro->name);
    listbox->SetCurSel (m_cursel);

    OnSelchangeList();
    }
  }
//}}}

//{{{
void cMacroPage::OnSelchangeList() {

  CListBox* listbox;
  listbox = (CListBox*) GetDlgItem (IDC_MACRO_LIST);
  ASSERT_VALID(listbox);

  if (!UpdateCurrentMacro()) {
    listbox->SetCurSel (m_cursel);
    return;
    }

  m_cursel = listbox->GetCurSel();
  CString macro_str;
  listbox->GetText (m_cursel, macro_str);

  if (m_language) {
    cMacro* macro = m_language->FindMacro (macro_str);
    if (macro) {
      listbox = (CListBox*) GetDlgItem (IDC_MACROKEYS_LIST);
      ASSERT_VALID(listbox);
      listbox->ResetContent();

      for (int n = 0; n < 8; n++) {
        int key = macro->keys[n];
        if (key > 0) {
          char str [40];
          sprintf (str, "%s%s", cKeyMap::StateName(key / 128), cKeyMap::KeyName(key % 128));
          listbox->AddString (str);
        }
      }

      CEdit* editbox = (CEdit*) GetDlgItem (IDC_MACRO_EDIT);
      ASSERT_VALID(editbox);
      macro->GetMacro (macro_str, true);
      if (macro_str)
        editbox->SetWindowText (macro_str);
      else
        editbox->SetWindowText ("");
    }
  }

  //enable delete button
  CWnd* item = GetDlgItem (IDC_MACRO_REMOVE);
  ASSERT (item);

  item->SetWindowText("Remove Macro");
  item->EnableWindow(TRUE);
  }
//}}}
//{{{
void cMacroPage::OnSelcancelList() {

  CListBox* listbox;
  listbox = (CListBox*) GetDlgItem (IDC_MACRO_LIST);
  ASSERT_VALID(listbox);

  if (!UpdateCurrentMacro()) {
    listbox->SetCurSel (m_cursel);
    return;
    }

  m_cursel = -1;

  listbox = (CListBox*) GetDlgItem (IDC_MACROKEYS_LIST);
  ASSERT_VALID(listbox);
  listbox->ResetContent();

  CEdit* editbox = (CEdit*) GetDlgItem (IDC_MACRO_EDIT);
  ASSERT_VALID(editbox);
  editbox->SetWindowText ("");

  //disable delete button
  CWnd* item = GetDlgItem (IDC_MACRO_REMOVE);
  ASSERT (item);
  item->EnableWindow(FALSE);
  }
//}}}

//{{{
void cMacroPage::OnSelchangeKeysList() {

  // enable delete button
  CWnd* item = GetDlgItem (IDC_MACRO_REMOVE);
  ASSERT (item);
  item->SetWindowText("Remove Macro Key");
  }
//}}}
//{{{
void cMacroPage::OnSelcancelKeysList() {

  //disable delete button
  CWnd* item = GetDlgItem (IDC_MACRO_REMOVE);
  ASSERT (item);
  item->EnableWindow(FALSE);
  }
//}}}

//{{{
void cMacroPage::OnChangedMacroEdit() {

  m_changed = true; // mark current macro as changed
  SetModified();
  }
//}}}
