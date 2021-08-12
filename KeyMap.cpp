// KeyMap.cpp
//{{{  includes
#include "pch.h"
#include "KeyMap.h"
//}}}

cKeyLut* cKeyMap::active_keyLut = 0;
//{{{
const cKeyLut cKeyMap::default_keyLut = {
  //{{{  0 Normal
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) k_CharDeleteLeft,   // 08  VK_BACK        backspace
  (const unsigned char) k_Tab,              // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) k_Return,           // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) k_KeywordComplete,  // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) k_ScrollPageUp,     // 21  VK_PRIOR       PgUp
  (const unsigned char) k_ScrollPageDown,   // 22  VK_NEXT        PgDn
  (const unsigned char) k_LineEnd,          // 23  VK_END         End
  (const unsigned char) k_LineStart,        // 24  VK_HOME        Home
  (const unsigned char) k_CharLeft,         // 25  VK_LEFT
  (const unsigned char) k_LineUp,           // 26  VK_UP
  (const unsigned char) k_CharRight,        // 27  VK_RIGHT
  (const unsigned char) k_LineDown,         // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) k_Paste,            // 2D  VK_INSERT      Ins
  (const unsigned char) k_CharDeleteRight,  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) 0,                  // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) 0,                  // 45  VK_E
  (const unsigned char) 0,                  // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,                  // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) 0,                  // 4C  VK_L
  (const unsigned char) 0,                  // 4D  VK_M
  (const unsigned char) 0,                  // 4E  VK_N
  (const unsigned char) 0,                  // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) 0,                  // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) 0,                  // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) 0,                  // 55  VK_U
  (const unsigned char) 0,                  // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) 0,                  // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) 0,                  // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) k_FoldCreate,       // 60  VK_NUMPAD0
  (const unsigned char) k_FoldOpen,         // 61  VK_NUMPAD1
  (const unsigned char) k_GotoNextFormfeed, // 62  VK_NUMPAD2
  (const unsigned char) k_FoldClose,        // 63  VK_NUMPAD3
  (const unsigned char) k_GotoPrevFile,     // 64  VK_NUMPAD4
  (const unsigned char) k_Refresh,          // 65  VK_NUMPAD5
  (const unsigned char) k_GotoNextFile,     // 66  VK_NUMPAD6
  (const unsigned char) k_FoldEnter,        // 67  VK_NUMPAD7
  (const unsigned char) k_GotoPrevFormfeed, // 68  VK_NUMPAD8
  (const unsigned char) k_FoldExit,         // 69  VK_NUMPAD9
  (const unsigned char) k_FindDown,         // 6A  VK_MULTIPLY
  (const unsigned char) k_FileOpen,         // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) k_LineDelete,       // 6D  VK_SUBTRACT
  (const unsigned char) k_WordSelect,       // 6E  VK_DECIMAL
  (const unsigned char) k_Replace,          // 6F  VK_DIVIDE
  (const unsigned char) k_Help,             // 70  VK_F1
  (const unsigned char) k_FileSave,         // 71  VK_F2
  (const unsigned char) k_FindDown,         // 72  VK_F3
  (const unsigned char) k_Copy,             // 73  VK_F4
  (const unsigned char) k_FindDown,         // 74  VK_F5
  (const unsigned char) k_Replace,          // 75  VK_F6
  (const unsigned char) k_LineMove,         // 76  VK_F7
  (const unsigned char) k_LineDuplicate,    // 77  VK_F8
  (const unsigned char) 0        ,          // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) k_Command,          // 7A  VK_F11
  (const unsigned char) k_LineMakeComment,  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  1 Shift
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) 0,                  // 08  VK_BACK        backspace
  (const unsigned char) k_ToggleTabs,       // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) k_Abort,            // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) k_ScrollPageUpSelect,   // 21  VK_PRIOR       PgUp
  (const unsigned char) k_ScrollPageDownSelect, // 22  VK_NEXT        PgDn
  (const unsigned char) k_LineEndSelect,    // 23  VK_END         End
  (const unsigned char) k_LineStartSelect,  // 24  VK_HOME        Home
  (const unsigned char) k_CharLeftSelect,   // 25  VK_LEFT
  (const unsigned char) k_LineUpSelect,     // 26  VK_UP
  (const unsigned char) k_CharRightSelect,  // 27  VK_RIGHT
  (const unsigned char) k_LineDownSelect,   // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) k_Paste,            // 2D  VK_INSERT      Ins
  (const unsigned char) k_Cut,              // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) 0,                  // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) 0,                  // 45  VK_E
  (const unsigned char) 0,                  // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,                  // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) 0,                  // 4C  VK_L
  (const unsigned char) 0,                  // 4D  VK_M
  (const unsigned char) 0,                  // 4E  VK_N
  (const unsigned char) 0,                  // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) 0,                  // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) 0,                  // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) 0,                  // 55  VK_U
  (const unsigned char) 0,                  // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) 0,                  // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) 0,                  // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) 0,                  // 60  VK_NUMPAD0
  (const unsigned char) 0,                  // 61  VK_NUMPAD1
  (const unsigned char) 0,                  // 62  VK_NUMPAD2
  (const unsigned char) 0,                  // 63  VK_NUMPAD3
  (const unsigned char) 0,                  // 64  VK_NUMPAD4
  (const unsigned char) 0,                  // 65  VK_NUMPAD5
  (const unsigned char) 0,                  // 66  VK_NUMPAD6
  (const unsigned char) 0,                  // 67  VK_NUMPAD7
  (const unsigned char) 0,                  // 68  VK_NUMPAD8
  (const unsigned char) 0,                  // 69  VK_NUMPAD9
  (const unsigned char) k_FindUp,           // 6A  VK_MULTIPLY
  (const unsigned char) 0,                  // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) 0,                  // 6D  VK_SUBTRACT
  (const unsigned char) 0,                  // 6E  VK_DECIMAL
  (const unsigned char) 0,                  // 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) 0,                  // 71  VK_F2
  (const unsigned char) k_FindUp,           // 72  VK_F3
  (const unsigned char) 0,                  // 73  VK_F4
  (const unsigned char) k_FindUp,           // 74  VK_F5
  (const unsigned char) 0,                  // 75  VK_F6
  (const unsigned char) 0,                  // 76  VK_F7
  (const unsigned char) 0,                  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) 0,                  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  2 Ctrl
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) k_WordDeleteLeft,   // 08  VK_BACK        backspace
  (const unsigned char) k_AlignNextColumn,  // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) k_Abort,            // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) k_GotoFoldTop,      // 21  VK_PRIOR       PgUp
  (const unsigned char) k_GotoFoldBottom,   // 22  VK_NEXT        PgDn
  (const unsigned char) 0,                  // 23  VK_END         End
  (const unsigned char) 0,                  // 24  VK_HOME        Home
  (const unsigned char) k_WordLeft,         // 25  VK_LEFT
  (const unsigned char) k_ScrollLineUp,     // 26  VK_UP
  (const unsigned char) k_WordRight,        // 27  VK_RIGHT
  (const unsigned char) k_ScrollLineDown,   // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) 0,                  // 2D  VK_INSERT      Ins
  (const unsigned char) k_WordDeleteRight,  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) k_SelectAll,        // 41  VK_A
  (const unsigned char) k_LineStart,        // 42  VK_B
  (const unsigned char) k_Copy,             // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) k_LineEnd,          // 45  VK_E
  (const unsigned char) k_FindDialog,       // 46  VK_F
  (const unsigned char) k_GotoLineNumber,   // 47  VK_G
  (const unsigned char) k_FindReplaceDialog,// 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) k_WordToLowercase,  // 4C  VK_L
  (const unsigned char) k_MacroCall,        // 4D  VK_M
  (const unsigned char) 0,                  // 4E  VK_N
  (const unsigned char) 0,                  // 4F  VK_O
  (const unsigned char) k_PrintClosed,      // 50  VK_P
  (const unsigned char) k_FileClose,        // 51  VK_Q
  (const unsigned char) k_FindReplaceDialog,// 52  VK_R
  (const unsigned char) k_FileSave,         // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) k_WordToUppercase,  // 55  VK_U
  (const unsigned char) k_Paste,            // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) k_Cut,              // 58  VK_X
  (const unsigned char) k_Redo,             // 59  VK_Y
  (const unsigned char) k_Undo,             // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) k_FoldRemove,       // 60  VK_NUMPAD0
  (const unsigned char) k_FoldOpenEntered,  // 61  VK_NUMPAD1
  (const unsigned char) 0,    // 62  VK_NUMPAD2
  (const unsigned char) k_FoldCloseAll,     // 63  VK_NUMPAD3
  (const unsigned char) k_LineIndentLess,   // 64  VK_NUMPAD4
  (const unsigned char) k_FileReload,       // 65  VK_NUMPAD5
  (const unsigned char) k_LineIndentMore,   // 66  VK_NUMPAD6
  (const unsigned char) k_FoldEnterOpened,  // 67  VK_NUMPAD7
  (const unsigned char) 0,    // 68  VK_NUMPAD8
  (const unsigned char) k_GotoProject,      // 69  VK_NUMPAD9
  (const unsigned char) 0,                  // 6A  VK_MULTIPLY
  (const unsigned char) k_FileOpenInclude,  // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) k_LineUndelete,     // 6D  VK_SUBTRACT
  (const unsigned char) k_WordSelectPart,   // 6E  VK_DECIMAL
  (const unsigned char) 0,                  // 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) 0,                  // 71  VK_F2
  (const unsigned char) k_FindDialog,       // 72  VK_F3
  (const unsigned char) 0,                  // 73  VK_F4
  (const unsigned char) k_FindDialog,       // 74  VK_F5
  (const unsigned char) k_FindReplaceDialog,// 75  VK_F6
  (const unsigned char) k_LineSwapDown,     // 76  VK_F7
  (const unsigned char) 0,                  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) k_LineRemoveComment,// 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  3 Ctrl+Shift
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) k_LineDeleteToStart,// 08  VK_BACK        backspace
  (const unsigned char) k_DealignNextColumn,// 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) 0,                  // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) k_GotoFileTop,      // 21  VK_PRIOR       PgUp
  (const unsigned char) k_GotoFileBottom,   // 22  VK_NEXT        PgDn
  (const unsigned char) 0,                  // 23  VK_END         End
  (const unsigned char) 0,                  // 24  VK_HOME        Home
  (const unsigned char) k_WordLeftSelect,   // 25  VK_LEFT
  (const unsigned char) 0,                  // 26  VK_UP
  (const unsigned char) k_WordRightSelect,  // 27  VK_RIGHT
  (const unsigned char) 0,                  // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) 0,                  // 2D  VK_INSERT      Ins
  (const unsigned char) k_LineDeleteToEnd,  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) k_FileSaveAll,      // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) k_ShowDebug,        // 44  VK_D
  (const unsigned char) k_FileCloseAll,     // 45  VK_E
  (const unsigned char) k_FileOpen,         // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,                  // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) 0,                  // 4C  VK_L
  (const unsigned char) k_MacroLearn,       // 4D  VK_M
  (const unsigned char) k_FileShowLineNumbers, // 4E  VK_N
  (const unsigned char) k_OverStrike,       // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) k_FileCloseAll,     // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) k_FileSaveAll,      // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) 0,                  // 55  VK_U
  (const unsigned char) 0,                  // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) 0,                  // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) k_UndoLine,         // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) k_FoldRemove,       // 60  VK_NUMPAD0
  (const unsigned char) k_FoldOpenAll,      // 61  VK_NUMPAD1
  (const unsigned char) 0,                  // 62  VK_NUMPAD2
  (const unsigned char) k_FoldCloseAll,     // 63  VK_NUMPAD3
  (const unsigned char) 0,                  // 64  VK_NUMPAD4
  (const unsigned char) 0,                  // 65  VK_NUMPAD5
  (const unsigned char) 0,                  // 66  VK_NUMPAD6
  (const unsigned char) 0,                  // 67  VK_NUMPAD7
  (const unsigned char) 0,                  // 68  VK_NUMPAD8
  (const unsigned char) 0,                  // 69  VK_NUMPAD9
  (const unsigned char) 0,                  // 6A  VK_MULTIPLY
  (const unsigned char) k_FileOpenAssociate,// 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) 0,                  // 6D  VK_SUBTRACT
  (const unsigned char) k_LineSelect,       // 6E  VK_DECIMAL
  (const unsigned char) 0,                  // 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) 0,                  // 71  VK_F2
  (const unsigned char) 0,                  // 72  VK_F3
  (const unsigned char) k_Cut,              // 73  VK_F4
  (const unsigned char) 0,                  // 74  VK_F5
  (const unsigned char) 0,                  // 75  VK_F6
  (const unsigned char) k_LineSwapUp,       // 76  VK_F7
  (const unsigned char) 0,                  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) 0,                  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  4 Alt
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) k_LineDeleteToStart, // 08  VK_BACK        backspace
  (const unsigned char) 0,                  // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) 0,                  // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) k_GotoFileTop,      // 21  VK_PRIOR       PgUp
  (const unsigned char) k_GotoFileBottom,   // 22  VK_NEXT        PgDn
  (const unsigned char) 0,                  // 23  VK_END         End
  (const unsigned char) 0,                  // 24  VK_HOME        Home
  (const unsigned char) k_GotoPrevFile,     // 25  VK_LEFT
  (const unsigned char) k_FoldExit,         // 26  VK_UP
  (const unsigned char) k_GotoNextFile,     // 27  VK_RIGHT
  (const unsigned char) k_FoldEnter,        // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) k_ClearPaste,       // 2D  VK_INSERT      Ins
  (const unsigned char) k_LineDeleteToEnd,  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) k_FileSaveAll,      // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) 0,   // Edit        // 45  VK_E
  (const unsigned char) 0,   // File        // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,   // Help        // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) k_CharToLowercase,  // 4C  VK_L
  (const unsigned char) 0,                  // 4D  VK_M
  (const unsigned char) k_GotoLineNumber,   // 4E  VK_N
  (const unsigned char) 0,   // Options     // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) k_FileClose,        // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) k_FileSave,         // 53  VK_S
  (const unsigned char) 0,   // Tools       // 54  VK_T
  (const unsigned char) k_CharToUppercase,  // 55  VK_U
  (const unsigned char) 0,   // View        // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) k_FileClose,        // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) 0,                  // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) 0,                  // 60  VK_NUMPAD0
  (const unsigned char) 0,                  // 61  VK_NUMPAD1
  (const unsigned char) 0,                  // 62  VK_NUMPAD2
  (const unsigned char) 0,                  // 63  VK_NUMPAD3
  (const unsigned char) 0,                  // 64  VK_NUMPAD4
  (const unsigned char) 0,                  // 65  VK_NUMPAD5
  (const unsigned char) 0,                  // 66  VK_NUMPAD6
  (const unsigned char) 0,                  // 67  VK_NUMPAD7
  (const unsigned char) 0,                  // 68  VK_NUMPAD8
  (const unsigned char) 0,                  // 69  VK_NUMPAD9
  (const unsigned char) k_FindDialog,       // 6A  VK_MULTIPLY
  (const unsigned char) 0,                  // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) k_LineUndelete,     // 6D  VK_SUBTRACT
  (const unsigned char) 0,                  // 6E  VK_DECIMAL
  (const unsigned char) k_FindReplaceDialog,// 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) k_FileSaveas,       // 71  VK_F2
  (const unsigned char) 0,                  // 72  VK_F3
  (const unsigned char) 0,                  // 73  VK_F4
  (const unsigned char) k_FindDialog,       // 74  VK_F5
  (const unsigned char) k_FindReplaceDialog,// 75  VK_F6
  (const unsigned char) k_WordDeleteLeft,   // 76  VK_F7
  (const unsigned char) k_WordDeleteRight,  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) 0,                  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  5 Alt+Shift
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) 0,                  // 08  VK_BACK        backspace
  (const unsigned char) 0,                  // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) 0,                  // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) 0,                  // 21  VK_PRIOR       PgUp
  (const unsigned char) 0,                  // 22  VK_NEXT        PgDn
  (const unsigned char) 0,                  // 23  VK_END         End
  (const unsigned char) 0,                  // 24  VK_HOME        Home
  (const unsigned char) 0,                  // 25  VK_LEFT
  (const unsigned char) 0,                  // 26  VK_UP
  (const unsigned char) 0,                  // 27  VK_RIGHT
  (const unsigned char) 0,                  // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) 0,                  // 2D  VK_INSERT      Ins
  (const unsigned char) 0,                  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) 0,                  // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) 0,                  // 45  VK_E
  (const unsigned char) 0,                  // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,                  // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) 0,                  // 4C  VK_L
  (const unsigned char) 0,                  // 4D  VK_M
  (const unsigned char) 0,                  // 4E  VK_N
  (const unsigned char) 0,                  // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) 0,                  // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) 0,                  // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) 0,                  // 55  VK_U
  (const unsigned char) 0,                  // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) 0,                  // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) 0,                  // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) 0,                  // 60  VK_NUMPAD0
  (const unsigned char) 0,                  // 61  VK_NUMPAD1
  (const unsigned char) 0,                  // 62  VK_NUMPAD2
  (const unsigned char) 0,                  // 63  VK_NUMPAD3
  (const unsigned char) 0,                  // 64  VK_NUMPAD4
  (const unsigned char) 0,                  // 65  VK_NUMPAD5
  (const unsigned char) 0,                  // 66  VK_NUMPAD6
  (const unsigned char) 0,                  // 67  VK_NUMPAD7
  (const unsigned char) 0,                  // 68  VK_NUMPAD8
  (const unsigned char) 0,                  // 69  VK_NUMPAD9
  (const unsigned char) 0,                  // 6A  VK_MULTIPLY
  (const unsigned char) 0,                  // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) 0,                  // 6D  VK_SUBTRACT
  (const unsigned char) 0,                  // 6E  VK_DECIMAL
  (const unsigned char) 0,                  // 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) 0,                  // 71  VK_F2
  (const unsigned char) 0,                  // 72  VK_F3
  (const unsigned char) 0,                  // 73  VK_F4
  (const unsigned char) 0,                  // 74  VK_F5
  (const unsigned char) 0,                  // 75  VK_F6
  (const unsigned char) 0,                  // 76  VK_F7
  (const unsigned char) 0,                  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) 0,                  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  6 Alt+Ctrl
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) 0,                  // 08  VK_BACK        backspace
  (const unsigned char) 0,                  // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) 0,                  // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) 0,                  // 21  VK_PRIOR       PgUp
  (const unsigned char) 0,                  // 22  VK_NEXT        PgDn
  (const unsigned char) 0,                  // 23  VK_END         End
  (const unsigned char) 0,                  // 24  VK_HOME        Home
  (const unsigned char) 0,                  // 25  VK_LEFT
  (const unsigned char) 0,                  // 26  VK_UP
  (const unsigned char) 0,                  // 27  VK_RIGHT
  (const unsigned char) 0,                  // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) 0,                  // 2D  VK_INSERT      Ins
  (const unsigned char) 0,                  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) 0,                  // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) 0,                  // 45  VK_E
  (const unsigned char) 0,                  // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,                  // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) 0,                  // 4C  VK_L
  (const unsigned char) 0,                  // 4D  VK_M
  (const unsigned char) 0,                  // 4E  VK_N
  (const unsigned char) 0,                  // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) 0,                  // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) 0,                  // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) 0,                  // 55  VK_U
  (const unsigned char) 0,                  // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) 0,                  // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) 0,                  // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) 0,                  // 60  VK_NUMPAD0
  (const unsigned char) 0,                  // 61  VK_NUMPAD1
  (const unsigned char) 0,                  // 62  VK_NUMPAD2
  (const unsigned char) 0,                  // 63  VK_NUMPAD3
  (const unsigned char) 0,                  // 64  VK_NUMPAD4
  (const unsigned char) 0,                  // 65  VK_NUMPAD5
  (const unsigned char) 0,                  // 66  VK_NUMPAD6
  (const unsigned char) 0,                  // 67  VK_NUMPAD7
  (const unsigned char) 0,                  // 68  VK_NUMPAD8
  (const unsigned char) 0,                  // 69  VK_NUMPAD9
  (const unsigned char) 0,                  // 6A  VK_MULTIPLY
  (const unsigned char) 0,                  // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) 0,                  // 6D  VK_SUBTRACT
  (const unsigned char) 0,                  // 6E  VK_DECIMAL
  (const unsigned char) 0,                  // 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) 0,                  // 71  VK_F2
  (const unsigned char) 0,                  // 72  VK_F3
  (const unsigned char) 0,                  // 73  VK_F4
  (const unsigned char) 0,                  // 74  VK_F5
  (const unsigned char) 0,                  // 75  VK_F6
  (const unsigned char) 0,                  // 76  VK_F7
  (const unsigned char) 0,                  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) 0,                  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  7 Alt+Ctrl+Shift
  (const unsigned char) 0,                  // 00
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) 0,                  // 08  VK_BACK        backspace
  (const unsigned char) 0,                  // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) 0,                  // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) 0,                  // 21  VK_PRIOR       PgUp
  (const unsigned char) 0,                  // 22  VK_NEXT        PgDn
  (const unsigned char) 0,                  // 23  VK_END         End
  (const unsigned char) 0,                  // 24  VK_HOME        Home
  (const unsigned char) 0,                  // 25  VK_LEFT
  (const unsigned char) 0,                  // 26  VK_UP
  (const unsigned char) 0,                  // 27  VK_RIGHT
  (const unsigned char) 0,                  // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) 0,                  // 2D  VK_INSERT      Ins
  (const unsigned char) 0,                  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) 0,                  // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) 0,                  // 45  VK_E
  (const unsigned char) 0,                  // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,                  // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) 0,                  // 4C  VK_L
  (const unsigned char) 0,                  // 4D  VK_M
  (const unsigned char) 0,                  // 4E  VK_N
  (const unsigned char) 0,                  // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) 0,                  // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) 0,                  // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) 0,                  // 55  VK_U
  (const unsigned char) 0,                  // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) 0,                  // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) 0,                  // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) 0,                  // 60  VK_NUMPAD0
  (const unsigned char) 0,                  // 61  VK_NUMPAD1
  (const unsigned char) 0,                  // 62  VK_NUMPAD2
  (const unsigned char) 0,                  // 63  VK_NUMPAD3
  (const unsigned char) 0,                  // 64  VK_NUMPAD4
  (const unsigned char) 0,                  // 65  VK_NUMPAD5
  (const unsigned char) 0,                  // 66  VK_NUMPAD6
  (const unsigned char) 0,                  // 67  VK_NUMPAD7
  (const unsigned char) 0,                  // 68  VK_NUMPAD8
  (const unsigned char) 0,                  // 69  VK_NUMPAD9
  (const unsigned char) 0,                  // 6A  VK_MULTIPLY
  (const unsigned char) 0,                  // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) 0,                  // 6D  VK_SUBTRACT
  (const unsigned char) 0,                  // 6E  VK_DECIMAL
  (const unsigned char) 0,                  // 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) 0,                  // 71  VK_F2
  (const unsigned char) 0,                  // 72  VK_F3
  (const unsigned char) 0,                  // 73  VK_F4
  (const unsigned char) 0,                  // 74  VK_F5
  (const unsigned char) 0,                  // 75  VK_F6
  (const unsigned char) 0,                  // 76  VK_F7
  (const unsigned char) 0,                  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) 0,                  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
  //{{{  8 Gold
  (const unsigned char) k_Refresh,          // 00  remapped VK_NUMLOCK
  (const unsigned char) 0,                  // 01  VK_LBUTTON     left mouse button
  (const unsigned char) 0,                  // 02  VK_RBUTTON     right mouse button
  (const unsigned char) 0,                  // 03  VK_CANCEL      control-break
  (const unsigned char) 0,                  // 04  VK_MBUTTON     middle mouse button
  (const unsigned char) 0,                  // 05
  (const unsigned char) 0,                  // 06
  (const unsigned char) 0,                  // 07
  (const unsigned char) 0,                  // 08  VK_BACK        backspace
  (const unsigned char) 0,                  // 09  VK_TAB
  (const unsigned char) 0,                  // 0A
  (const unsigned char) 0,                  // 0B
  (const unsigned char) 0,                  // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  (const unsigned char) 0,                  // 0D  VK_RETURN
  (const unsigned char) 0,                  // 0E
  (const unsigned char) 0,                  // 0F
  (const unsigned char) 0,                  // 10  VK_SHIFT
  (const unsigned char) 0,                  // 11  VK_CONTROL
  (const unsigned char) 0,                  // 12  VK_MENU        alt key
  (const unsigned char) 0,                  // 13  VK_PAUSE
  (const unsigned char) 0,                  // 14  VK_CAPITAL     caps lock
  (const unsigned char) 0,                  // 15
  (const unsigned char) 0,                  // 16
  (const unsigned char) 0,                  // 17
  (const unsigned char) 0,                  // 18
  (const unsigned char) 0,                  // 19
  (const unsigned char) 0,                  // 1A
  (const unsigned char) 0,                  // 1B  VK_ESCAPE      Esc
  (const unsigned char) 0,                  // 1C
  (const unsigned char) 0,                  // 1D
  (const unsigned char) 0,                  // 1E
  (const unsigned char) 0,                  // 1F
  (const unsigned char) 0,                  // 20  VK_SPACE       spacebar
  (const unsigned char) 0,                  // 21  VK_PRIOR       PgUp
  (const unsigned char) 0,                  // 22  VK_NEXT        PgDn
  (const unsigned char) 0,                  // 23  VK_END         End
  (const unsigned char) 0,                  // 24  VK_HOME        Home
  (const unsigned char) 0,                  // 25  VK_LEFT
  (const unsigned char) 0,                  // 26  VK_UP
  (const unsigned char) 0,                  // 27  VK_RIGHT
  (const unsigned char) 0,                  // 28  VK_DOWN
  (const unsigned char) 0,                  // 29  VK_SELECT      ?
  (const unsigned char) 0,                  // 2A
  (const unsigned char) 0,                  // 2B  VK_EXECUTE     ?
  (const unsigned char) 0,                  // 2C  VK_SNAPSHOT    ? Print screen
  (const unsigned char) 0,                  // 2D  VK_INSERT      Ins
  (const unsigned char) 0,                  // 2E  VK_DELETE      Del
  (const unsigned char) 0,                  // 2F  VK_HELP        ?
  (const unsigned char) 0,                  // 30  VK_0
  (const unsigned char) 0,                  // 31  VK_1
  (const unsigned char) 0,                  // 32  VK_2
  (const unsigned char) 0,                  // 33  VK_3
  (const unsigned char) 0,                  // 34  VK_4
  (const unsigned char) 0,                  // 35  VK_5
  (const unsigned char) 0,                  // 36  VK_6
  (const unsigned char) 0,                  // 37  VK_7
  (const unsigned char) 0,                  // 38  VK_8
  (const unsigned char) 0,                  // 39  VK_9
  (const unsigned char) 0,                  // 3A  ;:             BA remapped
  (const unsigned char) 0,                  // 3B  =+             BB remapped
  (const unsigned char) 0,                  // 3C  ,<             BC remapped
  (const unsigned char) 0,                  // 3D  -_             BD remapped
  (const unsigned char) 0,                  // 3E  .>             BE remapped
  (const unsigned char) 0,                  // 3F  /?             BF remapped
  (const unsigned char) 0,                  // 40  '@             C0 remapped
  (const unsigned char) 0,                  // 41  VK_A
  (const unsigned char) 0,                  // 42  VK_B
  (const unsigned char) 0,                  // 43  VK_C
  (const unsigned char) 0,                  // 44  VK_D
  (const unsigned char) 0,                  // 45  VK_E
  (const unsigned char) 0,                  // 46  VK_F
  (const unsigned char) 0,                  // 47  VK_G
  (const unsigned char) 0,                  // 48  VK_H
  (const unsigned char) 0,                  // 49  VK_I
  (const unsigned char) 0,                  // 4A  VK_J
  (const unsigned char) 0,                  // 4B  VK_K
  (const unsigned char) 0,                  // 4C  VK_L
  (const unsigned char) 0,                  // 4D  VK_M
  (const unsigned char) 0,                  // 4E  VK_N
  (const unsigned char) 0,                  // 4F  VK_O
  (const unsigned char) 0,                  // 50  VK_P
  (const unsigned char) 0,                  // 51  VK_Q
  (const unsigned char) 0,                  // 52  VK_R
  (const unsigned char) 0,                  // 53  VK_S
  (const unsigned char) 0,                  // 54  VK_T
  (const unsigned char) 0,                  // 55  VK_U
  (const unsigned char) 0,                  // 56  VK_V
  (const unsigned char) 0,                  // 57  VK_W
  (const unsigned char) 0,                  // 58  VK_X
  (const unsigned char) 0,                  // 59  VK_Y
  (const unsigned char) 0,                  // 5A  VK_Z
  (const unsigned char) 0,                  // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  (const unsigned char) 0,                  // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  (const unsigned char) 0,                  // 5D  ]}               DD remapped (VK_APPS  Application Key)
  (const unsigned char) 0,                  // 5E  #~               DE remapped
  (const unsigned char) 0,                  // 5F  `¬               DF remapped
  (const unsigned char) 0,                  // 60  VK_NUMPAD0
  (const unsigned char) k_FoldOpenAll,      // 61  VK_NUMPAD1
  (const unsigned char) 0,                  // 62  VK_NUMPAD2
  (const unsigned char) k_FoldCloseAll,     // 63  VK_NUMPAD3
  (const unsigned char) 0,                  // 64  VK_NUMPAD4
  (const unsigned char) 0,                  // 65  VK_NUMPAD5
  (const unsigned char) 0,                  // 66  VK_NUMPAD6
  (const unsigned char) 0,                  // 67  VK_NUMPAD7
  (const unsigned char) 0,                  // 68  VK_NUMPAD8
  (const unsigned char) 0,                  // 69  VK_NUMPAD9
  (const unsigned char) 0,                  // 6A  VK_MULTIPLY
  (const unsigned char) 0,                  // 6B  VK_ADD
  (const unsigned char) 0,                  // 6C    VK_SEPARATOR
  (const unsigned char) 0,                  // 6D  VK_SUBTRACT
  (const unsigned char) 0,                  // 6E  VK_DECIMAL
  (const unsigned char) 0,                  // 6F  VK_DIVIDE
  (const unsigned char) 0,                  // 70  VK_F1
  (const unsigned char) 0,                  // 71  VK_F2
  (const unsigned char) 0,                  // 72  VK_F3
  (const unsigned char) 0,                  // 73  VK_F4
  (const unsigned char) 0,                  // 74  VK_F5
  (const unsigned char) 0,                  // 75  VK_F6
  (const unsigned char) 0,                  // 76  VK_F7
  (const unsigned char) 0,                  // 77  VK_F8
  (const unsigned char) 0,                  // 78  VK_F9
  (const unsigned char) 0,                  // 79  VK_F10
  (const unsigned char) 0,                  // 7A  VK_F11
  (const unsigned char) 0,                  // 7B  VK_F12
  (const unsigned char) 0,                  // 7C    VK_F13
  (const unsigned char) 0,                  // 7D    VK_F14
  (const unsigned char) 0,                  // 7E    VK_F15
  (const unsigned char) 0,                  // 7F    VK_F16
  //}}}
 };

