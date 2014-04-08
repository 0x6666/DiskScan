// CCtrlBar.cpp : implementation file
//

#include "stdafx.h"
#include "DiskScan.h"
#include "CtrlBar.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCCtrlBar dialog

IMPLEMENT_DYNAMIC(CCtrlBar, CControlBar);
CCtrlBar::CCtrlBar(CWnd* pParent /*=NULL*/)
: CDialogBar()
{
	m_nDockBarID	= 0;
	m_szMinFloat	= CSize(20 , 20 );
	m_szMinHorz		= CSize(20 , 20 );
	m_szMinVert		= CSize(20 , 20);
	m_dwCtrlStyle	= 0;		//属性初始化为空
//	m_cxEdge		= 1;		//边框默认为5个像素
	m_bTracking		= FALSE;
	m_nTrackPosOld	= 0;
	m_nTrackPosMin	= 0;
	m_nTrackPosMax	= 0;
}

CCtrlBar::~CCtrlBar()
{
}


void CCtrlBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCCtrlBar)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCtrlBar, CDialogBar)
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCCtrlBar message handlers
BOOL CCtrlBar::Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID , CSize size)
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bRes = CDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID );
	
	this->m_szFloat = this->m_szHDocked = this->m_szVDocked = size;
	this->m_szVDocked.cx = 200;//纵向停靠时默认200的宽度
	this->m_szHDocked.cy = 100;//横向停靠时默认100的高度
	this->m_dwCtrlStyle |= CTRL_SHOWEDGES;
	return bRes;
}
LRESULT CCtrlBar::OnInitDialog(UINT wParam, LONG lParam)
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	
	if (!UpdateData(FALSE))
		TRACE0("Warning: UpdateData failed during dialog init.\n");

	//初始化空间
	return bRet;
}

CSize CCtrlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
// 	int temp = 0;
// 	//停靠了或者浮动的话返回默认的大小
// 	if ((dwMode & LM_VERTDOCK) || (dwMode & LM_HORZDOCK))
// 	{
// 		if (dwMode & LM_STRETCH) //停靠时拉伸  莫有实现 
// 			return CSize((dwMode & LM_HORZ) ? 32767 : m_szDocked.cx,
// 					     (dwMode & LM_HORZ) ? m_szDocked.cy : 32767);
// 		else {//停靠在框架的边上
// 			CRect rc;
// 			this->GetDockingFrame()->GetClientRect(&rc);
// 			//AfxGetMainWnd()
// 			if(dwMode & LM_HORZ)
// 			{//横向的停靠
// 				//计算父窗口的大小以返回父窗口的大小的
// 
// 				return CSize(rc.Width()//- ::GetSystemMetrics(SM_CXEDGE)*2
// 					,m_szHDocked.cy);
// /*				return mHDocked;*/
// 			}else{//纵向停靠
// 				return CSize(m_szVDocked.cx,
// 					rc.Height()- 
// 					::GetSystemMetrics(SM_CYMENUSIZE)
// 					//- 
// 					//::GetSystemMetrics(SM_CYEDGE)*2
// 					);
// /*				return mVDocked;*/
// 			}
// 		}
// 		//return CControlBar::CalcDynamicLayout(nLength, dwMode);
// 	}
// 	if (dwMode & LM_MRUWIDTH)
// 		return m_szFloat;
// 
// 	//在浮动的情况下拉动窗口
// 	if (dwMode & LM_LENGTHY){
// 		if(m_bChgDockedSize)
// 			m_szFloat.cy  = nLength ;
// 		else
// 			m_szFloat.cy = nLength;
// 
// 		return CSize(m_szFloat.cx,nLength);
// 		//return CSize(m_sizeFloating.cx, (m_bChgDockedSize) ?m_sizeFloating.cy = m_sizeDocked.cy = nLength :m_sizeFloating.cy = nLength);
// 	}else{
// 		
// 		if(m_bChgDockedSize)
// 			m_szFloat.cx = nLength ;
// 		else
// 			m_szFloat.cx = nLength;
// 		
// 		return CSize(nLength , m_szFloat.cy);
// 
// 		return CSize((m_bChgDockedSize) ? m_szFloat.cx = 
// 			m_szFloat.cx = nLength :m_szFloat.cx = nLength, m_szFloat.cy);   
// 	}

	if (dwMode & (LM_HORZDOCK | LM_VERTDOCK)) //已经停靠了?
	{
		if (0 == (dwMode & LM_STRETCH))
		{//只处理可停靠的控制条

			CRect rc;  //所停靠的框架的大小
			/*this->GetDockingFrame()->GetWindowRect(&rc);*/
			this->GetDockingFrame()->GetClientRect(&rc);
			if(dwMode & LM_HORZ)  //横向的停靠话，返回框架的宽度
				return CSize(rc.Width() + 3  , m_szHDocked.cy); //我不知道这里为什么要加3个像素，我感觉这里应该是可以不要加的
			else //纵向停靠,框架的高度
				return CSize(m_szVDocked.cx , rc.Height() );
		}

		return CControlBar::CalcDynamicLayout(nLength, dwMode);
	}

	//最常用的动态大小
	if (dwMode & LM_MRUWIDTH) return this->m_szFloat;
	//重置LM_MRUWIDTH到当前的浮动大小
	if (dwMode & LM_COMMIT) return m_szFloat;

	//浮动时改变窗口的大小
	((dwMode & LM_LENGTHY) ? m_szFloat.cy : m_szFloat.cx) = nLength;

	m_szFloat.cx = max(m_szFloat.cx, m_szMinFloat.cx);
	m_szFloat.cy = max(m_szFloat.cy, m_szMinFloat.cy);

	return m_szFloat;
}

