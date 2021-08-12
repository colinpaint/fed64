// TextFile.cpp
//{{{  includes
#include "pch.h"
#include "textfile.h"

#include "colour.h"
#include "language.h"
#include "text.h"
#include "textline.h"
#include "textfold.h"
#include "textproj.h"
#include "option.h"
#include "range.h"
//}}}

//{{{
cTextFile::cTextFile (cTextFold* fold, cTextLine* previous, eType type)
 : cTextFold (fold, previous, type),
  m_undo_list (NULL),
  m_redo_list (NULL),
  pathname (NULL),
  language (NULL),

  newfile (type == eNormal),
  empty (type == eNormal),
  modified (FALSE),
  write_protect (type != eNormal),
  edit_protect (type != eNormal),
  read_only (type != eNormal),
  backup_done (FALSE),
  has_tabs (FALSE),

  line_count (0),
  below_top_y (0),
  m_disktime (33, 0),
  m_filetime (33, 0),
  m_checktime (33, 0)
{
  m_tablen [0] = 0;
  m_tablen [1] = 0;

  below_y = 1;      // cTextFold variable

  SetEdited (false);
}
//}}}
//{{{
cTextFile::~cTextFile() {

  if (pathname)
    delete[] pathname;

  ClearUndoLists();
  }
//}}}

//{{{
char* cTextFile::show_debug (char* str) const {

  char* s = str;
  sprintf (s, "cTextFile: %s new:%1d empty:%1d mod:%1d eprot:%1d wprot:%1d ronly:%1d backup:%1d tabs:%1d",
     language && language->Name() ? language->Name() : "",
     ((int) newfile) & 1,
     ((int) empty) & 1,
     ((int) modified) & 1,
     ((int) edit_protect) & 1,
     ((int) write_protect) & 1,
     ((int) read_only) & 1,
     ((int) backup_done) & 1,
     ((int) has_tabs) & 1
     );
  s = strchr (s, 0);
  *s++ = 0;
  sprintf (s, "  lines:%d tab:%d:%d below_top:%d",
     line_count,
     m_tablen[0], m_tablen[1],
     below_top_y
     );
  s = strchr (s, 0);
  *s++ = 0;
  strcpy (s, m_filetime.Format ("  filetime:%c"));

  s = strchr (s, 0);
  strcpy (s, m_checktime.Format (" checktime:%c"));

  s = strchr (s, 0);
  *s++ = 0;
  *s++ = ' ';
  if (pathname) {
    *s++ = ' ';
    sprintf (s, "path:'%s'", pathname);
    s = strchr (s, 0);
    *s++ = 0;
    *s++ = ' ';
    }

  return cTextFold::show_debug (s);
  }
//}}}

// get status
//{{{
bool cTextFile::isNewerOnDisk() {

  CFileStatus status;
  if (pathname && *pathname && CFile::GetStatus (pathname, status)) {
    if (status.m_attribute & CFile::readOnly)
      write_protect = true;
    else
      write_protect = false;
    m_disktime = status.m_mtime;

    int delta = cOption::FileNewerDelta;
    if (delta < 0) delta = 0;
    if (delta > 59) delta = 59;

    if ((m_filetime + CTimeSpan (0, 0, 0, delta) < m_disktime) && (m_checktime < m_disktime)) {
      m_checktime = m_disktime; //CTime::GetCurrentTime();
      return true;
      }
    }
  return false;
  }
//}}}

// get attributes
//{{{
cTextFold* cTextFile::GetBelowTop() {

  int y = 0;
  cTextLine* cur = this;
  while (cur && (cur != fold_end)) {
    if (y == below_top_y) {
      if (cur->isFold())
        return cur->isFold();
      if (cur->ParentFold())
        return cur->ParentFold();
      break;
      }
    y += 1;
    cur = cur->Next();
    }
  return this;
  }
//}}}
//{{{
cTextFile* cTextFile::PrevFile() {

  if (prev && prev->isFoldEnd() && prev->ParentFold())
    return prev->ParentFold()->isFile();
  return 0;
  }
