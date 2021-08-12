// KeyMapPage.cpp
//{{{  includes
#include "pch.h"
#include "KeyMapPage.h"

#include "KeyMap.h"
#include "Fed.h"
#include "Language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//}}}

//{{{
cKeyMapPage::cKeyMapPage()
   : CDialog(cKeyMapPage::IDD), m_ignoreKeystokeChange (false),
   m_ignoreCommandChange (false), m_keystroke(), m_command() {

  state = 0;
  kbd_state = 0;
  curkey = -1;
  m_keylut.Clear();
  m_menuKeys = 0;
  }
//}}}
cKeyMapPage::~cKeyMapPage() {}

//{{{
void cKeyMapPage::DoDataExchange (CDataExchange* pDX) {

  if (pDX->m_bSaveAndValidate) {
    cKeyMap::PutKeyMap (m_keylut);
    cKeyMap::SaveToRegistry();
    }
  else
    cKeyMap::GetKeyMap (m_keylut);
  }
//}}}

IMPLEMENT_DYNCREATE(cKeyMapPage, CPropertyPage)
//{{{
BEGIN_MESSAGE_MAP(cKeyMapPage, CDialog)
  ON_CONTROL_RANGE (BN_CLICKED, IDC_KEY_0, IDC_KEY_9, OnAsciiKey)
  ON_CONTROL_RANGE (BN_CLICKED, IDC_KEY_A, IDC_KEY_Z, OnAsciiKey)
  ON_BN_CLICKED(IDC_KEY_LSHIFT, OnKeyLshift)
  ON_BN_CLICKED(IDC_KEY_RSHIFT, OnKeyRshift)
  ON_BN_CLICKED(IDC_KEY_LCTRL, OnKeyLctrl)
  ON_BN_CLICKED(IDC_KEY_RCTRL, OnKeyRctrl)
  ON_BN_CLICKED(IDC_KEY_LALT, OnKeyLalt)
  ON_BN_CLICKED(IDC_KEY_RALT, OnKeyRalt)
  ON_BN_CLICKED(IDC_KEY_NUM_0, OnKeyNum0)
  ON_BN_CLICKED(IDC_KEY_NUM_1, OnKeyNum1)
  ON_BN_CLICKED(IDC_KEY_NUM_2, OnKeyNum2)
  ON_BN_CLICKED(IDC_KEY_NUM_3, OnKeyNum3)
  ON_BN_CLICKED(IDC_KEY_NUM_4, OnKeyNum4)
  ON_BN_CLICKED(IDC_KEY_NUM_5, OnKeyNum5)
  ON_BN_CLICKED(IDC_KEY_NUM_6, OnKeyNum6)
  ON_BN_CLICKED(IDC_KEY_NUM_7, OnKeyNum7)
  ON_BN_CLICKED(IDC_KEY_NUM_8, OnKeyNum8)
  ON_BN_CLICKED(IDC_KEY_NUM_9, OnKeyNum9)
  ON_BN_CLICKED(IDC_KEY_NUMDEL, OnKeyNumdel)
  ON_BN_CLICKED(IDC_KEY_NUMENTER, OnKeyNumenter)
  ON_BN_CLICKED(IDC_KEY_NUMMINUS, OnKeyNumminus)
  ON_BN_CLICKED(IDC_KEY_NUMMUL, OnKeyNummul)
  ON_BN_CLICKED(IDC_KEY_NUMPLUS, OnKeyNumplus)
  ON_BN_CLICKED(IDC_KEY_NUMSLASH, OnKeyNumslash)
  ON_BN_CLICKED(IDC_KEY_TAB, OnKeyTab)
  ON_BN_CLICKED(IDC_KEY_COLON,  OnKeyColon)
  ON_BN_CLICKED(IDC_KEY_EQUAL,  OnKeyEqual)
  ON_BN_CLICKED(IDC_KEY_COMMA,  OnKeyComma)
  ON_BN_CLICKED(IDC_KEY_MINUS,  OnKeyMinus)
  ON_BN_CLICKED(IDC_KEY_DOT,    OnKeyDot)
  ON_BN_CLICKED(IDC_KEY_SLASH,  OnKeySlash)
  ON_BN_CLICKED(IDC_KEY_QUOTE1, OnKeyQuote1)
  ON_BN_CLICKED(IDC_KEY_OPEN,   OnKeyOpen)
  ON_BN_CLICKED(IDC_KEY_BSLASH, OnKeyBslash)
  ON_BN_CLICKED(IDC_KEY_CLOSE,  OnKeyClose)
  ON_BN_CLICKED(IDC_KEY_HASH,   OnKeyHash)
  ON_BN_CLICKED(IDC_KEY_QUOTEA, OnKeyQuoteA)
  ON_BN_CLICKED(IDC_KEY_ESC, OnKeyEsc)
  ON_BN_CLICKED(IDC_KEY_F1, OnKeyF1)
  ON_BN_CLICKED(IDC_KEY_F2, OnKeyF2)
  ON_BN_CLICKED(IDC_KEY_F3, OnKeyF3)
  ON_BN_CLICKED(IDC_KEY_F4, OnKeyF4)
  ON_BN_CLICKED(IDC_KEY_F5, OnKeyF5)
  ON_BN_CLICKED(IDC_KEY_F6, OnKeyF6)
  ON_BN_CLICKED(IDC_KEY_F7, OnKeyF7)
  ON_BN_CLICKED(IDC_KEY_F8, OnKeyF8)
  ON_BN_CLICKED(IDC_KEY_F9, OnKeyF9)
  ON_BN_CLICKED(IDC_KEY_F10, OnKeyF10)
  ON_BN_CLICKED(IDC_KEY_F11, OnKeyF11)
  ON_BN_CLICKED(IDC_KEY_F12, OnKeyF12)
  ON_BN_CLICKED(IDC_KEY_INSERT, OnKeyInsert)
  ON_BN_CLICKED(IDC_KEY_HOME, OnKeyHome)
  ON_BN_CLICKED(IDC_KEY_PGUP, OnKeyPgUp)
  ON_BN_CLICKED(IDC_KEY_DELETE, OnKeyDelete)
  ON_BN_CLICKED(IDC_KEY_END, OnKeyEnd)
  ON_BN_CLICKED(IDC_KEY_PGDN, OnKeyPgDn)
  ON_BN_CLICKED(IDC_KEY_UP, OnKeyUp)
  ON_BN_CLICKED(IDC_KEY_LEFT, OnKeyLeft)
  ON_BN_CLICKED(IDC_KEY_DOWN, OnKeyDn)
  ON_BN_CLICKED(IDC_KEY_RIGHT, OnKeyRight)
  ON_BN_CLICKED(IDC_KEY_BS, OnKeyBS)
  ON_BN_CLICKED(IDC_KEY_RETURN, OnKeyReturn)
  ON_BN_CLICKED(IDC_KEYMAP_APPLY, OnApply)
  ON_WM_SYSKEYDOWN()
  ON_LBN_DBLCLK(IDC_COMMAND_LIST, OnDblclkCommandList)
  ON_BN_CLICKED(IDC_KEY_SPACE, OnKeySpace)
  ON_BN_CLICKED(IDC_BUTTON_RESTORE, OnButtonRestore)
  ON_BN_CLICKED(IDC_BUTTON_RESTOREALL, OnButtonRestoreall)
  ON_LBN_SELCANCEL(IDC_KEYS_LIST, OnSelcancelKeyList)
  ON_LBN_SELCHANGE(IDC_KEYS_LIST, OnSelchangeKeyList)
  ON_LBN_SELCANCEL(IDC_COMMAND_LIST, OnSelcancelCommandList)
  ON_LBN_SELCHANGE(IDC_COMMAND_LIST, OnSelchangeCommandList)
  ON_WM_DRAWITEM()
  ON_EN_CHANGE (IDC_COMMAND_EDIT, OnCommandChanged)
  ON_EN_CHANGE (IDC_KEYSTROKE_EDIT, OnKeystrokeChanged)