//{{{  standard Windows keys
//          Windows            Alternatives         Altera
// Ctrl+A   Select All
// Ctrl+B
// Ctrl+C   Copy
// Ctrl+D
// Ctrl+E
// Ctrl+F   Find Dialog
// Ctrl+G   Find Next           F3  Ctrl N           Goto Line
// Ctrl+H   Replace Dialog      Ctrl R
// Ctrl+I
// Ctrl+J
// Ctrl+K
// Ctrl+L
// Ctrl+M                                            Find Matching Delimiter
// Ctrl+N   New File                                 Find Next
// Ctrl+O   Open File
// Ctrl+P   Print Dialog
// Ctrl+Q
// Ctrl+R                                            Replace Dialog
// Ctrl+S   Save File
// Ctrl+T
// Ctrl+U
// Ctrl+V   Paste
// Ctrl+W
// Ctrl+X   Cut
// Ctrl+Y   Redo
// Ctrl+Z   Undo

// Shift+F1
// Shift+F2
// Shift+F3
// Shift+F4  Tile Horizontally
// Shift+F5  Tile Cascade
// Shift+F6
// Shift+F7
// Shift+F8
// Shift+F9
// Shift+F10

// Ctrl+F1
// Ctrl+F2
// Ctrl+F3
// Ctrl+F4  Close File
// Ctrl+F5
// Ctrl+F6
// Ctrl+F7
// Ctrl+F8
// Ctrl+F9
// Ctrl+F10