//}}}
//{{{
cTextFile* cTextFile::NextFile() {

  if (fold_end && fold_end->Next())
    return fold_end->Next()->isFile();
  return 0;
  }
//}}}

//{{{
std::string cTextFile::DirPathname() {

  std::string dir (pathname);
  int index = (int)dir.rfind ('\\');   // search backwards for directory separator
  if (index > 0)
    dir.erase (index+1);
  else {
    index = (int)dir.find (':');       // eg c:dummy.txt
    if (index == 1)
      dir.erase (2);
    else
      dir.clear();
    }

  return dir;
  }
//}}}

//{{{
int cTextFile::GetNearestLinenum (cTextLine* line) {
// search outwards from line, returning the nearest valid linenum

  cTextLine* up = line;
  cTextLine* dn = line;
  while (up && dn && (up != this) && (dn != fold_end)) {
    if (up->LineNum() > 0)
      return up->LineNum();
    if (dn->LineNum() > 0)
      return dn->LineNum();
    up = up->Prev();
    dn = dn->Next();
    }
  if (dn == fold_end)
    return -1;

  return 0;
  }
//}}}
//{{{
cTextLine* cTextFile::FindLine (int line_number) {

  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    if (cur->LineNum() == line_number)
      return cur;
    cur = cur->Next();
    }

  return 0;
  }
//}}}
//{{{
int cTextFile::GetColour (int view, int xpos) const {

  if (write_protect || edit_protect)
    return c_ProtectedFile;

  return c_File;
  }
//}}}

//{{{
int cTextFile::ViewToFilePos (int view, cTextFold* top, int ypos) const {

  int y = 0;
  cTextLine* cur = static_cast<cTextLine*>(const_cast<cTextFile*>(this));
  while (cur && (cur != top) && (cur != fold_end)) {
    y++;
    cur = cur->Next();
    }

  int closed_fold_count = 0;
  while ((ypos > 0) && cur && (cur != fold_end)) {
    if (cur->isFold() && !cur->isFold()->isOpen(view))
      closed_fold_count += 1;
    else if (cur->isFoldEnd() && !cur->isFoldEnd()->isOpen(view))
      closed_fold_count -= 1;
    if (closed_fold_count <= 0)
      ypos--;
    y++;
    cur = cur->Next();
    }

  return y;
  }
//}}}
//{{{
int cTextFile::FileToViewPos (int view, cTextFold* top, int ypos) const {

  int y = 0;
  cTextLine* cur = static_cast<cTextLine*>(const_cast<cTextFile*>(this));
  while ((ypos > 0) && cur && (cur != top) && (cur != fold_end)) {
    ypos--;
    cur = cur->Next();
    }

  int closed_fold_count = 0;
  while ((ypos > 0) && cur && (cur != fold_end)) {
    if (cur->isFold() && !cur->isFold()->isOpen(view))
      closed_fold_count += 1;
    else if (cur->isFoldEnd() && !cur->isFoldEnd()->isOpen(view))
      closed_fold_count -= 1;
    if (closed_fold_count <= 0)
      y++;
    ypos--;
    cur = cur->Next();
    }

  if (closed_fold_count <= 0)
    return y;

  return -1;
  }
//}}}

// change attributes
//{{{
bool cTextFile::ToggleProtect() {

  if (write_protect)
    return false;
  edit_protect = !edit_protect;

  return true;
  }
//}}}
//{{{
bool cTextFile::Renumber() {

  if (fold_end) {
    int num = 1;
    cTextLine* cur = Next();
    while (cur && (cur != fold_end)) {
      cur->SetLinenum (num++);
      cur = cur->Next();
      }
    return true;
    }

  return false;
  }
