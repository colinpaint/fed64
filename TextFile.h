#pragma once

#include "textfold.h"
#include "language.h"
#include <string>
class cUndo;

class cTextFile : public cTextFold {
public:
  cTextFile (cTextFold* fold = 0, cTextLine* previous = 0, eType type = eNormal) ;
  virtual ~cTextFile();
  virtual char* show_debug (char* str) const;

  // get status
  virtual cTextFile* isFile() { return this; }
  virtual bool isFileLine() const { return true; }
  virtual bool isNewFile() const { return newfile; }
  virtual bool isEmpty() const { return empty; }
  virtual bool isModified() const { return modified; }
  virtual bool isWriteProtected() const { return write_protect || read_only; }
  virtual bool isEditProtected() const { return edit_protect; }
  virtual bool isProtected() const { return write_protect || edit_protect || read_only; }
  virtual bool isEditable() const { return !write_protect && !edit_protect && !read_only; }
  virtual bool isNewerOnDisk();
  virtual bool hasTabs() const { return has_tabs; }

  // read attributes
  int Tablen (int type = 0) const { return m_tablen [type] ? m_tablen [type] : (language ? language->Tablen (type) : 8); }
  virtual int LineCount() const { return line_count; }
  virtual const char* Pathname() const { return pathname; }
  virtual std::string PathnameForDisplay() { return std::string (pathname ? pathname : ""); }
  virtual std::string DirPathname();

  cLanguage* Language() { return language; }
  const char* CommentStart() const { return language ? language->CommentStart() : 0; }
  const char* CommentEnd() const { return language ? language->CommentEnd() : 0; }

  CTime FileOnDiskTime() const { return m_disktime; }
  CTime FileTime() const { return m_filetime; }
  CTime CheckTime() const { return m_checktime; }

  cUndo* UndoList() { return m_undo_list; }
  cUndo* RedoList() { return m_redo_list; }
  void ClearUndoLists();
  void ClearRedoList();
  void AddUndo (cUndo* undo);
  cUndo* PopUndo();
  cUndo* PopRedo();

  virtual cTextLine* GetTextLine() { return static_cast<cTextLine*>(this); }
  virtual int GetColour (int view, int xpos) const;

  // change attributes
  virtual void SetTablen (int tablen, int type = 0) {
    if ((type >= 0) && (type <= 1))
      m_tablen [type] = tablen;
    }
  virtual void SetNew() { newfile = true; }
  virtual void SetNotEmpty() { empty = false; }
  virtual void SetFilename (const char* name);
  virtual void SetLanguage (cLanguage* lang) { language = lang; }
  virtual void SetModified (bool changed = true) { modified = changed; }
  virtual void SetProtect (bool protect = true) { edit_protect = protect; }
  virtual void SetupLanguage (cLanguage* changed_language = 0);
  virtual bool ToggleProtect();
  virtual bool ToggleTabs() { has_tabs = !has_tabs; return has_tabs; }
  virtual bool Renumber();
  virtual bool Open (int view);

  // calculate and read attributes
  int ViewToFilePos (int view, cTextFold* top, int ypos) const;
  int FileToViewPos (int view, cTextFold* top, int ypos) const;
  virtual cTextFile* PrevFile();
  virtual cTextFile* NextFile();
  void SetBelowTop (cTextFold* top);

  cTextLine* FindLine (int line_number);
  int GetNearestLinenum (cTextLine* line);

  // calculate and change attributes
  cTextFold* GetBelowTop();

  // edits - disallow
  virtual void ReplaceText (int view, int xpos, int ch) {}
  virtual void InsertText (int view, int xpos, int ch, int len = 1) {}
  virtual void InsertText (int view, int xpos, const char* str, int len = 0) {}
  virtual void DeleteText (int view, int xpos, int len = 1) {}
  virtual void ToUpper (int view, int xpos, int len = 1) {}
  virtual void ToLower (int view, int xpos, int len = 1) {}

  virtual cTextLine* Duplicate() { return 0; }
  virtual cTextLine* Copy() { return 0; }
  virtual cTextLine* Extract() { return 0; }

  virtual void MakeComment (int view, cLanguage* language) {}
  virtual void RemoveComment (int view, cLanguage* language) {}
  virtual void RemoveFold (const cLanguage* language) {}
  virtual bool RevertToOriginal (int view) { return false; }

  // File routines
  int OpenFile (const char* path);
  virtual int ReadFile();
  virtual int WriteFile();
  virtual bool Reload();
  virtual void Foldup();
  virtual void Format();

protected:
  cUndo* m_undo_list;
  cUndo* m_redo_list;

  char* pathname;
  cLanguage* language;
  bool newfile:1, empty:1, modified:1, write_protect:1, edit_protect:1, read_only:1, backup_done:1, has_tabs:1;
  int line_count;
  int m_tablen [2];

  CTime m_disktime;
  CTime m_filetime;          // time the file was last modified
  CTime m_checktime;         // time the filetime was last checked

  long below_top_y;      // prev curTop by line-number
  } ; // class cTextFile
