// Range.cpp
//{{{  includes
#include "pch.h"
#include "range.h"
//}}}

//{{{
cRange::cRange()
  : m_view (0),
    m_file (0),
    m_topFold (0),
    m_startPos (0, 0),
    m_endPos (0, 0),
    m_column (FALSE),
    m_firstLine (0),
    m_lastLine (0)
{}
//}}}
//{{{
cRange::cRange (cRange& range)
  : m_view (range.m_view),
    m_file (range.m_file),
    m_topFold (range.m_topFold),
    m_startPos (range.m_startPos),
    m_endPos (range.m_endPos),
    m_column (range.m_column),
    m_firstLine (0),
    m_lastLine (0)
{}
//}}}
//{{{
cRange::cRange (int view, cTextFile* file)
  : m_view (view),
    m_file (file),
    m_topFold (file),
    m_startPos (0, 0),
    m_endPos (0, 0),
    m_column (FALSE),
    m_firstLine (0),
    m_lastLine (0)
{}
//}}}
//{{{
cRange::cRange (int view, cTextFile* file, cTextFold* fold, CPoint pos)
  : m_view (view),
    m_file (file),
    m_topFold (fold),
    m_startPos (pos),
    m_endPos (pos),
    m_column (FALSE),
    m_firstLine (0),
    m_lastLine (0)
{}
//}}}
//{{{
cRange::cRange (int view, cTextFile* file, cTextFold* fold, CPoint pos1, CPoint pos2)
  : m_view (view),
    m_file (file),
    m_topFold (fold),
    m_startPos (pos1),
    m_endPos (pos2),
    m_column (FALSE),
    m_firstLine (0),
    m_lastLine (0)
{
  if ((pos1.y > pos2.y) || ((pos1.y == pos2.y) && (pos1.x > pos2.x))) {
    m_startPos  = pos2;
    m_endPos    = pos1;
    }
  else {
    m_startPos  = pos1;
    m_endPos    = pos2;
    }
}
//}}}

//{{{
void cRange::Set (int view, cTextFile* file, cTextFold* fold, CPoint pos1, CPoint pos2)
{
  m_view      = view;
  m_file      = file;
  m_topFold   = fold;

  if (m_column) {
    // handle x & y independently
    if (pos1.x > pos2.x) {
      m_startPos.x = pos2.x;
      m_endPos.x = pos1.x;
      }
    else {
      m_startPos.x = pos1.x;
      m_endPos.x = pos2.x;
      }
    if (pos1.y > pos2.y) {
      m_startPos.y = pos2.y;
      m_endPos.y = pos1.y;
      }
    else {
      m_startPos.y = pos1.y;
      m_endPos.y = pos2.y;
      }
    }
  else if ((pos1.y > pos2.y) || ((pos1.y == pos2.y) && (pos1.x > pos2.x))) {
    m_startPos  = pos2;
    m_endPos    = pos1;
    }
  else {
    m_startPos  = pos1;
    m_endPos    = pos2;
    }

  m_firstLine = 0;
  m_lastLine  = 0;
}
//}}}
//{{{
void cRange::SetFile (cTextFile* file)
{
  m_file = file;
  m_topFold = file;
  m_startPos  = CPoint (0, 0);
  m_endPos    = CPoint (0, 0);
  m_firstLine = 0;
  m_lastLine  = 0;
}
//}}}
//{{{
void cRange::SetTopFold (cTextFold* fold)
{
  m_topFold = fold;
  m_startPos  = CPoint (0, 0);
  m_endPos    = CPoint (0, 0);
  m_firstLine = 0;
  m_lastLine  = 0;
}
//}}}
//{{{
void cRange::SetRange (CPoint pos)
{
  m_startPos  = pos;
  m_endPos    = pos;
  m_firstLine = 0;
  m_lastLine  = 0;
}
//}}}
//{{{
void cRange::SetRange (CPoint pos1, CPoint pos2)
{
  if ((pos1.y > pos2.y) || ((pos1.y == pos2.y) && (pos1.x > pos2.x))) {
    m_startPos  = pos2;
    m_endPos    = pos1;
    }
  else {
    m_startPos  = pos1;
    m_endPos    = pos2;
    }

  m_firstLine = 0;
  m_lastLine  = 0;
}
//}}}
//{{{
void cRange::SetRange (cTextLine* line)
{
  m_firstLine = line;
  m_lastLine = line;
}
//}}}
//{{{
void cRange::SetRange (cTextLine* line1, cTextLine* line2)
{
  m_firstLine = line1;
  m_lastLine = line2;
}
//}}}
//{{{
void cRange::ExtendToFoldEnd()
{
  //changes both m_endPos.y and m_lastLine
  cTextFold* fold = LastLine()->isFold();
  if (fold) {
    cTextFoldEnd* foldend = fold->FoldEnd();
    if (foldend) {
      if (fold->isOpen(m_view)) {
        cTextLine* cur = fold;
        while (cur && (cur != foldend)) {
          m_endPos.y += 1;
          cur = cur->NextLine(m_view);
          }
        }
      m_lastLine = foldend;
      }
    }
}
//}}}
//{{{
void cRange::ExtendToWholeLines()
{
  m_startPos.x = 0;
  if (m_endPos.x > 0) {
    m_endPos.x = 0;
    m_endPos.y += 1;
    m_lastLine = 0;         // force a re-calc later
    }
}
//}}}

