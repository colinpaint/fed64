// FedDoc.cpp
//{{{  includes
#include "pch.h"
#include "FedDoc.h"

#include "Fed.h"
#include "FedView.h"
#include "textProj.h"
#include "Option.h"
//}}}
cFedDoc* cFedDoc::m_Document = 0;
cTextProj* cFedDoc::m_Buffers = 0;

//{{{
cFedDoc::cFedDoc()
  : m_newCount (1),
    m_activeViews (0),
    m_moveLine (NULL),
    m_deletedLines (NULL),
    m_pasteFold (NULL),
    m_createFold (NULL)
{

  m_project = new cTextProj();
  new cTextFoldEnd (m_project, m_project);
  m_Document = this;
}
//}}}
//{{{
cFedDoc::~cFedDoc()
{
  if (m_project)
    delete m_project;
  if (m_moveLine)
    delete m_moveLine;
  if (m_deletedLines)
    delete m_deletedLines;
  if (m_pasteFold)
    delete m_pasteFold;
  if (m_Buffers)
    delete m_Buffers;
  m_Buffers = 0;
}
//}}}

IMPLEMENT_DYNCREATE(cFedDoc, CDocument)
//{{{
BEGIN_MESSAGE_MAP(cFedDoc, CDocument)
END_MESSAGE_MAP()
//}}}

//{{{
void cFedDoc::GetLongPathName (const char* ShortPathName, char* LongPathName) {

  WIN32_FIND_DATA finddata;
  char path [_MAX_PATH];
  ::GetFullPathName (ShortPathName, _MAX_PATH, path, 0);
  strcpy (LongPathName, path);

  char* src = path;
  if ((path[0] == '\\') && (path[1] = '\\'))
    src = strchr (path+2, '\\');    // UNC path - skip past computer name
  if (src)
    src = strchr (src+1, '\\');     // skip past disk name
  char* dst = LongPathName + (src - path);
  if (src)
    src = strchr (src+1, '\\');     // skip past next dirname

  while (src) {
    *src = 0;
    FindFirstFile (path, &finddata);     // convert a DOS shortfilename to a WIN32 longfilename
    *src = '\\';
    *dst++ = '\\';
    strcpy (dst, finddata.cFileName);
    dst = strchr (dst, 0);
    src = strchr (src+1, '\\');     // skip past next dirname
    }

  FindFirstFile (path, &finddata);     // convert a DOS shortfilename to a WIN32 longfilename
  *dst++ = '\\';
  strcpy (dst, finddata.cFileName);
  }
//}}}

//{{{
void cFedDoc::AddDocument(LPCTSTR lpszPathName, bool switchTo) {

  char filename [_MAX_PATH];
  GetLongPathName (lpszPathName, filename);

  if (m_Document) {
    m_Document->ForceNewProject();

    cTextFile* newfile = m_Document->DoFileOpen (filename);
    if (switchTo && newfile) {
      POSITION pos = m_Document->GetFirstViewPosition();
      if (pos != NULL) {
        cFedView* view = (cFedView*) m_Document->GetNextView (pos);
        if (view)
          view->ShowFile (newfile);
        }
      }
    }
  }
//}}}
//{{{
void cFedDoc::FontChanged() {

  if (m_Document)
    m_Document->UpdateAllViews (NULL, cRange::eFontChanged, NULL);
  }
//}}}
//{{{
void cFedDoc::LanguageChanged (cLanguage* changed_language) {

  if (m_Document) {
    m_Document->UpdateLanguages (NULL, changed_language);
    m_Document->UpdateAllViews (NULL, 0, NULL);
    }
  }
//}}}
//{{{
void cFedDoc::UpdateViews() {

  if (m_Document)
    m_Document->UpdateAllViews (NULL, 0, NULL);
  }
//}}}
//{{{
cTextProj* cFedDoc::getTopProject() {

  return m_Document->m_project;
  }
//}}}
//{{{
cTextProj* cFedDoc::getBuffers() {

  if (!m_Buffers) {
    m_Buffers = new cTextProj (0, 0, cTextFile::eBuffers);
    new cTextFoldEnd (m_Buffers, m_Buffers);
    m_Buffers->set_text ("Buffers");
    }

  return m_Buffers;
  }
//}}}
//{{{
cTextFile* cFedDoc::getOutputBuffer() {

  cTextProj* buffers = getBuffers();

  cTextFile* cur = buffers->FirstFile();
  while (cur)
    {
    if (strcmp (cur->cTextLine::Text(), "Output Buffer") == 0)
      return cur;
    cur = cur->NextFile();
    }

  cTextFile* file = new cTextFile (buffers, buffers, cTextFile::eOutput);
  (new cTextFoldEnd (file, file))->SetEdited(false);
  file->set_text ("Output Buffer");
  file->SetCommentsOnly(false);
  file->SetEdited(false);

  return file;
  }
//}}}

//{{{
cTextFile* cFedDoc::GetInitialFile()
{
  if (m_project && (m_project->FileCount() == 1)
        && m_project->Next() && m_project->Next()->isFile())
    return m_project->Next()->isFile();

  return m_project;
}
//}}}
//{{{
int cFedDoc::ObtainView() {
// returns the next available view as a bit mask

  int view = 1;
  while (m_activeViews & view)
    view <<= 1;
  m_activeViews |= view;

  return view;
  }
//}}}
//{{{
void cFedDoc::ReleaseView (int view) {

  m_activeViews &= ~view;
  }
//}}}
//{{{
bool cFedDoc::AnyFilesModified (cTextFile* except_file) {
// returns true if any files have been modified

  cTextFile* file = m_project->FirstFile();
  while (file) {
    if (file->isProj())
      file = file->isProj()->FirstFile();
    else if (file->isModified() && (file != except_file))
      return true;
    else if (file->NextFile())
      file = file->NextFile();
    else {
      // end of project
      while (file->ParentFold() && file->ParentFold()->isFile() && !file->ParentFold()->isFile()->NextFile())
        file = file->ParentFold()->isFile();  // skip up a level if parent project is finished
      if (file->ParentFold() && file->ParentFold()->isFile()) // && (file->ParentFold() != m_project))
        file = file->ParentFold()->isFile()->NextFile();  // continue proj above
      else
        return false;
      }
    }

  return false;
  }
//}}}

//{{{
void cFedDoc::ForceNewProject() {

  ASSERT (m_project);
  if (!m_project->isNewFile())
    {
    cTextProj* proj = m_project;
    m_project = new cTextProj();
    ASSERT (m_project);

    new cTextFoldEnd (m_project, m_project);
    m_project->InsertAfter (proj);
    m_project->SetModified();
    }
  }
//}}}
BOOL cFedDoc::OnNewDocument() { return TRUE; }
//{{{
BOOL cFedDoc::OnOpenDocument (LPCTSTR lpszPathName) {

  ASSERT (m_project);
  char filename [_MAX_PATH];
  GetLongPathName (lpszPathName, filename);

  if (cTextProj::isProjectExt (filename)) {
    m_project->SetFilename (filename);
    if (m_project->ReadFile() <= 0)
      return FALSE;
    }

  else {
    cTextFile* newfile = m_project->AddFile (filename);
    if (newfile->ReadFile() < 0) {
      delete newfile;
      return FALSE;
      }

    newfile->Foldup();
    newfile->Format();
    if (cFedApp::ReadOnly)
      newfile->SetProtect (true);
    }

  return TRUE;
  }
