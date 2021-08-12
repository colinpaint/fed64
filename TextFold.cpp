// TextFold.cpp
//{{{  includes
#include "pch.h"
#include "textfold.h"

#include "colour.h"
#include "language.h"
#include "text.h"
#include "textline.h"
//}}}

//{{{
cTextFold::cTextFold (cTextFold* fold, cTextLine* previous, eType type)
 : cTextLine (fold, previous),
   fold_end (NULL),
   m_containsEdits (false),
   m_commentsOnly (true),
   opened (0),
   HiddenLines (0),
   above_x (0),
   above_y (0),
   above_scroll (0),
   above_level (0),
   below_x (0),
   below_y (0),
   below_scroll (0),
   LineCount(1),
   StreamAppend(false),
   Type(type)
{
}
//}}}
//{{{
cTextFold::~cTextFold()
{
  if (parent)
    parent->HiddenLines -= HiddenLines + 1;
  if (fold_end) {
    while (next && (next != fold_end))
      delete next;
    delete fold_end;
    }
}
//}}}
//{{{
char* cTextFold::show_debug (char* str) const
{
  //cTextFoldEnd* fold_end;
  //int HiddenLines;
  //bool m_containsEdits;
  //bool m_commentsOnly;
  //ubyte opened;               // opened flags (for up to 8 views)
  //long above_x;              // Pos-of-fold
  //long above_y;              // Pos-of-fold
  //long above_scroll;
  //int  above_level;          // number of folds-above opened
  //long below_x;              // Pos-in-fold
  //long below_y;              // Pos-in-fold
  //long below_scroll;

  sprintf (str, "cTextFold: cmt:%d edt:%d open:%02x LineCount:%d HiddenLines:%d",
      m_commentsOnly ? 1: 0,
      m_containsEdits ? 1: 0,
      (int) opened,
      LineCount,
      HiddenLines
      );
  char* s = strchr (str, 0);
  if ((!text || !*text) && next && !const_cast<cTextFold*>(this)->isFile()) {
    *s = 0;
    *s++ = ' ';
    *s++ = ' ';
    s = next->show_text (s);
    }
  *s++ = 0;
  sprintf (s, "  above x:%d y:%d scr:%d lvl:%d  below x:%d y:%d scr:%d",
      above_x, above_y, above_scroll, above_level,
      below_x, below_y, below_scroll
      );
  s = strchr (s, 0);
  *s++ = 0;
  *s++ = ' ';
  return cTextLine::show_debug (s);
}
//}}}

//{{{
void cTextFold::SetCommentsOnly (bool newval)
{
  if (m_commentsOnly && !newval && parent)
    parent->SetCommentsOnly(false);
  m_commentsOnly = newval;
}
//}}}
//{{{
void cTextFold::SetContainsEdits (bool newval)
{
  if (!m_containsEdits && newval && parent)
    parent->SetContainsEdits(true);
  m_containsEdits = newval;
}
//}}}

