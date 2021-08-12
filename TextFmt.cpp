// TextFmt.cpp
//{{{  includes
#include "pch.h"
#include "TextFmt.h"

#include "colour.h"
#include "language.h"
//}}}

//{{{
cTextFmt::cTextFmt()
  : m_format (NULL),
    m_outfmt (fmt_OK)
{
}
//}}}
//{{{
cTextFmt::~cTextFmt()
{
  if (m_format)
    delete[] m_format;
}
//}}}

//{{{
char* cTextFmt::show_format (char* str) const
{
  sprintf (str, "cTextFmt: OutFmt:%d fmt:", m_outfmt);
  char* s = strchr (str, 0);
  if (m_format) {
    unsigned char* fmt = m_format;
    while (fmt) {
      sprintf (s, " %02x,%02x", (int) (*fmt), (int) (*(fmt+1)));
      s = strchr (s, 0);
      if (!*(fmt+1))
        break;
      fmt += 2;
      }
    }
  *s++ = 0;
  return s;
}
//}}}
//{{{
bool cTextFmt::resize (int len)
{
  if (m_format) {
    delete[] m_format;
    m_format = 0;
    }
  if (len > 0) {
    m_format = new unsigned char[len];
    // check ok
    if (!m_format)
      return false;
    }

  return true;
}
//}}}

//{{{
bool cTextFmt::isComment() const
{
  if (m_format && (*m_format == (unsigned char) c_CommentText)) {
    if ((*(m_format+1) == 0) || (*(m_format+2) == 0))
      return true;
    }
  return false;
}
//}}}
//{{{
int cTextFmt::get_format_colour (int xpos) const
{
  if (m_format) {
    const unsigned char* fmt = m_format;
    int col = *fmt++;
    int col_len = *fmt++;
    int cur_len = 0;
    //{{{  find colour region
    while (col_len && (xpos >= cur_len + col_len)) {
      cur_len += col_len;
      col = *fmt++;
      col_len = *fmt++;
      }
    //}}}
    return col;
    }
  return c_Text;
}
//}}}

