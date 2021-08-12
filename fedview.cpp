// FedView.cpp
//{{{  includes
#include "pch.h"
#include "FedView.h"

#include "Fed.h"
#include "MainFrm.h"
#include "FedDoc.h"
#include "DebugWin.h"
#include "Option.h"
#include "KeyMap.h"
#include "TextProj.h"
#include "Dialog.h"
//}}}
//{{{  static strings
static UINT WM_FINDREPLACE = ::RegisterWindowMessage (FINDMSGSTRING);
static const char szFonts[]  = "Fonts";
static const char szName[]   = "Name";
static const char szHeight[] = "Height";

static const char szFileProtected[]    = "File is protected";
static const char szUnknownKeyword[]   = "Unknown keyword";
static const char szAmbiguousKeyword[] = "Ambiguous keyword";
//}}}
//{{{  static members
CFont* cFedView::m_font = 0;
CFont* cFedView::m_textFont = 0;
CFont* cFedView::m_boldFont = 0;
CFont* cFedView::m_italicFont = 0;

cDebugWin* cFedView::m_debugWin = 0;
//}}}

//{{{
cFedView::cFedView()
  : Font(0),

    m_overstrike (false),
    m_recording (false),
    m_macro_textmode (false),
    m_findDialog (NULL),
    m_findString (NULL),
    m_replaceString (NULL),
    m_findOnly (false),
    m_matchCase (false),
    m_matchWholeWord (false),
    m_ignoreComments (false),
    m_findUp (false),
    m_searchAll (false),
    m_atFindString (false),
    m_atReplaceString (false),
    m_searchingProject (NULL),
    m_scrollPos (0, 0),
    m_displayMatch (false),
    m_matchPos (0, 0),
    m_tabLen (4),
    m_Margin (4, 0),
    m_xStart (4),
    m_goldOn (false),
    m_mouseDown (false),
    m_range(),
    m_selectRange(),
    m_selectActive (false),
    m_selectWholeLines (false),
    m_selectColumn (false),
    m_selectPos (0, 0),
    m_pos (0, 0),
    m_lineNumber (0),
    m_cChar (8, 8),
    m_cWlen (50, 50),
    m_ScrollMargin (10, 10),
    m_CurTopLeftMargin (0, 0),
    m_CurBottomRightMargin (0, 0),
    m_KeyWasModifier (false),
    m_AllowChar (false),
    m_ShowLineNumbers (false),
    m_ShowFolds (false),
    m_ShowChanges (false),
    m_FoldendVisible (false),
    m_view (0),
    m_curFile (NULL),
    m_curTop (NULL)

{
  // setup CScrollView mapping default
  m_nMapMode = MM_TEXT;
}
//}}}
//{{{
cFedView::~cFedView()
{
  if (m_font)
    delete m_font;

  if (m_debugWin)
    delete m_debugWin;

  GetDocument()->ReleaseView (m_view);
}
//}}}
BOOL cFedView::PreCreateWindow (CREATESTRUCT& cs) { return CView::PreCreateWindow(cs); }

//{{{
void cFedView::SetFont (CFont* font) {

  if (m_font == font)
    return;

  if (m_font)
    delete m_font;
  m_font = 0;

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  LOGFONT logFont;
  if (font && font->GetLogFont (&logFont) && (_stricmp (logFont.lfFaceName, "Fixedsys") != 0)) {
    m_font = font;
    pApp->WriteProfileString (szFonts, szName, logFont.lfFaceName);
    pApp->WriteProfileInt    (szFonts, szHeight, (int) logFont.lfHeight);
    }
  else {
    HKEY hAppKey = pApp->GetAppRegistryKey();
    if (hAppKey != NULL) {
      pApp->DelRegTree (hAppKey, szFonts);
      ::RegCloseKey(hAppKey);
      }
    }

  cFedDoc::FontChanged();
  }
//}}}
//{{{
void cFedView::GetFont (LOGFONT* pLogFont) {

  if (pLogFont) {
    if (m_font)
      m_font->GetLogFont (pLogFont);
    else
      ::GetObject (GetStockObject (SYSTEM_FIXED_FONT), sizeof(LOGFONT), pLogFont);
    }
  }
//}}}
//{{{
void cFedView::SetFont (LOGFONT* pLogFont) {

  if (pLogFont) {
    CFont* font = new CFont();
    if (font) {
      if (font->CreateFontIndirect (pLogFont))
        SetFont (font);
      else
        delete font;
      }
    }
  }
//}}}
//{{{
bool cFedView::LoadFontFromRegistry() {

  CWinApp* pApp = AfxGetApp();
  ASSERT_VALID(pApp);

  CString name = pApp->GetProfileString (szFonts, szName);
  int height   = pApp->GetProfileInt (szFonts, szHeight, 0);

  if ((height != 0) && !name.IsEmpty() && (_stricmp (name, "Fixedsys") != 0)) {
    CFont* font = new CFont;
    if (font) {
      if (font->CreateFont (height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, name)) {
        m_font = font;
        cFedDoc::FontChanged();
        }
      else
        delete font;
      }
    }

  return true;
  }
//}}}

IMPLEMENT_DYNCREATE(cFedView, CScrollView)
//{{{
BEGIN_MESSAGE_MAP(cFedView, CScrollView)
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_SYSKEYDOWN()
  ON_WM_SYSKEYUP()
  ON_WM_DROPFILES()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEWHEEL()

  ON_UPDATE_COMMAND_UI (ID_INDICATOR_REC, OnUpdateIndicatorRec)
  ON_UPDATE_COMMAND_UI (ID_INDICATOR_OVR, OnUpdateIndicatorOvr)
  ON_UPDATE_COMMAND_UI (ID_INDICATOR_POS, OnUpdateIndicatorPos)
  ON_UPDATE_COMMAND_UI (ID_FILE_SAVE, OnUpdateFileSave)
  ON_UPDATE_COMMAND_UI (ID_FILE_SAVEALL, OnUpdateFileSaveAll)
  ON_UPDATE_COMMAND_UI (ID_FILE_SAVEPROJECT, OnUpdateFileSaveProject)
  ON_UPDATE_COMMAND_UI (ID_FILE_PRINT, OnUpdateFilePrint)
  ON_UPDATE_COMMAND_UI (ID_EDIT_CUT, OnUpdateEditCut)
  ON_UPDATE_COMMAND_UI (ID_EDIT_COPY, OnUpdateEditCopy)
  ON_UPDATE_COMMAND_UI (ID_EDIT_PASTE, OnUpdateEditPaste)
  ON_UPDATE_COMMAND_UI (ID_EDIT_REPLACE, OnUpdateEditReplace)
  ON_UPDATE_COMMAND_UI (ID_EDIT_PROTECT, OnUpdateEditProtect)
  ON_UPDATE_COMMAND_UI (ID_VIEW_OUTPUT, OnUpdateViewOutput)
  ON_UPDATE_COMMAND_UI (ID_VIEW_LINENUMBERS, OnUpdateViewLinenumbers)
  ON_UPDATE_COMMAND_UI (ID_VIEW_FOLDS, OnUpdateViewFolds)
  ON_UPDATE_COMMAND_UI (ID_VIEW_GOTOLINE, OnUpdateViewGotoline)

  ON_REGISTERED_MESSAGE (WM_FINDREPLACE, OnFindReplace)

  ON_WM_RBUTTONDBLCLK()

  ON_COMMAND (ID_FILE_NEW, OnFileNew)
  ON_COMMAND (ID_FILE_OPEN, OnFileOpen)
  ON_COMMAND (ID_FILE_UPDATE, OnFileReload)
  ON_COMMAND (ID_FILE_SAVE, OnFileSave)
  ON_COMMAND (ID_FILE_SAVEALL, OnFileSaveAll)
  ON_COMMAND (ID_FILE_SAVE_AS, OnFileSaveas)
  ON_COMMAND (ID_FILE_SAVEPROJECT, OnFileSaveProject)
  ON_COMMAND (ID_FILE_CLOSE, OnFileClose)

  ON_COMMAND (ID_EDIT_CUT, OnCut)
  ON_COMMAND (ID_EDIT_COPY, OnCopy)
  ON_COMMAND (ID_EDIT_PASTE, OnPaste)

  ON_COMMAND (ID_EDIT_FINDAGAIN, OnFindAgain)
  ON_COMMAND (ID_EDIT_FINDDOWN, OnFindDown)
  ON_COMMAND (ID_EDIT_FINDUP, OnFindUp)
  ON_COMMAND (ID_EDIT_FIND, OnFindDialog)
  ON_COMMAND (ID_EDIT_REPLACE, OnFindReplaceDialog)

  ON_COMMAND (ID_EDIT_PROTECT, OnFileToggleProtect)

  ON_COMMAND (ID_VIEW_FOLDOPENALL, OnFoldOpenAll)
  ON_COMMAND (ID_VIEW_FOLDCLOSEALL, OnFoldCloseAll)

  ON_COMMAND (ID_VIEW_GOTOLINE, OnGotoLineNumber)
  ON_COMMAND (ID_VIEW_LINENUMBERS, OnFileShowLineNumbers)

  ON_COMMAND (ID_VIEW_FOLDS, OnFileShowFolds)
END_MESSAGE_MAP()
//}}}

//{{{  drawing
//{{{
void cFedView::SetColour (CDC* pDC, int col, int& curcol, bool beyond_wrap)
{
  if (col != curcol) {
    pDC->SetTextColor (cColour::GetColour (col));
    pDC->SetBkColor (cColour::GetBkColour (col, beyond_wrap));
    if (m_textFont && m_boldFont && m_textFont) {
      if ((col == c_Keyword)    || (col == c_FoldOpen)   ||
          (col == c_FoldClosed) || (col == c_FoldCreate) ||
          (col == c_FoldCopied))
        pDC->SelectObject (m_boldFont);
      else if (col == c_CommentText)
        pDC->SelectObject (m_italicFont);
      //else if ((curcol == c_Keyword)    || (curcol == c_FoldOpen)   ||
               //(curcol == c_FoldClosed) || (curcol == c_FoldCreate) ||
               //(curcol == c_FoldCopied) || (curcol == c_CommentText))
      else
        pDC->SelectObject (m_textFont);
      }
    curcol = col;
    }
}
//}}}

//{{{
int cFedView::DrawText (CDC* pDC, cTextLine* textline, int ypos, int fold_indent, bool wipe)
{
  bool showEditMark = textline->isEdited();
  bool showDeleteMark = textline->hasDeletedBelow();

  //{{{  setup select range parameters
  int x1 = m_selectRange.FirstXpos();
  int y1 = m_selectRange.FirstYpos();
  int x2 = m_selectRange.LastXpos();
  int y2 = m_selectRange.LastYpos();
  //}}}
  //{{{  setup booleans to control the select colour display
  bool select_start = m_selectActive && (ypos == y1) && (x1 > 0);
  bool select_all =  m_selectActive && ( ((ypos > y1) && (ypos < y2))
                             || ((ypos == y1) && (x1 == 0) && (ypos < y2)) );
  bool select_end = m_selectActive && (ypos == y2);
  bool select_margin = select_all || (select_end && !select_start);
  bool select_textend = select_all || (select_start && !select_end);
  if (m_selectColumn) {
    select_start = m_selectActive && (ypos >= y1) && (ypos <= y2);
    select_end = select_start;
    select_all = false;
    select_margin = false;
    select_textend = false;
    }
  //}}}
  int x = (textline && textline->isFileLine()) ? m_Margin.x : m_xStart;
  int y = m_Margin.y + ypos * m_cChar.cy;

  int xWrapPos = 0x7fffffff;
  if (cOption::ShowWrapMark && (cOption::LineWrapLength > 0) && !textline->isFileLine())
    xWrapPos = x + cOption::LineWrapLength * m_cChar.cx;

  int curcol = c_Null;
  int linecol;
  const char* text = textline ? textline->Text (m_view, linecol) : 0;
  int col = linecol;
  int len = 0;

  if (!cColour::isDefaultBkColour (col)) {
    int xs = GetScrollPosition().x;
    pDC->FillSolidRect (xs, y, m_cWlen.cx, m_cChar.cy, cColour::GetBkColour (col));
    }
  else if (wipe || (xWrapPos < 0x7fffffff)) {
    //{{{  pre-wipe text background
    int xs = GetScrollPosition().x;
    int len = min (xWrapPos - xs, m_cWlen.cx);
    if (wipe)
      pDC->FillSolidRect (xs, y, len, m_cChar.cy, cColour::GetBkColour (col));
    xs += len;
    len = m_cWlen.cx - len;
    if (len > 0)
      pDC->FillSolidRect (xs, y, len, m_cChar.cy, cColour::GetBkColour (col, cOption::LineWrapShade));
    //}}}
    }

  if (m_ShowLineNumbers && !textline->isFileLine()) {
    //{{{  print line number
    char ss [20] = " *** ";
    if (textline->LineNum() > 0)
      sprintf (ss, "%4d ", textline->LineNum());
    curcol = c_CommentText;
    pDC->FillSolidRect (0, y, m_Margin.x, m_cChar.cy, cColour::GetBkColour (curcol));
    pDC->SetTextColor (cColour::GetColour (curcol));
    pDC->SetBkColor (cColour::GetBkColour (curcol));
    pDC->TextOut (m_Margin.x, y, ss);
    //}}}
    }

  int indent = fold_indent;
  if (textline != m_curTop)
    indent += textline->Indent();
  //{{{  wipe indent space appropriately
  int xm = m_xStart - m_Margin.x;
  int xs = m_xStart + x1 * m_cChar.cx;
  int xe = m_xStart + x2 * m_cChar.cx;
  int xt = m_xStart + indent * m_cChar.cx;

  if (select_margin) {
    if (select_end && (x2 <= indent)) {
      pDC->FillSolidRect (xm, y, xe-xm, m_cChar.cy, cColour::GetBkColour (c_Select));
      pDC->FillSolidRect (xe, y, xt-xe, m_cChar.cy, cColour::GetBkColour (linecol));
      select_end = false;
      }
    else {
      pDC->FillSolidRect (xm, y, xt-xm, m_cChar.cy, cColour::GetBkColour (c_Select));
      col = c_Select;
      }
    }
  else {
    if (cOption::ShowLeftMargin) {
      pDC->FillSolidRect (xm, y, m_Margin.x, m_cChar.cy, cColour::GetBkColour (col, cOption::LineWrapShade));
      xm = m_xStart;
      }
    if (select_start && (x1 <= indent)) {
      select_start = false;
      if (x1 == indent) {
        select_all = !select_end;
        col = c_Select;
        }
      else {
        pDC->FillSolidRect (xm, y, xs-xm, m_cChar.cy, cColour::GetBkColour (linecol));
        if (select_end && (x2 <= indent)) {
          pDC->FillSolidRect (xs, y, xe-xs, m_cChar.cy, cColour::GetBkColour (c_Select));
          pDC->FillSolidRect (xe, y, xt-xe, m_cChar.cy, cColour::GetBkColour (linecol));
          select_end = false;
          }
        else {
          pDC->FillSolidRect (xs, y, xt-xs, m_cChar.cy, cColour::GetBkColour (c_Select));
          select_all = !select_end;
          col = c_Select;
          }
        }
      }
    else
      pDC->FillSolidRect (xm, y, xt-xm, m_cChar.cy, cColour::GetBkColour (linecol));
    }
  //}}}

  int xleft = x;
  if (indent > 0)
    x += indent * m_cChar.cx;

  SetColour (pDC, col, curcol);
  //{{{  print Fed marks
  char foldstr [20];
  *foldstr = 0;
  if (textline->isFold()) {
    if (!textline->isFold()->FoldEnd()) {
      if (m_ShowFolds && !textline->isFileLine() && m_curFile->CommentStart())
        strcat (foldstr, m_curFile->CommentStart());
      strcat (foldstr, "!!!  ");
      }
    else if (textline->isFold()->isOpen(m_view) || !m_view) {
      if (textline != m_curTop)
        fold_indent += textline->Indent();
      if (m_ShowFolds && !textline->isFileLine() && m_curFile->CommentStart())
        strcat (foldstr, m_curFile->CommentStart());
      strcat (foldstr, "{{{");
      if (textline->isProj())
        strcat (foldstr, "  Project: ");
      else if (textline->isFile()) {
        if (textline->isFile()->hasTabs())
          strcat (foldstr, "t File: ");
        else
          strcat (foldstr, "  File: ");
        }
      else if (text && *text)
        strcat (foldstr, "  ");
      }
    else {
      showEditMark |= textline->isFold()->hasEdits();
      if (textline->isProj()) {
        cTextProj* proj = textline->isProj();
        sprintf (foldstr, "... %6d ", proj->FileCount());
        strcat (foldstr, "Project: ");
        }
      else if (textline->isFile()) {
        cTextFile* file = textline->isFile();
        if (file->isEmpty())
          strcat (foldstr, "---        ");
        else if (file->isModified()) {
          if (file->hasTabs())
            sprintf (foldstr, "***t%6d ", file->LineCount());
          else
            sprintf (foldstr, "*** %6d ", file->LineCount());
          }
        else {
          if (file->hasTabs())
            sprintf (foldstr, "...t%6d ", file->LineCount());
          else
            sprintf (foldstr, "... %6d ", file->LineCount());
          }
        strcat (foldstr, "File: ");
        }
      else {
        if (m_ShowFolds && !textline->isFileLine() && m_curFile->CommentStart())
          strcat (foldstr, m_curFile->CommentStart());
        strcat (foldstr, "...  ");
        }
      }
    }
  else if (textline->isFoldEnd()) {
    if (m_ShowFolds && !textline->isFileLine() && m_curFile->CommentStart())
        strcat (foldstr, m_curFile->CommentStart());
    strcat (foldstr, "}}}  ");

    if (textline->ParentFold()) {
      if (textline->ParentFold()->isProj())
        strcat (foldstr, "Project: ");
      else if (textline->ParentFold()->isFile())
        strcat (foldstr, "File: ");
      fold_indent -= textline->ParentFold()->Indent();
      if (fold_indent < 0)
        fold_indent = 0;
      }
    }

  if (*foldstr) {
    //{{{  print fold string
    const char* str = foldstr;
    if (select_start || select_end) {
      int slen = (int)strlen(str);

      if (select_start) {
        //{{{  print text upto start of select area
        int xpos = (x - m_xStart) / m_cChar.cx;
        int len = x1 - xpos;
        if (len > slen)
          len = slen;
        if ((len > 0) && (len <= slen)) {
          SetColour (pDC, col, curcol);
          pDC->TextOut (x, y, str, len);
          x += pDC->GetTextExtent (str, len).cx;
          str += len;
          slen -= len;
          }
        if (len >= x1 - xpos)
          col = c_Select;
        //}}}
        }

      if (select_end && (slen > 0)) {
        int xpos = (x - m_xStart) / m_cChar.cx;
        int len = x2 - xpos;
        if (len <= 0)
          col = linecol;
        else if (len <= slen) {
          SetColour (pDC, col, curcol);
          pDC->TextOut (x, y, str, len);
          x += pDC->GetTextExtent (str, len).cx;
          str += len;
          col = linecol;
          }
        }

      }

    if (*str) {
      SetColour (pDC, col, curcol);
      pDC->TextOut (x, y, str);
      x += pDC->GetTextExtent (str).cx;
      }
    //}}}
    }
  //}}}

  if (text) {
    cTextLine* displayLine = 0;
    if (cOption::ShowClosedFoldComments && (text != textline->cTextLine::GetText(m_view)))
      displayLine = textline->GetTextLine(m_view);
    const unsigned char* format = displayLine ? displayLine->GetFormat() : textline->GetFormat();

    if (format && (!select_all)) {
      //{{{  print colour-formatted text
      int forceCol = c_FoldCopied; //linecol;
      col = *format++;
      len = *format++;
      if (displayLine && (col != c_CommentText))
        col = forceCol;
      linecol = col;
      if (select_end && !select_start)
        col = c_Select;
      //{{{  handle tabs
      while (*text == '\t') {
        text++;
        x += m_tabLen * m_cChar.cx;
        if (len > 0) {
          if (--len == 0) {
            col = *format++;
            len = *format++;
            if (displayLine && (col != c_CommentText))
              col = forceCol;
            }
          }
        }
      //}}}
      while (len && *text) {
        int xpos = (x - m_xStart) / m_cChar.cx;
        if (select_start && (x1 >= xpos) && (x1 < xpos + len)) {
          //{{{  print text upto start of select area
          int slen = x1 - xpos;
          if (slen > 0) {
            SetColour (pDC, col, curcol);
            pDC->TextOut (x, y, text, slen);
            x += pDC->GetTextExtent (text, slen).cx;
            text += slen;
            len -= slen;
            xpos += slen;
            }
          //}}}
          col = c_Select;
          if (!select_end)
            break;         //rest of line is now select-colour
          }
        if (select_end && (len > 0)) {
          //{{{  print selected text
          if (x2 >= xpos + len) {
            if (!select_start || (x1 <= xpos))
              col = c_Select;
            }
          else if (x2 > xpos) {
            int slen = x2 - xpos;
            col = c_Select;
            SetColour (pDC, col, curcol);
            pDC->TextOut (x, y, text, slen);
            x += pDC->GetTextExtent (text, slen).cx;
            text += slen;
            len -= slen;
            col = linecol;
            }
          //}}}
          }
        if (len > 0) {
          SetColour (pDC, col, curcol);
          pDC->TextOut (x, y, text, len);
          x += pDC->GetTextExtent (text, len).cx;
          text += len;
          }
        col = *format++;
        len = *format++;
        if (displayLine && (col != c_CommentText))
          col = forceCol;
        linecol = col;
        }
      }
      //}}}
    if (*text) {
      //{{{  print rest of line in a single colour
      //while (*text == '\t') {
      //  text++;
      //  x += m_tabLen * m_cChar.cx;
      //  }

      if (select_start || select_end) {
        int slen = (int)strlen(text);

        if (select_start) {
          //{{{  print text upto start of select area
          int xpos = (x - m_xStart) / m_cChar.cx;
          int len = x1 - xpos;
          if (len > slen)
            len = slen;
          //if ((len > 0) && (len <= slen)) {
          if (len > 0) {
            SetColour (pDC, col, curcol);
            pDC->TextOut (x, y, text, len);
            x += pDC->GetTextExtent (text, len).cx;
            text += len;
            slen -= len;
            }
          if (len >= 0)
            col = c_Select;
          //}}}
          }

        if (select_end && (slen > 0)) {
          int xpos = (x - m_xStart) / m_cChar.cx;
          int len = x2 - xpos;
          if (len > 0) {
            col = c_Select;
            if (len < slen) {
              SetColour (pDC, col, curcol);
              pDC->TextOut (x, y, text, len);
              x += pDC->GetTextExtent (text, len).cx;
              text += len;
              col = linecol;
              }
            }
          }

        }

      if (*text)
        {
        SetColour (pDC, col, curcol);

        cTextLine* line = textline;
        if (line && textline->isFoldEnd() && (textline->ParentFold() == m_curTop))
          line = textline->ParentFold();   // fetch proj-top if we are a foldend

        if (line && (line == m_curTop) && line->isProj())
          {
          std::string filename = line->isProj()->PathnameForDisplay();
          pDC->TextOut (x, y, filename.c_str());
          x += pDC->GetTextExtent (filename.c_str()).cx;
          }
        else
          {
          pDC->TextOut (x, y, text);
          x += pDC->GetTextExtent (text).cx;
          }
        //{{{  handle sub-sections of a project
        //cTextProj* proj = textline->isProj();
        //while (proj && !proj->Pathname())
          //{
          //if (proj->ParentFold())
            //proj = proj->ParentFold()->isProj();
          //else
            //proj = NULL;
          //if (proj)
            //{
            //pDC->TextOut (x, y, " - ");
            //x += pDC->GetTextExtent (" - ").cx;
            //const char* name = proj->Pathname();
            //if (!name || !*name)
              //name = proj->cTextLine::Text();
            //if (name)
              //{
              //pDC->TextOut (x, y, name);
              //x += pDC->GetTextExtent (name).cx;
              //}
            //}
          //}
        //}}}
        }
      }
      //}}}
    if (m_displayMatch) {
      //{{{  display matching bracket highlights
      if (ypos == m_pos.y) {
        col = c_HighLight;
        SetColour (pDC, col, curcol);
        char ch = textline->get_text_char (m_pos.x - fold_indent);
        pDC->TextOut (xleft + m_pos.x * m_cChar.cx, y, ch);
        }
      if (ypos == m_matchPos.y) {
        col = c_HighLight;
        SetColour (pDC, col, curcol);
        char ch = textline->get_text_char(m_matchPos.x - fold_indent);
        pDC->TextOut (xleft + m_matchPos.x * m_cChar.cx, y, ch);
        }
      }
      //}}}
    }

  if (m_ShowFolds && textline->isFoldLine() && !textline->isFileLine() && m_curFile->CommentEnd()) {
    pDC->TextOut (x, y, m_curFile->CommentEnd());
    x += pDC->GetTextExtent (m_curFile->CommentEnd()).cx;
    }

  if (x > m_xStart)
    pDC->FillSolidRect (x, y, m_cChar.cx / 3, m_cChar.cy, cColour::GetBkColour (select_textend ? c_Select : curcol));
  else if (select_textend)
    pDC->FillSolidRect (x-m_Margin.x, y, m_Margin.x, m_cChar.cy, cColour::GetBkColour (c_Select));

  if (cOption::ShowEditMarks) {
    if (showEditMark)
      pDC->FillSolidRect (0, y, m_cChar.cx / 4, m_cChar.cy, cColour::GetColour(c_Text));
    if (showDeleteMark)
      pDC->FillSolidRect (0, y+m_cChar.cy-1, m_Margin.x, 1, cColour::GetColour(c_Text));
    }

  return fold_indent;
  }