//}}}

//{{{
void cFedDoc::DeleteContents() {

  CWaitCursor waitcursor;
  }
//}}}

//{{{
cTextFile* cFedDoc::DoFileNew (const char* ext, cTextFile* after) {

  CWaitCursor waitcursor;
  ASSERT (m_project);
  if (ext && (*ext == '.'))
    ext++;

  cTextProj* project = m_project;
  if (after && after->isProj())
    project = after->isProj();
  else if (after && after->ParentFold() && after->ParentFold()->isProj())
    project = after->ParentFold()->isProj();
  else
    after = 0;

  char filename [_MAX_PATH];
  char* name = filename;
  if (project->Pathname()) {
    strcpy (name, project->Pathname());
    char* s = strrchr (name, '\\');
    if (s)
      name = s + 1;
    }
  sprintf (name, "new%d.%s", m_newCount++, ext ? ext : "txt");

  cTextFile* newfile = project->AddFile (filename, after);
  project->SetModified();

  if (newfile)
    newfile->SetNotEmpty();

  UpdateAllViews (NULL, 0, NULL);

  return newfile;
  }
//}}}
//{{{
cTextFile* cFedDoc::DoFileOpen (const char* filename, cTextFile* after, int view) {

  CWaitCursor waitcursor;
  cTextFile* newfile = m_project->FindFile (filename);

  if (!newfile) {
    cTextProj* project = m_project;
    if (after) {
      if (after->isProj() && after->isOpen(view))
        project = after->isProj();
      else if (after->ParentFold() && after->ParentFold()->isProj())
        project = after->ParentFold()->isProj();
      else
        after = 0;
      }

    CString str;
    str.Format ("Fed - [%s...]", filename);
    AfxGetMainWnd()->SetWindowText (str);

    ASSERT (project);
    newfile = project->AddFile (filename, after);
    project->SetModified();

    if (newfile && !newfile->isProj()) {
      if (newfile->ReadFile() < 0) {
        delete newfile;
        return FALSE;
        }
      newfile->Foldup();
      newfile->Format();
      if (cFedApp::ReadOnly)
        newfile->SetProtect (true);
      }

    AfxGetMainWnd()->SetWindowText ("");
    }

  UpdateAllViews (NULL, 0, NULL);
  return newfile;
  }
//}}}
//{{{
void cFedDoc::DoFileSave (cTextFile* file) {

  CWaitCursor waitcursor;
  if (!file)
    return;

  file->WriteFile();

  UpdateAllViews (NULL, 0, NULL);
  }
//}}}
//{{{
void cFedDoc::DoFileSaveAll() {

  CWaitCursor waitcursor;
  ASSERT (m_project);

  if (FileSaveAll (m_project))
    UpdateAllViews (NULL, 0, NULL);
  }
//}}}
//{{{
void cFedDoc::DoFileSaveas (cTextFile* file, const char* filename) {

  if (file && filename && *filename) {
    CWaitCursor waitcursor;

    file->SetFilename (filename);
    file->WriteFile();

    UpdateAllViews (NULL, 0, NULL);
    }
  }
//}}}
//{{{
BOOL cFedDoc::CanCloseFrame(CFrameWnd* pFrame) {

  int unsaved_count = 0;
  if (m_project)
    unsaved_count = m_project->UnsavedCount (true);

  if (unsaved_count > 0) {
    CString prompt;
    if (unsaved_count == 1)
      prompt.Format ("%d file is modified:\n", unsaved_count);
    else
      prompt.Format ("%d files are modified:\n", unsaved_count);

    int count = 0;
    cTextFile* file = m_project->FirstFile();
    while (file && (count < 10)) {
      if (file->isProj())
        file = file->isProj()->FirstFile();
      else {
        if (file->isModified() && file->Pathname()) {
          prompt += "\n";
          if (file->Pathname())
            prompt += file->Pathname();
          count += 1;
          }
        if (file->NextFile())
          file = file->NextFile();
        else if (file->ParentFold() && (file->ParentFold() != m_project)
                 && file->ParentFold()->isFile())
          file = file->ParentFold()->isFile()->NextFile();
        else
          file = 0;
        }
      }
    if (count < unsaved_count) {
      prompt += "\n";
      prompt += "<more...>";
      }
    if (unsaved_count == 1)
      prompt += "\n\nWould you like to save it?";
    else
      prompt += "\n\nWould you like to save them?";

    switch (AfxMessageBox (prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE)) {
      case IDCANCEL:
        return FALSE;       // don't continue

      case IDYES:
        FileSaveAll(m_project);
        break;

      case IDNO:
        break;

      default:
        ASSERT(FALSE);
        break;
      }
    }

  return TRUE;    // keep going
  }
//}}}
//{{{
bool cFedDoc::RemoveFile (cTextFile* file) {

  if (!file)
    return false;

  AbortFoldCreate();

  if ((file == m_project) && (m_project->isEmpty())) {
    AfxGetMainWnd()->PostMessage (WM_CLOSE);
    return false;
    }

  if (file->isModified()) {
    CString prompt;
    AfxFormatString1 (prompt, AFX_IDP_ASK_TO_SAVE, file->cTextLine::Text());
    switch (AfxMessageBox (prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE)) {
      case IDCANCEL:
        return false;
      case IDYES:
        file->WriteFile();
        break;
      case IDNO:
        break;
      default:
        ASSERT(FALSE);
        break;
      }
    }

  cRange range (0, file);
  range.SetRange (file);
  UpdateAllViews (NULL, cRange::eWarnDeleteFold, &range);

  if (file->ParentFold() && file->ParentFold()->isProj())
    file->ParentFold()->isProj()->SetModified();

  delete file;

  UpdateAllViews (NULL, 0, NULL);
  return true;
  }
//}}}
//{{{
void cFedDoc::ReloadFile (cTextFile* file) {

  if (file && file->Reload()) {
    file->Foldup();
    file->Format();
    }
  }
//}}}
//{{{
void cFedDoc::RenumberFile (cTextFile* file) {

  if (file && file->Renumber()) {
    cRange range (0, file);
    UpdateAllViews (NULL, cRange::eRenumbered, &range);
    }
  }
//}}}
//{{{
bool cFedDoc::FindFile (const char* filename) {
// checks if 'filename' exists in the master project

  if (m_project->FindFile (filename))
    return true;

  return false;
  }
//}}}

//{{{
void cFedDoc::UpdateLanguages (cTextProj* proj, cLanguage* changed_language)
{ // requests all files to update their language pointer

  cTextFile* file = NULL;
  if (proj)
    file = proj->FirstFile();
  else if (m_project)
    file = m_project->FirstFile();

  while (file) {
    if (file->isProj())
      UpdateLanguages (file->isProj(), changed_language);
    else
      file->SetupLanguage (changed_language);

    file = file->NextFile();
    }
  }