// Alt+F1
// Alt+F2
// Alt+F3
// Alt+F4   Exit Fed
// Alt+F5
// Alt+F6
// Alt+F7
// Alt+F8
// Alt+F9
// Alt+F10

//          Mouse Cut & Paste
//          Always-on-top option
//          Different colour in KeyMap to show User defined keys
//}}}
//}}}
//{{{
const char* cKeyMap::kKeyNames [128] = {
  "",              // 00
  "Left Mouse",    // 01  VK_LBUTTON     left mouse button
  "Right Mouse",   // 02  VK_RBUTTON     right mouse button
  "Break",         // 03  VK_CANCEL      control-break
  "Middle Mouse",  // 04  VK_MBUTTON     middle mouse button
  "",              // 05
  "",              // 06
  "",              // 07
  "Backspace",     // 08  VK_BACK        backspace
  "Tab",           // 09  VK_TAB
  "",              // 0A
  "",              // 0B
  "Clear",         // 0C  VK_CLEAR       NumPad 5 (NumLock off)
  "Return",        // 0D  VK_RETURN
  "",              // 0E
  "",              // 0F
  "Shift",         // 10  VK_SHIFT
  "Control",       // 11  VK_CONTROL
  "Alt",           // 12  VK_MENU        alt key
  "Pause",         // 13  VK_PAUSE
  "CapsLock",      // 14  VK_CAPITAL     caps lock
  "",              // 15   Kanji reserved
  "",              // 16   Kanji reserved
  "",              // 17   Kanji reserved
  "",              // 18   Kanji reserved
  "",              // 19   Kanji reserved
  "",              // 1A
  "Esc",           // 1B  VK_ESCAPE      Esc
  "",              // 1C   Kanji reserved
  "",              // 1D   Kanji reserved
  "",              // 1E   Kanji reserved
  "",              // 1F   Kanji reserved
  "Spacebar",      // 20  VK_SPACE       spacebar
  "PgUp",          // 21  VK_PRIOR       PgUp
  "PgDn",          // 22  VK_NEXT        PgDn
  "End",           // 23  VK_END         End
  "Home",          // 24  VK_HOME        Home
  "Left",          // 25  VK_LEFT
  "Up",            // 26  VK_UP
  "Right",         // 27  VK_RIGHT
  "Down",          // 28  VK_DOWN
  "Select",        // 29  VK_SELECT      ?
  "",              // 2A  VK_PRINT
  "Execute",       // 2B  VK_EXECUTE     ?
  "Snapshot",      // 2C  VK_SNAPSHOT    ? Print screen
  "Insert",        // 2D  VK_INSERT      Ins
  "Delete",        // 2E  VK_DELETE      Del
  "Help",          // 2F  VK_HELP        ?
  "0",             // 30  VK_0
  "1",             // 31  VK_1
  "2",             // 32  VK_2
  "3",             // 33  VK_3
  "4",             // 34  VK_4
  "5",             // 35  VK_5
  "6",             // 36  VK_6
  "7",             // 37  VK_7
  "8",             // 38  VK_8
  "9",             // 39  VK_9
  ";",             // 3A  ;:             BA remapped
  "=",             // 3B  =+             BB remapped
  ",",             // 3C  ,<             BC remapped
  "-",             // 3D  -_             BD remapped
  ".",             // 3E  .>             BE remapped
  "/",             // 3F  /?             BF remapped
  "'",             // 40  '@             C0 remapped
  "A",             // 41  VK_A
  "B",             // 42  VK_B
  "C",             // 43  VK_C
  "D",             // 44  VK_D
  "E",             // 45  VK_E
  "F",             // 46  VK_F
  "G",             // 47  VK_G
  "H",             // 48  VK_H
  "I",             // 49  VK_I
  "J",             // 4A  VK_J
  "K",             // 4B  VK_K
  "L",             // 4C  VK_L
  "M",             // 4D  VK_M
  "N",             // 4E  VK_N
  "O",             // 4F  VK_O
  "P",             // 50  VK_P
  "Q",             // 51  VK_Q
  "R",             // 52  VK_R
  "S",             // 53  VK_S
  "T",             // 54  VK_T
  "U",             // 55  VK_U
  "V",             // 56  VK_V
  "W",             // 57  VK_W
  "X",             // 58  VK_X
  "Y",             // 59  VK_Y
  "Z",             // 5A  VK_Z
  "[",             // 5B  [{               DB remapped (VK_LWIN  Left Windows Key)
  "\\",            // 5C  \|               DC remapped (VK_RWIN  Right Windows Key)
  "]",             // 5D  ]}               DD remapped (VK_APPS  Application Key)
  "#",             // 5E  #~               DE remapped
  "`",             // 5F  `¬               DF remapped
  "Numpad0",         // 60  VK_NUMPAD0
  "Numpad1",         // 61  VK_NUMPAD1
  "Numpad2",         // 62  VK_NUMPAD2
  "Numpad3",         // 63  VK_NUMPAD3
  "Numpad4",         // 64  VK_NUMPAD4
  "Numpad5",         // 65  VK_NUMPAD5
  "Numpad6",         // 66  VK_NUMPAD6
  "Numpad7",         // 67  VK_NUMPAD7
  "Numpad8",         // 68  VK_NUMPAD8
  "Numpad9",         // 69  VK_NUMPAD9
  "Numpad*",         // 6A  VK_MULTIPLY
  "Numpad+",         // 6B  VK_ADD
  "Numpad?",         // 6C    VK_SEPARATOR
  "Numpad-",         // 6D  VK_SUBTRACT
  "Numpad.",         // 6E  VK_DECIMAL
  "Numpad/",         // 6F  VK_DIVIDE
  "F1",            // 70  VK_F1
  "F2",            // 71  VK_F2
  "F3",            // 72  VK_F3
  "F4",            // 73  VK_F4
  "F5",            // 74  VK_F5
  "F6",            // 75  VK_F6
  "F7",            // 76  VK_F7
  "F8",            // 77  VK_F8
  "F9",            // 78  VK_F9
  "F10",           // 79  VK_F10
  "F11",           // 7A  VK_F11
  "F12",           // 7B  VK_F12
  "F13",           // 7C    VK_F13
  "F14",           // 7D    VK_F14
  "F15",           // 7E    VK_F15
  "F16",           // 7F    VK_F16
  };