//}}}
//{{{
void cTextFile::SetFilename (const char* name) {

  if (name && *name) {
    char path [_MAX_PATH];
    ::GetFullPathName (name, _MAX_PATH, path, 0);
    int len = (int)strlen(path);
    if (pathname)
      delete[] pathname;
    pathname = new char [len + 1];
    strcpy (pathname, path);

    write_protect = false;
    CFileStatus status;
    if (CFile::GetStatus (pathname, status)) {
      if (status.m_attribute & CFile::readOnly)
        write_protect = true;
      m_filetime = status.m_mtime;
      newfile = false;
      }

    // remove proj dir (or part of proj dir) from filename if it matches
    set_text (0);
    cTextProj* project = parent ? parent->isProj() : 0;
    if (project) {
      char proj_dir [_MAX_PATH];
      strcpy (proj_dir, project->DirPathname().c_str());
      int level = 0;
      char* first_dir = strchr (proj_dir, '\\');
      char* last_dir = strrchr (proj_dir, '\\');
      while (first_dir && last_dir && (last_dir > first_dir) && (level < 3)) {
        *(last_dir+1) = 0;
        int proj_len = int(last_dir - proj_dir) + 1;
        int path_len = (int)strlen(path);

        if ((proj_len < path_len) && (_memicmp (proj_dir, path, proj_len) == 0)) {
          append_text (path + proj_len);
          return;
          }

        append_text ("..\\");
        *last_dir = 0;
        last_dir = strrchr (proj_dir, '\\');  // search for partial match of project
        level += 1;
        }
      }

    set_text (pathname);
    }
  }
//}}}
//{{{
void cTextFile::SetBelowTop (cTextFold* top) {

  int y = 0;
  cTextLine* cur = this;
  while (cur && (cur != fold_end)) {
    if (cur == top) {
      below_top_y = y;
      return;
      }
    y += 1;
    cur = cur->Next();
    }

  below_top_y = 0;
  }
//}}}
//{{{
bool cTextFile::Open (int view) {

  if (empty) {
    CWaitCursor waitcursor;
    ReadFile();
    Foldup();
    Format();
    }

  return cTextFold::Open (view);
  }
//}}}

//{{{
void cTextFile::AddUndo (cUndo* undo) {

  if (undo) {
    if (m_undo_list)
      m_undo_list = m_undo_list->Add (undo);
    else
      m_undo_list = undo;
    ClearRedoList();
    }
  }
//}}}
//{{{
cUndo* cTextFile::PopUndo() {

  cUndo* top = m_undo_list;
  if (top) {
    m_undo_list = top->prev;

    // transfer to redo list
    cChange& change = *top;
    cUndo* redo = new cUndo (change);
    if (m_redo_list)
      m_redo_list = m_redo_list->Add (redo);
    else
      m_redo_list = redo;
    }

  return top;
  }
//}}}
//{{{
cUndo* cTextFile::PopRedo() {

  cUndo* top = m_redo_list;
  if (top)
    {
    m_redo_list = top->prev;

    // transfer to undo list
    cChange& change = *top;
    cUndo* undo = new cUndo (change);
    if (m_undo_list)
      m_undo_list = m_undo_list->Add (undo);
    else
      m_undo_list = undo;
    }

  return top;
  }
//}}}
//{{{
void cTextFile::ClearUndoLists() {

  while (m_undo_list) {
    cUndo* temp = m_undo_list;
    m_undo_list = m_undo_list->prev;
    delete temp;
    }

  ClearRedoList();
  }
//}}}
//{{{
void cTextFile::ClearRedoList()
{
  while (m_redo_list) {
    cUndo* temp = m_redo_list;
    m_redo_list = m_redo_list->prev;
    delete temp;
    }
  }
//}}}

// File routines
//{{{
bool cTextFile::Reload() {

  if (fold_end) {
    while (next && (next != fold_end))
      delete next;
    unsigned char was_open = opened;

    ReadFile();
    opened = was_open;
    return true;
    }

  return false;
  }