//}}}
//{{{
bool cFedDoc::FileSaveAll (cTextProj* proj) {

  //returns true if any files were saved
  ASSERT (proj);
  bool result = false;

  cTextFile* file = proj->FirstFile();
  while (file) {
    if (file->isProj())
      result |= FileSaveAll (file->isProj());
    else if (file->isModified()) {
      file->WriteFile();
      result = true;
      }

    file = file->NextFile();
    }

  return result;
  }
//}}}
//{{{
void cFedDoc::Serialize (CArchive& ar) {

  if (ar.IsStoring()) {
    }
  else {
    }
  }
//}}}

// cFedDoc edit requests
//{{{
int cFedDoc::DoDelete (cChange& change) {

  int updateHint = -1;
  if (!change.isSinglePoint())
  if (change.isEditable() && !change.Line()->isFile()) {
    if (change.isSingleLine())
      //{{{  delete characters within a single line
      {
      int indent = change.Line()->Indent();
      int xpos = change.Xpos() - change.Line()->GetFoldIndent (change.Fold());
      if (change.Line() == change.Fold())
        xpos += indent;

      change.Line()->DeleteText (change.View(), xpos, change.Xlen());

      if (change.isOpenFold() && (change.Line()->Indent() != indent))
        change.ExtendToFoldEnd();

      change.File()->SetModified();

      if (change.isSingleLine() && !change.Reformat())
        updateHint = cRange::eChangedLine;
      else
        updateHint = 0;
      }
      //}}}
    else if (change.Column())
      //{{{  delete column from the lines in change
      {
      cTextLine* cur = change.FirstLine();
      while (cur) {
        int indent = cur->Indent();
        int xpos = change.Xpos() - cur->GetFoldIndent (change.Fold());
        if (cur == change.Fold())
          xpos += indent;

        cur->DeleteText (change.View(), xpos, change.Xlen());

        if (cur == change.LastLine())
          break;
        cur = cur->NextLine (change.View());
        }

      change.File()->SetModified();
      if (!change.Reformat())
        updateHint = cRange::eChanged;
      else
        updateHint = 0;
      }
      //}}}
    else
      //{{{  delete over multiple lines
      {
      int oldfmt = change.LastLine()->Outfmt();
      //{{{  first delete the whole lines in change
      cTextLine* cur = change.FirstLine();
      if (change.FirstXpos() > 0)   // skip first line as not a whole line
        cur = cur->Next();
      cRange delrange (change);
      while (cur && (cur != change.LastLine()))
        {
        if ((cur == change.LastLine()->ParentFold()) || (cur->isFoldEnd()))
          {
          // if the change ends inside this fold
          // or if the change started inside a fold of which cur is the foldend
          //  - just delete the fold text
          cur->delete_to_eol (cur->Indent());
          cur = cur->Next();
          }
        else
          {
          cTextLine* temp = cur;
          if (cur == m_createFold)
            m_createFold = NULL;

          if (cur->isFold() && cur->isFold()->FoldEnd())
            {
            //{{{  check if we are deleting m_creatfold within this fold
            for (cTextFold* fold = m_createFold; fold; fold = fold->ParentFold())
              {
              if (fold == cur)
                m_createFold = NULL;
              }
            //}}}
            delrange.SetRange (cur);
            UpdateAllViews (NULL, cRange::eWarnDeleteFold, &delrange);
            cur = cur->isFold()->FoldEnd()->Next();
            }
          else
            cur = cur->Next();
          temp->Unlink(); // calls prev->SetDeletedBelow()
          delete temp;
          }
        }
      //}}}

      if (change.LastXpos() > 0)
        //{{{  delete start of last line
        {
        int xpos = change.LastXpos() - change.LastLine()->GetFoldIndent (change.Fold());
        change.LastLine()->DeleteText (change.View(), 0, xpos);
        }
        //}}}

      cur = change.LastLine();
      if (change.FirstXpos() > 0)
        //{{{  delete to the end of the first line and join to the last line
        {
        int xpos = change.FirstXpos() - change.FirstLine()->GetFoldIndent (change.Fold());
        change.FirstLine()->DeleteText (change.View(), xpos, 9999);

        if (change.FirstLine()->JoinLine (change.View(), 1))
          cur = change.FirstLine();
        else
          change.FirstLine()->SetFormat (change.File()->Language());
        }
        //}}}

      change.SetRange(change.Pos()); // adjust range to a point 'cos it is deleted

      cur->SetFormat (change.File()->Language());
      change.File()->SetModified();
      int newfmt = cur->Outfmt();

      if (newfmt != oldfmt)
        {
        cur = cur->Next();
        //{{{  re-format following lines
        cTextLine* last = change.File()->FoldEnd();
        while (cur && (cur != last) && (newfmt != oldfmt))
          {
          oldfmt = cur->Outfmt();
          cur->SetFormat (change.File()->Language());
          newfmt = cur->Outfmt();
          cur = cur->Next();
          }
        //}}}
        }
      updateHint = 0;
      }
      //}}}
    }

  return updateHint;
  }
//}}}
//{{{
int cFedDoc::DoInsert (cChange& change) {

  int updateHint = -1;
  if (change.Text() && *change.Text())
  if (change.isEditable() && !change.Line()->isFile()) {
    const char* pData = change.Text();
    int len = (int)strlen(pData);

    const char* nl = strchr (pData, '\n');
    if (change.Column())
      //{{{  multiple line column insert
      {
      int width = 0;
      int height = 0;
      const char* src = pData;
      //{{{  scan for max column width
      while (true)
        {
        int len;
        if (!nl)
          len = (int)strlen(src);
        else
          {
          len = (int)(nl - src);
          if ((nl > src) && (*(nl-1) == '\r'))
            len -= 1;
          }
        if (len > width)
          width = len;
        height += 1;

        if (!nl)
          break;
        src = nl + 1;
        nl = strchr (src, '\n');
        }
      //}}}
      if (!change.isSinglePoint())
        height = change.Ylen();

      nl = strchr (pData, '\n');
      src = pData;
      int y = 0;
      cTextLine* line = change.Line();
      while (line && !line->isFileLine() && (y++ < height))
        //{{{  insert a line
        {
        int indent = line->Indent();
        int xpos = change.Xpos() - line->GetFoldIndent (change.Fold());
        if (line == change.Fold())
          xpos += line->Indent();

        int len = 0;
        if (nl && src)
          {
          len = (int)(nl - src);
          if ((nl > src) && (*(nl-1) == '\r'))
            len -= 1;
          }
        else if (src)
          len = (int)strlen(src);

        if (len > 0)
          line->InsertText (change.View(), xpos, src, len);
        if (len < width)
          line->InsertText (change.View(), xpos+len, ' ', width - len);

        //if (line == change.LastLine())
          //break;
        line = line->NextLine (change.View());
        if (nl)
          {
          src = nl + 1;
          nl = strchr (src, '\n');
          }
        else
          src = 0;
        }
        //}}}

      // change range to inserted range
      change.SetRange (change.StartPos(), CPoint (change.StartPos().x + width, change.StartPos().y + height - 1));

      //if (change.isOpenFold() && (change.Line()->Indent() != indent))
        //change.ExtendToFoldEnd();

      change.File()->SetModified();
      updateHint = 0;
      }
      //}}}
    else if ((nl && (nl < pData + len)) || change.Line()->isFileLine())
      //{{{  multiple line insert
      {
      // create a temporary holding-fold for the paste data
      cTextFold* fold = new cTextFold (0, 0);
      cTextFoldEnd* foldend = new cTextFoldEnd (fold, fold);
      if (fold)
        {
        fold->InsertText (pData, len);
        fold->Foldup (change.Language());
        if (change.Line() == change.Fold())
          change.Line()->InsertAfter (fold);  // can't insert above topFold
        else
          change.Line()->InsertBefore (fold);
        fold->Format (change.Language());
        fold->RemoveFold (change.Language());

        change.File()->SetModified();
        updateHint = 0;
        }
      }
      //}}}
    else if (!change.Line()->isFileLine())
      //{{{  insert within a single line
      {
      int indent = change.Line()->Indent();
      int xpos = change.Xpos() - change.Line()->GetFoldIndent (change.Fold());
      if (change.Line() == change.Fold())
        xpos += change.Line()->Indent();
      change.Line()->InsertText (change.View(), xpos, pData, len);

      if (change.isOpenFold() && (change.Line()->Indent() != indent))
        change.ExtendToFoldEnd();

      change.File()->SetModified();
      if (change.isSingleLine() && !change.Reformat())
        updateHint = cRange::eChangedLine;
      else
        updateHint = 0;
      }
      //}}}
    }

  return updateHint; // for optional cursor movement
  }
