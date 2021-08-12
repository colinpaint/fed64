// Language.cpp
//{{{  includes
#include "pch.h"
#include "language.h"

#include <string>
#include "option.h"
#include "keymap.h"
//}}}
//{{{  static strings
static const int cIncludeLen = 7;
static const char szInclude[]               = "Include";
static const char szLanguages[]             = "Languages";
static const char szExtensions[]            = "Extensions";
static const char szCaseSensitive[]         = "Case_Sensitive";
static const char szUseTaggedColour[]       = "Use_Tagged_Colour";
static const char szKeyWords[]              = "Keywords";
static const char szDefaultCommentStart[]   = "Default_Comment_Start";
static const char szDefaultCommentEnd[]     = "Default_Comment_End";
static const char szAlternateCommentStart[] = "Alternate_Comment_Start";
static const char szAlternateCommentEnd[]   = "Alternate_Comment_End";
static const char szStringBoundsChar[]      = "String_Bounds_Char";
static const char szCharacterBoundsChar[]   = "Character_Bounds_Char";
static const char szStringEscapeChar[]      = "String_Escape_Char";
static const char szIndentTabSize[]         = "Indent_Tab_Size";
static const char szTextTabSize[]           = "Text_Tab_Size";

static const char szShell[] = "shell";
static const char szOpen[] = "Open With &Fed";
static const char szOpenCmd[] = "shell\\Open With &Fed\\command";
static const char szOpenNewCmd[] = "shell\\Open With new Fed\\command";
//}}}
cLanguage* cLanguage::m_firstLanguage = 0;
cLanguage* cLanguage::m_genericLanguage = 0;

//{{{
cLanguage* cLanguage::CreateLanguage (const char* name)
{
  cLanguage* lang = new cLanguage();

  if (name) {
    lang->m_name = new char [strlen (name) + 1];
    if (lang->m_name)
      strcpy (lang->m_name, name);
    }

  if (m_firstLanguage) {
    cLanguage* last = m_firstLanguage;
    while (last->next)
      last = last->next;
    last->next = lang;
    }
  else
    m_firstLanguage = lang;

  if (!m_genericLanguage && name && (_stricmp(name, "Generic") == 0))
    m_genericLanguage = lang;

  return lang;
}
//}}}
//{{{
void cLanguage::InitLanguages()
{
  LoadAllFromRegistry();

  if (!m_firstLanguage) {
    CreateLanguage ("Generic");
    char filename [_MAX_PATH];
    GetModuleFileName (0, filename, _MAX_PATH);
    char* s = strrchr (filename, '\\');
    if (s)
      strcpy (s, "\\fedlang.ini");
    else
      strcpy (filename, "fedlang.ini");
    //char filename [] = "c:\\Program Files\\Fed\\FedLang.ini";
    if (cLanguage::ReadIniFile (filename)) {
      cLanguage::SaveAllToRegistry();
      CString msg;
      msg.Format ("Loaded languages from file '%s'", filename);
      AfxMessageBox (msg, MB_OK);
      }
    }
  else if (!FindLanguage ("Generic")) {
    cLanguage* gen = CreateLanguage ("Generic");
    gen->SaveToRegistry();
    }
}
//}}}
//{{{
void cLanguage::Terminate()
{
  cLanguage* cur = m_firstLanguage;
  while (cur) {
    cLanguage* temp = cur;
    cur = cur->next;
    delete temp;
    }
  m_firstLanguage = 0;
  m_genericLanguage = 0;
}
//}}}

//{{{
cLanguage* cLanguage::FindLanguage (const char* name)
{
  cLanguage* cur = m_firstLanguage;

  while (cur) {
    if (_stricmp (cur->m_name, name) == 0)
      return cur;
    cur = cur->next;
    }

  return 0;
}
//}}}
//{{{
cLanguage* cLanguage::FindExtension (const char* ext)
{
  if (ext && *ext) {
    cLanguage* cur = m_firstLanguage;
    while (cur) {
      if (cur->valid_extension (ext))
        return cur;
      cur = cur->next;
      }
    }

  return 0;
}
//}}}

//{{{
bool cLanguage::ReadIniFile (const char* filename)
{
  FILE* fp = fopen (filename, "r");
  if (fp) {
    char line [256];
    while (fgets (line, 256, fp)) {
      char* cur = line;
      while (*cur && (*cur <= ' ')) cur++;
      if (*cur == '[') {
        cur++;
        char* name = cur;
        cur = strchr (name, ']');
        if (cur) {
          *cur = 0;
          cLanguage* language = FindLanguage (name);
          if (language) {
            //{{{  check for extra macros only
            CString langname;
            langname = name;
            long cur_pos = ftell (fp);
            while (fgets (line, 256, fp)) {
              char* cur = line;
              while (*cur && (*cur <= ' ')) cur++;
              if (*cur) {
                if (_memicmp (cur, "[Macro ", 7) != 0) {
                  //{{{  not just macros - so clear old language
                  delete language;
                  language = CreateLanguage (langname);
                  //}}}
                  }
                fseek (fp, cur_pos, SEEK_SET);
                break;
                }
              }
            //}}}
            }
          else
            language = CreateLanguage (name);
          if (language)
            language->LoadFromFile(fp);
          }
        }
      else if (_memicmp (cur, szInclude, cIncludeLen) == 0) {
        cur += cIncludeLen;
        while (*cur == ' ') cur++;
        char name [_MAX_PATH];
        if ((*cur == '\\') || (*(cur+1) == ':'))
          strcpy (name, cur);
        else {
          //{{{  make prefix filename directory
          strcpy (name, filename);
          char* s = strrchr (name, '\\');
          if (!s)
            s = strrchr (name, ':');
          if (s)
            strcpy (s+1, cur);
          else
            strcpy (name, cur);
          //}}}
          }
        char* s = strrchr (name, '\n');
        if (*s) *s = 0;
        s = strrchr (name, '.');
        if (!s)
          strcat (name, ".ini");
        ReadIniFile (name);
        }
      }
    fclose (fp);
    return true;
    }
  return false;
}
//}}}
//{{{
bool cLanguage::WriteIniFile (const char* filename)
{
  FILE* fp = fopen (filename, "w");
  if (fp) {
    cLanguage* cur = cLanguage::FirstLanguage();
    while (cur) {
      cur->SaveToFile (fp);
      cur = cur->Next();
      }
    fclose (fp);
    return true;
    }
  return false;
}
//}}}

//{{{
void cLanguage::LoadAllFromRegistry()
{
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  HKEY hKey = pApp->GetSectionKey ("Languages");

  if (hKey != NULL) {
    DWORD index = 0;
    char name [256];
    DWORD name_len = 256;
    FILETIME name_time;

    while (::RegEnumKeyEx (hKey, index++, name, &name_len, 0, 0, 0, &name_time) == ERROR_SUCCESS) {
      name_len = 256;

      cLanguage* language = FindLanguage (name);
      if (language)
        delete language;

      language = CreateLanguage (name);
      if (language)
        language->LoadFromRegistry();

      }
    ::RegCloseKey(hKey);
    }
}
//}}}
//{{{
void cLanguage::SaveAllToRegistry()
{
  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  HKEY hAppKey = pApp->GetAppRegistryKey();
  if (hAppKey != NULL) {
    pApp->DelRegTree (hAppKey, "Languages");
    ::RegCloseKey(hAppKey);
    }

  cLanguage* cur = cLanguage::FirstLanguage();
  while (cur) {
    cur->SaveToRegistry();
    cur = cur->Next();
    }
}
//}}}

