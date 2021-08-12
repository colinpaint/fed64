// ExtensionPage.cpp
//{{{  includes
#include "pch.h"
#include "ExtensionPage.h"

#include "Dialog.h"
#include "Language.h"
#include "LanguageSheet.h"
#include "Fed.h"
//}}}
//{{{  const strings
const char szSettings[] = "Settings";
const char szDir[] = "Language_Dir";
const char szOpenCmd[] = "Shell\\Open With &Fed\\command";
//}}}

//{{{
cExtensionPage::cExtensionPage()
  : CPropertyPage(cExtensionPage::IDD),
    m_remove_opens(false),
    m_language (NULL),
    m_sheet (NULL)
{
}
//}}}
cExtensionPage::~cExtensionPage() {}

//{{{
void cExtensionPage::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  if (!m_language) return;

  if (m_language == cLanguage::m_genericLanguage) {
    CButton* button = static_cast<CButton*>(GetDlgItem (IDC_LANGUAGE_REMOVE));
    ASSERT_VALID(button);
    button->EnableWindow (FALSE);
    button = static_cast<CButton*>(GetDlgItem (IDC_LANGUAGE_RENAME));
    ASSERT_VALID(button);
    button->EnableWindow (FALSE);
    }

  CListBox* listbox = (CListBox*) GetDlgItem (IDC_EXT_LIST);
  ASSERT_VALID(listbox);

  if (pDX->m_bSaveAndValidate) {
    CString list, str;
    int count = listbox->GetCount();
    if (count >= 1) {
      listbox->GetText (0, list);
      for (int n = 1; n < count; n++) {
        listbox->GetText (n, str);
        list += " ";
        list += str;
        }
      }

    m_language->set_extensions (list);
    }
  else {
    //{{{  load extension list box with all extensions
    const char* list = m_language->m_extensions;
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
    //}}}
    BOOL has_exts = (listbox->GetCount() > 0);

    //enable/disable 'Register All Extensions' button
    CWnd* button = GetDlgItem (IDC_EXT_REGISTER_ALL);
    ASSERT (button);
    button->EnableWindow(has_exts);

    //enable/disable 'Unregister All Extensions' button
    button = GetDlgItem (IDC_EXT_UNREGISTER_ALL);
    ASSERT (button);
    button->EnableWindow(FALSE);
    }

  DDX_Check (pDX, IDC_REMOVEOPENS, m_remove_opens);
}
//}}}
//{{{
void cExtensionPage::OnOK()
{
  m_sheet->SaveLanguage();
}
//}}}

//{{{
BOOL cExtensionPage::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog)
{
  CFileDialog dlgFile (bOpenFileDialog, "ini", 0);

  CString title;
  VERIFY (title.LoadString(nIDSTitle));

  dlgFile.m_ofn.Flags |= lFlags;

  CString strFilter;

  // append the "*.ini" project file filter
  strFilter += _T("Fed INI (*.ini)");
  strFilter += (TCHAR)'\0';   // next string please
  strFilter += _T("*.ini");
  strFilter += (TCHAR)'\0';   // next string please

  dlgFile.m_ofn.lpstrFilter = strFilter;
  dlgFile.m_ofn.lpstrTitle = title;
  dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

  BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
  fileName.ReleaseBuffer();
  return bResult;
}
//}}}