void CCtrlBar::OnSize(UINT nType, int cx, int cy)
{
	//在这里确保次DialogBar中的控件是最大的
	CWnd* pWnd = GetWindow(GW_CHILD);
	if (pWnd != NULL)
	{
		if (m_bTracking || IsFloating())
		{
			//这里需要重绘
			pWnd->MoveWindow(0, 0, cx, cy , TRUE);
			this->m_pDockSite->DelayRecalcLayout();
		}else
		{//要避免频繁绘制窗口，导致上窗口闪烁的结果  FALSE很重要
			pWnd->MoveWindow(0, 0, cx, cy , FALSE);
		}
		//要保证只有一个子窗口，否则就会出问题了
		ASSERT(pWnd->GetWindow(GW_HWNDNEXT) == NULL);
	}
}
void CCtrlBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
	//如果窗口大小改变或者移动了的话 强制其从新计算非客户区
	//这点很重要，否则浮动之后再停靠的话就边框就会很小
	lpwndpos->flags |= SWP_FRAMECHANGED;

    CControlBar::OnWindowPosChanging(lpwndpos);

    //获得当前停靠框架的ID,用于判断当前停靠的位置
    m_nDockBarID = GetParent()->GetDlgCtrlID();
}
BOOL CCtrlBar::IsHDocking()
{
	if (!IsFloating())
	{
		if (m_nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        m_nDockBarID == AFX_IDW_DOCKBAR_BOTTOM)
			return TRUE;
	}
	return FALSE;
}
BOOL CCtrlBar::IsVDocking()
{
	if (!IsFloating())
	{
		if (m_nDockBarID == AFX_IDW_DOCKBAR_LEFT ||
			m_nDockBarID == AFX_IDW_DOCKBAR_RIGHT)
			return TRUE;
	}
	return FALSE;
}
LRESULT CCtrlBar::OnNcHitTest(CPoint point)
{//框架通过这里判断是否可以拖动窗口的大小
	CRect rcBar, rcEdge;
	GetWindowRect(rcBar);  //获得当前DialogBar的窗口矩形

	if (!IsFloating())
	{//如果不是浮动的话 则需要此时是否在 边框上  否则就客户区
		for (int i = 0; i < 4; i++)
		{//迭代每一个边框  ,判断在哪一个位置
			if (GetEdgeRect(rcBar, GetEdgeHTCode(i), rcEdge))
			{	
				if (rcEdge.PtInRect(point))
					return GetEdgeHTCode(i);
			}
		}
	}
	return HTCLIENT;
}