// private
//{{{
void cLanguage::set_string (char*& string, const char* str)
{
  if (string) {
    delete[] string;
    string = 0;
    }
  if (str && *str) {
    string = new char [strlen (str) + 1];
    strcpy (string, str);
    }
}
//}}}
//{{{
char* cLanguage::find_word (const char* list, const char* key, int len, bool case_sensitive) const
{
  if (list && key) {
    if (len <= 0)
      len = (int)strlen(key);
    const char* s = list;
    while (*s == ' ') s++;

    if (case_sensitive) {
      while (*s) {
        if ((memcmp (s, key, len) == 0) && (*(s+len) <= ' '))
          return (char*) s;
        while (*s > ' ') s++;
        while (*s == ' ') s++;
        }
      }
    else {
      while (*s) {
        if ((_memicmp (s, key, len) == 0) && (*(s+len) <= ' '))
          return (char*) s;
        while (*s > ' ') s++;
        while (*s == ' ') s++;
        }
      }
    }
  return 0;
}
//}}}
//{{{
bool cLanguage::add_word (char*& list, const char* key)
{
  if (list) {
    if (find_word (list, key, 0))  // already have it
      return true;

    char* str = new char [strlen (list) + strlen (key) + 2];
    if (!str)
      return false;
    strcpy (str, list);
    delete[] list;
    list = str;
    }
  else {
    list = new char [strlen (key) + 3];
    if (!list)
      return false;
    strcpy (list, " ");
    }

  strcat (list, key);
  strcat (list, " ");
  return true;
}
//}}}
//{{{
bool cLanguage::remove_word (char* list, const char* key)
{ // 'key' is a single word or 'space' separated word list
  // returns true if 1 or more keyWords were deleted

  bool result = false;

  while (*key == ' ')
    key++;

  while (*key) {
    const char* keyend = strchr (key, ' ');
    int len = keyend ? (int) (keyend - key) : 0;

    char* dst = find_word (list, key, len);
    if (dst) {
      result = true;
      char* next_dst = strchr (dst, ' ');
      while (next_dst && (*next_dst == ' '))
        next_dst++;
      if (next_dst && *next_dst)    // copy tailend, overwriting word to remove
        memcpy (dst, next_dst, strlen (next_dst) + 1);
      else
        *dst = 0;    // terminate string to remove final word
      }
    key = keyend;
    while (*key == ' ')
      key++;
    }
  return result;
}
//}}}
//{{{
void cLanguage::save_string (FILE* fp, const char* key, const char* str)
{
  if (!fp || !key || !str)
    return;

  char* ss = (char*) str;  // naughty conversion!!

  while (*ss == ' ') ss++;
  int len = (int)strlen(ss);

  while (len > 70) {
    char* se = strchr (ss + 70, ' ');
    if (!se) break;

    *se = 0;                            // temporary patch to const char!!
    fprintf (fp, "%s=%s\n", key, ss);
    *se = ' ';                          // undo patch
    len = 0;

    ss = se + 1;
    while (*ss == ' ') ss++;
    len = (int)strlen(ss);
    }

  if (len > 0)
    fprintf (fp, "%s=%s\n", key, ss);
}
//}}}

//{{{
void cLanguage::set_str (char*& str, const char* fromstr)
{
  if (fromstr && *fromstr) {
    if (str) {
      if (strcmp (str, fromstr) == 0)
        return;
      delete[] str;
      }
    str = new char [strlen (fromstr) + 1];
    if (str)
      strcpy (str, fromstr);
    }
  else if (str) {
    delete[] str;
    str = 0;
    }
}
//}}}

// public
//{{{
cLanguage::cLanguage()
   : m_macros (NULL) {

  next = 0;

  m_name = 0;
  m_extensions = 0;

  m_subLanguage = 0;
  m_caseSensitive = false;
  m_useTaggedColour = false;
  m_keyWords = 0;

  m_commentStart1 = 0;
  m_commentEnd1 = 0;
  m_commentStart2 = 0;
  m_commentEnd2 = 0;
  m_stringChar = '\0';
  m_characterChar = '\0';
  m_escapeChar = '\0';

  m_tablen[0] = 2; // within indent
  m_tablen[1] = 2; // within text
  }
//}}}
//{{{
cLanguage::~cLanguage() {

  if (m_name) delete[] m_name;
  if (m_extensions) delete[] m_extensions;
  if (m_subLanguage) delete[] m_subLanguage;
  if (m_keyWords) delete[] m_keyWords;
  if (m_commentStart1) delete[] m_commentStart1;
  if (m_commentEnd1) delete[] m_commentEnd1;
  if (m_commentStart2) delete[] m_commentStart2;
  if (m_commentEnd2) delete[] m_commentEnd2;

  // delete macros
  while (m_macros) {
    cMacro* temp = m_macros;
    m_macros = m_macros->next;
    delete temp;
    }

  // unlink this
  if (m_firstLanguage == this)
    m_firstLanguage = next;
  else {
    cLanguage* cur = m_firstLanguage;
    while (cur) {
      if (cur->next == this) {
        cur->next = this->next;
        break;
        }
      cur = cur->next;
      }
    }

  if (m_genericLanguage == this)
    m_genericLanguage = 0;
  }
//}}}

//{{{
void cLanguage::Clear() {

  if (m_name) delete[] m_name;
  if (m_extensions) delete[] m_extensions;
  if (m_subLanguage) delete[] m_subLanguage;
  if (m_keyWords) delete[] m_keyWords;
  if (m_commentStart1) delete[] m_commentStart1;
  if (m_commentEnd1) delete[] m_commentEnd1;
  if (m_commentStart2) delete[] m_commentStart2;
  if (m_commentEnd2) delete[] m_commentEnd2;

  m_caseSensitive = true;
  m_useTaggedColour = false;
  m_stringChar = '\0';
  m_characterChar = '\0';
  m_escapeChar = '\0';
  m_tablen [0] = 0;
  m_tablen [1] = 0;

  ClearMacros();
  }
//}}}
//{{{
void cLanguage::ClearMacros() {

  // delete macros
  while (m_macros) {
    cMacro* temp = m_macros;
    m_macros = m_macros->next;
    delete temp;
    }
  }
//}}}
//{{{
void cLanguage::Set (cLanguage* lang) {

  if (lang) {
    set_str (m_name, lang->m_name);
    set_str (m_extensions, lang->m_extensions);

    m_caseSensitive = lang->m_caseSensitive;
    m_useTaggedColour = lang->m_useTaggedColour;

    set_str (m_keyWords, lang->m_keyWords);
    set_str (m_commentStart1, lang->m_commentStart1);
    set_str (m_commentEnd1, lang->m_commentEnd1);
    set_str (m_commentStart2, lang->m_commentStart2);
    set_str (m_commentEnd2, lang->m_commentEnd2);

    m_stringChar = lang->m_stringChar;
    m_characterChar = lang->m_characterChar;
    m_escapeChar = lang->m_escapeChar;
    m_tablen [0] = lang->m_tablen [0];
    m_tablen [1] = lang->m_tablen [1];

    ClearMacros();
    cMacro* cur = lang->m_macros;
    while (cur) {
      add_macro (*cur);
      cur = cur->next;
      }
    }
  else
    Clear();
  }
//}}}

//{{{
bool cLanguage::RemoveFromRegistry() {

  if (!m_name || !*m_name)
    return false;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  HKEY hKey = pApp->GetSectionKey (szLanguages);
  if (hKey != NULL) {
    pApp->DelRegTree (hKey, m_name);
    ::RegCloseKey(hKey);
    return true;
    }

  return false;
  }
