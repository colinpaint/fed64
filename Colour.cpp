// Colour.cpp
//{{{  includes
#include "pch.h"
#include "Colour.h"
//}}}

CPalette cColour::palette;
DWORD cColour::fcol_lut [c_Last];
DWORD cColour::bcol_lut [c_Last];
DWORD cColour::fcol_palette_lut [c_Last];
DWORD cColour::bcol_palette_lut [c_Last];
bool cColour::usePalette = false;

//{{{
void cColour::InitColours() {

  //  foreground
  fcol_lut [c_Text]          = RGB (  0,   0,   0);
  fcol_lut [c_FoldOpen]      = RGB (255,   0,   0);
  fcol_lut [c_FoldClosed]    = RGB (  0,   0, 255);
  fcol_lut [c_FoldCreate]    = RGB (255,   0,   0);
  fcol_lut [c_FoldCopied]    = RGB (  0,   0, 205);
  fcol_lut [c_File]          = RGB (  0,   0,   0);
  fcol_lut [c_ProtectedFile] = RGB (200,   0,   0);
  fcol_lut [c_Proj]          = RGB (  0,   0,   0);
  fcol_lut [c_TopProj]       = RGB (  0,   0,   0);
  fcol_lut [c_Select]        = RGB (255, 255, 255);
  fcol_lut [c_HighLight]     = RGB (  0,   0,   0);
  fcol_lut [c_CommentText]   = RGB ( 34, 139,  34);
  fcol_lut [c_StringText]    = RGB (128,   0, 128);
  fcol_lut [c_StringError]   = RGB (255,   0,   0);
  fcol_lut [c_Keyword]       = RGB (128,   0,   0);
  fcol_lut [c_TaggedText]    = RGB (200,   0,   0);

  //  background
  bcol_lut [c_Text]          = RGB (255, 255, 255);
  bcol_lut [c_FoldOpen]      = RGB (255, 255, 255);
  bcol_lut [c_FoldClosed]    = RGB (255, 255, 255);
  bcol_lut [c_FoldCreate]    = RGB (255, 255, 255);
  bcol_lut [c_FoldCopied]    = RGB (255, 255, 255);
  bcol_lut [c_File]          = RGB (218, 165,  32);
  bcol_lut [c_ProtectedFile] = RGB (218, 165,  32);
  bcol_lut [c_Proj]          = RGB (255, 255, 220);
  bcol_lut [c_TopProj]       = RGB (255, 210, 128);
  bcol_lut [c_Select]        = RGB (165,  42,  42);
  bcol_lut [c_HighLight]     = RGB (  0, 255,   0);
  bcol_lut [c_CommentText]   = RGB (255, 255, 255);
  bcol_lut [c_StringText]    = RGB (255, 255, 255);
  bcol_lut [c_StringError]   = RGB (255, 255, 255);
  bcol_lut [c_Keyword]       = RGB (255, 255, 255);
  bcol_lut [c_TaggedText]    = RGB (255, 255, 255);

  InitPalette();
  }
//}}}
//{{{
void cColour::InitPalette() {

  CClientDC dc(0);
  if (dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE) {
    int numColours = 0;
    for (int ncol = 1; ncol < c_Last; ncol++) {
      if (findColour (fcol_lut [ncol]) == ncol)
        numColours++;
      if (findColour (bcol_lut [ncol]) == ncol)
        numColours++;
      }

    LOGPALETTE* logpal = (LOGPALETTE*) new char [sizeof (LOGPALETTE) + sizeof (PALETTEENTRY) * numColours];
    if (logpal) {
      logpal->palVersion = 0x300;
      logpal->palNumEntries = (WORD) numColours;

      int entry = 0;
      for (int n = 1; n < c_Last; n++) {
        DWORD col = fcol_lut [n];
        for (int fb = 0; (entry < numColours) && (fb <= 1); fb++) {
          if (findColour (col) == n) {
            logpal->palPalEntry[entry].peRed   = (BYTE) (col & 0xff);
            logpal->palPalEntry[entry].peGreen = (BYTE) ((col >> 8) & 0xff);
            logpal->palPalEntry[entry].peBlue  = (BYTE) ((col >> 16) & 0xff);
            logpal->palPalEntry[entry].peFlags = 0; //PC_RESERVED;
            entry++;
            }
          col = bcol_lut [n];
          }
        }
      palette.CreatePalette (logpal);
      delete logpal;
      usePalette = true;

      // now convert colours to palette indices
      for (int i = 1; i < c_Last; i++) {
        fcol_palette_lut [i] = PALETTEINDEX (palette.GetNearestPaletteIndex (fcol_lut [i]));
        bcol_palette_lut [i] = PALETTEINDEX (palette.GetNearestPaletteIndex (bcol_lut [i]));
        }
      }
    }
  }
