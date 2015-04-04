// DiskScanDoc.cpp : implementation of the CDiskScanDoc class
//

#include "stdafx.h"
#include "DiskScan.h"

#include "DiskScanDoc.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "DiskScanView.h"
#include "DiskInfoView.h"
#include "SectorList.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDiskScanDoc

IMPLEMENT_DYNCREATE(CDiskDoc, CDataDoc)

BEGIN_MESSAGE_MAP(CDiskDoc, CDataDoc)
	//{{AFX_MSG_MAP(CDiskDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
 		ON_NOTIFY(NM_CLICK, IDC_DE_LIST, OnClickContextList)
 		ON_NOTIFY(NM_DBLCLK, IDC_DE_LIST, OnDbClickContextList)
		ON_BN_CLICKED(IDC_BTN_PRE_SECTOR, OnBnClickedPreSector)
		ON_BN_CLICKED(IDC_BTN_NEXT_SECTOR, OnBnClickedNextSector)
		ON_BN_CLICKED(IDC_BTN_FIRST_SECTOR, OnBnClickedFirstSector)
		ON_BN_CLICKED(IDC_BTN_LAST_SECTOR, OnBnClickedLastSector)
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskDoc construction/destruction

CDiskDoc::CDiskDoc()
{
	// TODO: add one-time construction code here
	m_pDisk = new Disk();
	m_liCurSec.QuadPart = 0;
}

CDiskDoc::~CDiskDoc()
{
	//�ͷŻ����Դ
	if (m_pDisk)
	{
		m_pDisk->CloseDisk();
		delete m_pDisk;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDiskScanDoc diagnostics

#ifdef _DEBUG
void CDiskDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDiskDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiskScanDoc commands

BOOL CDiskDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	CString		pDevName(lpszPathName);
	char		denName[DEVICE_NAME_LEN + 1] = {0};
	BOOL		bRes = FALSE;
	CView*		view;
	CString		strTemp;


	//�����Ҫ�������б�
	POSITION pos = GetFirstViewPosition();
	view = 	GetNextView(pos);

	this->m_pContentList = &((CChildFrm*)(view->GetParentFrame()))->m_DisList;
	//���б���г�ʼ��
	InitContentListHead();
	
	//Ҫ�򿪵��豸����
#ifdef  UNICODE
	UnicToMultyByte(lpszPathName , denName ,DEVICE_NAME_LEN + 1);
#else
	strcpy(denName , lpszPathName);
#endif
	//��ָ�����豸
	bRes = m_pDisk->OpenDisk(denName);

	if (!bRes)
	{//��ָ���豸ʧ��
		pDevName.LoadString(IDS_OPEN_FALIED);
		pDevName.Replace(STR_POS , lpszPathName);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , pDevName  , strTemp  , MB_OK|MB_ICONERROR);
	}else{
		//���豸�ɹ�
		this->m_strTitle = lpszPathName;
		//�豸�򿪳ɹ��Ļ� �������б��е�����
		SetLIstContent();

		//�豸������
		this->m_strDevName = lpszPathName;

		this->m_secList.AddSector( 0 , this->m_pDisk->GetSecCount());
		this->m_secList.m_strName = lpszPathName;
	}
	return bRes;
}

