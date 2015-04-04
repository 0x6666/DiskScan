// DataDoc.cpp : implementation file
//

#include "stdafx.h"
#include "diskscan.h"
#include "DataDoc.h"
#include "InfoView.h"
#include "DiskScanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataDoc

IMPLEMENT_DYNCREATE(CDataDoc, CDocument)

CDataDoc::CDataDoc()
: m_pContentList(NULL)
, m_pHexView(NULL)
{
	m_pCurSecList = &m_secList;
}

//BOOL CDataDoc::OnNewDocument()
//{
//	if (!CDocument::OnNewDocument())
//		return FALSE;
//	return TRUE;
//}

CDataDoc::~CDataDoc()
{
	if (NULL != m_pCurSecList && m_pCurSecList != &m_secList)
	{
		delete m_pCurSecList;
	}
}


BEGIN_MESSAGE_MAP(CDataDoc, CDocument)
	//{{AFX_MSG_MAP(CDataDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
// 		ON_NOTIFY(NM_CLICK, IDC_DE_LIST, OnClickContextList)
// 		ON_NOTIFY(NM_DBLCLK, IDC_DE_LIST, OnDbClickContextList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDataDoc diagnostics

#ifdef _DEBUG
void CDataDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDataDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CDataDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::SetTitle(m_strTitle);
}

/////////////////////////////////////////////////////////////////////////////
// CDataDoc serialization

void CDataDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

LONG_INT CDataDoc::GetCurSec(void)
{
	return m_liCurSec;
}
void CDataDoc::InitContentListHead()
{
	
}

LONG_INT CDataDoc::GetSecCount()
{
	LONG_INT li = {0};
	return li;
}

BOOL CDataDoc::ReadData( void* buf , PLONG_INT offset , BOOL isNext/* = TRUE */, DWORD size/* = SECTOR_SIZE */)
{
	return FALSE;
}

void CDataDoc::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU /*= FALSE*/ )
{
	//什么事都不要做了就可以了
	//CDocument::SetPathName(lpszPathName , FALSE);
}

CString CDataDoc::GetDevName()
{
	return m_strDevName;
}

CRuntimeClass* CDataDoc::GetInofViewClass()
{
	return RUNTIME_CLASS(CInfoView);
}

void CDataDoc::ChangeCurSector( LONG_INT liCurSector , CView* pSender )
{
	this->m_liCurSec = liCurSector;
	UpdateAllViews( pSender );
}

void CDataDoc::SetSel( LONG_INT start , LONG_INT end )
{
	CHexDataView* pView = GetHexDataView();

	if (NULL == pView)
	{//没有找到十六进制视图
		return ;
	}

	pView->SetSel(start , end);

}

void CDataDoc::SetCurSector( LONG_INT sector )
{
	this->m_liCurSec = sector;

	//更新所有的视图
	UpdateAllViews(NULL);
}

void CDataDoc::SetCurFile( CString strPath )
{

}

void CDataDoc::ReSetSectorList()
{
	if ((m_pCurSecList != NULL))
	{
		if (m_pCurSecList == &m_secList)
		{//不需要修改
			return ;
		}
		delete m_pCurSecList;
	}
	this->m_pCurSecList = &m_secList;

	CHexDataView* pView = GetHexDataView();

	if (NULL == pView)
	{//没有找到十六进制视图
		return ;
	}

	pView->SetStartSector(m_pCurSecList->GetStartSector());

	AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(m_pCurSecList->m_strName)) , 0);
}

BOOL CDataDoc::SetSectorList( SectorList* pSecList )
{
	if (NULL == pSecList)
	{
		return FALSE;
	}
	if ((m_pCurSecList != NULL) && (m_pCurSecList != &m_secList) )
	{
		if (0 == pSecList->m_strName.CompareNoCase(m_pCurSecList->m_strName))
		{//不需要修改
			return FALSE;
		}
		delete m_pCurSecList;
	}
	this->m_pCurSecList = pSecList->GetCopy();


	CHexDataView* pView = GetHexDataView();

	if (NULL == pView)
	{//没有找到十六进制视图
		return FALSE;
	}

	pView->SetStartSector(m_pCurSecList->GetStartSector());

	AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(m_pCurSecList->m_strName)) , 0);

	return TRUE;
}

BOOL CDataDoc::SetSectorListNoCopy( SectorList* pSecList )
{
	if (NULL == pSecList)
	{
		return FALSE;
	}
	if ((m_pCurSecList != NULL) && (m_pCurSecList != &m_secList))
	{//需要删除原有的扇区链表
		if (0 == pSecList->m_strName.CompareNoCase(m_pCurSecList->m_strName))
		{//不需要修改
			return FALSE;
		}
		delete m_pCurSecList;
	}
	this->m_pCurSecList = pSecList;

	CHexDataView* pView = GetHexDataView();

	ASSERT(pView);

	pView->SetStartSector(m_pCurSecList->GetStartSector());

	AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(m_pCurSecList->m_strName)) , 0);

	return TRUE;
}

CHexDataView* CDataDoc::GetHexDataView()
{
	if (NULL == m_pHexView)
	{//获得十六进制视图 
		POSITION pos = this->GetFirstViewPosition();
		CView*	p;
		while(NULL != pos)
		{
			p = this->GetNextView(pos);
			if (p->IsKindOf(RUNTIME_CLASS(CHexDataView)))
			{
				m_pHexView = (CHexDataView*)p;
				break;
			}				
		}
	}
	return m_pHexView;
}

// void CDataDoc::OnClickContextList( NMHDR* pNMHDR, LRESULT* pResult )
// {
// 	*pResult = 0;
// }
// 
// void CDataDoc::OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult) 
// {
// 	//列表的额双击事件
// 	*pResult = 0;
// }
/////////////////////////////////////////////////////////////////////////////
// CDataDoc commands

// BOOL CDataDoc::OnOpenDocument(LPCTSTR lpszPathName)
// {
// 	if (!CDocument::OnOpenDocument(lpszPathName))
// 		return FALSE;
// 
// 	// TODO:  ?ú′?ìí?ó?ú×¨ó?μ?′′?¨′ú??
// 
// 	return TRUE;
// }
