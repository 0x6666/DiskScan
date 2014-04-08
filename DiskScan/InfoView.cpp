// InfoView.cpp : implementation file
//

#include "stdafx.h"
#include "DiskScan.h"
#include "InfoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma warning(disable:4996)
/////////////////////////////////////////////////////////////////////////////
// CInfoView

IMPLEMENT_DYNCREATE(CInfoView, CFormView)

CInfoView::CInfoView()
	: CFormView(CInfoView::IDD)
{
	//{{AFX_DATA_INIT(CInfoView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CInfoView::CInfoView( UINT nIDTemplate )
	:CFormView(nIDTemplate)
{
	//���û��๹�췽��
}

CInfoView::~CInfoView()
{
}

void CInfoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoView, CFormView)
	//{{AFX_MSG_MAP(CInfoView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
//	ON_WM_CREATE()
//ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoView diagnostics

#ifdef _DEBUG
void CInfoView::AssertValid() const
{
	CFormView::AssertValid();
}

void CInfoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}




#endif //_DEBUG

int CInfoView::RePositionCtrl()
{
	return 0;
}

void CInfoView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	this->m_Font.DeleteObject();
}


void CInfoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	//������ʾ��Ϣ������
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);  //�ֻ��ĵ�ǰ��ͼ��Ĭ������ ��Ϊġ��
	_tcscpy(lf.lfFaceName , _T("����"));
	lf.lfHeight = 16;
	this->m_Font.DeleteObject();
	this->m_Font.CreateFontIndirect(&lf);
	this->SetFont(&m_Font , FALSE);

	//��õ�ǰ������ַ���ʾ��С
	GetTextExtentPoint32(this->GetDC()->GetSafeHdc() ,_T("A") , 1 , &m_szChar);
}

BOOL CInfoView::PreTranslateMessage(MSG* pMsg)
{
	//Ϊ���õ�ǰ����������еĿؼ�����ʹ�� ctrl+C������Ҫ���������Ϣ����
	UINT  nCode = pMsg->wParam;
	if (pMsg->message == WM_KEYDOWN)
	{   
		if ( (nCode == _T('A') || nCode == _T('C') || nCode == _T('X') || nCode == _T('V')) 
			 &&(::GetKeyState(VK_CONTROL) & 0x8000) )
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;
		}
	}
	return CFormView::PreTranslateMessage(pMsg);
}