UINT CCtrlBar::GetEdgeHTCode( int nEdge)
{
	if (nEdge == 0) return HTLEFT;
	if (nEdge == 1) return HTTOP;
	if (nEdge == 2) return HTRIGHT;
	if (nEdge == 3) return HTBOTTOM;

	//对不起我接受四个方向 ， 其他的都挂掉
	ASSERT(FALSE);
	return HTNOWHERE;
}

BOOL CCtrlBar::GetEdgeRect( CRect rcWnd, UINT  nHitTest, CRect& rcEdge )
{
	rcEdge = rcWnd;	//DialogBar的窗口矩形

	if (m_dwCtrlStyle & CTRL_SHOWEDGES)
		rcEdge.DeflateRect(1, 1);  //要显示边线的话 将原有的窗口缩小一个像素,用于绘制边线

	BOOL bHorz = IsHDocking();  //是否为横向停靠

	switch (nHitTest)
	{
	case HTLEFT:  //获取左边的边框范围 
		if (!(m_dwCtrlStyle & CTRL_EDGELEFT))
			return FALSE;	//不是左边可以拖动窗口大小的话，直接返回FALSE

		rcEdge.right = rcEdge.left +3 /*m_cxEdge*/;		//边框为3个像素
//		rcEdge.DeflateRect(0, bHorz ? m_cxEdge: 0); //横向停靠的话 也要扣除上下的边框
		break;

	case HTTOP:	//获取顶部的边框
		if (!(m_dwCtrlStyle & CTRL_EDGETOP))
			return FALSE;//不是顶部可以拖动窗口大小的话，直接返回FALSE

		rcEdge.bottom = rcEdge.top +3 /*m_cxEdge*/;
//		rcEdge.DeflateRect(bHorz ? 0 : m_cxEdge, 0);
		break;

	case HTRIGHT://获取右边的边框
		if (!(m_dwCtrlStyle & CTRL_EDGERIGHT))
			return FALSE;

		rcEdge.left = rcEdge.right - 3/*m_cxEdge*/;
//		rcEdge.DeflateRect(0, bHorz ? m_cxEdge: 0);
		break;

	case HTBOTTOM:  //获取下面的边框
		if (!(m_dwCtrlStyle & CTRL_EDGEBOTTOM))
			return FALSE;

		rcEdge.top = rcEdge.bottom -3 /*m_cxEdge*/;
//		rcEdge.DeflateRect(bHorz ? 0 : m_cxEdge, 0);
		break;

	default:		//对不起我只接受四种方向 出错了  只有四个方向
		ASSERT(FALSE);
	}
	return TRUE;
}

void CCtrlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (IsFloating())
	{//调整浮动是的小框架的大小

		CFrameWnd* pFrame = GetParentFrame();  //当前DialogBar的迷你框架对象

		if (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
		{
			DWORD dwStyle = ::GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
			if ((dwStyle & MFS_4THICKFRAME) != 0)
			{//已经禁止了迷你框架调整大小
				pFrame->ModifyStyle(MFS_4THICKFRAME, 0); //使之可以调整大小
				GetParent()->ModifyStyle(0, WS_CLIPCHILDREN);  //阻止父窗口对子窗口的绘制
			}
		}
	}

	//计算客户区的大小  先去掉位置属性记录
	m_dwCtrlStyle &= ~CTRL_EDGEALL;


	//计算客户区的大小,确定那一边可以拖动窗口的大小
	CRect rc(lpncsp->rgrc[0]);
	//先将窗口缩小一点,流出一点空隙，稍微好看一点，顶部多2个像素给浮动窗口的
	rc.DeflateRect(3, 5, 3, 3); 
	//不是浮动的话就各个方向一样的空隙了
	if (m_nDockBarID != AFX_IDW_DOCKBAR_FLOAT)
		rc.DeflateRect(2, 0, 2, 2);
	
	
	//在可以拖动的位置在留出5个像素的宽度
	switch(m_nDockBarID)
	{
	case AFX_IDW_DOCKBAR_TOP:  //当前停靠在顶部
		m_dwCtrlStyle |= CTRL_EDGEBOTTOM;   //光标在底部可以拖动窗口的大小
		rc.DeflateRect( 3 , 0 , 0 , 0 );
		break;
	case AFX_IDW_DOCKBAR_BOTTOM://底部
		m_dwCtrlStyle |= CTRL_EDGETOP;
		rc.DeflateRect( 3 , 0 , 0 , 0 );
		break;
	case AFX_IDW_DOCKBAR_LEFT://左边
		m_dwCtrlStyle |= CTRL_EDGERIGHT;
		rc.DeflateRect( 0 , 3 , 0 , 0 );
		break;
	case AFX_IDW_DOCKBAR_RIGHT://右边
		m_dwCtrlStyle |= CTRL_EDGELEFT;
		rc.DeflateRect( 0 , 3 , 0 , 0 );
		break;
	}
	
	lpncsp->rgrc[0] = rc;
//	*pRc = rc;
//	NcCalcClient(&lpncsp->rgrc[0], m_nDockBarID);
}
// 
// void CCtrlBar::NcCalcClient( LPRECT pRc, UINT nDockBarID )
// {
// 	CRect rc(pRc);
// 
// 	//先将窗口缩小一点,流出一点空隙，稍微好看一点，顶部多2个像素给浮动窗口的
// 	rc.DeflateRect(3, 5, 3, 3); 
// 
// 	//不是浮动的话就各个方向一样的空隙了
// 	if (nDockBarID != AFX_IDW_DOCKBAR_FLOAT)
// 		rc.DeflateRect(2, 0, 2, 2);
// 
// 
// 	//在可以拖动的位置在留出5个像素的宽度
// 	switch(nDockBarID)
// 	{
// 	case AFX_IDW_DOCKBAR_TOP:  //当前停靠在顶部
// 		m_dwCtrlStyle |= CTRL_EDGEBOTTOM;   //光标在底部可以拖动窗口的大小
// 		rc.DeflateRect( 5 , 0 , 0 , 0 );
// 		break;
// 	case AFX_IDW_DOCKBAR_BOTTOM://底部
// 		m_dwCtrlStyle |= CTRL_EDGETOP;
// 		rc.DeflateRect( 5 , 0 , 0 , 0 );
// 		break;
// 	case AFX_IDW_DOCKBAR_LEFT://左边
// 		m_dwCtrlStyle |= CTRL_EDGERIGHT;
// 		rc.DeflateRect( 0 , 5 , 0 , 0 );
// 		break;
// 	case AFX_IDW_DOCKBAR_RIGHT://右边
// 		m_dwCtrlStyle |= CTRL_EDGELEFT;
// 		rc.DeflateRect( 0 , 5 , 0 , 0 );
// 		break;
// 	}
// 
// 	*pRc = rc;
// }

