#pragma once

typedef unsigned char ubyte;

class cText {
public:
  cText (int length = 0);
  virtual ~cText();
  char* show_text (char* str) const;
  virtual char* show_debug (char* str) const;

  void convert_tabs (int fold_indent, int tablen = 8);
  void set_text (const char* str, int len, int tablen = 8);
  void set_text (const char* str, bool strip_trailing = true, int tablen = 8);

  void append_text (const char* str, int len); //, int tablen = 8);
  void append_text (const char* str, int append_spaces, bool strip_trailing);
  void append_text (const char* str) { append_text (str, 0, true); }
  char get_text_char (int xpos) const;
  void copy_text (char* str, int xpos, int len) const; // assumes that str is at least len+1

  void set_indent (int spaces);
  void change_indent (int by);
  void absorb_leading();
  void trim_trailing();
  bool revert_to_original();
  void delete_original();

  char* extract_text (int xpos, int length);
  // the following routines all return the new xpos
  int overwrite_text (int xpos, int ch, int length = 1);
  int overwrite_text (int xpos, const char* str, int length = -1);
  int insert_text (int xpos, int ch, int length = 1);
  int insert_text (int xpos, const char* str, int length = -1);
  int replace_text (int xpos, int xlen, const char* str, int length = -1);
  int delete_text (int xpos, int length = 1);
  int delete_to_sol (int xpos);
  int delete_to_eol (int xpos);
  int tolower_text (int xpos, int length = 1);
  int toupper_text (int xpos, int length = 1);

  int word_left (int xpos) const;
  int word_right (int xpos) const;
  int get_sot() const;
  int get_eol() const;

protected:
  bool resize (int length);
  void grabtext (cText* old);
  void remove_text();
  void set_original();

  char* text;
  int text_len;       // allocated length of 'text'
  ubyte indent;
  ubyte original_indent;
  char* original_text;
  };
