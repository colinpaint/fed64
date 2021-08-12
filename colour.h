#pragma once

enum eScreenCol {
  c_Null, c_Text,
  c_FoldOpen, c_FoldClosed, c_FoldCreate, c_FoldCopied,
  c_File, c_ProtectedFile,
  c_Proj, c_TopProj,
  c_Select, c_HighLight,
  c_CommentText, c_StringText, c_StringError, c_Keyword,
  c_TaggedText,
  c_Last };

class cColour {
public:
  static const char* ColourName (int col);
  static void InitColours();
  static CPalette* Palette() { return usePalette ? &palette : 0; }

  static DWORD GetRgbColour (int col);
  static DWORD GetRgbBkColour (int col);

  static DWORD GetColour (int col);
  static DWORD GetBkColour (int col, int shade = 0);
  static bool isDefaultBkColour (int col);

private:
  static const char* kColourNames [c_Last];

  static DWORD fcol_lut [c_Last];
  static DWORD bcol_lut [c_Last];
  static DWORD fcol_palette_lut [c_Last];
  static DWORD bcol_palette_lut [c_Last];

  static CPalette palette;
  static bool usePalette;
  static int findColour (DWORD col);
  static void InitPalette();
  };