//}}}
//{{{
int cTextFile::ReadFile() {

  if (!pathname || !*pathname)
    return -1;

  ClearUndoLists();
  has_tabs = false;
  newfile = false;
  empty = false;
  modified = false;
  SetEdited (false);
  SetContainsEdits (false);

  HiddenLines = 0;
  opened = (ubyte) false; //true;

  SetupLanguage();   // setup language according to filename extension

  int tablen = Tablen (cLanguage::InIndent);
  int linecount = 0;

  // read file using class CFile
  TRY {
    CStdioFile fp (pathname, CFile::modeRead | CFile::typeText | CFile::shareDenyNone);
      //{{{  read file
      char line [cMaxLineLen];

      bool longline = false;
      cTextLine* cur = this;
      while (fp.ReadString (line, cMaxLineLen) ) {
        char* str = line;

        // handles files which have just 'cr' and not 'lf'
        char* cr = strchr (str, '\r');
        while (cr) {
          *cr = 0;
          if (longline) {
            longline = false;
            cur->append_text (str, 0, !longline);
            }
          else {
            cur = new cTextLine (this, cur);
            cur->set_text (str, !longline, tablen);
            cur->SetLinenum (++linecount);
            if (*str == '\t')
              has_tabs = true;
            }
          str = cr + 1;
          cr = strchr (str, '\r');
          }

        char* lf = strchr (str, '\n');
        if (lf) *lf = 0;

        if (longline) {
          longline = (bool) (!lf);
          cur->append_text (str, 0, !longline);
          }
        else {
          longline = (bool) (!lf);
          cur = new cTextLine (this, cur);
          cur->set_text (str, !longline, tablen);
          cur->SetLinenum (++linecount);
          if (*str == '\t')
            has_tabs = true;
          }
        }
      //}}}
    }
  CATCH (CFileException, e) {
    CString msg;
    if (e->m_cause == CFileException::fileNotFound)
      msg.Format ("File '%s' not found", pathname);
    else if (e->m_cause == CFileException::accessDenied)
      msg.Format ("File '%s' access denied", pathname);
    else if (e->m_cause == CFileException::sharingViolation)
      msg.Format ("File '%s' sharing violation", pathname);
    else
      msg.Format ("Unable to open file '%s' for reading", pathname);
    AfxMessageBox (msg);
    return -1;
    }
  END_CATCH

  CFileStatus status;
  if (CFile::GetStatus (pathname, status)) {
    if (status.m_attribute & CFile::readOnly)
      write_protect = true;
    else
      write_protect = false;
    m_filetime = status.m_mtime;
    }

  HiddenLines = linecount;

  return linecount;
  }