//}}}
//{{{
const char* cKeyMap::kTokenNames [256] = {
  "Nop",                // 00
  "CharLeft",           // 01
  "CharRight",          // 02
  "CharLeftSelect",     // 03
  "CharRightSelect",    // 04
  "CharDeleteLeft",     // 05
  "CharDeleteRight",    // 06
  "CharToUppercase",    // 07
  "CharToLowercase",    // 08
  0,                    // 09
  0,                    // 0a
  0,                    // 0b
  0,0,0,0,              // 0c-0f

  "WordLeft",           // 10
  "WordRight",          // 11
  "WordSelect",         // 12
  "WordLeftSelect",     // 13
  "WordRightSelect",    // 14
  "WordDelete",         // 15
  "WordDeleteLeft",     // 16
  "WordDeleteRight",    // 17
  "WordToUppercase",    // 18
  "WordToLowercase",    // 19
  "WordSelectPart",     // 1a
  0,                    // 1b
  0,0,0,0,              // 1c-1f

  "LineStart",          // 20
  "LineEnd",            // 21
  "LineUp",             // 22
  "LineDown",           // 23
  "LineSelect",         // 24
  "LineStartSelect",    // 25
  "LineEndSelect",      // 26
  "LineUpSelect",       // 27
  "LineDownSelect",     // 28
  "LineDelete",         // 29
  "LineDeleteToStart",  // 2a
  "LineDeleteToEnd",    // 2b
  "LineUndelete",       // 2c
  "LineDuplicate",      // 2d
  "LineMove",           // 2e
  "LineIndentLess",     // 2f
  "LineIndentMore",     // 30
  "LineSwapUp",         // 31
  "LineSwapDown",       // 32
  "LineMakeComment",    // 33
  "LineRemoveComment",  // 34
  0,                    // 35
  0,                    // 36
  0,                    // 37

  "ScrollLineUp",       // 38
  "ScrollLineDown",     // 39
  "ScrollPageUp",       // 3a
  "ScrollPageDown",     // 3b
  "ScrollPageUpSelect", // 3c
  "ScrollPageDownSelect",//3d
  0,                    // 3e
  0,                    // 3f

  "GotoFoldTop",        // 40
  "GotoFoldBottom",     // 41
  "GotoFileTop",        // 42
  "GotoFileBottom",     // 43
  "GotoProject",        // 44
  "GotoLineNumber",     // 45
  "GotoPrevFile",       // 46
  "GotoNextFile",       // 47
  0,      // 48
  0,      // 49
  0,     // 4a
  "GotoBracketPair",    // 4b
  "GotoBracketPairSelect", // 4c
  "GotoPrevFormfeed",   // 4d
  "GotoNextFormfeed",   // 4e
  0,                    // 4f

  "FoldOpen",           // 50
  "FoldClose",          // 51
  "FoldEnter",          // 52
  "FoldExit",           // 53
  "FoldOpenAll",        // 54
  "FoldCloseAll",       // 55
  "FoldCreate",         // 56
  "FoldRemove",         // 57
  "FoldEnterOpened",    // 58
  "FoldOpenEntered",    // 59
  0,                    // 5a
  0,                    // 5b
  0,0,0,0,              // 5c-5f

  "FileNew",            // 60
  "FileOpen",           // 61
  "FileClose",          // 62
  "FileCloseAll",       // 63
  "FileSave",           // 64
  "FileSaveas",         // 65
  "FileSaveAll",        // 66
  "FileInsertHere",     // 67
  "FileReload",         // 68
  "FileToggleProtect",  // 69
  "FileRenumber",       // 6a
  "FileShowEdits",      // 6b
  "FileShowLineNumbers",// 6c
  "FileShowFolds",      // 6d
  "FileOpenAssociate",  // 6e
  "FileOpenInclude",    // 6f

  "Cut",                // 70
  "Copy",               // 71
  "Paste",              // 72
  "ClearPaste",         // 73
  "SelectAll",          // 74
  "UnSelect",           // 75
  "LineCut",            // 76
  "LineCopy",           // 77
  "KeywordComplete",    // 78
  0,                    // 79
  0,                    // 7a
  0,                    // 7b
  0,0,0,0,              // 7c-7f

  "FindUp",             // 80
  "FindDown",           // 81
  "Replace",            // 82
  "Substitute",         // 83
  "FindDialog",         // 84
  "FindReplaceDialog",  // 85
  0,                    // 86
  0,                    // 87
  0,0,0,0, 0,0,0,0,     // 88-8f

  "MacroDialog",        // 90
  "MacroLearn",         // 91
  "MacroCall",          // 92
  "MacroLoad",          // 93
  "MacroSave",          // 94
  0,                    // 95
  0,                    // 96
  0,                    // 97
  0,0,0,0, 0,0,0,0,     // 98-9f

  "Tab",                // a0
  "Return",             // a1
  "FormFeed",           // a2
  "Undo",               // a3
  "Refresh",            // a4
  "Abort",              // a5
  "Help",               // a6
  "OverStrike",         // a7
  "ToggleTabs",         // a8
  "Command",            // a9
  "PrintClosed",        // aa
  "AlignNextColumn",    // ab
  "DealignNextColumn",  // ac
  "UndoLine",           // ad
  "Redo",               // ae
  0,                    // af

  0,                    // b0
  0,0,0,                // b1-b3
  0,0,0,0, 0,0,0,0, 0,0,0,0,  // b4-bf

  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // c0-cf
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // d0-df

  "not",                // e0
  "blank",              // e1
  "alpha",              // e2
  "match",              // e3
  "comment",            // e4
  0,0,0,                // e5-e7
  0,0,0,0, 0,0,0,0,     // e8-ef

  "if",                 // f0
  "else",               // f1
  "end",                // f2
  "while",              // f3
  "domacro",            // f4
  0,0,0,                // f5-f7
  0,0,0,0,              // f8-fb
  0,0,0,                // fc-fe
  "ShowDebug"           // ff
  };
