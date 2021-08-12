// DebugWin.cpp
//{{{  includes
#include "pch.h"
#include "DebugWin.h"

#include "colour.h"
#include "TextLine.h"
#include "TextFold.h"
#include "TextFile.h"
#include "Fed.h"
#include "FedDoc.h"
#include "FedView.h"
//}}}

//{{{
cDebugWin::cDebugWin (cFedView* view)
    : m_page (1), m_cChar (8, 8), m_xStart (0), curView (view) {

  for (int n = 0; n < maxMsg; n++)
    messages [n] = 0;
  }
//}}}
//{{{
cDebugWin::~cDebugWin() {

  for (int n = 0; n < maxMsg; n++)
    if (messages [n])
      delete[] messages [n];
  }
//}}}
//{{{
BOOL cDebugWin::PreCreateWindow (CREATESTRUCT& cs) {

  cs.style |= WS_VSCROLL;

  return CWnd::PreCreateWindow(cs);
  }
//}}}
//{{{
int cDebugWin::OnCreate (LPCREATESTRUCT lpCreateStruct) {

  if (CWnd::OnCreate (lpCreateStruct) == -1)
    return -1;

  SetName();

  return 0;
  }
//}}}

//{{{
BEGIN_MESSAGE_MAP(cDebugWin, CWnd)
  ON_WM_PAINT()
  ON_WM_DESTROY()
  ON_WM_QUERYENDSESSION()
  ON_WM_CREATE()
END_MESSAGE_MAP()
//}}}

//{{{
void cDebugWin::PaintPos (CDC& dc, int& x, int& y) {

  CPoint curPos = curView->m_pos;
  CPoint curScroll = curView->GetScrollPosition();

  //  print View scroll and cursor pos and Match pos
  char str [100];
  sprintf (str, "View scroll: %d,%d  Cursor pos: %d,%d  Palette: %d",
           curScroll.x, curScroll.y, curPos.x, curPos.y, cColour::Palette() ? 1 : 0 );
  dc.TextOut (x, y, str);
  y += m_cChar.cy;

  sprintf (str, "Margin: %d,%d  CurMargin: %d,%d & %d,%d",
           curView->m_ScrollMargin.cx,
           curView->m_ScrollMargin.cy,
           curView->m_CurTopLeftMargin.cx,
           curView->m_CurTopLeftMargin.cy,
           curView->m_CurBottomRightMargin.cx,
           curView->m_CurBottomRightMargin.cy);
  if (curView->m_displayMatch)
    sprintf (strchr (str, 0), "  Match pos: %d %d",
           curView->m_matchPos.x, curView->m_matchPos.y);

  if (curView->m_selectActive) {
    sprintf (strchr (str, 0), "  Select: %d %d to %d %d",
           curView->m_selectRange.FirstXpos(), curView->m_selectRange.FirstYpos(),
           curView->m_selectRange.LastXpos(), curView->m_selectRange.LastYpos());
    }

  dc.TextOut (x, y, str);
  y += m_cChar.cy;
  y += m_cChar.cy;
  }
//}}}
//{{{
void cDebugWin::PaintLine (CDC& dc, int& x, int& y, cTextLine* line, const char* msg) {

  char* str = new char [5000];
  sprintf (str, "%s= %p parent: %p", msg, line, line?line->ParentFold():0);
  dc.TextOut (x, y, str);
  y += m_cChar.cy;
  if (line) {
    const char* strend = line->show_debug (str);
    const char* s = str;
    while (s < strend) {
      dc.TextOut (x, y, s);
      s = strchr (s, 0);
      s++;
      y += m_cChar.cy;
      }
    line->show_format (str);
    dc.TextOut (x, y, str);
    y += m_cChar.cy;
    }

  y += m_cChar.cy;
  delete[] str;
  }