//{{{
eTextFmt cTextFmt::SetFormat (const char* str, const cLanguage* language, eTextFmt infmt)
{
  m_outfmt = infmt;

  unsigned char new_format [256];
  unsigned char* fmt = new_format;
  int CommentText2 = language && language->m_useTaggedColour ? c_TaggedText : c_CommentText;

  if (str && language) {
    //{{{  scan format
    const char* start1 = language->m_commentStart1;
    const char* end1   = language->m_commentEnd1;
    const char* start2 = language->m_commentStart2;
    const char* end2   = language->m_commentEnd2;
    char string1_char = language->m_stringChar;
    char string2_char = language->m_characterChar;
    char string_escape_char = language->m_escapeChar;

    switch (infmt) {
      //{{{
      case fmt_Comment1:
        *fmt++ = (unsigned char) c_CommentText;
        if (end1 && *end1) {
          const char* se = strstr (str, end1);
          if (se) {
            se += strlen (end1);
            if (*se) {
              int len = (int) (se - str);
              *fmt++ = (unsigned char) len;
              str = se;
              }
            else {
              *fmt++ = 0;
              str = 0;
              }
            m_outfmt = fmt_OK;
            }
          else {
            *fmt++ = 0;
            str = 0;
            }
          }
        break;
      //}}}
      //{{{
      case fmt_Comment2:
        *fmt++ = (unsigned char) CommentText2;
        if (end2 && *end2) {
          const char* se = strstr (str, end2);
          if (se) {
            se += strlen (end2);
            if (*se) {
              int len = (int) (se - str);
              *fmt++ = (unsigned char) len;
              str = se;
              }
            else {
              *fmt++ = 0;
              str = 0;
              }
            m_outfmt = fmt_OK;
            }
          else {
            *fmt++ = 0;
            str = 0;
            }
          }
        break;
      //}}}
      //{{{
      case fmt_String1:
        {
        const char* se = string1_char ? strchr (str, string1_char) : 0;
        const char* esc = 0;
        if (string_escape_char) {
          esc = strchr (str, string_escape_char);
          while (esc && se && (esc < se)) {
            se = strchr (esc+2, string1_char);         //must do this line first!
            esc = strchr (esc+2, string_escape_char);
            }
          while (!se && esc && *(esc+1))
            esc = strchr (esc+2, string_escape_char);
          }

        if (se) {
          //{{{  process se
          se += 1;
          int len = (int) (se - str);
          *fmt++ = (unsigned char) c_StringText;
          *fmt++ = (unsigned char) len;
          //}}}
          str = se;
          m_outfmt = fmt_OK;
          }
        else {
          if (esc && !(*(esc+1))) {
            //{{{  escape_char at eol = string extended to next line
            *fmt++ = (unsigned char) c_StringText;
            *fmt++ = 0;
            //}}}
            m_outfmt = fmt_String1;
            }
          else {
            //{{{  no close to string !!
            *fmt++ = (unsigned char) c_StringError;
            *fmt++ = 0;
            //}}}
            m_outfmt = fmt_OK;
            }
          str = 0;
          }
        }
        break;
      //}}}
      //{{{
      case fmt_String2:
        {
        *fmt++ = (unsigned char) c_StringText;
        const char* se = string2_char ? strchr (str, string2_char) : 0;
        if (se) {
          se += 1;
          int len = (int) (se - str);
          *fmt++ = (unsigned char) len;
          str = se;
          m_outfmt = fmt_OK;
          }
        else {
          *fmt++ = 0;
          str = 0;
          }
        }
        break;
      //}}}
      }

    if (str) {
      //{{{  scan for comments & strings & keywords
      const char* ssc1 = (start1 && *start1) ? strstr (str, start1) : 0;
      const char* ssc2 = (start2 && *start2) ? strstr (str, start2) : 0;
      const char* sss1 = string1_char ? strchr (str, string1_char) : 0;
      const char* sss2 = string2_char ? strchr (str, string2_char) : 0;
      if (sss1) {
        //{{{  ensure there is a closing quote for string1 or a final escape char
        const char* se   = strchr (sss1+1, string1_char);
        const char* esc = 0;
        if (string_escape_char) {
          esc = strchr (sss1+1, string_escape_char);
          while (esc && se && (esc < se)) {
            se = strchr (esc+2, string1_char);        //must do this line first!
            esc = strchr (esc+2, string_escape_char);
            }
          while (!se && esc && *(esc+1))
            esc = strchr (esc+2, string_escape_char);
          }
        if (!se && !(esc && !(*(esc+1))))
          sss1 = 0;
        //}}}
        }
      while (sss2) {
        //{{{  ensure there is a closing quote for string2 after 1 char
        const char* s  = sss2 + 1;
        if (string_escape_char && (*s == string_escape_char)) {
          s++;                                  // skip esc char
          //{{{  skip escaped char
          if ((*s >= '0') && (*s <= '7')) {     // C++ numeric octal byte
            s++;
            while ((*s >= '0') && (*s <= '7'))  // skip numeric octal byte
              s++;
            }
          else if (*s)
            s++;
          //}}}
          }
        else if (*s)
          s++;                                  // skip quoted char
        if (*s == string2_char)
          break;                                // valid char string
        sss2 = strchr (sss2+1, string2_char);   // search for another quote
        //}}}
        }

      const char* ss;
      //{{{  set ss to first of ssc1, ssc2, sss1, sss2
      if (ssc2 && (!ssc1 || (ssc2 <= ssc1)) && (!sss1 || (ssc2 <= sss1) ) && (!sss2 || (ssc2 <= sss2) ))
        ss = ssc2;
      else if (ssc1 && (!sss1 || (ssc1 <= sss1) ) && (!sss2 || (ssc1 <= sss2) ))
        ss = ssc1;
      else if (sss2 && (!sss1 || (sss2 <= sss1) ))
        ss = sss2;
      else
        ss = sss1;
      //}}}

      while (*str) {
        const char* sk = str;
        while (*sk && (!ss || (sk < ss)) && (fmt < new_format + 250)) {
          //{{{  scan for keywords
          while (*sk && !(isalnum (*sk) || (*sk == '_') || (*sk == '#')))
            sk++;
          if ((isalnum (*sk) || (*sk == '_') || (*sk == '#')) && (!ss || (sk < ss))) {
            //{{{  scan a word and check for keyword match
            const char* sks = sk;
            while (isalnum (*sk) || (*sk == '_') || (*sk == '#'))
              sk++;
            int len = (int) (sk - sks);
            if (len > 1) {
              if (language->valid_keyWord (sks, len)) {
                //{{{  insert keyword format
                if (sks > str) {
                  *fmt++ = (unsigned char) c_Text;
                  *fmt++ = (unsigned char) (sks - str);
                  }
                *fmt++ = (unsigned char) c_Keyword;
                if (*sk)
                  *fmt++ = (unsigned char) len;
                else
                  *fmt++ = 0;
                //}}}
                str = sk;
                }
              }
            //}}}
            }
          //}}}
          }
        if (ss && (fmt < new_format + 250)) {
          if (ss == ssc2) {
            //{{{  process ssc2
            int len = (int) (ssc2 - str);
            if (len) {
              *fmt++ = (unsigned char) c_Text;
              *fmt++ = (unsigned char) len;
              }
            if (end2 && *end2) {
              const char* se = strstr (ssc2 + strlen (start2), end2);
              if (se) {
                //{{{  process se
                se += strlen (end2);
                *fmt++ = (unsigned char) CommentText2;
                if (*se) {
                  int len = (int) (se - ssc2);
                  *fmt++ = (unsigned char) len;
                  }
                else
                  *fmt++ = 0;
                //}}}
                str = se;
                ssc2 = strstr (str, start2);
                if (ssc1 && (ssc1 < str))
                  ssc1 = strstr (str, start1);
                if (sss1 && (sss1 < str))
                  sss1 = strchr (str, string1_char);
                if (sss2 && (sss2 < str))
                  sss2 = strchr (str, string2_char);
                m_outfmt = fmt_OK;
                }
              else {
                //{{{  no close to comment !!
                *fmt++ = (unsigned char) CommentText2;
                *fmt++ = 0;
                m_outfmt = fmt_Comment2;
                //}}}
                break;  // out of while (*str)
                }
              }
            else {
              //{{{  comment to eol
              *fmt++ = (unsigned char) CommentText2;
              *fmt++ = 0;
              m_outfmt = fmt_OK;
              //}}}
              break;  // out of while (*str)
              }
            //}}}
            }
          else if (ss == ssc1) {
            //{{{  process ssc1
            int len = (int) (ssc1 - str);
            if (len) {
              *fmt++ = (unsigned char) c_Text;
              *fmt++ = (unsigned char) len;
              }
            if (end1 && *end1) {
              const char* se = strstr (ssc1 + strlen (start1), end1);
              if (se) {
                //{{{  process se
                se += strlen (end1);
                *fmt++ = (unsigned char) c_CommentText;
                if (*se) {
                  int len = (int) (se - ssc1);
                  *fmt++ = (unsigned char) len;
                  }
                else
                  *fmt++ = 0;
                //}}}
                str = se;
                ssc1 = strstr (str, start1);
                if (ssc2 && (ssc2 < str))
                  ssc2 = strstr (str, start2);
                if (sss1 && (sss1 < str))
                  sss1 = strchr (str, string1_char);
                if (sss2 && (sss2 < str))
                  sss2 = strchr (str, string2_char);
                m_outfmt = fmt_OK;
                }
              else {
                //{{{  no close to comment !!
                *fmt++ = (unsigned char) c_CommentText;
                *fmt++ = 0;
                m_outfmt = fmt_Comment1;
                //}}}
                break;  // out of while (*str)
                }
              }
            else {
              //{{{  comment to eol
              *fmt++ = (unsigned char) c_CommentText;
              *fmt++ = 0;
              m_outfmt = fmt_OK;
              //}}}
              break;  // out of while (*str)
              }
            //}}}
            }
          else if (ss == sss2) {
            //{{{  process sss2
            int len = (int) (sss2 - str);
            if (len) {
              *fmt++ = (unsigned char) c_Text;
              *fmt++ = (unsigned char) len;
              }
            const char* se = strchr (sss2 + 1, string2_char);
            if (string_escape_char) {
              const char* esc = strchr (sss2 + 1, string_escape_char);
              while (esc && se && (esc < se)) {
                se = strchr (esc+2, string2_char);        //must do this line first!
                esc = strchr (esc+2, string_escape_char);
                }
              }

            if (se) {
              //{{{  process se
              se += 1;
              *fmt++ = (unsigned char) c_StringText;
              if (*se) {
                int len = (int) (se - sss2);
                *fmt++ = (unsigned char) len;
                }
              else
                *fmt++ = 0;
              //}}}
              str = se;
              sss2 = strchr (str, string2_char);
              while (sss2) {
                //{{{  ensure there is a closing quote for string2 after 1 char
                const char* s  = sss2 + 1;
                if (string_escape_char && (*s == string_escape_char)) {
                  s++;                                  // skip esc char
                  //{{{  skip escaped char
                  if ((*s >= '0') && (*s <= '7')) {     // C++ numeric octal byte
                    s++;
                    while ((*s >= '0') && (*s <= '7'))  // skip numeric octal byte
                      s++;
                    }
                  else if (*s)
                    s++;
                  //}}}
                  }
                else if (*s)
                  s++;                                  // skip quoted char
                if (*s == string2_char)
                  break;                                // valid char string
                sss2 = strchr (sss2+1, string2_char);   // search for another quote
                //}}}
                }
              if (ssc1 && (ssc1 < str))
                ssc1 = strstr (str, start1);
              if (ssc2 && (ssc2 < str))
                ssc2 = strstr (str, start2);
              if (sss1 && (sss1 < str))
                sss1 = strchr (str, string1_char);
              m_outfmt = fmt_OK;
              }
            else {
              //{{{  no close to string !!
              *fmt++ = (unsigned char) c_StringText; //c_StringError
              *fmt++ = 0;
              m_outfmt = fmt_OK; //fmt_String2;
              //}}}
              break;  // out of while (*str)
              }
            //}}}
            }
          else if (ss == sss1) {
            //{{{  process sss1
            int len = (int) (sss1 - str);
            if (len) {
              *fmt++ = (unsigned char) c_Text;
              *fmt++ = (unsigned char) len;
              }
            const char* se = strchr (sss1 + 1, string1_char);
            const char* esc = 0;
            if (string_escape_char) {
              esc = strchr (sss1 + 1, string_escape_char);
              while (esc && se && (esc < se)) {
                se = strchr (esc+2, string1_char);        //must do this line first!
                esc = strchr (esc+2, string_escape_char);
                }
              while (!se && esc && *(esc+1))
                esc = strchr (esc+2, string_escape_char);
              }

            if (se) {
              //{{{  process se
              se += 1;
              *fmt++ = (unsigned char) c_StringText;
              if (*se) {
                int len = (int) (se - sss1);
                *fmt++ = (unsigned char) len;
                }
              else
                *fmt++ = 0;
              //}}}
              str = se;
              sss1 = strchr (str, string1_char);
              if (sss1) {
                //{{{  ensure there is a closing quote for string1 or a final escape char
                const char* se   = strchr (sss1+1, string1_char);
                const char* esc  = strchr (sss1+1, string_escape_char);
                if (string_escape_char) {
                  esc  = strchr (sss1+1, string_escape_char);
                  while (esc && se && (esc < se)) {
                    se = strchr (esc+2, string1_char);        //must do this line first!
                    esc = strchr (esc+2, string_escape_char);
                    }
                  }
                if (!se && !(esc && !(*(esc+1))))
                  sss1 = 0;
                //}}}
                }
              if (ssc1 && (ssc1 < str))
                ssc1 = strstr (str, start1);
              if (ssc2 && (ssc2 < str))
                ssc2 = strstr (str, start2);
              if (sss2 && (sss2 < str))
                sss2 = strchr (str, string2_char);
              m_outfmt = fmt_OK;
              }
            else {
              if (esc && !(*(esc+1))) {
                //{{{  escape_char at eol = string extended to next line
                *fmt++ = (unsigned char) c_StringText;
                *fmt++ = 0;
                m_outfmt = fmt_String1;
                //}}}
                }
              else {
                //{{{  no close to string !!
                *fmt++ = (unsigned char) c_StringError;
                *fmt++ = 0;
                m_outfmt = fmt_OK;
                //}}}
                }
              break;  // out of while (*str)
              }
            //}}}
            }
          //{{{  set ss to first of ssc1, ssc2, sss1, sss2
          if (ssc2 && (!ssc1 || (ssc2 <= ssc1)) && (!sss1 || (ssc2 <= sss1) ) && (!sss2 || (ssc2 <= sss2) ))
            ss = ssc2;
          else if (ssc1 && (!sss1 || (ssc1 <= sss1) ) && (!sss2 || (ssc1 <= sss2) ))
            ss = ssc1;
          else if (sss2 && (!sss1 || (sss2 <= sss1) ))
            ss = sss2;
          else
            ss = sss1;
          //}}}
          }
        else
          break;
        }
      if (*str && (m_outfmt == fmt_OK) && (fmt > new_format)) {
        *fmt++ = (unsigned char) c_Text;
        *fmt++ = 0;
        }
      //}}}
      }
    //}}}
    }
  else {
    switch (infmt) {
      //{{{
      case fmt_Comment1:
        *fmt++ = (unsigned char) c_CommentText;
        *fmt++ = 0;
        break;
      //}}}
      //{{{
      case fmt_Comment2:
        *fmt++ = (unsigned char) CommentText2;
        *fmt++ = 0;
        break;
      //}}}
      //{{{
      case fmt_String1:   case fmt_String2:
        *fmt++ = (unsigned char) c_StringText;
        *fmt++ = 0;
        break;
      //}}}
      }
    }

  int len = (int) (fmt - new_format);
  if (resize (len) && (len > 0))
    memcpy (m_format, new_format, len);

  return m_outfmt;
}
//}}}
//{{{
void cTextFmt::ClearFormat()
{
  if (m_format) {
    delete[] m_format;
    m_format = 0;
    }
  m_outfmt = fmt_OK;
}
//}}}