//}}}
//{{{
int cFedDoc::Edit (cChange& change, bool makeUndo) {

  if (!change.isEditable() || change.Line()->isFile())
    return 0;
  if (change.isSinglePoint() && (!change.Text() || (*change.Text() == 0)))
    return 0;

  if (makeUndo && cOption::EnableUndo) {
    cUndo* undo = new cUndo (change);
    change.File()->AddUndo (undo);
    }

  int len = change.InsertLength();
  int deleteUpdateHint = DoDelete (change);
  int insertUpdateHint = DoInsert (change);
  change.Reformat();

  if ((deleteUpdateHint == 0) || (insertUpdateHint == 0))
    UpdateAllViews (NULL, 0, NULL);
  else if (deleteUpdateHint > 0)
    UpdateAllViews (NULL, deleteUpdateHint, &change);
  else if (insertUpdateHint > 0)
    UpdateAllViews (NULL, insertUpdateHint, &change);

  return len; // for optional cursor movement
  }
//}}}
//{{{
int cFedDoc::InsertText (cRange& range, const char* pData, int len, bool makeUndo) {

  if (pData && *pData && range.isEditable() && !range.Line()->isFile()) {
    if (len <= 0)
      len = (int)strlen(pData);

    const char* nl = strchr (pData, '\n');
    if ((nl && (nl < pData + len)) || range.Line()->isFileLine()) {
      // create a temporary holding-fold for the paste data
      cTextFold* fold = new cTextFold (0, 0);
      cTextFoldEnd* foldend = new cTextFoldEnd (fold, fold);
      if (fold) {
        fold->InsertText (pData, len);
        fold->Foldup (range.Language());
        if (range.Line() == range.Fold())
          range.Line()->InsertAfter (fold);  // can't insert above topFold
        else
          range.Line()->InsertBefore (fold);
        fold->Format (range.Language());
        fold->RemoveFold (range.Language());

        range.File()->SetModified();
        UpdateAllViews (NULL, 0, NULL);
        }
      }
    else if (!range.Line()->isFileLine()) {
      int indent = range.Line()->Indent();
      int xpos = range.Xpos() - range.Line()->GetFoldIndent (range.Fold());
      if (range.Line() == range.Fold())
        xpos += range.Line()->Indent();
      range.Line()->InsertText (range.View(), xpos, pData, len);

      if (makeUndo && cOption::EnableUndo) {
        cUndo* undo = new cUndo (range);
        ASSERT (undo);
        undo->SetRange (range.Pos(), CPoint (range.Xpos() + len, range.Ypos()));
        range.File()->AddUndo (undo);
        }

      if (range.isOpenFold() && (range.Line()->Indent() != indent))
        range.ExtendToFoldEnd();

      range.File()->SetModified();
      if (range.Reformat() || !range.isSingleLine())
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChangedLine, &range);
      return len;      // for optional cursor moveent
      }
    }

  return 0;
  }
//}}}
//{{{
void cFedDoc::InsertChar (cRange& range, char nChar, int repeat, bool makeUndo) {

  if (range.isEditable() && !range.Line()->isFileLine()) {
    int indent = range.Line()->Indent();
    int xpos = range.Xpos() - range.Line()->GetFoldIndent (range.Fold());
    if (range.Line() == range.Fold())
      xpos += indent;

    for (int n = 0; n < repeat; n++)
      range.Line()->InsertText (range.View(), xpos, nChar);

    if (range.isOpenFold() && (range.Line()->Indent() != indent))
      range.ExtendToFoldEnd();

    if (makeUndo && cOption::EnableUndo) {
      cUndo* undo = new cUndo (range);
      ASSERT (undo);
      undo->SetRange (range.Pos(), CPoint (range.Xpos() + 1, range.Ypos()));
      range.File()->AddUndo (undo);
      }

    range.File()->SetModified();
    if (range.Reformat() || !range.isSingleLine())
      UpdateAllViews (NULL, 0, NULL);
    else
      UpdateAllViews (NULL, cRange::eChangedLine, &range);
    }
  }
//}}}
//{{{
void cFedDoc::ReplaceChar (cRange& range, char nChar, int repeat, bool makeUndo) {


  if (range.isEditable() && !range.Line()->isFileLine()) {
    int indent = range.Line()->Indent();
    int xpos = range.Xpos() - range.Line()->GetFoldIndent (range.Fold());
    if (range.Line() == range.Fold())
      xpos += indent;

    for (int n = 0; n < repeat; n++)
      range.Line()->ReplaceText (range.View(), xpos, nChar);

    if (range.isOpenFold() && (range.Line()->Indent() != indent))
      range.ExtendToFoldEnd();

    range.File()->SetModified();
    if (range.Reformat() || !range.isSingleLine())
      UpdateAllViews (NULL, 0, NULL);
    else
      UpdateAllViews (NULL, cRange::eChangedLine, &range);
    }

  }
//}}}
//{{{
void cFedDoc::DeleteLine (cRange& range) {

  if (range.isEditable() && !range.Line()->isFileLine() && !range.Line()->isFoldEnd()) {
    if (range.Line()->isFold() && !range.Line()->isFold()->isOpen(range.View())) {
      //{{{  check if we are deleting m_creatfold within this fold
      for (cTextFold* fold = m_createFold; fold; fold = fold->ParentFold()) {
        if (fold == range.Line())
          m_createFold = NULL;
        }
      //}}}
      UpdateAllViews (NULL, cRange::eWarnDeleteFold, &range);
      }
    cTextLine* nextline = range.Line()->NextLine (range.View());
    cTextLine* line = range.Line()->Extract();
    if (m_deletedLines)
      m_deletedLines->InsertBefore (line);
    m_deletedLines = line;
    range.SetRange (nextline);

    range.File()->SetModified();
    range.Reformat();
    UpdateAllViews (NULL, cRange::eDeleteLine, &range);
    }
  }
