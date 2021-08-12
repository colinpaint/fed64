#pragma once

class cFedView;
class cTextLine;

class cDebugWin : public CWnd {

  enum { numberOfPages = 5, maxMsg = 4 };

public:
  cDebugWin (cFedView* view);
  virtual ~cDebugWin();

  void Update (cFedView* view);
  void NextPage();
  void Message (int msg_num, const char* msg = 0);

protected:
  virtual BOOL PreCreateWindow (CREATESTRUCT& cs);

  void PaintPos (CDC& dc, int& x, int& y);
  void PaintLine (CDC& dc, int& x, int& y, cTextLine* line, const char* msg);
  void PaintLanguage (CDC& dc, int& x, int& y);

  void PaintPage1 (CDC& dc);
  void PaintPage2 (CDC& dc);
  void PaintPage3 (CDC& dc);
  void PaintPage4 (CDC& dc);
  void PaintColourPage (CDC& dc);
  void PaintUndoPage (CDC& dc);

  void SetName();

  afx_msg void OnPaint();
  afx_msg void OnDestroy();
  afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);

  DECLARE_MESSAGE_MAP()

  cFedView* curView;
  char* messages [maxMsg];
  int m_page;
  CSize m_cChar;
  int m_xStart;
  };