//}}}
//{{{
void cDebugWin::PaintLanguage (CDC& dc, int& x, int& y) {

  char str [500];
  cTextFile* curFile = curView->m_curFile;
  cLanguage* language = curFile ? curFile->Language() : 0;
  sprintf (str, "language=");

  dc.TextOut (x, y, str);
  if (language && language->Name()) {
    int px = x + dc.GetTextExtent(str).cx;
    dc.TextOut (px, y, language->Name());
    }

  y += m_cChar.cy;
  if (language ) {
    if (language->Extensions()) {
      dc.TextOut (x, y, language->Extensions());
      y += m_cChar.cy;
      }
    if (language->CommentStart()) {
      dc.TextOut (x, y, language->CommentStart());
      int px = x + dc.GetTextExtent(language->CommentStart()).cx + m_cChar.cx;
      if (language->CommentEnd())
        dc.TextOut (x, y, language->CommentEnd());
      y += m_cChar.cy;
      }
    }

  y += m_cChar.cy;
  }
//}}}

//{{{
void cDebugWin::PaintPage1 (CDC& dc) {

  int x = m_xStart;
  int y = 0;

  int view = curView->m_view;
  cTextFile* curFile = curView->m_curFile;
  cTextFold* curTop = curView->m_curTop;
  CPoint curPos = curView->m_pos;

  PaintPos (dc, x, y);
  PaintLine (dc, x, y, curFile, "curFile");
  PaintLine (dc, x, y, curTop, "curTop");

  if (curTop) {
    cTextLine* line = curTop->GetLine(view, curPos.y);
    PaintLine (dc, x, y, line, "cur");
    }
  }
//}}}
//{{{
void cDebugWin::PaintPage2 (CDC& dc) {

  int x = m_xStart;;
  int y = 0;

  int view = curView->m_view;
  cTextFold* curTop = curView->m_curTop;
  CPoint curPos = curView->m_pos;

  PaintPos (dc, x, y);

  if (curTop) {
    cTextLine* line = curTop->GetLine(view, curPos.y);
    PaintLine (dc, x, y, line?line->Prev():0, "prev");
    PaintLine (dc, x, y, line, "cur");
    PaintLine (dc, x, y, line?line->Next():0, "next");
    }
  }
//}}}
//{{{
void cDebugWin::PaintPage3 (CDC& dc) {

  int x = m_xStart;;
  int y = 0;

  int view = curView->m_view;
  cTextFold* curTop = curView->m_curTop;
  CPoint curPos = curView->m_pos;

  PaintPos (dc, x, y);

  if (curTop) {
    cTextLine* line = curTop->GetLine(view, curPos.y);
    PaintLine (dc, x, y, line, "cur");
    }

  PaintLine (dc, x, y, curView->GetDocument()->GetMoveLine(), "moveLine");
  PaintLine (dc, x, y, curView->GetDocument()->GetDeletedLine(), "deletedLine");
  PaintLanguage (dc, x, y);
  }
