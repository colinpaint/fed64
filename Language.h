#pragma once

#include <cstring>

//{{{
class cMacro {
public:
  cMacro (int key, const char* namestr, const char* macrostr);
  cMacro (cMacro& from_macro);
  ~cMacro();

  const unsigned char* Macro() { return (const unsigned char*) macro; }
  const char* Name() { return (const char*) name; }

  void Clear();
  void AddKey (int key);
  bool MatchKey (int key);
  void DeleteKey (int key);

  void GetMacro (CString& macro_str, bool multi_line = false);
  int SetMacro (const char* str);
  int AddToMacro (const char* str);

  bool LoadFromRegistry (const char* registry_section);
  bool SaveToRegistry (const char* registry_section);

  bool SaveToFile (FILE* fp);
  void LoadFromFile (FILE* fp);

  cMacro* next;
  char* name;
  unsigned char* macro;
  int keys [8];

private:
  bool extend (int length);
  void add_text (const char* str, int len, bool append_newline = false);

  void save_string (FILE* fp, const char* key, const char* str);
  void load_string (CString& macro_str, char* str, int len = 0, bool split_on_newline = false);
  };
//}}}
//{{{
class cLanguage {
friend cMacro;
public:
  cLanguage();
  ~cLanguage();

  enum { InIndent = 0, InText = 1 };

  static cLanguage* m_firstLanguage;
  static cLanguage* m_genericLanguage;
  static void InitLanguages();
  static void Terminate();  // used to clear heap
  static cLanguage* FirstLanguage() { return m_firstLanguage; }
  static cLanguage* GenericLanguage() { return m_genericLanguage; }
  static cLanguage* FindLanguage (const char* name);
  static cLanguage* FindExtension (const char* ext);

  static bool ReadIniFile (const char* filename);
  static bool WriteIniFile (const char* filename);

  static void LoadAllFromRegistry();
  static void SaveAllToRegistry();
  static cLanguage* CreateLanguage (const char* name);

  void Clear();
  void ClearMacros();
  void Set (cLanguage* lang);

  bool RemoveFromRegistry();
  bool LoadFromRegistry();
  bool SaveToRegistry();

  bool IsRegistered (const char* ext_name, char* desc, int* desc_len);
  void RegisterExtension (const char* ext_name, const char* ext_desc, bool remove_opens);
  bool UnregisterExtension (const char* ext_name);
  void RegisterAllExtensions (bool remove_opens);
  void UnregisterAllExtensions();

  bool SaveToFile(FILE* fp);
  void LoadFromFile(FILE* fp, const char* filename = 0);

  cLanguage* Next() const { return next; }
  cMacro* FirstMacro() const { return m_macros; }

  const char* Name() const { return (const char*) m_name; }
  const char* Extensions() const { return (const char*) m_extensions; }
  const char* SubLanguage() const { return (const char*) m_subLanguage; }

  bool isCaseSensitive() const { return m_caseSensitive; }
  bool useTaggedColour() const { return m_useTaggedColour; }
  const char* KeyWords() const { return (const char*) m_keyWords; }

  const char* CommentStart() const { return (const char*) m_commentStart1; }
  const char* CommentEnd() const { return (const char*) m_commentEnd1; }
  const char* CommentStart2() const { return (const char*) m_commentStart2; }
  const char* CommentEnd2() const { return (const char*) m_commentEnd2; }

  char EscapeChar() const { return m_escapeChar; }
  char StringChar() const { return m_stringChar; }
  char CharacterChar() const { return m_characterChar; }
  int Tablen (int type = 0) const {
    return (type >= 0) && (type <= 1) ? m_tablen [type] : 8;
    }

  const char* find_extension (const char* extension, int len = 0) const
    { return find_word (m_extensions, extension, len); }
  bool valid_extension (const char* extension, int len = 0) const
    { return find_word (m_extensions, extension, len) ? true : false; }
  bool add_extension (const char* extension)
    { return add_word (m_extensions, extension); }
  bool remove_extension (const char* extension)
    { return remove_word (m_extensions, extension); }

  bool valid_keyWord (const char* keyWord, int len = 0) const
    { return find_word (m_keyWords, keyWord, len, m_caseSensitive) ? true : false; }
  bool add_keyWord (const char* keyWord)
    { return add_word (m_keyWords, keyWord); }
  bool remove_keyWord (const char* keyWord)
    { return remove_word (m_keyWords, keyWord); }
  int FindKeyWord (const char*& starting, int len = 0);

  cMacro* new_macro (const char* namestr);
  cMacro* add_macro (int key, const char* namestr, const char* macrostr);
  cMacro* add_macro (cMacro& macro);
  void delete_macro (cMacro* macro);
  void delete_macro (const char* name);

  const unsigned char* FindMacro (int key);
  const char* FindMacroName (int key);
  cMacro* FindMacro (const char* name);

  bool set_name (const char* name, int len = 0);
  bool set_sub_language (const char* sub_language);
  bool set_comment (const char* start, const char* end);
  bool set_comment1 (const char* start, const char* end);
  bool set_comment2 (const char* start, const char* end);
  void set_escape_char (char ch) { m_escapeChar = ch; }
  void set_escape_char (const char* str) { m_escapeChar = str ? *str : 0; }
  void set_string_char (const char* str) { m_stringChar = str ? *str : 0; }
  void set_character_char (const char* str) { m_characterChar = str ? *str : 0; }
  bool set_extensions (const char* str);
  bool set_keyWords (const char* str);
  void set_case_sensitive (bool state) { m_caseSensitive = state; }
  void set_use_tagged_colour (bool state) { m_useTaggedColour = state; }
  void set_tablen (int len, int type = 0) {
    if ((type >= 0) && (type <= 1))
      m_tablen [type] = len;
    }

  // vars
  cLanguage* next;
  char* m_name;
  char* m_extensions;
  char* m_subLanguage;
  bool  m_caseSensitive;
  bool  m_useTaggedColour;
  char* m_keyWords;
  char* m_commentStart1;
  char* m_commentEnd1;
  char* m_commentStart2;
  char* m_commentEnd2;
  char m_escapeChar;
  char m_stringChar;
  char m_characterChar;
  int m_tablen [2];

private:
  static void save_string (FILE* fp, const char* key, const char* str);

  void set_string (char*& string, const char* str);
  char* find_word (const char* list, const char* key, int len, bool case_sensitive = true) const;
  bool add_word (char*& list, const char* key);
  bool remove_word (char* list, const char* key);
  void set_str (char*& str, const char* fromstr);

  cMacro* m_macros;
  };
//}}}
