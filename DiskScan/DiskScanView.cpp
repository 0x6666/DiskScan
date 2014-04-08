// DiskScanView.cpp : implementation of the CDiskScanView class
//

#include "stdafx.h"
#include "DiskScan.h"

#include "DiskScanDoc.h"
#include "DiskScanView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiskScanView

IMPLEMENT_DYNCREATE(CHexDataView, CFormView)

BEGIN_MESSAGE_MAP(CHexDataView, CFormView)
	//{{AFX_MSG_MAP(CDiskScanView)
	ON_WM_SIZE()
	ON_MESSAGE(DATA_CTRL_MSG , OnGetData)
	ON_MESSAGE(DATA_CHANGE_WIDTH , OnChangeWidth)
	ON_MESSAGE(DATA_CHANGE_SECTOR ,OnChangeSector)
/*	ON_MESSAGE(DMSG_GOT_UNPARTABLE_SEC , OnGotUnPartableSecCnt)*/
	//}}AFX_MSG_MAP
//	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskScanView construction/destruction

CHexDataView::CHexDataView()
	: CFormView(CHexDataView::IDD)
{
	//{{AFX_DATA_INIT(CDiskScanView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_wndData = NULL;
//	m_pDisk = NULL;
}

CHexDataView::~CHexDataView()
{
	if (NULL != m_wndData)
	{
		m_wndData->DestroyWindow();
		delete m_wndData;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDiskScanView diagnostics

#ifdef _DEBUG
void CHexDataView::AssertValid() const
{
	CFormView::AssertValid();
}

void CHexDataView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDataDoc* CHexDataView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDataDoc)));
	return (CDataDoc*)m_pDocument;
}
#endif //_DEBUG
#ifndef _DEBUG  // debug version in DiskScanView.cpp
CDataDoc* CHexDataView::GetDocument()
{ 
	return (CDataDoc*)m_pDocument;
}
#endif

void CHexDataView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiskScanView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CHexDataView::OnInitialUpdate()
{
	CDataDoc* pDoc = this->GetDocument();

	m_wndData = new DataWnd;
	//������ڻ�û�����Ļ������ڴ���
	if(!IsWindow(this->m_wndData->GetSafeHwnd())) {
		RECT  r;
		this->GetClientRect(&r);
		m_wndData->Create(r , this);
		m_wndData->ShowWindow(SW_SHOW);
	}

	//��ʼ���������Ĺ�����Χ
	SetScrollSizes(MM_TEXT , CSize(m_wndData->GetMinWidth() - 10 , 10));


	//��ʼ��һЩ��Ҫ�ĳ�ʼ������
	//����Ҫ��ʾ������
	m_wndData->SetSecCount(pDoc->GetSecCount());
	//this->m_wndData->SetDataSec(start , m_pDisk->GetSecCount() );
	
// 	if (pDoc->IsKindOf(RUNTIME_CLASS(CDiskDoc)))
// 	{//����Ǹ�������ʾ���ݵĻ���Ҫ���� ���ɷ���ռ�Ĵ�С
// 		//�������㲻�ɷ���ռ�Ĵ�С�߳�
// 		CloseHandle(::CreateThread(NULL , NULL , 
// 			GetUnPartableSectorCntThread , this , 0 , 0 ));
// 	}

	CFormView::OnInitialUpdate();   //�˺����ᴥ�� ����OnUpdate  ������Щ������Ҫ�ڴ�֮ǰִ��
	ResizeParentToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CDiskScanView message handlers

void CHexDataView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	//֪ͨ�����������µ�����С
	if(m_wndData && ::IsWindow(m_wndData->GetSafeHwnd())){
		m_wndData->SetWindowPos(NULL ,  0 , 0 , m_wndData->GetMinWidth() ,
			cy , SWP_NOMOVE | SWP_NOREDRAW);
//		::MoveWindow(m_wndData->GetSafeHwnd() , 0 , 0 , m_wndData->GetMinWidth() ,cy , TRUE);
		//	this->mData->SendMessage(WM_SIZE , nType ,MAKELONG(cx , cy));
	}
}

int CHexDataView::GetMinWidth()
{
	return this->m_wndData->GetMinWidth();
}

void CHexDataView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	if(this == pSender)  //�Լ�֪ͨ�ı�ģ�����
		return ;
	
	//��ù������ĵ�����
	CDataDoc* pDoc = this->GetDocument();
	LONG_INT start = {0};
	LONG_INT cnt = {0};
	ASSERT(pDoc != NULL);

	//��ʾ��ǰҪ��ʾ������
	this->m_wndData->SetCurSec(pDoc->GetCurSec());
	this->m_wndData->SetStartSector(pDoc->m_pCurSecList->GetStartSector());

}


LRESULT CHexDataView::OnGetData(WPARAM isNext, LPARAM ptr)
{
	CDataDoc * pDoc = this->GetDocument();
	DataWnd::PDATA_BUF buf = (DataWnd::PDATA_BUF)ptr;

	//��ȡָ��������
	if(! pDoc->ReadData((void*)(buf->mBuf) , &(buf->mOff)  , isNext, SEC_SIZE))
		buf->mOff.QuadPart = -1;
	
	return 0 ;
}
LRESULT CHexDataView::OnChangeWidth(WPARAM width, LPARAM ptr)
{
	//�ӿؼ��Ĵ�С�ı���

	//���ù������Ĺ�����Χ
	SetScrollSizes(MM_TEXT , CSize(width - 10 , 50));

	//���ù��������������������
	((CChildFrm*)this->GetParentFrame())->m_wndSplitter.SetMinWidth(width);

	return 0;
}

void CHexDataView::SetSecCount(LONG_INT cnt)
{
	this->m_wndData->SetSecCount(cnt);
	//this->SetSecCount(cnt);
}

LRESULT CHexDataView::OnChangeSector( WPARAM low , LPARAM  hig)
{
	CDataDoc* pDoc = GetDocument();
	LONG_INT liSecrot	= {0};
	liSecrot.LowPart	= low;
	liSecrot.HighPart	= hig;

	pDoc->ChangeCurSector(liSecrot , this);

	return 0;
}

void CHexDataView::SetSel( LONG_INT start, LONG_INT end )
{
	this->m_wndData->SetSel(start , end);
}

// LRESULT CHexDataView::OnGotUnPartableSecCnt(WPARAM cnt, LPARAM ptr)
// {
// 	//AfxMessageBox(_T("fdas"));
// 	CDataDoc* pDoc = this->GetDocument();
// 
// 	//�����ĵ��Ĵ�����
// 	if (pDoc->IsKindOf(RUNTIME_CLASS(CDiskDoc)))
// 		((CDiskDoc*)pDoc)->GotUnpartableSecCont();
// 	
// 	return 0;
// }
//BOOL CHexDataView::DestroyWindow()
//{
// 	if (NULL != m_wndData)
// 	{
// 		m_wndData->DestroyWindow();
// 		delete m_wndData;
// 	}
//	return CFormView::DestroyWindow();
//}

//void CHexDataView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
//{
//	CFormView::OnActivate(nState, pWndOther, bMinimized);
//
//	CDataDoc * pDoc = this->GetDocument();
//	if (NULL == pDoc) return;
//	AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(pDoc->m_pCurSecList->m_strName)) , 0);
//}

void CHexDataView::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);

	CDataDoc * pDoc = this->GetDocument();
	if (NULL == pDoc) return;
	AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(pDoc->m_pCurSecList->m_strName)) , 0);
}

void CHexDataView::SetStartSector( LONG_INT sec )
{
	this->m_wndData->SetStartSector(sec);
}
