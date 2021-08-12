#pragma once

#include "textline.h"

class cLanguage;
class cTextFold;
class cTextFoldEnd;

//{{{
class cTextFold  : public cTextLine {
friend cTextFoldEnd ;
public:
  enum eType { eNormal=0, eOutput, ePaste, eLines, eBuffers };

  cTextFold (cTextFold* fold = 0, cTextLine* previous = 0, eType type = eNormal) ;
  virtual ~cTextFold() ;
  virtual char* show_debug (char* str) const;

  // status
  virtual cTextFold* isFold() { return this; }
  virtual bool isFoldLine() const { return true; }
  virtual bool isOpen (int view) const;
  virtual bool hasEdits() const { return m_containsEdits; }
  virtual int count_lines() const;
  virtual int count_lines (int view) const;
  virtual int max_width (int view) const;
  virtual cTextLine* NextLine (int view);
  virtual cTextFoldEnd* FoldEnd() { return fold_end; }

  // read attributes
  eType type() const { return Type; }
  void GetAbove (long& x, long& y, long& scroll, int& level) const;
  void GetBelow (long& x, long& y, long& scroll) const;
  virtual cTextLine* GetTextLine();
  virtual cTextLine* GetTextLine (int view);
  virtual const char* GetText (int view) const;
  virtual const char* Text (int view, int& col) const;
  virtual int GetTextOffset (cTextFold* top) const { return cTextLine::GetTextOffset(top) + 5; }
  virtual int GetColour (int view, int xpos) const;
  virtual int TextLen (int view) const { return cTextLine::TextLen(view) + 5; }

  virtual int lineCount() const { return LineCount; }
  virtual int hiddenLines() const { return HiddenLines; }
  virtual int enclosedLines() const;

  cTextLine* cTextFold::GetLine (int ypos);
  cTextLine* cTextFold::GetLine (int view, int ypos);
  int CheckValid (int view, int ypos) const;   // returns a validated ypos
  int GetLinePos (cTextLine* line) const;

  // change attributes
  void SetAbove (long x, long y, long scroll, int level);
  void SetBelow (long x, long y, long scroll);
  virtual bool Open (int view);
  virtual bool Close (int view);
  virtual bool OpenAll (int view);
  virtual bool CloseAll (int view);
  virtual void SetContainsEdits (bool newval);
  virtual void SetCommentsOnly (bool newval);
  virtual void countLines();
  virtual void adjustLineCount (int num);

  // edit
  void InsertText (const char* pData, int len = 0);
  virtual void ReplaceText (int view, int xpos, int ch);
  virtual void InsertText (int view, int xpos, int ch, int len = 1);
  virtual void InsertText (int view, int xpos, const char* str, int len = 0);
  virtual void DeleteText (int view, int xpos, int len = 1);
  virtual void ToUpper (int view, int xpos, int len = 1);
  virtual void ToLower (int view, int xpos, int len = 1);
  virtual char* ExtractText (int view, int xpos, int len = 1);

  virtual cTextLine* SplitLine (int view, int xpos);
  virtual bool JoinLine (int view, int append_spaces = 1);

  virtual void RemoveFold (const cLanguage* language);
  virtual void CreateFoldEnd (cTextLine* after);
  cTextFoldEnd* Foldup (const cLanguage* language, int cur_indent = 0, cTextLine* last = 0);
  cTextFoldEnd* Format (const cLanguage* language);

  virtual cTextLine* Duplicate();
  virtual cTextLine* Copy();
  virtual cTextLine* Unlink();      // keeps the line number
  virtual cTextLine* Extract();     // loses line nuber
  virtual cTextLine* InsertAfter (cTextLine* line);

  virtual void MakeComment (int view, cLanguage* language);
  virtual void RemoveComment (int view, cLanguage* language);

  virtual bool RevertToOriginal (int view);
  virtual bool SetFormat (const cLanguage* language);

  // stream
  cTextFold& operator<< (const char* data);

protected:
  virtual cTextLine* next_line (int view);

  cTextFoldEnd* fold_end;

  int HiddenLines;

  bool m_containsEdits;
  bool m_commentsOnly;
  ubyte opened;              // opened flags (for up to 8 views)

  long above_x;              // Pos-of-fold
  long above_y;              // Pos-of-fold
  long above_scroll;
  int  above_level;          // number of folds-above opened
  //cTextFold* above_top;      // prev-CurTop

  long below_x;              // Pos-in-fold
  long below_y;              // Pos-in-fold
  long below_scroll;

  int LineCount;             // total number of lines enclosed
  bool StreamAppend;         // last-line has not yet received a '\n' from stream-input
  eType Type;
  };
//}}}
//{{{
class cTextFoldEnd  : public cTextLine
{
  friend cTextFold;
  protected:
    virtual cTextLine* visible_line (int view) {
      return (parent && !(parent->opened & view)) ? static_cast<cTextLine*>(parent) : this;
      }

  public:
    cTextFoldEnd (cTextFold* fold = 0, cTextLine* previous = 0, int lineCount = 0);
    virtual ~cTextFoldEnd();
    virtual char* show_debug (char* str) const;

    // status
    virtual cTextFoldEnd* isFoldEnd() { return this; }
    virtual bool isFoldLine() const { return true; }
    virtual bool isFileLine() const { return parent && parent->isFile() ? true : false; }
    virtual bool isProjLine() const { return parent && parent->isProj() ? true : false; }
    virtual bool isOpen (int view) const { return parent ? parent->isOpen (view) : false; }
    virtual bool isEdited() const
      { return parent && parent->isFileLine() ? false : m_edited; }

    // read attributes
    virtual cTextLine* PrevLine (int view) {
      return (parent && !(parent->opened & view)) ? parent->prev : prev;
      }
    virtual int TextLen (int view) const { return cTextLine::TextLen(view) + 5; }
    virtual const char* GetText (int view) const;
    virtual const char* Text (int view, int& col) const;
    virtual int GetColour (int view, int xpos) const
        { return parent ? parent->GetColour(view, xpos) : c_FoldOpen; }

    // set attributes
    virtual bool SetFormat (const cLanguage* language);

    // make changes
    virtual void ReplaceText (int view, int xpos, int ch);
    virtual void InsertText (int view, int xpos, int ch);
    virtual void InsertText (int view, int xpos, const char* str);
    virtual void DeleteText (int view, int xpos, int len = 1);
    virtual void ToUpper (int view, int xpos, int len = 1);
    virtual void ToLower (int view, int xpos, int len = 1);

    virtual void MakeComment (int view, cLanguage* language) {}
    virtual void RemoveComment (int view, cLanguage* language) {}

    virtual cTextLine* SplitLine (int view, int xpos);

} ; // class cTextFoldEnd
//}}}

inline cTextLine* cTextFold::next_line (int view)  {
  return (fold_end && !(opened & view)) ? fold_end->next : next;
  }
