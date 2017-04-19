// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "DiskScan.h"
#include "InfoView.h"
#include "DiskScanView.h"
#include "ChildFrm.h"
#include "MainFrm.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrm, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame) IDC_NTFS_DIR_LIST
	ON_WM_CREATE()
	ON_COMMAND(IDC_NTFS_DIR_LIST, OnContentList)
	ON_COMMAND(IDC_DISK_CONTENT_LIST, OnContentList)
	ON_COMMAND(IDC_FAT3_DIR_LIST, OnContentList)
	ON_UPDATE_COMMAND_UI(IDC_CONTENT_LIST, OnUpdateContentList)
	ON_UPDATE_COMMAND_UI(IDC_DISK_CONTENT_LIST, OnUpdateContentList)
	ON_UPDATE_COMMAND_UI(IDC_FAT3_DIR_LIST, OnUpdateContentList)
	ON_UPDATE_COMMAND_UI(IDC_NTFS_DIR_LIST, OnUpdateContentList)
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_POS_PARAENT_DIR, &CChildFrm::OnPosParaentDir)
//ON_WM_ACTIVATE()
//ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrm::CChildFrm()
{
	// TODO: add member initialization code here
	
}

CChildFrm::~CChildFrm()
{
}

BOOL CChildFrm::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style =  WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;


	return TRUE;
}

void CChildFrm::ActivateFrame(int nCmdShow)
{
	// TODO: Modify this function to change how the frame is activated.
	//打开时最大化
	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWnd::ActivateFrame(nCmdShow);

	//设置分切窗口的最大宽度
	CHexDataView* pDSV =  (CHexDataView*)this->m_wndSplitter.GetPane( 0 , 0);
  	this->m_wndSplitter.SetMinWidth(pDSV->GetMinWidth());
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrm::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrm::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrm::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CSize size;
	RECT rc ;
	::GetClientRect(this->GetSafeHwnd() , &rc);
	size = CSize(rc.right - ::GetSystemMetrics(SM_CXEDGE)*2
		, rc.bottom - rc.top - ::GetSystemMetrics(SM_CYMENUCHECK) - ::GetSystemMetrics(SM_CYEDGE));

	//创建磁盘列别Bar
	if (!m_DisBar.Create(this, IDD_DVE_LIST, CBRS_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY,
		IDD_DVE_LIST ,size))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}

//	m_DisBar.


	if (!m_DisList.Create(LVS_SHAREIMAGELISTS | LVS_REPORT | WS_BORDER |WS_TABSTOP|WS_CHILD|WS_VISIBLE
		,rc ,&m_DisBar , IDC_DE_LIST))
	{
		TRACE0("Failed to create List Ctrl\n");
		return -1;      // fail to create
	}
	//设置一下列表的属性
	DWORD dwStyle = m_DisList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮
	dwStyle |= LVS_EX_GRIDLINES;	//网格线
	m_DisList.SetExtendedStyle(dwStyle); //设置扩展风格


	//设置设备列表可停靠
	m_DisBar.SetBarStyle(CBRS_ALIGN_TOP|CBRS_GRIPPER |\
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_DisBar.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
 	EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
    DockControlBar(&m_DisBar , AFX_IDW_DOCKBAR_TOP);
 	ShowControlBar(&m_DisBar, TRUE, FALSE);

/*	InitCtrl();*/
	return 0;
}

void CChildFrm::OnContentList() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_DisBar, !m_DisBar.IsVisible(), FALSE);
	
}

void CChildFrm::OnUpdateContentList(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_DisBar.IsVisible());	

}

BOOL CChildFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	///////一行两列
	if(!m_wndSplitter.CreateStatic(this , 1 , 2))
		return FALSE;
	 
	/////第0行 0列 
	if(!this->m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CHexDataView),
		CSize(638,300),pContext))
		return FALSE;
	

	if(!pContext->m_pCurrentDoc->IsKindOf(RUNTIME_CLASS(CDataDoc)))
		ASSERT(FALSE);

	//////第0行 1列  
	if(!this->m_wndSplitter.CreateView( 0 , 1 , 
		((CDataDoc*)pContext->m_pCurrentDoc)->GetInofViewClass()
		,CSize(270,200),pContext))
		return FALSE;
	
	 return TRUE;
}

// 获得用于显示信息的视图类
// 
// CRuntimeClass* CChildFrm::GetInofViewClass(void)
// {
// 	return RUNTIME_CLASS(CInfoView);
// }

BOOL CChildFrm::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle , const RECT& rect , CMDIFrameWnd* pParentWnd , CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	//去掉横向或者水平改变窗口大小时重绘窗口的属性  仿佛没用
	dwStyle &= ~(CS_VREDRAW|CS_HREDRAW);

	return CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}

//void CChildFrm::OnPosParaentDir()
//{
//	AfxMessageBox(_T("定位父目录"));
//}

//void CChildFrm::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
//{
//	CMDIChildWnd::OnActivate(nState, pWndOther, bMinimized);
//
//	if (nState == WA_CLICKACTIVE || nState == WA_ACTIVE )
//	{//窗口激活了
//		CDataDoc * pDoc = (CDataDoc *)(((CMainFrame*)AfxGetMainWnd())->GetActiveDocument());
//		if (NULL == pDoc) return;
//		AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(pDoc->m_pCurSecList->m_strName)) , 0);
//	}
//}

//BOOL CChildFrm::PreTranslateMessage(MSG* pMsg)
//{
//
//	return CMDIChildWnd::PreTranslateMessage(pMsg);
//}

//void CChildFrm::OnSetFocus(CWnd* pOldWnd)
//{
//	CMDIChildWnd::OnSetFocus(pOldWnd);
//
//	//if (nState == WA_CLICKACTIVE || nState == WA_ACTIVE )
//	//{//窗口激活了
//	CView* pView = ((CMainFrame*)AfxGetMainWnd())->GetActiveView();
//	if (NULL == pView) return;
//	CDataDoc * pDoc = (CDataDoc *)(pView->GetDocument());
//	if (NULL == pDoc) return;
//	AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(pDoc->m_pCurSecList->m_strName)) , 0);
//	//}
//}
