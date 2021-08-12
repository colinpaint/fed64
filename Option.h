#pragma once

class cOption {
public:
  // editor page
  static BOOL CheckFileIsNewer;
  static int  FileNewerDelta;
  static BOOL ProtectReadOnly;
  static BOOL AllowBackspAtSol;
  static BOOL AllowLeftAtSol;
  static BOOL SetCursorAtEndOfPaste;

  static BOOL CreateBackupFirstSave;
  static BOOL CreateBackupEverySave;
  static BOOL SuppressBeeps;
  static BOOL EnableUndo;

  static bool LoadStartOptionsFromRegistry();
  static bool LoadFromRegistry();
  static bool SaveToRegistry();

  static bool UpdateRegistry_Display (bool save);
  static bool UpdateRegistry_Editor (bool save);

  static BOOL SingleInstance;
  static BOOL ShowEditMarks;
  static BOOL ShowLeftMargin;
  static int  LineWrapLength;
  static int  LineWrapShade;
  static BOOL ShowWrapMark;
  static BOOL SaveWindowPlacement;
  static BOOL ShowClosedFoldComments;
  static BOOL ShowDebug;
  static BOOL SaveOptions;
  };