//}}}
//{{{
const char* cKeyMap::kStateNames [eNumberOfStates] = {
  "", "Shift ", "Ctrl ", "Ctrl Shift ",
  "Alt ", "Alt Shift ", "Alt Ctrl ", "Alt Ctrl Shift ",
  "Gold "
  };
//}}}

//{{{
void cKeyMap::Terminate() {

  if (active_keyLut)
    delete active_keyLut;
  active_keyLut = 0;
  }
//}}}

//{{{
int cKeyMap::VirtualKey (int key, int state, int scancode) {
  if (state >= eGold)
    state = eGold;

  if ((key == VK_NUMLOCK) && (state >= eGold))
    return eGold * 128;    // Gold Gold

  if ((key >= 0x3a) && (key <= 0x40))  // should be unassigned !
    return -1; //0;
  if ((key >= 0x5b) && (key <= 0x5f))  // WIN keys & unassigned !
    return -1; //0;

  if (((key >= 0xba) && (key <= 0xc0)) || ((key >= 0xdb) && (key <= 0xdf)))
    // remap OEM keys
    key &= 0x7f;

  if ((state <= eShift) && (key > VK_HELP) && (key < VK_NUMPAD0))
    return -2;   // allow WM_CHAR mapping
  if (((state == eNormal) || (state == eShift)) && (key == VK_SPACE))
    return -2;   // allow WM_CHAR mapping

  if ((scancode & 0x0100) == 0) {
    switch (key) {
      case VK_INSERT: key = VK_NUMPAD0; break;
      case VK_END:    key = VK_NUMPAD1; break;
      case VK_DOWN:   key = VK_NUMPAD2; break;
      case VK_NEXT:   key = VK_NUMPAD3; break;
      case VK_LEFT:   key = VK_NUMPAD4; break;
      case VK_CLEAR:  key = VK_NUMPAD5; break;
      case VK_RIGHT:  key = VK_NUMPAD6; break;
      case VK_HOME:   key = VK_NUMPAD7; break;
      case VK_UP:     key = VK_NUMPAD8; break;
      case VK_PRIOR:  key = VK_NUMPAD9; break;
      case VK_DELETE: key = VK_DECIMAL; break;
      }
    }

  return state * 128 + key;
  }
