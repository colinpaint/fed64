// TextLine.cpp
//{{{  includes
#include "pch.h"
#include "textline.h"

#include "colour.h"
#include "language.h"
#include "textfold.h"
//}}}

const char cTextLine::cNullStr [] = "";

//{{{
cTextLine::cTextLine (cTextFold* fold, cTextLine* previous)
  : cText(), cTextFmt(),
    prev (previous),
    next (NULL),
    parent (fold),
    m_edited (true),  // default, SetLinenum sets it false
    m_deletedBelow (false),
    line_num (0)
{
  if (prev) {
    next = prev->next;
    if (next)
      next->prev = this;
    prev->next = this;
    m_outfmt = prev->Outfmt();   // 'cos this is a blank line (so far)
    }
  if (parent)
    parent->adjustLineCount (lineCount());
}
//}}}
//{{{
cTextLine::~cTextLine()
{
//{{{  description
/*--------------------------------------------------------------------------
  Function:     DESTRUCTOR ~cTextLine()
  Description:  notifies world of its disappearing, and unlinks itself
        from the chain
*/
//}}}

  //unlink from chain
  if (prev)
    prev->next = next ;
  if (next)
    next->prev = prev ;
  if (parent)
    parent->adjustLineCount (-lineCount());
}
//}}}
//{{{
char* cTextLine::show_debug (char* str) const
{
  //cTextLine* prev;
  //cTextLine* next;
  //int line_num;
  //cTextFold* parent;

  sprintf (str, "cTextLine:%c %p line_num:%d outfmt:%d",
    m_edited ? '*' : '-',
    this, line_num, m_outfmt);
  char* s = strchr (str, 0);
  *s++ = 0;
  *s++ = ' ';
  return cText::show_debug (s);
}
//}}}

// status
//{{{
bool cTextLine::isWithin (cTextFold* fold) const
{
  if (this == fold)
    return true;
  if (fold) {
    cTextFold* cur = parent;
    while (cur) {
      if (cur == fold)
        return true;
      cur = cur->ParentFold();
      }
    }
  return false;
}
//}}}

// read attributes
//{{{
char cTextLine::GetChar (int /*view*/, int xpos) const
{
  if (text && *text && (xpos >= 0)) {
    xpos -= indent;
    if (xpos < 0)
      return ' ';
    int len = (int)strlen (text);
    if (xpos < len)
      return text[xpos];
    }
  return 0;
}
//}}}
//{{{
const char* cTextLine::GetTextAt (int /*view*/, int xpos) const
{
  if (text && *text && (xpos >= 0)) {
    xpos -= indent;
    int len = (int)strlen (text);
    if ((xpos >= 0) && (xpos < len))
      return (const char*) (text + xpos);
    }
  return 0;
}
//}}}
//{{{
const char* cTextLine::GetText (int /*view*/) const
{
  if (text) {
    const char* s = text;
    //if (indent > 200) {   // frig for ubyte !!
    //  for (int n = indent; n < 256; n++) {
    //    if (*s == ' ') s++;
    //    else break;
    //    }
    //  }
    return s;
    }
  return cNullStr;
}
//}}}
//{{{
const char* cTextLine::Text (int view, int& col) const
{
  col = c_Text;
  return GetText (view);
}
//}}}
//{{{
int cTextLine::TextLen() const
{
  if (text)
    return (int)strlen (text);
  return 0;
}
//}}}
//{{{
int cTextLine::TextLen (int view) const
{
  const char* s = GetText(view);
  if (s)
    return (int)strlen (s);
  return 0;
}
//}}}
//{{{
int cTextLine::GetIndent (cTextFold* top) const
{
  int full_indent = 0;
  if (this != top) {
    full_indent += indent;
    cTextFold* cur = parent;
    while (cur && (cur != top)) {
      full_indent += cur->indent;
      cur = cur->parent;
      }
    }
  return full_indent;
}
//}}}
//{{{
int cTextLine::GetFoldIndent (cTextFold* top) const
{
  int fold_indent = 0;
  if (this != top) {
    cTextFold* cur = parent;
    while (cur && (cur != top)) {
      fold_indent += cur->indent;
      cur = cur->parent;
      }
    }
  return fold_indent;
}
//}}}
//{{{
int cTextLine::GetPosInFold (cTextFold* fold) const
{
  int num = 0;
  cTextLine* cur = const_cast<cTextLine*>(this);
  while (cur && (cur != fold) && !cur->isFile()) {
    num++;
    cur = cur->Prev();
    }
  return (cur == fold) ? num : 0;
}
//}}}
//{{{
int cTextLine::GetColour (int view, int xpos) const
{
  return get_format_colour (xpos - indent);
}
//}}}