//}}}
//{{{
void cFedView::OnDraw (CDC* pDC)
{
  CPoint pos = GetScrollPosition();
  int y = pos.y / m_cChar.cy;
  int ylast  = (pos.y + m_cWlen.cy) / m_cChar.cy;

  cTextLine* cur = 0;
  cTextLine* last = 0;
  if (m_curTop)
    {
    cur = m_curTop->GetLine(m_view, y);
    last = m_curTop->FoldEnd();
    }
  bool lastVisible = false;

  if (cur) {
    int fold_indent = 0;
    if (cur != m_curTop)
      fold_indent = cur->GetFoldIndent (m_curTop);

    while (cur && (y <= ylast))
      {
      fold_indent = DrawText (pDC, cur, y, fold_indent);
      y += 1;
      if (cur == last) {
        lastVisible = true;
        break;
        }
      cur = cur->NextLine (m_view);
      }
    }
  m_FoldendVisible = lastVisible;

  if (cOption::ShowWrapMark && (cOption::LineWrapLength > 0) && !m_curTop->isProj()) {
    // wipe remaining wrap mark background
    y *= m_cChar.cy;
    int x = m_xStart + cOption::LineWrapLength * m_cChar.cx;
    if (x < pos.x)
      x = pos.x;
    int xlen = pos.x + m_cWlen.cx - x;
    int ylen = pos.y + m_cWlen.cy - y;

    if ((xlen > 0) && (ylen > 0))
      pDC->FillSolidRect (x, y, xlen, ylen, cColour::GetBkColour (c_Text, true));
    }

  }
//}}}

//{{{
void cFedView::OnPrepareDC (CDC* pDC, CPrintInfo* pInfo)
{
  ASSERT_VALID(pDC);

  CScrollView::OnPrepareDC (pDC, pInfo);

  if (cColour::Palette()) {
    pDC->SelectPalette (cColour::Palette(), TRUE);
    pDC->RealizePalette();
    }

  if (Font)
    pDC->SelectObject (Font);
  else if (m_font)
    pDC->SelectObject (m_font);
  else
    pDC->SelectStockObject (SYSTEM_FIXED_FONT);
  }
//}}}
//{{{
void cFedView::OnInitialUpdate()
{
  CClientDC dc (this);
  OnPrepareDC (&dc, NULL);

  TEXTMETRIC tm;
  dc.GetTextMetrics (&tm);
  m_cChar.cx = tm.tmAveCharWidth;
  m_cChar.cy = tm.tmHeight;
  m_pos.x = 0;
  m_pos.y = 0;

  cFedDoc* pDoc = GetDocument();
  m_view = pDoc->ObtainView();
  m_curFile = pDoc->GetInitialFile();
  m_curTop = m_curFile;
  m_selectActive = false;
  if (m_curTop) {
    m_curTop->Open (m_view);
    if (m_curTop->isFile())
      m_pos.y = 1;
    }

  DragAcceptFiles (TRUE);
  if (cFedApp::GotoLineNumber > 0) {
    GotoLineNumber (cFedApp::GotoLineNumber);
    cFedApp::GotoLineNumber = 0;
    }

  CView::OnInitialUpdate();
  }
//}}}
//{{{
void cFedView::OnUpdate (CView* pSender, LPARAM lHint, CObject* pHint)
{
  cRange* range = (cRange*) pHint;
  bool lastVisible = m_FoldendVisible;

  if (lHint == cRange::eFontChanged) {
    // adjust view parameters for a change of font
    CClientDC dc (this);
    OnPrepareDC (&dc, NULL);

    TEXTMETRIC tm;
    dc.GetTextMetrics (&tm);
    m_cChar.cx = tm.tmAveCharWidth;
    m_cChar.cy = tm.tmHeight;

    RECT rect;
    GetWindowRect (&rect);
    m_Margin = CPoint (m_cChar.cx, 0);

    m_xStart = m_Margin.x;
    if (m_ShowLineNumbers)
      m_xStart += m_Margin.x + 5 * m_cChar.cx;

    SetCursorPos (m_pos.x, m_pos.y);
    }

  if (range) {
    switch (lHint) {
      //{{{
      case cRange::eWarnDeleteFold:
        if (range->Line() && range->Line()->isFold()) {
          cTextFold* cur = m_curTop;
          while (cur) {
            if (cur == range->Line()) {
              CPoint pos = GetScrollPosition();
              pos.y /= m_cChar.cy;
              int level;
              cur->GetAbove (m_pos.x, m_pos.y, pos.y, level);
              cTextFold* newtop = cur->ParentFold();
              ASSERT (newtop);

              if (m_curFile == cur)
                m_curFile = newtop->isFile();
              m_curTop = newtop;
              m_curTop->Open(m_view);    // should already be open!

              pos.y *= m_cChar.cy;
              SetScrollPos (SB_VERT, pos.y);
              return;
              }

            cur = cur->ParentFold();
            }
          }

        return;
      //}}}
      }
    }

  SetupScrollBars();
  UpdateCaret();

  if (m_debugWin)
    m_debugWin->Update (this);

  if (range) {
    switch (lHint) {
      //{{{
      case cRange::eChangedLine:
        if (range->File() == m_curFile)
          {
          cTextLine* line = range->Line();
          while (line && (line != m_curTop))
            line = line->ParentFold();
          if (!line) return;   // updated line is not within m_curTop

          line = range->Line();

          CPoint pos = GetScrollPosition();
          int y = pos.y / m_cChar.cy;
          int ylast  = (pos.y + m_cWlen.cy) / m_cChar.cy;

          cTextLine* cur = 0;
          cTextLine* last = 0;
          if (m_curTop)
            {
            cur = m_curTop->GetLine(m_view, y);
            last = m_curTop->FoldEnd();
            }
          bool lastVisible = false;

          while (cur && (y <= ylast))
            {
            if (cur == line)
              //{{{  found line - update it & return
              {
              int fold_indent = 0;
              if (line != m_curTop)
                fold_indent = line->GetFoldIndent (m_curTop);

              HideCaret();
              CClientDC dc (this);
              OnPrepareDC (&dc, NULL);
              dc.SetViewportOrg (-pos);
              DrawText (&dc, line, y, fold_indent, true);
              ShowCaret();
              return;
              }
              //}}}
            y += 1;
            if (cur == last)
              break;
            cur = cur->NextLine (m_view);
            }
          }
        return;
      //}}}
      //{{{
      case cRange::eRenumbered:
        if (!m_ShowLineNumbers)
          return;
        break;
      //}}}
      }
    }

  if (lastVisible && (m_curTop == cFedDoc::getOutputBuffer()))
    DisplayEnd();

  Invalidate();
  }
//}}}

//{{{
void cFedView::SetupScrollBars()
{
  int mapMode;
  SIZE curTotal, curPage, curLine;
  GetDeviceScrollSizes (mapMode, curTotal, curPage, curLine);

  CSize line (m_cChar);
  CSize size ((m_curTop->max_width(m_view) + m_ScrollMargin.cx) * m_cChar.cx + m_xStart,
               m_curTop->count_lines(m_view) * m_cChar.cy);

  CSize page (m_cWlen.cx / m_cChar.cx, m_cWlen.cy / m_cChar.cy);

  if (page.cx > 5)
    page.cx -= 5;
  if (page.cy > 5)
    page.cy -= 5;

  page.cx *= line.cx;
  page.cy *= line.cy;

  if ((size != curTotal) || (page != curPage) || (line != curLine))
    SetScrollSizes (MM_TEXT, size, page, m_cChar);
  }
//}}}

//{{{
void cFedView::DrawChar (CDC* pDC, CPoint atpos, int col)
{
  CPoint pos = GetScrollPosition();
  if (((atpos.y + 1 ) * m_cChar.cy < pos.y) || (atpos.y * m_cChar.cy >= pos.y + m_cWlen.cy))
    return;

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, atpos.y);
  ASSERT (line);

  line = line->GetTextLine (m_view);
  ASSERT (line);

  int xpos = atpos.x;
  int x = line->isFileLine() ? m_Margin.x : m_xStart;
  int y = m_Margin.y + atpos.y * m_cChar.cy;
  char ch = ' ';
  int curcol = c_Null;
  if (col == c_Null)
    col = line->GetColour (m_view, xpos - line->GetFoldIndent (m_curTop));
  int spacelen = pDC->GetTextExtent (" ", 1).cx;

  int indent = line->GetIndent (m_curTop);

  if (xpos < indent)
    x += xpos * spacelen;
  else {
    x += indent * spacelen;
    xpos -= indent;
    if (line->isFoldLine()) {
      int len = xpos < 5 ? xpos : 5;
      if (line->isFoldEnd()) {
        x += pDC->GetTextExtent ("}}}  ", len).cx;
        if (xpos < 3) ch = '}';
        }
      else if (line->isFold() && line->isFold()->isOpen(m_view)) {
        x += pDC->GetTextExtent ("{{{  ", len).cx;
        if (xpos < 3) ch = '{';
        }
      else {
        x += pDC->GetTextExtent ("...  ", len).cx;
        if (xpos < 3) ch = '.';
        }
      xpos -= len;
      }

    const char* text = line->GetText (m_view);
    if (text && (xpos >= 0)) {
      int len = (int)strlen(line->GetText (m_view));
      if (xpos < len) {
        x += pDC->GetTextExtent (text, xpos).cx;
        ch = text [xpos];
        }
      else {
        x += pDC->GetTextExtent (text, len).cx;
        x += (xpos - len) * spacelen;
        }
      }
    }

  SetColour (pDC, col, curcol);
  pDC->TextOut (x, y, ch);
  }
//}}}
//{{{
void cFedView::DrawMatch (bool display) {

  if (m_displayMatch) {
    HideCaret();

    CClientDC dc (this);
    OnPrepareDC (&dc, NULL);

    dc.SetViewportOrg (-GetScrollPosition());

    int col = display ? c_HighLight : c_Null;
    DrawChar (&dc, m_matchPos, col);
    DrawChar (&dc, m_pos, col);

    ShowCaret();
    }
  }
//}}}
//}}}
//{{{
void cFedView::OnShowDebug() {

  if (m_debugWin) {
    if (m_debugWin->IsWindowVisible()) {
      m_debugWin->NextPage();
      }
    else {
      m_debugWin->ShowWindow (SW_SHOWNORMAL);
      SetFocus();  // back to here from debug window
      }
    }

  else {
    m_debugWin = new cDebugWin (this);
    m_debugWin->CreateEx (0,
      AfxRegisterWndClass (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS),
      "Fed Debug",
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      0, 0, 500, 700,
      this->m_hWnd, 0);

    SetFocus();  // back to here from debug window
    }
  }
//}}}

//{{{  Windows message handlers
//{{{
void cFedView::OnSize (UINT nType, int cx, int cy) {

  CScrollView::OnSize(nType, cx, cy);

  m_cWlen.cx = cx;
  m_cWlen.cy = cy;

  CClientDC dc (this);
  OnPrepareDC (&dc, NULL);

  TEXTMETRIC tm;
  dc.GetTextMetrics (&tm);
  m_cChar.cx = tm.tmAveCharWidth;
  m_cChar.cy = tm.tmHeight;

  RECT rect;
  GetWindowRect (&rect);
  m_Margin = CPoint (m_cChar.cx, 0);

  m_xStart = m_Margin.x;
  if (m_ShowLineNumbers)
    m_xStart += m_Margin.x + 5 * m_cChar.cx;

  SetCursorPos (m_pos.x, m_pos.y);
  }
//}}}
//{{{
void cFedView::OnSetFocus (CWnd* pOldWnd) {

  CView::OnSetFocus(pOldWnd);

  CPoint pos = GetScrollPosition();
  POINT caretpos;
  caretpos.x = m_pos.x * m_cChar.cx + m_xStart - 1 - pos.x;
  caretpos.y = m_pos.y * m_cChar.cy - pos.y;

  CreateSolidCaret (2, m_cChar.cy);
  SetCaretPos (caretpos);
  ShowCaret();

  ShowFilename();

  if (cOption::CheckFileIsNewer)
    if (CheckFileTime ("SetFocus debug info"))
      OnFileReload();
  }
//}}}

//{{{
void cFedView::OnKillFocus (CWnd* pNewWnd) {

  m_scrollPos = GetScrollPosition();

  CView::OnKillFocus(pNewWnd);

  HideCaret();
  ::DestroyCaret();
  }
//}}}
//{{{
void cFedView::OnDropFiles (HDROP hDropInfo) {

  char filename [_MAX_PATH];
  cFedDoc* pDoc = GetDocument();
  cTextFile* newfile = 0;

  cTextFile* after = m_curFile;
  if (m_curFile->isProj()) {
    POINT point;
    DragQueryPoint (hDropInfo, &point);
    int y = (GetScrollPosition().y + point.y) / m_cChar.cy;
    cTextLine* line = m_curTop->GetLine(m_view, y);
    if (line) {
      if (line->isFile())
        after = line->isFile();
      else if (line->isFoldEnd()
            && line->isFoldEnd()->ParentFold()
            && line->isFoldEnd()->ParentFold()->isProj()) {
        if (line->isFoldEnd()->ParentFold()->isProj()->LastFile())
          after = line->isFoldEnd()->ParentFold()->isProj()->LastFile();
        else
          after = line->isFoldEnd()->ParentFold()->isProj();
        }
      }
    }

  int filecount = DragQueryFile (hDropInfo, -1, 0, 0);
  for (int n = 0; n < filecount; n++) {
    DragQueryFile (hDropInfo, n, filename, _MAX_PATH);
    after = pDoc->DoFileOpen (filename, after, m_view);
    if (!newfile)
      newfile = after;
    }

  if (newfile) {
    if (newfile->ParentFold() == m_curFile)
      DisplayAtLine (newfile);
    else
      SwitchToFile (newfile);
    }

  ShowFilename();
  SetForegroundWindow();
  }
//}}}

//{{{
void cFedView::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {

  if (pScrollBar != NULL && pScrollBar->SendChildNotifyLastMsg())
    return;     // eat it

  // ignore scroll bar msgs from other controls
  if (pScrollBar != GetScrollBarCtrl(SB_HORZ))
    return;

  // This section is added to CScrollView version to obtain 32-bit Positions
  if (nSBCode == SB_THUMBTRACK) {
    // nPos is only 16-bit, get the 32-bit version
    SCROLLINFO si;
    if (GetScrollInfo (SB_HORZ, &si, SIF_TRACKPOS))
      nPos = (UINT) si.nTrackPos;
    }

  OnScroll(MAKEWORD(nSBCode, -1), nPos);
  }
//}}}
//{{{
void cFedView::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {

  if (pScrollBar != NULL && pScrollBar->SendChildNotifyLastMsg())
    return;     // eat it

  // ignore scroll bar msgs from other controls
  if (pScrollBar != GetScrollBarCtrl(SB_VERT))
    return;

  // This section is added to CScrollView version to obtain 32-bit Positions
  if (nSBCode == SB_THUMBTRACK) {
    // nPos is only 16-bit, get the 32-bit version
    SCROLLINFO si;
    if (GetScrollInfo (SB_VERT, &si, SIF_TRACKPOS))
      nPos = (UINT) si.nTrackPos;
    }

  OnScroll(MAKEWORD(-1, nSBCode), nPos);
  }
//}}}
//}}}
//{{{  Mouse message handlers
//{{{
void cFedView::OnLButtonDown (UINT nFlags, CPoint point) {

  ClearCurMargins();
  CPoint pos = TranslateScreenPos (point);
  ASSERT (m_curTop);

  if ((pos.y == 0) && (m_curTop->isFile()))
    pos.y = 1;           // File line is not allowed

  cTextLine* line = m_curTop->GetLine (m_view, pos.y);
  ASSERT (line);
  if (line->isFileLine())
    pos.x = 0;         // Only start of FileEnd line allowed

  Unselect();
  SetCapture();
  SetCursorPos (pos);

  m_selectPos = m_pos;
  m_mouseDown = true;
  m_selectColumn = (GetKeyState (VK_CONTROL) < 0);
  m_selectWholeLines = !m_selectColumn && IsLeftMargin (point.x); // ? true : false;
  m_selectRange.SetColumn (m_selectColumn);

  if (m_debugWin)
    m_debugWin->Update(this);

  CView::OnLButtonDown(nFlags, point);
  }
//}}}
//{{{
void cFedView::OnMButtonDown (UINT nFlags, CPoint point) {

  OnRButtonDblClk (nFlags, point);
  }
//}}}
//{{{
void cFedView::OnRButtonDown (UINT nFlags, CPoint point) {

  if (nFlags & MK_CONTROL)
    OnRButtonDblClk (nFlags, point);
  else
    {
    Unselect();
    ASSERT (m_curTop);
    CPoint pos = TranslateScreenPos (point);

    cTextLine* line = m_curTop->GetLine (m_view, pos.y);
    if (line) {
      cTextFold* fold = line->isFoldEnd() ? line->ParentFold() : line->isFold();

      if (fold) {
        SetCursorPos (pos);
        if (fold == m_curTop)
          OnFoldExit();
        else if (fold->isOpen(m_view)) {
          fold->Close(m_view);
          OnUpdate(this, 0L, NULL);
          }
        else if (fold->isFile())
          OnFoldEnter (fold);
        else {
          fold->Open(m_view);
          OnUpdate(this, 0L, NULL);
          }
        }
      else {
        fold = line->ParentFold();
        if (fold && (fold == m_curTop))
          OnFoldExit();
        else if (fold && fold->isOpen(m_view)) {
          fold->Close(m_view);
          OnUpdate(this, 0L, NULL);
          }
        else {
          if ((pos.y == 0) && m_curTop && (m_curTop->isFile()))
            pos.y = 1;           // File line is not allowed
          cTextLine* line = m_curTop ? m_curTop->GetLine (m_view, pos.y) : 0;
          if (line->isFoldEnd() && (m_curTop->isFile()) && (line->ParentFold() == m_curTop))
            pos.x = 0;         // Only start of FileEnd line allowed
          SetCursorPos (pos);
          }
        }

      if (m_debugWin)
        m_debugWin->Update(this);
      }
    }
  }
//}}}

//{{{
void cFedView::OnMouseMove (UINT nFlags, CPoint point) {

  if (m_mouseDown) {
    CPoint oldpos = m_pos;
    CPoint pos = TranslateScreenPos (point);
    ASSERT (m_curTop);

    if ((pos.y == 0) && (m_curTop->isFile()))
      pos.y = 1;           // File line is not allowed
    cTextLine* line = m_curTop->GetLine (m_view, pos.y);
    ASSERT (line);
    if (line->isFileLine())
      pos.x = 0;         // Only start of FileEnd line allowed
    SetCursorPos (pos);

    if (m_pos != oldpos) {
      if (!m_curTop->isProj() && ((m_selectPos.x != m_pos.x) || ((m_selectPos.y != m_pos.y) && !m_selectColumn)) ) {
        m_selectActive = true;
        if (m_selectWholeLines)
          m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, CPoint (0x7fff, m_pos.y));
        else
          m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
        }
      else if (m_selectActive)
        m_selectActive = false;


      CClientDC dc (this);
      OnPrepareDC (&dc, NULL);
      dc.SetViewportOrg (-GetScrollPosition());

      int yfirst = m_pos.y < oldpos.y ? m_pos.y : oldpos.y;
      int ylast  = m_pos.y < oldpos.y ? oldpos.y : m_pos.y;

      if (m_selectColumn && (m_pos.x != oldpos.x)) {
        // force redisplay of whole of current and old select areas
        if (m_selectPos.y < yfirst)
          yfirst = m_selectPos.y;
        if (m_selectPos.y > ylast)
          ylast = m_selectPos.y;
        }

      if (m_curTop) {
        cTextLine* cur = m_curTop->GetLine (m_view, yfirst);
        cTextLine* last = m_curTop->FoldEnd();

        if (cur) {
          //{{{  redisplay selected area change
          int fold_indent;
          if (cur != m_curTop)
            fold_indent = cur->GetFoldIndent (m_curTop);
          else
            fold_indent = -m_curTop->Indent();

          int y = yfirst;

          HideCaret();
          while (cur && (y <= ylast)) {
            fold_indent = DrawText (&dc, cur, y, fold_indent);
            if (cur == last)
              break;
            y += 1;
            cur = cur->NextLine (m_view);
            }
          ShowCaret();
          //}}}
          }
        }
      if (m_debugWin)
        m_debugWin->Update(this);
      }
    }

  CScrollView::OnMouseMove(nFlags, point);
  }
