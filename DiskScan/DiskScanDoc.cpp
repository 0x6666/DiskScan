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
	//释放获得资源
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


	//获得需要的内容列表
	POSITION pos = GetFirstViewPosition();
	view = 	GetNextView(pos);

	this->m_pContentList = &((CChildFrm*)(view->GetParentFrame()))->m_DisList;
	//对列表进行初始化
	InitContentListHead();
	
	//要打开的设备名字
#ifdef  UNICODE
	UnicToMultyByte(lpszPathName , denName ,DEVICE_NAME_LEN + 1);
#else
	strcpy(denName , lpszPathName);
#endif
	//打开指定的设备
	bRes = m_pDisk->OpenDisk(denName);

	if (!bRes)
	{//打开指定设备失败
		pDevName.LoadString(IDS_OPEN_FALIED);
		pDevName.Replace(STR_POS , lpszPathName);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , pDevName  , strTemp  , MB_OK|MB_ICONERROR);
	}else{
		//打开设备成功
		this->m_strTitle = lpszPathName;
		//设备打开成功的话 就这是列表中的内容
		SetLIstContent();

		//设备的名字
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

	//初始化 列表中的内容
	nCnt = this->m_pDisk->GetPartCount();
	
	//  ID  文件名  类型	起始扇区号  总扇区数  大小 主分区  活动分区
	for (i = 0 ; i <  nCnt ; ++i)
	{
		Disk::PDPart pDPart = m_pDisk->GetPart(i);
		//格式
		format = pDPart->mType;
		
		strTemp.Format(_T("%d") , i);//ID
		m_pContentList->InsertItem( i  , strTemp ,1 );
		
		//设备区域的名字
		m_pContentList->SetItemText(i , 1  ,
			GetPartName(format ,m_pDisk->GetDevName() , pDPart->mOffset) );//名字
		
		//设备区域的类型
		m_pContentList->SetItemText(i , 2  , GetPartFormatName(format));//类型
		
		li = pDPart->mOffset;
		li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
		strTemp.Format(_T("%X") , li.LowPart);
		m_pContentList->SetItemText(i , 3  , strTemp);//起始山区
		
		li = pDPart->mSecCount;
		li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
		strTemp.Format(_T("%X") , li.LowPart);
		m_pContentList->SetItemText(i , 4  , strTemp);//总扇区数
		
		li.QuadPart = pDPart->mSecCount.QuadPart * SECTOR_SIZE;
		strTemp = GetSizeToString(li);
		m_pContentList->SetItemText(i , 5  , strTemp);//大小
		
		if ( -1 != pDPart->mVolIndex )
		{//是否为一个文件系统
			if (pDPart->mIsMainPart) //主分区?
				strTemp.LoadString(IDS_YES);	
			else
				strTemp.LoadString(IDS_NO);
			m_pContentList->SetItemText(i , 6  , strTemp);//大小
			
			if (pDPart->mIsActivity) //活动分区?
				strTemp.LoadString(IDS_YES);	
			else
				strTemp.LoadString(IDS_NO);
			m_pContentList->SetItemText(i , 7  , strTemp);//大小
			
			if (pDPart->mLogicalLetter)
			{
				strTemp = pDPart->mLogicalLetter;
				m_pContentList->SetItemText(i , 8  , strTemp);//大小
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
// 	//设置图片列表
// 	m_pContentList->SetImageList(&(((CMainFrame*)::AfxGetMainWnd())->m_wndImageList)
// 		,TVSIL_NORMAL);
	
	//  ID  文件名  类型	其实扇区号  总扇区数  大小 主分区？ 活动分区? 逻辑驱动
	strHead.LoadString(IDS_ID);
	m_pContentList->InsertColumn( 0, strHead , LVCFMT_LEFT, 30 );//插入列
	
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

	//相应列表的点击事件
	if (this->m_pContentList)
	{
		//获得选中的项	
		nItem = m_pContentList->GetSelectionMark();
		strStartSec = m_pContentList->GetItemText(nItem , 3 );
		this->m_liCurSec = HexStrToLONG_INT(strStartSec);

		LONG_INT secCnt;
		strStartSec = m_pContentList->GetItemText(nItem , 4 );
		secCnt = HexStrToLONG_INT(strStartSec);
		//设置显示列表
		SectorList secList;
		secList.AddSector(this->m_liCurSec  , secCnt);

		//获取扇区序列名字 
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

	//必须已经获得列表控件的指针
	ASSERT(NULL != m_pContentList);

	*pResult = 0;
	
	//相应列表的点击事件
	pos = m_pContentList->GetFirstSelectedItemPosition();
	if (pos == NULL){//没有选择任何数据
		TRACE0("No items were selected!\n");
		return ;
	}
	
	//获得选中的项	
	nItem = m_pContentList->GetNextSelectedItem(pos);

	//获得选中项的类型
	strType = m_pContentList->GetItemText(nItem , 2);

	//是否为FAT32
	if (sTemp.LoadString(IDS_FAT32_PART) , 
		0 == sTemp.CompareNoCase(strType))
	{//选择的是FAT32类型

		dwType = PART_FAT32;
	
	}else if (sTemp.LoadString(IDS_NTFS_PART) , 
		0 == sTemp.CompareNoCase(strType))
	{//选择的是NTFS类型
		
		dwType = PART_NTFS;

	}else{
		//其他类型的话只是简单的显示一下

		strStartSec = m_pContentList->GetItemText(nItem , 3 );
		this->m_liCurSec = HexStrToLONG_INT(strStartSec);
		this->UpdateAllViews(NULL);

		return ;
	}

// 	//获得当前选择的节点
// 	Disk::PDPart pPart = this->m_pDisk->GetPart(nItem);

	//打开设备的消息参数
	sTemp = this->m_pDisk->GetDevName() ;  //设备的名字

	//序号
	strStartSec.Format(_T(" %d ") , nItem );
	sTemp += _T(" ");
	sTemp += PN_INDEX;		//接下来是 缩影参数
	sTemp += strStartSec;	//添加了索引
	
	//偏移
	sTemp += PN_OFFSET;
	sTemp += _T(" ");		
	strStartSec = m_pContentList->GetItemText(nItem , 3 );  //其实扇区号
	sTemp += strStartSec;

	//盘符
	strStartSec = m_pContentList->GetItemText(nItem , 8 );
	strStartSec.TrimLeft();
	strStartSec.TrimRight();
	if (0 != strStartSec.GetLength())
	{//有盘符
		sTemp += _T(" ");	
		sTemp += PN_LETTER;
		sTemp += _T(" ");	
		sTemp += strStartSec;
	}
	
	//向主框架发送消息 需要打开一个新的文档
	AfxGetMainWnd()->SendMessage( DMSG_OPEN_NEW_DOC , WPARAM(&sTemp)  , dwType);
}




// void CDiskDoc::GotUnpartableSecCont()
// {
// 	//初始化 列表中的内容
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
// 	//  ID  文件名  类型	起始扇区号  总扇区数  大小 
// 	for (i = 0 ; i <  nCnt ; ++i)
// 		if((format = m_pDisk->GetPartFormat(i)) == PART_UNPARTBLE)
// 			break;
// 
// 	if (i == nCnt) 	return ;  //没有找到不可分配的空间节点
// 
// 	
// 	strTemp.Format(_T("%d") , i);//ID
// 	m_pContentList->InsertItem( i  , strTemp ,1 );
// 	
// 	m_pContentList->SetItemText(i , 1  ,
// 			GetPartName(format ,m_pDisk->GetDevName() , m_pDisk->GetPartOffset(i)) );//名字
// 	//m_pContentList->SetItemText(i , 1  , GetPartFormatName(format));//名字
// 	
// 	m_pContentList->SetItemText(i , 2  , GetPartFormatName(format));//类型
// 	
// 	li = m_pDisk->GetPartOffset(i);
// 	li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
// 	strTemp.Format(_T("%X") , li.LowPart);
// 	m_pContentList->SetItemText(i , 3  , strTemp);//起始山区
// 	
// 	li = m_pDisk->GetPartSectorCount(i);
// 	li.HighPart ? strTemp.Format(_T("%X%8X") , li.HighPart , li.LowPart) :
// 	strTemp.Format(_T("%X") , li.LowPart);
// 	m_pContentList->SetItemText(i , 4  , strTemp);//总扇区数
// 	
// 	li.QuadPart = m_pDisk->GetPartSectorCount(i).QuadPart * SECTOR_SIZE;
// 	strTemp = GetSizeToString(li);
// 	m_pContentList->SetItemText(i , 5  , strTemp);//大小
// 
// 
// 	//改变视图的可以显示的最大的扇区号
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
	/*AfxMessageBox(_T("上一扇区"));*/
	ReSetSectorList();
	if (0 != this->m_liCurSec.QuadPart )
	{//当前显示的不是第一扇区
		--this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::OnBnClickedNextSector()
{
/*	AfxMessageBox(_T("下一扇区"));*/
	//总扇区数
	LONG_INT liSecCnt = this->m_pDisk->GetSecCount();
	ReSetSectorList();
	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//当前不是最后一个扇区
		++this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::OnBnClickedFirstSector()
{
	ReSetSectorList();
	/*AfxMessageBox(_T("第一个扇区"));*/
	if (0 != this->m_liCurSec.QuadPart )
	{//当前显示的不是第一扇区
		this->m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::OnBnClickedLastSector()
{
/*	AfxMessageBox(_T("最后一个扇区"));*/
	//总扇区数
	ReSetSectorList();
	LONG_INT liSecCnt = this->m_pDisk->GetSecCount();

	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//当前不是最后一个扇区
		this->m_liCurSec.QuadPart = liSecCnt.QuadPart - 1;
		UpdateAllViews(NULL);
	}
}

void CDiskDoc::SetCurFile( CString strPath )
{//这个函数式绝对不允许进来的
	ASSERT(FALSE);
}