//{{{
bool cRange::Reformat()
{
  // returns true if range is extended to cover extra reformatted lines
  if (Language()) {
    cTextLine* cur  = FirstLine();
    cTextLine* last = LastLine();
    cTextLine* limit = File()->FoldEnd();

    //    adjust range for a single copied fold line
    if ((cur == last) && cur->isFold()) {
      cTextLine* actual_line = cur->isFold()->GetTextLine (m_view);
      if (cur != actual_line) {
        cur = actual_line;
        last = actual_line;
        }
      }

    // reformat all of range except the last line
    while (cur && (cur != last) && (cur != limit)) {
      cur->SetFormat (Language());
      cur = cur->Next();
      }

    // reformat last and any following lines while until outfmt is changed
    while (cur && (cur != limit) && cur->SetFormat (Language()))
      cur = cur->Next();
    if (cur != last) {
      m_lastLine = cur;
      return true;
      }
    }

  return false;
}
//}}}
//{{{
cTextLine* cRange::EditLine()
{ // assumes single line

  if ((Line()->isFold()) && (m_startPos.x >= 5 + Line()->Indent() + Line()->GetFoldIndent (m_topFold))) {
    m_lastLine = Line()->isFold()->GetTextLine(m_view); // and extend range
    if (m_lastLine != m_firstLine) {
      m_startPos.x -= 5;
      m_endPos.x -= 5;
      }
    return m_lastLine;
    }
  return m_firstLine;
}
//}}}
//{{{
cTextLine* cRange::set_firstLine()
{
  if (m_topFold) {
    m_firstLine = m_topFold->GetLine (m_view, m_startPos.y);
    if (m_endPos.y == m_startPos.y)
      m_lastLine = m_firstLine;
    }
  return m_firstLine;
}
//}}}
//{{{
cTextLine* cRange::set_lastLine()
{
  if (m_topFold) {
    m_lastLine = m_topFold->GetLine (m_view, m_endPos.y);
    if (m_startPos.y == m_endPos.y)
      m_firstLine = m_lastLine;
    }
  return m_lastLine;
}
//}}}