// change attributes
//{{{
void cTextLine::SetEdited (bool newval)
{
  if (newval && !m_edited && parent)
    parent->SetContainsEdits (true);
  m_edited = newval;
}
//}}}
//{{{
void cTextLine::SetDeletedBelow (bool newval)
{
  if (newval && !m_deletedBelow && parent)
    parent->SetContainsEdits (true);
  m_deletedBelow = newval;
}
//}}}
//{{{
bool cTextLine::SetFormat (const cLanguage* language)
{
  // returns true if 'outfmt' changed
  //if (text)
  //  cTextFmt::SetFormat (text, language, prev ? prev->Outfmt() : fmt_OK);
  //else
  //  cTextFmt::ClearFormat();

  int old_outfmt = m_outfmt;
  cTextFmt::SetFormat (text, language, prev ? prev->Outfmt() : fmt_OK);

  if (parent && text && *text && !isComment())
    parent->SetCommentsOnly (false);

  return (bool) (m_outfmt != old_outfmt);
}
//}}}

// edits
//{{{
void cTextLine::ReplaceText (int view, int xpos, int ch)
{
  overwrite_text (xpos, ch);
  SetEdited();
}
//}}}
//{{{
void cTextLine::InsertText (int view, int xpos, int ch, int len)
{
  insert_text (xpos, ch, len);
  SetEdited();
}
//}}}
//{{{
void cTextLine::InsertText (int view, int xpos, const char* str, int len)
{
  insert_text (xpos, str, len);
  SetEdited();
}
//}}}
//{{{
void cTextLine::DeleteText (int view, int xpos, int len)
{
  delete_text (xpos, len);
  absorb_leading();
  trim_trailing();
  SetEdited();
}
//}}}
//{{{
void cTextLine::ToUpper (int view, int xpos, int len)
{
  toupper_text (xpos, len);
  SetEdited();
}
//}}}
//{{{
void cTextLine::ToLower (int view, int xpos, int len)
{
  tolower_text (xpos, len);
  SetEdited();
}
//}}}
//{{{
char* cTextLine::ExtractText (int view, int xpos, int len)
{
  char* result = extract_text (xpos, len);
  absorb_leading();
  trim_trailing();
  SetEdited();
  return result;
}
//}}}

//{{{
cTextLine* cTextLine::SplitLine (int view, int xpos)
{
  if (xpos < 0) xpos = 0;
  int pos = xpos - indent;

  cTextLine* newline = new cTextLine (parent, this);

  if (newline) {
    if (text && (pos >= 0) && (pos < (int) strlen (text)) ) {
      newline->set_text (text + pos);
      delete_to_eol (xpos);
      SetEdited();
      }
    else if (parent)
      parent->SetContainsEdits (true);

    newline->set_indent (indent);
    }

  return newline;
}
//}}}
//{{{
bool cTextLine::JoinLine (int view, int append_spaces)
{

  cTextLine* line = (view != 0) ? NextLine(view) : Next();
  if (line && !(line->isFoldLine())) {
    if (line->GetText(view))
      append_text (line->GetText(view), append_spaces, true);
    delete line;
    SetEdited();
    return true;
    }
  return false;
}
//}}}

