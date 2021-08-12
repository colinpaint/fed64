#pragma once

#include "Range.h"
#include <string>

class cTextProj;
class cMainFrame;
class cFedDoc;
class cDebugWin;
class cMyFindReplaceDialog;

class cFedView : public CScrollView {
friend cDebugWin;
protected: // create from serialization only
  cFedView();
  DECLARE_DYNCREATE(cFedView)

  static CFont* m_font;
  static CFont* m_textFont;
  static CFont* m_boldFont;
  static CFont* m_italicFont;
  static cFedView* m_firstView;
  static cDebugWin* m_debugWin;

  CFont* Font;
  cMyFindReplaceDialog* m_findDialog;
  BOOL m_overstrike;
  BOOL m_recording;
  BOOL m_macro_textmode;
  CString m_macroStr;

  char* m_findString;
  char* m_replaceString;
  BOOL m_findOnly;
  BOOL m_matchCase;
  BOOL m_matchWholeWord;
  BOOL m_ignoreComments;
  BOOL m_findUp;
  BOOL m_searchAll;
  BOOL m_atFindString;
  BOOL m_atReplaceString;
  cTextProj* m_searchingProject;     // Safety Note: don't use as a pointer, only for comparison
  CPoint m_scrollPos;

  BOOL m_displayMatch;
  CPoint m_matchPos;

  int m_tabLen;
  CPoint m_Margin;
  int m_xStart;
  bool m_goldOn;
  bool m_mouseDown;
  cRange m_range;
  cRange m_selectRange;
  bool m_selectActive;
  bool m_selectWholeLines;
  bool m_selectColumn;
  CPoint m_selectPos;
  CPoint m_pos;
  int m_lineNumber;
  CSize m_cChar;
  CSize m_cWlen;
  CSize m_ScrollMargin;
  CSize m_CurTopLeftMargin;
  CSize m_CurBottomRightMargin;
  BOOL m_KeyWasModifier;
  BOOL m_AllowChar;
  BOOL m_ShowLineNumbers;
  BOOL m_ShowFolds;
  BOOL m_ShowChanges;
  bool m_FoldendVisible;

  int m_view;
  cTextFile* m_curFile;
  cTextFold* m_curTop;

public:
  cFedDoc* GetDocument();
  cMainFrame* GetFrame();

  // Operations
  void Beep (const char* msg = 0);
  static CFont* GetFont() { return m_font; }
  static void SetFont (CFont* font);
  static void GetFont (LOGFONT* pLogFont);
  static void SetFont (LOGFONT* pLogFont);
  static bool LoadFontFromRegistry();

  void OnDestroyDebugwin() { m_debugWin = 0; }

  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void OnInitialUpdate();

protected:
  virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
  virtual void ClearCurMargins();
  virtual void SetCurMargins();

public:
  virtual ~cFedView();
  void ShowFile (cTextFile* file);

protected:
  void UpdateCaret();
  void Unselect();
  CPoint TranslateScreenPos (CPoint point);
  BOOL IsLeftMargin (int x);
  bool IsWordChar (char ch) { return isalnum(ch) || (ch == '_'); }
  void SwitchToFile (cTextFile* file);
  void OpenUpFrom (cTextFold* fold, long& yscroll);
  void GotoLine (cTextLine* line);
  void GotoLineNumber (int linenum);
  int FindLine (cTextLine* line);  // returns window y-pos of line
  int DrawText (CDC* pDC, cTextLine* textline, int y, int fold_indent = 0, bool wipe = false);
  void DrawChar (CDC* pDC, CPoint atpos, int col);
  void DrawMatch (bool display);

  cTextFold* getNewOutputBufferFold (const char* name);
  int GetFilters(CString& strFilter, const char* filename, const char* defaultExt); // returns filterIndex that matches filename
  bool DoPromptFileName(CString& fileName, const char* defaultExt, BOOL bOpenFileDialog);
  void DisplayAtLine (cTextLine* line);
  void DisplayEnd();
  bool MatchBracket();
  cTextFile* CurFile();

  void ShowFilename();
  cTextLine* GetLine (int linenum);
  void SetColour (CDC* pDC, int col, int& curcol, bool beyond_wrap = false);

  void CopySelectToClipboard();
  void CopyPasteToClipboard();
  //bool DeleteSelected();
  void SetupScrollBars();
  void SetCursorPos (CPoint pos);
  void SetCursorPos (int x, int y);
  void MoveCursorBy (int dx, int dy);
  void KeepCursorOnScreen();
  void KeyProc (int token);
  const unsigned char* DoMacro (const unsigned char* macro, int conditional = 0, int level = 0);
  bool CheckFileTime (const char* title, cTextFile* fileToCheck = 0);

  CString GetSelectString();
  CString SubstitutePath (const char* format, const char* path, int flags, const char* comment, const char* selection, CString* parsed_dir = 0);