//{{{
bool cRange::isBlankLine()
{
  if (!m_firstLine)
    set_firstLine();
  if (m_firstLine) {
    const char* text = m_firstLine->GetText (m_view);
    if (text && *text)
      return false;
    }

  return true;
}
//}}}
//{{{
bool cRange::isComment()
{
  if (!m_firstLine)
    set_firstLine();
  if (m_firstLine && m_firstLine->isComment())
    return true;

  return isBlankLine();
}
//}}}
//{{{
bool cRange::isAlpha()
{
  if (!m_firstLine)
    set_firstLine();
  if (m_firstLine) {
    char ch = m_firstLine->GetChar (m_view, m_startPos.x);
    if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')))
      return true;
    }

  return false;
}
//}}}
//{{{
bool cRange::isMatch (const char* str, int len)
{
  if (!str || !*str)
    return true;
  if (!m_firstLine)
    set_firstLine();
  if (m_firstLine) {
    if (len <= 0)
      len = (int)strlen(str);
    const char* txt = m_firstLine->GetTextAt (m_view, m_startPos.x);
    if (txt && (strlen(txt) >= (unsigned) len) && (_memicmp (txt, str, len) == 0))
      return true;
    }
  return false;
}
//}}}
//{{{
const unsigned char* cRange::FindMacro (const char* name, int len)
{
  if (Language()) {
    cMacro* cur = Language()->FirstMacro();
    while (cur) {
      const char* macro_name = cur->Name();
      if (macro_name) {
        int namelen = (int)strlen(macro_name);
        if ((namelen >= len) && (_memicmp (name, macro_name, len) == 0))
          return cur->Macro();
        }
      cur = cur->next;
      }
    }

  return 0;
}
//}}}

