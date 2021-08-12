#pragma once
//{{{
enum eTokens {
  k_Nop,                // 00
  k_CharLeft,           // 01
  k_CharRight,          // 02
  k_CharLeftSelect,     // 03
  k_CharRightSelect,    // 04
  k_CharDeleteLeft,     // 05
  k_CharDeleteRight,    // 06
  k_CharToUppercase,    // 07
  k_CharToLowercase,    // 08

  k_WordLeft     =0x10, // 10
  k_WordRight,          // 11
  k_WordSelect,         // 12
  k_WordLeftSelect,     // 13
  k_WordRightSelect,    // 14
  k_WordDelete,         // 15
  k_WordDeleteLeft,     // 16
  k_WordDeleteRight,    // 17
  k_WordToUppercase,    // 18
  k_WordToLowercase,    // 19
  k_WordSelectPart,     // 1a

  k_LineStart    =0x20, // 20
  k_LineEnd,            // 21
  k_LineUp,             // 22
  k_LineDown,           // 23
  k_LineSelect,         // 24
  k_LineStartSelect,    // 25
  k_LineEndSelect,      // 26
  k_LineUpSelect,       // 27
  k_LineDownSelect,     // 28
  k_LineDelete,         // 29
  k_LineDeleteToStart,  // 2a
  k_LineDeleteToEnd,    // 2b
  k_LineUndelete,       // 2c
  k_LineDuplicate,      // 2d
  k_LineMove,           // 2e
  k_LineIndentLess,     // 2f
  k_LineIndentMore,     // 30
  k_LineSwapUp,         // 31
  k_LineSwapDown,       // 32
  k_LineMakeComment,    // 33
  k_LineRemoveComment,  // 34

  k_ScrollLineUp =0x38, // 38
  k_ScrollLineDown,     // 39
  k_ScrollPageUp,       // 3a
  k_ScrollPageDown,     // 3b
  k_ScrollPageUpSelect, // 3c
  k_ScrollPageDownSelect,//3d

  k_GotoFoldTop  =0x40, // 40
  k_GotoFoldBottom,     // 41
  k_GotoFileTop,        // 42
  k_GotoFileBottom,     // 43
  k_GotoProject,        // 44
  k_GotoLineNumber,     // 45
  k_GotoPrevFile,       // 46
  k_GotoNextFile,       // 47
  k_GotoBracketPair = 0x4b,    // 4b
  k_GotoBracketPairSelect, // 4c
  k_GotoPrevFormfeed,   // 4d
  k_GotoNextFormfeed,   // 4e

  k_FoldOpen     =0x50, // 50
  k_FoldClose,          // 51
  k_FoldEnter,          // 52
  k_FoldExit,           // 53
  k_FoldOpenAll,        // 54
  k_FoldCloseAll,       // 55
  k_FoldCreate,         // 56
  k_FoldRemove,         // 57
  k_FoldEnterOpened,    // 58
  k_FoldOpenEntered,    // 59

  k_FileNew      =0x60, // 60
  k_FileOpen,           // 61
  k_FileClose,          // 62
  k_FileCloseAll,       // 63
  k_FileSave,           // 64
  k_FileSaveas,         // 65
  k_FileSaveAll,        // 66
  k_FileInsertHere,     // 67
  k_FileReload,         // 68
  k_FileToggleProtect,  // 69
  k_FileRenumber,       // 6a
  k_FileShowEdits,      // 6b
  k_FileShowLineNumbers,// 6c
  k_FileShowFolds,      // 6d
  k_FileOpenAssociate,  // 6e
  k_FileOpenInclude,    // 6f

  k_Cut          =0x70, // 70
  k_Copy,               // 71
  k_Paste,              // 72
  k_ClearPaste,         // 73
  k_SelectAll,          // 74
  k_UnSelect,           // 75
  k_LineCut,            // 76
  k_LineCopy,           // 77
  k_KeywordComplete,    // 78

  k_FindUp       =0x80, // 80
  k_FindDown,           // 81
  k_Replace,            // 82
  k_Substitute,         // 83
  k_FindDialog,         // 84
  k_FindReplaceDialog,  // 85

  k_MacroDialog  =0x90, // 90
  k_MacroLearn,         // 91
  k_MacroCall,          // 92
  k_MacroLoad,          // 93
  k_MacroSave,          // 94

  k_Tab          =0xa0, // a0
  k_Return,             // a1
  k_FormFeed,           // a2
  k_Undo,               // a3
  k_Refresh,            // a4
  k_Abort,              // a5
  k_Help,               // a6
  k_OverStrike,         // a7
  k_ToggleTabs,         // a8
  k_Command,            // a9
  k_PrintClosed,        // aa
  k_AlignNextColumn,    // ab
  k_DealignNextColumn,  // ac
  k_UndoLine,           // ad
  k_Redo,               // ae

// booleans for macro conditionals only
  k_not          =0xe0, // e0
  k_blank,              // e1
  k_alpha,              // e2
  k_match,              // e2
  k_comment,            // e3

// for macros only
  k_if           =0xf0, // f0
  k_else,               // f1
  k_end,                // f2
  k_while,              // f3
  k_domacro,            // f4

// never allowed in macros (=macro token mark)
  k_ShowDebug    =0xff  // ff

  };
//}}}
//{{{
enum eKeyStates {
  eNormal, eShift, eCtrl, eCtrl_Shift,
  eAlt, eAlt_Shift, eAlt_Ctrl, eAlt_Ctrl_Shift,
  eGold,
  eNumberOfStates,
  eNumberOfKeys = 128
  };
//}}}

struct cKeyLut {
public:
  enum { eMaxKey = 128 * eNumberOfStates };

  void Clear();
  void Initialize (const cKeyLut& keyLut);
  void SetKey (int key, int token);

  int VirtualKey (int key, int state, int scancode) const;
  int MapVirtualKey (int key, int state, int scancode) const;
  const char* VirtualKeyName (int key, int state, int scancode) const;

  int MapKey (int key) const;
  int MapKey (int key, int state) const;

  int FindKey (int token, int start_key = 0) const;

  unsigned char m_keyLut [eMaxKey];
  };

class cKeyMap {
public:
  static void Terminate();   // used to clear heap

  static int VirtualKey (int key, int state, int scancode);
  static int MapVirtualKey (int key, int state, int scancode);
  static const char* VirtualKeyName (int key, int state, int scancode);

  static int MapKeyDefault (int key);
  static int MapKeyDefault (int key, int state);
  static int MapKey (int key);
  static int MapKey (int key, int state);

  static const char* KeyName (int key);
  static const char* TokenName (int token);
  static const char* StateName (int state);

  static int FindToken (const char* name);
  static int FindKey (const char* name);
  static int FindKey (int token, int start_key = 0);
  static int MatchToken (const char* starting, int& match_len);

  static void SetKey (int key, int token);
  static void PutKeyMap (cKeyLut& keyLut);
  static void GetKeyMap (cKeyLut& keyLut);
  static void GetDefaultKeyMap (cKeyLut& keyLut);

  static bool LoadFromRegistry();
  static bool SaveToRegistry();
  static bool SaveToFile (FILE* fp);
  static void LoadFromFile (FILE* fp);

private:
  static const char* kTokenNames [256];
  static const char* kStateNames [eNumberOfStates];
  static const char* kKeyNames [128];
  static const cKeyLut default_keyLut;
  static cKeyLut* active_keyLut;
  };
