#pragma once

#include "resource.h"
#include "KeyMap.h"

class cKeyMapPage : public CDialog {
public:
  cKeyMapPage();
  ~cKeyMapPage();

  enum { IDD = IDD_KEYMAP_PAGE };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  void OnApply();

  //{{{  Generated message map functions
  afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
  afx_msg void OnKeyLshift();
  afx_msg void OnKeyRshift();
  afx_msg void OnKeyLctrl();
  afx_msg void OnKeyRctrl();
  afx_msg void OnKeyLalt();
  afx_msg void OnKeyRalt();
  afx_msg void OnKeyNum0();
  afx_msg void OnKeyNum1();
  afx_msg void OnKeyNum2();
  afx_msg void OnKeyNum3();
  afx_msg void OnKeyNum4();
  afx_msg void OnKeyNum5();
  afx_msg void OnKeyNum6();
  afx_msg void OnKeyNum7();
  afx_msg void OnKeyNum8();
  afx_msg void OnKeyNum9();
  afx_msg void OnKeyNumdel();
  afx_msg void OnKeyNumenter();
  afx_msg void OnKeyNumminus();
  afx_msg void OnKeyNummul();
  afx_msg void OnKeyNumplus();
  afx_msg void OnKeyNumslash();
  afx_msg void OnKeyTab();
  afx_msg void OnKeyEsc();
  afx_msg void OnKeyF1();
  afx_msg void OnKeyF2();
  afx_msg void OnKeyF3();
  afx_msg void OnKeyF4();
  afx_msg void OnKeyF5();
  afx_msg void OnKeyF6();
  afx_msg void OnKeyF7();
  afx_msg void OnKeyF8();
  afx_msg void OnKeyF9();
  afx_msg void OnKeyF10();
  afx_msg void OnKeyF11();
  afx_msg void OnKeyF12();
  afx_msg void OnKeyInsert();
  afx_msg void OnKeyHome();
  afx_msg void OnKeyPgUp();
  afx_msg void OnKeyDelete();
  afx_msg void OnKeyEnd();
  afx_msg void OnKeyPgDn();
  afx_msg void OnKeyUp();
  afx_msg void OnKeyLeft();
  afx_msg void OnKeyDn();
  afx_msg void OnKeyRight();
  afx_msg void OnKeyQuoteA();
  afx_msg void OnKeyBS();
  afx_msg void OnKeyReturn();
  afx_msg void OnKeyMinus();
  afx_msg void OnKeyEqual();
  afx_msg void OnKeyOpen();
  afx_msg void OnKeyClose();
  afx_msg void OnKeyColon();
  afx_msg void OnKeyQuote1();
  afx_msg void OnKeyHash();
  afx_msg void OnKeyBslash();
  afx_msg void OnKeyComma();
  afx_msg void OnKeyDot();
  afx_msg void OnKeySlash();
  afx_msg void OnKeySpace();
  afx_msg void OnButtonRestore();
  afx_msg void OnButtonRestoreall();
  afx_msg void OnDblclkCommandList();
  afx_msg void OnSelcancelKeyList();
  afx_msg void OnSelchangeKeyList();
  afx_msg void OnSelcancelCommandList();
  afx_msg void OnSelchangeCommandList();
  afx_msg void OnKeystrokeChanged();
  afx_msg void OnCommandChanged();
  //}}}
  afx_msg void OnAsciiKey (UINT nID) { ShowAsciiKey (nID & 0x7f); }
  DECLARE_MESSAGE_MAP()

private:
  void SetModified (BOOL state = TRUE);
  void InvalidateKeyboard();
  void ShowAsciiKey (int key, const char* name = 0);
  void ShowSpecialKey (int key);
  void SetStateKey (int new_state);
  void UpdateKeyMapList (int token, int key);
  void SetKeystroke (const char* str);
  void SetCommand (const char* str);
  int MatchEntry (CEdit* editbox, CListBox* listbox, CString& prev);

  DECLARE_DYNCREATE(cKeyMapPage)

  cKeyLut m_keylut;
  int state;
  int kbd_state;
  int curkey;
  int m_menuKeys;
  CString m_command;
  CString m_keystroke;
  bool m_ignoreKeystokeChange;
  bool m_ignoreCommandChange;
};