//}}}
//{{{
int cColour::findColour (DWORD col) {

  for (int n = 1; n < c_Last; n++)
    if ((col == fcol_lut [n]) || (col == bcol_lut [n]))
      return n;

  return 0;
  }
//}}}

//{{{
const char* cColour::kColourNames [c_Last] = {
  "None", "Text",
  "FoldOpen", "FoldClosed", "FoldCreate", "FoldCopied",
  "File", "ProtectedFile",
  "Proj", "TopProj",
  "Select", "HighLight",
  "CommentText", "StringText", "StringError", "Keyword",
  "TaggedText"
  };
//}}}
//{{{
const char* cColour::ColourName (int col) {

  if ((col >= 0) && (col < c_Last))
    return kColourNames [col];

  return 0;
  }
//}}}

//{{{
DWORD cColour::GetColour (int col) {

  if ((col <= c_Null) || (col >= c_Last))
    col = c_Text;

  return usePalette ? fcol_palette_lut [col] : fcol_lut [col];
  }
//}}}
//{{{
DWORD cColour::GetBkColour (int col, int shade) {

  if ((col <= c_Null) || (col >= c_Last))
    col = c_Text;

  if (isDefaultBkColour (col)) {
    DWORD result = GetSysColor (COLOR_WINDOW);
    if (shade > 0) {
      int r = GetGValue (result);
      int g = GetGValue (result);
      int b = GetGValue (result);
      if ((r <= 255 - shade) && (g <= 255 - shade) && (b <= 255 - shade)) {
        r += shade;
        g += shade;
        b += shade;
        }
      else {
        r = max (r - shade, 0);
        g = max (g - shade, 0);
        b = max (b - shade, 0);
        }
      result = RGB (r, g, b);
      }

    if (usePalette)
      result = PALETTEINDEX (palette.GetNearestPaletteIndex (result));

    return result;
    }

  return usePalette ? bcol_palette_lut [col] : bcol_lut [col];
  }
//}}}
//{{{
bool cColour::isDefaultBkColour (int col) {

  if ((col <= c_Null) || (col >= c_Last))
    col = c_Text;

  return (bcol_lut [col] == RGB (255, 255, 255));
  }
//}}}

//{{{
DWORD cColour::GetRgbColour (int col) {

  if (!usePalette)
    return GetColour (col);

  if ((col <= c_Null) || (col >= c_Last))
    return 0;

  PALETTEENTRY entry;
  palette.GetPaletteEntries (fcol_palette_lut [col], 1, &entry);

  return RGB (entry.peRed, entry.peGreen, entry.peBlue);
  }
//}}}
//{{{
DWORD cColour::GetRgbBkColour (int col) {

  if (!usePalette)
    return GetBkColour (col);

  if ((col <= c_Null) || (col >= c_Last))
    return 0;

  PALETTEENTRY entry;
  palette.GetPaletteEntries (bcol_palette_lut [col], 1, &entry);

  return RGB (entry.peRed, entry.peGreen, entry.peBlue);
  }
//}}}
