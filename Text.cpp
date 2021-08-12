// Text.cpp
//{{{  includes
#include "pch.h"
#include "Text.h"
#include <string>
//}}}

//{{{
cText::cText (int length)
   : text (NULL), text_len (0), indent (0), original_text (NULL), original_indent (0) {

  if (length > 0)
    text = new char [length + 1];

  if (text) {
    text_len = length;
    memset (text, 0, text_len + 1);
    }
  }
//}}}
//{{{
cText::~cText() {

  if (text)
    delete[] text;
  if (original_text)
    delete[] original_text;
  }
//}}}

//{{{
char* cText::show_text (char* str) const {

  if (text)
    sprintf (str, "\"%s\"", text);
  else
    sprintf (str, "NULL");
  char* s = strchr (str, 0);

  if (original_text) {
    *s++ = 0;
    *s++ = ' ';
    sprintf (s, "orig=\"%s\"", original_text);
    s = strchr (s, 0);
    }

  return s;
  }
//}}}
//{{{
char* cText::show_debug (char* str) const {

  sprintf (str, "cText: len:%d indent:%d ", text_len, indent);
  char* s = strchr (str, 0);

  return show_text (s);
  }
//}}}

//{{{
void cText::grabtext (cText* old) {

  text = old->text;
  text_len = old->text_len;
  indent = old->indent;

  old->text = 0;
  old->text_len = 0;
  }
//}}}
//{{{
void cText::remove_text() {

  if (text)
    delete[] text;

  text = 0;
  text_len = 0;
  }
//}}}
//{{{
void cText::set_original() {

  if (!original_text) {
    int len = text ? (int)strlen(text) : 0;
    original_text = new char [len + 1];
    if (original_text) {
      if (text)
        strcpy (original_text, text);
      else
        *original_text = 0;
      original_indent = indent;
      }
    }
  }
//}}}

//{{{
bool cText::resize (int length) {

  if (length <= 0) {
    if (text)
      delete[] text;
    text = 0;
    text_len = 0;
    }
  else if (length > text_len)
    // extend text buffer on heap
    {
    char* newtext = new char [length + 1];     // 1 more for terminating '\0'
    //{{{  check ok
    if (!newtext) {
      //tError::heap_full();
      return false;
      }
    //}}}
    if (text) {
      memcpy (newtext, text, text_len);
      memset (newtext + text_len, 0, length - text_len + 1);
      delete[] text;
      }
    else
      memset (newtext, 0, length + 1);
    text = newtext;
    text_len = length;
    }
  else if (text)
    memset (text + length, 0, text_len - length + 1);

  return true;
  }
//}}}

//{{{
void cText::convert_tabs (int fold_indent, int tablen) {

  if (text) {
    char* tab = strchr (text, '\t');
    if (tab) {
      // calculate the new text len with tabs expanded
      char* pos = text;
      int len = fold_indent + indent;
      while (tab) {
        len += (int) (tab - pos);
        len += tablen - (len % tablen);
        pos = tab + 1;
        while (*pos == '\t') {
          len += tablen;
          pos++;
          }
        tab = strchr (pos, '\t');
        }
      len += (int)strlen(pos);
      len -= fold_indent + indent;

      if (resize (len)) {
        //  now expand the tabs
        int endlen = (int)strlen(text);
        tab = strchr (text, '\t');
        pos = text;
        len = fold_indent + indent;
        while (tab) {
          len += (int) (tab - pos);
          endlen -= (int) (tab - pos) + 1;
          int spaces = tablen - (len % tablen);
          pos = tab + 1;
          while (*pos == '\t') {
            spaces += tablen;
            pos++;
            endlen--;
            }
          if (endlen > 0)
            memmove (tab + spaces, pos, endlen);
          memset (tab, ' ', spaces);
          pos = tab + spaces;
          len = 0;   //at a tab
          tab = strchr (pos, '\t');
          }
        }
      }
    }
  }
