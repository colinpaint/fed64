#pragma once

class cLanguage;

enum eTextFmt { fmt_OK, fmt_Comment1, fmt_Comment2, fmt_String1, fmt_String2 };

class cTextFmt {
public:
  cTextFmt();

  virtual ~cTextFmt();
  char* show_format (char* str) const;

  bool isComment() const;
  eTextFmt Outfmt() const { return m_outfmt; }
  int get_format_colour (int xpos) const;
  const unsigned char* GetFormat() const { return (const unsigned char*) m_format; }
  void ClearFormat();
  eTextFmt SetFormat (const char* str, const cLanguage* language, eTextFmt within = fmt_OK);

protected:
  unsigned char* m_format;
  eTextFmt m_outfmt;

  bool resize (int len);
  };