void CDiskDoc::SetLIstContent()
{
	LONG_INT	li; 
	int			nCnt = 0 , i = 0;
	USHORT		format = 0;
	CString		strTemp;

	//��ʼ�� �б��е�����
	nCnt = this->m_pDisk->GetPartCount();
	
	//  ID  �ļ���  ����	��ʼ������  ��������  ��С ������  �����
	for (i = 0 ; i <  nCnt ; ++i)
	{
		Disk::PDPart pDPart = m_pDisk->GetPart(i);
		//��ʽ
		format = pDPart->mType;
		
		strTemp.Format(_T("%d") , i);//ID
		m_pContentList->InsertItem( i  , strTemp ,1 );
		
		//�豸���������
		m_pContentList->SetItemText(i , 1  ,
			GetPartName(format ,m_pDisk->GetDevName() , pDPart->mOffset) );//����
		
		//�豸���������
		m_pContentList->SetItemText(i , 2  , GetPartFormatName(format));//����
		
		li = pDPart->mOffset;
		li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
		strTemp.Format(_T("%X") , li.LowPart);
		m_pContentList->SetItemText(i , 3  , strTemp);//��ʼɽ��
		
		li = pDPart->mSecCount;
		li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
		strTemp.Format(_T("%X") , li.LowPart);
		m_pContentList->SetItemText(i , 4  , strTemp);//��������
		
		li.QuadPart = pDPart->mSecCount.QuadPart * SECTOR_SIZE;
		strTemp = GetSizeToString(li);
		m_pContentList->SetItemText(i , 5  , strTemp);//��С
		
		if ( -1 != pDPart->mVolIndex )
		{//�Ƿ�Ϊһ���ļ�ϵͳ
			if (pDPart->mIsMainPart) //������?
				strTemp.LoadString(IDS_YES);	
			else
				strTemp.LoadString(IDS_NO);
			m_pContentList->SetItemText(i , 6  , strTemp);//��С
			
			if (pDPart->mIsActivity) //�����?
				strTemp.LoadString(IDS_YES);	
			else
				strTemp.LoadString(IDS_NO);
			m_pContentList->SetItemText(i , 7  , strTemp);//��С
			
			if (pDPart->mLogicalLetter)
			{
				strTemp = pDPart->mLogicalLetter;
				m_pContentList->SetItemText(i , 8  , strTemp);//��С
			}
		}
		}
}

void CDiskDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (this->m_pDisk )
	{
		m_pDisk->CloseDisk();
	}
	
	CDocument::OnCloseDocument();
}
void CDiskDoc::InitContentListHead()
{
	CString  strHead;
// 	//����ͼƬ�б�
// 	m_pContentList->SetImageList(&(((CMainFrame*)::AfxGetMainWnd())->m_wndImageList)
// 		,TVSIL_NORMAL);
	
	//  ID  �ļ���  ����	��ʵ������  ��������  ��С �������� �����? �߼�����
	strHead.LoadString(IDS_ID);
	m_pContentList->InsertColumn( 0, strHead , LVCFMT_LEFT, 30 );//������
	
	strHead.LoadString(IDS_FILE_NAME);
	m_pContentList->InsertColumn( 1, strHead , LVCFMT_LEFT, 90 );
	
	strHead.LoadString(IDS_TYPE);
	m_pContentList->InsertColumn( 2, strHead , LVCFMT_LEFT, 65 );
	
	strHead.LoadString(IDS_START_SECTOR);
	m_pContentList->InsertColumn( 3, strHead , LVCFMT_RIGHT, 90 );
	
	strHead.LoadString(IDS_SECTOR_COUNT);
	m_pContentList->InsertColumn( 4, strHead , LVCFMT_RIGHT, 90 );
	
	strHead.LoadString(IDS_SIZE);
	m_pContentList->InsertColumn( 5, strHead , LVCFMT_RIGHT, 75 );
	
	strHead.LoadString(IDS_MAIN_PART);
	m_pContentList->InsertColumn( 6, strHead , LVCFMT_CENTER, 50 );

	strHead.LoadString(IDS_ACTIVITY_PART);
	m_pContentList->InsertColumn( 7, strHead , LVCFMT_CENTER, 60 );
	

	strHead.LoadString(IDS_LOGICAL_DRIVER);
	m_pContentList->InsertColumn( 8, strHead , LVCFMT_CENTER, 60 );

}
void CDiskDoc::OnClickContextList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = NULL;
	int nItem = 0;
	CString strStartSec = _T("");
	LONG_INT liStart = {0};

	//��Ӧ�б�ĵ���¼�
	if (this->m_pContentList)
	{
		//���ѡ�е���	
		nItem = m_pContentList->GetSelectionMark();
		strStartSec = m_pContentList->GetItemText(nItem , 3 );
		this->m_liCurSec = HexStrToLONG_INT(strStartSec);

		LONG_INT secCnt;
		strStartSec = m_pContentList->GetItemText(nItem , 4 );
		secCnt = HexStrToLONG_INT(strStartSec);
		//������ʾ�б�
		SectorList secList;
		secList.AddSector(this->m_liCurSec  , secCnt);

		//��ȡ������������ 
		CString strTemp;
		strTemp = this->m_strTitle;
		strTemp +=(_T("->") + m_pContentList->GetItemText(nItem , 0 ));
		strTemp +=(_T(" ") + m_pContentList->GetItemText(nItem , 1 ));
		secList.m_strName = strTemp;
		this->SetSectorList(&secList);
		AfxGetMainWnd()->SendMessage(DMSG_SET_CUR_DATA_VIEW_NAME , WPARAM(&(strTemp)) , 0);

		this->UpdateAllViews(NULL);
	}
	
	*pResult = 0;
}