//}}}
//{{{
void cText::set_text (const char* str, int len, int tablen) {

  if (str && *str && (len > 0)) {
    const char* first = str;
    const char* last  = str + len;

    indent = 0;
    // convert leading spaces & tabs to 'indent'
    while ((first < last) && ((*first == ' ') || (*first == '\t'))) {
      if (*first++ == '\t')
        indent += tablen - (indent % tablen);
      else
        indent++;
      }

    int len = (int) (last - first);
    if (resize (len))
      memcpy (text, first, len);   //note: resize fills with (terminating) zero
    }

  else {
    indent = 0;
    resize (0);
    }
  }
//}}}
//{{{
void cText::set_text (const char* str, bool strip_trailing, int tablen) {

  if (str && *str) {
    const char* last  = strchr (str, 0);

    // strip trailing spaces and tabs etc
    if (strip_trailing) {
      while ((last > str) && (*(last - 1) <= ' '))
        last--;
      }

    set_text (str, (int) (last - str), tablen);
    }

  else {
    indent = 0;
    resize (0);
    }
  }
//}}}
//{{{
void cText::append_text (const char* str, int len) {

  const int tablen = 8;
  if (str && *str && (len > 0)) {
    int oldlen = text ? (int)strlen(text) : 0;

    if (oldlen == 0)
      {
      while ((len > 0) && ((*str == ' ') || (*str == '\t')))
        {
        if (*str++ == '\t')
          indent += tablen - (indent % tablen);
        else
          indent++;
        len--;
        }
      }

    if (len > 0)
      if (resize(oldlen + len))
        memcpy (text + oldlen, str, len);
    }
  }
//}}}
//{{{
void cText::append_text (const char* str, int append_spaces, bool strip_trailing) {

  if (append_spaces < 0)
    append_spaces = 0;

  if (!text || !*text) {
    int old_indent = indent;
    set_text (str, strip_trailing);
    indent += old_indent;
    }

  else if (str && *str) {
    const char* first = str;
    const char* last  = strchr (str, 0);

    // strip trailing spaces and tabs etc
    if (strip_trailing)
      while ((last > first) && (*(last - 1) <= ' ')) last--;

    int oldlen = text ? (int)strlen(text) : 0;
    int len = (int) (last - first);
    if (resize (oldlen + append_spaces + len)) {
      if (append_spaces > 0)
        memset (text + oldlen, ' ', append_spaces);
      memcpy (text + oldlen + append_spaces, first, len);
      }
    }
  }
//}}}

//{{{
char cText::get_text_char (int xpos) const {

  xpos -= indent;
  if (text && (xpos >= 0) && (xpos < (int) strlen (text)) )
    return *(text + xpos);
  return ' ';
  }
//}}}
//{{{
void cText::copy_text (char* str, int xpos, int len) const {
// assumes that str is at least xlen+1 long

  xpos -= indent;
  if (xpos < 0) {
    //{{{  insert spaces from indent
    int spaces = -xpos;
    if (spaces > len)
      spaces = len;
    memset (str, ' ', spaces);
    xpos = 0;
    len -= spaces;
    str += spaces;
    //}}}
    }
  if (text && (len > 0)) {
    int slen = (int)strlen(text);
    if (xpos < slen) {
      //{{{  copy from text
      slen -= xpos;
      if (slen > len)
        slen = len;
      memcpy (str, text + xpos, slen);
      len -= slen;
      str += slen;
      //}}}
      }
    }
  if (len > 0) {
    //{{{  insert trailing spaces
    memset (str, ' ', len);
    str += len;
    //}}}
    }
  *str = 0;
  }
//}}}

//{{{
void cText::set_indent (int spaces) {

  indent = (ubyte) spaces;
  }
//}}}
//{{{
void cText::change_indent (int by) {

  set_original();
  int newindent = by + (int) indent;
  if (newindent < 0)
    indent = 0;
  else if (newindent > 255)
    indent = 255;
  else
    indent = (ubyte) newindent;
  }