//}}}
//{{{
bool cLanguage::LoadFromRegistry() {

  if (!m_name || !*m_name)
    return false;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  CString section = szLanguages;
  section += '\\';
  section += m_name;

  set_extensions (pApp->GetProfileString (section, szExtensions));
  m_caseSensitive = pApp->GetProfileInt (section, szCaseSensitive, 0) ? true : false;
  m_useTaggedColour = pApp->GetProfileInt (section, szUseTaggedColour, 0) ? true : false;
  set_keyWords (pApp->GetProfileString (section, szKeyWords));
  set_comment1 (pApp->GetProfileString (section, szDefaultCommentStart),
                pApp->GetProfileString (section, szDefaultCommentEnd));
  set_comment2 (pApp->GetProfileString (section, szAlternateCommentStart),
                pApp->GetProfileString (section, szAlternateCommentEnd));
  m_stringChar    = (char) pApp->GetProfileInt (section, szStringBoundsChar, '"');
  m_characterChar = (char) pApp->GetProfileInt (section, szCharacterBoundsChar, '\'');
  m_escapeChar    = (char) pApp->GetProfileInt (section, szStringEscapeChar, 0);
  m_tablen [0] = pApp->GetProfileInt (section, szIndentTabSize, 4);
  m_tablen [1] = pApp->GetProfileInt (section, szTextTabSize, 8);


  HKEY hKey = pApp->GetSectionKey (section);

  if (hKey != NULL) {
    DWORD index = 0;
    char name [256];
    DWORD name_len = 256;
    FILETIME name_time;

    while (::RegEnumKeyEx (hKey, index++, name, &name_len, 0, 0, 0, &name_time) == ERROR_SUCCESS) {
      name_len = 256;

      cMacro* macro = FindMacro (name);
      if (!macro)
        macro = new_macro (name);
      if (macro)
        macro->LoadFromRegistry (section);
      }
    ::RegCloseKey(hKey);
    }

  return true;
  }
//}}}
//{{{
bool cLanguage::SaveToRegistry() {

  if (!m_name || !*m_name)
    return false;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  CString section = szLanguages;
  section += '\\';
  section += m_name;

  pApp->WriteProfileString (section, szExtensions, m_extensions);
  pApp->WriteProfileInt    (section, szCaseSensitive, m_caseSensitive ? 1 : 0);
  pApp->WriteProfileInt    (section, szUseTaggedColour, m_useTaggedColour ? 1 : 0);
  pApp->WriteProfileString (section, szKeyWords, m_keyWords);
  pApp->WriteProfileString (section, szDefaultCommentStart, m_commentStart1);
  pApp->WriteProfileString (section, szDefaultCommentEnd, m_commentEnd1);
  pApp->WriteProfileString (section, szAlternateCommentStart, m_commentStart2);
  pApp->WriteProfileString (section, szAlternateCommentEnd, m_commentEnd2);
  pApp->WriteProfileInt    (section, szStringBoundsChar, m_stringChar);
  pApp->WriteProfileInt    (section, szCharacterBoundsChar, m_characterChar);
  pApp->WriteProfileInt    (section, szStringEscapeChar, m_escapeChar);
  pApp->WriteProfileInt    (section, szIndentTabSize, m_tablen [0]);
  pApp->WriteProfileInt    (section, szTextTabSize, m_tablen [1]);

  if (m_macros) {
    cMacro* macro = m_macros;
    while (macro) {
      macro->SaveToRegistry (section);
      macro = macro->next;
      }
    }

  return true;
  }
//}}}

//{{{
bool cLanguage::IsRegistered (const char* ext_name, char* desc, int* desc_len) {

  if (!ext_name || !*ext_name)
    return false;

  bool result = false;
  char extstr [MAX_PATH+2] = ".";
  char* ext = extstr + 1;
  strcpy (ext, ext_name);
  char* ext_end = strchr (ext, 0);
  // eg extstr=.cpp   ext=cpp   extend=

  char command [256];
  *command = '\"';
  GetModuleFileName (0, command+1, 255);
  strcat (command, "\" \"%1\"");
  // eg command="c:\Program Files\fed.exe" "%1"

  HKEY hKeyExt = NULL;
  BYTE value [MAX_PATH+1];
  DWORD len = MAX_PATH;
  // open .cpp key
  if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, extstr, 0, KEY_QUERY_VALUE, &hKeyExt) == ERROR_SUCCESS) {
    BYTE fileref [MAX_PATH+1];
    // read .cpp key value = eg cppfile
    if (::RegQueryValueEx (hKeyExt, 0, 0, 0, fileref, &len) == ERROR_SUCCESS) {
      HKEY hKeyFile = NULL;   // eg cppfile
      // open cppfile key
      if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, (char*) fileref, 0, KEY_QUERY_VALUE, &hKeyFile) == ERROR_SUCCESS) {
        if ((desc != NULL) && (desc_len != NULL)) {
          len = *desc_len;
          // read cppfile key value = description
          if (::RegQueryValueEx (hKeyFile, 0, 0, 0, (BYTE*) desc, &len) == ERROR_SUCCESS)
            *desc_len = len;
          else {
            *desc = 0;
            *desc_len = 0;
            }
          }
        strcpy (ext_end, "_auto_file");
        if (strcmp ((char*) fileref, ext) != 0) {   // ignore auto associations eg cpp_auto_file
            HKEY hKeyCmd = NULL;
            // open Open with &Fed key
            if (::RegOpenKeyEx (hKeyFile, szOpenCmd, 0, KEY_QUERY_VALUE, &hKeyCmd) == ERROR_SUCCESS) {
              len = MAX_PATH;
              //
              // read Open with &Fed key value
              if (::RegQueryValueEx (hKeyCmd, 0, 0, 0, value, &len) == ERROR_SUCCESS) {
                if (_stricmp ((char*) value, command) == 0)
                  result = true;
                }
              ::RegCloseKey (hKeyCmd);
              }
            }
          }
        ::RegCloseKey (hKeyFile);
        }
    ::RegCloseKey (hKeyExt);
    }

  return result;
  }
