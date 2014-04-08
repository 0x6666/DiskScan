#if !defined(AFX_CtrlBar_H__B56B072B_8088_439A_B4B2_D7DEAAC4C116__INCLUDED_)
#define AFX_CtrlBar_H__B56B072B_8088_439A_B4B2_D7DEAAC4C116__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxpriv.h>
// CtrlBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCtrlBar dialog
//Ϊ�˱��ڲ��� ��CCtrlBar������һ�¶��������
#define CTRL_EDGELEFT       0x00000001		//���
#define CTRL_EDGERIGHT      0x00000002		//�ұ�
#define CTRL_EDGETOP        0x00000004		//����
#define CTRL_EDGEBOTTOM     0x00000008		//�ײ�
#define CTRL_EDGEALL        0x0000000F		//��������λ�õļ���
#define CTRL_SHOWEDGES      0x00000010		//��Ҫ��ʾ����
#define CTRL_SIZECHILD      0x00000020		//���ӿؼ�



class CCtrlBar : public CDialogBar
{
	//����������Ŀ��ֻ��Ϊ��ʹCCtrlBar����
	//����CDockBar�ı�����Ա
	class CCtrlDockBar : public CDockBar{friend class CCtrlBar;};

	DECLARE_DYNAMIC(CCtrlBar);
// Construction
public:
	CCtrlBar(CWnd* pParent = NULL);   // standard constructor
	~CCtrlBar();
	CSize	m_szFloat;		//���ڸ����ǵĴ�С
	CSize   m_szMinFloat;	//��С�ĸ�����С
	CSize	m_szVDocked;	//����ͣ��
	CSize	m_szHDocked;	//����ͣ���Ĵ�С
	CSize	m_szMinHorz;	//��С�ĺ���ͣ��
	CSize	m_szMinVert;	//�������С��С

	BOOL	m_bChgDockedSize;//�Ƿ�Ҫ����Ĭ�ϵĴ�С
	
	int		m_nDockBarID;	//��ǰ��������ID   �ж�ͣ���ĵط�
	DWORD	m_dwCtrlStyle;	//�ؼ�������
	//UINT	m_cxEdge;		//�߿�Ŀ��

	BOOL	m_bTracking;	//�Ƿ����϶����ڵĴ�С
	UINT	m_htEdge;		//�϶����ڴ�Сʱ��������ڵĴ��ڷǿͻ�ȥ��λ��
	LONG	m_nTrackPosOld;	//�϶����ڴ�Сʱ�Ĵ���ԭ��λ�ô�С
	LONG	m_nTrackEdgeOfs;//�϶�λ�õ��������ĵ�ƫ��
	LONG	m_nTrackPosMin;	//�϶������´�С
	LONG	m_nTrackPosMax;	//�϶�������С


	// Dialog Data
	//{{AFX_DATA(CCtrlBar)
	enum { IDD = IDD_DVE_LIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlBar)
	public:
virtual BOOL Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID ,CSize size);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	//����ͻ����Ĵ�С
	//virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);
	//��ʼ�϶����ڵĴ�С
	virtual void StartTracking( UINT nHitTest, CPoint point );
	//ֹͣ�϶����ڵĴ�С
	virtual void StopTracking();
	//�϶����ڵĴ�С(�ڷǿͻ�ȥ��ס�������ƶ����)
	virtual void OnTrackUpdateSize(CPoint& point);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCtrlBar)
	afx_msg LRESULT OnInitDialog (UINT wParam, LONG lParam);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNcHitTest(CPoint point);  //������λ��
	//����ͻ����Ĵ�С
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	//���Ʒǿͻ���
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	//�ѺͿͻ�ȥ��갴��
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//����ƶ���Ϣ
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	//����������
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//����Ҽ�����
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//��ǰ����ʧȥ��������뽹��,�п����ǵ�����ReleaseCapture
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	BOOL IsHDocking();//�жϵ������Ƿ��Ǻ���ͣ������
	BOOL IsVDocking();//������ͣ������


	UINT GetEdgeHTCode( int i );	//���λ�ö�Ӧ����� 	0-HTLEFT; 1-HTTOP; 2-HTRIGHT; 3-HTBOTTOM;
	BOOL GetEdgeRect( CRect rcWnd/*���ھ���*/,
					  UINT nHitTest/*�ƶ��Ǹ�����ı߿�*/,
					  CRect& rcEdge/*��õ��ƶ��߿�*/ );//��������κα߿�Ļ�����FALSE

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CtrlBar_H__B56B072B_8088_439A_B4B2_D7DEAAC4C116__INCLUDED_)