void CCtrlBar::OnNcPaint()
{
	//获得窗口的DC  用于绘制非客户区
	CWindowDC dc(this);

	//取出客户区
	CRect rcClient, rcBar;
	GetClientRect(rcClient);	//获得客户区的大小
	ClientToScreen(rcClient);	//将客户区的举行抓换到屏幕
	GetWindowRect(rcBar);		//窗口的矩形

	//将两个矩形对应到"客户区"的样子 便于绘制非客户区的边框等其他的东西
	//只是移动两个矩形，不改变大小，移动后rcClient还是rcBar得客户区
	rcClient.OffsetRect(-rcBar.TopLeft());
	rcBar.OffsetRect(-rcBar.TopLeft());

	//创建一个与当前窗口设备上下文相对应的设备上下文 ，以便于绘制操作
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);

	//创建一副位图用绘制边框
	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, rcBar.Width(), rcBar.Height());
	CBitmap* pOldBm = mdc.SelectObject(&bm);

	//在非客户区绘制边框(位置窗口的一个线条，一般是一条黑色的)
	CRect rcDraw = rcBar;
	DrawBorders(&mdc, rcDraw); 

	//用当前窗口类的背景刷子清理非客户区背景
	mdc.FillRect(rcDraw, CBrush::FromHandle(
		(HBRUSH) GetClassLong(m_hWnd, GCL_HBRBACKGROUND)));  

	if (m_dwCtrlStyle & CTRL_SHOWEDGES)
	{//需要显示边框，此边框非比边框  这是我自己留下来的一个"边框区域"
		CRect rcEdge;//绘制
		for (int i = 0; i < 4; i++)
		{
			if (GetEdgeRect(rcBar, GetEdgeHTCode(i), rcEdge))
			{//在四个边上绘制3d的边框
				mdc.Draw3dRect(rcEdge,
					::GetSysColor(COLOR_BTNHIGHLIGHT),
					::GetSysColor(COLOR_BTNSHADOW));
			}
		}
	}

	//NcPaintGripper(&mdc, rcClient);

	//磕掉客户区的区域  客户区不在这里绘制
	dc.IntersectClipRect(rcBar); //将当前窗口的举行进行裁剪
	dc.ExcludeClipRect(rcClient);//裁剪掉客户区
	
	//用已经创建很高的位图填充剩下的区域(非客户区)
	dc.BitBlt(0, 0, rcBar.Width(), rcBar.Height(), &mdc, 0, 0, SRCCOPY);

	//资源的释放与还原
	ReleaseDC(&dc);
	mdc.SelectObject(pOldBm);
	bm.DeleteObject();
	mdc.DeleteDC();
}

CSize CCtrlBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	if (bStretch) 
	{//被拉升了 不是一个停靠的控制条
		if (bHorz)
			return CSize(32767, m_szHDocked.cy);
		else
			return CSize(m_szVDocked.cx, 32767);
	}

	//在这里强制父类的指针转换成子类的指针，
	//在这里是没问题的，因为在子类没有定义任何数据字段和方法
	CCtrlDockBar* pDockBar = (CCtrlDockBar*) m_pDockBar;

	// 强制立即使用delayShow/delayHide调用RecalcDelayShow()
	//避免IsVisible()问题

	AFX_SIZEPARENTPARAMS layout = {0};
	layout.hDWP = pDockBar->m_bLayoutQuery ? NULL : 
		::BeginDeferWindowPos( 1 );
	if(this->m_nStateFlags & (delayHide|delayShow))
		this->RecalcDelayShow(&layout);

	if (layout.hDWP != NULL)
		::EndDeferWindowPos(layout.hDWP);

	if (bHorz)
		return CSize(max(m_szMinHorz.cx, m_szHDocked.cx),
		max(m_szMinHorz.cy, m_szHDocked.cy));

	return CSize(max(m_szMinVert.cx, m_szVDocked.cx),
		max(m_szMinVert.cy, m_szVDocked.cy));
}

void CCtrlBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CCtrlBar::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (m_bTracking || IsFloating())
		return;

	if ((nHitTest >= HTSIZEFIRST) && (nHitTest <= HTSIZELAST))
		StartTracking(nHitTest, point); // sizing edge hit
}

