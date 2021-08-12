#pragma once

#include "colour.h"
#include "text.h"
#include "textfmt.h"

class cLanguage;
class cTextFold;
class cTextFoldEnd;
class cTextFile;
class cTextProj;

class cTextLine : public cText, public cTextFmt {
friend class cTextFold;
public:
  static cTextLine* readfile (const char* path);

  cTextLine (cTextFold* fold = 0, cTextLine* previous = 0) ;
  virtual ~cTextLine();
  virtual char* show_debug (char* str) const;

  // get status
  virtual cTextLine* PrevLine (int view) { return prev ? prev->visible_line (view) : 0; }
  virtual cTextLine* NextLine (int view) { return next; }

  bool isWithin (cTextFold* fold) const;
  bool isEdited() const { return m_edited; }
  bool hasDeletedBelow() const { return m_deletedBelow; }

  virtual cTextFold* isFold() { return 0; }
  virtual cTextFoldEnd* isFoldEnd() { return 0; }
  virtual cTextFile* isFile() { return 0; }
  virtual cTextProj* isProj() { return 0; }
  virtual bool isFoldLine() const { return false; }
  virtual bool isBufferLine() const { return false; }
  virtual bool isFileLine() const { return false; }
  virtual bool isProjLine() const { return false; }

  // read attributes
  cTextLine* Prev() { return prev; }
  cTextLine* Next() { return next; }
  cTextFold* ParentFold() { return parent; }
  int Indent() const { return (int) indent; }
  int LineNum() const { return line_num; }
  virtual int lineCount() const { return 1; }
  int GetIndent (cTextFold* top) const;
  int GetFoldIndent (cTextFold* top) const;
  virtual int GetTextOffset (cTextFold* top) const { return indent + GetFoldIndent(top); }
  virtual int GetColour (int view, int xpos) const;
  eTextFmt Infmt() const { return prev ? prev->Outfmt() : fmt_OK; }

  virtual int toTextPos (cTextFold* top, int x) const { return x - GetFoldIndent(top) - indent; }
  virtual cTextLine* GetTextLine (int view) { return this; };
  virtual char GetChar (int view, int xpos) const;
  virtual const char* GetTextAt (int view, int xpos) const;
  virtual const char* GetText (int view) const;
  virtual const char* Text() const { return (const char*) text; }
  virtual const char* Text (int view, int& col) const;
  virtual int TextLen() const;
  virtual int TextLen (int view) const;
  virtual const char* OriginalText() const { return (const char*) original_text; }
  int GetPosInFold (cTextFold* fold) const;

  // change attributes
  void SetEdited (bool newval = true);
  void SetDeletedBelow (bool newval = true);
  void SetLinenum (int num) { line_num = num; m_edited = false; m_deletedBelow = false; }
  bool OpenToTop (int view, cTextFold* topfold);

  // edits
  void GrabContents (cTextLine* old);

  virtual cTextLine* SplitLine (int view, int xpos);
  virtual bool JoinLine (int view, int append_spaces = 1);

  virtual void ReplaceText (int view, int xpos, int ch);
  virtual void InsertText (int view, int xpos, int ch, int len = 1);
  virtual void InsertText (int view, int xpos, const char* str, int len = 0);
  virtual void DeleteText (int view, int xpos, int len = 1);
  virtual void ToUpper (int view, int xpos, int len = 1);
  virtual void ToLower (int view, int xpos, int len = 1);
  virtual char* ExtractText (int view, int xpos, int len = 1);

  virtual cTextLine* Duplicate();
  virtual cTextLine* Copy();
  virtual cTextLine* Copy (int xpos, int len);    // Column Copy
  virtual cTextLine* Unlink();      // keeps the line number
  virtual cTextLine* Extract();     // loses line nuber
  virtual cTextLine* Extract (int xpos, int len); // Column Extract
  virtual cTextLine* InsertBefore (cTextLine* line);
  virtual cTextLine* InsertAfter (cTextLine* line);

  virtual void MakeComment (int view, cLanguage* language);
  virtual void RemoveComment (int view, cLanguage* language);

  virtual bool RevertToOriginal (int view);
  virtual bool SetFormat (const cLanguage* language);

protected:
  enum eConstants {cMaxLineLen = 256};
  static const char cNullStr [];

  virtual cTextLine* visible_line (int view) { return this; }
  virtual cTextLine* prev_line (int view) { return prev ? prev->visible_line (view) : 0; }
  virtual cTextLine* next_line (int view) { return next; }

  cTextLine* prev;
  cTextLine* next;
  int line_num;
  cTextFold* parent;
  bool m_edited;
  bool m_deletedBelow;
  };