//{{{
void cTextFold::ReplaceText (int view, int xpos, int ch)
{ // assumes a closed fold
  if ((xpos <= indent) && (ch == ' ') && !isFoldEnd())
    indent += 1;
  else if (xpos >= indent + 5) {      // allow for fold mark
    xpos -= 5;
    if ((xpos > indent) || (ch != ' ')) {
      cTextLine* line = GetTextLine (view);
      if (line != this)
        line->overwrite_text (xpos - indent + line->Indent(), ch);
      else {
        overwrite_text (xpos, ch);
        SetEdited();
        }
      }
    }
}
//}}}
//{{{
void cTextFold::InsertText (int view, int xpos, int ch, int len)
{ // assumes a closed fold
  if ((xpos <= indent) && (ch == ' ') && !isFoldEnd())
    indent += 1;
  else if (xpos >= indent + 5) {      // allow for fold mark
    xpos -= 5;
    if ((xpos > indent) || (ch != ' ')) {
      cTextLine* line = GetTextLine (view);
      if (line != this)
        line->insert_text (xpos - indent + line->Indent(), ch);
      else {
        insert_text (xpos, ch, len);
        SetEdited();
        }
      }
    }
}
//}}}
//{{{
void cTextFold::InsertText (int view, int xpos, const char* str, int len)
{ // assumes a closed fold
  if (str && *str) {
    if (len <= 0)
      len = (int)strlen(str);
    const char* s = str;
    while (*s == ' ')
      s++;
    bool blank = (s >= str + len);

    if ((xpos <= indent) && blank) {  // insert a spaces-only-string into indent
      if (!isFoldEnd())
        indent += len;
      }
    else if (xpos >= indent + 5) {  // allow for fold mark
      xpos -= 5;
      if (xpos <= indent) {           // leading spaces only allowed within text
        len -= (int) (s - str);     // reduce length by leading spaces
        str = s;
        }

      if (len > 0) {
        cTextLine* line = GetTextLine (view);
        if (line == this)
          insert_text (xpos, str, len);
        else if (xpos >= 0) {
          line->insert_text (xpos - indent + line->Indent(), str, len);
          SetEdited();
          }
        }
      }
    }
}
//}}}
//{{{
void cTextFold::DeleteText (int view, int xpos, int len)
{ // assumes a closed fold
  if (xpos < indent + 5) {      // within indent or foldmark
    if (xpos + len <= indent) {
      indent -= len;
      return;
      }
    if (xpos < indent) {
      int sublen = indent - xpos;
      len -= sublen;             // remove rest of indent
      indent = xpos;
      //xpos += sublen;
      }
    //len -= 5;                    // cannot delete any of foldmark
    }

  xpos -= 5;                     // allow for foldmark
  if (xpos < indent) {
    len -= indent - xpos;
    xpos = indent;
    }

  if (len > 0) {
    cTextLine* line = GetTextLine (view);
    if (line != this)
      line->delete_text (xpos - indent + line->Indent(), len);
    else {
      delete_text (xpos, len);
      SetEdited();
      }
    }
}
//}}}
//{{{
void cTextFold::InsertText (const char* pData, int len)
{
  if (pData && *pData) {
    if (len <= 0)
      len = (int)strlen(pData);

    cTextLine* cur = this;
    while (*pData && (len > 0) && cur) {
      const char* eol = strchr (pData, '\n');
      if (eol && (eol < pData + len)) {
        int insertlen = (int) (eol - pData);
        len -= insertlen + 1;
        if (*(eol - 1) == '\r')
          insertlen -= 1;
        cur = new cTextLine (this, cur);
        if (cur && (insertlen > 0))
          cur->set_text (pData, insertlen);
        pData = eol + 1;
        }
      else {
        cur = new cTextLine (this, cur);
        if (cur)
          cur->set_text (pData, len);
        break;
        }
      }
    }
}
//}}}
//{{{
void cTextFold::ToUpper (int view, int xpos, int len)
{ // assumes a closed fold
  if (xpos <= indent)             // skip foldmark
    len -= 5;
  else if (xpos < indent + 5) {   // start is within foldmark
    len -= indent + 5 - xpos;
    xpos = indent;
    }
  else
    xpos -= 5;

  if (len > 0) {
    cTextLine* line = GetTextLine (view);
    if (line != this)
      line->toupper_text (xpos - indent + line->Indent(), len);
    else {
      toupper_text (xpos, len);
      SetEdited();
      }
    }
}
//}}}
//{{{
void cTextFold::ToLower (int view, int xpos, int len)
{ // assumes a closed fold
  if (xpos <= indent)             // skip foldmark
    len -= 5;
  else if (xpos < indent + 5) {   // start is within foldmark
    len -= indent + 5 - xpos;
    xpos = indent;
    }
  else
    xpos -= 5;

  if (len > 0) {
    cTextLine* line = GetTextLine (view);
    if (line != this)
      line->tolower_text (xpos - indent + line->Indent(), len);
    else {
      tolower_text (xpos, len);
      SetEdited();
      }
    }
}
//}}}
//{{{
char* cTextFold::ExtractText (int view, int xpos, int len)
{
  char* result = 0;
  if (xpos < indent + 5) {      // within indent or foldmark
    if (xpos + len <= indent) {
      SetEdited();
      return extract_text (xpos, len);
      }
    if (xpos < indent) {
      int sublen = indent - xpos;
      len -= sublen;             // remove rest of indent
      indent = xpos;
      // no undo !!!
      SetEdited();
      }
    }

  xpos -= 5;                     // allow for foldmark
  if (xpos < indent) {
    len -= indent - xpos;
    xpos = indent;
    }

  if (len > 0) {
    cTextLine* line = GetTextLine (view);
    if (line != this)
      result = line->extract_text (xpos - indent + line->Indent(), len);
    else
      result = extract_text (xpos, len);
    SetEdited();
    }

  //absorb_leading();
  //trim_trailing();
  return result;
}
//}}}