//}}}
//{{{
void cFedView::OnLButtonUp (UINT nFlags, CPoint point) {

  CPoint pos = TranslateScreenPos (point);
  ASSERT (m_curTop);

  if ((pos.y == 0) && (m_curTop->isFile()))
    pos.y = 1;           // File line is not allowed

  cTextLine* line = m_curTop->GetLine (m_view, pos.y);
  ASSERT (line);
  if (line->isFileLine())
    pos.x = 0;         // Only start of FileEnd line allowed

  ReleaseCapture();
  if (m_mouseDown) {
    m_mouseDown = false;
    SetCursorPos (pos);

    if ((m_selectPos != m_pos) && !m_curTop->isProj()) {
      m_selectActive = true;
      if (m_selectWholeLines)
        m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, CPoint (0x7fff, m_pos.y));
      else
        m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      Invalidate();      //total cleanup (for the moment)
      }
    else if (m_selectActive) {
      m_selectActive = false;
      Invalidate();
      }

    if (m_debugWin)
      m_debugWin->Update(this);
    }

  CScrollView::OnLButtonUp(nFlags, point);
  }
//}}}

//{{{
void cFedView::OnLButtonDblClk (UINT nFlags, CPoint point) {

  if (IsLeftMargin (point.x))
    OnLineSelect();
  else if (GetKeyState (VK_CONTROL) < 0)
    OnWordSelectPart();
  else
    OnWordSelect();


  if (m_debugWin)
    m_debugWin->Update(this);
  }
//}}}
//{{{
void cFedView::OnRButtonDblClk (UINT nFlags, CPoint point) {

  Unselect();
  ASSERT (m_curTop);

  CPoint pos = TranslateScreenPos (point);

  cTextLine* line = m_curTop->GetLine (m_view, pos.y);
  if (line) {
    cTextFold* fold = line->isFoldEnd() ? line->ParentFold() : line->isFold();

    if (fold) {
      SetCursorPos (pos);
      if (fold == m_curTop)
        OnFoldExit();
      else //if (!fold->isOpen(m_view))
        OnFoldEnter (fold);
      }
    else {
      if ((pos.y == 0) && m_curTop && (m_curTop->isFile()))
        pos.y = 1;           // File line is not allowed
      cTextLine* line = m_curTop ? m_curTop->GetLine (m_view, pos.y) : 0;
      if (line->isFoldEnd() && (m_curTop->isFile()) && (line->ParentFold() == m_curTop))
        pos.x = 0;         // Only start of FileEnd line allowed
      SetCursorPos (pos);
      }

    if (m_debugWin)
      m_debugWin->Update(this);
    }

  }
//}}}

//{{{
BOOL cFedView::OnMouseWheel (UINT nFlags, short zDelta, CPoint point) {

  CSize scroll (0, 0);
  if (nFlags & MK_CONTROL) {
    scroll.cx = -zDelta;
    scroll.cx /= WHEEL_DELTA;
    scroll.cx *= 3 * m_cChar.cx;
    }
  else {
    scroll.cy = -zDelta;
    scroll.cy /= WHEEL_DELTA;
    scroll.cy *= 3 * m_cChar.cy;
    }

  OnScrollBy (scroll, TRUE);
  return CScrollView::OnMouseWheel(nFlags, zDelta, point);
  }
//}}}
//}}}
//{{{  Keyboard message handlers
//{{{
bool cFedView::OnAnyKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) {
  bool actioned = false;
  int token = 0;
  char str [80];
  sprintf (str, "%04x %04x ", nChar, nFlags);

  //{{{  setup key 'state'
  int state = 0;
  if (m_goldOn) {
    state |= eGold;
    strcat (str, "Gold ");
    }
  if (GetKeyState (VK_MENU) < 0) {
    state |= eAlt;
    strcat (str, "Alt ");
    }
  if (GetKeyState (VK_CONTROL) < 0) {
    state |= eCtrl;
    strcat (str, "Ctrl ");
    }
  if (GetKeyState (VK_SHIFT) < 0) {
    state |= eShift;
    strcat (str, "Shift ");
    }
  //}}}

  m_KeyWasModifier = (nChar == VK_SHIFT) || (nChar == VK_CONTROL) || (nChar == VK_MENU);

  if ((nChar == VK_NUMLOCK) && !m_goldOn)
    m_goldOn = true;
  else if (!m_KeyWasModifier) {
    m_goldOn = false;
    //{{{  map key to token
    const char* name;
    name = cKeyMap::VirtualKeyName (nChar, state, nFlags);
    if (name)
      strcat (str, name);

    token = cKeyMap::MapVirtualKey (nChar, state, nFlags);
    m_AllowChar = (token < -1);

    name = cKeyMap::TokenName (token);
    if (name) {
      strcat (str, " = ");
      strcat (str, name);
      }
    //}}}
    if (token > 0) {
      if (m_recording && (token != k_MacroLearn)
                      && (token != k_MacroCall)
                      && (token != k_MacroLoad)
                      && (token != k_MacroSave)
                      && (token != k_MacroDialog) ) {
        if (m_macro_textmode) {
          m_macroStr += (char) 255;
          m_macro_textmode = FALSE;
          }
        m_macroStr += (char) token;
        }
      KeyProc (token);
      actioned = true;
      }
    else if (token == 0) {
      //{{{  check for a language macro key
      if (m_curFile->Language()) {
        int key = cKeyMap::VirtualKey (nChar, state, nFlags);
        const unsigned char* macro = m_curFile->Language()->FindMacro (key);
        if (macro && *macro) {
          if (m_recording) {
            if (m_macro_textmode) {
              m_macroStr += (char) 255;
              m_macro_textmode = FALSE;
              }
            m_macroStr += (char*) macro;
            const unsigned char* s = macro;
            while (*s) {
              if (*s++ == 255)
                m_macro_textmode = !m_macro_textmode;
              }
            }
          DoMacro (macro);
          SetCursorPos (m_pos);  // dummy to update caret
          actioned = true;
          }
        }
      //}}}
      }
    }

  if (m_debugWin) {
    m_debugWin->Message (0);
    m_debugWin->Message (1, str);
    m_debugWin->Message (2);
    }

  return actioned;
  }
//}}}
//{{{
bool cFedView::OnAnyKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags) {

  bool actioned = false;
  if (m_debugWin) {
    if ((nChar == VK_SHIFT) || (nChar == VK_CONTROL) || (nChar == VK_MENU)) {
      if (m_KeyWasModifier) {
        char str [80];
        *str = 0;

        int state = 0;
        if (m_goldOn)
          strcat (str, "Gold ");
        if (GetKeyState (VK_MENU) < 0)
          strcat (str, "Alt ");
        if (GetKeyState (VK_CONTROL) < 0)
          strcat (str, "Ctrl ");
        if (GetKeyState (VK_SHIFT) < 0)
          strcat (str, "Shift ");

          m_debugWin->Message (1, str);
        }
      }
    }

  return actioned;
  }
//}}}
//{{{
void cFedView::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags) {

  if (m_AllowChar) {
    char str [80];
    sprintf (str, "%04x %04x ", nChar, nFlags);
    //{{{  setup 'state'
    int state = 0;
    if (m_goldOn) {
      state |= eGold;
      strcat (str, "Gold ");
      }
    if (GetKeyState (VK_MENU) < 0) {
      state |= eAlt;
      strcat (str, "Alt+");
      }
    if (GetKeyState (VK_CONTROL) < 0) {
      state |= eCtrl;
      strcat (str, "Ctrl+");
      }
    if (GetKeyState (VK_SHIFT) < 0) {
      state |= eShift;
      strcat (str, "Shift+");
      }
    //}}}

    char* s = strchr (str, 0);
    *s++ = nChar;
    *s = 0;

    if (m_debugWin)
      m_debugWin->Message (0, str);

    if (m_curFile && m_curFile->isEditable()) {
      if ((state == eNormal) || (state = eShift)) {
        SetCurMargins();
        bool overstrike = m_overstrike && !m_selectActive;
        if (m_selectActive) {
          //{{{  first delete the select range
          m_selectActive = false;
          GetDocument()->Delete (m_selectRange);
          SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
          //}}}
          }

        ASSERT (m_curTop);
        cTextLine* line = m_curTop->GetLine (m_view, m_pos.y);

        if (line && line->isFileLine() && line->isFoldEnd()) {
          //{{{  create a new blank line above the end-of-file
          cRange range (m_view, m_curFile, m_curTop, m_pos);
          line = GetDocument()->SplitLine (range);
          if (line) {
            m_pos.x = line->GetIndent(m_curTop);
            //SetCursorPos (m_pos);
            }
          //}}}
          }

        if (line) {
          if (m_recording) {
            if (!m_macro_textmode) {
              m_macroStr += (char) 255;
              m_macro_textmode = TRUE;
              }
            m_macroStr += (char) nChar;
            }

          cRange range (m_view, m_curFile, m_curTop, m_pos);
          if (overstrike)
            GetDocument()->ReplaceChar (range, nChar);
          else
            GetDocument()->InsertChar (range, nChar);
          MoveCursorBy (1, 0);
          }
        }
      }
    else
      Beep ("File is Protected");
    return;
    }

  CView::OnChar(nChar, nRepCnt, nFlags);
  }
//}}}

//{{{
void cFedView::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) {

  if (!OnAnyKeyDown (nChar, nRepCnt, nFlags))
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
  }
//}}}
//{{{
void cFedView::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) {

  if (!OnAnyKeyDown (nChar, nRepCnt, nFlags))
    CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
  }
//}}}
//{{{
void cFedView::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags) {

  if (!OnAnyKeyUp (nChar, nRepCnt, nFlags))
    CView::OnKeyUp(nChar, nRepCnt, nFlags);
  }
//}}}
//{{{
void cFedView::OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags) {

  if (!OnAnyKeyUp (nChar, nRepCnt, nFlags))
    CView::OnKeyUp(nChar, nRepCnt, nFlags);
  }
//}}}

//{{{
void cFedView::KeyProc (int token) {

  SetCurMargins();
  GetParentFrame()->SetMessageText ("");

  switch (token) {
    case k_CharLeft:             OnCharLeft();              break;
    case k_CharRight:            OnCharRight();             break;
    case k_CharLeftSelect:       OnCharLeftSelect();        break;
    case k_CharRightSelect:      OnCharRightSelect();       break;
    case k_CharDeleteLeft:       OnCharDeleteLeft();        break;
    case k_CharDeleteRight:      OnCharDeleteRight();       break;
    case k_CharToUppercase:      OnCharToUppercase();       break;
    case k_CharToLowercase:      OnCharToLowercase();       break;

    case k_WordLeft:             OnWordLeft();              break;
    case k_WordRight:            OnWordRight();             break;
    case k_WordSelect:           OnWordSelect();            break;
    case k_WordLeftSelect:       OnWordLeftSelect();        break;
    case k_WordRightSelect:      OnWordRightSelect();       break;
    case k_WordDelete:           OnWordDelete();            break;
    case k_WordDeleteLeft:       OnWordDeleteLeft();        break;
    case k_WordDeleteRight:      OnWordDeleteRight();       break;
    case k_WordToUppercase:      OnWordToUppercase();       break;
    case k_WordToLowercase:      OnWordToLowercase();       break;
    case k_WordSelectPart:       OnWordSelectPart();        break;

    case k_LineStart:            OnLineStart();             break;
    case k_LineEnd:              OnLineEnd();               break;
    case k_LineUp:               OnLineUp();                break;
    case k_LineDown:             OnLineDown();              break;
    case k_LineSelect:           OnLineSelect();            break;
    case k_LineStartSelect:      OnLineStartSelect();       break;
    case k_LineEndSelect:        OnLineEndSelect();         break;
    case k_LineUpSelect:         OnLineUpSelect();          break;
    case k_LineDownSelect:       OnLineDownSelect();        break;
    case k_LineDelete:           OnLineDelete();            break;
    case k_LineDeleteToStart:    OnLineDeleteToStart();     break;
    case k_LineDeleteToEnd:      OnLineDeleteToEnd();       break;
    case k_LineUndelete:         OnLineUndelete();          break;
    case k_LineDuplicate:        OnLineDuplicate();         break;
    case k_LineMove:             OnLineMove();              break;
    case k_LineIndentLess:       OnLineIndentLess();        break;
    case k_LineIndentMore:       OnLineIndentMore();        break;
    case k_LineSwapUp:           OnLineSwapUp();            break;
    case k_LineSwapDown:         OnLineSwapDown();          break;
    case k_LineMakeComment:      OnLineMakeComment();       break;
    case k_LineRemoveComment:    OnLineRemoveComment();     break;

    case k_ScrollLineUp:         OnScrollLineUp();          break;
    case k_ScrollLineDown:       OnScrollLineDown();        break;
    case k_ScrollPageUp:         OnScrollPageUp();          break;
    case k_ScrollPageDown:       OnScrollPageDown();        break;
    case k_ScrollPageUpSelect:   OnScrollPageUpSelect();    break;
    case k_ScrollPageDownSelect: OnScrollPageDownSelect();  break;

    case k_GotoFoldTop:          OnGotoFoldTop();           break;
    case k_GotoFoldBottom:       OnGotoFoldBottom();        break;
    case k_GotoFileTop:          OnGotoFileTop();           break;
    case k_GotoFileBottom:       OnGotoFileBottom();        break;
    case k_GotoProject:          OnGotoProject();           break;
    case k_GotoLineNumber:       OnGotoLineNumber();        break;
    case k_GotoPrevFile:         OnGotoPrevFile();          break;
    case k_GotoNextFile:         OnGotoNextFile();          break;
    case k_GotoBracketPair:      OnGotoBracketPair();       break;
    case k_GotoBracketPairSelect:OnGotoBracketPairSelect(); break;
    case k_GotoPrevFormfeed:     OnGotoPrevFormfeed();      break;
    case k_GotoNextFormfeed:     OnGotoNextFormfeed();      break;

    case k_FoldOpen:             OnFoldOpen();              break;
    case k_FoldClose:            OnFoldClose();             break;
    case k_FoldEnter:            OnFoldEnter();             break;
    case k_FoldExit:             OnFoldExit();              break;
    case k_FoldOpenAll:          OnFoldOpenAll();           break;
    case k_FoldCloseAll:         OnFoldCloseAll();          break;
    case k_FoldCreate:           OnFoldCreate();            break;
    case k_FoldRemove:           OnFoldRemove();            break;
    case k_FoldEnterOpened:      OnFoldEnterOpened();       break;
    case k_FoldOpenEntered:      OnFoldOpenEntered();       break;

    case k_FileOpen:             OnFileOpen();              break;
    case k_FileClose:            OnFileClose();             break;
    case k_FileCloseAll:         OnFileCloseAll();          break;
    case k_FileSave:             OnFileSave();              break;
    case k_FileSaveas:           OnFileSaveas();            break;
    case k_FileSaveAll:          OnFileSaveAll();           break;
    case k_FileInsertHere:       OnFileInsertHere();        break;
    case k_FileReload:           OnFileReload();            break;
    case k_FileToggleProtect:    OnFileToggleProtect();     break;
    case k_FileRenumber:         OnFileRenumber();          break;
    case k_FileShowEdits:        OnFileShowEdits();         break;
    case k_FileShowLineNumbers:  OnFileShowLineNumbers();   break;
    case k_FileOpenAssociate:    OnFileOpenAssociate();     break;
    case k_FileOpenInclude:      OnFileOpenInclude();       break;

    case k_Cut:                  OnCut();                   break;
    case k_Copy:                 OnCopy();                  break;
    case k_Paste:                OnPaste();                 break;
    case k_ClearPaste:           OnClearPaste();            break;
    case k_SelectAll:            OnSelectAll();             break;
    case k_UnSelect:             OnUnSelect();              break;
    case k_LineCut:              OnLineCut();               break;
    case k_LineCopy:             OnLineCopy();              break;
    case k_KeywordComplete:      OnKeywordComplete();       break;

    case k_FindUp:               OnFindUp();                break;
    case k_FindDown:             OnFindDown();              break;
    case k_Replace:              OnReplace();               break;
    case k_Substitute:           OnSubstitute();            break;
    case k_FindDialog:           OnFindDialog();            break;
    case k_FindReplaceDialog:    OnFindReplaceDialog();     break;

    case k_MacroDialog:          OnMacroDialog();           break;
    case k_MacroLearn:           OnMacroLearn();            break;
    case k_MacroCall:            OnMacroCall();             break;
    case k_MacroLoad:            OnMacroLoad();             break;
    case k_MacroSave:            OnMacroSave();             break;

    case k_Tab:                  OnTab();                   break;
    case k_Return:               OnReturn();                break;
    case k_FormFeed:             OnFormFeed();              break;
    case k_Undo:                 OnUndo();                  break;
    case k_Refresh:              OnRefresh();               break;
    case k_Abort:                OnAbort();                 break;
    case k_Help:                 OnHelp();                  break;
    case k_OverStrike:           OnOverStrike();            break;
    case k_ToggleTabs:           OnToggleTabs();            break;
    case k_AlignNextColumn:      OnAlignNextColumn();       break;
    case k_DealignNextColumn:    OnDealignNextColumn();     break;
    case k_UndoLine:             OnUndoLine();              break;
    case k_ShowDebug:            OnShowDebug();             break;
    }
  }
//}}}
//}}}
//{{{  token Char ops
//{{{
void cFedView::OnCharLeft() {

  Unselect();

  if (m_pos.x > 0)
    MoveCursorBy (-1, 0);
  else if (cOption::AllowLeftAtSol) {
    OnLineUp();
    OnLineEnd();
    }
  }
//}}}
//{{{
void cFedView::OnCharRight() {

  Unselect();
  if (m_pos.x + 1 < GetTotalSize().cx / m_cChar.cx)
    MoveCursorBy (1, 0);
  }
//}}}
//{{{
void cFedView::OnCharLeftSelect() {

  CPoint oldpos = m_pos;

  int x = GetScrollPosition().x;
  if (((m_pos.x - m_ScrollMargin.cx) * m_cChar.cx <= x) && (x > 0))
    OnScroll (MAKEWORD (SB_LINEUP, -1), 0);

  if (m_pos.x > 0)
    MoveCursorBy (-1, 0);

  if (m_pos.x != oldpos.x) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }

    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    }
  }
//}}}
//{{{
void cFedView::OnCharRightSelect() {

  CPoint oldpos = m_pos;

  int xright = GetScrollPosition().x + m_cWlen.cx;
  int xmax = GetTotalSize().cx;
  if (((m_pos.x + m_ScrollMargin.cx) * m_cChar.cx >= xright) && (xright < xmax))
    OnScroll (MAKEWORD (SB_LINEDOWN, -1), 0);

  if (m_pos.x < xmax / m_cChar.cx)
    MoveCursorBy (1, 0);

  if (m_pos.x != oldpos.x) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    }
  }
//}}}

//{{{
void cFedView::OnCharDeleteLeft() {

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      // delete the selected range
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }

    else {
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      if (line) {
        if (m_pos.x > 0) {
          cRange range (m_view, m_curFile, m_curTop, CPoint (m_pos.x - 1, m_pos.y), m_pos);
          GetDocument()->Delete (range);
          MoveCursorBy (-1, 0);
          }
        else if (!line->isFold() && cOption::AllowBackspAtSol) {
          // join current line to end of previous line
          line = line->PrevLine (m_view);
          if (line && !line->isFileLine() && !(line->isFold() && !line->isFold()->isOpen(m_view))) {
            int textlen = line->TextLen();
            int xpos = textlen + line->GetIndent (m_curTop);
            if (line->isFoldLine())
              xpos += 5;
            if (textlen > 0)
              xpos += 1;
            cRange range (m_view, m_curFile, m_curTop, CPoint (xpos, m_pos.y-1), m_pos);
            GetDocument()->Delete (range);
            SetCursorPos (xpos, m_pos.y - 1);
            }
          }
        }
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnCharDeleteRight() {

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      // delete the selected range
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else {
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      if (line) {
        int eol = line->TextLen(m_view) + line->GetIndent(m_curTop);
        if (m_pos.x >= eol) {
          if (!line->isFold() || line->isFold()->isOpen(m_view)) {
            cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (0, m_pos.y+1));
            GetDocument()->Delete (range);
            }
          }
        else {
          cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (m_pos.x+1, m_pos.y));
          GetDocument()->Delete (range);
          }
        }
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnCharToLowercase() {

  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      m_selectActive = false;
      GetDocument()->ToLower (m_selectRange);
      }
    else {
      cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (m_pos.x+1, m_pos.y));
      GetDocument()->ToLower (range);
      MoveCursorBy (1, 0);
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnCharToUppercase() {

  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      m_selectActive = false;
      GetDocument()->ToUpper (m_selectRange);
      //SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else {
      cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (m_pos.x+1, m_pos.y));
      GetDocument()->ToUpper (range);
      MoveCursorBy (1, 0);
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//}}}
//{{{  token Word ops
//{{{
bool cFedView::WordSelect (int selectmore) {

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  cTextFold* fold = line->isFold();

  line = line->GetTextLine (m_view);
  ASSERT (line);
  const char* text = line->Text();
  if (!text)
    return false;

  Unselect();

  CPoint pos1 = m_pos;
  if (fold && (line != fold))
    pos1.x += line->Indent() /*- fold->Indent()*/ - 5;

  int indent = line->GetIndent (m_curTop);
  if (line->isFold())
    indent += 5;

  if ((pos1.x >= indent) && (pos1.x <= indent + line->TextLen())) {
    CPoint pos2 = pos1;

    if (selectmore > 0) {       // all characters - bounded by spaces
      while ((pos1.x > indent) && (text[pos1.x-indent-1] > ' '))
        pos1.x--;
      while (text[pos2.x-indent] > ' ')
        pos2.x++;
      }
    else if (selectmore < 0) {  // alphnumeric only, with capital letter seperation
      while ((pos1.x > indent) && isalnum (text[pos1.x-indent-1]) && !isupper(text[pos1.x-indent]))
        pos1.x--;
      while ((pos1.x > indent) && isupper (text[pos1.x-indent-1]))
        pos1.x--;
      while (isupper(text[pos2.x-indent]))
        pos2.x++;
      while (isalnum (text[pos2.x-indent]) && !isupper(text[pos2.x-indent]))
        pos2.x++;
      }
    else {                      // alphnumeric and underscore
      while ((pos1.x > indent) && IsWordChar (text[pos1.x-indent-1]))
        pos1.x--;
      while (IsWordChar (text[pos2.x-indent]))
        pos2.x++;
      }

    if (pos1 != pos2) {
      if (fold && (line != fold)) {
        pos1.x -= line->Indent() /*- fold->Indent()*/ - 5;
        pos2.x -= line->Indent() /*- fold->Indent()*/ - 5;
        }
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, pos1, pos2);

      m_selectPos = pos1;
      m_pos = pos2;
      return true;
      }
    }

  return false;
  }