//}}}
//{{{
int cKeyMap::MapVirtualKey (int key, int state, int scancode) {
  if ((key == VK_NUMLOCK) && (state >= eGold))
    return MapKey (0, state);     // Gold Gold

  if (state >= eGold)
    state = eGold;

  if ((key >= 0x3a) && (key <= 0x40))  // should be unassigned !
    return -1; //0;
  if ((key >= 0x5b) && (key <= 0x5f))  // WIN keys & unassigned !
    return -1; //0;

  if (((key >= 0xba) && (key <= 0xc0)) || ((key >= 0xdb) && (key <= 0xdf)))
    // remap OEM keys
    key &= 0x7f;

  if ((state <= eShift) && (key > VK_HELP) && (key < VK_NUMPAD0))
    return -2;   // allow WM_CHAR mapping
  if (((state == eNormal) || (state == eShift)) && (key == VK_SPACE))
    return -2;   // allow WM_CHAR mapping

  if ((scancode & 0x0100) == 0) {
    switch (key) {
      case VK_INSERT: key = VK_NUMPAD0; break;
      case VK_END:    key = VK_NUMPAD1; break;
      case VK_DOWN:   key = VK_NUMPAD2; break;
      case VK_NEXT:   key = VK_NUMPAD3; break;
      case VK_LEFT:   key = VK_NUMPAD4; break;
      case VK_CLEAR:  key = VK_NUMPAD5; break;
      case VK_RIGHT:  key = VK_NUMPAD6; break;
      case VK_HOME:   key = VK_NUMPAD7; break;
      case VK_UP:     key = VK_NUMPAD8; break;
      case VK_PRIOR:  key = VK_NUMPAD9; break;
      case VK_DELETE: key = VK_DECIMAL; break;
      }
    }

  return MapKey (key, state);
  }
