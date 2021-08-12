#pragma once

#include "resource.h"

extern const char szFedClass[];

class cFedApp : public CWinApp {
public:
  cFedApp();
  virtual ~cFedApp();

  virtual BOOL InitInstance();

  static CMenu* GetSubMenu (CMenu* menu, const char* label);
  static CMenu* GetSubMenu (const char* label);
  static CMenu* GetSubMenu (const char* label, const char* sub_label);

  static BOOL AddLanguageToMenu (const char* name);
  static BOOL RemoveLanguageFromMenu (const char* name);
  static void AddGenericLanguageToMenu();
  static void AddAllLanguagesToMenu();

  static int GetMenuKeys();

  static BOOL CALLBACK StaticEnumProc (HWND hWnd, LPARAM lParam);

  static int m_nOpenMsg;
  static BOOL NewInstance;
  static BOOL ReadOnly;
  static int GotoLineNumber;
  static CString InitialMessage;

  DECLARE_MESSAGE_MAP()

private:
  BOOL IsDocOpen (LPCTSTR lpszFileName);

  void log_keys (FILE* fp, HKEY hKey, int level);
  void log_values (FILE* fp, HKEY hKey, int level);
  };