//}}}
//{{{
void cText::absorb_leading() {
// absorb any leading spaces into indent

  if (text) {
    char* src = text;
    while (*src == ' ') {
      indent++;
      src++;
      }

    if (src > text) {
      int len = (int)strlen(src);
      if (len > 0)
        memmove (text, src, len);
      resize (len);
      }
    }
  }
//}}}
//{{{
void cText::trim_trailing() {
// remove any trailing spaces

  if (text && *text) {
    char* s = strchr (text, 0);
    while ((s > text) && (*(s-1) == ' '))
      s--;
    int len = int(s - text);
    resize (len);
    }
  }
//}}}

//{{{
bool cText::revert_to_original() {

  if (original_text) {
    if (text)
      delete[] text;
    text = 0;
    text_len = 0;

    if (*original_text) {
      text = original_text;
      text_len = (int)strlen(text);
      }
    else {
      delete[] original_text;
      indent = 0;
      }
    indent = original_indent;
    original_text = 0;
    return true;
    }

  return false;
  }
//}}}
//{{{
void cText::delete_original() {

  if (original_text)
    delete[] original_text;
  original_text = 0;
  }
//}}}

// routines return the new xpos
//{{{
int cText::overwrite_text (int xpos, int ch, int length) {

  if (xpos < 0) xpos = 0;
  int result = xpos;
  if (length > 0) {
    set_original();
    if (text) {
      int len = (int)strlen(text);
      result += length;
      if (xpos <= indent) {
        if (ch == ' ') {
          //{{{  absorb into indent, then extend indent
          length -= (indent - xpos);
          if (length >= len) {
            if (text)
              *text = 0;
            // indent?
            }
          else if (length > 0) {
            indent += (ubyte) length;
            delete_text (indent, length);
            }
          //}}}
          }
        else {
          //{{{  reduce indent, and insert before, and overwrite front of text
          int newchars = indent - xpos;
          indent = (ubyte) xpos;
          if (length < newchars + len) {
            //{{{  insert before and overwrite front of text
            if (resize (newchars + len)) {
              memmove (text + newchars, text, len);
              memset (text, ch, length);
              if (length < newchars)
                memset (text + length, ' ', newchars - length);
              }
            //}}}
            }
          else {
            //{{{  completely overwrite text
            if (resize (length))
              memset (text, ch, length);
            //}}}
            }
          //}}}
          }
        }
      else {
        xpos -= indent;
        if (xpos < len) {
          //{{{  overwrite all or part of text
          if (xpos + length <= len)
            memset (text + xpos, ch, length);
          else if (ch == ' ')
            resize (xpos);
          else if (resize (xpos + length))
            memset (text + xpos, ch, length);
          //}}}
          }
        else if (ch != ' ') {
          //{{{  append to text, infilling with spaces as required
          if (resize (xpos + length)) {
            memset (text + len, ' ', xpos - len);
            memset (text + xpos, ch, length);
            }
          //}}}
          }
        }
      }
    else if (ch != ' ') {
      indent = (ubyte) xpos;
      if (resize (length))
        memset (text, ch, length);
      }
    }

  return result;
  }
