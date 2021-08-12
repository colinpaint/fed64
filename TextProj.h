#pragma once

#include "textfile.h"

class cTextProj : public cTextFile {
public:
  cTextProj (cTextFold* fold = 0, cTextLine* previous = 0, eType type = eNormal) ;
  virtual ~cTextProj() ;
  virtual char* show_debug (char* str) const;

  static bool isInUse (const cLanguage* language);
  static cTextProj* TopProject() { return m_topProject; }
  static bool isProjectExt (const char* filename);   // return true for supported project types

  virtual std::string PathnameForDisplay();
  virtual std::string DirPathname();

  void setupSubprojectFlags();

  virtual cTextFile* FirstFile() const;
  virtual cTextFile* LastFile() const;

  virtual bool OpenAll (int view) { return false; }
  //virtual bool CloseAll (int view);

  virtual bool isNewFile() const { return (pathname == NULL) || (*pathname == 0); }
  virtual bool isEmpty() const { return next && (next == fold_end); }
  virtual bool isEditable() const { return false; } //true; }
  virtual cTextProj* isProj() { return this; }
  virtual bool isProjLine() const { return true; }

  virtual int GetColour (int view, int xpos) const { return parent ? c_TopProj : c_Proj; }

  virtual void Foldup() {}
  virtual void Format() {}
  virtual void RemoveFold (const cLanguage* language);

  virtual int ReadFile();
  virtual int WriteFile();

  cTextFile* FindFile (const char* path, bool searchAll = true);
  cTextFile* AddFile (const char* path, cTextLine* after = 0);
  int FileCount();
  int UnsavedCount (bool recursive = true);

protected:
  static cTextProj* m_topProject;
  static cTextProj* m_bufferProject;

private:
  cTextFile* FindFullFile (const char* fullpath, bool searchAll);
  int ReadFile_Fed();
  int ReadFile_VcProj();
} ; // class cTextProj