END_MESSAGE_MAP()
//}}}

//{{{
BOOL cKeyMapPage::OnInitDialog() {

  CDialog::OnInitDialog();
  CListBox* listbox;

  listbox = (CListBox*) GetDlgItem (IDC_KEYS_LIST);
  ASSERT_VALID(listbox);
  for (int state = 0; state < eNumberOfStates; state++) {
    char str [80];
    const char* statename = cKeyMap::StateName (state);
    ASSERT (statename);
    for (int key = VK_BACK; key <= 127; key++) {
      if ((key >= VK_SHIFT) && (key <= VK_CAPITAL))
        continue;
      const char* keyname = cKeyMap::KeyName (key);
      if (keyname && *keyname) {
        if (((state <= 1) && !*(keyname+1)) || ((state == 0) && (key == VK_SPACE)))
          continue;
        strcpy (str, statename);
        strcat (str, keyname);
        listbox->AddString (str);
        }
      }
    }

  listbox = (CListBox*) GetDlgItem (IDC_COMMAND_LIST);
  ASSERT_VALID(listbox);
  for (int token = 0; token < 256; token++) {
    const char* name = cKeyMap::TokenName (token);
    if (name)
      listbox->AddString (name);
    }

  cFedApp* pApp = (cFedApp*) AfxGetApp();
  ASSERT_VALID(pApp);
  m_menuKeys = pApp->GetMenuKeys();

  return TRUE;
  }
