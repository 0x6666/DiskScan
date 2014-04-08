// 用于显示十六进制数据的视图
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISKSCANVIEW_H__CD01A111_9CBA_4214_AFB2_10E20156F0C1__INCLUDED_)
#define AFX_DISKSCANVIEW_H__CD01A111_9CBA_4214_AFB2_10E20156F0C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DiskScanDoc.h"
#include "DataWnd.h"

class CHexDataView : public CFormView
{
protected: // create from serialization only
	CHexDataView();
	DECLARE_DYNCREATE(CHexDataView)

public:
	//{{AFX_DATA(CDiskScanView)
	enum{ IDD = IDD_DISKSCAN_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	DataWnd* m_wndData;
	/*	Disk* m_pDisk;*/

	CDataDoc* GetDocument();
// Operations
public:
	//////////////////////////////////////////////////////////////////////////
	//设置可以显示的扇区总数
	//param
	//		cnt		扇区总数
	//////////////////////////////////////////////////////////////////////////
	void SetSecCount(LONG_INT cnt);

	//////////////////////////////////////////////////////////////////////////
	//设置可以显示的起始号
	//param
	//		sec		扇区总数
	//////////////////////////////////////////////////////////////////////////
	void SetStartSector(LONG_INT sec);

	//////////////////////////////////////////////////////////////////////////
	//获得视图显示需要的最小大小
	//////////////////////////////////////////////////////////////////////////
	int GetMinWidth(); 

	//////////////////////////////////////////////////////////////////////////
	//设置视图中的数据选择区域 
	//param
	//		start	选择区的起始位置
	//		end		选择区的结束位置
	//////////////////////////////////////////////////////////////////////////
	void SetSel( LONG_INT start, LONG_INT end );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskScanView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHexDataView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDiskScanView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGetData(WPARAM, LPARAM);			//空间需要获取数据
	afx_msg LRESULT OnChangeWidth(WPARAM, LPARAM);		//控件的狂度改变了
	afx_msg LRESULT OnChangeSector(WPARAM , LPARAM);	//显示的第一个扇区已经生了改变
//	afx_msg LRESULT OnGotUnPartableSecCnt(WPARAM cnt, LPARAM ptr);//获取了不可分配空间扇区数的消息函数
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
//	virtual BOOL DestroyWindow();
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKSCANVIEW_H__CD01A111_9CBA_4214_AFB2_10E20156F0C1__INCLUDED_)