//}}}
//{{{
bool cFedView::WordLeft() {

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);

  const char* text = line->GetText (m_view);
  int indent = line->GetIndent (m_curTop);
  if (line->isFold())
    indent += 5;
  int eol = text ? (int)strlen(text) : 0;

  if (m_pos.x > 0) {
    if (!text || (m_pos.x <= indent)) {
      SetCursorPos (0, m_pos.y);
      return true;
      }
    else if (m_pos.x > indent + (int)(int)strlen(text)) {
      SetCursorPos (indent + (int)strlen(text), m_pos.y);
      return true;
      }
    else {
      const char* s = text + (m_pos.x - indent);
      //{{{  skip past spaces
      while ((s > text) && (*(s-1) <= ' '))
        s--;
      //}}}
      //{{{  skip until we meet an alphanumeric or space
      while ((s > text) && !IsWordChar (*(s-1)) && (*(s-1) > ' '))
        s--;
      //}}}
      if ((s > text) && (*(s-1) > ' ')) {
        //{{{  skip past alphanumeric
        while ((s > text) && (IsWordChar (*(s-1)) ))
          s--;
        //}}}
        }
      int x = indent + (int) (s - text);
      if (x != m_pos.x) {
        SetCursorPos (x, m_pos.y);
        return true;
        }
      }
    }
  return false;
  }
//}}}
//{{{
bool cFedView::WordRight() {

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);

  const char* text = line->GetText (m_view);
  int indent = line->GetIndent (m_curTop);
  if (line->isFold())
    indent += 5;

  if (m_pos.x < indent) {
    SetCursorPos (indent, m_pos.y);
    return true;
    }
  else if (text && (m_pos.x < indent + (int) strlen (text))) {
    const char* s = text + (m_pos.x - indent);
    //{{{  skip past alphanumeric
    while (IsWordChar (*s))
      s++;
    //}}}
    //{{{  skip until we meet an alphanumeric or space
    while (*s && !IsWordChar (*s) && (*s > ' '))
      s++;
    //}}}
    //{{{  skip past spaces
    while (*s && (*s <= ' '))
      s++;
    //}}}
    int x = indent + (int) (s - text);
    if (x != m_pos.x) {
      SetCursorPos (x, m_pos.y);
      return true;
      }
    }

  return false;
  }
//}}}
//{{{
bool cFedView::WordLeftSelect() {

  CPoint oldpos = m_pos;
  WordLeft();

  if (m_pos.x != oldpos.x) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    return true;
    }

  return false;
  }
//}}}
//{{{
bool cFedView::WordRightSelect() {

  CPoint oldpos = m_pos;
  WordRight();

  if (m_pos.x != oldpos.x) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    return true;
    }

  return false;
  }
//}}}
//{{{
void cFedView::OnWordLeft() {

  Unselect();
  WordLeft();
  }
//}}}
//{{{
void cFedView::OnWordRight() {

  Unselect();
  WordRight();
  }
//}}}
//{{{
void cFedView::OnWordSelect() {

  if (WordSelect(m_selectActive ? 1 : 0)) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    MoveCursorBy (0, 0);   // force an update
    }
  }
//}}}
//{{{
void cFedView::OnWordSelectPart() {

  if (WordSelect(-1)) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    MoveCursorBy (0, 0);   // force an update
    }
  }
//}}}
//{{{
void cFedView::OnWordLeftSelect() {

  if (WordLeftSelect()) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    }
  }
//}}}
//{{{
void cFedView::OnWordRightSelect() {

  if (WordRightSelect()) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    }
  }
//}}}