//}}}
//{{{
const char* cKeyMap::VirtualKeyName (int key, int state, int scancode) {
  if ((key == VK_NUMLOCK) && (state >= eGold))
    return kStateNames [eGold];     // Gold Gold

  if (state > eGold)
    state = eGold;

  //if ((key >= 0) && (key <= 127))
  //  return kKeyNames [key];

  if ((key >= 0x3a) && (key <= 0x40))  // should be unassigned !
    return "3a-40";
  if ((key >= 0x5b) && (key <= 0x5f))  // WIN keys & unassigned !
    return "5b-5f";

  if (((key >= 0xba) && (key <= 0xc0)) || ((key >= 0xdb) && (key <= 0xdf)))
    // remap OEM keys
    key &= 0x7f;

  if ((state <= eShift) && (key > VK_HELP) && (key < VK_NUMPAD0))
    return 0;   // allow WM_CHAR mapping
  if ((state == eNormal) && (key == VK_SPACE))
    return 0;   // allow WM_CHAR mapping

  if ((key >= VK_PRIOR) && (key <= VK_DELETE) && ((scancode & 0x0100) == 0)) {
    switch (key) {
      case VK_INSERT: key = VK_NUMPAD0; break;
      case VK_END:    key = VK_NUMPAD1; break;
      case VK_DOWN:   key = VK_NUMPAD2; break;
      case VK_NEXT:   key = VK_NUMPAD3; break;
      case VK_LEFT:   key = VK_NUMPAD4; break;
      case VK_CLEAR:  key = VK_NUMPAD5; break;
      case VK_RIGHT:  key = VK_NUMPAD6; break;
      case VK_HOME:   key = VK_NUMPAD7; break;
      case VK_UP:     key = VK_NUMPAD8; break;
      case VK_PRIOR:  key = VK_NUMPAD9; break;
      }
    }

  if ((key < 0) || (key > 0x7f))
    return 0;    // outside of lookup table

  return kKeyNames [key];
  }
//}}}

//{{{
void cKeyMap::SetKey (int key, int token) {

  if ((key >= 0) && (key < eNumberOfStates*128) && (token >= 0) && (token <= 255)) {
    if (!active_keyLut) {
      active_keyLut = new cKeyLut;
      if (active_keyLut)
        active_keyLut->Initialize (default_keyLut);
      }
    if (active_keyLut)
      active_keyLut->SetKey (key, token);
    }
  }
//}}}
//{{{
void cKeyMap::PutKeyMap (cKeyLut& keyLut) {

  if (!active_keyLut)
    active_keyLut = new cKeyLut;
  if (active_keyLut)
    active_keyLut->Initialize (keyLut);
  }
//}}}
//{{{
void cKeyMap::GetKeyMap (cKeyLut& keyLut) {

  if (active_keyLut)
    keyLut.Initialize (*active_keyLut);
  else
    keyLut.Initialize (default_keyLut);
  }
//}}}
//{{{
void cKeyMap::GetDefaultKeyMap (cKeyLut& keyLut) {

  keyLut.Initialize (default_keyLut);
  }
//}}}

//{{{
int cKeyMap::MapKeyDefault (int key) {

  return default_keyLut.MapKey (key);
  }
//}}}
//{{{
int cKeyMap::MapKeyDefault (int key, int state) {

  return default_keyLut.MapKey (key, state);
  }
//}}}
//{{{
int cKeyMap::MapKey (int key) {

  return active_keyLut ? active_keyLut->MapKey (key) : default_keyLut.MapKey (key);
  }
//}}}
//{{{
int cKeyMap::MapKey (int key, int state) {

  return active_keyLut ? active_keyLut->MapKey (key, state) : default_keyLut.MapKey (key, state);
  }
//}}}
//{{{
int cKeyMap::FindKey (int token, int start_key) {

  return active_keyLut ? active_keyLut->FindKey (token, start_key) : default_keyLut.FindKey (token, start_key);
  }
//}}}

//{{{
const char* cKeyMap::KeyName (int key) {

  if ((key >= 0) && (key <= 127))
    return kKeyNames [key];

  return 0;
  }
