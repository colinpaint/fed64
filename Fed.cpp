// Fed.cpp
//{{{  includes
#include "pch.h"
#include "Fed.h"

#include "MainFrm.h"
#include "FedDoc.h"
#include "FedView.h"
#include "Option.h"
#include "Colour.h"
#include "Language.h"
#include "KeyMap.h"
//}}}
extern BOOL AFXAPI AfxFullPath (LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

const char szFedClass[] = "FedEditorClass";

cFedApp theApp;

//{{{
class cUniqueLetter
{
public:
  long int letters_used;     // bitmap of ascii A-Z
  long int numbers_used;     // bitmap of ascii 0-9
  cUniqueLetter() : letters_used(0), numbers_used(0) {}
  cUniqueLetter(const char* charlist);

  bool isAvailable (char ch);
  bool isCharAvailable (char ch);
  void SetUsed (char ch);
  void SetUsed (const char* charlist);
  void ObtainUniqueLetter (CString& str);
};

//{{{
cUniqueLetter::cUniqueLetter (const char* charlist)
 : letters_used(0),
   numbers_used(0)
{
  SetUsed (charlist);
}
//}}}
//{{{
bool cUniqueLetter::isAvailable (char ch)
{
  if ((ch >= '0') && (ch <= '9'))
    return numbers_used & (1 << (ch - 32)) ? false : true;
  else if ((ch >= 'A') && (ch <= 'Z'))
    return letters_used & (1 << (ch - 64)) ? false : true;
  else if ((ch >= 'a') && (ch <= 'z'))
    return letters_used & (1 << (ch - 96)) ? false : true;
  return false;
}
//}}}
//{{{
bool cUniqueLetter::isCharAvailable (char ch)
{ // any char except space and underscore
  if ((ch > 32) && (ch <= 63))
    return numbers_used & (1 << (ch - 32)) ? false : true;
  else if ((ch >= 64) && (ch < 95))
    return letters_used & (1 << (ch - 64)) ? false : true;
  else if ((ch >= 96) && (ch < 127))
    return letters_used & (1 << (ch - 96)) ? false : true;
  return false;
}
//}}}
//{{{
void cUniqueLetter::SetUsed (char ch)
{
  if ((ch >= 32) && (ch <= 63))
    numbers_used |= 1 << (ch - 32);
  else if ((ch >= 64) && (ch <= 95))
    letters_used |= 1 << (ch - 64);
  else if ((ch >= 96) && (ch <= 127))
    letters_used |= 1 << (ch - 96);
}
//}}}
//{{{
void cUniqueLetter::SetUsed (const char* charlist)
{
  while (charlist && *charlist)
    SetUsed (*charlist++);
}
//}}}
//{{{
void cUniqueLetter::ObtainUniqueLetter (CString& str)
{ // for use in MENUs
// intelligently finds a unique letter
// then modifies str to insert an '&' before that letter

  if (!str.IsEmpty()) {
    int newlen = str.GetLength() + 1;
    char* buf = str.GetBuffer (newlen);
    // first check for an available word start
    char* s = buf;
    while (*s && !isAvailable (*s)) {
      while (*s > ' ') s++;
      while (*s == ' ') s++;
      }

    if (!*s) {
      // find an available phrase start
      s = buf;
      while (*s && !isAvailable (*s)) {
        while (isalnum (*s)) s++;
        while (*s && !isalnum (*s)) s++;
        }
      }

    if (!*s) {
      // find first letter available
      s = buf;
      while (*s && !isAvailable (*s))
        s++;
      }

    if (!*s) {
      // find first char available
      s = buf;
      while (*s && !isCharAvailable (*s))
        s++;
      }

    if (*s) {
      SetUsed (*s);
      memmove (s+1, s, strlen (s) + 1);   // make space
      *s = '&';
      }
    str.ReleaseBuffer();
    }
}
//}}}
//}}}

int cFedApp::m_nOpenMsg = RegisterWindowMessage(_T("FedEditorOpenMessage"));
BOOL cFedApp::NewInstance = FALSE;
BOOL cFedApp::ReadOnly = FALSE;
int cFedApp::GotoLineNumber = 0;
CString cFedApp::InitialMessage ("InitialMessage");

// cFedApp
//{{{
BEGIN_MESSAGE_MAP(cFedApp, CWinApp)
END_MESSAGE_MAP()
//}}}

//{{{
cFedApp::cFedApp() {}
//}}}
//{{{
cFedApp::~cFedApp()
{
  cKeyMap::Terminate();
  cLanguage::Terminate();
}
//}}}
//{{{
BOOL cFedApp::InitInstance() {

SetRegistryKey(_T("ColinFed"));
cOption::LoadStartOptionsFromRegistry();

// Parse command line for standard shell commands, DDE, file open
CCommandLineInfo cmdInfo;
//{{{  parse command line for flags that ParseCommandLine ignores!!
// first check if any flags were set
BOOL bFlag = FALSE;
NewInstance = !cOption::SingleInstance;
ReadOnly = FALSE;
GotoLineNumber = 0;

for (int i = 1; i < __argc; i++) {
  LPCTSTR pszParam = __targv[i];
  if (pszParam[0] == '-' || pszParam[0] == '/') {
    bFlag = TRUE;
    ++pszParam;
    if ((*pszParam == 'r') || (*pszParam == 'R'))
      ReadOnly = TRUE;
    else if ((*pszParam == 'i') || (*pszParam == 'I'))
      NewInstance = TRUE;
    else if ((*pszParam == 'g') || (*pszParam == 'G')) {
      ++pszParam;
      GotoLineNumber = 0;
      while ((*pszParam >= '0') && (*pszParam <= '9'))
        GotoLineNumber = GotoLineNumber * 10 + (*pszParam++ - '0');
      }
    }
  }
//}}}
ParseCommandLine (cmdInfo);

if (!NewInstance && ::FindWindow(szFedClass, NULL) && IsDocOpen(cmdInfo.m_strFileName))
  return FALSE;

LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

#ifndef _DEBUG
  //{{{  register the .fed association
  char path [_MAX_PATH];
  *path = '\"';
  GetModuleFileName (0, path+1, 255);
  char* pathend = strchr (path, 0);
  *pathend++ = '\"';
  *pathend = 0;

  HKEY hKeyExt = NULL;
  HKEY hKeyFile = NULL;
  DWORD dw;
  if (RegCreateKeyEx(HKEY_CLASSES_ROOT, ".fed", 0, REG_NONE,
          REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
          &hKeyExt, &dw) == ERROR_SUCCESS) {
    bool setkey = true;
    if (dw != REG_CREATED_NEW_KEY) {
      BYTE value [MAX_PATH+1];
      dw = MAX_PATH;
      if (RegQueryValueEx (hKeyExt, 0, 0, 0, value, &dw) == ERROR_SUCCESS) {
        if (dw > 0) {
          if (strcmp ((char*) value, "fedfile") == 0)
            setkey = false;
          else
            AfxGetApp()->DelRegTree(HKEY_CLASSES_ROOT, value);
          }
        }
      }
    if (setkey)
      RegSetValueEx (hKeyExt, 0, 0, REG_SZ, (const BYTE*) "fedfile", 7 + 1);

    if (RegCreateKeyEx (HKEY_CLASSES_ROOT, "fedfile", 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
            &hKeyFile, &dw) == ERROR_SUCCESS) {
      if (dw == REG_CREATED_NEW_KEY)
        RegSetValueEx (hKeyFile, 0, 0, REG_SZ, (const BYTE*) "Fed Project", 11 + 1);
      HKEY hKey = NULL;
      if (RegCreateKeyEx (hKeyFile, "DefaultIcon", 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
            &hKey, &dw) == ERROR_SUCCESS) {
        if (dw == REG_CREATED_NEW_KEY) {
          strcpy (pathend, ",2");
          RegSetValueEx (hKey, 0, 0, REG_SZ, (const BYTE*) path, (int)strlen(path) + 1);
          }
        RegCloseKey (hKey);
        }
      if (RegCreateKeyEx (hKeyFile, "Shell\\Open\\command", 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
            &hKey, &dw) == ERROR_SUCCESS) {
        strcpy (pathend, " \"%1\"");
        RegSetValueEx (hKey, 0, 0, REG_SZ, (const BYTE*) path, (int)strlen(path) + 1);
        RegCloseKey (hKey);
        if (RegOpenKeyEx (hKeyFile, "Shell", 0, KEY_WRITE|KEY_READ, &hKey) == ERROR_SUCCESS) {
          RegSetValueEx (hKey, 0, 0, REG_SZ, (const BYTE*) "Open", 4 + 1);
          RegCloseKey (hKey);
          }
        }
      if (cOption::SingleInstance) {
        if (RegCreateKeyEx (hKeyFile, "Shell\\Open With New Fed\\command", 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
            &hKey, &dw) == ERROR_SUCCESS) {
          strcpy (pathend, " /i \"%1\"");
          RegSetValueEx (hKey, 0, 0, REG_SZ, (const BYTE*) path, (int)strlen(path) + 1);
          RegCloseKey (hKey);
          }
        }
      }
    if (hKeyFile != NULL)
      RegCloseKey (hKeyFile);
    }
  if (hKeyExt != NULL)
    RegCloseKey (hKeyExt);
  //}}}
#endif

#ifdef _DEBUG
  FILE* fp = fopen ("n:\\Tools\\Fed\\FedReg.log", "w");
  if (fp) {
    HKEY hAppKey = GetAppRegistryKey();
    if (hAppKey != NULL) {
      log_keys (fp, hAppKey, 0);
      ::RegCloseKey (hAppKey);
      }
    fclose (fp);
    }
#endif

  cOption::LoadFromRegistry();
  cColour::InitColours();
  cLanguage::InitLanguages();

  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate (
    IDR_MAINFRAME, RUNTIME_CLASS(cFedDoc), RUNTIME_CLASS(cMainFrame), RUNTIME_CLASS(cFedView));
  AddDocTemplate (pDocTemplate);

  // Dispatch commands specified on the command line
  if (!ProcessShellCommand (cmdInfo))
    return FALSE;
  //{{{  parse rest of command line that ParseCommandLine ignores!!
  // first check if any flags were set
  BOOL bFirstFile = TRUE;
  for (int n = 1; n < __argc; n++) {
    LPCTSTR pszParam = __targv[n];
    if (pszParam[0] == '-' || pszParam[0] == '/') {
      }
    else if (bFirstFile)  // first file is already loaded by ParseCommandLine
      bFirstFile = FALSE;
    else
      cFedDoc::AddDocument (__targv[n]);
    }
  //}}}

  m_pMainWnd->ShowWindow (SW_SHOW);
  m_pMainWnd->UpdateWindow();

  AddGenericLanguageToMenu();
  AddAllLanguagesToMenu();

  cKeyMap::LoadFromRegistry();
  cFedView::LoadFontFromRegistry();

  //AllocConsole();
  return TRUE;
  }
//}}}

//{{{
BOOL cFedApp::IsDocOpen (LPCTSTR lpszFileName)
{
  if (lpszFileName[0] == NULL)
    return FALSE;

  TCHAR szPath[_MAX_PATH];
  AfxFullPath (szPath, lpszFileName);
  if (ReadOnly || (GotoLineNumber > 0)) {
    int len = (int)strlen(szPath);
    if (ReadOnly && (len+3 <= _MAX_PATH)) {
      strcat (szPath, " -r");
      len += 3;
      }
    if (GotoLineNumber > 0) {
      char str[30];
      sprintf (str, " -g%d", GotoLineNumber);
      if (len+strlen(str) <= _MAX_PATH)
        strcat (szPath, str);
      }
    }

  ATOM atom = GlobalAddAtom (szPath);
  ASSERT(atom != NULL);
  if (atom == NULL)
    return FALSE;

  EnumWindows (StaticEnumProc, (LPARAM)&atom);

  if (atom == NULL)
    return TRUE;

  DeleteAtom (atom);
  return FALSE;
}
//}}}
//{{{
BOOL CALLBACK cFedApp::StaticEnumProc (HWND hWnd, LPARAM lParam)
{
  TCHAR szClassName[30];
  GetClassName (hWnd, szClassName, 30);
  if (lstrcmp (szClassName, szFedClass) != 0)
    return TRUE;

  ATOM* pAtom = (ATOM*)lParam;
  ASSERT (pAtom != NULL);

  DWORD_PTR dw = NULL;
  ::SendMessageTimeout(hWnd, m_nOpenMsg, (WPARAM)TRUE, (LPARAM)*pAtom, SMTO_ABORTIFHUNG, 500, &dw);
  if (dw) {
    // parse rest of command line that ParseCommandLine ignores, first check if any flags were set
    TCHAR szPath[_MAX_PATH];
    BOOL bFirstFile = TRUE;
    for (int n = 1; n < __argc; n++) {
      LPCTSTR pszParam = __targv[n];
      if (pszParam[0] == '-' || pszParam[0] == '/') {
        }
      else if (bFirstFile)  // first file is already loaded by ParseCommandLine
        bFirstFile = FALSE;
      else {
        AfxFullPath (szPath, pszParam);
        ATOM atom = GlobalAddAtom (szPath);
        ASSERT(atom != NULL);
        if (atom != NULL) {
          ::SendMessageTimeout(hWnd, m_nOpenMsg, (WPARAM)FALSE, (LPARAM)(&atom),
              SMTO_ABORTIFHUNG, 500, &dw);
          DeleteAtom (atom);
          }
        }
      }

    ::SetForegroundWindow(hWnd);
    DeleteAtom(*pAtom);
    *pAtom = NULL;
    return FALSE;
    }

  return TRUE;
  }
//}}}

//{{{
CMenu* cFedApp::GetSubMenu (CMenu* menu, const char* label)
{
  if (menu && label && *label) {
    CString str;
    int count = menu->GetMenuItemCount();
    for (int n = 0; n < count; n++) {
      menu->GetMenuString (n, str, MF_BYPOSITION);
      if (str.Compare (label) == 0)
        return menu->GetSubMenu (n);
      }
    }
  return 0;
}
//}}}
//{{{
CMenu* cFedApp::GetSubMenu (const char* label)
{
  CMenu* main_menu = AfxGetMainWnd()->GetMenu();
  ASSERT(main_menu && ::IsMenu(main_menu->m_hMenu));
  return GetSubMenu (main_menu, label);
}
//}}}
//{{{
CMenu* cFedApp::GetSubMenu (const char* label, const char* sub_label)
{
  return GetSubMenu (GetSubMenu (label), sub_label);
}
//}}}

//{{{
BOOL cFedApp::AddLanguageToMenu (const char* name)
{
  if (name && *name) {
    CMenu* menu = cFedApp::GetSubMenu ("&Options", "&Languages");
    if (menu && ::IsMenu(menu->m_hMenu)) {

      CString str;
      int pos = -1;
      int count = menu->GetMenuItemCount();
      for (int n = 0; n < count; n++) {
        menu->GetMenuString (n, str, MF_BYPOSITION);
        int result = str.CompareNoCase (name);
        if (result < 0)
          pos = n;
        else if (result == 0)
          return FALSE;
        }

      menu->AppendMenu (MF_STRING, ID_LANGUAGE0 + count, name);
      return TRUE;
      }
    }
  return FALSE;
}
//}}}
//{{{
BOOL cFedApp::RemoveLanguageFromMenu (const char* name)
{
  if (name && *name) {
    CMenu* menu = cFedApp::GetSubMenu ("&Options", "&Languages");
    if (menu && ::IsMenu(menu->m_hMenu)) {

      CString str;
      int count = menu->GetMenuItemCount();
      for (int n = 0; n < count; n++) {
        menu->GetMenuString (n, str, MF_BYPOSITION);
        if (str.CompareNoCase (name) == 0)
          return menu->RemoveMenu (n, MF_BYPOSITION);
        }
      }
    }
  return FALSE;
}
//}}}
//{{{
void cFedApp::AddGenericLanguageToMenu()
{
  CMenu* menu = GetSubMenu ("&Options", "&Languages");
  ASSERT(menu && ::IsMenu(menu->m_hMenu));

  cUniqueLetter unique ("NL");
  int itemID = ID_LANGUAGE0;
  cLanguage* language = cLanguage::m_genericLanguage;
  if (language && language->Name()) {
    CString entry (language->Name());
    unique.ObtainUniqueLetter (entry);
    menu->AppendMenu (MF_STRING, itemID++, entry);
    menu->AppendMenu (MF_SEPARATOR);
    }
}
//}}}
//{{{
void cFedApp::AddAllLanguagesToMenu()
{
  CMenu* menu = cFedApp::GetSubMenu ("&Options", "&Languages");
  if (menu && ::IsMenu(menu->m_hMenu)) {
    int count = menu->GetMenuItemCount();

    // first remove all existings language menu items
    int index = 0;
    for (int n = 0; n < count; n++) {     // use a for-loop to ensure we cannot hang!
      UINT id = menu->GetMenuItemID (index);
      if (id < 0)
        break;
      if ((id > ID_LANGUAGE0) && (id <= ID_LANGUAGE99))  // don't remove "Generic" = ID_LANGUAGE0
        menu->RemoveMenu (index, MF_BYPOSITION);
        // don't change index as it will now point to 'next' menu item
      else
        index++;
      }

    cUniqueLetter unique ("NLG");
    count = 1;
    cLanguage* language = cLanguage::FirstLanguage();
    while (language) {
      if ((language != cLanguage::m_genericLanguage) && language->Name()) {
        CString entry (language->Name());
        unique.ObtainUniqueLetter (entry);
        menu->AppendMenu (MF_STRING, ID_LANGUAGE0 + count, entry);
        count++;
        }
      language = language->Next();
      }
    }
}
//}}}
//{{{
int cFedApp::GetMenuKeys()
{ // returns a bitmap of Alt-alpha keys used for Menu (where Alt-A = bit0. Alt-Z = bit25)

  int bitmap = 0;
  CMenu* menu = AfxGetMainWnd()->GetMenu();
  ASSERT(menu && ::IsMenu(menu->m_hMenu));
  int count = menu->GetMenuItemCount();

  CString str;
  for (int index = 0; index < count; index++) {
    menu->GetMenuString (index, str, MF_BYPOSITION);
    const char* s = strchr (str, '&');
    if (s) {
      s++;
      if ((*s >= 'A') && (*s <= 'Z'))
        bitmap |= (1 << (int) (*s - 'A'));
      else if ((*s >= 'a') && (*s <= 'z'))
        bitmap |= (1 << (int) (*s - 'a'));
      }
    }
  return bitmap;
}
//}}}

//{{{
void cFedApp::log_keys (FILE* fp, HKEY hKey, int level)
{
  DWORD index = 0;
  char subkey [256];
  DWORD subkey_len = 256;
  char subkey_class [256];
  DWORD subkey_class_len = 256;
  FILETIME subkey_time;

  log_values (fp, hKey, level);

  while (::RegEnumKeyEx (hKey, index++, subkey, &subkey_len, NULL, subkey_class, &subkey_class_len, &subkey_time) == ERROR_SUCCESS) {
    fprintf (fp, "\n%*s[%s]\n", level, "", subkey);
    subkey_len = 256;
    subkey_class_len = 256;
    HKEY hSubKey = NULL;
    if (::RegOpenKeyEx(hKey, subkey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
      log_keys (fp, hSubKey, level+2);
      }
    if (hSubKey != NULL)
      ::RegCloseKey(hSubKey);
    }

  }
//}}}
//{{{
void cFedApp::log_values (FILE* fp, HKEY hKey, int level)
{
  const char* cTypeName [11] = {
    "NONE",
    "SZ",
    "EXPAND_SZ",
    "BINARY",
    "DWORD",
    "DWORD_BE",
    "LINK",
    "MULTI_SZ",
    "RESOURCE_LIST",
    "FULL_RESIOURCE_DESCRIPTOR",
    "RESOURCE_REQUIREMENTS_LIST"
    };
  DWORD index = 0;
  char name [256];
  DWORD name_len = 256;
  DWORD type;
  unsigned char data [2048];
  DWORD data_len = 2048;

  while (::RegEnumValue (hKey, index++, name, &name_len, NULL, &type, data, &data_len) == ERROR_SUCCESS) {
    const char* typestr = (type >= 0) && (type <= 10) ? cTypeName [type] : "???";
    DWORD* pVal = (DWORD*) data;
    switch (type) {
      case REG_SZ:
        fprintf (fp, "%*s%s=%d \"%s\"\n", level, "", name, data_len, data);
        break;
      case REG_DWORD:
        if ((*pVal > 32) && (*pVal < 127))
          fprintf (fp, "%*s%s=%08x %d '%c'\n", level, "", name, *pVal, *pVal, *pVal);
        else if (*pVal > 9)
          fprintf (fp, "%*s%s=%08x %d\n", level, "", name, *pVal, *pVal);
        else
          fprintf (fp, "%*s%s=%08x\n", level, "", name, *pVal);
        break;
      default:
        fprintf (fp, "%*s%s (%s) len=%d\n", level, "", name, typestr, data_len);
        break;
      }
    name_len = 256;
    data_len = 2048;
    }

}
//}}}
