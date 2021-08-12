#pragma once

class cRange;
class cChange;
class cTextLine;
class cTextFold;
class cTextFile;
class cTextProj;
class cLanguage;

class cFedDoc : public CDocument {
public:
  static void UpdateViews();
  static void FontChanged();
  static void LanguageChanged (cLanguage* changed_language = 0);
  static void AddDocument (LPCTSTR filename, bool switchTo = false);
  static void GetLongPathName (const char* ShortPathName, char* LongPathName);

  static cTextProj* getBuffers();
  static cTextProj* getTopProject();
  static cTextFile* getOutputBuffer();

  // Implementation
  virtual ~cFedDoc();

  // Overrides
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);
  virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
  virtual BOOL CanCloseFrame(CFrameWnd* pFrame);

  // Attributes
  cTextFile* GetInitialFile();
  int ObtainView();
  void ReleaseView (int view);
  bool AnyFilesModified (cTextFile* except_file = 0);
  cTextLine* GetMoveLine() { return m_moveLine; }
  cTextLine* GetDeletedLine() { return m_deletedLines; }

  // Operations
  cTextFile* DoFileNew (const char* ext, cTextFile* after = 0);
  cTextFile* DoFileOpen (const char* filename, cTextFile* after = 0, int view = 0);
  void DoFileSaveAll();
  void DoFileCloseAll();
  void DoFileSave (cTextFile* file);
  void DoFileSaveas (cTextFile* file, const char* filename);
  void DeleteContents();
  bool RemoveFile (cTextFile* file);
  void ReloadFile (cTextFile* file);
  void RenumberFile (cTextFile* file);
  bool FindFile (const char* filename);
  void ForceNewProject();

  int Edit (cChange& change, bool makeUndo = true);

  int InsertText (cRange& range, const char* pData, int len = 0, bool makeUndo = true);
  void InsertChar (cRange& range, char nChar, int repeat = 1, bool makeUndo = true);
  void ReplaceChar (cRange& range, char nChar, int repeat = 1, bool makeUndo = true);
  void DeleteLine (cRange& range);
  cTextLine* SplitLine (cRange& range);
  bool SwapLines (cRange& range, bool reverse = false);

  void PerformUndo (cTextFile& file);

  void UndoLine (cRange& range);
  void Duplicate (cRange& range);
  void MoveLine (cRange& range);
  void UndeleteLine (cRange& range);

  void MakeComment (cRange& range);
  void RemoveComment (cRange& range);

  void CopyLine (cRange& range);
  void CutLine (cRange& range);
  void ClearPaste();
  cTextFold* GetPasteFold() { return m_pasteFold; }
  int KeywordComplete (cRange& range);

  cTextFold* FoldCreate (cRange& range);
  void FoldRemove (cRange& range);
  void AbortFoldCreate();

  bool Reformat (cRange& range);

  cTextLine* Copy (cRange& range);
  void Delete (cRange& range, bool makeUndo = true);
  void ToUpper (cRange& range);
  void ToLower (cRange& range);
  bool IndentBy (cRange& range, int by);
  void AlignNextColumn (cRange& range);
  void DealignNextColumn (cRange& range);

protected:
  cFedDoc();

  DECLARE_DYNCREATE(cFedDoc)

  // editing
  int DoDelete (cChange& change); // returns updateHint
  int DoInsert (cChange& change); // returns updateHint

  bool FileSaveAll (cTextProj* proj);
  void UpdateLanguages (cTextProj* proj, cLanguage* changed_language);

  // Generated message map functions
  DECLARE_MESSAGE_MAP()

  static cFedDoc* m_Document;
  static cTextProj* m_Buffers;

  int m_newCount;
  int m_activeViews;
  cTextProj* m_project;
  cTextLine* m_moveLine;
  cTextLine* m_deletedLines;
  cTextFold* m_pasteFold;
  cTextFold* m_createFold;
  CString debug_msg;
  };
