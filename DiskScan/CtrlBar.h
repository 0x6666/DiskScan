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
//为了便于操作 给CCtrlBar定义了一下额外的属性
#define CTRL_EDGELEFT       0x00000001		//左边
#define CTRL_EDGERIGHT      0x00000002		//右边
#define CTRL_EDGETOP        0x00000004		//顶部
#define CTRL_EDGEBOTTOM     0x00000008		//底部
#define CTRL_EDGEALL        0x0000000F		//以上四种位置的集合
#define CTRL_SHOWEDGES      0x00000010		//需要显示边线
#define CTRL_SIZECHILD      0x00000020		//是子控件



class CCtrlBar : public CDialogBar
{
	//定义这个类的目的只是为了使CCtrlBar可以
	//访问CDockBar的保护成员
	class CCtrlDockBar : public CDockBar{friend class CCtrlBar;};

	DECLARE_DYNAMIC(CCtrlBar);
// Construction
public:
	CCtrlBar(CWnd* pParent = NULL);   // standard constructor
	~CCtrlBar();
	CSize	m_szFloat;		//窗口浮动是的大小
	CSize   m_szMinFloat;	//最小的浮动大小
	CSize	m_szVDocked;	//纵向停靠
	CSize	m_szHDocked;	//横向停靠的大小
	CSize	m_szMinHorz;	//最小的横向停靠
	CSize	m_szMinVert;	//纵向的最小大小

	BOOL	m_bChgDockedSize;//是否要保持默认的大小
	
	int		m_nDockBarID;	//当前控制条的ID   判断停靠的地方
	DWORD	m_dwCtrlStyle;	//控件的属性
	//UINT	m_cxEdge;		//边框的宽度

	BOOL	m_bTracking;	//是否在拖动窗口的大小
	UINT	m_htEdge;		//拖动窗口大小时的鼠标所在的窗口非客户去的位置
	LONG	m_nTrackPosOld;	//拖动窗口大小时的窗口原有位置大小
	LONG	m_nTrackEdgeOfs;//拖动位置到窗口中心的偏移
	LONG	m_nTrackPosMin;	//拖动的最下大小
	LONG	m_nTrackPosMax;	//拖动的最大大小


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
	//计算客户区的大小
	//virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);
	//开始拖动窗口的大小
	virtual void StartTracking( UINT nHitTest, CPoint point );
	//停止拖动窗口的大小
	virtual void StopTracking();
	//拖动窗口的大小(在非客户去按住鼠标左键移动鼠标)
	virtual void OnTrackUpdateSize(CPoint& point);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCtrlBar)
	afx_msg LRESULT OnInitDialog (UINT wParam, LONG lParam);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNcHitTest(CPoint point);  //鼠标光标的位置
	//计算客户区的大小
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	//绘制非客户区
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	//费和客户去鼠标按下
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	//鼠标左键弹起
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标右键按下
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//当前窗口失去了鼠标输入焦点,有可能是调用了ReleaseCapture
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	BOOL IsHDocking();//判断当窗口是否是很行停靠窗口
	BOOL IsVDocking();//是纵向停靠窗口


	UINT GetEdgeHTCode( int i );	//鼠标位置对应的序号 	0-HTLEFT; 1-HTTOP; 2-HTRIGHT; 3-HTBOTTOM;
	BOOL GetEdgeRect( CRect rcWnd/*窗口矩形*/,
					  UINT nHitTest/*制定那个方向的边框*/,
					  CRect& rcEdge/*获得的制定边框*/ );//如果不在任何边框的话返回FALSE

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CtrlBar_H__B56B072B_8088_439A_B4B2_D7DEAAC4C116__INCLUDED_)