  //{{{  token actions
  void OnCharLeft();
  void OnCharRight();
  void OnCharLeftSelect();
  void OnCharRightSelect();
  void OnCharDeleteLeft();
  void OnCharDeleteRight();
  void OnCharToUppercase();
  void OnCharToLowercase();

  bool WordLeft();
  bool WordRight();
  bool WordSelect(int selectmore = 0);
  bool WordLeftSelect();
  bool WordRightSelect();

  void OnWordLeft();
  void OnWordRight();
  void OnWordSelect();
  void OnWordSelectPart();
  void OnWordLeftSelect();
  void OnWordRightSelect();
  void OnWordDelete();
  void OnWordDeleteLeft();
  void OnWordDeleteRight();
  void OnWordToUppercase();
  void OnWordToLowercase();

  void OnLineStart();
  void OnLineEnd();
  void OnLineUp();
  void OnLineDown();
  void OnLineSelect();
  void OnLineStartSelect();
  void OnLineEndSelect();
  void OnLineUpSelect();
  void OnLineDownSelect();
  void OnLineDelete();
  void OnLineDeleteToStart();
  void OnLineDeleteToEnd();
  void OnLineUndelete();
  void OnLineDuplicate();
  void OnLineMove();
  void OnLineIndentLess();
  void OnLineIndentMore();
  void OnLineSwapUp();
  void OnLineSwapDown();
  void OnLineMakeComment();
  void OnLineRemoveComment();

  void OnScrollLineUp();
  void OnScrollLineDown();
  void OnScrollPageUp();
  void OnScrollPageDown();
  void OnScrollPageUpSelect();
  void OnScrollPageDownSelect();

  void OnGotoFoldTop();
  void OnGotoFoldBottom();
  void OnGotoFileTop();
  void OnGotoFileBottom();
  void OnGotoProject();
  void OnGotoLineNumber();
  void OnGotoPrevFile();
  void OnGotoNextFile();
  void OnGotoBracketPair();
  void OnGotoBracketPairSelect();
  void OnGotoPrevFormfeed();
  void OnGotoNextFormfeed();
  void OnGotoError (const char* str);

  bool FoldEnter (cTextFold* fold, long& yscroll);
  bool FoldExit (bool leave_open, long& yscroll);

  void OnFoldOpen (cTextFold* fold = 0);
  void OnFoldClose (cTextFold* fold = 0);
  void OnFoldEnter (cTextFold* fold = 0);
  void OnFoldExit (bool leave_open = false);
  void OnFoldOpenAll();
  void OnFoldCloseAll();
  void OnFoldCreate();
  void OnFoldRemove();
  void OnFoldEnterOpened();
  void OnFoldOpenEntered();

  void OnFileNew();
  void OnFileOpen();
  void OnFileClose();
  void OnFileCloseAll();
  void OnFileSave();
  void OnFileSaveas();
  void OnFileSaveAll();
  void OnFileSaveProject();
  void OnFileInsertHere();
  void OnFileReload();
  void OnFileToggleProtect();
  void OnFileRenumber();
  void OnFileShowEdits();
  void OnFileShowLineNumbers();
  void OnFileShowFolds();
  void OnFileOpenAssociate();
  void OnFileOpenInclude();

  void OnCut();
  void OnCopy();
  void OnPaste();
  void OnClearPaste();
  void OnSelectAll();
  void OnUnSelect();
  void OnLineCut();
  void OnLineCopy();
  void OnKeywordComplete();

  void OnFindAgain();
  void OnFindDown();
  void OnFindUp();
  bool OnReplace();
  void OnReplaceAll();
  void OnSubstitute();
  void OnFindDialog();
  void OnFindReplaceDialog();

  void OnMacroDialog();
  void OnMacroLearn();
  void OnMacroCall();
  void OnMacroLoad();
  void OnMacroSave();

  void OnTab();
  void OnReturn();
  void OnFormFeed();
  void OnAlignNextColumn();
  void OnDealignNextColumn();
  void OnUndo();
  void OnUndoLine();
  void OnRefresh();
  void OnAbort();
  void OnOverStrike();
  void OnToggleTabs();

  void OnShowDebug();
  //}}}

// Generated message map functions
protected:
  bool OnAnyKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
  bool OnAnyKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);

  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnSetFocus(CWnd* pOldWnd);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
  afx_msg void OnUpdateIndicatorRec(CCmdUI* pCmdUI);
  afx_msg void OnUpdateIndicatorOvr(CCmdUI* pCmdUI);
  afx_msg void OnUpdateIndicatorPos(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFileSaveAll(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFileSaveProject(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEditReplace(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEditProtect(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewLinenumbers(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewFolds(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewGotoline(CCmdUI* pCmdUI);
  afx_msg LRESULT OnFindReplace (WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
};

inline cFedDoc* cFedView::GetDocument() { return (cFedDoc*)m_pDocument; }
inline cMainFrame* cFedView::GetFrame() { return (cMainFrame*) GetParentFrame(); }