IMPLEMENT_DYNCREATE(cExtensionPage, CPropertyPage)
//{{{
BEGIN_MESSAGE_MAP(cExtensionPage, CPropertyPage)
  ON_BN_CLICKED(IDC_LANGUAGE_REMOVE, OnLanguageRemove)
  ON_BN_CLICKED(IDC_LANGUAGE_RENAME, OnLanguageRename)

  ON_BN_CLICKED(IDC_LANGUAGE_LOAD, OnLanguageLoad)
  ON_BN_CLICKED(IDC_LANGUAGE_SAVE, OnLanguageSave)

  ON_BN_CLICKED(IDC_EXT_REMOVE, OnRemove)
  ON_BN_CLICKED(IDC_EXT_NEW, OnNew)

  ON_LBN_SELCHANGE(IDC_EXT_LIST, OnSelchangeList)
  ON_LBN_SELCANCEL(IDC_EXT_LIST, OnSelcancelList)

  ON_BN_CLICKED(IDC_EXT_REGISTER, OnExtRegister)
  ON_BN_CLICKED(IDC_EXT_UNREGISTER, OnExtUnregister)

  ON_BN_CLICKED(IDC_EXT_REGISTER_ALL, OnExtRegisterAll)
  ON_BN_CLICKED(IDC_EXT_UNREGISTER_ALL, OnExtUnregisterAll)
END_MESSAGE_MAP()
//}}}

//{{{
void cExtensionPage::OnLanguageRename()
{
  cInputDialog dlg ("Enter new name for this language", this);
  dlg.inputStr = m_language->Name();

  if (dlg.DoModal() == IDOK) {
    if (!dlg.inputStr.IsEmpty() && (strcmp (dlg.inputStr, m_language->Name()) != 0)) {
      if (cLanguage::FindLanguage (dlg.inputStr)) {
        MessageBox ("Name already used", "Rename Language", MB_OK);
        }
      else {
        cFedApp::RemoveLanguageFromMenu (m_language->Name());

        m_language->RemoveFromRegistry();
        m_language->set_name (dlg.inputStr);
        m_language->SaveToRegistry();

        m_sheet->SetTitle (m_language->Name());

        cFedApp::AddLanguageToMenu (m_language->Name());
        }
      }
    }
}
//}}}
//{{{
void cExtensionPage::OnLanguageRemove()
{
  if (m_sheet)
    m_sheet->RemoveLanguage();
}
//}}}

//{{{
void cExtensionPage::OnLanguageLoad()
{
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);
  CString filename;
  filename = pApp->GetProfileString (szSettings, szDir, "C:");
  filename += "\\Lang ";
  filename += m_language->Name();
  filename += ".ini";

  if (!DoPromptFileName (filename, AFX_IDS_OPENFILE, OFN_FILEMUSTEXIST, TRUE))
    return; // open cancelled

  if (cLanguage::ReadIniFile (filename)) {
    cLanguage::SaveAllToRegistry();

    CString msg;
    msg.Format ("Loaded languages from file '%s'", filename);
    MessageBox ("Load New Languages from File", msg, MB_OK);

    LPTSTR name = filename.GetBuffer(0);
    char* dir = strrchr (name, '\\');
    if (dir && (dir > name)) {
      *dir = 0;
      pApp->WriteProfileString (szSettings, szDir, name);
      }
    filename.ReleaseBuffer(-1);

    m_sheet->PressButton (PSBTN_CANCEL);
    }
}
//}}}
//{{{
void cExtensionPage::OnLanguageSave()
{
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  CString filename;
  filename = pApp->GetProfileString (szSettings, szDir, "C:");
  filename += "\\Lang ";
  filename += m_language->Name();
  filename += ".ini";

  if (!DoPromptFileName (filename, AFX_IDS_SAVEFILE, 0, FALSE))
    return; // save cancelled

  //{{{  create a backup file
  char backupname [MAX_PATH + 1];
  strcpy (backupname, filename);
  char* ext = strrchr (backupname, '.');
  if (ext) {
    ext++;
    size_t len = strlen (ext);
    if (len < 3)
      memmove (ext+1, ext, len+1);
    *ext = '~';
    }
  else
    strcat (backupname, ".~");

  ::DeleteFile (backupname);
  ::MoveFile (filename, backupname);
  //}}}

  FILE* fp = fopen (filename, "w");

  if (fp && m_language->SaveToFile (fp)) {
    CString msg;
    msg.Format ("Saved to file '%s'", filename);
    MessageBox (msg, "Save Language to File", MB_OK);

    LPTSTR name = filename.GetBuffer(0);
    char* dir = strrchr (name, '\\');
    if (dir && (dir > name)) {
      *dir = 0;
      pApp->WriteProfileString (szSettings, szDir, name);
      }
    filename.ReleaseBuffer(-1);
    }

  if (fp)
    fclose (fp);
}
//}}}