//}}}
//{{{
int cTextFile::WriteFile() {

  if (!pathname || !*pathname || !fold_end || write_protect || empty)
    return 0;

  ClearUndoLists();
  if (cOption::CreateBackupFirstSave && !backup_done) {
    //{{{  create a primary .~ext backup file
    char* backupname = new char [strlen (pathname) + 4];
    if (backupname) {
      strcpy (backupname, pathname);
      char* ext = strrchr (backupname, '.');
      if (ext) {
        ext++;
        int len = (int)strlen(ext);
        //if (len < 3)
          memmove (ext+1, ext, len+1);
        *ext = '~';
        }
      else
        strcat (backupname, ".~");

      ::DeleteFile (backupname);
      ::MoveFile (pathname, backupname);
      delete[] backupname;
      }
    //}}}
    backup_done = true;
    }
  else if (cOption::CreateBackupEverySave) {
    //{{{  create a .~ext~ backup file
    char* backupname = new char [strlen (pathname) + 4];
    if (backupname) {
      strcpy (backupname, pathname);
      char* ext = strrchr (backupname, '.');
      if (ext) {
        ext++;
        int len = (int)strlen(ext);
        memmove (ext+1, ext, len+1);
        *ext = '~';
        strcat (ext, "~");
        }
      else
        strcat (backupname, ".~~");

      ::DeleteFile (backupname);
      ::MoveFile (pathname, backupname);
      delete[] backupname;
      }
    //}}}
    }

  FILE* fp = fopen (pathname, "w");
  //{{{  check ok
  if (!fp) {
    CString msg;
    msg.Format ("Unable to open file '%s' for writing", pathname);
    AfxMessageBox (msg);
    return 0;
    }
  //}}}
  int tablen = Tablen (cLanguage::InIndent);
  const char* comment_start = language ? language->m_commentStart1 : 0;
  const char* comment_end = language ? language->m_commentEnd1 : 0;
  char openmark [4] = "{{{";
  char closemark [4] = "}}}";
  if (comment_start && strchr(comment_start, '{')) {
    strcpy (openmark, "<<<");
    strcpy (closemark, ">>>");
    }

  int linecount = 0;
  int cur_indent = 0;
  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    cur->delete_original();
    cur->SetLinenum (++linecount);
    if (cur->isFoldLine() || cur->Text()) {
      //{{{  print the indent
      int indent = cur_indent + cur->Indent();
      if (indent > 0) {
        if (has_tabs && (tablen > 1)) {
          while (indent >= tablen) {
            fprintf (fp, "\t");
            indent -= tablen;
            }
          }
        if (indent > 0)
          fprintf (fp, "%*s", indent, "");
        }
      //}}}
      if (cur->isFold() || cur->isFoldEnd()) {
        if (comment_start)
          fprintf (fp, "%s", comment_start);
        if (cur->isFold()) {
          //{{{  print fold open mark
          fprintf (fp, openmark);
          cur_indent += cur->Indent();
          //}}}
          cur->isFold()->SetContainsEdits (false);
          }
        else {
          //{{{  print fold close mark
          fprintf (fp, closemark);
          if (cur->ParentFold()) {
            cur_indent -= cur->ParentFold()->Indent();
            if (cur_indent < 0)
              cur_indent = 0;
            }
          //}}}
          }
        if (cur->Text())
          fprintf (fp, "  %s", cur->Text());
        if (comment_end)
          fprintf (fp, "%s", comment_end);
        }
      else if (cur->Text())
        fprintf (fp, "%s", cur->Text());
      }
    fprintf (fp, "\n");
    cur = cur->Next();
    }
  fflush (fp);

  CStdioFile file (fp);
  CFileStatus status;
  if (file.GetStatus (status))
    m_filetime = status.m_mtime;

  fclose (fp);

  line_count = linecount;

  newfile = false;
  empty = false;
  modified = false;
  return 1;
  }
//}}}
//{{{
int cTextFile::OpenFile (const char* path) {

  SetFilename (path);
  return ReadFile();
  }
//}}}

//{{{
void cTextFile::SetupLanguage (cLanguage* changed_language) {
//  setup language according to filename extension

  char* name = new char [strlen (text) + 1];
  strcpy (name, text);

  char* s  = strstr (name, "@@");    // clearcase marker
  if (s) *s = 0;

  s = strrchr (name, '\\');     // find last directory mark
  if (!s) s = name;

  char* ext= strchr (s, '.');   // search forward to allow for fred.cpp.keep
  if (ext) {
    ext++;
    if (*ext == '~')   // allow backup files to be recognised by the language
      ext++;

    s = strpbrk (ext, ".,@;~");   // eg fred.~cpp~
    if (s) *s = 0;                //    fred.cpp.keep
                                  //    fred.cpp,v
                                  //    fred.cpp@@\main\4   clearcase

    cLanguage* lang = cLanguage::FindExtension (ext);
    if (lang)
      language = lang;
    }
  delete[] name;

  if (changed_language && (language == changed_language))
    Format();
  }
//}}}

//{{{
void cTextFile::Foldup() {

  cTextFold::Foldup (language);
  }
//}}}
//{{{
void cTextFile::Format() {

  line_count = 0;
  cTextLine* cur = next;
  while (cur && (cur != fold_end)) {
    if (cur->isFold())
      cur->isFold()->SetCommentsOnly (true);
    cur->SetFormat(language);
    line_count++;
    cur = cur->Next();
    }
  }
//}}}