void CDiskDoc::OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION	pos			= NULL;
	int			nItem		= 0;
	CString		strStartSec = _T("");
	CString		strType		= _T("");
	CString		sTemp		= _T("");
	LONG_INT	liStart		= {0};
	DWORD		dwType		= 0;

	//�����Ѿ�����б�ؼ���ָ��
	ASSERT(NULL != m_pContentList);

	*pResult = 0;
	
	//��Ӧ�б�ĵ���¼�
	pos = m_pContentList->GetFirstSelectedItemPosition();
	if (pos == NULL){//û��ѡ���κ�����
		TRACE0("No items were selected!\n");
		return ;
	}
	
	//���ѡ�е���	
	nItem = m_pContentList->GetNextSelectedItem(pos);

	//���ѡ���������
	strType = m_pContentList->GetItemText(nItem , 2);

	//�Ƿ�ΪFAT32
	if (sTemp.LoadString(IDS_FAT32_PART) , 
		0 == sTemp.CompareNoCase(strType))
	{//ѡ�����FAT32����

		dwType = PART_FAT32;
	
	}else if (sTemp.LoadString(IDS_NTFS_PART) , 
		0 == sTemp.CompareNoCase(strType))
	{//ѡ�����NTFS����
		
		dwType = PART_NTFS;

	}else{
		//�������͵Ļ�ֻ�Ǽ򵥵���ʾһ��

		strStartSec = m_pContentList->GetItemText(nItem , 3 );
		this->m_liCurSec = HexStrToLONG_INT(strStartSec);
		this->UpdateAllViews(NULL);

		return ;
	}

// 	//��õ�ǰѡ��Ľڵ�
// 	Disk::PDPart pPart = this->m_pDisk->GetPart(nItem);

	//���豸����Ϣ����
	sTemp = this->m_pDisk->GetDevName() ;  //�豸������

	//���
	strStartSec.Format(_T(" %d ") , nItem );
	sTemp += _T(" ");
	sTemp += PN_INDEX;		//�������� ��Ӱ����
	sTemp += strStartSec;	//���������
	
	//ƫ��
	sTemp += PN_OFFSET;
	sTemp += _T(" ");		
	strStartSec = m_pContentList->GetItemText(nItem , 3 );  //��ʵ������
	sTemp += strStartSec;

	//�̷�
	strStartSec = m_pContentList->GetItemText(nItem , 8 );
	strStartSec.TrimLeft();
	strStartSec.TrimRight();
	if (0 != strStartSec.GetLength())
	{//���̷�
		sTemp += _T(" ");	
		sTemp += PN_LETTER;
		sTemp += _T(" ");	
		sTemp += strStartSec;
	}
	
	//������ܷ�����Ϣ ��Ҫ��һ���µ��ĵ�
	AfxGetMainWnd()->SendMessage( DMSG_OPEN_NEW_DOC , WPARAM(&sTemp)  , dwType);
}