//}}}
//{{{
cTextLine* cFedDoc::Copy (cRange& range) {

  cTextLine* result = 0;
  cTextLine* cur = range.FirstLine();

  if (range.isSingleLine()) {
    result = cur->Copy (range.Xpos(), range.Xlen());
    result->change_indent (cur->GetFoldIndent (range.Fold()));
    }
  else {
    cTextLine* lastline = range.LastLine();
    int fold_indent = cur->GetFoldIndent (range.Fold());
    result = cur->Copy (range.Xpos(), range.Xlen());
    result->change_indent (fold_indent);
    cur = cur->NextLine (range.View());
    cTextLine* prevline = cur;
    cTextLine* newline = 0;

    while (cur && (cur != lastline)) {
      if (range.Column())
        newline = cur->Copy (range.Xpos(), range.Xlen());
      else
        newline = cur->Copy();   // copy the whole line
      newline->change_indent (fold_indent);

      prevline = prevline->InsertAfter (newline);

      cur = cur->NextLine (range.View());
      }
    }

  return result;
  }
//}}}
//{{{
void cFedDoc::Delete  (cRange& range, bool makeUndo) {

  Edit (cChange (range), makeUndo);
  }
//}}}
//{{{
void cFedDoc::ToUpper (cRange& range) {

  if (range.isEditable()) {
    range.File()->SetModified();

    if (range.isSingleLine()) {
      int xpos = range.Xpos() - range.Line()->GetFoldIndent (range.Fold());
      if (range.Line() == range.Fold())
        xpos += range.Line()->Indent();

      range.Line()->ToUpper (range.View(), xpos, range.Xlen());
      if (Reformat (range))
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChangedLine, &range);
      }
    else {
      cTextLine* cur = range.FirstLine();
      int xpos = range.FirstXpos() - cur->GetFoldIndent (range.Fold());
      if (cur == range.Fold())
        xpos += cur->Indent();
      cur->ToUpper (range.View(), xpos, 9999);
      cur = cur->Next();

      while (cur && (cur != range.LastLine())) {
        cur->ToUpper (range.View(), 0, 9999);
        cur = cur->Next();
        }

      if ((cur == range.LastLine()) && (range.LastXpos() > 0)) {
        xpos = range.LastXpos() - cur->GetFoldIndent (range.Fold());
        cur->ToUpper (range.View(), 0, xpos);
        }

      if (Reformat (range))
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChanged, &range);
      }
    }
  }
//}}}
//{{{
void cFedDoc::ToLower (cRange& range) {

  if (range.isEditable()) {
    range.File()->SetModified();

    if (range.isSingleLine()) {
      int xpos = range.Xpos() - range.Line()->GetFoldIndent (range.Fold());
      if (range.Line() == range.Fold())
        xpos += range.Line()->Indent();

      range.Line()->ToLower (range.View(), xpos, range.Xlen());
      if (Reformat (range))
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChangedLine, &range);
      }
    else {
      cTextLine* cur = range.FirstLine();
      int xpos = range.FirstXpos() - cur->GetFoldIndent (range.Fold());
      if (cur == range.Fold())
        xpos += cur->Indent();
      cur->ToLower (range.View(), xpos, 9999);
      cur = cur->Next();

      while (cur && (cur != range.LastLine())) {
        cur->ToLower (range.View(), 0, 9999);
        cur = cur->Next();
        }

      if ((cur == range.LastLine()) && (range.LastXpos() > 0)) {
        xpos = range.LastXpos() - cur->GetFoldIndent (range.Fold());
        cur->ToLower (range.View(), 0, xpos);
        }

      if (Reformat (range))
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChanged, &range);
      }
    }
  }
//}}}

//{{{
void cFedDoc::AlignNextColumn (cRange& range) {

  if (range.isEditable() && !range.isSingleLine() && range.FirstLine()) {
    cTextLine* cur = range.FirstLine();
    //{{{  check that all visible lines are in the same fold
    cTextFold* fold = cur->ParentFold();
    if (cur->isFold() && cur->isFold()->isOpen(range.View()))
      fold = cur->isFold();
    while (cur) {
      if (cur->ParentFold() != fold) return;
      if (cur == range.LastLine()) break;
      cur = cur->NextLine(range.View());
      }
    //}}}

    int fold_indent = range.FirstLine()->GetFoldIndent (range.Fold());
    int column_to_align = 0;
    int align_pos = 0;
    //{{{  find the first unaligned column and its best align position
    bool column_is_aligned = true;
    while (column_is_aligned) {
      column_to_align += 1;
      align_pos = -1;
      cTextLine* cur = range.FirstLine();
      while (cur) {
        const char* text = cur->GetText (range.View());
        if (text) {
          int column = 0;
          const char* s = text;
          while (*s && (column++ < column_to_align)) {
            //{{{  skip past spaces
            while (*s && (*s <= ' '))
              s++;
            //}}}
            //{{{  skip past current column (non-space characters)
            while (*s && (*s > ' '))
              s++;
            //}}}
            }
          int indent = cur->Indent() + fold_indent;
          if (cur->isFold()) indent += 5;
          int column_xmin = indent + (int) (s - text) + 1;  // minimum position for the column
          //{{{  skip past spaces
          while (*s && (*s <= ' '))
            s++;
          //}}}
          if (*s) {  // then the column exists on this line
            int column_x = indent + (int) (s - text);  // column position in this line
            if (cur == range.FirstLine())
              align_pos = column_x;   // use first line as column reference
            else if (column_x != align_pos) {
              column_is_aligned = false;
              if (column_xmin > align_pos)
                align_pos = column_xmin;
              }
            }
          }
        if (cur == range.LastLine())
          break;
        cur = cur->NextLine(range.View());
        }
      if (align_pos < 0)
        return;  // all columns are aligned
      }
    //}}}

    range.File()->SetModified();
    //{{{  perform the column alignment
    cur = range.FirstLine();
    while (cur) {
      const char* text = cur->GetText (range.View());
      if (text) {
        int column = 0;
        const char* s = text;
        while (*s && (column++ < column_to_align)) {
          //{{{  skip past spaces
          while (*s && (*s <= ' '))
            s++;
          //}}}
          //{{{  skip past current column (non-space characters)
          while (*s && (*s > ' '))
            s++;
          //}}}
          }
        int indent = cur->Indent(); // + fold_indent;
        if (cur->isFold()) indent += 5;
        int adjust_pos = indent + (int) (s - text);
        //{{{  skip past spaces
        while (*s && (*s <= ' '))
          s++;
        //}}}
        if (*s) {  // then the column exists on this line
          int column_x = fold_indent + indent + (int) (s - text);  // column position in this line
          if (column_x < align_pos) {     // insert spaces
            for (int n = column_x; n < align_pos; n++)
              cur->InsertText (range.View(), adjust_pos, ' ');
            }
          else if (column_x > align_pos)  // delete spaces
            cur->DeleteText (range.View(), adjust_pos, column_x - align_pos);
          }
        }
      if (cur == range.LastLine())
        break;
      cur = cur->NextLine(range.View());
      }
    //}}}

    if (Reformat (range))
      UpdateAllViews (NULL, 0, NULL);
    else
      UpdateAllViews (NULL, cRange::eChanged, &range);
    }
  }