//{{{
char* cRange::CopyRangeText()
{
  cLanguage* language       = m_file        ? m_file->Language()        : 0;
  const char* comment_start = language      ? language->m_commentStart1 : 0;
  const char* comment_end   = language      ? language->m_commentEnd1   : 0;

  int comment_start_len     = comment_start ? (int)strlen(comment_start)     : 0;
  int comment_end_len       = comment_end   ? (int)strlen(comment_end)       : 0;

  char openmark [4]  = "{{{";
  char closemark [4] = "}}}";

  if (comment_start && strchr(comment_start, '{')) {
    strcpy (openmark, "<<<");
    strcpy (closemark, ">>>");
    }

  cTextLine* line1 = FirstLine();
  cTextLine* line2 = LastLine();
  int slen1 = line1->GetTextLine(m_view)->TextLen() + line1->GetTextOffset (m_topFold);
  int slen2 = line2->GetTextLine(m_view)->TextLen() + line2->GetTextOffset (m_topFold);
  DWORD len = 1;
  if (m_column)
    // count length of column copy heap memory required
    {
    if ((Xlen() > 0) && (Ylen() > 0))
      len += (Xlen() + 2) * Ylen() - 2;
    }
  else
    //{{{  count length of heap memory required
    if (isSingleLine()) {
      if (LastXpos() <= slen1)
        len += Xlen();
      else if (Xpos() < slen1)
        len += slen1 - Xpos();
      }
    else {
      int fold_indent = 0;
      cTextLine* cur = line1;
      if (Xpos() > 0) {
        len += (slen1 > Xpos()) ? slen1 - Xpos() + 2 : 2;
        cur = cur->NextLine (m_view);
        }

      while (cur && (cur != line2)) {
        int copylen = fold_indent + cur->Indent() + cur->TextLen();
        if (cur->isFold() || cur->isFoldEnd()) {
          len += comment_start_len + 3 + comment_end_len;
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

      len += (slen2 > LastXpos()) ? LastXpos() : slen2;
      }
    //}}}

  char* buffer = new char [len];
  if (m_column)
    //{{{  column copy to heap memory
    {
    char* pData = buffer;
    while (line1)
      {
      //{{{  copy text from a single line
      {
      int x = Xpos();
      int copylen = 0;

      if (LastXpos() <= slen1)
        copylen = Xlen();
      else if (x < slen1)
        copylen = slen1 - x;

      int indent = line1->GetTextOffset (m_topFold);

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
    //{{{  copy text to heap memory
    {
    char* pData = buffer;
    if (isSingleLine()) {
      int x = Xpos();
      int copylen = 0;

      if (LastXpos() <= slen1)
        copylen = Xlen();
      else if (x < slen1)
        copylen = slen1 - x;

      int indent = line1->GetTextOffset (m_topFold);

      if (x + copylen <= indent)
        memset (pData, ' ', copylen);
      else {
        if (x < indent) {
          memset (pData, ' ', indent - x);
          copylen -= indent - x;
          pData += indent - x;
          x = indent;
          }
        memcpy (pData, line1->GetText(m_view) + x - indent, copylen);
        }
      pData += copylen;
      }

    else {
      int fold_indent = 0;
      int copylen;
      cTextLine* cur = line1;
      if (Xpos() > 0) {
        int x = Xpos();
        copylen = slen1 - x;
        if (copylen > 0) {
          int indent = line1->GetTextOffset (m_topFold);
          if (x + copylen <= indent)
            memset (pData, ' ', copylen);
          else {
            if (x < indent) {
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

      while (cur && (cur != line2)) {
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

      copylen = (slen2 > LastXpos()) ? LastXpos() : slen2;
      if (copylen > 0) {
        int indent = line2->GetTextOffset (m_topFold);
        if (copylen <= indent)
          memset (pData, ' ', copylen);
        else {
          if (indent > 0) {
            memset (pData, ' ', indent);
            copylen -= indent;
            pData += indent;
            }
          memcpy (pData, line2->GetText(m_view), copylen);
          }
        pData += copylen;
        }
      }

    *pData = 0;
    }
    //}}}

  return buffer;
}
//}}}

// cChange
//{{{
cChange::cChange (cRange& range, const char* insert_text)
 : cRange (range),
   m_text (NULL)
{
  if (insert_text && *insert_text) {
    int len = (int)strlen(insert_text);
    m_text = new char [len + 1];
    strcpy (m_text, insert_text);
    }
}
//}}}
//{{{
cChange::cChange (cChange& change)
 : cRange (change),
   m_text (NULL)
{
  if (change.Text() && *change.Text()) {
    int len = (int)strlen(change.Text());
    m_text = new char [len + 1];
    strcpy (m_text, change.Text());
    }
}
//}}}
//{{{
cChange::~cChange()
{
  if (m_text)
    delete[] m_text;
}
//}}}

// cUndo
//{{{
cUndo::cUndo (cRange& range, const char* insert_text)
 : cChange (range, insert_text),
   prev (NULL)
{}
//}}}
//{{{
cUndo::cUndo (cChange& change)
 : cChange (change, change.CopyRangeText()),
   prev (NULL)
{
// undo is the reverse of change - so exchange range-length and insert-text
const char* pData = change.Text();
CPoint endpos (StartPos());

if (pData && *pData)
  {
  int maxlen = 0;
  const char* nl = strchr (pData, '\n');

  while (nl) // count newlines
    {
    if ((nl - pData) > maxlen)
      maxlen = (int)(nl - pData);
    endpos.y += 1;
    pData = nl + 1;
    nl = strchr (pData, '\n');
    }

  if (change.Column())
    endpos.x += maxlen - 1;
  else // add-in the length of the remaining string
    endpos.x = (int)strlen(pData);
  }

SetRange (StartPos(), endpos);
}
//}}}
//{{{
cUndo::~cUndo() {}
//}}}
//{{{
cUndo* cUndo::Add (cUndo* undo)
{
  if (undo) {
    if (!m_text && !undo->m_text && (undo->m_startPos == m_endPos)) {
      // merge typing
      m_endPos = undo->m_endPos;
      delete undo;
      return this;
      }
    else if (m_text && undo->m_text
             && isSinglePoint() && undo->isSinglePoint()
             && (undo->m_startPos.y == m_startPos.y)
             && ((undo->m_startPos.x == m_startPos.x) || (undo->m_startPos.x + (int)strlen(undo->m_text) == m_startPos.x)) ) {
      // merge deletions
      int len = (int)strlen(m_text) + (int)strlen(undo->m_text) + 1;
      char* newtext = new char [len];
      if (undo->m_startPos.x == m_startPos.x) {
        strcpy (newtext, m_text);
        strcat (newtext, undo->m_text);
        }
      else {
        SetRange (undo->Pos());
        strcpy (newtext, undo->m_text);
        strcat (newtext, m_text);
        }
      delete[] m_text;
      m_text = newtext;
      delete undo;
      return this;
      }
    else
      undo->prev = this;
    }

  return undo;
}
//}}}