//{{{
bool cTextFold::RevertToOriginal (int view)
{
  cTextLine* line = GetTextLine (view);
  if (line) {
    if (line->revert_to_original()) {
      line->SetEdited (false);
      return true;
      }
    }
  else if (revert_to_original()) {
    SetEdited (false);
    return true;
    }
  return false;
}
//}}}

//{{{
void cTextFold::RemoveFold (const cLanguage* language)
{

  if (fold_end) {
    //{{{  scan fold contents changing their parent to parent above & adjust indent
    cTextLine* cur = next;
    while (cur && (cur != fold_end)) {
      cur->change_indent (indent);
      cur->parent = parent;
      LineCount -= cur->lineCount();
      cur = cur->NextLine(0);
      }
    //}}}
    if (fold_end->prev)
      fold_end->prev->SetDeletedBelow();
    delete fold_end;
    fold_end = 0;
    }
  if (text) {
    cTextLine* newline = new cTextLine (parent, this);
    newline->GrabContents (this);
    if (language && newline->text) {
      //{{{  insert comment mark
      if (language->CommentStart()) {
        newline->insert_text (0, language->CommentStart());
        if (language->CommentEnd())
          newline->append_text (language->CommentEnd());
        }
      //}}}
      }
    }
  else if (prev)
    prev->SetDeletedBelow();

  delete this;
}
//}}}
//{{{
void cTextFold::CreateFoldEnd (cTextLine* after)
{
  if (!fold_end) {
    fold_end = new cTextFoldEnd (this, after ? after : this);
    if (fold_end) {
      //{{{  set fold indent to minimum indent of fold contents
      ubyte min_indent = 255;
      cTextLine* cur = next;
      while (cur && (cur != fold_end)) {
        if (cur->text && *(cur->text) && (cur->indent < min_indent))
          min_indent = cur->indent;
        cur = cur->NextLine(0);    //View bit=0, therefore fold always closed
        }
      if (min_indent < 255)
        indent = min_indent;
      //}}}
      //{{{  scan fold contents changing their parents & adjust indent
      int count = 0;
      cur = next;
      while (cur && (cur != fold_end)) {
        cur->change_indent (-indent);
        cur->parent = this;
        LineCount += cur->lineCount();
        cur = cur->NextLine(0);
        }
      //}}}
      }
    }
}
//}}}
//{{{
cTextLine* cTextFold::SplitLine (int view, int xpos)
{
  if (xpos >= indent + 5)
    xpos -= 5;             // correct for foldmark

  if (xpos < 0)
    xpos = 0;
  int pos = xpos - indent;

  cTextLine* newline;
  int new_indent = 0;

  if (fold_end && !(opened & view))
    newline = new cTextLine (parent, fold_end);
  else
    newline = new cTextLine (this, this);

  if (text && (pos >= 0) && (pos < (int) strlen (text)) ) {
    newline->set_text (text + pos);
    delete_to_eol (xpos);
    }

  if (newline && fold_end && !(opened & view))
    newline->set_indent (indent);

  return newline;
}
//}}}
//{{{
bool cTextFold::JoinLine (int view, int append_spaces)
{

  if (!fold_end)
    return cTextLine::JoinLine (view, append_spaces);

  else if ((opened & view) && next && !next->isFoldLine()) {
    if (next->GetText(view)) {
      int old_indent = indent;
      append_text (next->GetText(view), append_spaces, true);
      indent = old_indent;
      }
    delete next;
    return true;
    }
  return false;
}
//}}}