//}}}
//{{{
void cLanguage::RegisterExtension (const char* ext_name, const char* ext_desc, bool remove_opens) {

  if (!ext_name || !*ext_name)
    return;

  char extstr [MAX_PATH+2] = ".";
  char* ext = extstr + 1;
  strcpy (ext, ext_name);
  char* ext_end = strchr (ext, 0);
  // eg extstr=.cpp   ext=cpp   extend=

  CString desc (ext_desc);
  if (desc.IsEmpty())
    desc.Format ("%s Source File", Name());
  // eg desc=C++ Source FIle

  char path [_MAX_PATH];
  *path = '\"';
  GetModuleFileName (0, path+1, 255);
  char* pathend = strchr (path, 0);

  HKEY hKeyExt = NULL;
  DWORD dw;
  BYTE value [MAX_PATH+1];
  // create or open .cpp key
  if (::RegCreateKeyEx(HKEY_CLASSES_ROOT, extstr, 0, REG_NONE,
          REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
          &hKeyExt, &dw) == ERROR_SUCCESS) {  // found eg .cpp entry
    bool setkey = true;
    if (dw != REG_CREATED_NEW_KEY) {
      dw = MAX_PATH;
      // read existing .cpp key value (could be cppfile)
      if (::RegQueryValueEx (hKeyExt, 0, 0, 0, value, &dw) == ERROR_SUCCESS) {
        strcpy (ext_end, "_auto_file");
        if (dw > 0) {  // remove any eg cpp_auto_file reference key
          if (strcmp ((char*) value, ext) == 0)
            AfxGetApp()->DelRegTree(HKEY_CLASSES_ROOT, value);
          else {       // or use an existing reference key
            strcpy (ext, (char*) value);
            setkey = false;
            }
          }
        }
      }
    if (setkey) {      // set reference key to eg cppfile
      strcpy (ext_end, "file");
      ::RegSetValueEx (hKeyExt, 0, 0, REG_SZ, (const BYTE*) ext, (int)strlen(ext) + 1);
      }

    HKEY hKeyFile = NULL;
    // create or open cppfile key
    if (::RegCreateKeyEx (HKEY_CLASSES_ROOT, ext, 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
            &hKeyFile, &dw) == ERROR_SUCCESS) {  // find/create the reference key
      if (dw == REG_CREATED_NEW_KEY) {
        const char* s = desc;
        // set cppfile key description
        ::RegSetValueEx (hKeyFile, 0, 0, REG_SZ, (const BYTE*) s, (int)strlen(s) + 1);
        }
      HKEY hKeyIcon = NULL;
      // create or open the DefaultIcon key
      if (::RegCreateKeyEx (hKeyFile, "DefaultIcon", 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
            &hKeyIcon, &dw) == ERROR_SUCCESS) {
        if (dw == REG_CREATED_NEW_KEY) {
          strcpy (pathend, ",1");
          // set DefaultIcon key = C:\Program Files\*\Fed\Fed.exe,1     (using path+1)
          ::RegSetValueEx (hKeyIcon, 0, 0, REG_SZ, (const BYTE*) path+1, (int)strlen(path+1) + 1);
          }
        else {
          // read the DefaultIcon key value
          if (::RegQueryValueEx (hKeyIcon, 0, 0, 0, value, &dw) == ERROR_SUCCESS) {
            if (dw > 0) {
              char* str = (char*) value;
              char* iconnum = strrchr (str, ',');    //remember which icon was being used
              if (iconnum && ((iconnum-str) >= 7) && (_memicmp (iconnum-7, "fed.exe", 7) != 0)) {
                strcpy (pathend, iconnum);
                // change icon from previous fed, to use the same icon from this version of fed
                ::RegSetValueEx (hKeyIcon, 0, 0, REG_SZ, (const BYTE*) path+1, (int)strlen(path+1) + 1);
                }
              else if (remove_opens) {
                strcpy (pathend, ",1");
                ::RegSetValueEx (hKeyIcon, 0, 0, REG_SZ, (const BYTE*) path+1, (int)strlen(path+1) + 1);
                }
              }
            }
          }
        ::RegCloseKey (hKeyIcon);
        }

      if (remove_opens) {
        AfxGetApp()->DelRegTree (hKeyFile, "shell");
        AfxGetApp()->DelRegTree (hKeyFile, "Shell");   // just in case
        }

      // create or open Shell\Open With &Fed\command key and
      HKEY hKeyCmd = NULL;
      if (::RegCreateKeyEx (hKeyFile, szOpenCmd, 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
            &hKeyCmd, &dw) == ERROR_SUCCESS) {
        strcpy (pathend, "\" \"%1\"");
        // set command key value = "C:\Program Files\*\Fed\Fed.exe" "%1"
        ::RegSetValueEx (hKeyCmd, 0, 0, REG_SZ, (const BYTE*) path, (int)strlen(path) + 1);
        ::RegCloseKey (hKeyCmd);

        HKEY hKeyShell = NULL;
        // open shell key
        if (::RegOpenKeyEx (hKeyFile, szShell, 0, KEY_WRITE|KEY_READ, &hKeyShell) == ERROR_SUCCESS) {
          // set its default to Open With &Fed command
          ::RegSetValueEx (hKeyShell, 0, 0, REG_SZ, (const BYTE*) szOpen, (int)strlen(szOpen) + 1);
          ::RegCloseKey (hKeyShell);
          }
        ::RegCloseKey (hKeyCmd);
        }
      if (cOption::SingleInstance) {
        // create or open Shell\Open With new Fed\command key and
        if (::RegCreateKeyEx (hKeyFile, szOpenNewCmd, 0, REG_NONE,
              REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
              &hKeyCmd, &dw) == ERROR_SUCCESS) {
          strcpy (pathend, "\" /i \"%1\"");
          // set command key value = "C:\Program Files\*\Fed\Fed.exe" /i "%1"
          ::RegSetValueEx (hKeyCmd, 0, 0, REG_SZ, (const BYTE*) path, (int)strlen(path) + 1);
          ::RegCloseKey (hKeyCmd);
          }
        }
      ::RegCloseKey (hKeyFile);
      }
    ::RegCloseKey (hKeyExt);
    }
  }
//}}}
//{{{
bool cLanguage::UnregisterExtension (const char* ext_name) {

  if (!ext_name || !*ext_name)
    return false;

  bool delete_key = false;
  char extstr [MAX_PATH+2] = ".";
  char* ext = extstr + 1;
  strcpy (ext, ext_name);
  char* ext_end = strchr (ext, 0);
  // eg extstr=.cpp   ext=cpp   extend=


  HKEY hKeyExt = NULL;
  BYTE value [MAX_PATH+1];
  DWORD len = MAX_PATH;
  // open .cpp key
  if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, extstr, 0, KEY_QUERY_VALUE, &hKeyExt) == ERROR_SUCCESS) {
    BYTE fileref [MAX_PATH+1];
    // read .cpp key value = cppfile
    if (::RegQueryValueEx (hKeyExt, 0, 0, 0, fileref, &len) == ERROR_SUCCESS) {
      strcpy (ext_end, "_auto_file");
      if (_stricmp ((char*) fileref, ext) != 0) {   // ignore auto associations eg cpp_auto_file
        HKEY hKeyFile = NULL;   // eg cppfile
        // open cppfile key
        if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, (char*) fileref, 0, KEY_QUERY_VALUE, &hKeyFile) == ERROR_SUCCESS) {
          HKEY hKeyShell = NULL;
          // open shell key
          if (::RegOpenKeyEx (hKeyFile, szShell, 0, KEY_ENUMERATE_SUB_KEYS , &hKeyShell) == ERROR_SUCCESS) {
            delete_key = true;
            char open_cmd [MAX_PATH+1];
            int index = 0;
            len = MAX_PATH;
            // search list of shell-commands eg Open with &Fed
            while (::RegEnumKeyEx(hKeyShell, index, open_cmd, &len, 0, 0, 0, 0) == ERROR_SUCCESS) {
              bool delete_cmd = false;
              HKEY hKeyOpen = NULL;
              // open Open With key
              if (::RegOpenKeyEx (hKeyShell, open_cmd, 0, KEY_QUERY_VALUE, &hKeyOpen) == ERROR_SUCCESS) {
                HKEY hKeyCmd = NULL;
                // open command key
                if (::RegOpenKeyEx (hKeyOpen, "command", 0, KEY_QUERY_VALUE, &hKeyCmd) == ERROR_SUCCESS) {
                  len = MAX_PATH;
                  // read command key value = "C:\Program Files\*\Fed\Fed.exe" "%1"
                  if (::RegQueryValueEx (hKeyCmd, 0, 0, 0, value, &len) == ERROR_SUCCESS) {
                    _strlwr ((char*) value);
                    // test if it uses Fed
                    if (strstr ((char*) value, "\\fed.exe") || strstr ((char*) value, "\\fedwin.exe"))
                      delete_cmd = true;
                    }
                  ::RegCloseKey (hKeyCmd);
                  }
                ::RegCloseKey (hKeyOpen);
                }

              if (delete_cmd)
                AfxGetApp()->DelRegTree (hKeyShell, open_cmd);
              else {
                index += 1;
                delete_key = false;
                }
              len = MAX_PATH;
              }
            ::RegCloseKey (hKeyShell);
            }
          ::RegCloseKey (hKeyFile);
          }

        if (delete_key) {
          // remove cppfile and .cpp keys
          AfxGetApp()->DelRegTree (HKEY_CLASSES_ROOT, fileref);
          AfxGetApp()->DelRegTree (HKEY_CLASSES_ROOT, extstr);
          }
        }
      }
    ::RegCloseKey (hKeyExt);
    }

  return delete_key;
  }
//}}}
//{{{
void cLanguage::RegisterAllExtensions (bool remove_opens) {

  char ext [80];
  const char* list = m_extensions;
  while (list && *list) {
    while (*list == ' ')  // skip leading spaces
      list++;
    char* s = ext;
    while (*list > ' ')   // copy next word
      *s++ = *list++;
    if (s > ext) {
      *s = 0;
      RegisterExtension (ext, 0, remove_opens);
      }
    }
  }