void CCtrlBar::StartTracking( UINT nHitTest, CPoint point )
{
	//从此以后获得所有的鼠标输入焦点
	SetCapture();
	
	//状态的修改
	m_htEdge = nHitTest;
	m_bTracking = TRUE;

	//是否为横向停靠
	BOOL bHorz = IsHDocking();
	//是否为横向拉动窗口的大小
	BOOL bHorzTracking = m_htEdge == HTLEFT || m_htEdge == HTRIGHT;

	//保存一下原有的大小
	m_nTrackPosOld = bHorzTracking ? point.x : point.y;

	//计算拖动位置到中心的偏移
	CRect rcBar, rcEdge;
	GetWindowRect(rcBar);
	GetEdgeRect(rcBar, m_htEdge, rcEdge);
	m_nTrackEdgeOfs = m_nTrackPosOld -
		(bHorzTracking ? rcEdge.CenterPoint().x : rcEdge.CenterPoint().y);

	//拖动的最大最小值
	m_nTrackPosMin = m_nTrackPosMax = m_nTrackPosOld;

	//计算最小的宽度
	int nMinWidth = bHorz ? m_szMinHorz.cy : m_szMinVert.cx;

	//最大的宽度
	int nExcessWidth = (bHorz ? m_szHDocked.cy : m_szVDocked.cx) - nMinWidth;

	//控制条无法正大查过剩余框架客户区的大小
	//所以在这里需要手动调整
	CRect rcT;
	m_pDockSite->RepositionBars(0, 0xFFFF, AFX_IDW_PANE_FIRST,
		reposQuery, &rcT, NULL, TRUE);
	int nMaxWidth = bHorz ? rcT.Height() - 2 : rcT.Width() - 2;

	BOOL bTopOrLeft = m_htEdge == HTTOP || m_htEdge == HTLEFT;

	m_nTrackPosMin -= bTopOrLeft ? nMaxWidth : nExcessWidth;
	m_nTrackPosMax += bTopOrLeft ? nExcessWidth : nMaxWidth;
}
void CCtrlBar::StopTracking()
{
	//停止调整窗口的大小
	m_bTracking = FALSE;
	ReleaseCapture();

	m_pDockSite->DelayRecalcLayout();
}
void CCtrlBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{//在拖动窗口消息
		//转换鼠标位置的坐标系
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen);

		//更新窗口大小
		OnTrackUpdateSize(ptScreen);
	}

	CControlBar::OnMouseMove(nFlags, point);
}
void CCtrlBar::OnTrackUpdateSize(CPoint& point)
{
	ASSERT(!IsFloating());

	//是否为横向拖动
	BOOL bHorzTrack = m_htEdge == HTLEFT || m_htEdge == HTRIGHT;

	//要拖动的位置
	int nTrackPos = bHorzTrack ? point.x : point.y;
	nTrackPos = max(m_nTrackPosMin, min(m_nTrackPosMax, nTrackPos));

	//拖动单元大小
	int nDelta = nTrackPos - m_nTrackPosOld;

	if (nDelta == 0) return; //莫有改变位置

	//更新原有的位置
	m_nTrackPosOld = nTrackPos;

	//是否为横向停靠
	BOOL bHorz = IsHDocking();

	//计算新的大小
	CSize sizeNew = bHorz ? m_szHDocked : m_szVDocked;
	switch (m_htEdge)
	{
	case HTLEFT:    sizeNew -= CSize(nDelta, 0); break;
	case HTTOP:     sizeNew -= CSize(0, nDelta); break;
	case HTRIGHT:   sizeNew += CSize(nDelta, 0); break;
	case HTBOTTOM:  sizeNew += CSize(0, nDelta); break;
	}

	//调整实际的大小
	(bHorz ? this->m_szHDocked.cy : this->m_szVDocked.cx) = 
		bHorz ? sizeNew.cy : sizeNew.cx;
	

	//延迟一下，计量避免闪屏
	m_pDockSite->DelayRecalcLayout();
}

void CCtrlBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
		StopTracking();

	CControlBar::OnLButtonUp(nFlags, point);
}

void CCtrlBar::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_bTracking)
		StopTracking();

	CControlBar::OnRButtonDown(nFlags, point);
}

void CCtrlBar::OnCaptureChanged(CWnd *pWnd)
{
	if (m_bTracking && (pWnd != this))
		StopTracking();

	CControlBar::OnCaptureChanged(pWnd);
}