//}}}
//{{{
void cFedDoc::DealignNextColumn (cRange& range) {

  if (range.isEditable() && !range.isSingleLine() && range.FirstLine()) {
    cTextLine* cur = range.FirstLine();
    //{{{  check that all visible lines are in the same fold
    cTextFold* fold = cur->ParentFold();
    if (cur->isFold() && cur->isFold()->isOpen(range.View()))
      fold = cur->isFold();
    while (cur) {
      if (cur->ParentFold() != fold) return;
      if (cur == range.LastLine()) break;
      cur = cur->NextLine(range.View());
      }
    //}}}

    int fold_indent = range.FirstLine()->GetFoldIndent (range.Fold());
    int column_to_align = 0;
    int last_aligned_column = 0;
    int align_pos = 0;
    //{{{  find the last aligned column
    bool column_is_aligned = true;
    while (align_pos >= 0) {
      column_to_align += 1;
      align_pos = -1;
      bool can_be_unaligned = false;
      cTextLine* cur = range.FirstLine();
      while (cur) {
        const char* text = cur->GetText (range.View());
        if (text) {
          int column = 0;
          const char* s = text;
          while (*s && (column++ < column_to_align)) {
            //{{{  skip past spaces
            while (*s && (*s <= ' '))
              s++;
            //}}}
            //{{{  skip past current column (non-space characters)
            while (*s && (*s > ' '))
              s++;
            //}}}
            }
          int indent = cur->Indent() + fold_indent;
          if (cur->isFold()) indent += 5;
          int column_xmin = indent + (int) (s - text) + 1;  // minimum position for the column
          //{{{  skip past spaces
          while (*s && (*s <= ' '))
            s++;
          //}}}
          if (*s) {  // then the column exists on this line
            int column_x = indent + (int) (s - text);  // column position in this line
            if (cur == range.FirstLine())
              align_pos = column_x;   // use first line as column reference
            else if (column_x != align_pos) {
              column_is_aligned = false;
              if (column_xmin > align_pos)
                align_pos = column_xmin;
              }
            if (column_x > column_xmin)
              can_be_unaligned = true;
            }
          }
        if (cur == range.LastLine())
          break;
        cur = cur->NextLine(range.View());
        }

      if (column_is_aligned && can_be_unaligned && (align_pos >= 0))
        last_aligned_column = column_to_align;
      }
    //}}}

    if (last_aligned_column > 0) {
      range.File()->SetModified();
      //{{{  perform the column de-alignment
      cur = range.FirstLine();
      while (cur) {
        const char* text = cur->GetText (range.View());
        if (text) {
          int column = 0;
          const char* s = text;
          while (*s && (column++ < last_aligned_column)) {
            //{{{  skip past spaces
            while (*s && (*s <= ' '))
              s++;
            //}}}
            //{{{  skip past current column (non-space characters)
            while (*s && (*s > ' '))
              s++;
            //}}}
            }
          int indent = cur->Indent(); // + fold_indent;
          if (cur->isFold()) indent += 5;
          int adjust_pos = indent + (int) (s - text);
          //{{{  skip past spaces
          while (*s && (*s <= ' '))
            s++;
          //}}}
          if (*s) {  // then the column exists on this line
            int column_x = indent + (int) (s - text);  // column position in this line
            if (column_x > adjust_pos + 1)  // delete extra spaces
              cur->DeleteText (range.View(), adjust_pos, column_x - adjust_pos - 1);
            }
          }
        if (cur == range.LastLine())
          break;
        cur = cur->NextLine(range.View());
        }
      //}}}

      if (Reformat (range))
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChanged, &range);
      }
    }
  }
//}}}

//{{{
cTextLine* cFedDoc::SplitLine (cRange& range) {

  cTextLine* newline = 0;
  if (range.isEditable()) {
    if (range.Line()->isFileLine()) {
      if (range.Line()->isFoldEnd() && (range.Xpos() == 0))
        newline = new cTextLine (range.Line()->ParentFold(), range.Line()->Prev());
      else
        return 0;
      }
    else {
      cTextLine* line = range.Line();
      int xpos = range.Xpos() - line->GetFoldIndent (range.Fold());
      int indent = line->Indent();

      if (range.Line() == range.Fold())
        xpos += indent;

      if (xpos <= indent) {
        newline = new cTextLine (line->ParentFold(), line->Prev());
        if (newline)
          newline->set_indent (indent);
        }
      else {
        newline = line->SplitLine (range.View(), xpos);
        cRange modrange (range);
        modrange.SetRange (range.Pos(), CPoint (0, range.Ypos()+1));
        modrange.Reformat();
        }
      }
    range.File()->SetModified();
    UpdateAllViews (NULL, cRange::eSplitLine, &range);
    }

  return newline;
  }
//}}}
//{{{
bool cFedDoc::SwapLines (cRange& range, bool reverse) {

  if (!range.isSingleLine() && (range.isEditable() || range.FileIsProj())) {
    cTextLine* line1 = range.FirstLine();

    if ((line1->isFold() && line1->isFold()->isOpen(range.View())) || line1->isFoldEnd())
      return false;
    cTextLine* line2 = range.LastLine();

    if ((line2->isFold() && line2->isFold()->isOpen(range.View())) || line2->isFoldEnd())
      return false;

    if (reverse)
      line1->InsertBefore (line2->Unlink());
    else if (line2->isFold() && line2->isFold()->FoldEnd())
      line2->isFold()->FoldEnd()->InsertAfter (line1->Unlink());
    else
      line2->InsertAfter (line1->Unlink());

    range.File()->SetModified();
    UpdateAllViews (NULL, cRange::eChanged, &range);
    return true;
    }

  return false;
  }
//}}}
//{{{
void cFedDoc::PerformUndo (cTextFile& file) {

  cUndo* undo = file.UndoList();

  if (undo && file.isEditable()) {

    DoDelete (*undo);
    DoInsert (*undo);

    if (undo->Reformat())
      UpdateAllViews (NULL, 0, NULL);
    else
      UpdateAllViews (NULL, cRange::eChanged, undo);

    file.PopUndo();
    }
  }
//}}}
//{{{
void cFedDoc::UndoLine (cRange& range) {

  if (range.isEditable() && !range.Line()->isFileLine() && range.isSingleLine()) {
    if (range.Line()->RevertToOriginal (range.View())) {
      if (range.Reformat())
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChanged, &range);
      }
    }
  }
