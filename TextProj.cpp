// TextProj.cpp
//{{{  includes
#include "pch.h"
#include "textproj.h"
//}}}

cTextProj* cTextProj::m_topProject = 0;
cTextProj* cTextProj::m_bufferProject = 0;
//{{{
cTextProj::cTextProj (cTextFold* fold, cTextLine* previous, eType type)
   : cTextFile (fold, previous, type) {

  if (!fold) {
    if (type == eBuffers) {
      ASSERT (m_bufferProject == 0);
      m_bufferProject = this;
      }
    else {
      ASSERT (m_topProject == 0);
      m_topProject = this;
      }
    opened = 0xff;    // all views open
    }
  }
//}}}
//{{{
cTextProj::~cTextProj() {

  if (m_topProject == this)
    m_topProject = 0;
  }
//}}}

//{{{
bool cTextProj::isInUse (const cLanguage* language) {

  cTextFile* cur = m_topProject;
  while (cur) {
    if (cur->Language() == language)
      return true;
    else if (cur->isProj())
      // pop down into project
      cur = cur->isProj()->FirstFile();
    else if (cur->NextFile())
      cur = cur->NextFile();
    else if (cur->ParentFold() && cur->ParentFold()->isFile())
      // pop up & continue scanning project above
      cur = cur->ParentFold()->isFile()->NextFile();
    else
      return false;
    }

  return false;
  }
//}}}
//{{{
bool cTextProj::isProjectExt (const char* filename) {

  const char* ext = strrchr (filename, '.');
  if (ext)
    if (_stricmp (ext, ".fed") == 0)
      return true;

  return false;
  }
//}}}

//{{{
char* cTextProj::show_debug (char* str) const {

  sprintf (str, "cTextProj:");
  char* s = strchr (str, 0);
  *s++ = 0;
  *s++ = ' ';

  return cTextFile::show_debug (s);
  }
//}}}

//{{{
std::string cTextProj::PathnameForDisplay() {

  if (pathname && *pathname)
    return std::string (pathname);
  if (!parent || !parent->isProj())
    return std::string();

  std::string ret = parent->isProj()->PathnameForDisplay();
  ret += " - ";
  ret += text;

  return ret;
  }
//}}}
//{{{
std::string cTextProj::DirPathname() {

  cTextProj* proj = static_cast<cTextProj*>(this);
  while (proj)
    {
    if (proj->pathname && *(proj->pathname))
      return proj->cTextFile::DirPathname();

    if (!proj->ParentFold())
      break;

    proj = proj->ParentFold()->isProj();
    }

  return std::string();
  }
//}}}

//{{{
cTextFile* cTextProj::FirstFile() const {

  return next ? next->isFile() : 0;
  }
//}}}
//{{{
cTextFile* cTextProj::LastFile() const {

  if (fold_end && fold_end->Prev() && fold_end->Prev()->isFoldEnd())
    if (fold_end->Prev()->ParentFold())
      return fold_end->Prev()->ParentFold()->isFile();

  return 0;
  }
//}}}

//{{{
int cTextProj::FileCount() {

  int count = 0;
  cTextFile* cur = next ? next->isFile() : 0;
  while (cur) {
    count += 1;
    cur = (cur->FoldEnd() && cur->FoldEnd()->Next()) ? cur->FoldEnd()->Next()->isFile() : 0;
    }

  return count;
  }
//}}}
//{{{
int cTextProj::UnsavedCount (bool recursive) {

  int count = 0;
  cTextFile* cur = next ? next->isFile() : 0;
  while (cur) {
    if (cur->isProj()) {
      if (recursive)
        count += cur->isProj()->UnsavedCount (recursive);
      }
    else if (cur->isModified())
      count += 1;
    cur = (cur->FoldEnd() && cur->FoldEnd()->Next()) ? cur->FoldEnd()->Next()->isFile() : 0;
    }

  return count;
  }
//}}}
//{{{
void cTextProj::RemoveFold (const cLanguage* language) {

  if (fold_end) {
    delete fold_end;
    fold_end = 0;
    }

  delete this;
  }
//}}}

//{{{
int cTextProj::ReadFile() {

  if (!pathname || !*pathname)
    return -1;

  char* ext = strrchr (pathname, '.');
  if (ext)
    ext++;

  return ReadFile_Fed();
  }
//}}}
//{{{
int cTextProj::WriteFile() {

  if (!pathname || !*pathname || !fold_end || write_protect)
    return 0;

  char* ext = strrchr (pathname, '.');
  if (ext && (_stricmp (ext, ".fed") != 0))
    return 0;  // don't modify Visual Studio project files

  FILE* fp = fopen (pathname, "w");
  // check ok
  if (!fp) {
    CString msg;
    msg.Format ("Unable to open file '%s' for writing", pathname);
    AfxMessageBox (msg);
    return 0;
    }

  cTextFile* cur = FirstFile();
  while (cur) {
    if (cur->cTextLine::Text()) {
      if (cur->isEditProtected())
        fprintf (fp, "!%s\n", cur->cTextLine::Text());
      else
        fprintf (fp, "%s\n", cur->cTextLine::Text());
      }
    cur = cur->NextFile();
    }
  fclose (fp);

  modified = false;
  return 1;
  }
