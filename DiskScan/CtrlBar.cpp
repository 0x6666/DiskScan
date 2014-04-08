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
	m_dwCtrlStyle	= 0;		//���Գ�ʼ��Ϊ��
//	m_cxEdge		= 1;		//�߿�Ĭ��Ϊ5������
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
	this->m_szVDocked.cx = 200;//����ͣ��ʱĬ��200�Ŀ��
	this->m_szHDocked.cy = 100;//����ͣ��ʱĬ��100�ĸ߶�
	this->m_dwCtrlStyle |= CTRL_SHOWEDGES;
	return bRes;
}
LRESULT CCtrlBar::OnInitDialog(UINT wParam, LONG lParam)
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	
	if (!UpdateData(FALSE))
		TRACE0("Warning: UpdateData failed during dialog init.\n");

	//��ʼ���ռ�
	return bRet;
}

CSize CCtrlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
// 	int temp = 0;
// 	//ͣ���˻��߸����Ļ�����Ĭ�ϵĴ�С
// 	if ((dwMode & LM_VERTDOCK) || (dwMode & LM_HORZDOCK))
// 	{
// 		if (dwMode & LM_STRETCH) //ͣ��ʱ����  Ī��ʵ�� 
// 			return CSize((dwMode & LM_HORZ) ? 32767 : m_szDocked.cx,
// 					     (dwMode & LM_HORZ) ? m_szDocked.cy : 32767);
// 		else {//ͣ���ڿ�ܵı���
// 			CRect rc;
// 			this->GetDockingFrame()->GetClientRect(&rc);
// 			//AfxGetMainWnd()
// 			if(dwMode & LM_HORZ)
// 			{//�����ͣ��
// 				//���㸸���ڵĴ�С�Է��ظ����ڵĴ�С��
// 
// 				return CSize(rc.Width()//- ::GetSystemMetrics(SM_CXEDGE)*2
// 					,m_szHDocked.cy);
// /*				return mHDocked;*/
// 			}else{//����ͣ��
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
// 	//�ڸ������������������
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

	if (dwMode & (LM_HORZDOCK | LM_VERTDOCK)) //�Ѿ�ͣ����?
	{
		if (0 == (dwMode & LM_STRETCH))
		{//ֻ�����ͣ���Ŀ�����

			CRect rc;  //��ͣ���Ŀ�ܵĴ�С
			/*this->GetDockingFrame()->GetWindowRect(&rc);*/
			this->GetDockingFrame()->GetClientRect(&rc);
			if(dwMode & LM_HORZ)  //�����ͣ���������ؿ�ܵĿ��
				return CSize(rc.Width() + 3  , m_szHDocked.cy); //�Ҳ�֪������ΪʲôҪ��3�����أ��Ҹо�����Ӧ���ǿ��Բ�Ҫ�ӵ�
			else //����ͣ��,��ܵĸ߶�
				return CSize(m_szVDocked.cx , rc.Height() );
		}

		return CControlBar::CalcDynamicLayout(nLength, dwMode);
	}

	//��õĶ�̬��С
	if (dwMode & LM_MRUWIDTH) return this->m_szFloat;
	//����LM_MRUWIDTH����ǰ�ĸ�����С
	if (dwMode & LM_COMMIT) return m_szFloat;

	//����ʱ�ı䴰�ڵĴ�С
	((dwMode & LM_LENGTHY) ? m_szFloat.cy : m_szFloat.cx) = nLength;

	m_szFloat.cx = max(m_szFloat.cx, m_szMinFloat.cx);
	m_szFloat.cy = max(m_szFloat.cy, m_szMinFloat.cy);

	return m_szFloat;
}