//}}}
//{{{
void cFedDoc::Duplicate (cRange& range) {

  if (range.isEditable() && !range.Line()->isFileLine()) {
    if (range.Line()->isFold() && range.Line()->isFold()->isOpen(range.View())) {
      cTextLine* newline = range.Line()->cTextLine::Duplicate();
      newline->change_indent (- range.Line()->Indent());
      range.SetRange (newline);
      }
    else {
      cTextLine* newline = range.Line()->Duplicate();
      if (newline->isFold())
        range.SetRange (newline, newline->isFold()->FoldEnd());
      else
        range.SetRange (newline);
      }

    range.File()->SetModified();
    if (range.Reformat())
      UpdateAllViews (NULL, 0, NULL);
    else
      UpdateAllViews (NULL, cRange::eChanged, &range);
    }
  }
//}}}
//{{{
void cFedDoc::MoveLine (cRange& range) {

  if (range.isEditable()) {
    if (m_moveLine) {
      int fold_indent = range.Line()->GetFoldIndent (range.File());
      if (range.Line() == range.Fold()) {
        // remove this fold indent to line before insertion
        fold_indent += range.Line()->Indent();
        m_moveLine->change_indent (-fold_indent);
        range.Line()->InsertAfter (m_moveLine);
        }
      else {
        // remove current fold indent to line before insertion
        m_moveLine->change_indent (-fold_indent);
        range.Line()->InsertBefore (m_moveLine);
        }
      if (m_moveLine->isFold())
        range.SetRange (m_moveLine, m_moveLine->isFold()->FoldEnd());
      else
        range.SetRange (m_moveLine);
      m_moveLine = NULL;
      }
    else if (!range.Line()->isFileLine() && !range.Line()->isFoldEnd()) {
      if (range.Line()->isFold() && !range.Line()->isFold()->isOpen(range.View()))
        UpdateAllViews (NULL, cRange::eWarnDeleteFold, &range);

      int fold_indent = range.Line()->GetFoldIndent (range.File());
      cTextLine* nextline = range.Line()->NextLine (range.View());
      m_moveLine = range.Line()->Extract();
      if (m_moveLine)  // add current fold indent to extracted line
        m_moveLine->change_indent (fold_indent);
      range.SetRange (nextline);
      }

    range.File()->SetModified();
    range.Reformat();
    UpdateAllViews (NULL, 0, NULL);
    }
  }
//}}}
//{{{
void cFedDoc::UndeleteLine (cRange& range) {

  if (range.isEditable() && m_deletedLines) {
    cTextLine* nextline = m_deletedLines->NextLine (-1);
    cTextLine* newline = m_deletedLines->Extract();
    m_deletedLines = nextline;
    if (range.Line() == range.Fold())
      range.Line()->InsertAfter (newline);
    else
      range.Line()->InsertBefore (newline);
    if (newline->isFold())
      range.SetRange (newline, newline->isFold()->FoldEnd());
    else
      range.SetRange (newline);

    range.File()->SetModified();
    range.Reformat();
    UpdateAllViews (NULL, 0, NULL);
    }
  }
//}}}
//{{{
void cFedDoc::CopyLine (cRange& range) {

  if (!range.Line()->isFileLine()) {
    if (!m_pasteFold) {
      m_pasteFold = new cTextFold (0, 0);
      new cTextFoldEnd (m_pasteFold, m_pasteFold);
      }

    if (m_pasteFold && m_pasteFold->FoldEnd()) {
      cTextLine* line = range.Line()->Copy();
      if (line) {
        if (range.Line() == range.Fold())
          m_pasteFold->FoldEnd()->InsertAfter (line);
        else
          m_pasteFold->FoldEnd()->InsertBefore (line);
        }
      }
    }
  }
//}}}
//{{{
void cFedDoc::CutLine (cRange& range) {

  if (range.isEditable() && !range.Line()->isFileLine() && !range.Line()->isFoldEnd()) {
    if (!m_pasteFold) {
      m_pasteFold = new cTextFold (0, 0);
      new cTextFoldEnd (m_pasteFold, m_pasteFold);
      }

    if (m_pasteFold && m_pasteFold->FoldEnd()) {
      if (range.Line()->isFold() && !range.Line()->isFold()->isOpen(range.View()))
        UpdateAllViews (NULL, cRange::eWarnDeleteFold, &range);

      cTextLine* line = range.Line()->Extract();
      if (line)
        m_pasteFold->FoldEnd()->InsertBefore (line);

      range.File()->SetModified();
      UpdateAllViews (NULL, 0, NULL);
      }
    }
  }
//}}}
//{{{
void cFedDoc::ClearPaste() {

  if (m_pasteFold) {
    delete m_pasteFold;
    m_pasteFold = 0;
    }
  }
//}}}

//{{{
int cFedDoc::KeywordComplete (cRange& range) {
// returns the matched keyword extra len
//   with the flags 0x1000  = no match found
//                  0x2000  = ambiguous match

  int result = 0x1000;
  if (range.isEditable() && !range.Line()->isFoldLine() && range.Language()) {
    int indent = range.Line()->Indent();
    int xpos = range.Xpos() - range.Line()->GetFoldIndent (range.Fold());

    int x = xpos - indent;
    const char* text = range.Line()->Text();
    int textlen = text ? (int)strlen(text) : 0;
    if (text && (x > 0) && (x <= textlen) && (text[x] <= ' ')) {
      int len = 0;
      while ((x > 0) && (text[x-1] > ' ')) {
        x -= 1;
        len += 1;
        }
      if (len > 0) {
        const char* keyword = text + x;
        int match_len = range.Language()->FindKeyWord (keyword, len);
        if (match_len > 0) {
          if (keyword[match_len] > ' ')
            result = 0x2000;
          else
            result = 0;

          if (match_len > len) {
            keyword += len;
            int insert_len = match_len - len;
            range.Line()->insert_text (xpos, keyword, insert_len);
            range.Line()->insert_text (xpos + insert_len++, ' ');
            result += insert_len;

            range.File()->SetModified();
            if (range.Reformat())
              UpdateAllViews (NULL, 0, NULL);
            else
              UpdateAllViews (NULL, cRange::eChangedLine, &range);
            }
          }
        }
      }
    }

  return result;
  }
//}}}