//{{{
cTextFoldEnd* cTextFold::Foldup (const cLanguage* language, int cur_indent, cTextLine* last)
{
  // searches for foldmark and creates a fold - then calls FoldUp iteratively
  // returns 'foldend'
  int tablen           = language ? language->Tablen (cLanguage::InText) : 8;
  const char* c_start  = language ? language->CommentStart() : 0;
  const char* c_end    = language ? language->CommentEnd() : 0;
  const char* c_start2 = language ? language->CommentStart2() : 0;
  const char* c_end2   = language ? language->CommentEnd2() : 0;

  char openmark [4]  = "{{{";
  char closemark [4] = "}}}";
  if (c_start && strchr(c_start, '{')) {
    strcpy (openmark,  "<<<");
    strcpy (closemark, ">>>");
    }
  const int marklen = 3;

  int startlen  = c_start  ? (int)strlen(c_start)  : 0;
  int endlen    = c_end    ? (int)strlen(c_end)    : 0;
  int startlen2 = c_start2 ? (int)strlen(c_start2) : 0;
  int endlen2   = c_end2   ? (int)strlen(c_end2)   : 0;
  int openlen   = startlen  + marklen + endlen;
  int openlen2  = startlen2 + marklen + endlen2;
  HiddenLines  = 0;
  int line_count = 1;

  cTextLine* limit = last ? last : fold_end;
  cTextLine* prevline = this;
  cTextLine* cur = next;
  while (cur && (cur != limit)) {
    prevline = cur;
    cur->parent = this;
    HiddenLines += 1;
    line_count += 1;
    if (cur->text) {
      if (cur->indent > (ubyte) cur_indent)
        cur->indent -= (ubyte) cur_indent;
      else
        cur->indent = 0;
      cur->convert_tabs (cur_indent, tablen);
      //{{{  check and action foldmarks
      const char* s = cur->text;

      while (*s && (*s <= ' ')) s++;          // skip whitespace

      bool match_start  = false;
      bool match_start2 = false;
      if (openlen + (s - cur->text) <= cur->text_len)
        {
        if (startlen == 0)
          match_start = true;
        else
          match_start = (memcmp (s, c_start, startlen) == 0);
        }
      if (!match_start && (startlen2 > 0) && (openlen2 + (s - cur->text) <= cur->text_len))
        match_start2 = (memcmp (s, c_start2, startlen2) == 0);

      if (match_start || match_start2) { // comment start
        s += match_start ? startlen : startlen2;

        if (memcmp (s, openmark, marklen) == 0)
          //{{{  create a new fold and call foldup (re-entrant)
          {
          s += marklen;
          while (*s == ' ')
            s++;
          const char* s2 = strchr (s, 0);
          while ((s2 > s) && (*(s2-1) == ' '))  // strip trailing spaces
            s2--;
          if (match_start2) {
            if ((endlen2 > 0) && ((s2 - s) >= endlen2) && (memcmp ((s2 - endlen2), c_end2, endlen2) == 0))
              s2 -= endlen2;                   // strip closing comment string
            }
          else {
            if ((endlen > 0) && ((s2 - s) >= endlen) && (memcmp ((s2 - endlen), c_end, endlen) == 0))
              s2 -= endlen;                   // strip closing comment string
            }
          while ((s2 > s) && (*(s2-1) == ' '))  // strip trailing spaces
            s2--;
          cTextFold* newfold = new cTextFold (this, cur);
          if (*s && (s2 > s))
            newfold->set_text (s, (int) (s2 - s));
          newfold->set_indent (cur->indent);
          newfold->SetLinenum (cur->line_num);
          cTextFoldEnd* newfoldend = newfold->Foldup (language, cur_indent + newfold->indent, limit);
          //line_count -= 1;
          //if (newfoldend && newfoldend->ParentFold())
            //line_count += newfoldend->ParentFold()->line_count() - 1;

          if (prevline == cur)
            prevline = newfoldend;
          delete cur;
          cur = newfoldend;
          }
          //}}}
        else if (last && (memcmp (s, closemark, marklen) == 0))
          //{{{  foldend found, finish this fold
          {
          s += marklen;
          while (*s == ' ')
            s++;
          const char* s2 = strchr (s, 0);
          while ((s2 > s) && (*(s2-1) == ' '))  // strip trailing spaces
            s2--;
          if (match_start2) {
            if ((endlen2 > 0) && ((s2 - s) >= endlen2) && (memcmp ((s2 - endlen2), c_end2, endlen2) == 0))
              s2 -= endlen2;                   // strip closing comment string
            }
          else {
            if ((endlen > 0) && ((s2 - s) >= endlen) && (memcmp ((s2 - endlen), c_end, endlen) == 0))
              s2 -= endlen;                   // strip closing comment string
            }
          while ((s2 > s) && (*(s2-1) == ' '))  // strip trailing spaces
            s2--;
          fold_end = new cTextFoldEnd (this, cur, line_count+1);
          if (*s)
            fold_end->set_text (s, (int) (s2 - s));
          fold_end->set_indent (cur->indent);
          fold_end->SetLinenum (cur->line_num);

          delete cur;
          return fold_end;   // force terminate
          }
          //}}}
        }
      //}}}
      }
    if (cur) {
      //if (cur->indent > (ubyte) cur_indent)
      //  cur->indent -= (ubyte) cur_indent;
      //else
      //  cur->indent = 0;
      cur = cur->Next();
      }
    }

  if (prevline && !fold_end)  // mis-matched fold marks - create extra fold-close
    fold_end = new cTextFoldEnd (this, prevline, line_count+1);

  return fold_end;
}
//}}}
//{{{
cTextFoldEnd* cTextFold::Format (const cLanguage* language)
{
  // returns 'foldend'

  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    if (cur->isFold())
      cur->isFold()->SetCommentsOnly (true);
    cur->SetFormat(language);
    cur = cur->Next();
    }

  return fold_end;
}
//}}}