//{{{
cTextLine* cTextLine::Duplicate()
{
  cTextLine* line = new cTextLine (parent, this);
  if (line) {
    if (text)
      line->set_text (text);
    line->set_indent (indent);
    }
  return line;
}
//}}}
//{{{
cTextLine* cTextLine::Copy()
{
  cTextLine* result = new cTextLine (0, 0);
  if (result) {
    if (text)
      result->set_text (text);
    result->set_indent (indent);
    }
  return result;
}
//}}}
//{{{
cTextLine* cTextLine::Copy(int xpos, int len)
{
  cTextLine* result = new cTextLine (0, 0);
  if (result) {
    int new_indent = xpos < indent ? indent - xpos : 0;
    if (text && (xpos + len > indent)) {
      int slen = (int)strlen(text);
      int offset = xpos - new_indent;
      if (offset < slen)
        result->set_text (text + offset, len - new_indent);
      }
    if (xpos < indent)
      result->set_indent (indent - xpos);
    }
  return result;
}
//}}}
//{{{
cTextLine* cTextLine::Unlink()
{
  if (prev)
    prev->SetDeletedBelow();
  //unlink
  if (prev)
    prev->next = next;
  if (next)
    next->prev = prev;
  if (parent)
    parent->adjustLineCount (-lineCount());

  prev = 0;
  next = 0;
  parent = 0;

  m_edited = true;

  return this;
}
//}}}
//{{{
cTextLine* cTextLine::Extract()
{
  line_num = 0;
  return Unlink();
}
//}}}
//{{{
cTextLine* cTextLine::Extract(int xpos, int len)
{
  cTextLine* result = Copy (xpos, len);
  DeleteText (0, xpos, len);
  return result;
}
//}}}
//{{{
cTextLine* cTextLine::InsertBefore (cTextLine* new_line)
{
  if (!new_line)
    return this;
  cTextLine* last_line = new_line->isFold() ? new_line->isFold()->FoldEnd() : new_line;

  //copy outfmt across inserted lines so that Reformat detects changes
  last_line->m_outfmt = prev ? prev->m_outfmt : fmt_OK;

  new_line->prev = prev;
  last_line->next = this;
  new_line->parent = parent;
  if (prev)
    prev->next = new_line;
  prev = last_line;

  if (new_line->parent)
    new_line->parent->adjustLineCount (new_line->lineCount());

  //if (next->parent && next->parent->isCommentFold())
  //  next->SetComment (true);
  return new_line;
}
//}}}
//{{{
cTextLine* cTextLine::InsertAfter (cTextLine* new_line)
{
  if (!new_line)
    return this;
  cTextLine* last_line = new_line->isFold() ? new_line->isFold()->FoldEnd() : new_line;

  //copy outfmt across inserted lines so that Reformat detects changes
  last_line->m_outfmt = m_outfmt;

  new_line->prev = this;
  last_line->next = next;
  if (next) {
    next->prev = last_line;
    new_line->parent = next->parent;
    }
  next = new_line;

  if (new_line->parent)
    new_line->parent->adjustLineCount (new_line->lineCount());

  //if (next->parent && next->parent->isCommentFold())
  //  next->SetComment (true);
  return last_line;
}
//}}}

//{{{
void cTextLine::MakeComment (int view, cLanguage* language)
{
  if (language && text && *text) {
    const char* start = language->CommentStart();
    const char* end   = language->CommentEnd();
    if (start && *start)  {
      insert_text (indent, start);
      if (end && *end)
        append_text (end);
      SetEdited();
      }
    }
}
//}}}
//{{{
void cTextLine::RemoveComment (int view, cLanguage* language)
{
  if (language && text && *text) {
    const char* start = language->CommentStart();
    const char* end   = language->CommentEnd();
    int start_len = start ? (int)strlen (start) : 0;
    int end_len = end ? (int)strlen (end) : 0;
    int text_len = text ? (int)strlen(text) : 0;
    if ((start_len > 0) && (text_len >= start_len + end_len)) {
      if (memcmp(text, start, start_len) == 0)  {
        if (end_len > 0) {
          const char* last = strchr (text, 0);
          while ((last > text) && (*(last-1) == ' '))
            last--;
          last -= end_len;
          if ((last < text + start_len) || (memcmp (last, end, end_len) != 0))
            return;
          delete_text (indent + (int) (last - text), end_len);
          }
        delete_text (indent, start_len);
        SetEdited();
        }
      }
    }
}
//}}}

//{{{
bool cTextLine::RevertToOriginal (int view)
{
  if (revert_to_original()) {
    SetEdited (false);
    return true;
    }
  return false;
}
//}}}

//{{{
bool cTextLine::OpenToTop (int view, cTextFold* topfold)
{
  bool did_open = false;
  if (topfold && (this != topfold)) {
    cTextFold* cur = parent;
    while (cur && (cur != topfold)) {
      if (cur->Open (view))
        did_open = true;
      cur = cur->ParentFold();
      }
    }
  return did_open;
}
//}}}
//{{{
void cTextLine::GrabContents (cTextLine* old)
{
  grabtext (old);
  line_num = old->line_num;
  SetEdited();
}
//}}}

// static routines
//{{{
cTextLine* cTextLine::readfile (const char* path)  // static
{
  FILE* fp = fopen (path, "r");
  //{{{  check ok
  if (!fp) {
    cTextLine* msg = new cTextLine (0);
    msg->set_text ("File not found");
    return msg;
    }
  //}}}

  cTextLine* top = 0;
  char line [cMaxLineLen];

  bool longline;
  if (fgets (line, cMaxLineLen, fp) ) {
    char* s = strchr (line, '\n');
    if (s) *s = 0;
    longline = (bool) (!s);
    top = new cTextLine (0, 0);
    top->set_text (line, longline);
    }

  cTextLine* cur = top;
  while (fgets (line, cMaxLineLen, fp) ) {
    char* s = strchr (line, '\n');
    if (s) *s = 0;
    if (longline) {
      longline = (bool) (!s);
      top->append_text (line, 0, longline);
      }
    else {
      longline = (bool) (!s);
      cur = new cTextLine (0, cur);
      cur->set_text (line, longline);
      }
    }

  fclose (fp);

  return top;
}
//}}}
