// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__8B913FA4_F4F2_4830_8B47_598F79D31F7C__INCLUDED_)
#define AFX_CHILDFRM_H__8B913FA4_F4F2_4830_8B47_598F79D31F7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CtrlBar.h"
#include "ColumnSplitter.h"


class CChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrm)
public:
	CChildFrm();

// Attributes
public:
	CCtrlBar		m_DisBar;		//磁盘设备的显Bar
	CListCtrl		m_DisList;		//磁盘列表
	CColumnSplitter	m_wndSplitter;	//分切窗口空间

// Operations
public:
/*	void InitCtrl();*/
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContentList();
	afx_msg void OnUpdateContentList(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
// 	//////////////////////////////////////////////////////////////////////////
// 	//获得用于显示信息的视图类，这个方法需要由具体的子框架实现
// 	virtual CRuntimeClass* GetInofViewClass(void);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
//	afx_msg void OnPosParaentDir();
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__8B913FA4_F4F2_4830_8B47_598F79D31F7C__INCLUDED_)