//{{{
cTextLine* cTextFold::Duplicate()
{
  if (!fold_end)
    return cTextLine::Duplicate();

  cTextLine* fold = Copy();
  fold_end->InsertAfter (fold);
  return fold;
}
//}}}
//{{{
cTextLine* cTextFold::Copy()
{
  cTextFold* fold = new cTextFold (0, 0);
  cTextFoldEnd* fend = new cTextFoldEnd (fold, fold);
  if (fend && fold_end) {
    if (fold_end->text)
      fend->set_text (fold_end->text);
    fend->set_indent (fold_end->indent);
    }
  if (fold) {
    if (text)
      fold->set_text (text);
    fold->set_indent (indent);
    cTextLine* src = next;
    while (src && (src != fold_end)) {
      fend->InsertBefore (src->Copy());
      if (src->isFold() && src->isFold()->FoldEnd())
        src = src->isFold()->FoldEnd()->Next();
      else
        src = src->Next();
      }
    }
  return static_cast<cTextLine*>(fold);
}
//}}}
//{{{
cTextLine* cTextFold::Unlink()
{
  if (!fold_end)
    return cTextLine::Unlink();

  if (prev)
    prev->SetDeletedBelow();
  //unlink
  if (prev)
    prev->next = fold_end->next;
  if (fold_end->next)
    fold_end->next->prev = prev;
  if (parent)
    parent->adjustLineCount (-lineCount());

  prev = 0;
  fold_end->next = 0;
  parent = 0;

  m_edited = true;

  return this;
}
//}}}
//{{{
cTextLine* cTextFold::Extract()
{
  opened = 0;
  line_num = 0;
  return Unlink();
}
//}}}
//{{{
cTextLine* cTextFold::InsertAfter (cTextLine* new_line)
{
  if (!new_line)
    return this;

  //if (fold_end && !(opened & view))
    //return fold_end->InsertAfter (new_line);

  return cTextLine::InsertAfter (new_line);
}
//}}}

//{{{
void cTextFold::MakeComment (int view, cLanguage* language)
{
  if (!fold_end || (opened & view))
    cTextLine::MakeComment (view, language);
  else {
    cTextLine* cur = next;
    while (cur && (cur != fold_end)) {
      cur->MakeComment (view, language);
      cur = cur->NextLine(view);
      }
    m_commentsOnly = true;
    }
}
//}}}
//{{{
void cTextFold::RemoveComment (int view, cLanguage* language)
{
  if (!fold_end || (opened & view))
    cTextLine::RemoveComment (view, language);
  else {
    cTextLine* cur = next;
    while (cur && (cur != fold_end)) {
      cur->RemoveComment (view, language);
      cur = cur->NextLine(view);
      }
    }
}
//}}}

//{{{
bool cTextFold::SetFormat (const cLanguage* language)
{
  int old_outfmt = m_outfmt;
  m_outfmt =  prev ? prev->Outfmt() : fmt_OK;
  return (bool) (m_outfmt != old_outfmt);

  //int old_outfmt = fold_end ? fold_end->Outfmt() : fmt_OK;

  //if (language) {
  //  cTextLine* cur = next;
  //  while (cur && (cur != fold_end)) {
  //    cur->SetFormat (language);
  //    cur = cur->Next();
  //    }
  //  }

  //m_outfmt =  fold_end ? fold_end->Outfmt() : fmt_OK;
  //return (bool) (m_outfmt != old_outfmt);
}
//}}}
//{{{
void cTextFold::SetAbove (long x, long y, long scroll, int level)
{
  above_x = x;
  above_y = y;
  above_scroll = scroll;
  above_level = level;
}
//}}}
//{{{
void cTextFold::SetBelow (long x, long y, long scroll)
{
  below_x = x;
  below_y = y;
  below_scroll = scroll;
}
//}}}
//{{{
void cTextFold::GetAbove (long& x, long& y, long& scroll, int& level) const
{
  x = above_x;
  y = above_y;
  scroll = above_scroll;
  level = above_level;
}
//}}}
//{{{
void cTextFold::GetBelow (long& x, long& y, long& scroll) const
{
  x = below_x;
  y = below_y;
  scroll = below_scroll;
}
//}}}