//}}}

//{{{
cTextFile* cTextProj::FindFile (const char* path, bool searchAll) {

  if (path && (*path == '!'))
    path++;
  if (!path || !*path)
    return 0;

  char fullpath [_MAX_PATH];
  ::GetFullPathName (path, _MAX_PATH, fullpath, 0);

  return FindFullFile (fullpath, searchAll);
  }
//}}}
//{{{
cTextFile* cTextProj::AddFile (const char* path, cTextLine* after) {

  if (!path && !*path)
    return 0;
  bool protect = (*path == '!');

  if (protect && !*(++path))
    return 0;

  if (after && (after != this)) {
    //{{{  check that after is valid
    cTextFile* file = next ? next->isFile() : 0;
    while (file) {
      cTextLine* fileend = file->FoldEnd();
      if ((file == after) || (fileend == after))
        break;
      file = fileend && fileend->Next() ? fileend->Next()->isFile() : 0;
      }
    if (file && file->FoldEnd())
      after = file->FoldEnd();
    else
      after = 0;
    //}}}
    }
  if (!after)
    after = (fold_end && fold_end->Prev()) ? fold_end->Prev() : this;
  cTextFile* newfile;

  char fullpath [256];
  ::GetFullPathName (path, 256, fullpath, 0);

  //{{{  check if we already have it
  cTextFile* file = FirstFile();
  while (file) {
    if (file->Pathname() && (_stricmp (file->Pathname(), fullpath) == 0))
      return file;
    file = file->NextFile();
    }
  //}}}

  bool myself = pathname && (_stricmp (pathname, fullpath) == 0);
  const char* ext = strrchr (path, '.');
  if (ext) ext++;

  if (!myself && isProjectExt (path)) {
    newfile = new cTextProj (this, after);
    new cTextFoldEnd (newfile, newfile);
    newfile->SetFilename (path);
    newfile->ReadFile();
    }
  else
    {
    newfile = new cTextFile (this, after);
    new cTextFoldEnd (newfile, newfile);
    newfile->SetFilename (path);

    const char* ext = strrchr (path, '.');
    if (ext++)
    if (*ext) {
      _strlwr ((char*)ext);
      newfile->SetLanguage (cLanguage::FindExtension (ext));
      }
    }

  newfile->SetProtect (protect);

  modified = true;
  return newfile;
  }
//}}}
//{{{
void cTextProj::setupSubprojectFlags() {

  empty = false;
  modified = false;
  read_only = true;
  }
//}}}

// private
//{{{
cTextFile* cTextProj::FindFullFile (const char* fullpath, bool searchAll) {

  cTextFile* file = FirstFile();
  while (file) {
    if (file->Pathname() && (_stricmp (file->Pathname(), fullpath) == 0))
      return file;
    if (searchAll && file->isProj()) {
      cTextFile* foundfile = file->isProj()->FindFile (fullpath);
      if (foundfile)
        return foundfile;
      }
    file = file->NextFile();
    }

  return file;
  }
//}}}

//{{{
int cTextProj::ReadFile_Fed() {
// .fed project file

  if (!pathname || !*pathname)
    return -1;
  int filecount = 0;
  FILE* fp = fopen (pathname, "r");
  //{{{  check ok
  if (!fp) {
    CString msg;
    msg.Format ("Unable to open file '%s' for reading", pathname);
    AfxMessageBox (msg);
    return -1;
    }
  //}}}

  char path [cMaxLineLen];
  strcpy (path, pathname);
  char* name = strrchr (path, '\\');
  if (name)
    name++;
  else
    name = path;

  char line [cMaxLineLen];

  while (fgets (line, cMaxLineLen, fp) ) {
    char* s = strchr (line, '\n');
    if (s)
      *s = 0;
    char* filename = (*line == '!') ? line+1 : line;
    if ((*filename != '\\') && (*(filename+1) != ':')) {
      // pre-pend the project directory
      strcpy (name, filename);
      filename = path;
      }

    cTextFile* file;
    if ((strchr (filename, '*')) || (strchr (filename, '?'))) {
      // wildcards
      CFileFind find;
      BOOL found = find.FindFile (filename);
      while (found) {
        found = find.FindNextFile();
        if (found) {
          file = AddFile (find.GetFilePath());
          if (file && (*line == '!'))
            file->SetProtect (true);
          filecount += 1;
          }
        }
      find.Close();
      }
    else {  // single file
      file = AddFile (filename);
      if (file && (*line == '!'))
        file->SetProtect (true);
      filecount += 1;
      }
    }

  fclose (fp);

  HiddenLines = filecount;

  empty = false;
  modified = false;

  return filecount;
  }
//}}}