//}}}
//{{{
void cKeyMapPage::OnOK() {

  CDialog::OnOK();
  }
//}}}
//{{{
void cKeyMapPage::OnApply() {

  cKeyMap::PutKeyMap (m_keylut);
  cKeyMap::SaveToRegistry();
  SetModified(FALSE);
  }
//}}}
//{{{
void cKeyMapPage::SetModified (BOOL state) {

  CButton* button = static_cast<CButton*>(GetDlgItem (IDC_KEYMAP_APPLY));
  ASSERT_VALID(button);
  button->EnableWindow (state);
  }
//}}}

//{{{
void cKeyMapPage::InvalidateKeyboard() {

  CRect rect;
  GetClientRect(&rect);
  rect.bottom = 110 + 66;            // empirical !!
  InvalidateRect (rect, FALSE);      // FALSE = don't erase
  }
//}}}

//{{{
void cKeyMapPage::ShowAsciiKey (int key, const char* name) {

  curkey = -1;

  // setup 'state' if state-key is held down
  int new_state = 0;
  if (GetKeyState (VK_MENU) < 0)
    new_state |= eAlt;
  if (GetKeyState (VK_CONTROL) < 0)
    new_state |= eCtrl;
  if (GetKeyState (VK_SHIFT) < 0)
    new_state |= eShift;
  if (new_state != kbd_state) {
    state = new_state;
    }

  char str [80] = "";
  if (state > eNormal) {
    strcat (str, cKeyMap::StateName (state));
    }
  if (name)
    strcat (str, name);
  else {
    char* s = strchr (str, 0);
    *s++ = (char) key;
    *s++ = 0;
    }

  SetKeystroke (str);

  if (new_state != kbd_state) {
    kbd_state = new_state;
    InvalidateKeyboard();
    }
  }
//}}}
//{{{
void cKeyMapPage::ShowSpecialKey (int key) {

  curkey = -1;

  // setup 'state' if state-key is held down
  int new_state = 0;
  if (GetKeyState (VK_MENU) < 0)
    new_state |= eAlt;
  if (GetKeyState (VK_CONTROL) < 0)
    new_state |= eCtrl;
  if (GetKeyState (VK_SHIFT) < 0)
    new_state |= eShift;
  if (new_state != kbd_state)
    state = new_state;

  char str [80] = "";
  strcat (str, cKeyMap::StateName (state));
  strcat (str, cKeyMap::KeyName (key));

  SetKeystroke (str);

  if (new_state != kbd_state) {
    kbd_state = new_state;
    InvalidateKeyboard();
    }
  }
//}}}
//{{{
void cKeyMapPage::SetStateKey (int new_state) {

  if (state & new_state)
    state &= ~new_state;      // turn off state
  else
    state |= new_state;       // turn on state

  SetKeystroke (cKeyMap::StateName (state));

  InvalidateKeyboard();
  }
//}}}

//{{{  Key Handlers
//{{{
void cKeyMapPage::OnKeyLshift()
{
  SetStateKey (eShift);
}
//}}}
//{{{
void cKeyMapPage::OnKeyRshift()
{
  SetStateKey (eShift);
}
//}}}
//{{{
void cKeyMapPage::OnKeyLctrl()
{
  SetStateKey (eCtrl);
}
//}}}
//{{{
void cKeyMapPage::OnKeyRctrl()
{
  SetStateKey (eCtrl);
}
//}}}
//{{{
void cKeyMapPage::OnKeyLalt()
{
  SetStateKey (eAlt);
}
//}}}
//{{{
void cKeyMapPage::OnKeyRalt()
{
  SetStateKey (eAlt);
}
//}}}