//{{{
cTextFold* cFedDoc::FoldCreate (cRange& range)
{

  if (range.isEditable() && !range.Line()->isFile()) {
    if (range.isSinglePoint()) {
      if (m_createFold) {
        //{{{  create a fold between m_createFold and line
        cTextLine* line = range.Line();
        cTextFold* parent = m_createFold->ParentFold();
        if (!parent || (parent != line->ParentFold()))
          return 0;        // not allowed

        cTextFold* fold = m_createFold;
        if (m_createFold->GetPosInFold (parent) >= line->GetPosInFold (parent)) {
          //{{{  create a fold between line and m_createFold
          fold = new cTextFold (parent, line->Prev());
          ASSERT (fold);
          fold->CreateFoldEnd (m_createFold);   //->Prev());
          delete m_createFold;
          fold->Open (range.View());
          //}}}
          }
        else
          fold->CreateFoldEnd (line->Prev());

        m_createFold = NULL;

        range.File()->SetModified();
        range.SetRange (fold, fold->FoldEnd());
        range.Reformat();
        UpdateAllViews (NULL, 0, NULL);
        return fold;
        //}}}
        }
      else {
        //{{{  insert a dangling fold-start
        m_createFold = new cTextFold (range.FirstLine()->ParentFold(), range.FirstLine()->Prev());
        m_createFold->Open (range.View());
        if ((range.Line()->Text() || range.Line()->isFold()) && (range.Xpos() > range.Line()->Indent()))
          m_createFold->set_indent (range.Line()->Indent());
        else
          m_createFold->set_indent (range.Xpos());

        range.File()->SetModified();
        UpdateAllViews (NULL, 0, NULL);
        return m_createFold;
        //}}}
        }
      }
    else {
      //{{{  create a fold of the selected area
      cTextFold* parent = range.FirstLine()->ParentFold();
      if (!parent || (parent != range.LastLine()->ParentFold()))
        return 0;        // not allowed

      cTextFold* newfold = new cTextFold (parent, range.FirstLine()->Prev());
      if (newfold) {
        newfold->CreateFoldEnd (range.LastLine());
        newfold->Open(range.View());
        }
      range.File()->SetModified();
      range.Reformat();
      UpdateAllViews (NULL, 0, NULL);
      return newfold;
      //}}}
      }
    }
  return 0;
}
//}}}
//{{{
void cFedDoc::FoldRemove (cRange& range)
{
  if (range.isEditable() && range.Line()->isFold()) {

    UpdateAllViews (NULL, cRange::eWarnDeleteFold, &range);

    cTextFold* fold = range.Line()->isFold();
    if (fold == m_createFold)
      m_createFold = NULL;

    fold->RemoveFold (range.Language());

    range.File()->SetModified();
    UpdateAllViews (NULL, cRange::eDeleteFold, &range);
    }
}
//}}}
//{{{
void cFedDoc::AbortFoldCreate() {

  if (m_createFold) {
    delete m_createFold;
    m_createFold = NULL;
    UpdateAllViews (NULL, 0, NULL);
    }
  }
//}}}

//{{{
void cFedDoc::MakeComment (cRange& range) {

  if (range.isEditable()) {
    cTextLine* cur = range.FirstLine();
    while (cur) {
      cur->MakeComment (range.View(), range.Language());
      if (cur == range.LastLine())
        break;
      cur = cur->NextLine (range.View());
      }

    if (range.LastLine()->isFold())
      range.SetRange (range.FirstLine(), range.LastLine()->isFold()->FoldEnd());
    range.ExtendToWholeLines();
    range.Reformat();

    range.File()->SetModified();
    UpdateAllViews (NULL, 0, NULL);
    }
  }
//}}}
//{{{
void cFedDoc::RemoveComment (cRange& range) {

  if (range.isEditable()) {
    cTextLine* cur = range.FirstLine();
    while (cur) {
      cur->RemoveComment (range.View(), range.Language());
      if (cur == range.LastLine())
        break;
      cur = cur->NextLine(range.View());
      }

    if (range.LastLine()->isFold())
      range.SetRange (range.FirstLine(), range.LastLine()->isFold()->FoldEnd());
    range.ExtendToWholeLines();
    range.Reformat();

    range.File()->SetModified();
    UpdateAllViews (NULL, 0, NULL);
    }
  }
//}}}

//{{{
bool cFedDoc::Reformat (cRange& range) {
// returns true if lines outside of range are reformatted

  bool result = false;
  if (range.Language()) {
    bool changed = false;
    cTextLine* cur  = range.FirstLine();
    cTextLine* last = range.File()->FoldEnd();
    //{{{  reformat all of range except the last line
    while (cur && (cur != last) && (cur != range.LastLine())) {
      cur->SetFormat (range.File()->Language());
      cur = cur->Next();
      }
    //}}}
    //{{{  reformat the last line, note if outfmt is changed
    if (cur && (cur != last)) {  // LastLine
      changed = cur->SetFormat (range.File()->Language());
      cur = cur->Next();
      }
    //}}}
    //{{{  reformat following lines until outfmt is not changed
    //{{{  reformat first following line in order to set result if changed
    if (changed && cur && (cur != last)) {
      changed = cur->SetFormat (range.File()->Language());
      cur = cur->Next();
      }
    //}}}
    result = changed;
    //{{{  reformat following lines until outfmt is not changed
    while (changed && cur && (cur != last)) {
      changed = cur->SetFormat (range.File()->Language());
      cur = cur->Next();
      }
    //}}}
    //}}}
    }

  return result;
  }
//}}}
//{{{
bool cFedDoc::IndentBy (cRange& range, int by) {

  if (range.isEditable()) {
    range.File()->SetModified();

    if (range.Column()) {
      if (by <0)
        //{{{  scan to check that we are only removing spaces
        {
        cTextLine* cur = range.FirstLine();
        while (cur)
          {
          int indent = cur->Indent();
          int xpos = range.Xpos() - cur->GetFoldIndent (range.Fold());
          if (cur == range.Fold())
            xpos += indent;
          for (int x = xpos+by; x < xpos; x++)
            {
            if (cur->GetChar (range.View(), x) != ' ')
              return false;
            }

          if (cur == range.LastLine())
            break;
          cur = cur->NextLine(0);      // skip over any fold (open or closed)
          }
        }
        //}}}
      cTextLine* cur = range.FirstLine();
      while (cur)
        //{{{  perform space insertion/deletion
        {
        int indent = cur->Indent();
        int xpos = range.Xpos() - cur->GetFoldIndent (range.Fold());
        if (cur == range.Fold())
          xpos += indent;

        if (by < 0)
          cur->DeleteText (range.View(), xpos+by, -by);
        else
          cur->InsertText (range.View(), xpos, ' ', by);

        //if (curisOpenFold() && (cur->Indent() != indent))
          //range.ExtendToFoldEnd();

        if (cur == range.LastLine())
          break;
        cur = cur->NextLine(0);      // skip over any fold (open or closed)
        }
        //}}}
      CPoint pos1 (range.StartPos());
      CPoint pos2 (range.EndPos());
      pos1.x += by;
      pos2.x += by;
      range.SetRange (pos1, pos2);
      UpdateAllViews (NULL, cRange::eChanged, &range);
      }

    else if (range.isSingleLine()) {
      cTextLine* line = range.EditLine();
      range.Line()->change_indent (by);

      if (range.isOpenFold())
        range.ExtendToFoldEnd();

      range.File()->SetModified();
      if (range.Reformat() || !range.isSingleLine())
        UpdateAllViews (NULL, 0, NULL);
      else
        UpdateAllViews (NULL, cRange::eChangedLine, &range);
      }

    else {
      if (range.FirstLine()->ParentFold() != range.LastLine()->ParentFold())
        return false;   // not allowed
      cTextLine* cur = range.FirstLine();
      while (cur) {
        cur->change_indent (by);
        if (cur == range.LastLine())
          break;
        cur = cur->NextLine(0);      // skip over any fold (open or closed)
        }
      UpdateAllViews (NULL, cRange::eChanged, &range);
      }
    }

  return true;
  }
//}}}