//}}}
//{{{
const char* cKeyMap::TokenName (int token) {

  if ((token >= 0) && (token <= 255))
    return kTokenNames [token];

  return 0;
  }
//}}}
//{{{
const char* cKeyMap::StateName (int state) {

  if ((state >= 0) && (state < eNumberOfStates))
    return kStateNames [state];

  return 0;
  }
//}}}

//{{{
int cKeyMap::FindToken (const char* name) {

  int best_match = -1;
  if (name && *name) {
    unsigned int name_len = (int)strlen(name);
    for (int n = 0; n < 256; n++) {
      if (kTokenNames[n]) {
        if (_stricmp (name, kTokenNames[n]) == 0)
          return n;
        if ((strlen (kTokenNames[n]) > name_len)
           && (_memicmp (name, kTokenNames[n], name_len) == 0)) {
          if (best_match >= 0)
            return -1;
          best_match = n;
          }
        }
      }
    }

  return best_match;
  }
//}}}
//{{{
int cKeyMap::FindKey (const char* name) {

  if (name && *name) {
    int name_len = (int)strlen(name);
    int state = eNumberOfStates - 1;
    while (state > 0) {
      int len = (int)strlen(kStateNames[state]);
      if ((name_len > len) && (_memicmp (name, kStateNames[state], len) == 0)) {
        name += len;
        break;
        }
      state--;
      }

    int key;
    for (key = 0; key < 128; key++) {
      if (kKeyNames[key] && (_stricmp (name, kKeyNames[key]) == 0))
        return key + state * 128;
      }
    }

  return -1;
  }
//}}}
//{{{
int cKeyMap::MatchToken (const char* starting, int& match_len) {
// finds a token which starts with the 'starting' string
// returns the length of the matching string or, if ambiguous, the length
// of the common part
// and sets 'starting' to point to the matching keyword

  match_len = 0;
  int match_num = -1;
  const char* match = 0;

  if (starting && *starting) {
    int len = (int)strlen(starting);

    for (int n = 0; n < 256; n++) {
      const char* token = kTokenNames[n];
      if (token) {
        int tokenlen = (int)strlen(token);
        if ((tokenlen >= len) && (_memicmp (token, starting, len) == 0)) {
          if (match_num >= 0) {  // ambiguous
            const char* s = kTokenNames[match_num];
            int new_len = 0;
            while ((*token > ' ') && (*token == *s)) { // skip past matching part
              token++;
              s++;
              new_len++;
              }
            if (new_len < match_len)
              match_len = new_len;
            if (_stricmp (token, kTokenNames[match_num]) < 0)
              match_num = n;  // first in alphabetical order
            }
          else {        // unique - so far
            match_num = n;
            match_len = tokenlen;
            match = token;
            }
          }
        }
      }
    }

  return match_num;
  }
//}}}

//{{{
bool cKeyMap::LoadFromRegistry() {

  if (!active_keyLut)
    active_keyLut = new cKeyLut;
  if (!active_keyLut)
    return false;

  active_keyLut->Initialize (default_keyLut);
  bool changed = false;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  HKEY hKey = pApp->GetSectionKey ("KeyMap");

  if (hKey != NULL) {
    DWORD index = 0;
    char keyname [256];
    DWORD keyname_len = 256;
    unsigned char tokenname [256];
    DWORD tokenname_len = 256;

    while (::RegEnumValue (hKey, index++, keyname, &keyname_len, 0, 0, tokenname, &tokenname_len) == ERROR_SUCCESS) {
      keyname_len = 256;
      tokenname_len = 256;
      changed = true;
      int key = FindKey (keyname);
      int token = FindToken ((char*)tokenname);
      active_keyLut->SetKey (key, token);
      }
    ::RegCloseKey(hKey);
    }

  if (!changed) {
    delete active_keyLut;
    active_keyLut = 0;
    }

  return changed;
  }
//}}}
//{{{
bool cKeyMap::SaveToRegistry() {

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  HKEY hKey = pApp->GetAppRegistryKey();
  if (hKey != NULL) {
    pApp->DelRegTree (hKey, "KeyMap");
    ::RegCloseKey(hKey);
    }

  if (!active_keyLut)
    return false;
  bool differ = false;
  CString keyname;

  for (int key = 0; key < cKeyLut::eMaxKey; key++) {
    int token = active_keyLut->MapKey(key);
    if (token != default_keyLut.MapKey(key)) {
      keyname = StateName (key / 128);
      keyname += KeyName (key % 128);
      const char* tokenname = TokenName (token);
      if (!keyname.IsEmpty() && tokenname && *tokenname) {
        pApp->WriteProfileString ("KeyMap", keyname, tokenname);
        differ = true;
        }
      }
    }

  return differ;
  }
//}}}

//{{{
bool cKeyMap::SaveToFile (FILE* fp) {

  bool differ = false;
  if (fp) {
    fprintf (fp, "[KeyMap]\n");
    if (active_keyLut) {
      CString keyname;

      for (int key = 0; key < cKeyLut::eMaxKey; key++) {
        int token = active_keyLut->MapKey(key);
        if (token != default_keyLut.MapKey(key)) {
          keyname = StateName (key / 128);
          keyname += KeyName (key % 128);
          const char* tokenname = TokenName (token);
          if (!keyname.IsEmpty() && tokenname && *tokenname) {
            fprintf (fp, "%s=%s\n", (LPCTSTR)keyname, tokenname);
            differ = true;
            }
          }
        }
      }
    }

  return differ;
  }
//}}}
//{{{
void cKeyMap::LoadFromFile (FILE* fp) {

  if (!fp)
    return;
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
    else {
      char* keyword = cur;
      cur = strchr (keyword, '=');
      if (cur) {
        char* keyend = cur;
        cur = keyend + 1;
        while (cur && (*cur <= ' ')) cur++;
        while ((keyend > keyword) && (*(keyend - 1) <= ' '))
          keyend--;
        *keyend = 0;
        int key = FindKey (keyword);
        if (key >= 0) {
          int token = FindToken (cur);
          if (token >= 0)
            SetKey (key, token);
          else {
            CString msg;
            msg.Format ("Unknown command '%s' in KeyMap definition", cur);
            AfxMessageBox (msg, MB_OK);
            return;
            }
          }
        else {
          CString msg;
          msg.Format ("Unknown key '%s' in KeyMap definition", keyword);
          AfxMessageBox (msg, MB_OK);
          return;
          }
        }
      }
    }
  }
//}}}

// struct cKeyLut
//{{{
void cKeyLut::Clear() {

  memset (m_keyLut, 0, eMaxKey);
  }
//}}}
//{{{
void cKeyLut::Initialize (const cKeyLut& keyLut) {

  memcpy (m_keyLut, keyLut.m_keyLut, eMaxKey);
  }
//}}}

//{{{
void cKeyLut::SetKey (int key, int token) {

  if ((key >= 0) && (key < eMaxKey) && (token >= 0) && (token <= 255))
    m_keyLut [key] = token;
  }
//}}}
//{{{
int cKeyLut::MapKey (int key) const {

  if ((key >= 0) && (key < eMaxKey))
    return m_keyLut [key];

  return 0;
  }
//}}}
//{{{
int cKeyLut::MapKey (int key, int state) const {

  if (state >= eGold)
    state = eGold;

  if ((key >= 0) && (key <= 127) && (state >= 0) && (state < eNumberOfStates))
    return m_keyLut [key + state * 128];

  return 0;
  }
//}}}
//{{{
int cKeyLut::FindKey (int token, int start_key) const {

  if ((start_key >= 0) && (start_key < eMaxKey)) {
    for (int key = start_key; key < eMaxKey; key++) {
      if (m_keyLut [key] == token)
        return key;
      }
    }

  return -1;
  }
//}}}