// void CDiskDoc::GotUnpartableSecCont()
// {
// 	//��ʼ�� �б��е�����
// 	CString strTemp;
// 	USHORT format;
// 	LONG_INT li;
// 	POSITION  pos = NULL;
// 	CView* view = NULL;
// 	int i = 0;
// 
// 
// 	int nCnt = this->m_pDisk->GetPartCount();
// 	
// 	//  ID  �ļ���  ����	��ʼ������  ��������  ��С 
// 	for (i = 0 ; i <  nCnt ; ++i)
// 		if((format = m_pDisk->GetPartFormat(i)) == PART_UNPARTBLE)
// 			break;
// 
// 	if (i == nCnt) 	return ;  //û���ҵ����ɷ���Ŀռ�ڵ�
// 
// 	
// 	strTemp.Format(_T("%d") , i);//ID
// 	m_pContentList->InsertItem( i  , strTemp ,1 );
// 	
// 	m_pContentList->SetItemText(i , 1  ,
// 			GetPartName(format ,m_pDisk->GetDevName() , m_pDisk->GetPartOffset(i)) );//����
// 	//m_pContentList->SetItemText(i , 1  , GetPartFormatName(format));//����
// 	
// 	m_pContentList->SetItemText(i , 2  , GetPartFormatName(format));//����
// 	
// 	li = m_pDisk->GetPartOffset(i);
// 	li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
// 	strTemp.Format(_T("%X") , li.LowPart);
// 	m_pContentList->SetItemText(i , 3  , strTemp);//��ʼɽ��
// 	
// 	li = m_pDisk->GetPartSectorCount(i);
// 	li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
// 	strTemp.Format(_T("%X") , li.LowPart);
// 	m_pContentList->SetItemText(i , 4  , strTemp);//��������
// 	
// 	li.QuadPart = m_pDisk->GetPartSectorCount(i).QuadPart * SECTOR_SIZE;
// 	strTemp = GetSizeToString(li);
// 	m_pContentList->SetItemText(i , 5  , strTemp);//��С
// 
// 
// 	//�ı���ͼ�Ŀ�����ʾ������������
// 	pos = this->GetFirstViewPosition();
// 	while(pos != NULL)
// 	{
// 		view = this->GetNextView(pos);
// 		if (view->IsKindOf(RUNTIME_CLASS(CHexDataView)))
// 			((CHexDataView*)view)->SetSecCount(this->m_pDisk->GetSecCount());
// 	}
// }

LONG_INT CDiskDoc::GetSecCount()
{
	return this->m_pDisk->GetSecCount();
}

BOOL CDiskDoc::ReadData( void* buf , PLONG_INT offset  , BOOL isNext/* = TRUE */, DWORD size /*= SECTOR_SIZE*/ )
{
	if (isNext)
	{
		*offset = this->m_pCurSecList->FixToNextSector(*offset);
	}else{
		*offset = this->m_pCurSecList->FixToPreSector(*offset);
	}
	return DR_OK == this->m_pDisk->ReadData(buf , *offset , size) ;
}
BOOL CDiskDoc::GetUnPartSecCount(DWORD* secCnt)
{
	return this->m_pDisk->GetUnPartableSize() / SECTOR_SIZE;
}

CRuntimeClass* CDiskDoc::GetInofViewClass()
{
	return RUNTIME_CLASS(CDiskInfoView);
}

void CDiskDoc::OnBnClickedPreSector()
{
	/*AfxMessageBox(_T("��һ����"));*/
	ReSetSectorList();
	if (0 != this->m_liCurSec.QuadPart )
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		--this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::OnBnClickedNextSector()
{
/*	AfxMessageBox(_T("��һ����"));*/
	//��������
	LONG_INT liSecCnt = this->m_pDisk->GetSecCount();
	ReSetSectorList();
	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//��ǰ�������һ������
		++this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::OnBnClickedFirstSector()
{
	ReSetSectorList();
	/*AfxMessageBox(_T("��һ������"));*/
	if (0 != this->m_liCurSec.QuadPart )
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		this->m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::OnBnClickedLastSector()
{
/*	AfxMessageBox(_T("���һ������"));*/
	//��������
	ReSetSectorList();
	LONG_INT liSecCnt = this->m_pDisk->GetSecCount();

	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//��ǰ�������һ������
		this->m_liCurSec.QuadPart = liSecCnt.QuadPart - 1;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::SetCurFile( CString strPath )
{//�������ʽ���Բ����������
	ASSERT(FALSE);
}
