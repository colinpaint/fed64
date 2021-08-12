// GotoDialog.cpp
//{{{  includes
#include "pch.h"
#include "Dialog.h"

#include "MainFrm.h"
//}}}
#define psh1        0x0400
#define psh2        0x0401
#define edt1        0x0480
#define edt2        0x0481

UINT cGotoDialog::m_lineNumber = 1;
//{{{
DWORD cGotoDialog::HelpLinks[] = {
  IDC_VIEW_LINENUMBER, 1010,
  0, 0
  };
//}}}
//{{{
cGotoDialog::cGotoDialog(CWnd* pParent /*=NULL*/)
        : CDialog(cGotoDialog::IDD, pParent)
{
  //{{AFX_DATA_INIT(cGotoDialog)
    //m_lineNumber = 1;
  //}}AFX_DATA_INIT
}
//}}}
//{{{
void cGotoDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
    DDX_Text (pDX, IDC_VIEW_LINENUMBER, m_lineNumber);
    DDV_MinMaxUInt (pDX, m_lineNumber, 1, 2147483647);
}
//}}}
//{{{
BEGIN_MESSAGE_MAP(cGotoDialog, CDialog)
  ON_WM_HELPINFO()
  ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()
//}}}

// cInputDialog
//{{{
cInputDialog::cInputDialog (const char* name, CWnd* pParent /*=NULL*/)
  : CDialog(cInputDialog::IDD, pParent)
{
  if (name && *name)
    m_title = name;
}
//}}}
//{{{
void cInputDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Text (pDX, IDC_INPUT_EDIT, inputStr);

  inputStr.TrimLeft();
  inputStr.TrimRight();
}
//}}}
//{{{
BOOL cInputDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetWindowText (m_title);

  CWnd* item = GetDlgItem (IDC_INPUT_EDIT);
  ASSERT (item);
  item->SetFocus();

  return FALSE;      // we have explicitly set the focus
}
//}}}
//{{{
BEGIN_MESSAGE_MAP(cInputDialog, CDialog)
END_MESSAGE_MAP()
//}}}

// cMyFindReplaceDialog - modified CFindReplaceDialog
//{{{
cMyFindReplaceDialog::cMyFindReplaceDialog() : CFindReplaceDialog() {}
//}}}
//{{{
BEGIN_MESSAGE_MAP(cMyFindReplaceDialog, CFindReplaceDialog)
  ON_CBN_EDITCHANGE (edt1, OnEditChange)
  ON_CBN_SELCHANGE (edt1, OnEditChange)
END_MESSAGE_MAP()
//}}}
//{{{
void cMyFindReplaceDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  CComboBox* findcombo = (CComboBox*) GetDlgItem (edt1);
  ASSERT(findcombo);
  CComboBox* replacecombo = (CComboBox*) GetDlgItem (edt2);

  if (!pDX->m_bSaveAndValidate) {
    ASSERT(GetParentFrame()->IsKindOf(RUNTIME_CLASS(cMainFrame)));
    cMainFrame* frame = (cMainFrame*) GetParentFrame();
    if (findcombo)
      frame->FillSearchBox (*findcombo, false);
    if (replacecombo)
      frame->FillSearchBox (*replacecombo, true);
    OnEditChange();
    }

  DDX_Check (pDX, IDC_SEARCH_ALL, m_searchAll);
  DDX_Check (pDX, IDC_IGNORE_COMMENTS, m_ignoreComments);
}
//}}}
//{{{
void cMyFindReplaceDialog::OnEditChange()
{
  CString str;
  GetDlgItem (edt1)->GetWindowText (str);

  BOOL enable = !str.IsEmpty();
  GetDlgItem (IDOK)->EnableWindow (enable);

  CWnd* item = GetDlgItem (psh1);
  if (item)
    item->EnableWindow (enable);

  item = GetDlgItem (psh2);
  if (item)
    item->EnableWindow (enable);
}
//}}}