//{{{
void cFedView::OnWordDelete() {

  if (m_curFile && m_curFile->isEditable()) {
    if (!m_selectActive)
      WordSelect();

    if (m_selectActive) {
      //{{{  delete the selected range
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      //}}}
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnWordDeleteLeft() {

  if (m_curFile && m_curFile->isEditable()) {
    if (!m_selectActive)
      WordLeftSelect();

    if (m_selectActive) {
      //{{{  delete the selected range
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      //}}}
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnWordDeleteRight() {

  if (m_curFile && m_curFile->isEditable()) {
    if (!m_selectActive)
      WordRightSelect();

    if (m_selectActive) {
      //{{{  delete the selected range
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      //}}}
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnWordToUppercase() {

  if (m_curFile && m_curFile->isEditable()) {
    if (WordSelect()) {
      if (m_selectActive) {
        m_selectActive = false;
        GetDocument()->ToUpper (m_selectRange);
        }
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnWordToLowercase() {

  if (m_curFile && m_curFile->isEditable()) {
    if (WordSelect()) {
      if (m_selectActive) {
        m_selectActive = false;
        GetDocument()->ToLower (m_selectRange);
        }
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//}}}
//{{{  token Line ops
//{{{
void cFedView::OnLineStart()
{
  Unselect();
  int x = GetScrollPosition().x;
  if (x > 0)
    OnScroll (MAKEWORD (SB_TOP, -1), 0);

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  int xpos = line->GetIndent (m_curTop);

  if (line->isFileLine())
    SetCursorPos (0, m_pos.y);
  else if (line->isFoldLine()) {
    if (m_pos.x == xpos)
      SetCursorPos (xpos+5, m_pos.y);
    else if (m_pos.x == xpos + 5)
      SetCursorPos (0, m_pos.y);
    else
      SetCursorPos (xpos, m_pos.y);
    }
  else if (m_pos.x == xpos)
    SetCursorPos (0, m_pos.y);
  else
    SetCursorPos (xpos, m_pos.y);
}
//}}}
//{{{
void cFedView::OnLineEnd()
{
  Unselect();
  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);

  int xpos = line->TextLen(m_view) + line->GetIndent (m_curTop);
  if (line->isFileLine())
    xpos = 0;

  int xp = xpos + m_ScrollMargin.cx - (m_cWlen.cx / m_cChar.cx);
  if (xp < 0)
    xp = 0;
  else if (xp > GetTotalSize().cx / m_cChar.cx)
    xp = GetTotalSize().cx / m_cChar.cx;

  OnScroll (MAKEWORD (SB_THUMBTRACK, -1), xp * m_cChar.cx);

  SetCursorPos (xpos, m_pos.y);
}
//}}}
//{{{
void cFedView::OnLineUp()
{
  Unselect();
  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  m_lineNumber = line->LineNum();

  //int y = GetScrollPosition().y;
  //if (((m_pos.y - m_ScrollMargin.cy) * m_cChar.cy <= y) && (y > 0))
    //OnScroll (MAKEWORD (-1, SB_LINEUP), 0);

  if ((line->PrevLine(m_view) != m_curTop->isFile()) && (line != m_curTop))
    MoveCursorBy (0, -1);
}
//}}}
//{{{
void cFedView::OnLineDown()
{
  Unselect();
  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  m_lineNumber = line->LineNum();

  //int y = GetScrollPosition().y + m_cWlen.cy;
  //if (((m_pos.y + m_ScrollMargin.cy) * m_cChar.cy >= y) && (y < GetTotalSize().cy))
    //OnScroll (MAKEWORD (-1, SB_LINEDOWN), 0);

  cTextLine* foldend = m_curTop->FoldEnd();
  cTextLine* nextline = line->NextLine (m_view);
  if (nextline && (line != foldend)) {
    if ((nextline == foldend) && (nextline->isFileLine()))
      SetCursorPos (0, m_pos.y + 1);
    else if (!(m_curTop->isProj() && (nextline == foldend)))
      MoveCursorBy (0, 1);
    }
}
//}}}
//{{{
void cFedView::OnLineSelect()
{
  CPoint oldpos = m_pos;

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);

  int y = GetScrollPosition().y + m_cWlen.cy;
  if (((m_pos.y + m_ScrollMargin.cy) * m_cChar.cy >= y) && (y < GetTotalSize().cy))
    OnScroll (MAKEWORD (-1, SB_LINEDOWN), 0);

  cTextLine* foldend = m_curTop->FoldEnd();
  if ((line != foldend) && !(m_curTop->isFile() && (line->NextLine(m_view) == foldend)))
    MoveCursorBy (0, 1);

  if (m_pos.y != oldpos.y) {
    if (!m_selectActive) {
      m_selectPos.x = 0;
      m_selectPos.y = oldpos.y;
      }

    CPoint pos (0, m_pos.y);
    if (m_selectPos == pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, pos);
      }
    cRange range (m_view, m_curFile, m_curTop, oldpos, pos);
    OnUpdate (this, cRange::eChanged, &range);
    }
}
//}}}
//{{{
void cFedView::OnLineStartSelect()
{
  CPoint oldpos = m_pos;

  int x = GetScrollPosition().x;
  if (x > 0)
    OnScroll (MAKEWORD (SB_TOP, -1), 0);

  SetCursorPos (0, m_pos.y);

  if (m_pos.x != oldpos.x) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    }

}
//}}}
//{{{
void cFedView::OnLineEndSelect()
{
  CPoint oldpos = m_pos;

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);

  int xpos = line->TextLen(m_view) + line->GetIndent (m_curTop);

  int xp = xpos + m_ScrollMargin.cx - (m_cWlen.cx / m_cChar.cx);
  if (xp < 0)
    xp = 0;
  else if (xp > GetTotalSize().cx / m_cChar.cx)
    xp = GetTotalSize().cx / m_cChar.cx;

  OnScroll (MAKEWORD (SB_THUMBTRACK, -1), xp * m_cChar.cx);

  SetCursorPos (xpos, m_pos.y);

  if (m_pos.x != oldpos.x) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    OnUpdate (this, cRange::eChangedLine, &range);
    }

}
//}}}
//{{{
void cFedView::OnLineUpSelect()
{
  CPoint oldpos = m_pos;

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  m_lineNumber = line->LineNum();

  int y = GetScrollPosition().y;
  if (((m_pos.y - m_ScrollMargin.cy) * m_cChar.cy <= y) && (y > 0))
    OnScroll (MAKEWORD (-1, SB_LINEUP), 0);

  if ((line->PrevLine(m_view) != m_curTop->isFile()) && (line != m_curTop))
    MoveCursorBy (0, -1);

  if (m_pos.y != oldpos.y) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    cRange range (m_view, m_curFile, m_curTop, oldpos, m_pos);
    OnUpdate (this, cRange::eChanged, &range);
    }
}
//}}}
//{{{
void cFedView::OnLineDownSelect()
{
  CPoint oldpos = m_pos;

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  m_lineNumber = line->LineNum();

  int y = GetScrollPosition().y + m_cWlen.cy;
  if (((m_pos.y + m_ScrollMargin.cy) * m_cChar.cy >= y) && (y < GetTotalSize().cy))
    OnScroll (MAKEWORD (-1, SB_LINEDOWN), 0);

  cTextLine* foldend = m_curTop->FoldEnd();
  if ((line != foldend) && !(m_curTop->isFile() && (line->NextLine(m_view) == foldend)))
    MoveCursorBy (0, 1);

  if (m_pos.y != oldpos.y) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    cRange range (m_view, m_curFile, m_curTop, oldpos, m_pos);
    OnUpdate (this, cRange::eChanged, &range);
    }
}
//}}}

//{{{
void cFedView::OnLineDelete()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      // extend selection to whole lines
      m_selectRange.ExtendToWholeLines();
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else {
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      ASSERT (line);
      if (!line->isFileLine()) {
        cTextFold* fold = line->isFold();
        cRange range (m_view, m_curFile, m_curTop, m_pos);
        if (fold && !fold->FoldEnd()) {
          range.SetRange (CPoint (0, m_pos.y), CPoint (0, m_pos.y + 1));
          GetDocument()->Delete (range);
          }
        else if (fold && fold->isOpen (m_view)) {
          range.SetRange (CPoint (0, m_pos.y), CPoint (9999, m_pos.y));
          GetDocument()->Delete (range);
          }
        else {
          range.SetRange (CPoint (0, m_pos.y), CPoint (0, m_pos.y + 1));
          GetDocument()->DeleteLine (range);
          }
        }
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineDeleteToStart()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else if (m_pos.x > 0) {
      cRange range (m_view, m_curFile, m_curTop,
                    CPoint (0, m_pos.y), CPoint (m_pos.x, m_pos.y));
                    //CPoint (0, m_pos.y), CPoint (m_pos.x - 1, m_pos.y));
      GetDocument()->Delete (range);
      SetCursorPos (0, m_pos.y);
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineDeleteToEnd()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else {
      cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (9999, m_pos.y));
      GetDocument()->Delete (range);
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineUndelete()
{
  if (m_curFile && m_curFile->isEditable()) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    GetDocument()->UndeleteLine (range);
    //MoveCursorBy (0, 1);
    MoveCursorBy (0, 0);
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineDuplicate()
{
  if (m_curFile && m_curFile->isEditable()) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    GetDocument()->Duplicate (range);
    OnLineDown();
    //MoveCursorBy (0, 1);
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineMove()
{
  if (m_curFile && m_curFile->isEditable()) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    GetDocument()->MoveLine (range);
    //MoveCursorBy (0, 1);
    MoveCursorBy (0, 0);
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineIndentLess()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      GetDocument()->IndentBy (m_selectRange, -1);
      }
    else {
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      ASSERT (line);
      if (!line->isFileLine()) {
        cRange range (m_view, m_curFile, m_curTop, m_pos);
        GetDocument()->IndentBy (range, -1);
        }
      }
    }
  else
    Beep (szFileProtected);

}
//}}}
//{{{
void cFedView::OnLineIndentMore()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      GetDocument()->IndentBy (m_selectRange, 1);
      }
    else {
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      ASSERT (line);
      if (!line->isFileLine()) {
        cRange range (m_view, m_curFile, m_curTop, m_pos);
        GetDocument()->IndentBy (range, 1);
        }
      }
    }
  else
    Beep (szFileProtected);

}
//}}}
//{{{
void cFedView::OnLineSwapUp()
{
  ASSERT (m_curTop);

  if (m_curFile && (m_curFile->isEditable() || m_curTop->isProj())) {
    Unselect();

    cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (0, m_pos.y-1));
    if (GetDocument()->SwapLines (range, true))
      OnLineUp();
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineSwapDown()
{
  ASSERT (m_curTop);

  if (m_curFile && (m_curFile->isEditable() || m_curTop->isProj())) {
    Unselect();

    cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (0, m_pos.y+1));
    if (GetDocument()->SwapLines (range, false))
      OnLineDown();
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineMakeComment()
{
  ASSERT (m_curTop);
  ASSERT (m_curFile);

  if (m_curFile->isEditable()) {
    if (m_selectActive) {
      // extend selection to whole lines
      //m_selectRange.ExtendToWholeLines();
      m_selectActive = false;
      GetDocument()->MakeComment (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else {
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      ASSERT (line);
      cRange range (m_view, m_curFile, m_curTop, m_pos);
      GetDocument()->MakeComment (range);
      OnLineDown();
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnLineRemoveComment()
{
  ASSERT (m_curTop);
  ASSERT (m_curFile);

  if (m_curFile->isEditable()) {
    if (m_selectActive) {
      // extend selection to whole lines
      //m_selectRange.ExtendToWholeLines();
      m_selectActive = false;
      GetDocument()->RemoveComment (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else {
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      ASSERT (line);
      cRange range (m_view, m_curFile, m_curTop, m_pos);
      GetDocument()->RemoveComment (range);
      OnLineDown();
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//}}}
//{{{  token Scroll
//{{{
void cFedView::OnScrollLineUp()
{
  Unselect();
  OnScroll (MAKEWORD (-1, SB_LINEUP), 0);

  int mapmode;
  CSize total, page, line;
  GetDeviceScrollSizes (mapmode, total, page, line);
  CPoint pos = m_pos;
  pos.y -= 1;

  if (pos.y <= 0) {
    if (m_curTop->isFile())
      return;
    pos.y = 0;
    }

  SetCursorPos (pos);
}
//}}}
//{{{
void cFedView::OnScrollLineDown()
{
  Unselect();
  OnScroll (MAKEWORD (-1, SB_LINEDOWN), 0);

  int mapmode;
  CSize total, page, line;
  GetDeviceScrollSizes (mapmode, total, page, line);
  int maxy = (total.cy - 1) / m_cChar.cy;

  CPoint pos = m_pos;
  pos.y += 1;

  if (pos.y >= maxy) {
    if (m_curTop->isFile())
      return;
    pos.y = maxy;
    }

  SetCursorPos (pos);
}
//}}}
//{{{
void cFedView::OnScrollPageUp()
{
  Unselect();
  OnScroll (MAKEWORD (-1, SB_PAGEUP), 0);

  int mapmode;
  CSize total, page, line;
  GetDeviceScrollSizes (mapmode, total, page, line);
  CPoint pos = m_pos;
  pos.y -= page.cy / m_cChar.cy;

  if (pos.y <= 0) {
    if (m_curTop->isFile()) {
      pos.x = 0;
      pos.y = 1;
      }
    else
      pos.y = 0;
    }

  SetCursorPos (pos);
}
//}}}
//{{{
void cFedView::OnScrollPageDown()
{
  Unselect();
  OnScroll (MAKEWORD (-1, SB_PAGEDOWN), 0);

  int mapmode;
  CSize total, page, line;
  GetDeviceScrollSizes (mapmode, total, page, line);
  int maxy = (total.cy - 1) / m_cChar.cy;

  CPoint pos = m_pos;
  pos.y += page.cy / m_cChar.cy;

  if (pos.y >= maxy) {
    pos.y = maxy;
    if (m_curTop->isFile())
      pos.x = 0;
    }

  SetCursorPos (pos);
}
//}}}
//{{{
void cFedView::OnScrollPageUpSelect()
{
  CPoint oldpos = m_pos;

  OnScroll (MAKEWORD (-1, SB_PAGEUP), 0);

  int mapmode;
  CSize total, page, line;
  GetDeviceScrollSizes (mapmode, total, page, line);
  CPoint pos = m_pos;
  pos.y -= page.cy / m_cChar.cy;

  if (pos.y <= 0) {
    if (m_curTop->isFile()) {
      pos.x = 0;
      pos.y = 1;
      }
    else
      pos.y = 0;
    }

  SetCursorPos (pos);

  if (m_pos.y != oldpos.y) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    cRange range (m_view, m_curFile, m_curTop, oldpos, m_pos);
    OnUpdate (this, cRange::eChanged, &range);
    }
}
//}}}
//{{{
void cFedView::OnScrollPageDownSelect()
{
  CPoint oldpos = m_pos;

  OnScroll (MAKEWORD (-1, SB_PAGEDOWN), 0);

  int mapmode;
  CSize total, page, line;
  GetDeviceScrollSizes (mapmode, total, page, line);
  int maxy = (total.cy - 1) / m_cChar.cy;

  CPoint pos = m_pos;
  pos.y += page.cy / m_cChar.cy;

  if (pos.y >= maxy) {
    pos.y = maxy;
    if (m_curTop->isFile())
      pos.x = 0;
    }

  SetCursorPos (pos);

  if (m_pos.y != oldpos.y) {
    if (!m_selectActive)
      m_selectPos = oldpos;

    if (m_selectPos == m_pos)
      m_selectActive = false;
    else if (!m_curTop->isProj()) {
      m_selectActive = true;
      m_selectRange.Set (m_view, m_curFile, m_curTop, m_selectPos, m_pos);
      }
    cRange range (m_view, m_curFile, m_curTop, oldpos, m_pos);
    OnUpdate (this, cRange::eChanged, &range);
    }
}
//}}}
//}}}
//{{{  token Goto
//{{{
void cFedView::OnGotoFoldTop()
{
  cTextFold* fold;
  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  //if (line->isFold() && line->isFold()->isOpen(m_view))
  //  fold = line->isFold();
  //else
    fold = line->ParentFold();
  ASSERT (fold);

  Unselect();

  DisplayAtLine (fold);
  Invalidate();
}
//}}}
//{{{
void cFedView::OnGotoFoldBottom()
{
  cTextFold* fold;
  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  if (line->isFold() && line->isFold()->isOpen(m_view))
    fold = line->isFold();
  else
    fold = line->ParentFold();
  ASSERT (fold);

  Unselect();

  ASSERT (fold->FoldEnd());
  DisplayAtLine (fold->FoldEnd());
  Invalidate();
}
//}}}
//{{{
void cFedView::OnGotoFileTop()
{
  Unselect();

  ASSERT (m_curFile);
  while (m_curTop &&(m_curTop != m_curFile)) {
    m_curTop->Close (m_view);
    m_curTop = m_curTop->ParentFold();
    }
  ASSERT (m_curTop);
  DisplayAtLine (m_curTop);
  Invalidate();
}
//}}}
//{{{
void cFedView::OnGotoFileBottom()
{
  Unselect();

  ASSERT (m_curFile);
  while (m_curTop &&(m_curTop != m_curFile)) {
    m_curTop->Close (m_view);
    m_curTop = m_curTop->ParentFold();
    }
  ASSERT (m_curTop);
  ASSERT (m_curTop->FoldEnd());
  DisplayAtLine (m_curTop->FoldEnd());
  Invalidate();
}
//}}}
//{{{
void cFedView::OnGotoProject()
{
  ASSERT(m_curFile);
  if (m_curFile->ParentFold()) {
    cTextProj* project = m_curFile->ParentFold()->isProj();
    if (project) {
      cTextFile* oldfile = m_curFile;
      SwitchToFile (project);
      //force cursor to current closed fold
      DisplayAtLine (oldfile);
      }
    }
}
//}}}
//{{{
void cFedView::OnGotoLineNumber()
{
  if (!m_curFile) return;
  //CPoint pos = GetScrollPosition();
  //int yfirst = (pos.y - m_cChar.cy + 1) / m_cChar.cy;  // adjust so a partial line will print
  //int ylast  = (pos.y + m_cWlen.cy) / m_cChar.cy;
  //cTextLine* line = 0;
  ////CString str;
  //int linenum = 0;

  cGotoDialog dlg (this);
  switch (dlg.DoModal()) {
    case IDOK:
      GotoLineNumber (dlg.m_lineNumber);
      //line = m_curFile->FindLine (dlg.m_lineNumber);
      //if (line)
        //linenum = line->LineNum();
      ////str.Format ("Goto Line %d - %d", dlg.m_lineNumber, linenum);
      ////GetParentFrame()->SetMessageText (str);

      //if (line) {
        ////if (line->OpenToTop (m_view, m_curTop)) {
        ////  SetupScrollBars();
        ////  Invalidate();
        ////  }
        //DisplayAtLine (line);
        //m_pos.x = line->GetIndent (m_curTop);
        //SetCursorPos (m_pos);
        //}
      break;
    case IDCANCEL:
      //GetParentFrame()->SetMessageText ("Goto Line - CANCEL");
      break;
    }
}
//}}}
//{{{
void cFedView::OnGotoPrevFile()
{
  Unselect();
  ASSERT (m_curFile);

  if (m_curFile->PrevFile())
    SwitchToFile (m_curFile->PrevFile());
  else if (m_curFile->ParentFold() && m_curFile->ParentFold()->isProj())
    SwitchToFile (m_curFile->ParentFold()->isProj()->LastFile());
}
//}}}
//{{{
void cFedView::OnGotoNextFile()
{
  Unselect();
  ASSERT (m_curFile);

  if (m_curFile->NextFile())
    SwitchToFile (m_curFile->NextFile());
  else if (m_curFile->ParentFold() && m_curFile->ParentFold()->isProj())
    SwitchToFile (m_curFile->ParentFold()->isProj()->FirstFile());
}
//}}}
//{{{
void cFedView::OnGotoBracketPair() //
{
  Beep ("GotoBracketPair - Command not available");
}
//}}}
//{{{
void cFedView::OnGotoPrevFormfeed() //
{
  Beep ("GotoPrevFormfeed - Command not available");
}
//}}}
//{{{
void cFedView::OnGotoNextFormfeed() //
{
  Beep ("GotoNextFormfeed - Command not available");
}
//}}}
//{{{
void cFedView::OnGotoBracketPairSelect() //
{
  Beep ("GotoBracketPairSelect - Command not available");
}
//}}}

//{{{
void cFedView::OnGotoError (const char* str)
{
  if (!str)
    return;

  //{{{  parse filename
  //const char* path = m_curFile->Pathname();
  //int pathlen = path ? strlen (path) : 0;

  //const char* s = str;
  //while (*s > ' ')
    //s++;
  //int slen = (int) (s - str);

  //if ((pathlen != slen) || (memicmp (str, path, pathlen) != 0)) {
    //// reference to another file - just show the message
    //Beep (str);
    //return;
    //}
  //str = s;
  //while (*str == ' ')
    //str++;
  //}}}
  //{{{  parse filename
  const char* p = m_curFile->Pathname();
  const char* s = str;
  bool match = true;
  while (match && *p && *s)
    {
    char sch = tolower (*s++);
    char pch = tolower (*p++);
    if (sch == '/')   // allow Unix-like filenames to match
      match = (pch == '\\');
    else
      match = (sch == pch);
    }

  if (*p || (*s != ' '))
    { // reference to another file - just show the message
    Beep (str);
    return;
    }

  str = s;
  while (*str == ' ')
    str++;
  //}}}
  //{{{  parse linenum
  int linenum = 0;
  while ((*str >= '0') && (*str <= '9'))
    linenum = linenum * 10 + *str++ - '0';
  while (*str == ' ')
    str++;
  //}}}
  //{{{  parse charnum
  int charnum = 0;
  while ((*str >= '0') && (*str <= '9'))
    charnum = charnum * 10 + *str++ - '0';
  while (*str == ' ')
    str++;
  //}}}

  GetParentFrame()->SetMessageText (str);

  cTextLine* line = m_curFile->FindLine (linenum);
  if (line) {
    DisplayAtLine (line);
    if (charnum > 0)
      m_pos.x = charnum;
    else
      m_pos.x = line->GetIndent (m_curTop);
    SetCursorPos (m_pos);
    }
}
//}}}

//}}}
//{{{  token Fold ops
//{{{
bool cFedView::FoldEnter (cTextFold* fold, long& yscroll)
{
  if (fold)
    {
    //{{{  find current fold_level
    int fold_level = 0;
    cTextFold* cur = fold;
    while (cur && (cur != m_curTop)) {
      fold_level += 1;
      cur = cur->ParentFold();
      }
    //}}}

    fold->Open(m_view);
    fold->SetAbove (m_pos.x, m_pos.y, yscroll, fold_level);
    fold->GetBelow (m_pos.x, m_pos.y, yscroll);

    if (fold->isFile()) {
      m_curFile = fold->isFile();
      m_curTop = fold->isFile()->GetBelowTop();
      }
    else
      m_curTop = fold;

    return true;
    }
  return false;
}
//}}}
//{{{
bool cFedView::FoldExit (bool leave_open, long& yscroll)
{
  ASSERT (m_curTop);

  if (m_curTop->ParentFold()) {
    cTextFold* oldtop = m_curTop;

    if (m_curTop->isFile())
      m_curTop->isFile()->SetBelowTop (m_curTop);

    if (!leave_open || m_curTop->isFile())
      m_curTop->Close(m_view);
    int fold_level;
    m_curTop->SetBelow (m_pos.x, m_pos.y, yscroll);
    m_curTop->GetAbove (m_pos.x, m_pos.y, yscroll, fold_level);
    m_curTop = m_curTop->ParentFold();
    while ((--fold_level > 0) && m_curTop->ParentFold() && !m_curTop->isFile())
      m_curTop = m_curTop->ParentFold();
    m_curTop->Open(m_view);    // should already be open!

    //force cursor to current closed fold
    DisplayAtLine (oldtop);

    if (m_curTop->isFile())
      m_curFile = m_curTop->isFile();
    return true;
    }
  return false;
}
//}}}

//{{{
void cFedView::OnFoldOpen (cTextFold* fold)
{
  if (!fold) {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    fold = line->isFold();
    }

  Unselect();

  if (fold && !fold->isOpen(m_view)) {
    if (fold->isFile())
      OnFoldEnter (fold);
    else {
      fold->Open(m_view);
      OnUpdate(this, 0L, NULL);
      }
    }
}
//}}}
//{{{
void cFedView::OnFoldClose (cTextFold* fold)
{
//if (m_curBuffer)
  //OnAbort();
//else
  {
  if (!fold) {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    if (line->isFold() && line->isFold()->isOpen(m_view))
      fold = line->isFold();
    else
      fold = line->ParentFold();
    }

  Unselect();

  if (fold && fold->isOpen(m_view)) {
    if (fold->isFile() || (fold == m_curTop))
      OnFoldExit();
    else {
      fold->Close(m_view);
      DisplayAtLine (fold);
      OnUpdate(this, 0L, NULL);
      }
    }
  }
}
//}}}
//{{{
void cFedView::OnFoldEnter (cTextFold* fold)
{
  if (!fold)
    {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    fold = line->isFold();
    }

  Unselect();

  if (fold)
    {
    if (fold->isFile())
      SwitchToFile (fold->isFile());
    else
      {
      long yscroll = GetScrollPosition().y / m_cChar.cy;

      if (FoldEnter (fold, yscroll))
        {
        SetScrollPos (SB_VERT, yscroll * m_cChar.cy);

        OnUpdate(this, 0L, NULL);
        if (!m_curTop->isProj())
          ShowFilename();
        MoveCursorBy (0, 0);  // dummy to redisplay caret
        }
      }
    }
}
//}}}
//{{{
void cFedView::OnFoldExit (bool leave_open)
{
//if (m_curBuffer)
  //OnAbort();
//else
  {
  Unselect();
  long yscroll = GetScrollPosition(). y / m_cChar.cy;

  if (FoldExit (leave_open, yscroll))
    {
    if (m_curTop->isProj())
      ShowFilename();

    SetScrollPos (SB_VERT, yscroll * m_cChar.cy);
    OnUpdate(this, 0L, NULL);
    MoveCursorBy (0, 0);  // dummy to redisplay caret
    }
  }
}
//}}}
//{{{
void cFedView::OnFoldOpenAll()
{
  ASSERT (m_curTop);
  Unselect();
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);

  m_curTop->OpenAll (m_view);

  SetupScrollBars();
  DisplayAtLine (line);

  OnUpdate (NULL, 0, NULL);
}
//}}}
//{{{
void cFedView::OnFoldCloseAll()
{
  ASSERT (m_curTop);
  Unselect();
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);

  m_curTop->CloseAll (m_view);

  while (line && (line->ParentFold() != m_curTop))
    line = line->ParentFold();
  SetupScrollBars();
  DisplayAtLine (line);

  OnUpdate (NULL, 0, NULL);
}
//}}}
//{{{
void cFedView::OnFoldEnterOpened()
{
  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  cTextFold* fold;
  if (line->isFold() && line->isFold()->isOpen(m_view))
    fold = line->isFold();
  else
    fold = line->ParentFold();

  Unselect();

  if (fold && (fold != m_curTop))
    OnFoldEnter (fold);
}
//}}}
//{{{
void cFedView::OnFoldOpenEntered()
{
  ASSERT (m_curTop);
  if (!m_curTop->isFile())
    OnFoldExit (true);
}
//}}}

//{{{
void cFedView::OnFoldCreate()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    cFedDoc* pDoc = GetDocument();
    cTextFold* fold = 0;
    if (m_selectActive) {
      m_selectActive = false;
      fold = pDoc->FoldCreate (m_selectRange);
      if (fold) {
        int xpos = 5 + fold->GetIndent (m_curTop);
        SetCursorPos (xpos, m_selectRange.Ypos());
        }
      }
    else if (m_pos.y > 0) {
      cRange range (m_view, m_curFile, m_curTop, m_pos);
      fold = pDoc->FoldCreate (range);
      if (fold && fold->FoldEnd()) {
        int xpos = 5 + fold->GetIndent (m_curTop);
        SetCursorPos (xpos, m_pos.y);
        }
      else
        SetCursorPos (m_pos.x, m_pos.y + 1);
      }
    if (fold && fold->FoldEnd()) {
      if (fold->cTextLine::Text()) {
        fold->Close(m_view);
        SetupScrollBars();
        Invalidate();
        }
      DisplayAtLine (fold);
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnFoldRemove()
{
  Unselect();
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    cTextFold* fold = line->isFold();

    if (fold && (fold != m_curTop)) {
      cRange range (m_view, m_curFile, m_curTop,
                    CPoint (0, m_pos.y), CPoint (9999, m_pos.y));
      GetDocument()->FoldRemove (range);
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//}}}
//{{{  token File ops
//{{{
void cFedView::OnFileNew()
{
  cTextFile* after = CurFile();
  const char* ext = after ? after->Pathname() : 0;
  if (ext)
    ext = strrchr (ext, '.');
  if (ext && (_stricmp (ext, ".fed") == 0))
    ext = 0;

  cTextFile* newfile = GetDocument()->DoFileNew (ext, after);

  if (newfile) {
    if (newfile->ParentFold() == m_curFile)
      DisplayAtLine (newfile);
    else
      SwitchToFile (newfile);
    }
  ShowFilename();
}
//}}}
//{{{
void cFedView::OnFileOpen()
{
  ClearCurMargins();
  cTextFile* file = m_curFile;
  if (file && file->isProj()) {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    file = line->isProj() ? 0 : line->isFile();
    }

  //{{{  do open file dialog
  const int kFileNameBufLen = 512;
  CString fileName;
  LPTSTR fileNameBuf = fileName.GetBuffer (kFileNameBufLen);
  if (file)
    strcpy (fileNameBuf, file->Pathname());
  else
    *fileNameBuf = 0;

  const char* defaultExt = strrchr (fileNameBuf, '.');
  if (defaultExt)
    defaultExt++;
  bool isProject = defaultExt ? (_stricmp (defaultExt, "fed") == 0) : false;


  CFileDialog dlgFile (TRUE, defaultExt, fileNameBuf);


  dlgFile.m_ofn.Flags |= OFN_ALLOWMULTISELECT |  // multiple files
                         OFN_EXPLORER |          // + null char separated
                         OFN_CREATEPROMPT;       // prompt if user creates a new filename

  CString title;
  VERIFY (title.LoadString (AFX_IDS_OPENFILE));

  CString strFilter;
  dlgFile.m_ofn.nFilterIndex = GetFilters (strFilter, fileNameBuf, defaultExt);
  dlgFile.m_ofn.lpstrFilter  = strFilter;
  dlgFile.m_ofn.lpstrTitle   = title;
  dlgFile.m_ofn.lpstrFile    = fileNameBuf;
  dlgFile.m_ofn.nMaxFile     = kFileNameBufLen;

  if (dlgFile.DoModal() != IDOK)
    return;
  //}}}

  cTextFile* firstFile = 0;
  cTextFile* after = CurFile();

  POSITION pos = dlgFile.GetStartPosition();
  while (pos != NULL)
    {
    after = GetDocument()->DoFileOpen (dlgFile.GetNextPathName (pos), after, m_view);

    if (!firstFile)
      firstFile = after;
    }

  if (firstFile) {
    if (firstFile->ParentFold() == m_curFile)
      DisplayAtLine (firstFile);
    else
      SwitchToFile (firstFile);
    }
  ShowFilename();
}
//}}}
//{{{
void cFedView::OnFileClose()
{
  ASSERT (m_curFile);
  if (m_curFile->isProj() && m_curFile->isEmpty())
    GetDocument()->RemoveFile (m_curFile);

  else if (m_curFile->isProj()) {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    if ((line != m_curFile) && (line != m_curFile->FoldEnd())) {
      cTextFile* file = line->isFile();
      ASSERT (file);
      GetDocument()->RemoveFile (file);
      }
    }

  else {
    cTextFile* nextfile = 0;

    if (m_curFile->PrevFile())
      nextfile = m_curFile->PrevFile();
    else if (m_curFile->NextFile())
      nextfile = m_curFile->NextFile();
    else if (m_curFile->ParentFold())
      nextfile = m_curFile->ParentFold()->isFile();

    if (nextfile && GetDocument()->RemoveFile (m_curFile)) {
      m_curFile = 0;
      SwitchToFile (nextfile);
      }
    }
}
//}}}
//{{{
void cFedView::OnFileCloseAll()
{
  AfxGetMainWnd()->PostMessage (WM_CLOSE);
}
//}}}
//{{{
void cFedView::OnFileSave()
{
  ASSERT (m_curFile);
  cTextFile* file = m_curFile;
  if (m_curFile->isProj()) {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    if (line != m_curFile->FoldEnd())
      file = line->isFile();
    ASSERT (file);
    }
  if (file->isEmpty())
    return;

  if (file->isNewFile())
    OnFileSaveas();
  else
    GetDocument()->DoFileSave (file);
}
//}}}
//{{{
void cFedView::OnFileSaveas()
{
  ASSERT (m_curFile);
  cTextFile* file = m_curFile;
  if (m_curFile->isProj())
    {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    if (line != m_curFile->FoldEnd())
      file = line->isFile();
    }
  ASSERT (file);
  //if (file->isEmpty())
    //return;

  CString newName;
  if (file->Pathname())
    newName = file->Pathname();

  if (DoPromptFileName(newName, 0, FALSE))
    {
    //if (!strchr (newName, '.')) {
      //const char* s = strrchr (m_curFile->cTextLine::Text(), '.');
      //if (s)
        //newName += s;
      //}

    CString str;
    str.Format ("Fed - [%s...]", newName);
    AfxGetMainWnd()->SetWindowText (str);

    GetDocument()->DoFileSaveas (file, newName);

    ShowFilename();
    Invalidate();
    }
}
//}}}
//{{{
void cFedView::OnFileSaveAll()
{
  GetDocument()->DoFileSaveAll();
}
//}}}
//{{{
void cFedView::OnFileSaveProject()
{
  ASSERT (m_curFile);
  if (m_curFile->isProj())
    {
    if (m_curFile->isNewFile())
      {
      CString newName;
      if (m_curFile->Pathname())
        newName = m_curFile->Pathname();

      if (DoPromptFileName (newName, "fed", FALSE))
        GetDocument()->DoFileSaveas (m_curFile, newName);
      }
    else
      GetDocument()->DoFileSave (m_curFile);
    }
}
//}}}
//{{{
void cFedView::OnFileInsertHere()  //
{
  Beep ("FileInsertHere - Command not available");
}
//}}}
//{{{
void cFedView::OnFileReload()
{
  Unselect();
  ASSERT (m_curFile);
  cTextFile* file = m_curFile;
  if (m_curFile->isProj()) {
    Beep ("FileReload - Command not available for Projects");
    return;
    }

  if (file->isModified()) {
    if (MessageBox ("File modified. Ok to reload File ?", "File Reload", MB_OKCANCEL) != IDOK)
      return;
    }

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  int toplinenum = m_curFile->GetNearestLinenum (m_curTop);
  int curlinenum = m_curFile->GetNearestLinenum (line);

  CPoint pos = GetScrollPosition();
  //pos.y /= m_cChar.cy;

  file->SetBelowTop (m_curTop);
  file->SetBelow (m_pos.x, m_pos.y, pos.y / m_cChar.cy);

  GetDocument()->ReloadFile (m_curFile);

  //{{{  try to set curTop to match where it was before
  if (toplinenum <= 0)
    m_curTop = file;
  else {
    line = file->FindLine (toplinenum);
    if (line && line->isFold())
      m_curTop = line->isFold();
    else if (line && line->ParentFold())
      m_curTop = line->ParentFold();
    else
      m_curTop = file;
    }
  //}}}
  //{{{  try to set curLine to match where it was before
  if (curlinenum < 0)
    line = file->FoldEnd();
  else if (curlinenum == 0)
    line = file->Next();
  else {
    line = file->FindLine (curlinenum);
    if (!line)
      line = file->Next();
    }
  //}}}
  if (line == m_curTop)
    m_curTop->Open (m_view);
  else
    //{{{  ensure curLine is within fold curTop
    {
    while (m_curTop && (m_curTop != file) && !line->isWithin (m_curTop))
      m_curTop = m_curTop->ParentFold();
    }
    //}}}
  line->OpenToTop (m_view, file);

  SetupScrollBars();
  Invalidate();
  DisplayAtLine (line);
  }
//}}}
//{{{
void cFedView::OnFileToggleProtect()
{
  cTextFile* file = CurFile();
  if (file && file->ToggleProtect())
    Invalidate();
}
//}}}
//{{{
void cFedView::OnFileRenumber()
{
  GetDocument()->RenumberFile (m_curFile);
}
//}}}
//{{{
void cFedView::OnFileShowEdits()   //
{
  Beep ("FileShowEdits - Command not available");
}
//}}}
//{{{
void cFedView::OnFileShowLineNumbers()
{
  m_ShowLineNumbers = !m_ShowLineNumbers;

  m_xStart = m_Margin.x;
  if (m_ShowLineNumbers)
    m_xStart += m_Margin.x + 5 * m_cChar.cx;

  OnUpdate (NULL, 0, NULL);
}
//}}}
//{{{
void cFedView::OnFileShowFolds()
{
  m_ShowFolds = !m_ShowFolds;

  Invalidate();
}
//}}}
//{{{
void cFedView::OnFileOpenAssociate()
{
  ClearCurMargins();
  cTextFile* file = m_curFile;
  if (file && file->isProj()) {
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    file = line->isProj() ? 0 : line->isFile();
    }
  if (!file || !file->Pathname() || !file->Language())
    return;

  CString newName (file->Pathname());
  int ext = newName.ReverseFind ('.');
  if (ext++ < 0)
    return;      // no extension !!

  const char* curext = file->Language()->find_extension (((const char*) newName) + ext);
  if (!curext)
    return;   // this shouldn't happen !!

  CFileFind filefind;
  const char* newext = curext + 1;
  while (true) {
    while (*newext > ' ') newext++;
    while (*newext == ' ') newext++;
    if (!*newext)
      newext = file->Language()->Extensions();   // jump to first extension
    if (newext == curext)
      return;    // unable to find any associate file

    newName.Delete (ext, (int)strlen(((const char*) newName) + ext));
    while (*newext > ' ')
      newName += *newext++;
    if (GetDocument()->FindFile (newName) || (filefind.FindFile (newName)))
      break;  // and open the file
    }

  cTextFile* after = CurFile();

  CString str;
  str.Format ("Fed - [%s...]", newName);
  AfxGetMainWnd()->SetWindowText (str);

  cTextFile* newfile = GetDocument()->DoFileOpen (newName, after, m_view);

  if (newfile) {
    if (newfile->ParentFold() == m_curFile)
      DisplayAtLine (newfile);
    else
      SwitchToFile (newfile);
    }
  ShowFilename();
}
//}}}
//{{{
void cFedView::OnFileOpenInclude()
{
  ClearCurMargins();
  if (m_curFile && m_curFile->isProj())
    return;

  ASSERT (m_curTop);
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  ASSERT (line);
  const char* text = line->GetText (m_view);
  if (!text || !*text)
    return;

  const char* s = strchr (text, 0);
  while ((s > text) && (*s <= ' '))
    s--;
  char delim = '\0';
  if ((*s == '\'') || (*s == '"'))
    delim = *s;
  else if (*s == '>') {
    delim = '<';
    s;
    }
  int len = 0;
  while ((s > text) && (*(s-1) != delim)) {
    s--;
    len++;
    }

  if (len > 0) {
    CString filename (s, len);

    ASSERT (m_curFile);
    CString newName (m_curFile->Pathname());
    int dir = newName.ReverseFind ('\\');
    if (dir < 0)
      dir = newName.ReverseFind (':');
    if (dir < 0)
      dir = 0;
    else
      dir++;
    newName.Delete (dir, (int)strlen(((const char*) newName) + dir));
    while (len-- > 0)
      newName += *s++;


    cTextFile* after = CurFile();

    CString str;
    str.Format ("Fed - [%s...]", newName);
    AfxGetMainWnd()->SetWindowText (str);

    cTextFile* newfile = GetDocument()->DoFileOpen (newName, after, m_view);

    if (newfile) {
      if (newfile->ParentFold() == m_curFile)
        DisplayAtLine (newfile);
      else
        SwitchToFile (newfile);
      }
    ShowFilename();
    }
}
//}}}
//}}}
//{{{  token Cut & Paste
//{{{
void cFedView::OnCopy()
{
  if (m_selectActive) {
    CopySelectToClipboard();
    m_selectActive = false;
    OnUpdate (NULL, cRange::eChanged, &m_selectRange);
    }
  else {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    GetDocument()->CopyLine (range);
    CopyPasteToClipboard();
    OnLineDown();
    }
}
//}}}
//{{{
void cFedView::OnClearPaste()
{
  GetDocument()->ClearPaste();
}
//}}}

//{{{
void cFedView::OnSelectAll()
{
  if (!m_curTop->isProj()) {
    int lines = m_curTop->count_lines (m_view);
    m_selectRange.SetColumn (false);
    m_selectRange.Set (m_view, m_curFile, m_curTop, CPoint (0, 1), CPoint (0x7fff, lines - 2));
    m_selectActive = true;
    m_selectColumn = false;
    m_selectWholeLines = false;
    Invalidate();
    }

}
//}}}
//{{{
void cFedView::OnUnSelect()   //
{
  Beep ("UnSelect - Command not available");
}
//}}}
//{{{
void cFedView::OnLineCopy()   //
{
  Beep ("LineCopy - Command not available");
}
//}}}
//{{{
void cFedView::OnLineCut()    //
{
  Beep ("LineCut - Command not available");
}
//}}}

//{{{
void cFedView::OnCut()
{
  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      CopySelectToClipboard();
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }
    else {
      cRange range (m_view, m_curFile, m_curTop, m_pos);
      GetDocument()->CutLine (range);
      CopyPasteToClipboard();
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnPaste()
{
  if (m_curFile && m_curFile->isEditable()) {
    GetDocument()->ClearPaste(); // clear local copy of paste data
    if (OpenClipboard()) {
      CWaitCursor waitcursor;
      int len = 0;

      HGLOBAL hData = GetClipboardData (CF_TEXT);
      const char* pData = (char*) GlobalLock (hData);

      if (m_selectActive) {
        m_selectActive = false;
        cChange change (m_selectRange, pData);
        len = GetDocument()->Edit (change);
        SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
        }
      else {
        cRange range (m_view, m_curFile, m_curTop, m_pos);
        len = GetDocument()->InsertText (range, pData);
        }
      GlobalUnlock (hData);
      CloseClipboard();

      if (cOption::SetCursorAtEndOfPaste && (len > 0))
        MoveCursorBy (len, 0);
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnKeywordComplete()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable() & !m_selectActive) {
    cRange range (m_view, m_curFile, m_curTop, m_pos);
    int len = GetDocument()->KeywordComplete (range);

    if ((len & 0xfff) > 0)
      MoveCursorBy (len & 0xfff, 0);

    if (len & 0x1000)
      Beep (szUnknownKeyword);
    else if (len & 0x2000)
      Beep (szAmbiguousKeyword);
    }
  else
    Beep (szFileProtected);
}
//}}}
//}}}
//{{{  token Search
//{{{
void cFedView::OnFindAgain()
{
  GetParentFrame()->SetMessageText ("");
  CString str;
  GetFrame()->GetSearchStr (str);
  if (m_findString)
    delete[] m_findString;
  m_findString = new char [str.GetLength()+1];
  strcpy (m_findString, str);

  if (m_findString && *m_findString) {
    str.Format ("Searching for '%s'...", m_findString);
    GetParentFrame()->SetMessageText (str);

    const char* findText = m_findString;
    while (*findText == ' ')   // skip leading spaces and point to text
      findText++;
    int findSpaces = int (findText - m_findString);
    int findlen = (int)strlen(m_findString);
    if ((m_searchingProject == NULL) && m_curFile->isProj())
      m_searchingProject = m_curFile->isProj();      // start a new project-wide search
    ASSERT (m_curTop);
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    cTextFold* top = m_curTop;

    if (m_searchingProject)
      {
      while (top && (top != m_searchingProject))
        top = top->ParentFold();
      if (!top)   // m_searchingProject must have gone away
        {
        top = m_curTop;
        m_searchingProject = m_curTop->isProj();  // reset the flag (pointer)
        }
      }

    cTextLine* limit = m_findUp ? static_cast<cTextLine*>(top) : static_cast<cTextLine*>(top->FoldEnd());
    ASSERT (line);
    int x = m_pos.x; // - line->GetIndent (m_curTop);

    if (m_selectActive && m_atFindString) {
      if (m_findUp)
        x -= m_selectRange.Xlen();
      else
        x += m_selectRange.Xlen();
      Unselect();
      }
    else if (m_atReplaceString && m_replaceString && *m_replaceString) {
      if (m_findUp)
        x -= (int)strlen(m_replaceString);
      else
        x += (int)strlen(m_replaceString);
      }
    m_selectActive = false;

    while (line) {
      if (((line != limit) || !limit->isFileLine()) && (x >= 0)) {
        if (line->isFileLine()) {
          cTextFile* file = (line->isFoldEnd() && line->ParentFold()) ? line->ParentFold()->isFile() : line->isFile();
          if (file) {
            if (file->isEmpty()) {
              file->ReadFile();
              file->Foldup();
              file->Format();
              }
            else if (cOption::CheckFileIsNewer) {
              if (CheckFileTime ("Search file reload", file)) {
                if (file->Reload()) {
                  file->Foldup();
                  file->Format();
                  }
                }
              }
            }
          }
        else {
          //{{{  search this line
          const char* text = line->cTextLine::Text();
          if (text) {
            x -= line->GetIndent (m_curTop);
            if (line->isFoldLine())
              x -= 5;
            int lastpos = (int)strlen(text) - findlen;
            if (lastpos >= 0) {
              const char* found = 0;
              if (!m_findUp) {
                if ((x <= 0) && (findSpaces > 0) && !line->isFoldLine()
                    && (((m_matchCase && (memcmp (text, findText, findlen-findSpaces) == 0))
                         || (!m_matchCase && (_memicmp (text, findText, findlen-findSpaces) == 0)))
                        && (!m_matchWholeWord || ((*(text+findlen-findSpaces) <= ' '))))
                    && (findSpaces <= line->GetIndent (m_curTop))) {
                  found = text;
                  }
                else {
                  //{{{  search right
                  const char* txt = text + x;
                  const char* last_txt = text + lastpos;
                  while (txt <= last_txt)
                    {
                    //{{{  check for match
                    bool match;
                    if (m_matchCase)
                      match = (memcmp (txt, m_findString, findlen) == 0);
                    else
                      match = (_memicmp (txt, m_findString, findlen) == 0);

                    if (match && m_matchWholeWord)
                      {
                      if (IsWordChar (*(txt+findlen)) )  // following char is within word
                        match = false;
                      else if (txt > text)               // not at start of line-text
                        {
                        if (IsWordChar (*(txt-1)) )      // previous char is within word
                          match = false;
                        }
                      }

                    if (match && m_ignoreComments)
                      {
                      if (line->get_format_colour ((int) (txt - text)) == c_CommentText)
                        match = false;
                      }
                    //}}}
                    if (match)
                      {
                      found = txt;
                      break;
                      }
                    txt++;
                    }
                  //}}}
                  }
                }
              else if (x >= 0) {
                //{{{  search left
                const char* txt = x < lastpos ? text + x : text + lastpos;

                while (txt >= text)
                  {
                  //{{{  check for match
                  bool match;
                  if (m_matchCase)
                    match = (memcmp (txt, m_findString, findlen) == 0);
                  else
                    match = (_memicmp (txt, m_findString, findlen) == 0);

                  if (match && m_matchWholeWord)
                    {
                    if (IsWordChar (*(txt+findlen)) )  // following char is within word
                      match = false;
                    else if (txt > text)               // not at start of line-text
                      {
                      if (IsWordChar (*(txt-1)) )      // previous char is within word
                        match = false;
                      }
                    }

                  if (match && m_ignoreComments)
                    {
                    if (line->get_format_colour ((int) (txt - text)) == c_CommentText)
                      match = false;
                    }
                  //}}}
                  if (match)
                    {
                    found = txt;
                    break;
                    }
                  txt--;
                  }
                //}}}
                if (!found && (findSpaces > 0) && !line->isFoldLine()
                    && (((m_matchCase && (memcmp (text, findText, findlen-findSpaces) == 0))
                         || (!m_matchCase && (_memicmp (text, findText, findlen-findSpaces) == 0)))
                        && (!m_matchWholeWord || ((*(text+findlen-findSpaces) <= ' '))))
                    && (findSpaces <= line->GetIndent (m_curTop))) {
                  found = text;
                  }
                }

              if (found) {
                GetParentFrame()->SetMessageText ("");
                if (m_searchingProject) {
                  //{{{  open the file where found
                  cTextFold* top = line->ParentFold();
                  while (top && !top->isFile())
                    top = top->ParentFold();
                  if (!top || !top->isFile())
                    return;
                  SwitchToFile (top->isFile());
                  //}}}
                  }
                //{{{  select text found & display it
                m_pos.x = (int) (found - text) + line->GetIndent (m_curTop);
                if ((findSpaces > 0) && (found == text))
                  m_pos.x -= findSpaces;
                if (line->isFoldLine())
                  m_pos.x += 5;

                if (line->OpenToTop (m_view, m_curTop)) {
                  SetupScrollBars();
                  Invalidate();
                  }
                DisplayAtLine (line);

                m_atFindString = true;
                m_atReplaceString = false;
                m_selectPos.x = m_pos.x + (int)strlen(m_findString);
                m_selectPos.y = m_pos.y;
                m_selectActive = true;
                m_selectRange.Set (m_view, m_curFile, m_curTop, m_pos, m_selectPos);

                OnUpdate (this, cRange::eChangedLine, &m_selectRange);
                //}}}
                return;
                }
              }
            }
          //}}}
          }
        }
      x = m_findUp ? 0x7fffffff : 0;
      if (line == limit)
        break;
      line = m_findUp ? line->Prev() : line->Next();
      }
    }

  str.Format ("Find -- '%s' not found", m_findString);
  Beep (str);
}
//}}}
//{{{
void cFedView::OnFindDown()
{
  m_findUp = false;
  OnFindAgain();
}
//}}}

//{{{
void cFedView::OnFindUp()
{
  m_findUp = true;
  OnFindAgain();
}
//}}}
//{{{
bool cFedView::OnReplace()
{
  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive && m_atFindString) {
      CString str;
      GetFrame()->GetSearchStr (str);
      if (m_findString)
        delete[] m_findString;
      m_findString = new char [str.GetLength()+1];
      strcpy (m_findString, str);

      GetFrame()->GetReplaceStr (str);
      if (m_replaceString)
        delete[] m_replaceString;
      m_replaceString = new char [str.GetLength()+1];
      strcpy (m_replaceString, str);

      m_atFindString = false;
      m_atReplaceString = false;
      cFedDoc* pDoc = GetDocument();
      m_selectActive = false;
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      pDoc->Delete (m_selectRange);
      if (m_replaceString && *m_replaceString) {
        cRange range (m_view, m_curFile, m_curTop, m_pos, m_pos);
        pDoc->InsertText (range, m_replaceString);
        }
      m_atReplaceString = true;
      return true;
      }
    }
  else
    Beep (szFileProtected);

  return false;
  }
//}}}
//{{{
void cFedView::OnReplaceAll()      //
{
  Beep ("ReplaceAll - Command not available");
}
//}}}

//{{{
void cFedView::OnSubstitute()      //
{
  Beep ("Substitute - Command not available");
}
//}}}

//{{{
void cFedView::OnFindDialog()
{
  CString findstr;

  if (m_selectActive && m_selectRange.isSingleLine()) {
    int len = m_selectRange.Xlen();
    if ((len > 0) && m_selectRange.Line()) {
      cTextLine* line = m_selectRange.Line();
      int pos = m_selectRange.Xpos() - line->GetFoldIndent (m_curTop);
      if (line == m_curTop)
        pos += line->Indent();

      cTextFold* fold = line->isFold();
      if (fold) {
        pos -= 5;           // adjust by fold mark characters
        line = fold->GetTextLine (m_view);
        if (line != fold)   // adjust for indent difference of fold and actual line
          pos += line->Indent() - fold->Indent();
        }

      line->copy_text (findstr.GetBuffer (len + 1), pos, len); // cText call - knows about indent only
      findstr.ReleaseBuffer();
      GetFrame()->AddSearchStr (findstr);
      }
    Unselect();
    }
  else
    findstr = m_findString;

  if (m_findDialog) {
    }
  else {
    m_findDialog = new cMyFindReplaceDialog();
    m_findDialog->m_fr.hInstance = AfxGetApp()->m_hInstance;
    m_findDialog->m_fr.Flags |= FR_ENABLETEMPLATE;
    m_findDialog->m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_FIND_DIALOG);

    m_findDialog->SetIgnoreComments (m_ignoreComments);
    m_findDialog->SetSearchAll (m_searchAll);

    DWORD flags = 0;
    if (!m_findUp) flags |= FR_DOWN;
    if (m_matchCase) flags |= FR_MATCHCASE;
    if (m_matchWholeWord) flags |= FR_WHOLEWORD;

    if (!m_findDialog->Create (true, findstr, m_replaceString, flags, this)) {
      MessageBox ("Failed to create Find Dialog", "Find", MB_ICONERROR);
      return;
      }
    m_searchingProject = m_curFile->isProj();
    m_findOnly = true;
    }
}
//}}}
//{{{
void cFedView::OnFindReplaceDialog()
{
  CString findstr;

  if (m_selectActive && m_selectRange.isSingleLine()) {
    int len = m_selectRange.Xlen();
    if ((len > 0) && m_selectRange.Line()) {
      cTextLine* line = m_selectRange.Line();
      int pos = m_selectRange.Xpos() - line->GetFoldIndent (m_curTop);
      if (line == m_curTop)
        pos += line->Indent();

      cTextFold* fold = line->isFold();
      if (fold) {
        pos -= 5;           // adjust by fold mark characters
        line = fold->GetTextLine (m_view);
        if (line != fold)   // adjust for indent difference of fold and actual line
          pos += line->Indent() - fold->Indent();
        }

      line->copy_text (findstr.GetBuffer (len + 1), pos, len); // cText call - knows about indent only
      findstr.ReleaseBuffer();
      GetFrame()->AddSearchStr (findstr);
      m_atFindString= true;
      }
    else
      Unselect();
    }
  else
    findstr = m_findString;

  if (m_findDialog) {
    }
  else {
    m_findDialog = new cMyFindReplaceDialog();
    m_findDialog->m_fr.hInstance = AfxGetApp()->m_hInstance;
    m_findDialog->m_fr.Flags |= FR_ENABLETEMPLATE;
    m_findDialog->m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_REPLACE_DIALOG);

    m_findDialog->SetIgnoreComments (m_ignoreComments);
    m_findDialog->SetSearchAll (m_searchAll);

    DWORD flags = 0;
    if (!m_findUp) flags |= FR_DOWN;
    if (m_matchCase) flags |= FR_MATCHCASE;
    if (m_matchWholeWord) flags |= FR_WHOLEWORD;

    if (!m_findDialog->Create (false, findstr, m_replaceString, flags, this)) {
      MessageBox ("Failed to create Find&Replace Dialog", "Find", MB_ICONERROR);
      return;
      }
    m_searchingProject = m_curFile->isProj();
    m_findOnly = false;
    }

}
//}}}
//}}}
//{{{  token Macro
//{{{
void cFedView::OnMacroDialog()     //
{
  Beep ("MacroDialog - Command not available");
}
//}}}
//{{{
void cFedView::OnMacroLearn()
{
  if (m_recording) {
    m_recording = FALSE;
    cLanguage* gen = cLanguage::m_genericLanguage;
    if (gen) {
      int key = cKeyMap::FindKey ("Ctrl Shift H");
      gen->add_macro (key, "Temp", m_macroStr);
      }
    }
  else {
    m_recording = TRUE;
    m_macro_textmode = FALSE;
    m_macroStr.Empty();
    }
}
//}}}
//{{{
void cFedView::OnMacroCall()
{
  if (m_recording)
    OnMacroLearn();  // stop recording first

  if (!m_macroStr.IsEmpty()) {
    const char* macro = m_macroStr;
    DoMacro ((const unsigned char*) macro);
    SetCursorPos (m_pos);  // dummy to update caret
    }
}
//}}}
//{{{
void cFedView::OnMacroLoad()       //
{
  Beep ("MacroLoad - Command not available");
}
//}}}
//{{{
void cFedView::OnMacroSave()       //
{
  Beep ("MacroSave - Command not available");
}
//}}}
//}}}
//{{{  token Sundry
//{{{
void cFedView::OnFormFeed()        //
{
  Beep ("FormFeed - Command not available");
}
//}}}
//{{{
void cFedView::OnUndo()
{
  if (!cOption::EnableUndo)
    OnUndoLine();
  else // new 'proper' undo
    {
    Unselect();
    if (m_curFile && m_curFile->isEditable()) {
      if (m_curFile->UndoList())
        GetDocument()->PerformUndo (*m_curFile);
      else
        Beep ("Undo - Nothing to undo");
      }
    else
      Beep (szFileProtected);
    }
}
//}}}
//{{{
void cFedView::OnUndoLine()
{ // old 'line' undo
  Unselect();
  if (m_curFile && m_curFile->isEditable()) {
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    if (!line->isFileLine()) {
      cRange range (m_view, m_curFile, m_curTop, m_pos);
      GetDocument()->UndoLine (range);
      }
    else
      Beep ("UndoLine - Line was not changed");
    }
  else
    Beep (szFileProtected);

}
//}}}
//{{{
void cFedView::OnRefresh()
{
  if (m_curFile && !m_curFile->isProj())
    m_curFile->Format();
  GetDocument()->UpdateAllViews (NULL, 0, NULL);
}
//}}}
//{{{
void cFedView::OnAbort()
{
  GetDocument()->AbortFoldCreate();
}
//}}}
//{{{
void cFedView::OnOverStrike()
{
  m_overstrike = !m_overstrike;
}
//}}}
//{{{
void cFedView::OnToggleTabs()
{
  cTextFile* file = CurFile();
  if (file) {
    file->ToggleTabs();
    GetDocument()->UpdateAllViews (NULL, 0, NULL);
    }
}
//}}}

//{{{
void cFedView::OnTab()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      //{{{  delete the selection first
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      //}}}
      }

    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    if (line && !line->isFileLine()) {
      int tablen;
      if (line->cTextLine::Text() && (m_pos.x > line->GetIndent (m_curTop)))
        tablen = m_curFile->Tablen (cLanguage::InText);
      else
        tablen = m_curFile->Tablen (cLanguage::InIndent);

      int spaces = tablen - ((m_pos.x + m_curTop->GetIndent (m_curFile)) % tablen);
      cRange range (m_view, m_curFile, m_curTop, m_pos);
      GetDocument()->InsertChar (range, ' ', spaces);
      MoveCursorBy (spaces, 0);
      }
    }
  else
    Beep (szFileProtected);
}
//}}}
//{{{
void cFedView::OnReturn()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive) {
      // delete selected area first
      m_selectActive = false;
      GetDocument()->Delete (m_selectRange);
      SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
      }

    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    if (line) {
      cRange range (m_view, m_curFile, m_curTop, m_pos);
      cTextLine* newline = GetDocument()->SplitLine (range);
      if (newline) {
        OnLineDown();
        m_pos.x = newline->GetIndent(m_curTop);
        SetCursorPos (m_pos);
        }
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnAlignNextColumn()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive)
      GetDocument()->AlignNextColumn (m_selectRange);
    else {  // range is current line to first blank line
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      if (line) {
        //{{{  scan for a block of lines
        int block_indent = line->Indent();
        cTextFold* fold = line->ParentFold();
        if (line->isFold() && line->isFold()->isOpen(m_view)) {
          block_indent = 0;
          fold = line->isFold();
          }
        //bool aligningFolds = (bool) line->isFold();
        cTextLine* startline = line;
        int y = m_pos.y - 1;
        while (line && line->GetTextLine(m_view) && line->GetTextLine(m_view)->Text()
                    && (line->Indent() == block_indent)
                    && (line->ParentFold() == fold)
                    && !(line->isFold() && line->isFold()->isOpen(m_view))
                    && ((startline->isFold() && line->isFold())
                       || (!startline->isFold() && !line->isFold())) ) {
          y += 1;
          line = line->NextLine(m_view);
          }
        //}}}
        if (y > m_pos.y) {
          cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (0, y));
          GetDocument()->AlignNextColumn (range);
          }
        }
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//{{{
void cFedView::OnDealignNextColumn()
{
  ASSERT (m_curTop);

  if (m_curFile && m_curFile->isEditable()) {
    if (m_selectActive)
      GetDocument()->DealignNextColumn (m_selectRange);
    else {  // range is current line to first blank line
      cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
      if (line) {
        //{{{  scan for a block of lines
        int block_indent = line->Indent();
        cTextFold* fold = line->ParentFold();
        if (line->isFold() && line->isFold()->isOpen(m_view)) {
          block_indent = 0;
          fold = line->isFold();
          }
        int y = m_pos.y - 1;
        while (line && line->GetTextLine(m_view) && line->GetTextLine(m_view)->Text()
                    && (line->Indent() == block_indent)
                    && (line->ParentFold() == fold)
                    && !(line->isFold() && line->isFold()->isOpen(m_view))) {
          y += 1;
          line = line->NextLine(m_view);
          }
        //}}}
        if (y > m_pos.y) {
          cRange range (m_view, m_curFile, m_curTop, m_pos, CPoint (0, y));
          GetDocument()->DealignNextColumn (range);
          }
        }
      }
    }
  else
    Beep (szFileProtected);
  }
//}}}
//}}}
//{{{  Menu/Button Update UI handlers
//{{{
void cFedView::OnUpdateIndicatorRec (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (m_recording);
}
//}}}
//{{{
void cFedView::OnUpdateIndicatorOvr (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (TRUE); //m_overstrike);
  if (m_overstrike)
    pCmdUI->SetText ("OVR");
  else
    pCmdUI->SetText ("INS");
}
//}}}
//{{{
void cFedView::OnUpdateIndicatorPos (CCmdUI* pCmdUI)
{
  char msg [40];
  if (m_lineNumber > 0)
    sprintf (msg, "%d %d", m_pos.x, m_lineNumber);
  else
    sprintf (msg, "%d ***", m_pos.x);

  pCmdUI->SetText (msg);
  pCmdUI->Enable (!m_curFile || !m_curFile->isProj());
}
//}}}

//{{{
void cFedView::OnUpdateFileSave (CCmdUI* pCmdUI)
{
  cTextFile* file = m_curFile;
  if (m_curTop && file && file->isProj()) {
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    if (!line)
      file = 0;
    else if (line != m_curFile->FoldEnd())
      file = line->isFile();
    }

  pCmdUI->Enable (file && !file->isProtected() && file->isModified());
}
//}}}
//{{{
void cFedView::OnUpdateFileSaveAll (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (GetDocument()->AnyFilesModified(m_curFile));
}
//}}}
//{{{
void cFedView::OnUpdateFileSaveProject (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (m_curFile && m_curFile->isProj()
                  && m_curFile->isModified()
                  && !m_curFile->isEmpty());
}
//}}}
//{{{
void cFedView::OnUpdateFilePrint (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (TRUE);
}
//}}}

//{{{
void cFedView::OnUpdateEditCut (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (m_selectActive && m_curFile && m_curFile->isEditable());
}
//}}}
//{{{
void cFedView::OnUpdateEditCopy (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (m_selectActive);
}
//}}}
//{{{
void cFedView::OnUpdateEditPaste (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (m_curFile && m_curFile->isEditable()
                  && ::IsClipboardFormatAvailable (CF_TEXT));
}
//}}}
//{{{
void cFedView::OnUpdateEditReplace (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (m_curFile && m_curFile->isEditable());
}
//}}}
//{{{
void cFedView::OnUpdateEditProtect (CCmdUI* pCmdUI)
{
  cTextFile* file = m_curFile;
  if (m_curTop && file && file->isProj()) {
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    if (!line)
      file = 0;
    else if (line != m_curFile->FoldEnd())
      file = line->isFile();
    }

  pCmdUI->SetCheck (file && file->isProtected());
  pCmdUI->Enable (file && !file->isWriteProtected() && !cFedApp::ReadOnly);
}
//}}}

//{{{
void cFedView::OnUpdateViewOutput (CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_curFile && m_curFile->isWithin (cFedDoc::getBuffers()));
}
//}}}
//{{{
void cFedView::OnUpdateViewLinenumbers (CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ShowLineNumbers);
}
//}}}
//{{{
void cFedView::OnUpdateViewFolds (CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ShowFolds);
}
//}}}
//{{{
void cFedView::OnUpdateViewGotoline (CCmdUI* pCmdUI)
{
  pCmdUI->Enable (m_curFile && !m_curFile->isProj()
                 && (m_curFile->Next() != m_curFile->FoldEnd()));
}
//}}}
//}}}
//{{{  clipboard
//{{{
void cFedView::CopySelectToClipboard()
{
  if (m_selectActive) {
    cLanguage* language = m_curFile ? m_curFile->Language() : 0;
    const char* comment_start = language ? language->m_commentStart1 : 0;
    const char* comment_end = language ? language->m_commentEnd1 : 0;
    size_t comment_start_len = comment_start ? strlen(comment_start) : 0;
    size_t comment_end_len = comment_end ? strlen(comment_end) : 0;
    char openmark [4] = "{{{";
    char closemark [4] = "}}}";
    if (comment_start && strchr(comment_start, '{')) {
      strcpy (openmark, "<<<");
      strcpy (closemark, ">>>");
      }

    cTextLine* line1 = m_selectRange.FirstLine();
    cTextLine* line2 = m_selectRange.LastLine();
    int slen1 = line1->GetTextLine(m_view)->TextLen() + line1->GetTextOffset (m_curTop);
    int slen2 = line2->GetTextLine(m_view)->TextLen() + line2->GetTextOffset (m_curTop);
    DWORD len = 1;
    if (m_selectRange.Column())
      //{{{  count length of column copy global memory required
      {
      if ((m_selectRange.Xlen() <= 0) || (m_selectRange.Ylen() <= 0))
        return;  // nothing to copy
      len += (m_selectRange.Xlen() + 2) * m_selectRange.Ylen() - 2;
      }
      //}}}
    else
      //{{{  count length of global memory required
      {
      if (m_selectRange.isSingleLine()) {
        if (m_selectRange.LastXpos() <= slen1)
          len += m_selectRange.Xlen();
        else if (m_selectRange.Xpos() < slen1)
          len += slen1 - m_selectRange.Xpos();
        }
      else {
        int fold_indent = 0;
        cTextLine* cur = line1;
        if (m_selectRange.Xpos() > 0) {
          len += (slen1 > m_selectRange.Xpos()) ? slen1 - m_selectRange.Xpos() + 2 : 2;
          cur = cur->NextLine (m_view);
          }

        while (cur && (cur != line2)) {
          int copylen = fold_indent + cur->Indent() + cur->TextLen();
          if (cur->isFold() || cur->isFoldEnd()) {
            len += int(comment_start_len + 3 + comment_end_len);
            if (copylen > 0)
              len += 2;
            if (cur->isFold())
              fold_indent += cur->Indent();
            else if (cur->ParentFold()) {
              fold_indent -= cur->ParentFold()->Indent();
              if (fold_indent < 0)
                fold_indent = 0;
              }
            }
          len += copylen + 2;
          cur = cur->Next();
          }

        len += (slen2 > m_selectRange.LastXpos()) ? m_selectRange.LastXpos() : slen2;
        }
      }
      //}}}

    HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE, len);
    char* pData = (char*) GlobalLock (hData);
    if (m_selectRange.Column())
      //{{{  column copy to global memory
      {
      while (line1)
        {
        //{{{  copy text from a single line
        {
        int x = m_selectRange.Xpos();
        int copylen = 0;

        if (m_selectRange.LastXpos() <= slen1)
          copylen = m_selectRange.Xlen();
        else if (x < slen1)
          copylen = slen1 - x;

        int indent = line1->GetTextOffset (m_curTop);

        if (x + copylen <= indent)
          memset (pData, ' ', copylen);
        else
          {
          if (x < indent)
            {
            memset (pData, ' ', indent - x);
            copylen -= indent - x;
            pData += indent - x;
            x = indent;
            }
          memcpy (pData, line1->GetText(m_view) + x - indent, copylen);
          }
        pData += copylen;
        }
        //}}}
        if (line1 == line2)
          break;
        *pData++ = '\r';
        *pData++ = '\n';
        line1 = line1->NextLine (m_view);
        }
      *pData = 0;
      }
      //}}}
    else
      //{{{  copy text to global memory
      {
      if (m_selectRange.isSingleLine())
        //{{{  copy text from a single line
        {
        int x = m_selectRange.Xpos();
        int copylen = 0;

        if (m_selectRange.LastXpos() <= slen1)
          copylen = m_selectRange.Xlen();
        else if (x < slen1)
          copylen = slen1 - x;

        int indent = line1->GetTextOffset (m_curTop);

        if (x + copylen <= indent)
          memset (pData, ' ', copylen);
        else
          {
          if (x < indent)
            {
            memset (pData, ' ', indent - x);
            copylen -= indent - x;
            pData += indent - x;
            x = indent;
            }
          memcpy (pData, line1->GetText(m_view) + x - indent, copylen);
          }
        pData += copylen;
        }
        //}}}
      else
        //{{{  copy text from multiple lines
        {
        int fold_indent = 0;
        int copylen;
        cTextLine* cur = line1;
        if (m_selectRange.Xpos() > 0)
          {
          int x = m_selectRange.Xpos();
          copylen = slen1 - x;
          if (copylen > 0)
            {
            int indent = line1->GetTextOffset (m_curTop);
            if (x + copylen <= indent)
              memset (pData, ' ', copylen);
            else {
              if (x < indent)
                {
                memset (pData, ' ', indent - x);
                copylen -= indent - x;
                pData += indent - x;
                x = indent;
                }
              memcpy (pData, line1->GetText(m_view) + x - indent, copylen);
              }
            pData += copylen;
            }
          *pData++ = '\r';
          *pData++ = '\n';
          cur = cur->NextLine (m_view);
          }

        while (cur && (cur != line2))
          {
          copylen = fold_indent + cur->Indent();
          if (copylen > 0) {
            memset (pData, ' ', copylen);
            pData += copylen;
            }
          copylen = cur->TextLen();
          if (cur->isFold() || cur->isFoldEnd())
            {
            //{{{  copy fold marks & text to global memory
            if (comment_start_len > 0)
              {
              memcpy (pData, comment_start, comment_start_len);
              pData += comment_start_len;
              }
            if (cur->isFoldEnd())
              {
              memcpy (pData, closemark, 3);
              pData += 3;
              }
            else
              {
              memcpy (pData, openmark, 3);
              pData += 3;
              }
            if (copylen > 0)
              {
              memcpy (pData, "  ", 2);
              pData += 2;
              memcpy (pData, cur->Text(), copylen);
              pData += copylen;
              }
            if (comment_end_len > 0)
              {
              memcpy (pData, comment_end, comment_end_len);
              pData += comment_end_len;
              }
            //}}}
            if (cur->isFold())
              fold_indent += cur->Indent();
            else if (cur->ParentFold())
              {
              fold_indent -= cur->ParentFold()->Indent();
              if (fold_indent < 0)
                fold_indent = 0;
              }
            }
          else if (copylen > 0)
            {
            memcpy (pData, cur->Text(), copylen);
            pData += copylen;
            }
          *pData++ = '\r';
          *pData++ = '\n';
          cur = cur->Next();
          }

        copylen = (slen2 > m_selectRange.LastXpos()) ? m_selectRange.LastXpos() : slen2;
        if (copylen > 0)
          {
          int indent = line2->GetTextOffset (m_curTop);
          if (copylen <= indent)
            memset (pData, ' ', copylen);
          else {
            if (indent > 0)
              {
              memset (pData, ' ', indent);
              copylen -= indent;
              pData += indent;
              }
            memcpy (pData, line2->GetText(m_view), copylen);
            }
          pData += copylen;
          }
        }
        //}}}
      *pData = 0;
      }
      //}}}
    GlobalUnlock (hData);

    if (OpenClipboard()) {
      EmptyClipboard();
      SetClipboardData (CF_TEXT, hData);
      CloseClipboard();
      }
    else
      GlobalFree (hData);
    }
}
//}}}
//{{{
void cFedView::CopyPasteToClipboard()
{
  cTextFold* pasteFold = GetDocument()->GetPasteFold();
  if (pasteFold && (pasteFold->Next() != pasteFold->FoldEnd())) {
    cLanguage* language = m_curFile ? m_curFile->Language() : 0;
    const char* comment_start = language ? language->m_commentStart1 : 0;
    const char* comment_end = language ? language->m_commentEnd1 : 0;
    size_t comment_start_len = comment_start ? strlen(comment_start) : 0;
    size_t comment_end_len = comment_end ? strlen(comment_end) : 0;
    char openmark [4] = "{{{";
    char closemark [4] = "}}}";
    if (comment_start && strchr(comment_start, '{')) {
      strcpy (openmark, "<<<");
      strcpy (closemark, ">>>");
      }

    DWORD len = 1;
    //{{{  count length of global memory required
    int fold_indent = 0;
    cTextLine* cur = pasteFold->Next();

    while (cur && (cur != pasteFold->FoldEnd())) {
      int copylen = fold_indent + cur->Indent() + cur->TextLen();
      if (cur->isFold() || cur->isFoldEnd()) {
        len += int(comment_start_len + 3 + comment_end_len);
        if (copylen > 0)
          len += 2;
        if (cur->isFold())
          fold_indent += cur->Indent();
        else if (cur->ParentFold()) {
          fold_indent -= cur->ParentFold()->Indent();
          if (fold_indent < 0)
            fold_indent = 0;
          }
        }
      len += copylen + 2;
      cur = cur->Next();
      }

    //}}}

    HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE, len);
    char* pData = (char*) GlobalLock (hData);
    //{{{  copy text to global memory
    fold_indent = 0;
    int copylen;
    cur = pasteFold->Next();

    while (cur && (cur != pasteFold->FoldEnd())) {
      copylen = fold_indent + cur->Indent();
      if (copylen > 0) {
        memset (pData, ' ', copylen);
        pData += copylen;
        }
      copylen = cur->TextLen();
      if (cur->isFold() || cur->isFoldEnd()) {
        //{{{  copy fold marks & text to global memory

        if (comment_start_len > 0) {
          memcpy (pData, comment_start, comment_start_len);
          pData += comment_start_len;
          }
        if (cur->isFoldEnd()) {
          memcpy (pData, closemark, 3);
          pData += 3;
          }
        else {
          memcpy (pData, openmark, 3);
          pData += 3;
          }
        if (copylen > 0) {
          memcpy (pData, "  ", 2);
          pData += 2;
          memcpy (pData, cur->Text(), copylen);
          pData += copylen;
          }
        if (comment_end_len > 0) {
          memcpy (pData, comment_end, comment_end_len);
          pData += comment_end_len;
          }
        //}}}
        if (cur->isFold())
          fold_indent += cur->Indent();
        else if (cur->ParentFold()) {
          fold_indent -= cur->ParentFold()->Indent();
          if (fold_indent < 0)
            fold_indent = 0;
          }
        }
      else if (copylen > 0) {
        memcpy (pData, cur->Text(), copylen);
        pData += copylen;
        }
      *pData++ = '\r';
      *pData++ = '\n';
      cur = cur->Next();
      }

    *pData = 0;
    //}}}
    GlobalUnlock (hData);

    if (OpenClipboard()) {
      EmptyClipboard();
      SetClipboardData (CF_TEXT, hData);
      CloseClipboard();
      }
    else
      GlobalFree (hData);
    }
}
//}}}
//}}}
//{{{  line num
//{{{
cTextLine* cFedView::GetLine (int linenum) {
  return m_curTop ? m_curTop->GetLine(m_view, linenum) : 0;

  //cTextLine* cur= m_curTop;
  //int num = 0;
  //while (cur && (num < linenum)) {
  //  cur = cur->NextLine (m_view);
  //  num += 1;
  //  }
  //return cur;
  }
//}}}
//{{{
int cFedView::FindLine (cTextLine* line) {
  cTextLine* cur = m_curTop;
  cTextLine* last = m_curTop ? m_curTop->FoldEnd() : 0;

  CPoint pos = GetScrollPosition();
  int y = 0;

  while (cur) {
    if (cur == line)
      return y;
    else if (cur == last)
      break;
    y += 1;
    cur = cur->NextLine (m_view);
    }
  return -1;
  }
//}}}
//{{{
void cFedView::ShowFile (cTextFile* file)
{
  if (file && m_curFile)
    {
    if (!cFedApp::GotoLineNumber && (file->ParentFold() == m_curFile))
      DisplayAtLine (file);
    else
      {
      if ((file != m_curFile) || (m_curFile->isProj()))
        SwitchToFile (file);
      if (cFedApp::GotoLineNumber > 0)
        {
        GotoLineNumber (cFedApp::GotoLineNumber);
        cFedApp::GotoLineNumber = 0;
        }
      }
    ShowFilename();
    //SetForegroundWindow();
    }
}
//}}}
//{{{
void cFedView::SwitchToFile (cTextFile* file)
{
  if (file && (file != m_curFile)) {
    m_atFindString = false;
    m_atReplaceString = false;

    long yscroll = GetScrollPosition().y / m_cChar.cy;
    ASSERT (m_curTop);

    // first exit the current File, leaving any Folds open
    if (m_curFile && m_curFile->ParentFold()) {
      m_curFile->SetBelowTop (m_curTop);
      m_curFile->SetBelow (m_pos.x, m_pos.y, yscroll);
      m_curFile->Close (m_view);

      int fold_level;
      m_curFile->GetAbove (m_pos.x, m_pos.y, yscroll, fold_level);
      m_curTop = m_curFile->ParentFold();
      m_curTop->Open(m_view);    // should already be open!
      m_curFile = m_curTop->isFile();
      }

    // new exit Projects until we find one enclosing file
    while (m_curTop->ParentFold() && !file->isWithin(m_curTop))
      FoldExit (!m_curTop->isFile(), yscroll);

    // finally open downto file, using a recursive call to first search up
    // from file to m_curTop, then open down from m_curTop to file, following
    // the route found
    OpenUpFrom (file, yscroll);  // recursive routine
    m_curFile = file;

    SetupScrollBars();
    yscroll *= m_cChar.cy;
    SetScrollPos (SB_VERT, yscroll);

    ShowFilename();
    OnUpdate(this, 0L, NULL);
    MoveCursorBy (0, 0);  // dummy to redisplay caret

    if (cOption::CheckFileIsNewer) {
      if (CheckFileTime ("SwitchToFile debug info")) {
        OnFileReload();
        }
      }
    }
}
//}}}
//{{{
void cFedView::OpenUpFrom (cTextFold* fold, long& yscroll)
{
if (fold)
  {
  if (fold->ParentFold() && (fold->ParentFold() != m_curTop))
    OpenUpFrom (fold->ParentFold(), yscroll);    // recursive call to find route

  FoldEnter (fold, yscroll);
  //fold->Open (m_view);
  //fold->SetAbove (m_pos.x, m_pos.y, yscroll, 1); //fold_level);
  //fold->GetBelow (m_pos.x, m_pos.y, yscroll);
  }
}
//}}}
//{{{
void cFedView::GotoLineNumber (int linenum)
{
  Unselect();
  ASSERT (m_curTop);
  cTextFile* file = m_curFile;

  if (m_curTop->isProj())
    file = (m_curTop->GetLine(m_view, m_pos.y))->isFile();

  if (!file || file->isProj()) return;

  if (file != m_curFile)
    SwitchToFile (file);

  cTextLine* line = file->FindLine (linenum);

  if (line)
    {
    DisplayAtLine (line);
    m_pos.x = line->GetIndent (m_curTop);
    SetCursorPos (m_pos);
    }
}
//}}}
//}}}
//{{{  margins
//{{{
void cFedView::ClearCurMargins()
{
  m_CurTopLeftMargin.cx = 0;
  m_CurTopLeftMargin.cy = 0;
  m_CurBottomRightMargin.cx = 0;
  m_CurBottomRightMargin.cy = 0;
}
//}}}
//{{{
void cFedView::SetCurMargins()
{
  CPoint pos = GetScrollPosition();

  m_CurTopLeftMargin = m_ScrollMargin;
  m_CurBottomRightMargin = m_ScrollMargin;

  if (m_CurTopLeftMargin.cx > m_pos.x - (pos.x  / m_cChar.cx))
    m_CurTopLeftMargin.cx = m_pos.x - (pos.x / m_cChar.cx);

  if (m_CurTopLeftMargin.cy > m_pos.y - (pos.y / m_cChar.cy))
    m_CurTopLeftMargin.cy = m_pos.y - (pos.y / m_cChar.cy);

  if (m_CurBottomRightMargin.cx > ((pos.x + m_cWlen.cx) / m_cChar.cx) - m_pos.x)
    m_CurBottomRightMargin.cx = ((pos.x + m_cWlen.cx) / m_cChar.cx) - m_pos.x;

  if (m_CurBottomRightMargin.cy > ((pos.y + m_cWlen.cy) / m_cChar.cy) - m_pos.y)
    m_CurBottomRightMargin.cy = ((pos.y + m_cWlen.cy) / m_cChar.cy) - m_pos.y;
}
//}}}
//}}}
//{{{  cursor
//{{{
void cFedView::KeepCursorOnScreen()
{
  CPoint pos = GetScrollPosition();

  int left_guard   = (m_pos.x - m_CurTopLeftMargin.cx) * m_cChar.cx;
  int right_guard  = (m_pos.x + m_CurBottomRightMargin.cx) * m_cChar.cx;
  int top_guard    = (m_pos.y - m_CurTopLeftMargin.cy) * m_cChar.cy;
  int bottom_guard = (m_pos.y + m_CurBottomRightMargin.cy) * m_cChar.cy;

  CSize scrollby (0, 0);
  if ((left_guard < pos.x) && (pos.x > 0))
    scrollby.cx = left_guard - pos.x;
  else if (right_guard > pos.x + m_cWlen.cx)
    scrollby.cx = right_guard - (pos.x + m_cWlen.cx);

  if ((top_guard < pos.y) && (pos.y > 0))
    scrollby.cy = top_guard - pos.y;
  else if (bottom_guard > pos.y + m_cWlen.cy)
    scrollby.cy = bottom_guard - (pos.y + m_cWlen.cy);

  OnScrollBy (scrollby, TRUE);
}
//}}}
//{{{
void cFedView::UpdateCaret()
{
  CPoint pos = GetScrollPosition();
  POINT caretpos;
  caretpos.x = m_pos.x * m_cChar.cx + m_xStart - 1 - pos.x;
  caretpos.y = m_pos.y * m_cChar.cy - pos.y;
  if (caretpos.x < m_xStart - 1)
    caretpos.x = m_xStart - 1;

  SetCaretPos (caretpos);
  }
//}}}
//{{{
void cFedView::SetCursorPos (int x, int y)
{
  if (x < 0) x = 0;
  if (y < 0) y = 0;

  CPoint oldpos (m_pos);
  CPoint oldmatchpos (m_matchPos);
  BOOL oldmatch = m_displayMatch;

  if ((x != m_pos.x) || (y != m_pos.y)) {
    m_atFindString = false;
    m_atReplaceString = false;
    if (m_displayMatch)
      DrawMatch (false);
    }

  m_pos.x = x;
  m_pos.y = y;

  if (m_curTop) {
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    ASSERT (line);
    m_lineNumber = line->LineNum();
    }

  if (MatchBracket())
    DrawMatch (true);

  KeepCursorOnScreen();
  UpdateCaret();

  if (m_debugWin)
    m_debugWin->Update (this);
}
//}}}
//{{{
void cFedView::SetCursorPos (CPoint pos)
{
  SetCursorPos (pos.x, pos.y);
}
//}}}
//{{{
void cFedView::MoveCursorBy (int dx, int dy)
{
  SetCursorPos (m_pos.x + dx, m_pos.y + dy);
}
//}}}
//}}}

//{{{
LRESULT cFedView::OnFindReplace (WPARAM wParam, LPARAM lParam) {

  if (m_findDialog) {
    CString str;
    //{{{  fetch FindString
    str = m_findDialog->GetFindString();
    if (m_findString && (str.Compare (m_findString) != 0)) {
      delete[] m_findString;
      m_findString = 0;
      }

    if (!str.IsEmpty() && !m_findString) {
      m_findString = new char [str.GetLength() + 1];
      strcpy (m_findString, str);
      }

    if (m_findString && !m_findDialog->IsTerminating())
      GetFrame()->AddSearchStr (m_findString);
    //}}}
    //{{{  fetch ReplaceString
    str = m_findDialog->GetReplaceString();

    if (m_replaceString && (str.Compare (m_replaceString) != 0)) {
      delete[] m_replaceString;
      m_replaceString = 0;
      }

    if (!m_replaceString) {
      m_replaceString = new char [str.GetLength() + 1];
      strcpy (m_replaceString, str);
      }

    if (m_replaceString && !m_findOnly && !m_findDialog->IsTerminating())
      GetFrame()->AddReplaceStr (m_replaceString);
    //}}}

    m_matchCase = m_findDialog->MatchCase();
    m_matchWholeWord = m_findDialog->MatchWholeWord();
    m_ignoreComments = m_findDialog->IgnoreComments();
    m_searchAll = m_findDialog->SearchAll();
    m_findUp = !m_findDialog->SearchDown();

    if (m_findDialog->IsTerminating())
      m_findDialog = 0;

    else if (m_findDialog->ReplaceAll()) {
      if (m_searchingProject) {
        switch (AfxMessageBox ("Replace for all files in the Project?", MB_OKCANCEL)) {
          case IDCANCEL:
            return 0;       // don't continue
          case IDOK:
            break;
          default:
            ASSERT(FALSE);
            break;
          }
        }
      CPoint startpos = m_pos;

      if (m_findDialog->SearchAll()) {
        SetCursorPos (0, 0);
        m_atFindString = false;
        }

      if (!m_atFindString)
        OnFindAgain();
      while (OnReplace())
        OnFindAgain();

      SetCursorPos (startpos);   // return to starting position
      }

    else if (m_findDialog->ReplaceCurrent()) {
      if (OnReplace())
        OnFindAgain();
      }

    else if (m_findDialog->FindNext()) {
      if (m_findDialog->SearchAll()) {
        SetCursorPos (0, 0);
        m_atFindString = false;
        m_findDialog->SetSearchAll (false);
        }

      OnFindAgain();
      if (m_atFindString && m_findDialog && m_findOnly) {
        m_findDialog->DestroyWindow();
        m_findDialog = 0;
        }
      }
    }
  else
    Beep ("OnFindReplace = NULL");

  return 0;
  }
//}}}

//{{{
void cFedView::Beep (const char* msg) {

  if (msg)
    GetParentFrame()->SetMessageText (msg);
  else
    GetParentFrame()->SetMessageText ("");

  if (!cOption::SuppressBeeps)
    ::MessageBeep (0xffffffff);
  }
//}}}
//{{{
void cFedView::ShowFilename() {

  cTextFold* cur = m_curTop;
  while (cur && !cur->isFile())
    cur = cur->ParentFold();
  cTextFile* file = cur ? cur->isFile() : 0;

#ifdef _DEBUG
  std::string str ("Fed(debug) - [");
#else
  std::string str ("Fed - [");
#endif
  if (file)
    str += file->PathnameForDisplay().c_str();
  str += "]";

  AfxGetMainWnd()->SetWindowText (str.c_str());
  }
//}}}
//{{{
int cFedView::GetFilters (CString& strFilter, const char* filename, const char* defaultExt) {
// returns filterIndex that matches filename

  if (!defaultExt)
    {
    defaultExt = strrchr (filename, '.');
    if (defaultExt)
      defaultExt++;
    }
  bool isProject = defaultExt ? (_stricmp (defaultExt, "fed") == 0) : false;

  int filterIndex = 0;
  int langIndex = 0;

  if (!isProject)
    //{{{  enter all language file filters
    {
    cLanguage* lang = cLanguage::m_firstLanguage;

    while (lang) {
      if (lang->m_name && lang->m_extensions) {
        //{{{  append the language file filter
        strFilter += _T(lang->m_name);

        const char* ext = lang->m_extensions;
        while (*ext == ' ') ext++;
        if (*ext) {
          strFilter += _T(" (*.");
          while (*ext > ' ')
            strFilter += TCHAR(*ext++);
          while (*ext == ' ') ext++;
          while (*ext) {
            strFilter += _T(";*.");
            while (*ext > ' ')
              strFilter += TCHAR(*ext++);
            while (*ext == ' ') ext++;
            }
          strFilter += TCHAR(')');
          }
        strFilter += (TCHAR)'\0';   // next string please

        ext = lang->m_extensions;
        while (*ext == ' ') ext++;
        if (*ext) {
          strFilter += _T("*.");
          while (*ext > ' ')
            strFilter += TCHAR(*ext++);
          while (*ext == ' ') ext++;
          while (*ext) {
            strFilter += _T(";*.");
            while (*ext > ' ')
              strFilter += TCHAR(*ext++);
            while (*ext == ' ') ext++;
            }
          }
        strFilter += (TCHAR)'\0';   // next string please
        //}}}
        langIndex += 1;
        if (defaultExt && lang->valid_extension (defaultExt) && !filterIndex)
          filterIndex = langIndex;
        }
      lang = lang->next;
      }
    }
    //}}}

  //{{{  append the "*.fed" project file filter
  strFilter += _T("Fed Projects (*.fed)");
  strFilter += (TCHAR)'\0';   // next string please
  strFilter += _T("*.fed");
  strFilter += (TCHAR)'\0';   // next string please
  langIndex += 1;
  //}}}

  if (!isProject)
    //{{{  append the "*.*" all files filter
    {
    CString allFilter;
    VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
    strFilter += allFilter;
    strFilter += (TCHAR)'\0';   // next string please
    strFilter += _T("*.*");
    strFilter += (TCHAR)'\0';   // last string
    langIndex += 1;
    }
    //}}}

  return filterIndex ? filterIndex : langIndex;
  }
//}}}
//{{{
bool cFedView::DoPromptFileName (CString& filename, const char* defaultExt, BOOL bOpenFileDialog) {

  const int kFileNameBufLen = 512;
  LPTSTR filenameBuf = filename.GetBuffer (kFileNameBufLen);

  if (!defaultExt) {
    defaultExt = strrchr (filenameBuf, '.');
    if (defaultExt)
      defaultExt++;
    }
  bool isProject = defaultExt ? (_stricmp (defaultExt, "fed") == 0) : false;

  CFileDialog dlgFile (bOpenFileDialog, defaultExt, filenameBuf);

  CString title;
  if (bOpenFileDialog)
    VERIFY (title.LoadString (AFX_IDS_OPENFILE));
  else if (isProject)
    title = "Save Project As";
  else
    VERIFY (title.LoadString (AFX_IDS_SAVEFILE));

  dlgFile.m_ofn.Flags |= OFN_CREATEPROMPT | OFN_EXPLORER | OFN_OVERWRITEPROMPT;

  CString strFilter;
  dlgFile.m_ofn.nFilterIndex = GetFilters (strFilter, filenameBuf, defaultExt);
  dlgFile.m_ofn.lpstrFilter = strFilter;
  dlgFile.m_ofn.lpstrTitle = title;
  dlgFile.m_ofn.lpstrFile = filenameBuf;
  dlgFile.m_ofn.nMaxFile = kFileNameBufLen;

  bool result = (dlgFile.DoModal() == IDOK) ? true : false;

  filename.ReleaseBuffer();
  return result;
  }
//}}}
//{{{
void cFedView::DisplayAtLine (cTextLine* line) {

  bool view_changed = false;
  if (line)
    {
    if (m_curTop->isProj() && !line->isFile())
      return;  // shouldn't happen !!
    if (!line->isWithin (m_curFile))
      return;  // shouldn't happen !!
    if (!line->isWithin (m_curTop)) {
      if (!m_curTop->isWithin(m_curFile))
        {
        Beep ("Internal error 1");
        return;  // shouldn't happen !!
        }
      m_curTop = m_curFile;
      view_changed = true;
      }
    view_changed |= line->OpenToTop (m_view, m_curFile);

    if (view_changed) {
      SetupScrollBars();
      Invalidate();
      }

    int y = FindLine (line);
    if (y < 0)    // failed to find the line in the current view
      return;

    CPoint pos = GetScrollPosition();
    int ypos = y * m_cChar.cy;
    int ymargin = m_ScrollMargin.cy * m_cChar.cy;
    int yscroll = pos.y;

    if (ypos < ymargin)
      yscroll = 0;
    else if (ypos < pos.y + ymargin)
      yscroll = ypos - ymargin;
    else if (ypos > pos.y + m_cWlen.cy - ymargin)
      yscroll = ypos - ymargin;

    if (yscroll != pos.y) {
      SetScrollPos (SB_VERT, yscroll);
      Invalidate();
      }

    if (line->isFileLine())
      m_pos.x = 0;
    if ((y == 0) && line->isFile())
      y = 1;
    else if ((y > 1) && line->isProjLine() && !line->isProj())
      y -= 1;

    SetCursorPos (m_pos.x, y);
    }
  }
//}}}
//{{{
void cFedView::DisplayEnd() {

  if (!m_curTop)
    return;

  cTextLine* cur = m_curTop;
  cTextLine* last = m_curTop->FoldEnd();

  int y = 1;
  while (cur && (cur != last)) {
    y++;
    cur = cur->NextLine (m_view);
    }

  int yscroll = y * m_cChar.cy - m_cWlen.cy;
  if (yscroll < 0)
    yscroll = 0;

  SetScrollPos (SB_VERT, yscroll);
  SetCursorPos (0, y);
  Invalidate();
  }
//}}}

//{{{
CString cFedView::GetSelectString() {

  CString str;

  if (m_selectActive && m_selectRange.isSingleLine())
    {
    int len = m_selectRange.Xlen();
    if ((len > 0) && m_selectRange.Line())
      {
      cTextLine* line = m_selectRange.Line();
      int pos = m_selectRange.Xpos() - line->GetFoldIndent (m_curTop);
      if (line == m_curTop)
        pos += line->Indent();

      cTextFold* fold = line->isFold();
      if (fold)
        {
        pos -= 5;           // adjust by fold mark characters
        line = fold->GetTextLine (m_view);
        if (line != fold)   // adjust for indent difference of fold and actual line
          pos += line->Indent() - fold->Indent();
        }

      line->copy_text (str.GetBuffer (len + 1), pos, len); // cText call - knows about indent only
      str.ReleaseBuffer();
      }
    }

  return str;
  }
//}}}
//{{{
bool cFedView::MatchBracket() {

  m_displayMatch = false;
  const char open_str [] = "({[";
  const char close_str [] = ")}]";
  cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
  if (line && !line->isFoldLine()) {
    int xp = m_pos.x - line->GetIndent (m_curTop);
    if ((xp < 0) || (xp >= line->TextLen()))
      return false;   // not on a bracket character
    if (line->get_format_colour (xp) != c_Text)
      return false;
    const char* linetext = line->Text();
    const char* text = linetext + xp;
    const char* br_open = strchr (open_str, *text);
    const char* br_close = strchr (close_str, *text);

    if (br_open) {
      br_close = close_str + (br_open - open_str);
      //{{{  search down for matching close bracket
      text++;
      int ypos = m_pos.y;
      int closed_fold_count = 0;
      int bracket_count = 0;
      while (line && (line != m_curTop->FoldEnd())) {
        while ((!text || !*text) && line && (line != m_curTop->FoldEnd())) {
          line = line->Next();
          if (closed_fold_count <= 0)
            ypos++;
          if (line) {
            if (!line->isFoldLine()) {
              linetext = line->Text();
              text = linetext;
              }
            else if (line->isFold() && !line->isFold()->isOpen(m_view))
              closed_fold_count += 1;
            else if (line->isFoldEnd() && !line->isFoldEnd()->isOpen(m_view))
              closed_fold_count -= 1;
            }
          }

        while (text && *text) {
          const char* p_open = strchr (text, *br_open);
          const char* p_close = strchr (text, *br_close);
          if (p_open && (!p_close || (p_open < p_close))) {
            int xp = int(p_open - linetext);
            text = p_open + 1;
            if (line->get_format_colour (xp) == c_Text)
              bracket_count += 1;
            }
          else if (p_close) {
            int xp = int(p_close - linetext);
            if (line->get_format_colour (xp) == c_Text) {
              if (bracket_count-- <= 0) {
                int x = int(p_close - linetext);
                m_matchPos.x = x + line->GetIndent(m_curTop);
                m_matchPos.y = ypos;
                m_displayMatch = (closed_fold_count <= 0);
                return true;
                }
              }
            text = p_close + 1;
            }
          else
            text = 0;  // do next line
          }
        }
      //}}}
      }
    else if (br_close) {
      br_open = open_str + (br_close - close_str);
      //{{{  search up for matching open bracket
      text--;
      int ypos = m_pos.y;
      int closed_fold_count = 0;
      int bracket_count = 0;
      while (line && (line != m_curTop)) {
        while ((!text || (text < linetext)) && line && (line != m_curTop)) {
          line = line->Prev();
          if (closed_fold_count <= 0)
            ypos--;
          if (line) {
            if (!line->isFoldLine()) {
              linetext = line->Text();
              text = 0;
              if (linetext && *linetext)
                text = strchr (linetext, 0) - 1;
              }
            else if (line->isFoldEnd() && !line->isFoldEnd()->isOpen(m_view))
              closed_fold_count += 1;
            else if (line->isFold() && !line->isFold()->isOpen(m_view))
              closed_fold_count -= 1;
            }
          }

        while (text && (text >= linetext)) {
          //{{{  find prev open bracket
          const char* p_open = text;
          while ((p_open > linetext) && (*p_open != *br_open))
            p_open--;
          if (*p_open != *br_open)
            p_open = 0;
          //}}}
          //{{{  find prev close bracket
          const char* p_close = text;
          while ((p_close > linetext) && (*p_close != *br_close))
            p_close--;
          if (*p_close != *br_close)
            p_close = 0;
          //}}}
          if (p_close && (!p_open || (p_close > p_open))) {
            int xp = int(p_close - linetext);
            text = p_close - 1;
            if (line->get_format_colour (xp) == c_Text)
              bracket_count += 1;
            }
          else if (p_open) {
            int xp = int(p_open - linetext);
            if (line->get_format_colour (xp) == c_Text) {
              if (bracket_count-- <= 0) {
                int x = int(p_open - linetext);
                m_matchPos.x = x + line->GetIndent(m_curTop);
                m_matchPos.y = ypos;
                m_displayMatch = (closed_fold_count <= 0);
                return true;
                }
              }
            text = p_open - 1;
            }
          else
            text = 0;  // do next line
          }
        }
      //}}}
      }
    }

  return false;
  }
//}}}
//{{{
void cFedView::Unselect() {

  if (m_selectActive) {
    // unselect and redisplay selected area
    m_selectActive = FALSE;
    CClientDC dc (this);
    OnPrepareDC (&dc, NULL);
    dc.SetViewportOrg (-GetScrollPosition());

    if (m_curTop) {
      int y = m_selectRange.FirstYpos();
      cTextLine* cur = m_curTop ? m_curTop->GetLine (m_view, y) : 0;
      cTextLine* last = m_curTop ? m_curTop->FoldEnd() : 0;
      // --- add here 'only in window' optimisation

      int fold_indent = cur ? cur->GetFoldIndent (m_curTop) : 0;
      if (cur) {
        int fold_indent;
        if (cur != m_curTop)
          fold_indent = cur->GetFoldIndent (m_curTop);
        else
          fold_indent = -m_curTop->Indent();

        HideCaret();
        while (cur && (y <= m_selectRange.LastYpos())) {
          fold_indent = DrawText (&dc, cur, y, fold_indent);
          if (cur == last)
            break;
          y += 1;
          cur = cur->NextLine (m_view);
          }
        ShowCaret();
        }
      }
    }
  }
//}}}
//{{{
CPoint cFedView::TranslateScreenPos (CPoint point) {

  CPoint pos = GetScrollPosition();
  pos.x = (pos.x + point.x - m_xStart + (m_cChar.cx / 2) - 1) / m_cChar.cx;
  pos.y = (pos.y + point.y) / m_cChar.cy;

  if (m_curTop)
    pos.y = m_curTop->CheckValid (m_view, pos.y);

  return pos;
  }
//}}}
//{{{
BOOL cFedView::IsLeftMargin (int x) {

  return (GetScrollPosition().x + x < m_xStart);
  }
//}}}

//{{{
cTextFile* cFedView::CurFile()
{
  cTextFile* file = m_curFile;
  if (m_curFile->isProj()) {
    cTextLine* line = m_curTop->GetLine(m_view, m_pos.y);
    if (line && line->isFile())
      file = line->isFile();
    else
      file = m_curFile->isProj()->LastFile();
    }

  return file;
}
//}}}
//{{{
bool cFedView::CheckFileTime (const char* title, cTextFile* fileToCheck) {

  cTextFile* file = fileToCheck ? fileToCheck : m_curFile;

  if (file) {
    CString msg;
    const char* filename = file->Pathname();
    msg.Format ("File %s is newer on Disk\n\n", filename ? filename : "?");

    if (file->CheckTime().GetTime() > CTime (33, 0).GetTime())
      msg += file->CheckTime().Format ("%c  Last Check time\n");
    else
      msg += "not Checked before\n";

    msg += file->FileTime().Format ("%c  File time in Fed\n");

    if (file && file->isNewerOnDisk()) {
      msg += file->CheckTime().Format ("%c  File time on Disk\n");
      msg += "\n\nReload the file ?";
      if (MessageBox (msg, title, MB_YESNO) == IDYES)
        return true;
      }
    }

  return false;
  }
//}}}

//{{{
cTextFold* cFedView::getNewOutputBufferFold (const char* name) {

  cTextFold* fold = cFedDoc::getOutputBuffer();
  if (fold) {
    fold->CloseAll(-1);
    cTextLine* after = 0;
    if (fold->FoldEnd())
      after = fold->FoldEnd()->Prev();
    if (after) {
      // count number of folds & limit to 10
      int count = 0;
      cTextLine* cur = fold->Next();
      while (cur && cur->isFold()) {
        cur = cur->NextLine(0);  // view=0 - all folds will be closed!
        count++;
        }
      if (count >= 10)
        delete fold->Next();

      fold = new cTextFold (fold, after);
      (new cTextFoldEnd (fold, fold))->SetEdited(false);
      if (fold) {
        fold->set_text (name);
        fold->Open(-1);
        fold->SetEdited(false);
        fold->SetCommentsOnly(false);
        }

      }
    }

  return fold;
  }
//}}}
//{{{
const unsigned char* cFedView::DoMacro (const unsigned char* macro, int conditional, int level) {
// conditional -> -1 = skip section  0 = unconditional 1 = perform section
// format changed - default is command, 255 now toggles between command & text

  if (macro) {
    cFedDoc* pDoc = GetDocument();
    while (*macro) {
      if (*macro == 255) {
        // insert a string
        macro++;
        const unsigned char* str = macro;
        while (*macro && (*macro != 255)) macro++;
        if ((macro > str) && (conditional >= 0)) {
          if (m_selectActive) {
            //{{{  first delete the select range
            m_selectActive = false;
            pDoc->Delete (m_selectRange);
            SetCursorPos (m_selectRange.Xpos(), m_selectRange.Ypos());
            //}}}
            }
          cRange range (m_view, m_curFile, m_curTop, m_pos);
          m_pos.x += pDoc->InsertText (range, (char*) str, (int) (macro - str));
          }
        if (*macro == 255) macro++;
        }

      else {
        int token = *macro++;
        bool cond = false;
        bool not  = false;
        switch (token) {
          //{{{
          case k_if:
            {
            cRange range (m_view, m_curFile, m_curTop, m_pos);
            token = *macro++;
            if (token == k_not) {
              not = true;
              token = *macro++;
              }
            if (conditional < 0)
              macro = DoMacro (macro, -2, level + 1);
            else {
              switch (token) {
                case k_blank:   cond = range.isBlankLine(); break;
                case k_alpha:   cond = range.isAlpha(); break;
                case k_match:
                  cond = true;
                  if (*macro && (*macro == 255)) {
                    macro++;
                    const unsigned char* str = macro;
                    while (*macro && (*macro != 255)) macro++;
                    int len = int (macro - str);
                    if (len > 0)
                      cond = range.isMatch((const char*) str, len);
                    if (*macro == 255) macro++;
                    }
                  break;
                case k_comment: cond = range.isComment(); break;
                }
              cond ^= not;
              macro = DoMacro (macro, cond ? 1 : -1, level + 1);
              }
            }
            break;
          //}}}
          //{{{
          case k_else:
            if (conditional == -1)
              conditional = 1;
            else if (conditional == 1)
              conditional = -1;
            break;
          //}}}
          //{{{
          case k_end:
            if (conditional)
              return macro;
            break;
          //}}}
          //{{{
          case k_while:
            {
            token = *macro++;
            if (token == k_not) {
              not = true;
              token = *macro++;
              }
            const unsigned char* loop_point = macro;
            int loop_count = 0;
            if (conditional < 0)
              macro = DoMacro (macro, -2, level + 1);
            else {
              cond = true;
              while (cond) {
                macro = loop_point;
                cond = false;
                cRange range (m_view, m_curFile, m_curTop, m_pos);
                switch (token) {
                  case k_blank:   cond = range.isBlankLine(); break;
                  case k_alpha:   cond = range.isAlpha(); break;
                  case k_match:
                    cond = true;
                    if (*macro && (*macro == 255)) {
                      macro++;
                      const unsigned char* str = macro;
                      while (*macro && (*macro != 255)) macro++;
                      int len = int (macro - str);
                      if (len > 0)
                        cond = range.isMatch((const char*) str, len);
                      if (*macro == 255) macro++;
                      }
                    break;
                  case k_comment: cond = range.isComment(); break;
                  }
                cond ^= not;
                macro = DoMacro (macro, cond ? 1 : -1, level + 1);
                if (loop_count++ > 10000) {
                  Beep ("ERROR: Macro stuck in loop");
                  macro = (const unsigned char*) strchr ((const char*) macro, 0);
                  return macro;
                  }
                }
              }
            }
            break;
          //}}}
          //{{{
          case k_domacro:
            {
            if (*macro && (*macro == 255)) {
              macro++;
              const unsigned char* str = macro;
              while (*macro && (*macro != 255)) macro++;
              int len = int (macro - str);
              if (*macro == 255) macro++;
              if ((len > 0) && (conditional >= 0)) {
                cRange range (m_view, m_curFile, m_curTop, m_pos);
                const unsigned char* sub_macro = range.FindMacro((const char*) str, len);
                if (sub_macro)
                  DoMacro (sub_macro, 0, level + 1);
                }
              }
            }
            break;
          //}}}
          case k_MacroDialog:   break;
          case k_MacroLearn:    break;
          case k_MacroCall:     break;
          case k_MacroLoad:     break;
          case k_MacroSave:     break;
          //{{{
          default:
            if (conditional >= 0)
              KeyProc (token);
          //}}}
          }
        }
      }
    }

  return macro;
  }
//}}}

//{{{
CString cFedView::SubstitutePath (const char* format, const char* path,
                     int flags, const char* comment,  const char* selection, CString* parsed_dir) {

  CString result;
  if (format && *format && path && *path)
    {
    //{{{  extract parts of path
    CString lowerpath (path);
    lowerpath.MakeLower();

    CString drive (path);
    CString dir;
    CString curdir;
    CString file;
    CString name;
    CString ext;

    char* pDrive = drive.GetBuffer(0);
    if (pDrive) {
      char* pColon = strchr (pDrive, ':');
      char* pSep = strrchr (pDrive, '\\');
      char* pName = pDrive;
      if (pSep)
        pName = pSep + 1;
      else if (pColon)
        pName = pColon + 1;
      file = pName;

      char* pDot = strrchr (pName, '.');
      if (pDot)
        {
        char* pExt = pDot + 1;
        ext = pExt;
        *pDot = 0;   // strip Extension off drive String
        }
      name = pName;

      if (pSep)
        *pSep = 0;   // strip Filename off drive String
      else
        *pName = 0;  // strip Filename off drive String

      pSep = strrchr (pDrive, '\\');
      if (pSep)
        curdir = pSep + 1;
      else if (pColon)
        curdir = pColon + 1;
      else if (*pDrive)
        curdir = pDrive;
      else
        curdir = ".";

      if (pColon)
        dir = pColon + 1;
      else if (*pDrive)
        dir = pDrive;
      else
        dir = ".";

      if (pColon)
        *(pColon + 1) = 0;
      else
        pDrive = 0;

      drive.ReleaseBuffer();
      }

    if (parsed_dir) {
      *parsed_dir += drive;
      *parsed_dir += dir;
      }
    //}}}
    CString linenumber;
    linenumber.Format ("%d", m_lineNumber);

    char separator = 0;
    bool in_conditional = false;
    bool flagged = false;
    CString source (format);
    char* fmt = source.GetBuffer(0);
    while (*fmt)
      {
      char* cur = fmt;
      while (*cur && (*cur != '%') && (*cur != '?') && (*cur != separator))
        cur++;
      int len = int(cur - fmt);   // length of input format to be copied
      char ch = *cur;
      if (*cur)
        *cur++ = 0;          // overwrite special-char to terminate the string-so-far
      if (len > 0)
        result += fmt;       // copy up-to special-char

      if (ch == '%')
        //{{{  handle Substitution
        {
        if (*cur)
          ch = *cur++;

        switch (ch)
          {
          case 'P': result += path; break;
          case 'p': result += lowerpath; break;
          case 'c': result += curdir; break;
          case 'D': result += drive;  break;
          case 'd': result += dir;  break;
          case 'f': result += file; break;
          case 'n': result += name; break;
          case 'e': result += ext;  break;
          case 'l': result += linenumber; break;
          case 'C': result += comment;    break;
          case 'S': result += selection;  break;
          default:  result += ch;   break;
          }
        }
        //}}}
      else if (separator && (ch == separator)) {
        if (flagged)
          //{{{  skip the condition-false string
          {
          while (*cur && (*cur != separator)) if (*cur++ == '%') cur++;
          if (*cur) cur++;  // skip separator char
          }
          //}}}
        separator = 0;  // flag that we are no longer in a conditional section
        }
      fmt = cur;
      }
    }

  return result;
  }
//}}}
