#pragma once

#include "language.h"
#include "textfile.h"

//{{{
class cRange : public CObject {
public:
  enum eHint { eNop,
    eChangedLine, eSplitLine, eJoinLine, eDeleteLine, eWarnDeleteFold, eDeleteFold,
    eInsertRange, eDeleteRange, eChanged, eRenumbered,
    eFontChanged
    };

  cRange();
  cRange (cRange& range);
  cRange (int view, cTextFile* file);
  cRange (int view, cTextFile* file, cTextFold* fold, CPoint pos);
  cRange (int view, cTextFile* file, cTextFold* fold, CPoint pos1, CPoint pos2);
  virtual ~cRange() {}

  void Set (int view, cTextFile* file, cTextFold* fold, CPoint pos1, CPoint pos2);
  void SetColumn (BOOL toColumn) { m_column = toColumn; }
  void SetView (int view);
  void SetFile (cTextFile* file);
  void SetTopFold (cTextFold* fold);
  void SetRange (CPoint pos);
  void SetRange (CPoint pos1, CPoint pos2);
  void SetRange (cTextLine* line);
  void SetRange (cTextLine* line1, cTextLine* line2);
  void ExtendToFoldEnd();
  void ExtendToWholeLines();
  bool Reformat();

  BOOL Column()     { return m_column; }
  int View()        { return m_view; }
  cTextFile* File() { return m_file; }
  cTextFold* Fold() { return m_topFold; }
  CPoint Pos()      { return m_startPos; }
  CPoint StartPos() { return m_startPos; }
  CPoint EndPos()   { return m_endPos; }
  int Xpos()        { return m_startPos.x; }
  int Ypos()        { return m_startPos.y; }
  int FirstXpos()   { return m_startPos.x; }
  int LastXpos()    { return m_endPos.x; }
  int FirstYpos()   { return m_startPos.y; }
  int LastYpos()    { return m_endPos.y; }
  int Xlen() { return (m_endPos.x >= m_startPos.x) ? m_endPos.x - m_startPos.x : 0; }
  int Ylen() { return (m_endPos.y >= m_startPos.y) ? m_endPos.y - m_startPos.y + 1 : 0; }

  cLanguage* Language() { return m_file ? m_file->Language() : 0; }
  const char* CommentStart() { return m_file ? m_file->CommentStart() : 0; }
  const char* CommentEnd() { return m_file ? m_file->CommentEnd() : 0; }

  cTextLine* Line()      { return m_firstLine ? m_firstLine : set_firstLine(); }
  cTextLine* EditLine();
  cTextLine* FirstLine() { return m_firstLine ? m_firstLine : set_firstLine(); }
  cTextLine* LastLine()  { return m_lastLine ? m_lastLine : set_lastLine(); }

  bool isRange()      { return (m_endPos.y > m_startPos.y); }
  bool isSinglePoint() { return (m_startPos.x == m_endPos.x) && (m_startPos.y == m_endPos.y); }
  bool isSingleLine() { return (m_startPos.y == m_endPos.y); }
  bool isWholeLines() { return (m_startPos.x == 0) && (m_endPos.x == 0); }
  bool isEditable()   { return m_file && m_file->isEditable(); }
  bool isOpenFold() { return Line()->isFold() && Line()->isFold()->isOpen(m_view); }
  bool FileIsProj()   { return m_file && m_file->isProj(); }

  bool isBlankLine();
  bool isComment();
  bool isAlpha();
  bool isMatch (const char* str, int len = 0);

  const unsigned char* FindMacro (const char* name, int len);
  char* CopyRangeText();

protected:
  cTextLine* m_firstLine;
  cTextLine* m_lastLine;

  cTextLine* set_firstLine();
  cTextLine* set_lastLine();

  int        m_view;
  cTextFile* m_file;
  cTextFold* m_topFold;
  CPoint     m_startPos;
  CPoint     m_endPos;
  BOOL       m_column;
  };
//}}}
//{{{
class cChange : public cRange {
friend class cTextFile;
public:
  cChange (cRange& range, const char* insert_text = 0);
  cChange (cChange& change);
  virtual ~cChange();

  const char* Text() { return m_text; }
  int InsertLength() const { return m_text ? (int)strlen (m_text) : 0; }

protected:
  char* m_text;
  };
//}}}
//{{{
class cUndo : public cChange {
friend class cTextFile;
public:
  cUndo (cRange& range, const char* insert_text = 0);
  cUndo (cChange& change);
  virtual ~cUndo();

  cUndo* Prev() { return prev; }
  cUndo* Add (cUndo* head);

protected:
  cUndo* prev;
  };
//}}}
