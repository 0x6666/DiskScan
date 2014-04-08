// ������ʾʮ���������ݵ���ͼ
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
	//���ÿ�����ʾ����������
	//param
	//		cnt		��������
	//////////////////////////////////////////////////////////////////////////
	void SetSecCount(LONG_INT cnt);

	//////////////////////////////////////////////////////////////////////////
	//���ÿ�����ʾ����ʼ��
	//param
	//		sec		��������
	//////////////////////////////////////////////////////////////////////////
	void SetStartSector(LONG_INT sec);

	//////////////////////////////////////////////////////////////////////////
	//�����ͼ��ʾ��Ҫ����С��С
	//////////////////////////////////////////////////////////////////////////
	int GetMinWidth(); 

	//////////////////////////////////////////////////////////////////////////
	//������ͼ�е�����ѡ������ 
	//param
	//		start	ѡ��������ʼλ��
	//		end		ѡ�����Ľ���λ��
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
	afx_msg LRESULT OnGetData(WPARAM, LPARAM);			//�ռ���Ҫ��ȡ����
	afx_msg LRESULT OnChangeWidth(WPARAM, LPARAM);		//�ؼ��Ŀ�ȸı���
	afx_msg LRESULT OnChangeSector(WPARAM , LPARAM);	//��ʾ�ĵ�һ�������Ѿ����˸ı�
//	afx_msg LRESULT OnGotUnPartableSecCnt(WPARAM cnt, LPARAM ptr);//��ȡ�˲��ɷ���ռ�����������Ϣ����
	
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