//{{{
int cTextFold::enclosedLines() const
{
  int count = HiddenLines;
  cTextLine* cur = next;

  while (cur && (cur != fold_end)) {
    if (cur->isFold()) {
      count += cur->isFold()->enclosedLines();
      cur = cur->isFold()->fold_end ? cur->isFold()->fold_end->next : 0;
      }
    else
      cur = cur->next;
    }
  return count;
}
//}}}

//{{{
int cTextFold::count_lines() const
{
  int linecount = 2;
  cTextLine* cur = next;
  while (cur && (cur != fold_end))
    {
    linecount++;
    cur = cur->Next();
    }
  return linecount;
}
//}}}
//{{{
int cTextFold::count_lines (int view) const
{
  int linecount = 2;
  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    linecount++;
    cur = cur->NextLine (view);
    }
  return linecount;
}
//}}}
//{{{
int cTextFold::max_width (int view) const
{
  int maxlen = Indent() + TextLen (view);
  int foldindent = 0;
  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    int len = foldindent + cur->Indent() + cur->TextLen (view);
    if (maxlen < len)
      maxlen = len;
    if (cur->isFold() && cur->isFold()->FoldEnd())
      foldindent += cur->Indent();
    else if (cur->isFoldEnd() && cur->ParentFold())
      foldindent -= cur->ParentFold()->Indent();
    cur = cur->NextLine (view);
    }
  return maxlen;
}
//}}}

//{{{
void cTextFold::countLines()
{
  int LineCount = 2;
  cTextLine* cur = next;
  while (cur && (cur != fold_end))
    {
    LineCount += cur->lineCount();
    cur = cur->NextLine (0);
    }
}
//}}}
//{{{
void cTextFold::adjustLineCount (int num)
{
  LineCount += num;
  if (parent)
    parent->adjustLineCount (num);
}
//}}}

//{{{
cTextLine* cTextFold::GetTextLine()
{
  if (fold_end && (!text || !*text)) {
    //{{{  search for and return first non-comment textline
    cTextLine* first_text = 0;
    cTextLine* cur = next;
    while (cur && (cur != fold_end)) {
      if (cur->text) {
        if (!cur->isComment() && !cur->isFoldLine())
          return cur;
        if (!first_text)
          first_text = cur;
        }
      cur = cur->next;
      }
    if (first_text)
      return first_text;
    //}}}
    }

  return static_cast<cTextLine*>(this);
}
//}}}
//{{{
cTextLine* cTextFold::GetTextLine (int view)
{
  if (!(opened & view))
    return GetTextLine();

  return static_cast<cTextLine*>(this);
}
//}}}
//{{{
const char* cTextFold::GetText (int view) const
{
  const cTextLine* textline = const_cast<cTextFold*>(this)->GetTextLine (view);

  return textline->cTextLine::GetText (view);
}
//}}}
//{{{
const char* cTextFold::Text (int view, int& col) const
{
  col = GetColour (view, -1);
  if (!view)
    return cTextLine::GetText(view);
  return GetText (view);
}
//}}}
//{{{
int cTextFold::GetColour (int view, int xpos) const
{
  if (opened & view)
    return c_FoldOpen;

  if (m_commentsOnly)
    return c_CommentText;

  if (text && *text)
    return c_FoldClosed;

  return c_FoldCopied;
}
//}}}

//{{{
cTextLine* cTextFold::GetLine (int ypos)
{
  cTextLine* cur = this;
  while (cur && (cur != fold_end) && (ypos-- > 0)) {
    cur = cur->Next();
    }
  return cur;
}
//}}}
//{{{
cTextLine* cTextFold::GetLine (int view, int ypos)
{
  cTextLine* cur = this;
  while (cur && (cur != fold_end) && (ypos-- > 0)) {
    cur = cur->next_line (view);
    }
  return cur;
}
//}}}
//{{{
int cTextFold::CheckValid (int view, int ypos) const
{
  int y = 0;
  cTextLine* cur = const_cast<cTextFold*>(this);
  while (cur && (cur != fold_end) && (y < ypos)) {
    cur = cur->next_line (view);
    y++;
    }
  return y;
}
//}}}
//{{{
int cTextFold::GetLinePos (cTextLine* line) const
{
  int num = 0;
  while (line && (line != static_cast<const cTextLine*>(this)) && !line->isFile()) {
    num++;
    line = line->Prev();
    }
  return (line == static_cast<const cTextLine*>(this)) ? num : 0;
}
//}}}