//{{{
void cKeyMapPage::OnKeyEsc()
{
  ShowSpecialKey (VK_ESCAPE);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF1()
{
  ShowSpecialKey (VK_F1);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF2()
{
  ShowSpecialKey (VK_F2);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF3()
{
  ShowSpecialKey (VK_F3);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF4()
{
  ShowSpecialKey (VK_F4);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF5()
{
  ShowSpecialKey (VK_F5);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF6()
{
  ShowSpecialKey (VK_F6);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF7()
{
  ShowSpecialKey (VK_F7);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF8()
{
  ShowSpecialKey (VK_F8);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF9()
{
  ShowSpecialKey (VK_F9);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF10()
{
  ShowSpecialKey (VK_F10);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF11()
{
  ShowSpecialKey (VK_F11);
}
//}}}
//{{{
void cKeyMapPage::OnKeyF12()
{
  ShowSpecialKey (VK_F12);
}
//}}}

//{{{
void cKeyMapPage::OnKeyInsert()
{
  ShowSpecialKey (VK_INSERT);
}
//}}}
//{{{
void cKeyMapPage::OnKeyHome()
{
  ShowSpecialKey (VK_HOME);
}
//}}}
//{{{
void cKeyMapPage::OnKeyPgUp()
{
  ShowSpecialKey (VK_PRIOR);
}
//}}}
//{{{
void cKeyMapPage::OnKeyDelete()
{
  ShowSpecialKey (VK_DELETE);
}
//}}}
//{{{
void cKeyMapPage::OnKeyEnd()
{
  ShowSpecialKey (VK_END);
}
//}}}
//{{{
void cKeyMapPage::OnKeyPgDn()
{
  ShowSpecialKey (VK_NEXT);
}
//}}}
//{{{
void cKeyMapPage::OnKeyUp()
{
  ShowSpecialKey (VK_UP);
}
//}}}
//{{{
void cKeyMapPage::OnKeyLeft()
{
  ShowSpecialKey (VK_LEFT);
}
//}}}
//{{{
void cKeyMapPage::OnKeyDn()
{
  ShowSpecialKey (VK_DOWN);
}
//}}}
//{{{
void cKeyMapPage::OnKeyRight()
{
  ShowSpecialKey (VK_RIGHT);
}
//}}}

//{{{
void cKeyMapPage::OnKeyQuoteA()
{
  ShowAsciiKey ('`');
}
//}}}
//{{{
void cKeyMapPage::OnKeyBS()
{
  ShowSpecialKey (VK_BACK);
}
//}}}
//{{{
void cKeyMapPage::OnKeyReturn()
{
  ShowSpecialKey (VK_RETURN);
}
//}}}
//{{{
void cKeyMapPage::OnKeyMinus()
{
  ShowAsciiKey ('-');
}
//}}}
//{{{
void cKeyMapPage::OnKeyEqual()
{
  ShowAsciiKey ('=');
}
//}}}
//{{{
void cKeyMapPage::OnKeyOpen()
{
  ShowAsciiKey ('[');
}
//}}}
//{{{
void cKeyMapPage::OnKeyClose()
{
  ShowAsciiKey (']');
}
//}}}
//{{{
void cKeyMapPage::OnKeyColon()
{
  ShowAsciiKey (';');
}
//}}}
//{{{
void cKeyMapPage::OnKeyQuote1()
{
  ShowAsciiKey ('\'');
}
//}}}
//{{{
void cKeyMapPage::OnKeyHash()
{
  ShowAsciiKey ('#');
}
//}}}
//{{{
void cKeyMapPage::OnKeyBslash()
{
  ShowAsciiKey ('\\');
}
//}}}
//{{{
void cKeyMapPage::OnKeyComma()
{
  ShowAsciiKey (',');
}
//}}}
//{{{
void cKeyMapPage::OnKeyDot()
{
  ShowAsciiKey ('.');
}
//}}}
//{{{
void cKeyMapPage::OnKeySlash()
{
  ShowAsciiKey ('/');
}
//}}}

//{{{
void cKeyMapPage::OnKeyNum0()
{
  ShowSpecialKey (VK_NUMPAD0);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum1()
{
  ShowSpecialKey (VK_NUMPAD1);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum2()
{
  ShowSpecialKey (VK_NUMPAD2);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum3()
{
  ShowSpecialKey (VK_NUMPAD3);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum4()
{
  ShowSpecialKey (VK_NUMPAD4);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum5()
{
  ShowSpecialKey (VK_NUMPAD5);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum6()
{
  ShowSpecialKey (VK_NUMPAD6);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum7()
{
  ShowSpecialKey (VK_NUMPAD7);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum8()
{
  ShowSpecialKey (VK_NUMPAD8);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNum9()
{
  ShowSpecialKey (VK_NUMPAD9);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNumdel()
{
  ShowSpecialKey (VK_DECIMAL);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNumenter()
{
  ShowSpecialKey (VK_RETURN);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNumminus()
{
  ShowSpecialKey (VK_SUBTRACT);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNummul()
{
  ShowSpecialKey (VK_MULTIPLY);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNumplus()
{
  ShowSpecialKey (VK_ADD);
}
//}}}
//{{{
void cKeyMapPage::OnKeyNumslash()
{
  ShowSpecialKey (VK_DIVIDE);
}
//}}}

//{{{
void cKeyMapPage::OnKeyTab()
{
  ShowSpecialKey (VK_TAB);
}
//}}}
//{{{
void cKeyMapPage::OnKeySpace()
{
  ShowAsciiKey (' ', "Space");
}
//}}}
//}}}

//{{{
void cKeyMapPage::UpdateKeyMapList (int token, int key) {

  CListBox* listbox = (CListBox*) GetDlgItem (IDC_KEYMAP_LIST);
  ASSERT_VALID(listbox);
  listbox->ResetContent();
  CString str;

  if (token > 0) {
    int key = m_keylut.FindKey (token);
    while (key >= 0) {
      const char* state = cKeyMap::StateName (key / 128);
      const char* name = cKeyMap::KeyName (key % 128);
      if (state && name && *name && ((key > 255) || *(name+1))) {
        str = state;
        str += name;
        listbox->AddString (str);
        }
      key = m_keylut.FindKey (token, key+1);
      }
    }

  if (key > 0) {
    if (token > 0)
      listbox->AddString (" ");
    cLanguage* lang = cLanguage::FirstLanguage();
    while (lang) {
      const char* macroname = lang->FindMacroName (key);
      if (macroname) {
        if (token > 0)
          str.Format ("#%s: %s", lang->Name(), macroname);
        else
          str.Format ("%s: %s", lang->Name(), macroname);
        listbox->AddString (str);
        }
      lang = lang->Next();
      }
    }
  }
//}}}

//{{{
void cKeyMapPage::OnButtonRestore() {

  int key = curkey;
  if (key < 0) {
    CString name;
    CListBox* listbox = (CListBox*) GetDlgItem (IDC_KEYS_LIST);
    listbox->GetText (listbox->GetCurSel(), name);
    key = cKeyMap::FindKey (name);
    }

  if (key >= 0) {
    if (GetKeyState (VK_CONTROL) < 0) {
      int default_token = cKeyMap::MapKeyDefault (key);
      int cur_token = cKeyMap::MapKey (key);
      m_keylut.SetKey (key, default_token);
      if (cur_token != default_token)
        SetModified();
      }
    else
      m_keylut.SetKey (key, cKeyMap::MapKey (key));


    int token = m_keylut.MapKey (key);
    const char* tokenname = cKeyMap::TokenName (token);
    SetCommand (tokenname);

    UpdateKeyMapList (token, key);
    InvalidateKeyboard();
    }
  }
//}}}
//{{{
void cKeyMapPage::OnButtonRestoreall() {

  if (GetKeyState (VK_CONTROL) < 0)
    cKeyMap::GetDefaultKeyMap (m_keylut);
  else
    cKeyMap::GetKeyMap (m_keylut);

  SetModified();

  OnSelchangeKeyList();

  InvalidateKeyboard();
  }
//}}}

//{{{
int cKeyMapPage::MatchEntry (CEdit* editbox, CListBox* listbox, CString& prev) {
// Finds a listbox entry which starts with the editbox string
// If found, it highlights the entry in the listbox, and appends the
// remaining part, selected, to the editbox text. Thus if the user continues
// to type the next char auto-deletes the appended selected text first.

  CString str;
  int matched = LB_ERR;

  editbox->GetWindowText (str);
  int len = str.GetLength();

  if (len > 0) {
    if (str.CompareNoCase (prev) == 0) {
      // must be a delete - assume a backspace delete
      char* s = str.GetBuffer(0);
      len -= 1;
      s[len] = 0;
      str.ReleaseBuffer();
      }
    if ((len > 0) && (str[len-1] != ' ')) {
      CString match;
      int entry = listbox->FindStringExact (-1, str);
      if (entry == LB_ERR)
        entry = listbox->FindString (-1, str);
      if (entry != LB_ERR)
        listbox->GetText (entry, match);
      if (len <= match.GetLength()) {
        editbox->SetWindowText (match);
        editbox->SetSel (len, -1);
        matched = entry;
        }
      }
    else {
      editbox->SetWindowText (str);
      editbox->SetSel (len, -1);
      }
    }

  if (matched)
    prev = str;
  else
    prev.Empty();

  return matched;
  }
//}}}
//{{{
void cKeyMapPage::OnKeystrokeChanged() {

  if (m_ignoreKeystokeChange)
    m_ignoreKeystokeChange = false;

  else {
    CEdit* editbox = (CEdit*) GetDlgItem (IDC_KEYSTROKE_EDIT);
    ASSERT (editbox);
    CListBox* listbox = (CListBox*) GetDlgItem (IDC_KEYS_LIST);
    ASSERT_VALID(listbox);

    CString str;
    int matched = LB_ERR;
    editbox->GetWindowText (str);
    int len = str.GetLength();

    if (len > 0) {
      if (str.CompareNoCase (m_keystroke) == 0) {
        // must be a delete - assume a backspace delete
        char* s = str.GetBuffer(0);
        len -= 1;
        s[len] = 0;
        str.ReleaseBuffer();
        }
      if ((len > 0) && (str[len-1] != ' ')) {
        CString match;
        int matched = listbox->FindStringExact (-1, str);
        if (matched == LB_ERR)
          matched = listbox->FindString (-1, str);
        if (matched != LB_ERR)
          listbox->GetText (matched, match);
        if (len <= match.GetLength()) {
          m_keystroke = str;
          GetDlgItem (IDC_KEYS_LIST)->Invalidate();
          listbox->SetCurSel (matched);

          m_ignoreKeystokeChange = true;
          editbox->SetWindowText (match);
          m_ignoreKeystokeChange = true;
          editbox->SetSel (len, -1);
          }
        else
          matched = LB_ERR;
        }
      else {
        m_ignoreKeystokeChange = true;
        editbox->SetWindowText (str);
        m_ignoreKeystokeChange = true;
        editbox->SetSel (len, -1);
        }
      }
    }
  }
//}}}
//{{{
void cKeyMapPage::OnCommandChanged() {

  if (m_ignoreCommandChange)
    m_ignoreCommandChange = false;

  else {
    CEdit* editbox = (CEdit*) GetDlgItem (IDC_COMMAND_EDIT);
    ASSERT (editbox);
    CListBox* listbox = (CListBox*) GetDlgItem (IDC_COMMAND_LIST);
    ASSERT_VALID(listbox);

    int entry = MatchEntry (editbox, listbox, m_command);

    if (entry >= 0) {
      CString str;
      listbox->GetText (entry, str);
      int token = cKeyMap::FindToken (str);
      UpdateKeyMapList (token, 0);
      }
    }
  }
//}}}

//{{{
void cKeyMapPage::SetKeystroke (const char* str) {

  CString newstr (str);

  int matched = LB_ERR;
  CEdit* editbox = (CEdit*) GetDlgItem (IDC_KEYSTROKE_EDIT);
  ASSERT (editbox);

  CListBox* listbox = (CListBox*) GetDlgItem (IDC_KEYS_LIST);
  ASSERT_VALID(listbox);

  GotoDlgCtrl (editbox);
  int len = str ? (int)strlen(str) : 0;

  if ((len > 0) && (str[len-1] != ' ')) {
    CString match;
    matched = listbox->FindStringExact (-1, str);
    if (matched == LB_ERR)
      matched = listbox->FindString (-1, str);
    if (matched != LB_ERR)
      listbox->GetText (matched, match);
    if (len <= match.GetLength()) {
      m_keystroke = str;
      listbox->Invalidate();
      listbox->SetCurSel (matched);
      newstr = match;
      }
    else
      matched = LB_ERR;
    }

  m_ignoreKeystokeChange = true;
  editbox->SetWindowText (newstr);
  editbox->SetSel (len, -1);

  if (matched)
    OnSelchangeKeyList();
  }
//}}}
//{{{
void cKeyMapPage::SetCommand (const char* str) {

  CString newstr (str);
  int matched = LB_ERR;
  CEdit* editbox = (CEdit*) GetDlgItem (IDC_COMMAND_EDIT);
  ASSERT (editbox);
  CListBox* listbox = (CListBox*) GetDlgItem (IDC_COMMAND_LIST);
  ASSERT_VALID(listbox);
  int len = str ? (int)strlen(str) : 0;

  if (len > 0) {
    CString match;
    matched = listbox->FindStringExact (-1, str);
    if (matched == LB_ERR)
      matched = listbox->FindString (-1, str);
    if (matched != LB_ERR)
      listbox->GetText (matched, match);
    if (len <= match.GetLength()) {
      m_command = str;
      listbox->Invalidate();
      listbox->SetCurSel (matched);
      newstr = match;
      }
    else
      matched = LB_ERR;
    }

  m_ignoreCommandChange = true;
  editbox->SetWindowText (newstr);
  editbox->SetSel (len, -1);
  }
//}}}

//{{{
void cKeyMapPage::OnSelcancelCommandList() {

  CListBox* listbox = (CListBox*) GetDlgItem (IDC_KEYMAP_LIST);
  ASSERT_VALID(listbox);
  listbox->ResetContent();
  }
//}}}
//{{{
void cKeyMapPage::OnSelchangeCommandList() {

  CListBox* listbox;
  listbox = (CListBox*) GetDlgItem (IDC_COMMAND_LIST);
  ASSERT_VALID(listbox);
  int sel = listbox->GetCurSel();

  CString seltext;
  listbox->GetText (sel, seltext);

  int token = cKeyMap::FindToken (seltext);

  UpdateKeyMapList (token, 0);
  }
//}}}
//{{{
void cKeyMapPage::OnDblclkCommandList() {

  CString name;
  CListBox* listbox;

  listbox = (CListBox*) GetDlgItem (IDC_COMMAND_LIST);
  listbox->GetText (listbox->GetCurSel(), name);
  int token = cKeyMap::FindToken (name);

  listbox = (CListBox*) GetDlgItem (IDC_KEYS_LIST);
  listbox->GetText (listbox->GetCurSel(), name);
  int key = cKeyMap::FindKey (name);

  m_keylut.SetKey (key, token);
  SetModified();

  UpdateKeyMapList (token, key);
  InvalidateKeyboard();
  }
//}}}
//{{{
void cKeyMapPage::OnSelcancelKeyList() {

  CListBox* listbox = (CListBox*) GetDlgItem (IDC_COMMAND_LIST);
  listbox->SetCurSel (-1);

  OnSelcancelCommandList();
  }
//}}}
//{{{
void cKeyMapPage::OnSelchangeKeyList() {

  CListBox* listbox;
  listbox = (CListBox*) GetDlgItem (IDC_KEYS_LIST);
  ASSERT_VALID(listbox);
  int sel = listbox->GetCurSel();

  if (sel != LB_ERR) {
    CString seltext;
    listbox->GetText (sel, seltext);

    int key = cKeyMap::FindKey (seltext);
    int token = m_keylut.MapKey (key % 128, key /128);
    const char* tokenname = cKeyMap::TokenName (token);
    SetCommand (tokenname);

    UpdateKeyMapList (token, key);
    }
  }
//}}}

//{{{
void cKeyMapPage::OnDrawItem (int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {

  if (lpDrawItemStruct->CtlType == ODT_BUTTON) {
    CDC dc;
    VERIFY (dc.Attach (lpDrawItemStruct->hDC));
    COLORREF hilight = (lpDrawItemStruct->itemState & ODS_SELECTED) ? RGB (0, 0, 0) : GetSysColor (COLOR_BTNHIGHLIGHT);
    COLORREF shadow = GetSysColor(COLOR_BTNSHADOW);
    COLORREF black = RGB (0, 0, 0);
    COLORREF face = GetSysColor(COLOR_BTNFACE);

    cFedApp* pApp = (cFedApp*) AfxGetApp();
    ASSERT (pApp);
    int menukeys = pApp->GetMenuKeys();

    switch (nIDCtl) {
      case IDC_KEYMAP_COLOUR1:       // mapped
        face += RGB (0, 0, 40);
        break;
      case IDC_KEYMAP_COLOUR2:       // not mapable
        face -= RGB (0, 10, 10);
        break;
      case IDC_KEYMAP_COLOUR3:       // modifier off
        face -= RGB (0, 25, 25);
        break;
      case IDC_KEYMAP_COLOUR4:       // macro
        face += RGB (0, 40, 0);
        break;
      case IDC_KEYMAP_COLOUR5:       // menu
        face -= RGB (160, 0, 0);
        break;
      case IDC_KEYMAP_COLOUR6:       // modifier on
        face += RGB (40, 0, 0);
        break;
      default:
        //{{{  set colour according to mapping
        int key  = nIDCtl & 0x1ff;
        int keybit = ((key >= 'A') && (key <= 'Z')) ? 1 << (key - 'A') : 0;
        BOOL menukey = (state == eAlt) && (keybit & menukeys);

        if ((key == VK_LSHIFT) || (key == VK_RSHIFT)) {
          if (state & eShift)
            face += RGB (40, 0, 0);
          else
            face -= RGB (0, 25, 25);
          }
        else if ((key == VK_LCONTROL) || (key == VK_RCONTROL)) {
          if (state & eCtrl)
            face += RGB (40, 0, 0);
          else
            face -= RGB (0, 25, 25);
          }
        else if ((key == VK_LMENU) || (key == VK_RMENU)) {
          if (state & eAlt)
            face += RGB (40, 0, 0);
          else
            face -= RGB (0, 25, 25);
          }
        else if (key == VK_NUMLOCK) {
          if (state & eGold)
            face += RGB (40, 0, 0);
          else
            face -= RGB (0, 25, 25);
           }
        else if ((key == 0x100+VK_LWIN) || (key == 0x100+VK_RWIN) || (key == 0x100+VK_APPS)) {
          face -= RGB (0, 10, 10);
          }
        else if ((key == VK_PRINT) || (key == VK_SCROLL) || (key == VK_PAUSE) || (key == VK_CAPITAL)) {
          face -= RGB (0, 10, 10);
          }
        else if ((((key >= 0x30) && (key <= 0x5f)) || (key == VK_SPACE)) && (state <= eShift)) {
          face -= RGB (0, 10, 10);
          }
        else if (menukey) {
          face -= RGB (160, 0, 0);
          }
        else {
          key &= 0xff;
          int token = m_keylut.MapKey (key, state);
          int default_token = cKeyMap::MapKeyDefault (key, state);
          if (token > 0) {
            face += RGB (0, 0, 40);
            if (token != default_token)
              face -= RGB (20, 20, 0);
            }
          else {
            if ((key >= 0) && (key <= 127)) {
              key += state * 128;
              cLanguage* lang = cLanguage::FirstLanguage();
              while (lang) {
                if (lang->FindMacro (key)) {
                  face += RGB (0, 40, 0);
                  break;
                  }
                lang = lang->Next();
                }
              }
            if (default_token > 0)
              face += RGB (20, 20, 30);
            }
          }
        //}}}
        break;
      }

    CRect rect (lpDrawItemStruct->rcItem);
    CPoint centre = rect.CenterPoint();

    dc.FillSolidRect (&rect, face);
    rect.right -= 1;
    rect.bottom -= 1;
    dc.Draw3dRect (&rect, hilight, shadow);
    dc.Draw3dRect (&lpDrawItemStruct->rcItem, hilight, RGB(0, 0, 0));

    char str [80];
    if (::GetWindowText (lpDrawItemStruct->hwndItem, str, 80) > 0) {
      CSize sz = dc.GetTextExtent (str);
      centre.x -= sz.cx / 2;
      centre.y -= sz.cy / 2;
      dc.SetBkColor (face);
      dc.TextOut (centre.x, centre.y, str);
      }

    dc.Detach();
    }
  }
//}}}
