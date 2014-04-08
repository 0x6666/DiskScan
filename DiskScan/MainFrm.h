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

// 	//�������ά��һ��ͼ���б�
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
/*	void InitImageList();//��ʼ��imagelist*/
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();

	//////////////////////////////////////////////////////////////////////////
	//param
	//		wParam=��CString* ���ڴ��ĵ��Ĳ���
	//		lParam			  Ҫ�򿪵��豸����	PART_*  ����ֻ֧�� PART_FAT32
	//return	���ش��豸��ָ��,���ʧ�ܵĻ�����NULL
	//////////////////////////////////////////////////////////////////////////
	afx_msg LRESULT OnOpenNewDoc(WPARAM, LPARAM);

	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰ������������
	//param
	//		wParam=>CString* Ҫ���õ�����
	afx_msg LRESULT OnSetCurDataViewName(WPARAM wParam, LPARAM);

	//////////////////////////////////////////////////////////////////////////
	//��ָ�����ļ�����Ŀ¼,���豸�϶�λָ�����ļ�
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnOpenFileDir();

	//////////////////////////////////////////////////////////////////////////
	//��λѡ����˵��ļ�·��
	//param
	//		strPath	��Ҫ��λ��·��  ��"c:\teste.fuck"
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