//}}}
//{{{
void cLanguage::UnregisterAllExtensions() {

  char ext [80];
  const char* list = m_extensions;
  while (list && *list) {
    while (*list == ' ')  // skip leading spaces
      list++;
    char* s = ext;
    while (*list > ' ')   // copy next word
      *s++ = *list++;
    if (s > ext) {
      *s = 0;
      UnregisterExtension (ext);
      }
    }
  }
//}}}

//{{{
bool cLanguage::SaveToFile (FILE* fp) {

  if (!m_name || !*m_name)
    return false;
  FILE* local_fp = 0;

  if (!fp) {
    char filename [_MAX_PATH];
    strcpy (filename, m_name);
    char* s = filename;
    while (*s) {
      //{{{  convert space to underscore
      if (*s == ' ')
        *s = '_';
      s++;
      //}}}
      }
    strcat (filename, ".ini");

    local_fp = fopen (filename, "w");
    fp = local_fp;
    }

  if (fp) {
    fprintf (fp, "[%s]\n", m_name);
    if (m_caseSensitive)
      fprintf (fp, "%s=1\n", szCaseSensitive);
    if (m_useTaggedColour)
      fprintf (fp, "%s=1\n", szUseTaggedColour);
    save_string (fp, szExtensions, m_extensions);
    save_string (fp, szKeyWords, m_keyWords);
    if (m_commentStart1)
      fprintf (fp, "%s=%s\n", szDefaultCommentStart, m_commentStart1);
    if (m_commentEnd1)
      fprintf (fp, "%s=%s\n", szDefaultCommentEnd, m_commentEnd1);
    if (m_commentStart2)
      fprintf (fp, "%s=%s\n", szAlternateCommentStart, m_commentStart2);
    if (m_commentEnd2)
      fprintf (fp, "%s=%s\n", szAlternateCommentEnd, m_commentEnd2);
    if (m_stringChar)
      fprintf (fp, "%s=%c\n", szStringBoundsChar, m_stringChar);
    if (m_characterChar)
      fprintf (fp, "%s=%c\n", szCharacterBoundsChar, m_characterChar);
    if (m_escapeChar)
      fprintf (fp, "%s=%c\n", szStringEscapeChar, m_escapeChar);
    fprintf (fp, "\n");
    }

  cMacro* macro = m_macros;
  while (macro) {
    macro->SaveToFile (fp);
    macro = macro->next;
    }
  return true;

  if (local_fp)
    fclose (local_fp);
  return true;
  }
//}}}
//{{{
void cLanguage::LoadFromFile (FILE* fp, const char* filename) {

  if (fp) {
    char line [256];
    long cur_pos = ftell (fp);
    while (fgets (line, 256, fp)) {
      //{{{  strip trailing whitespace
      char* se = strchr (line, 0);
      if (se) {
        se--;
        while ((se > line) && (*se <= ' '))
          *se-- = 0;
        }
      //}}}
      char* cur = line;
      while (cur && (*cur <= ' ')) cur++;
      if (*cur == '[') {
        if (_memicmp (cur, "[Macro ", 7) == 0) {
          //{{{  load a macro
          cur += 7;
          while (*cur == ' ') cur++;
          char* last = strchr (cur, ']');
          if (last) *last = 0;
          cMacro* macro = FindMacro(cur);
          if (macro)
            macro->Clear();
          else
            macro = new_macro (cur);
          if (macro)
            macro->LoadFromFile (fp);
          //}}}
          }
        else {
          fseek (fp, cur_pos, SEEK_SET);
          return;   // to parse the next language
          }
        }
      else {
        char* keyWord = cur;
        cur = strchr (keyWord, '=');
        if (cur) {
          //{{{  parse a keyWord line
          char* keyend = cur;
          cur = keyend + 1;
          while (cur && (*cur <= ' ')) cur++;
          while ((keyend > keyWord) && (*(keyend - 1) <= ' '))
            keyend--;
          *keyend = 0;
          if (_stricmp (keyWord, szInclude) == 0) {
            //{{{  parse include file
            char name [_MAX_PATH];
            strcpy (name, cur);
            if (filename && (*cur != '\\') || (*(cur+1) != ':')) {
              //{{{  make prefix filename directory
              strcpy (name, filename);
              char* s = strrchr (name, '\\');
              if (!s)
                s = strrchr (name, ':');
              if (s)
                strcpy (s+1, cur);
              else
                strcpy (name, cur);
              //}}}
              }
            char* s = strrchr (name, '\n');
            if (s) *s = 0;
            s = strrchr (name, '.');
            if (!s)
              strcat (name, ".ini");
            FILE* fp = fopen (cur, "r");
            if (fp) {
              LoadFromFile (fp, name);
              fclose (fp);
              }
            //}}}
            }
          else if (_stricmp (keyWord, szCaseSensitive) == 0)
            m_caseSensitive = (*cur == '1');
          else if (_stricmp (keyWord, szUseTaggedColour) == 0)
            m_useTaggedColour = (*cur == '1');
          else if (_stricmp (keyWord, szExtensions) == 0)
            add_extension(cur);
          else if (_stricmp (keyWord, szKeyWords) == 0)
            add_keyWord (cur);
          else if (_stricmp (keyWord, szDefaultCommentStart) == 0)
            set_string (m_commentStart1, cur);
          else if (_stricmp (keyWord, szDefaultCommentEnd) == 0)
            set_string (m_commentEnd1, cur);
          else if (_stricmp (keyWord, szAlternateCommentStart) == 0)
            set_string (m_commentStart2, cur);
          else if (_stricmp (keyWord, szAlternateCommentEnd) == 0)
            set_string (m_commentEnd2, cur);
          else if (_stricmp (keyWord, szStringBoundsChar) == 0)
            m_stringChar = *cur;
          else if (_stricmp (keyWord, szCharacterBoundsChar) == 0)
            m_characterChar = *cur;
          else if (_stricmp (keyWord, szStringEscapeChar) == 0)
            m_escapeChar = *cur;
          //}}}
          }
        }
      cur_pos = ftell (fp);
      }
    }
  }
//}}}

//{{{
int cLanguage::FindKeyWord (const char*& starting, int len)
// finds a keyWord which starts with the 'starting' string
// returns the length of the matching string or, if ambiguous, the length
// of the common part
// and sets 'starting' to point to the matching keyWord
{
  const char* match = 0;
  int match_len = 0;
  const char* keyWords = m_keyWords;
  if (m_keyWords && starting && (*starting > ' ')) {
    if (len <= 0)
      len = (int)strlen(starting);
    const char* kw = m_keyWords;
    while (*kw == ' ') kw++;

    while (*kw) {
      bool matched;
      if (m_caseSensitive)
        matched = (memcmp (kw, starting, len) == 0);
      else
        matched = (_memicmp (kw, starting, len) == 0);

      if (matched) {
        if (match) {  // ambiguous
          const char* s = match;
          while ((*kw > ' ') && (*kw == *s)) { // skip past matching part
            kw++;
            s++;
            }
          int new_len = (int) (s - match);
          if (new_len < match_len)
            match_len = new_len;
          }
        else {
          match = kw;
          while (*kw > ' ') kw++;  // skip to end of current keyWord
          match_len = (int) (kw - match);
          }
        }

      while (*kw > ' ') kw++;    // skip to end of current keyWord
      while (*kw == ' ') kw++;   // skip to start of next keyWord
      }
    }

  if (match_len > 0)
    starting = match;

  return match_len;
}
//}}}