//{{{
void cExtensionPage::OnRemove()
{
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
void cExtensionPage::OnNew()
{
  const char illegal_chars [] = "\\/:*?\"<>|.\t";

  cInputDialog dlg ("Enter new extension", this);

  if (dlg.DoModal() == IDOK) {
    if (!dlg.inputStr.IsEmpty()) {
      dlg.inputStr.MakeLower();
      if (dlg.inputStr.FindOneOf (illegal_chars) >= 0) {
        CString msg;
        msg.Format ("An extension cannot contain any of the following characters:\n %s",
                     illegal_chars);
        MessageBox (msg, "New Extension", MB_ICONERROR);
        return;
        }

      CListBox* listbox = (CListBox*) GetDlgItem (IDC_LIST);
      ASSERT_VALID(listbox);

      CString ext;
      int curext = -1;
      const char* s1 = dlg.inputStr;
      while (s1 && *s1) {
        while (*s1 == ' ') s1++;          // skip leading spaces
        const char* s2 = s1;
        while (*s2 > ' ') s2++;           // skip current extension
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
void cExtensionPage::OnSelchangeList()
{
  //enable 'Remove Extension' button
  CWnd* item = GetDlgItem (IDC_REMOVE);
  ASSERT (item);
  item->EnableWindow(TRUE);

  //enable 'Register Extension' button
  CWnd* register_button = GetDlgItem (IDC_EXT_REGISTER);
  ASSERT (register_button);
  register_button->EnableWindow(TRUE);

  //disable 'Unregister Extension' button
  CWnd* unregister_button = GetDlgItem (IDC_EXT_UNREGISTER);
  ASSERT (unregister_button);
  unregister_button->EnableWindow(FALSE);

  //enable 'Remove other Opens' checkbox
  CWnd* remove_check = GetDlgItem (IDC_REMOVEOPENS);
  ASSERT (remove_check);
  remove_check->EnableWindow(TRUE);

  //locate and clear 'File Description' box
  CEdit* editbox = (CEdit*) GetDlgItem (IDC_EXT_NAME);
  ASSERT_VALID(editbox);
  editbox->SetWindowText (0);

  char ext [MAX_PATH+1] = "";
  CListBox* list = (CListBox*) GetDlgItem (IDC_LIST);
  ASSERT_VALID(list);
  int index = list->GetCurSel();
  list->GetText (index, ext);
  // eg extstr=.cpp   ext=cpp   extend=

  char desc [MAX_PATH+1] = "";
  int desc_len = MAX_PATH;

  if (m_language->IsRegistered (ext, desc, &desc_len)) {
    register_button->EnableWindow(FALSE);
    unregister_button->EnableWindow(TRUE);
    remove_check->EnableWindow(FALSE);
    }
  editbox->SetWindowText (desc);

  //{{{  old registry access
  //char command [256];
  //*command = '\"';
  //GetModuleFileName (0, command+1, 255);
  //strcat (command, "\" \"%1\"");
  //// eg command="c:\Program Files\fed.exe" "%1"

  //HKEY hKeyExt = NULL;
  //BYTE value [MAX_PATH+1];
  //DWORD value_len = MAX_PATH;
  //if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, extstr, 0, KEY_QUERY_VALUE, &hKeyExt) == ERROR_SUCCESS) {
    //if (::RegQueryValueEx (hKeyExt, 0, 0, 0, value, &value_len) == ERROR_SUCCESS) {
      //strcpy (extend, "_auto_file");
      //if (strcmp ((char*) value, ext) != 0) {   // ignore auto associations eg cpp_auto_file
        //HKEY hKeyFile = NULL;   // eg cppfile
        //if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, (char*) value, 0, KEY_QUERY_VALUE, &hKeyFile) == ERROR_SUCCESS) {
          ////read file description and load to 'File Description' box
          //value_len = MAX_PATH;
          //if (::RegQueryValueEx (hKeyFile, 0, 0, 0, value, &value_len) == ERROR_SUCCESS) {
            //editbox->SetWindowText ((char*) value);
            //}
          //HKEY hKeyCmd = NULL;  // = Open with &Fed
          //if (::RegOpenKeyEx (hKeyFile, szOpenCmd, 0, KEY_QUERY_VALUE, &hKeyCmd) == ERROR_SUCCESS) {
            //value_len = MAX_PATH;
            //if (::RegQueryValueEx (hKeyCmd, 0, 0, 0, value, &value_len) == ERROR_SUCCESS) {
              //if (_stricmp ((char*) value, command) == 0) {
                //register_button->EnableWindow(FALSE);
                //unregister_button->EnableWindow(TRUE);
                //}
              //}
            //}
          //if (hKeyCmd != NULL)
            //::RegCloseKey (hKeyCmd);
          //}
        //if (hKeyFile != NULL)
          //::RegCloseKey (hKeyFile);
        //}
      //}
    //}
  //if (hKeyExt != NULL)
    //::RegCloseKey (hKeyExt);
  //}}}
}
//}}}
//{{{
void cExtensionPage::OnSelcancelList()
{
  //disable 'Remove Extension' button
  CWnd* item = GetDlgItem (IDC_REMOVE);
  ASSERT (item);
  item->EnableWindow(FALSE);

  //disable 'Register Extension' button
  CWnd* register_button = GetDlgItem (IDC_EXT_REGISTER);
  ASSERT (register_button);
  register_button->EnableWindow(FALSE);

  //disable 'Unregister Extension' button
  CWnd* unregister_button = GetDlgItem (IDC_EXT_UNREGISTER);
  ASSERT (unregister_button);
  unregister_button->EnableWindow(FALSE);

  //??
  CEdit* editbox = (CEdit*) GetDlgItem (IDC_EXT_NAME);
  ASSERT_VALID(editbox);
  editbox->SetWindowText (0);
}
//}}}

//{{{
void cExtensionPage::OnExtRegister()
{
  char ext [MAX_PATH+1] = "";
  CListBox* list = (CListBox*) GetDlgItem (IDC_LIST);
  ASSERT_VALID(list);
  int index = list->GetCurSel();
  list->GetText (index, ext);

  CEdit* editbox = (CEdit*) GetDlgItem (IDC_EXT_NAME);
  ASSERT_VALID(editbox);
  CString desc;
  editbox->GetWindowText (desc);

  CButton* checkbox = (CButton*) GetDlgItem (IDC_REMOVEOPENS);
  ASSERT_VALID(checkbox);

  if (*ext)
    m_language->RegisterExtension (ext, desc, checkbox->GetCheck() ? true : false);

  OnSelchangeList();
}
//}}}
//{{{
void cExtensionPage::OnExtUnregister()
{
  char ext [MAX_PATH+1] = "";
  CListBox* list = (CListBox*) GetDlgItem (IDC_LIST);
  ASSERT_VALID(list);
  int index = list->GetCurSel();
  list->GetText (index, ext);
  if (*ext) {
    if (m_language->UnregisterExtension (ext)) {
      //locate and clear 'File Description' box
      CEdit* editbox = (CEdit*) GetDlgItem (IDC_EXT_NAME);
      ASSERT_VALID(editbox);
      editbox->SetWindowText (0);
      }
    }

  OnSelchangeList();
}
//}}}

//{{{
void cExtensionPage::OnExtRegisterAll()
{
  m_language->RegisterAllExtensions (m_remove_opens ? true : false);

  OnSelchangeList();
}
//}}}
//{{{
void cExtensionPage::OnExtUnregisterAll()
{
  m_language->UnregisterAllExtensions();

  OnSelchangeList();
}
//}}}