//{{{
bool cTextFold::isOpen (int view) const
{
  if (opened & view)
    return true;
  return false;
}
//}}}
//{{{
bool cTextFold::Open (int view)
{
  if (parent && !(opened & view))
    parent->HiddenLines += HiddenLines;
  bool result = (opened & view) == 0;
  opened |= view;
  return result;
}
//}}}
//{{{
bool cTextFold::Close (int view)
{
  if (parent && (opened & view))
    parent->HiddenLines -= HiddenLines;
  bool result = (opened & view) != 0;
  opened &= ~view;
  return result;
}
//}}}
//{{{
bool cTextFold::OpenAll (int view)
{
  bool result = false;

  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    cTextFold* fold = cur->isFold();
    if (fold) {
      result |= fold->Open (view);
      result |= fold->OpenAll (view);
      cur = fold->FoldEnd() ? fold->FoldEnd()->Next() : cur->Next();
      }
    else
      cur = cur->Next();
    }

  return result;
}
//}}}
//{{{
bool cTextFold::CloseAll (int view)
{
  bool result = false;

  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    cTextFold* fold = cur->isFold();
    if (fold) {
      result |= fold->Close (view);
      result |= fold->CloseAll (view);
      cur = fold->FoldEnd() ? fold->FoldEnd()->Next() : cur->Next();
      }
    else
      cur = cur->Next();
    }

  return result;
}
//}}}

//{{{
cTextLine* cTextFold::NextLine (int view)
{
  if (fold_end && !(opened & view))
    return fold_end->next;
  return next;
}
//}}}
//{{{
cTextFold& cTextFold::operator<< (const char* data)
{
if (data && *data)
  {
  int len = (int)strlen(data);
  cTextLine* curLine = fold_end ? fold_end->Prev() : 0;
  const char* eol = strchr (data, '\n');

  while (eol && curLine)
    {
    int insertlen = (int) (eol - data);
    len -= insertlen + 1;

    if ((eol > data) && (*(eol-1) == '\r'))
      insertlen -= 1;

    if (StreamAppend)
      curLine->append_text (data, insertlen);
    else
      {
      curLine = new cTextLine (this, curLine);
      if (curLine) {
        curLine->set_text (data, insertlen);
        curLine->SetLinenum (LineCount);
        }
      }
    if (curLine)
      curLine->convert_tabs(0);
    data = eol + 1;
    StreamAppend = false;
    eol = strchr (data, '\n');
    }

  if ((len > 0) && *data && curLine)
    {
    if (*(data+len-1) == '\r')
      len -= 1;
    if (StreamAppend)
      curLine->append_text (data, len);
    else
      {
      curLine = new cTextLine (this, curLine);
      if (curLine)
        {
        curLine->set_text (data, len);
        curLine->SetLinenum (LineCount++);
        }
      }
    if (curLine)
      curLine->convert_tabs(0);
    StreamAppend = true;
    }
  }
return (*this);
}
//}}}

// cTextFoldEnd
//{{{
cTextFoldEnd::cTextFoldEnd (cTextFold* fold, cTextLine* previous, int lineCount)
  : cTextLine (fold, previous)
{
  if (fold) {
    fold->fold_end = this;
    if (fold->isFile())
      SetEdited (false);
    if (lineCount > 0)
      fold->LineCount = lineCount;
    else
      fold->countLines();
    }
}
//}}}
//{{{
cTextFoldEnd::~cTextFoldEnd()
{
  if (parent)
    parent->fold_end = 0;
}
//}}}
//{{{
char* cTextFoldEnd::show_debug (char* str) const
{
  sprintf (str, "cTextFoldEnd:");
  char* s = strchr (str, 0);
  *s++ = 0;
  *s++ = ' ';
  s = cTextLine::show_debug (s);

  s++;
  sprintf (s, "parent= ");
  s = strchr (s, 0);
  if (parent)
    s = parent->show_debug (s);
  else {
    sprintf (s, "NULL");
    s = strchr (s, 0);
    }

  return s;
}
//}}}