//}}}
//{{{
int cText::overwrite_text (int xpos, const char* str, int length) {

  if (xpos < 0)
    xpos = 0;
  int result = xpos;

  if (str && *str) {
    set_original();
    if (text) {
      if (length < 0)
        length = (int)strlen(text);
      if (length > 0) {
        //{{{  perform overwrite
        int str_len = (int)strlen(str);
        result += str_len;
        if (xpos <= indent) {
          //{{{  overwrite all or part of the indent
          if (xpos + str_len >= indent + length) {
            indent = (ubyte) xpos;
            set_text (str);
            }
          else {
            //{{{  absorb leading spaces into indent
            while (*str == ' ') {
              str++;
              xpos++;
              str_len--;
              }
            //}}}
            if (xpos < indent) {
              int newchars = indent - xpos;
              indent = (ubyte) xpos;
              //{{{  insert before and overwrite front of text
              if (resize (newchars + length)) {
                memmove (text + str_len + newchars, text + str_len, newchars);
                memcpy (text, str, str_len);
                }
              //}}}
              }
            else {
              //{{{  overwrite front of text
              memcpy (text, str, str_len);
              //}}}
              }
            }
          //}}}
          }
        else {
          xpos -= indent;
          if (xpos < length) {
            //{{{  overwrite all or part of text
            if (xpos + str_len >= length) {
              //{{{  strip trailing spaces
              const char* last = strchr (str, 0);
              while ((last > str) && (*(last - 1) == ' '))
                last--;
              str_len = (int) (last - str);
              //}}}
              //{{{  overwrite end of text
              if (resize (xpos + str_len))
                memcpy (text + xpos, str, str_len);
              //}}}
              }
            else {
              //{{{  insert into body of text
              memcpy (text + xpos, str, str_len);
              //}}}
              }
            //}}}
            }
          else {
            //{{{  append to text, infilling with spaces as required
            //{{{  strip trailing spaces
            const char* last = strchr (str, 0);
            while ((last > str) && (*(last - 1) == ' '))
              last--;
            int str_len = (int) (last - str);
            //}}}
            if (resize (xpos + str_len)) {
              if (str_len > 0) {
                memset (text + length, ' ', xpos - length);
                memcpy (text + xpos, str, str_len);
                }
              }
            //}}}
            }
          }
        //}}}
        }
      }
    else {
      indent = (ubyte) xpos;
      set_text (str, length);
      }
    }

  return result;
  }
//}}}

//{{{
int cText::insert_text (int xpos, int ch, int length) {

  if (xpos < 0) xpos = 0;
  int result = xpos + length;
  if (length > 0) {
    set_original();
    if (text) {
      //{{{  modify text
      int len = (int)strlen(text);
      result += length;
      if (xpos <= indent) {
        if (ch == ' ')
          change_indent (length);
        else {
          //{{{  reduce indent, and insert in front of text
          int spaces = indent - xpos;
          indent = (ubyte) xpos;
          if (resize (length + spaces + len)) {
            memmove (text + length + spaces, text, len);
            memset (text, ch, length);
            memset (text + length, ' ', spaces);
            }
          //}}}
          }
        }
      else {
        xpos -= indent;
        if (xpos < len) {
          //{{{  insert within text
          if (resize (len + length)) {
            memmove (text + xpos + length, text + xpos, len - xpos);
            memset (text + xpos, ch, length);
            }
          //}}}
          }
        else if (ch != ' ') {
          //{{{  append to text, infilling with spaces as required
          if (resize (xpos + length)) {
            memset (text + len, ' ', xpos - len);
            memset (text + xpos, ch, length);
            }
          //}}}
          }
        }
      //}}}
      }
    else if (ch == ' ') {
      //{{{  extend the indent
      if (xpos < indent)
        change_indent (length);
      else
        indent = (ubyte) (xpos + length);
      //}}}
      }
    else {
      //{{{  set new indent and set text
      indent = (ubyte) xpos;
      if (resize (length))
        memset (text, ch, length);
      //}}}
      }
    }

  return result;
  }