void CCtrlBar::OnSize(UINT nType, int cx, int cy)
{
	//������ȷ����DialogBar�еĿؼ�������
	CWnd* pWnd = GetWindow(GW_CHILD);
	if (pWnd != NULL)
	{
		if (m_bTracking || IsFloating())
		{
			//������Ҫ�ػ�
			pWnd->MoveWindow(0, 0, cx, cy , TRUE);
			this->m_pDockSite->DelayRecalcLayout();
		}else
		{//Ҫ����Ƶ�����ƴ��ڣ������ϴ�����˸�Ľ��  FALSE����Ҫ
			pWnd->MoveWindow(0, 0, cx, cy , FALSE);
		}
		//Ҫ��ֻ֤��һ���Ӵ��ڣ�����ͻ��������
		ASSERT(pWnd->GetWindow(GW_HWNDNEXT) == NULL);
	}
}
void CCtrlBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
	//������ڴ�С�ı�����ƶ��˵Ļ� ǿ������¼���ǿͻ���
	//������Ҫ�����򸡶�֮����ͣ���Ļ��ͱ߿�ͻ��С
	lpwndpos->flags |= SWP_FRAMECHANGED;

    CControlBar::OnWindowPosChanging(lpwndpos);

    //��õ�ǰͣ����ܵ�ID,�����жϵ�ǰͣ����λ��
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
{//���ͨ�������ж��Ƿ�����϶����ڵĴ�С
	CRect rcBar, rcEdge;
	GetWindowRect(rcBar);  //��õ�ǰDialogBar�Ĵ��ھ���

	if (!IsFloating())
	{//������Ǹ����Ļ� ����Ҫ��ʱ�Ƿ��� �߿���  ����Ϳͻ���
		for (int i = 0; i < 4; i++)
		{//����ÿһ���߿�  ,�ж�����һ��λ��
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

	//�Բ����ҽ����ĸ����� �� �����Ķ��ҵ�
	ASSERT(FALSE);
	return HTNOWHERE;
}

BOOL CCtrlBar::GetEdgeRect( CRect rcWnd, UINT  nHitTest, CRect& rcEdge )
{
	rcEdge = rcWnd;	//DialogBar�Ĵ��ھ���

	if (m_dwCtrlStyle & CTRL_SHOWEDGES)
		rcEdge.DeflateRect(1, 1);  //Ҫ��ʾ���ߵĻ� ��ԭ�еĴ�����Сһ������,���ڻ��Ʊ���

	BOOL bHorz = IsHDocking();  //�Ƿ�Ϊ����ͣ��

	switch (nHitTest)
	{
	case HTLEFT:  //��ȡ��ߵı߿�Χ 
		if (!(m_dwCtrlStyle & CTRL_EDGELEFT))
			return FALSE;	//������߿����϶����ڴ�С�Ļ���ֱ�ӷ���FALSE

		rcEdge.right = rcEdge.left +3 /*m_cxEdge*/;		//�߿�Ϊ3������
//		rcEdge.DeflateRect(0, bHorz ? m_cxEdge: 0); //����ͣ���Ļ� ҲҪ�۳����µı߿�
		break;

	case HTTOP:	//��ȡ�����ı߿�
		if (!(m_dwCtrlStyle & CTRL_EDGETOP))
			return FALSE;//���Ƕ��������϶����ڴ�С�Ļ���ֱ�ӷ���FALSE

		rcEdge.bottom = rcEdge.top +3 /*m_cxEdge*/;
//		rcEdge.DeflateRect(bHorz ? 0 : m_cxEdge, 0);
		break;

	case HTRIGHT://��ȡ�ұߵı߿�
		if (!(m_dwCtrlStyle & CTRL_EDGERIGHT))
			return FALSE;

		rcEdge.left = rcEdge.right - 3/*m_cxEdge*/;
//		rcEdge.DeflateRect(0, bHorz ? m_cxEdge: 0);
		break;

	case HTBOTTOM:  //��ȡ����ı߿�
		if (!(m_dwCtrlStyle & CTRL_EDGEBOTTOM))
			return FALSE;

		rcEdge.top = rcEdge.bottom -3 /*m_cxEdge*/;
//		rcEdge.DeflateRect(bHorz ? 0 : m_cxEdge, 0);
		break;

	default:		//�Բ�����ֻ�������ַ��� ������  ֻ���ĸ�����
		ASSERT(FALSE);
	}
	return TRUE;
}

void CCtrlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (IsFloating())
	{//���������ǵ�С��ܵĴ�С

		CFrameWnd* pFrame = GetParentFrame();  //��ǰDialogBar�������ܶ���

		if (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
		{
			DWORD dwStyle = ::GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
			if ((dwStyle & MFS_4THICKFRAME) != 0)
			{//�Ѿ���ֹ�������ܵ�����С
				pFrame->ModifyStyle(MFS_4THICKFRAME, 0); //ʹ֮���Ե�����С
				GetParent()->ModifyStyle(0, WS_CLIPCHILDREN);  //��ֹ�����ڶ��Ӵ��ڵĻ���
			}
		}
	}

	//����ͻ����Ĵ�С  ��ȥ��λ�����Լ�¼
	m_dwCtrlStyle &= ~CTRL_EDGEALL;


	//����ͻ����Ĵ�С,ȷ����һ�߿����϶����ڵĴ�С
	CRect rc(lpncsp->rgrc[0]);
	//�Ƚ�������Сһ��,����һ���϶����΢�ÿ�һ�㣬������2�����ظ��������ڵ�
	rc.DeflateRect(3, 5, 3, 3); 
	//���Ǹ����Ļ��͸�������һ���Ŀ�϶��
	if (m_nDockBarID != AFX_IDW_DOCKBAR_FLOAT)
		rc.DeflateRect(2, 0, 2, 2);
	
	
	//�ڿ����϶���λ��������5�����صĿ��
	switch(m_nDockBarID)
	{
	case AFX_IDW_DOCKBAR_TOP:  //��ǰͣ���ڶ���
		m_dwCtrlStyle |= CTRL_EDGEBOTTOM;   //����ڵײ������϶����ڵĴ�С
		rc.DeflateRect( 3 , 0 , 0 , 0 );
		break;
	case AFX_IDW_DOCKBAR_BOTTOM://�ײ�
		m_dwCtrlStyle |= CTRL_EDGETOP;
		rc.DeflateRect( 3 , 0 , 0 , 0 );
		break;
	case AFX_IDW_DOCKBAR_LEFT://���
		m_dwCtrlStyle |= CTRL_EDGERIGHT;
		rc.DeflateRect( 0 , 3 , 0 , 0 );
		break;
	case AFX_IDW_DOCKBAR_RIGHT://�ұ�
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
// 	//�Ƚ�������Сһ��,����һ���϶����΢�ÿ�һ�㣬������2�����ظ��������ڵ�
// 	rc.DeflateRect(3, 5, 3, 3); 
// 
// 	//���Ǹ����Ļ��͸�������һ���Ŀ�϶��
// 	if (nDockBarID != AFX_IDW_DOCKBAR_FLOAT)
// 		rc.DeflateRect(2, 0, 2, 2);
// 
// 
// 	//�ڿ����϶���λ��������5�����صĿ��
// 	switch(nDockBarID)
// 	{
// 	case AFX_IDW_DOCKBAR_TOP:  //��ǰͣ���ڶ���
// 		m_dwCtrlStyle |= CTRL_EDGEBOTTOM;   //����ڵײ������϶����ڵĴ�С
// 		rc.DeflateRect( 5 , 0 , 0 , 0 );
// 		break;
// 	case AFX_IDW_DOCKBAR_BOTTOM://�ײ�
// 		m_dwCtrlStyle |= CTRL_EDGETOP;
// 		rc.DeflateRect( 5 , 0 , 0 , 0 );
// 		break;
// 	case AFX_IDW_DOCKBAR_LEFT://���
// 		m_dwCtrlStyle |= CTRL_EDGERIGHT;
// 		rc.DeflateRect( 0 , 5 , 0 , 0 );
// 		break;
// 	case AFX_IDW_DOCKBAR_RIGHT://�ұ�
// 		m_dwCtrlStyle |= CTRL_EDGELEFT;
// 		rc.DeflateRect( 0 , 5 , 0 , 0 );
// 		break;
// 	}
// 
// 	*pRc = rc;
// }

void CCtrlBar::OnNcPaint()
{
	//��ô��ڵ�DC  ���ڻ��Ʒǿͻ���
	CWindowDC dc(this);

	//ȡ���ͻ���
	CRect rcClient, rcBar;
	GetClientRect(rcClient);	//��ÿͻ����Ĵ�С
	ClientToScreen(rcClient);	//���ͻ����ľ���ץ������Ļ
	GetWindowRect(rcBar);		//���ڵľ���

	//���������ζ�Ӧ��"�ͻ���"������ ���ڻ��Ʒǿͻ����ı߿�������Ķ���
	//ֻ���ƶ��������Σ����ı��С���ƶ���rcClient����rcBar�ÿͻ���
	rcClient.OffsetRect(-rcBar.TopLeft());
	rcBar.OffsetRect(-rcBar.TopLeft());

	//����һ���뵱ǰ�����豸���������Ӧ���豸������ ���Ա��ڻ��Ʋ���
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);

	//����һ��λͼ�û��Ʊ߿�
	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, rcBar.Width(), rcBar.Height());
	CBitmap* pOldBm = mdc.SelectObject(&bm);

	//�ڷǿͻ������Ʊ߿�(λ�ô��ڵ�һ��������һ����һ����ɫ��)
	CRect rcDraw = rcBar;
	DrawBorders(&mdc, rcDraw); 

	//�õ�ǰ������ı���ˢ������ǿͻ�������
	mdc.FillRect(rcDraw, CBrush::FromHandle(
		(HBRUSH) GetClassLong(m_hWnd, GCL_HBRBACKGROUND)));  

	if (m_dwCtrlStyle & CTRL_SHOWEDGES)
	{//��Ҫ��ʾ�߿򣬴˱߿�Ǳȱ߿�  �������Լ���������һ��"�߿�����"
		CRect rcEdge;//����
		for (int i = 0; i < 4; i++)
		{
			if (GetEdgeRect(rcBar, GetEdgeHTCode(i), rcEdge))
			{//���ĸ����ϻ���3d�ı߿�
				mdc.Draw3dRect(rcEdge,
					::GetSysColor(COLOR_BTNHIGHLIGHT),
					::GetSysColor(COLOR_BTNSHADOW));
			}
		}
	}

	//NcPaintGripper(&mdc, rcClient);

	//�ĵ��ͻ���������  �ͻ��������������
	dc.IntersectClipRect(rcBar); //����ǰ���ڵľ��н��вü�
	dc.ExcludeClipRect(rcClient);//�ü����ͻ���
	
	//���Ѿ������ܸߵ�λͼ���ʣ�µ�����(�ǿͻ���)
	dc.BitBlt(0, 0, rcBar.Width(), rcBar.Height(), &mdc, 0, 0, SRCCOPY);

	//��Դ���ͷ��뻹ԭ
	ReleaseDC(&dc);
	mdc.SelectObject(pOldBm);
	bm.DeleteObject();
	mdc.DeleteDC();
}

CSize CCtrlBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
{
	if (bStretch) 
	{//�������� ����һ��ͣ���Ŀ�����
		if (bHorz)
			return CSize(32767, m_szHDocked.cy);
		else
			return CSize(m_szVDocked.cx, 32767);
	}

	//������ǿ�Ƹ����ָ��ת���������ָ�룬
	//��������û����ģ���Ϊ������û�ж����κ������ֶκͷ���
	CCtrlDockBar* pDockBar = (CCtrlDockBar*) m_pDockBar;

	// ǿ������ʹ��delayShow/delayHide����RecalcDelayShow()
	//����IsVisible()����

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
	//�Ӵ��Ժ������е�������뽹��
	SetCapture();
	
	//״̬���޸�
	m_htEdge = nHitTest;
	m_bTracking = TRUE;

	//�Ƿ�Ϊ����ͣ��
	BOOL bHorz = IsHDocking();
	//�Ƿ�Ϊ�����������ڵĴ�С
	BOOL bHorzTracking = m_htEdge == HTLEFT || m_htEdge == HTRIGHT;

	//����һ��ԭ�еĴ�С
	m_nTrackPosOld = bHorzTracking ? point.x : point.y;

	//�����϶�λ�õ����ĵ�ƫ��
	CRect rcBar, rcEdge;
	GetWindowRect(rcBar);
	GetEdgeRect(rcBar, m_htEdge, rcEdge);
	m_nTrackEdgeOfs = m_nTrackPosOld -
		(bHorzTracking ? rcEdge.CenterPoint().x : rcEdge.CenterPoint().y);

	//�϶��������Сֵ
	m_nTrackPosMin = m_nTrackPosMax = m_nTrackPosOld;

	//������С�Ŀ��
	int nMinWidth = bHorz ? m_szMinHorz.cy : m_szMinVert.cx;

	//���Ŀ��
	int nExcessWidth = (bHorz ? m_szHDocked.cy : m_szVDocked.cx) - nMinWidth;

	//�������޷�������ʣ���ܿͻ����Ĵ�С
	//������������Ҫ�ֶ�����
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
	//ֹͣ�������ڵĴ�С
	m_bTracking = FALSE;
	ReleaseCapture();

	m_pDockSite->DelayRecalcLayout();
}
void CCtrlBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{//���϶�������Ϣ
		//ת�����λ�õ�����ϵ
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen);

		//���´��ڴ�С
		OnTrackUpdateSize(ptScreen);
	}

	CControlBar::OnMouseMove(nFlags, point);
}
void CCtrlBar::OnTrackUpdateSize(CPoint& point)
{
	ASSERT(!IsFloating());

	//�Ƿ�Ϊ�����϶�
	BOOL bHorzTrack = m_htEdge == HTLEFT || m_htEdge == HTRIGHT;

	//Ҫ�϶���λ��
	int nTrackPos = bHorzTrack ? point.x : point.y;
	nTrackPos = max(m_nTrackPosMin, min(m_nTrackPosMax, nTrackPos));

	//�϶���Ԫ��С
	int nDelta = nTrackPos - m_nTrackPosOld;

	if (nDelta == 0) return; //Ī�иı�λ��

	//����ԭ�е�λ��
	m_nTrackPosOld = nTrackPos;

	//�Ƿ�Ϊ����ͣ��
	BOOL bHorz = IsHDocking();

	//�����µĴ�С
	CSize sizeNew = bHorz ? m_szHDocked : m_szVDocked;
	switch (m_htEdge)
	{
	case HTLEFT:    sizeNew -= CSize(nDelta, 0); break;
	case HTTOP:     sizeNew -= CSize(0, nDelta); break;
	case HTRIGHT:   sizeNew += CSize(nDelta, 0); break;
	case HTBOTTOM:  sizeNew += CSize(0, nDelta); break;
	}

	//����ʵ�ʵĴ�С
	(bHorz ? this->m_szHDocked.cy : this->m_szVDocked.cx) = 
		bHorz ? sizeNew.cy : sizeNew.cx;
	

	//�ӳ�һ�£�������������
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