//{{{
cMacro* cLanguage::new_macro (const char* namestr)
{
  cMacro* macro = new cMacro (0, namestr, 0);
  if (macro) {
    macro->next = m_macros;
    m_macros = macro;
    return macro;
    }
  return 0;
}
//}}}
//{{{
cMacro* cLanguage::add_macro (int key, const char* namestr, const char* macrostr)
{
  if (macrostr && *macrostr) {
    cMacro* macro = new cMacro (key, namestr, macrostr);
    macro->next = m_macros;
    m_macros = macro;
    return macro;
    }
  return 0;
}
//}}}
//{{{
cMacro* cLanguage::add_macro (cMacro& from_macro)
{
  cMacro* macro = new cMacro (from_macro);
  if (macro) {
    if (m_macros) {
      cMacro* last = m_macros;
      while (last->next)
        last = last->next;
      last->next = macro;
      }
    else
      m_macros = macro;
    return macro;
    }
  return 0;
}
//}}}
//{{{
void cLanguage::delete_macro (cMacro* macro)
{
  if (m_macros && macro) {
    if (m_macros == macro) {
      m_macros = m_macros->next;
      delete macro;
      }
    else {
      cMacro* cur = m_macros;
      while (cur) {
        if (cur->next == macro) {
          cur->next = macro->next;
          delete macro;
          return;
          }
        cur = cur->next;
        }
      }
    }
}
//}}}
//{{{
void cLanguage::delete_macro (const char* name)
{
  if (m_macros && name && *name) {
    if (_stricmp (m_macros->name, name) == 0) {
      cMacro* macro = m_macros;
      m_macros = m_macros->next;
      delete macro;
      }
    else {
      cMacro* cur = m_macros;
      while (cur && cur->next) {
        if (_stricmp (cur->next->name, name) == 0) {
          cMacro* macro = cur->next;
          cur->next = macro->next;
          delete macro;
          return;
          }
        cur = cur->next;
        }
      }
    }
}
//}}}

//{{{
const unsigned char* cLanguage::FindMacro (int key)
{
  cMacro* cur = m_macros;
  while (cur) {
    if (cur->MatchKey(key))
      return cur->Macro();
    cur = cur->next;
    }
  if (m_genericLanguage && (m_genericLanguage != this))
    return m_genericLanguage->FindMacro (key);
  return 0;
}
//}}}
//{{{
const char* cLanguage::FindMacroName (int key)
{
  cMacro* cur = m_macros;
  while (cur) {
    if (cur->MatchKey(key))
      return cur->Name();
    cur = cur->next;
    }
  //if (m_genericLanguage && (m_genericLanguage != this))
    //return m_genericLanguage->FindMacroName (key);
  return 0;
}
//}}}
//{{{
cMacro* cLanguage::FindMacro (const char* name)
{
  if (name) {
    cMacro* cur = m_macros;
    while (cur) {
      if (_stricmp (cur->name, name) == 0)
        return cur;
      cur = cur->next;
      }
    }
  return 0;
}
//}}}

//{{{
bool cLanguage::set_name (const char* name, int len)
{
  if (!name)
    return false;
  if (m_name) delete[] m_name;
  m_name = 0;

  if (name) {
    if (len <= 0)
      len = (int)strlen(name);
    m_name = new char [len + 1];
    if (!m_name) return false;
    memcpy (m_name, name, len);
    *(m_name + len) = 0;
    }
  return true;
}
//}}}
//{{{
bool cLanguage::set_sub_language (const char* sub_language)
{
  if (!sub_language)
    return false;
  if (m_subLanguage) delete[] m_subLanguage;
  m_subLanguage = 0;

  if (sub_language) {
    m_subLanguage = new char [strlen (sub_language) + 1];
    if (!m_subLanguage) return false;
    strcpy (m_subLanguage, sub_language);
    }
  return true;
}
//}}}
//{{{
bool cLanguage::set_comment (const char* start, const char* end)
{
  if (!start || !*start || (m_commentStart1 && m_commentStart2))
    return false;

  if (m_commentStart1 && m_commentEnd1 && (!end || !*end)) {
    m_commentStart2 = m_commentStart1;
    m_commentEnd2 = m_commentEnd1;
    m_commentStart1 = 0;
    m_commentEnd1 = 0;
    }

  if (m_commentStart1) {
    m_commentStart2 = new char [strlen (start) + 1];
    if (!m_commentStart2) return false;
    strcpy (m_commentStart2, start);
    if (end && *end) {
      m_commentEnd2 = new char [strlen (end) + 1];
      if (!m_commentEnd2) {
        delete[] m_commentStart2;
        m_commentStart2 = 0;
        return false;
        }
      strcpy (m_commentEnd2, end);
      }
    else
      m_commentEnd2 = 0;
    }
  else {
    m_commentStart1 = new char [strlen (start) + 1];
    if (!m_commentStart1) return false;
    strcpy (m_commentStart1, start);
    if (end && *end) {
      m_commentEnd1 = new char [strlen (end) + 1];
      if (!m_commentEnd1) {
        delete[] m_commentStart1;
        m_commentStart1 = 0;
        return false;
        }
      strcpy (m_commentEnd1, end);
      }
    else
      m_commentEnd1 = 0;
    }
  return true;
}
//}}}
//{{{
bool cLanguage::set_comment1 (const char* start, const char* end)
{
  if (m_commentStart1)
    delete[] m_commentStart1;
  m_commentStart1 = 0;
  if (m_commentEnd1)
    delete[] m_commentEnd1;
  m_commentEnd1 = 0;

  if (start && *start) {
    m_commentStart1 = new char [strlen (start) + 1];
    strcpy (m_commentStart1, start);
    if (end && *end) {
      m_commentEnd1 = new char [strlen (end) + 1];
      strcpy (m_commentEnd1, end);
      }
    }

  return true;
}
//}}}
//{{{
bool cLanguage::set_comment2 (const char* start, const char* end)
{
  if (m_commentStart1)
    delete[] m_commentStart2;
  m_commentStart2 = 0;
  if (m_commentEnd2)
    delete[] m_commentEnd2;
  m_commentEnd2 = 0;

  if (start && *start) {
    m_commentStart2 = new char [strlen (start) + 1];
    strcpy (m_commentStart2, start);
    if (end && *end) {
      m_commentEnd2 = new char [strlen (end) + 1];
      strcpy (m_commentEnd2, end);
      }
    }

  return true;
}
//}}}
//{{{
bool cLanguage::set_extensions (const char* str)
{
  if (m_extensions) delete[] m_extensions;
  m_extensions = 0;

  if (str && *str) {
    m_extensions = new char [strlen (str) + 1];
    if (!m_extensions) return false;
    strcpy (m_extensions, str);
    }
  return true;
}
//}}}
//{{{
bool cLanguage::set_keyWords (const char* str)
{
  if (m_keyWords) delete[] m_keyWords;
  m_keyWords = 0;

  if (str && *str) {
    m_keyWords = new char [strlen (str) + 1];
    if (!m_keyWords) return false;
    strcpy (m_keyWords, str);
    }
  return true;
}
//}}}

// cMacro
//{{{
cMacro::cMacro (int key, const char* namestr, const char* macrostr)
 : next (NULL),
   name (NULL),
   macro (NULL)
{
  keys[0] = key;
  for (int n = 1; n < 8; n++)
    keys[n] = 0;

  if (namestr && *namestr) {
    name = new char [strlen (namestr) + 1];
    strcpy (name, namestr);
    }
  if (macrostr && *macrostr) {
    macro = new unsigned char [strlen (macrostr) + 1];
    strcpy ((char*) macro, macrostr);
    }
}
//}}}
//{{{
cMacro::cMacro (cMacro& from_macro)
 : next (NULL),
   name (NULL),
   macro (NULL)
{
  for (int n = 0; n < 8; n++)
    keys[n] = from_macro.keys[n];

  if (from_macro.name && *from_macro.name) {
    name = new char [strlen (from_macro.name) + 1];
    strcpy (name, from_macro.name);
    }
  if (from_macro.macro && *from_macro.macro) {
    macro = new unsigned char [strlen ((char*) from_macro.macro) + 1];
    strcpy ((char*) macro, (char*) from_macro.macro);
    }
}
//}}}
//{{{
cMacro::~cMacro()
{
  if (name)
    delete[] name;
  if (macro)
    delete[] macro;
}
//}}}