//}}}
//{{{
void cDebugWin::PaintPage4 (CDC& dc)
{
  int x = m_xStart;;
  int y = 0;

  for (int n = 0; n < maxMsg; n++) {
    if (messages [n])
      dc.TextOut (x, y, messages [n]);
    y += m_cChar.cy;
    }

  int view = curView->m_view;
  cTextFold* curTop = curView->m_curTop;
  CPoint curPos = curView->m_pos;

  PaintPos (dc, x, y);

  if (curTop) {
    int n = 5;
    cTextLine* line = curTop->GetLine(view, curPos.y);
    PaintLine (dc, x, y, line, "cur");
    while (line && (--n > 0))
      {
      line = line->ParentFold();
      PaintLine (dc, x, y, line, "parent");
      }
    }
}
//}}}
//{{{
void cDebugWin::PaintColourPage (CDC& dc) {

  int x = m_xStart;
  int y = 0;
  int col;
  int maxlen = 0;
  for (col = c_Text; col < c_Last; col++) {
    const char* name = cColour::ColourName (col);
    if (name) {
      int len = (int)strlen(name);
      if (len > maxlen)
        maxlen = len;
      }
    }
  int xtab1 = x + (maxlen + 2) * m_cChar.cx;
  int xtab2 = xtab1 + 20 * m_cChar.cx;

  bool hasPalette = cColour::Palette() ? true : false;

  if (hasPalette)
    dc.TextOut (x, y, "Colours:   Palette: Yes");
  else
    dc.TextOut (x, y, "Colours:   Palette: No");
  y += m_cChar.cy;

  dc.TextOut (x, y, "Name");
  dc.TextOut (xtab1, y, "Foreground");
  dc.TextOut (xtab2, y, "Background");
  y += m_cChar.cy;

  char str [100];
  dc.TextOut (xtab1, y, "  R   G   B  Index");
  dc.TextOut (xtab2, y, "  R   G   B  Index");
  y += m_cChar.cy;

  for (col = c_Text; col < c_Last; col++) {
    const char* name = cColour::ColourName (col);
    int fgnd_index = cColour::GetColour (col) & 0xffffff;
    int bgnd_index = cColour::GetBkColour (col) & 0xffffff;
    COLORREF fgnd = cColour::GetRgbColour (col);
    COLORREF bgnd = cColour::GetRgbBkColour (col);
    if (name)
      dc.TextOut (x, y, name);

    int r = GetRValue (fgnd);
    int g = GetGValue (fgnd);
    int b = GetBValue (fgnd);
    if (hasPalette)
      sprintf (str, "%3d %3d %3d  %3d", r, g, b, fgnd_index);
    else
      sprintf (str, "%3d %3d %3d", r, g, b);
    dc.TextOut (xtab1, y, str);

    r = GetRValue (bgnd);
    g = GetGValue (bgnd);
    b = GetBValue (bgnd);
    if (hasPalette)
      sprintf (str, "%3d %3d %3d  %3d", r, g, b, bgnd_index);
    else
      sprintf (str, "%3d %3d %3d", r, g, b);
    dc.TextOut (xtab2, y, str);

    y += m_cChar.cy;
    }
  }
//}}}
//{{{
void cDebugWin::PaintUndoPage (CDC& dc) {

  int x = m_xStart;
  int y = 0;
  int view = curView->m_view;
  cTextFile* curFile = curView->m_curFile;
  cTextFold* curTop = curView->m_curTop;
  CPoint curPos = curView->m_pos;

  char str [100];
  sprintf (str, "Undo List for file %s", curFile->Pathname());
  dc.TextOut (x, y, str);
  y += m_cChar.cy * 3;

  cUndo* undo = curFile->UndoList();
  for (int list = 1; list <= 2; list++) {
    while (undo) {
      if (undo->Column()) {
        if (undo->Text())
          sprintf (str, "Column: %d,%d-%d,%d  Text: '%s'", undo->FirstXpos(), undo->FirstYpos(),
                   undo->LastXpos(), undo->LastYpos(), undo->Text());
        else
          sprintf (str, "Column: %d,%d-%d,%d", undo->FirstXpos(), undo->FirstYpos(),
                   undo->LastXpos(), undo->LastYpos());
        }
      else if (undo->isSinglePoint()) {
        if (undo->Text())
          sprintf (str, "Pos: %d,%d  Text: '%s'", undo->Xpos(), undo->Ypos(), undo->Text());
        else
          sprintf (str, "Pos: %d,%d", undo->Xpos(), undo->Ypos());
        }
      else {
        if (undo->Text())
          sprintf (str, "Range: %d,%d-%d,%d  Text: '%s'", undo->FirstXpos(), undo->FirstYpos(),
                   undo->LastXpos(), undo->LastYpos(), undo->Text());
        else
          sprintf (str, "Range: %d,%d-%d,%d", undo->FirstXpos(), undo->FirstYpos(),
                   undo->LastXpos(), undo->LastYpos());
        }
      dc.TextOut (x, y, str);
      y += m_cChar.cy;
      undo = undo->Prev();
      }

    if (list == 1) {
      undo = curFile->RedoList();
      if (undo) {
        y += m_cChar.cy;
        sprintf (str, "Redo List:");
        dc.TextOut (x, y, str);
        y += m_cChar.cy * 2;
        }
      }
    }
  }
