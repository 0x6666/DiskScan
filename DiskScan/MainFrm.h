// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__FC5F4441_CD5D_46F7_BF97_851624C71954__INCLUDED_)
#define AFX_MAINFRM_H__FC5F4441_CD5D_46F7_BF97_851624C71954__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlBar.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame(); 

// Attributes
public:

// 	//由主框架维护一个图标列表
// 	CImageList m_wndImageList;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

//	CCtrlBar	m_wndDevList;

// Generated message map functions
protected:
/*	void InitImageList();//初始化imagelist*/
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();

	//////////////////////////////////////////////////////////////////////////
	//param
	//		wParam=》CString* 用于打开文档的参数
	//		lParam			  要打开的设备类型	PART_*  现在只支持 PART_FAT32
	//return	返回打开设备的指针,如果失败的话返回NULL
	//////////////////////////////////////////////////////////////////////////
	afx_msg LRESULT OnOpenNewDoc(WPARAM, LPARAM);

	//////////////////////////////////////////////////////////////////////////
	//设置当前数据区的名字
	//param
	//		wParam=>CString* 要设置的名字
	afx_msg LRESULT OnSetCurDataViewName(WPARAM wParam, LPARAM);

	//////////////////////////////////////////////////////////////////////////
	//打开指定的文件或者目录,在设备上定位指定的文件
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnOpenFileDir();

	//////////////////////////////////////////////////////////////////////////
	//定位选择好了的文件路径
	//param
	//		strPath	需要定位的路径  如"c:\teste.fuck"
	//////////////////////////////////////////////////////////////////////////
	void	PosFileDir(CString	strPath);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__FC5F4441_CD5D_46F7_BF97_851624C71954__INCLUDED_)