//}}}
//{{{
int cText::insert_text (int xpos, const char* str, int length) {

  if (xpos < 0) xpos = 0;
  int result = xpos;

  if (str && *str) {
    set_original();
    if (length < 0)
      length = (int)strlen(str);
    if (length > 0) {
      result += length;
      if (text) {
        //{{{  modify text
        int slen = (int)strlen(text);
        if (xpos <= indent) {
          int spaces = indent - xpos;
          //{{{  strip leading spaces into indent
          while (*str == ' ') {
            str++;
            indent++;
            length--;
            }
          //}}}
          if (length > 0) {
            indent -= spaces;
            //{{{  insert str+spaces infront of text
            if (resize (length + spaces + slen)) {
              memmove (text + length + spaces, text, slen);
              memset (text + length, ' ', spaces);
              memcpy (text, str, length);
              }
            //}}}
            }
          }
        else {
          xpos -= indent;
          if (xpos < slen) {
            //{{{  insert into body of text
            if (resize (slen + length)) {
              memmove (text + xpos + length, text + xpos, slen - xpos);
              memcpy (text + xpos, str, length);
              }
            //}}}
            }
          else {
            //{{{  append to text, infilling with spaces as required
            const char* last = str + length;
            while ((last > str) && (*(last - 1) == ' '))
              last--;
            int length = (int) (last - str);
            if ((length > 0) && resize (xpos + length)) {
              memset (text + slen, ' ', xpos - slen);
              memcpy (text + xpos, str, length);
              }
            //}}}
            }
          }
        //}}}
        }
      else {
        set_text (str, length);
        indent += (ubyte) xpos;
        }
      }
    }

  return result;
  }
//}}}

//{{{
int cText::replace_text (int xpos, int xlen, const char* str, int length) {

  set_original();
  delete_text (xpos, xlen);
  return insert_text (xpos, str, length);
  }
//}}}

//{{{
char* cText::extract_text (int xpos, int length) {

  char* result = new char [length + 1];
  char* p = result;

  if (xpos < 0) {
    length -= -xpos;
    xpos = 0;
    }

  if (length > 0) {
    set_original();
    if (xpos < indent) {
      //{{{  delete all or part of the indent
      int len = indent - xpos;
      if (length <= len) {
        indent -= (ubyte) length;
        memset (result, ' ', length);
        result[length] = 0;
        return result;
        }
      memset (result, ' ', len);
      p += len;
      indent -= (ubyte) len;
      length -= len;
      xpos = 0;
      //}}}
      }
    else
      xpos -= indent;

    if (text && (length > 0)) {
      //{{{  delete from text
      int len = (int)strlen(text) - xpos;
      if (len > 0) {
        if (len > length) {
          memcpy (p, text + xpos, length);
          p[length] = 0;
          len -= length;     // remove delete-length from remaining-text-len
          char* dst = text + xpos;
          char* src = text + xpos + length;
          //if (xpos == 0) {
            //{{{  absorb any now leading spaces into indent
            //while (*src == ' ') {
              //indent++;
              //src++;
              //len--;
              //}
            //}}}
            //}
          memmove (dst, src, len);
          resize (xpos + len);
          }
        else {
          memcpy (p, text + xpos, len);
          p[len] = 0;
          resize (xpos);   // perform delete to eol
          }
        }
      //}}}
      }
    }

  return result;
  }
//}}}

//{{{
int cText::delete_text (int xpos, int length) {

  if (xpos < 0) {
    length -= -xpos;
    xpos = 0;
    }
  int result = xpos;

  if (length > 0) {
    set_original();
    if (xpos < indent) {
      //{{{  delete all or part of the indent
      int len = indent - xpos;
      if (length <= len) {
        indent -= (ubyte) length;
        return result;
        }
      indent -= (ubyte) len;
      length -= len;
      xpos = 0;
      //}}}
      }
    else
      xpos -= indent;

    if (text && (length > 0)) {
      //{{{  delete from text
      int len = (int)strlen(text) - xpos;
      if (len > 0) {
        if (len > length) {
          len -= length;     // remove delete-length from remaining-text-len
          char* dst = text + xpos;
          char* src = text + xpos + length;
          //if (xpos == 0) {
            //{{{  absorb any now leading spaces into indent
            //while (*src == ' ') {
              //indent++;
              //src++;
              //len--;
              //}
            //}}}
            //}
          memmove (dst, src, len);
          resize (xpos + len);
          }
        else
          resize (xpos);   // perform delete to eol
        }
      //}}}
      }
    }

  return result;
  }