//{{{
bool cMacro::extend (int length) {
  if (macro) {
    int curlen = (int)strlen((char*) macro);
    unsigned char* newmacro = new unsigned char [curlen + length + 1];
    if (!newmacro) return false;
    memcpy (newmacro, macro, curlen);
    memset (newmacro + curlen, 0, length + 1);
    delete[] macro;
    macro = newmacro;
    }
  else {
    macro = new unsigned char [length + 1];
    if (!macro) return false;
    memset (macro, 0, length + 1);
    }
  return true;
  }
//}}}
//{{{
void cMacro::save_string (FILE* fp, const char* key, const char* str)
{
  if (!fp || !key || !str)
    return;
  char* local_str = new char [strlen (str) + 1];
  strcpy (local_str, str);                      // so that we can patch it
  char* curchar = local_str;

  while (*curchar == ' ') curchar++;

  char* se = curchar;

  while (*curchar) {
    char* space = 0;
    char* s = curchar;
    if ((*s >= '0') && (*s <= '9'))
      { // ensure repeat-number and its command remain together
      while ((*s >= '0') && (*s <= '9')) s++;
      while (*s >= ' ') s++;
      }

    while ((*s >= ' ') && (s - curchar < 70)) {
      if (*s == ' ')
        space = s++;
      else if (*s == '\"') {
        char* close = strchr (s+1, '\"');
        if (close && (close - curchar < 70))
          s = close + 1;      // skip quoted string and continue
        else {
          //{{{  scan and split the quoted string
          char* ss = s + 1;
          while (*ss && (*ss != '\"')) {
            if ((ss[0] == '\\') && (ss[1] == 'n') && (ss[2] != '\"')) {
              //{{{  print string up to newline mark
              *ss++ = 0;
              fprintf (fp, "%s=%s\\n\"\n", key, curchar);
              curchar = ss;
              *curchar = '\"';
              //}}}
              }
            ss++;
            }
          if (space && (ss - curchar > 70)) {
            //{{{  print line prior to string to make more room
            *space = 0;
            fprintf (fp, "%s=%s\n", key, curchar);
            curchar = space + 1;
            space = 0;
            //}}}
            }
          s = ss;
          if (*s = '\"')
            s++;
          //}}}
          }
        }
      else
        s++;
      }

    if ((*s == '\r') && (*(s+1) == '\n'))
      *s++ = 0;

    if (*s == '\n') {
      *s = 0;
      fprintf (fp, "%s=%s\n", key, curchar);
      curchar = s + 1;
      }
    else if (space && *s) {
      *space = 0;
      fprintf (fp, "%s=%s\n", key, curchar);
      curchar = space + 1;
      }
    else {
      fprintf (fp, "%s=%s\n", key, curchar);
      break;
      }
    }

  delete[] local_str;
}
//}}}
//{{{
void cMacro::load_string (CString& macro_str, char* str, int len, bool split_on_newline)
{
  char last_char = str [len];
  if (len > 0)
    str [len] = 0;

  if (macro_str.IsEmpty() || (macro_str[macro_str.GetLength() - 1] == '\n'))
    macro_str += "\"";
  else
    macro_str += " \"";

  char* s = str;
  char* newline = strchr (s, '\n');
  while (newline) {
    *newline = 0;
    macro_str += s;
    macro_str += "\\n";
    *newline = '\n';
    s = newline + 1;
    if (split_on_newline) {
      macro_str += "\"\r\n";
      if (*s)
        macro_str += "\"";
      }
    newline = strchr (s, '\n');
    }
  if (*s)
    macro_str += s;

  if (macro_str[macro_str.GetLength() - 1] != '\n')
    macro_str += "\"";

  if (len > 0)
    str [len] = last_char;
}
//}}}
//{{{
void cMacro::add_text (const char* str, int len, bool append_newline)
{
  if (str) {
    int needlen = append_newline ? len + 1 : len;
    if ((len > 0) && extend (needlen)) {
      char* last = strchr ((char*) macro, 0);
      memcpy (last, str, len);
      last += len;
      if (append_newline)
        *last++ = '\n';
      *last = 0;
      }
    }
}
//}}}

//{{{
void cMacro::Clear()
{
  for (int n = 0; n < 8; n++)
    keys[n] = 0;
  if (macro)
    delete[] macro;
  macro = 0;
}
//}}}
//{{{
void cMacro::AddKey (int key)
{
  if (key > 0) {
    for (int n = 0; n < 8; n++) {
      if (!keys[n]) {
        keys[n] = key;
        return;
        }
      }
    }
  AfxMessageBox ("Too many keys in Macro", MB_OK);
}
//}}}
//{{{
bool cMacro::MatchKey (int key)
{
  for (int n = 0; n < 8; n++) {
    if (keys[n] == key)
      return true;
    }
  return false;
}
//}}}
//{{{
void cMacro::DeleteKey (int key)
{
  for (int n = 0; n < 8; n++) {
    if (keys[n] == key) {
      for (int i = n; i < 7; i++)
        keys[i] = keys[i+1];
      keys[7] = 0;
      }
    }
}
//}}}

