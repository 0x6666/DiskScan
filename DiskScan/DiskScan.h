// DiskScan.h : main header file for the DISKSCAN application
//

#if !defined(AFX_DISKSCAN_H__283E6228_61B8_42DA_BC9A_4BEAC24E208C__INCLUDED_)
#define AFX_DISKSCAN_H__283E6228_61B8_42DA_BC9A_4BEAC24E208C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "publicFun.h"

/////////////////////////////////////////////////////////////////////////////
// CDiskScanApp:
// See DiskScan.cpp for the implementation of this class
//

class CDiskScanApp : public CWinApp
{
public:
	CDiskScanApp();

	//磁盘的文档管理器
	CMultiDocTemplate* m_pDiskDocTemplate;

	//Fat32卷的文档管理器
	CMultiDocTemplate* m_pFat32DocTemplate;

	//Ntfs卷的文档管理器
	CMultiDocTemplate* m_pNtfsDocTemplate;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskScanApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDiskScanApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKSCAN_H__283E6228_61B8_42DA_BC9A_4BEAC24E208C__INCLUDED_)