//}}}
//{{{
int cText::delete_to_sol (int xpos) {

  if (xpos < 0) xpos = 0;

  if (xpos <= indent)
    indent -= (ubyte) xpos;
  else if (text) {
    set_original();
    int len = (int)strlen(text);
    xpos -= indent;
    memmove (text, text + xpos, len - xpos);
    resize (len - xpos);
    indent = 0;
    }
  else
    indent = 0;

  return 0;
  }
//}}}
//{{{
int cText::delete_to_eol (int xpos) {

  if (xpos < 0) xpos = 0;

  if (xpos <= indent) {
    indent = (ubyte) xpos;
    resize (0);
    }
  else if (text) {
    set_original();
    int len = (int)strlen(text);
    xpos -= indent;
    if (xpos < len)
      resize (xpos);
    }

  return 0;
  }
//}}}

//{{{
int cText::tolower_text (int xpos, int length) {

  if (length <= 0)
    length = (int)strlen(text) - xpos;
  int result = xpos + length;
  if (text && (result > indent)) {
    xpos -= indent;
    if (xpos < 0) {
      length -= -xpos;
      xpos = 0;
      }
    int len = (int)strlen(text);
    if (xpos < len) {
      set_original();
      if (xpos + length > len)
        length = len - xpos;
      char* s = text + xpos;
      while (length--) {
        *s = (char) tolower (*s);
        s++;
        }
      }
    }

  return result;
  }
//}}}
//{{{
int cText::toupper_text (int xpos, int length) {

  if (length <= 0)
    length = (int)strlen(text) - xpos;
  int result = xpos + length;
  if (text && (result > indent)) {
    xpos -= indent;
    if (xpos < 0) {
      length -= -xpos;
      xpos = 0;
      }
    int len = (int)strlen(text);
    if (xpos < len) {
      set_original();
      if (xpos + length > len)
        length = len - xpos;
      char* s = text + xpos;
      while (length--) {
        *s = (char) toupper (*s);
        s++;
        }
      }
    }

  return result;
  }
//}}}

//{{{
int cText::word_right (int xpos) const {
// moves right a word from position 'xpos'
// returns the new xpos

  if (xpos < indent)
    return indent;

  if (text) {
    int len = (int)strlen(text);
    if (xpos - indent < len) {
      const char* s = text + xpos - indent;
      //{{{  skip alphanum
      while (isalnum (*s) || (*s == '_')) s++;
      //}}}
      //{{{  skip non-alphanum to whitespace
      while ((*s > ' ') && !(isalnum (*s) || (*s == '_'))) s++;
      //}}}
      //{{{  skip whitespace
      while (*s == ' ') s++;
      //}}}
      xpos = indent + (int) (s - text);
      }
    }

  return xpos;
  }
//}}}
//{{{
int cText::word_left (int xpos) const {
// moves left a word from position 'xpos'
// returns the new xpos

  if ((xpos <= indent) || !text)
    return 0;

  if (text) {
    int len = (int)strlen(text);
    if (xpos - indent <= len) {
      const char* s = text + (xpos - indent) - 1;
      //{{{  skip whitespace
      while ((s >= text) && (*s == ' ')) s--;
      //}}}
      //{{{  skip non-alphanum to whitespace
      while ((s >= text) && (*s > ' ') && !(isalnum (*s) || (*s == '_'))) s--;
      //}}}
      //{{{  skip alphanum
      while ((s >= text) && (isalnum (*s) || (*s == '_'))) s--;
      //}}}
      xpos = indent + 1 + (int) (s - text);
      }
    }

  return xpos;
  }
//}}}

//{{{
int cText::get_sot() const {

  const char* s = text;
  while (*s && (*s <= ' ')) s++;

  return indent + (int) (s - text);
  }
//}}}
//{{{
int cText::get_eol() const {

  if (text)
    return indent + (int)strlen(text);

  return indent;
  }
//}}}