//{{{
void cMacro::GetMacro (CString& macro_str, bool multi_line)
{ // format changed - default is command, 255 now toggles between command & text
  int indent = 0;
  int token = 0;
  int char_count = 0;
  macro_str.Empty();
  unsigned char* s = macro;

  while (*s) {
    if (multi_line && (token == k_end) && (char_count > 0)) {
      macro_str += "\r\n";  // always start a new line after 'end'
      char_count = 0;
      }

    if (*s == 255) {
      //{{{  insert a string
      s++;
      if (*s && (*s != 255)) {
        unsigned char* k = s;
        while (*s == *k) s++;
        int count = int (s - k);
        if ((count > 4) && ((*s == 255) || (*s == 0))) {
          char num [20];
          if (char_count > 0) {
            macro_str += " ";
            char_count++;
            }
          char ch = (char) (*k);
          sprintf (num, "%d \"%c\"", count, ch);
          macro_str += num;
          char_count += (int)strlen(num);
          }
        else {
          bool has_newline = false;
          while (*s && (*s != 255))
            if (*s++ == '\n') has_newline = true;

          if (multi_line && has_newline && !macro_str.IsEmpty()) {
            macro_str += "\r\n";  // start multiple-line strings on a newline
            char_count = 0;
            }

          if (char_count++ > 0)
            macro_str += " ";
          else {
            for (int i=0; i<indent; i++) macro_str += " ";
            char_count += indent - 1;
            }

          //load_string (macro_str, (char*) k, int (s - k), multi_line);
          //{{{  load string
          char last_char = *s;  // save for restoration later
          *s = 0;
          macro_str += "\"";
          char_count++;

          char* newline = strchr ((char*) k, '\n');
          while (newline) {
            *newline = 0;
            macro_str += (char*) k;
            macro_str += "\\n";
            *newline = '\n';    // restore the nl

            k = (unsigned char*) (newline + 1);
            if (multi_line) {
              macro_str += "\"\r\n";
              char_count = 0;
              if (*k) {
                for (int i=0; i<indent; i++) macro_str += " ";
                macro_str += "\"";
                char_count += indent + 1;
                }
              }
            newline = strchr ((char*) k, '\n');
            }
          if (*k) {
            macro_str += (char*) k;
            char_count += int(s - k);
            }

          if (char_count > 0) {
            macro_str += "\"";
            char_count += 1;
            }

          *s = last_char;   // restore last char
          //}}}

          if (multi_line && (token == k_domacro)) {
            macro_str += "\r\n";  // domacro is always on its own line
            char_count = 0;
            }
          }
        }
      if (*s == 255) s++;  //skip terminating 255
      //}}}
      }
    else {
      //{{{  insert a command
      token = *s++;
      int repeat_count = 1;
      if ((token == k_domacro) || (token == k_if) || (token == k_while))
        {
        if (multi_line && (char_count > 0)) {
          macro_str += "\r\n";  // domacro & conditionals always start on a new line
          char_count = 0;
          }
        }
      else if (token != k_end)
        while (*s == token) {
          s++;
          repeat_count += 1;
          }

      CString new_str;

      if (repeat_count > 1) {
        char num [20];
        sprintf (num, "%d ", repeat_count);
        new_str = num;
        }
      new_str += cKeyMap::TokenName (token);
      int new_count = new_str.GetLength();

      if (multi_line && (char_count > 0) && (char_count + 1 + new_count > 70)) {
        macro_str += "\r\n";  // limit line length to 70
        char_count = 0;
        }

      if (char_count++ > 0)
        macro_str += " ";
      else if (indent > 0) {
        for (int i=0; i<indent; i++) macro_str += " ";
        char_count += indent - 1;  // already incremented !
        }
      macro_str += new_str;
      char_count += new_count;

      if ((token == k_if) || (token == k_while))
        indent++;
      else if ((token == k_end) && (indent > 0))
        indent--;
      //}}}
      }
    }
}
//}}}
//{{{
int  cMacro::SetMacro (const char* str)
{ // format changed - default is command, 255 now toggles between command & text
  // Return ok=0, error=CEdit-range low-word error-start, high-word error-end
  // Return ok=0, error=CEdit style range containing error, low-word error-start, high-word error-end
  unsigned char* old_macro = macro;
  macro = 0;
  int error_range = AddToMacro (str);

  if (error_range)
    macro = old_macro;   // error - replace old macro
  else if (old_macro)
    delete[] old_macro;

  return error_range;
}
//}}}
//{{{
int  cMacro::AddToMacro (const char* str)
{ // format changed - default is command, 255 now toggles between command & text
  // Return ok=0, error=CEdit style range containing error, low-word error-start, high-word error-end
  if (!str) return 0;
  const char* src = str;

  while (*src == ' ') src++;
  int count = 1;
  char tokenstr [40];
  bool textmode = false;
  if (macro) {
    const char* s = strchr ((const char*) macro, 255);
    while (s) {
      textmode = !textmode;
      s = strchr (s + 1, 255);
      }
    }

  while (*src >= ' ') { // note: embedded \n's will be skipped
    if ((*src >= '0') && (*src <= '9')) {
      //{{{  set count
      count = 0;
      while ((*src >= '0') && (*src <= '9'))
        count = count * 10 + *src++ - '0';
      if (count < 1)
        count = 1;     // safety
      //}}}
      }
    else if (*src == '\"') {
      //{{{  insert a string
      if (!textmode) {
        if (extend(1)) {
          char* last = strchr ((char*) macro, 0);
          *last = (char) 255;
          }
        textmode = true;
        }
      const char* ss;
      for (int n = 0; n < count; n++) {
        //{{{  insert the string
          ss = src + 1;
          const char* se = ss;
          while (*se && (*se != '\"')) {
            if ((se[0] == '\\') && (se[1] == 'n')) {
              add_text (ss, int (se - ss), true);
              se += 2;
              ss = se;
              }
            else
              se++;
            }
          if (*ss && (se > ss))
            add_text (ss, int (se - ss), false);
          ss = se;
          if (*ss == '\"') ss++;
        //}}}
        }
      src = ss;
      //}}}
      count = 1;
      }
    else {
      //{{{  insert a token
      if (textmode) {
        if (extend(1)) {
          char* last = strchr ((char*) macro, 0);
          *last = (char) 255;
          }
        textmode = false;
        }
      int token_start = int(src - str);
      char* dst = tokenstr;
      while (*src > ' ')
        *dst++ = *src++;
      *dst = 0;
      int token = cKeyMap::FindToken (tokenstr);
      if ((token >= 0) && extend (count)) {
        char* last = strchr ((char*) macro, 0);
        for (int n = 0; n < count; n++)
          *last++ = (char) token;
        }
      else {
        int token_end = token_start + (int)strlen(tokenstr);
        CString msg;
        msg.Format ("Unknown keyWord '%s' in Macro definition %d,%d", tokenstr, token_start, token_end);
        AfxMessageBox (msg, MB_OK);
        return token_start + (token_end << 16);
        }
      //}}}
      count = 1;
      }
    while (*src == ' ') src++;
    }
  return 0;
}
//}}}

//{{{
bool cMacro::LoadFromRegistry (const char* registry_section)
{
  if (!name || !*name)
    return false;
  for (int n = 0; n < 8; n++)
    keys[n] = 0;
  if (macro)
    delete[] macro;
  macro = 0;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  CString section;
  section = registry_section;
  section += "\\";
  section += name;

  CString str (pApp->GetProfileString (section, "Keys"));
  char* keystr = str.GetBuffer (0);
  char* ke = strchr (keystr, ';');
  while (ke) {
    *ke = 0;
    AddKey (cKeyMap::FindKey (keystr));
    keystr = ke + 1;
    ke = strchr (keystr, ';');
    }
  if (*keystr)
    AddKey (cKeyMap::FindKey (keystr));
  str.ReleaseBuffer();

  AddToMacro (pApp->GetProfileString (section, "Macro"));

  return true;
}
//}}}
//{{{
bool cMacro::SaveToRegistry (const char* registry_section)
{
  if (!name || !*name || !macro || !*macro)
    return false;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  CString section;
  section = registry_section;
  section += "\\";
  section += name;

  CString entry;   //.Empty();
  for (int n = 0; n < 8; n++) {
    if (keys[n]) {
      if (!entry.IsEmpty())
        entry += ';';
      entry += cKeyMap::StateName (keys[n] / 128);
      entry += cKeyMap::KeyName (keys[n] % 128);
      }
    }
  pApp->WriteProfileString (section, "Keys", entry);

  CString macro_str;
  GetMacro (macro_str);
  pApp->WriteProfileString (section, "Macro", macro_str);

  return true;
}
//}}}

//{{{
void cMacro::LoadFromFile (FILE* fp)
{
  if (fp) {
    char line [256];
    long cur_pos = ftell (fp);
    while (fgets (line, 256, fp)) {
      //{{{  strip trailing whitespace
      char* se = strchr (line, 0);
      if (se) {
        se--;
        while ((se > line) && (*se <= ' '))
          *se-- = 0;
        }
      //}}}
      char* cur = line;
      while (cur && (*cur <= ' ')) cur++;
      if (*cur == '[') {
        fseek (fp, cur_pos, SEEK_SET);
        return;
        }
      char* keyWord = cur;
      cur = strchr (keyWord, '=');
      if (cur) {
        char* keyend = cur;
        cur = keyend + 1;
        while (cur && (*cur <= ' ')) cur++;
        while ((keyend > keyWord) && (*(keyend - 1) <= ' '))
          keyend--;
        *keyend = 0;
        if (_stricmp (keyWord, "Key") == 0) {
          int key = cKeyMap::FindKey (cur);
          if (key < 0) {
            CString msg;
            msg.Format ("Unknown key '%s' in Macro definition", cur);
            AfxMessageBox (msg, MB_OK);
            }
          else
            AddKey (key);
          }
        else if (_stricmp (keyWord, "Macro") == 0)
          AddToMacro (cur);
        }
      cur_pos = ftell (fp);
      }
    }
}
//}}}
//{{{
bool cMacro::SaveToFile (FILE* fp)
{
  if (!fp || !name || !*name || !macro || !*macro)
    return false;

  CString macro_str;
  GetMacro (macro_str, true);

  if (!macro_str.IsEmpty()) {
    fprintf (fp, "[Macro %s]\n", name);
    for (int n = 0; n < 8; n++) {
      if (keys[n])
        fprintf (fp, "Key=%s%s\n", cKeyMap::StateName (keys[n] / 128), cKeyMap::KeyName (keys[n] % 128));
      }
    save_string (fp, "Macro", macro_str);
    fprintf (fp, "\n");
    return true;
    }

  return false;
}
//}}}