//{{{
void cTextFoldEnd::ReplaceText (int view, int xpos, int ch)
{
  xpos -= 5;         // allow for fold mark
  if (xpos >= indent) {
    if (text && *text) {
      if ((xpos > indent) || (ch != ' '))
        overwrite_text (xpos, ch);
      }
    else if (ch != ' ')
      overwrite_text (indent, ch);
    }

}
//}}}
//{{{
void cTextFoldEnd::InsertText (int view, int xpos, int ch)
{
  xpos -= 5;         // allow for fold mark
  if (xpos >= indent) {
    if (text && *text) {
      if ((xpos > indent) || (ch != ' '))
        insert_text (xpos, ch);
      }
    else if (ch != ' ')
      insert_text (indent, ch);
    }

}
//}}}
//{{{
void cTextFoldEnd::InsertText (int view, int xpos, const char* str)
{
  if (str && *str) {
    const char* s = str;
    while (*s == ' ')       // skip past leading spaces
      s++;

    xpos -= 5;         // allow for fold mark
    if (xpos >= indent) {
      if (text && *text) {
        if (xpos > indent)            // leading spaces only allowed within text
          s = str;
        if (*s)
          insert_text (xpos, s);
        }
      else if (*s)
        insert_text (indent, s);
      }
    }
}
//}}}
//{{{
void cTextFoldEnd::DeleteText (int view, int xpos, int len)
{
  if (xpos < indent + 5) {      // within indent or foldmark
    if (xpos + len <= indent) {
      indent -= len;
      return;
      }
    if (xpos < indent) {
      int sublen = indent - xpos;
      len -= sublen;             // remove rest of indent
      indent = xpos;
      //xpos += sublen;
      }
    //len -= 5;                    // cannot delete any of foldmark
    }

  xpos -= 5;                     // allow for foldmark
  if (xpos < indent) {
    len -= indent - xpos;
    xpos = indent;
    }

  if (len > 0)
    delete_text (xpos, len);
}
//}}}
//{{{
void cTextFoldEnd::ToUpper (int view, int xpos, int len)
{ // assumes a closed fold
  if (xpos <= indent)             // skip foldmark
    len -= 5;
  else if (xpos < indent + 5) {   // start is within foldmark
    len -= indent + 5 - xpos;
    xpos = indent;
    }
  else
    xpos -= 5;

  if (len > 0)
    toupper_text (xpos, len);
}
//}}}
//{{{
void cTextFoldEnd::ToLower (int view, int xpos, int len)
{ // assumes a closed fold
  if (xpos <= indent)             // skip foldmark
    len -= 5;
  else if (xpos < indent + 5) {   // start is within foldmark
    len -= indent + 5 - xpos;
    xpos = indent;
    }
  else
    xpos -= 5;

  if (len > 0)
    tolower_text (xpos, len);
}
//}}}

//{{{
cTextLine* cTextFoldEnd::SplitLine (int view, int xpos) {
  if (xpos < indent)
    return cTextLine::SplitLine (view, xpos);
  if (xpos < indent + 5)
    xpos = indent;
  else
    xpos -= 5;             // correct for foldmark

  if (!parent)
    return cTextLine::SplitLine (view, xpos);

  if (xpos < 0) xpos = 0;
  int pos = xpos - indent;

  cTextLine* newline = new cTextLine (parent->ParentFold(), this);

  if (text && (pos >= 0) && (pos < (int) strlen (text)) ) {
    newline->set_text (text + pos);
    delete_to_eol (xpos);
    }

  if (newline)
    newline->set_indent (parent->indent);

  return newline;
  }
//}}}
//{{{
bool cTextFoldEnd::SetFormat (const cLanguage* language)
{
  int old_outfmt = m_outfmt;
  m_outfmt =  prev ? prev->Outfmt() : fmt_OK;
  return (bool) (m_outfmt != old_outfmt);
}
//}}}
//{{{
const char* cTextFoldEnd::GetText (int view) const
{
  if (parent && parent->isFile())
    return parent->GetText (view);
  return cTextLine::GetText (view);
}
//}}}
//{{{
const char* cTextFoldEnd::Text (int view, int& col) const {

  col = GetColour (view, -1);
  return GetText (view);
  }
//}}}