//}}}

//{{{
void cDebugWin::SetName() {

  CString str;
  str.Format ("Fed Debug %d", m_page);
  SetWindowText (str);
  }
//}}}
//{{{
void cDebugWin::NextPage() {

  m_page = (m_page + 1) % (numberOfPages + 1);
  SetName();
  Invalidate();
  }
//}}}
//{{{
void cDebugWin::OnPaint() {

  CPaintDC dc (this);

  if (cColour::Palette()) {
    dc.SelectPalette (cColour::Palette(), TRUE);
    dc.RealizePalette();
    }

  CRect rect;
  dc.GetClipBox (&rect);
  dc.FillSolidRect (&rect, cColour::GetBkColour (c_Text));

  dc.SelectStockObject (SYSTEM_FIXED_FONT);
  TEXTMETRIC tm;
  dc.GetTextMetrics (&tm);
  m_cChar.cx = tm.tmAveCharWidth;
  m_cChar.cy = tm.tmHeight;
  m_xStart = m_cChar.cx / 2;

  dc.SetTextColor (cColour::GetColour (c_Text));
  dc.SetBkColor (cColour::GetBkColour (c_Text));

  if (!curView) {
    dc.TextOut (m_xStart, 0, "No View window set");
    return;
    }

  switch (m_page) {
    case 0:   PaintColourPage (dc); break;
    case 1:   PaintPage1 (dc);      break;
    case 2:   PaintPage2 (dc);      break;
    case 3:   PaintPage3 (dc);      break;
    case 4:   PaintPage4 (dc);      break;
    case 5:   PaintUndoPage (dc);   break;
    default:  PaintPage1 (dc);      break;
    }

  }
//}}}
//{{{
void cDebugWin::Update (cFedView* view) {

  curView = view;
  Invalidate();
  }
//}}}
//{{{
void cDebugWin::Message (int msg_num, const char* msg) {

  HideCaret();
  CClientDC dc (this);
  if (cColour::Palette()) {
    dc.SelectPalette (cColour::Palette(), TRUE);
    dc.RealizePalette();
    }
  dc.SelectStockObject (SYSTEM_FIXED_FONT);

  TEXTMETRIC tm;
  dc.GetTextMetrics (&tm);
  CSize m_cChar;
  m_cChar.cx = tm.tmAveCharWidth;
  m_cChar.cy = tm.tmHeight;
  int m_xStart = m_cChar.cx / 2;
  int x = m_xStart;
  int y = 0;

  RECT r;
  GetClientRect (&r);

  if (msg_num < 0) {
    msg_num = -msg_num;
    for (int n = 0; n < maxMsg; n++) {
      if (n != msg_num) {
        if (messages [n])
          delete[] messages [n];
        messages [n] = 0;
        y = n * m_cChar.cy;
        dc.FillSolidRect (0, y, r.right, m_cChar.cy, cColour::GetBkColour (c_Text));
        }
      }
    }
  else if (msg_num < maxMsg) {
    dc.SetTextColor (cColour::GetColour (c_Text));
    dc.SetBkColor (cColour::GetBkColour (c_Text));
    y = msg_num * m_cChar.cy;
    dc.FillSolidRect (0, y, r.right, m_cChar.cy, cColour::GetBkColour (c_Text));

    if (messages [msg_num])
      delete[] messages [msg_num];
    messages [msg_num] = 0;
    if (msg && *msg) {
      messages [msg_num] = new char [strlen (msg) + 1];
      strcpy (messages [msg_num], msg);
      dc.TextOut (x, y, messages [msg_num]);
      }
    }

  ShowCaret();
  }
//}}}
//{{{
void cDebugWin::OnDestroy() {

  CWnd::OnDestroy();

  if (curView)
    curView->OnDestroyDebugwin();
  }
//}}}
