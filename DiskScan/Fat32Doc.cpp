// Fat32Doc.cpp : implementation file
//

#include "stdafx.h"
#include "diskscan.h"
#include "Fat32Doc.h"
#include "ChildFrm.h"
#include "..\DiskTool\disktool.h"
#include "MainFrm.h"
#include "Fat32InfoView.h"
#include "Fat32FileAttr.h"
#include "CopyProcessDlg.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFat32Doc

//判断选择的是否是文件
//pList 列表控件
//item	列表项
//strTemp 辅助字符串
#define IsSelFAT32File(pList , item , strTemp) (strTemp = \
	(pList)->GetItemText(item , 3 ),0 != strTemp.GetLength() )


IMPLEMENT_DYNCREATE(CFat32Doc, CDataDoc)

CFat32Doc::CFat32Doc()
: m_pDlgFileAttr(NULL)
, m_hThread(NULL)
, m_bIsRun(FALSE)
, m_hChkDelFile(NULL)
, m_bIsChkDelFileRun(FALSE)
, m_hGetSectorListThread(NULL)
, m_bIsGetSeclistRun(FALSE)
{
	m_pFat32			= new DFat32();
	m_strDevAreaIdx		= _T("0");
	m_strDevStartSec	= _T("0");
	m_pImgList			= new CImageList();
}

CFat32Doc::~CFat32Doc()
{
	//释放需要释放的资源
	if (m_pFat32)
	{
		m_pFat32->CloseDev();
		delete m_pFat32;
	}

	if (NULL != m_hThread && INVALID_HANDLE_VALUE != m_hThread)
	{
		CloseHandle(m_hThread);
	}
	if (NULL != m_hChkDelFile && INVALID_HANDLE_VALUE != m_hChkDelFile)
	{
		CloseHandle(m_hChkDelFile);
	}
	if (NULL != m_hGetSectorListThread && INVALID_HANDLE_VALUE != m_hGetSectorListThread)
	{
		CloseHandle(m_hGetSectorListThread);
	}

	if (m_pImgList)
	{
		delete m_pImgList;
	}
}


BEGIN_MESSAGE_MAP(CFat32Doc, CDataDoc)
	//{{AFX_MSG_MAP(CFat32Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_NOTIFY(NM_CLICK, IDC_DE_LIST, OnClickContextList)
		ON_NOTIFY(NM_DBLCLK, IDC_DE_LIST, OnDbClickContextList)
		ON_NOTIFY(NM_RCLICK, IDC_DE_LIST, OnRClickContextList)
		ON_BN_CLICKED(IDC_BTN_PRE_SECTOR, OnBnClickedPreSector)
		ON_BN_CLICKED(IDC_BTN_NEXT_SECTOR, OnBnClickedNextSector)
		ON_BN_CLICKED(IDC_BTN_FIRST_SECTOR, OnBnClickedFirstSector)
		ON_BN_CLICKED(IDC_BTN_LAST_SECTOR, OnBnClickedLastSector)
		ON_BN_CLICKED(IDC_BTN_PRE_CLUST, OnBnClickedPreClust)
		ON_BN_CLICKED(IDC_BTN_NEXT_CLUST, OnBnClickedNextClust)
		ON_BN_CLICKED(IDC_BTN_FIRST_CLUST, OnBnClickedFirstClust)
		ON_BN_CLICKED(IDC_BTN_LAST_CLUST, OnBnClickedLastClust)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FAT32_POS_DATA, OnFat32PosData)
	ON_COMMAND(ID_FAT32_FILE_ATTR, OnFat32FileAttr)
	ON_COMMAND(ID_FAT32_POS_PARAENT_DIR, OnFat32PosParaentDir)
	ON_COMMAND(ID_FAT32_SERVE_AS, OnFat32ServeAs)
	ON_UPDATE_COMMAND_UI(ID_FAT32_SERVE_AS, OnUpdateFat32ServeAs)
	ON_COMMAND(ID_CHECK_DELETE_FILE, OnCheckDeleteFile)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DELETE_FILE, OnUpdateCheckDeleteFile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFat32Doc diagnostics

#ifdef _DEBUG
void CFat32Doc::AssertValid() const
{
	CDataDoc::AssertValid();
}

void CFat32Doc::Dump(CDumpContext& dc) const
{
	CDataDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFat32Doc commands

BOOL CFat32Doc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	//如果打开的是一个卷的话就就只是一个卷的的名字 如"\\\\?\\C:"
	//如果是一个物理的话传过来的是一个物理的名字和一个偏移，如"\\\\.\\PhysicalDrive ABCDEF10"
	CString sPath = lpszPathName;
	CString strTemp = _T("");
	CString strOff = _T("");
	DRES res = DR_OK;
	CView * view;

	//创建图标列表
	m_pImgList->Create(12,12,ILC_COLORDDB|ILC_MASK , 0 , 1);
	//m_pImgList->Create(16 , 16 ,ILC_COLOR32|ILC_MASK , 0 , 4);
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_FILE);//文件  0
	m_pImgList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_FOLDER);//目录     1
	m_pImgList->Add(hIcon);	


	//获得需要的内容列表
	POSITION pos = GetFirstViewPosition();
	view = 	GetNextView(pos);
	m_pContentList = &((CChildFrm*)(view->GetParentFrame()))->m_DisList;
	//对列表进行初始化
	InitContentListHead();
	m_pContentList->SetImageList(m_pImgList , LVSIL_SMALL);

	//简单的数据初始化
	m_liCurSec.QuadPart = 0;
	m_liStartSec.QuadPart = 0;

	//在这里进行卷的打开处理

	//释放两边的空格
	sPath.TrimLeft();
	sPath.TrimRight();

	//获得设备的名字
	strTemp = GetPathParam(sPath , PT_DEVNAME);
	strOff = GetPathParam(sPath , PT_OFFSET);
	if (strOff.GetLength() != 0) //有偏移参数
	{
		m_liStartSec = HexStrToLONG_INT(strOff);
		m_strDevStartSec = strOff;
	}
//要打开的设备名字
	m_strTitle = strTemp;
	res = m_pFat32->OpenDev(strTemp.GetBuffer(), m_liStartSec);
	if (res != DR_OK)
	{//TODO 打开设备失败
		sPath.LoadString(IDS_OPEN_FALIED);
		sPath.Replace(STR_POS , lpszPathName);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , sPath  , strTemp  , MB_OK|MB_ICONERROR);
		m_strTitle = _T("");
		return FALSE;
	}

	//打开设备成功
	strTemp = GetPathParam(sPath , PT_INDEX);
	if (strTemp.GetLength() != 0)
	{
		m_strTitle += _T("->");
		m_strTitle += strTemp;
		m_strDevAreaIdx = strTemp;
	}	
	
	//加上卷标
	m_strTitle += _T(" ");
	WCHAR volName[DEVICE_NAME_LEN + 1] = { 0 };
	m_pFat32->GetVolumeName(volName, DEVICE_NAME_LEN + 1);
	m_strTitle += volName;

	m_secList.AddSector(0 , m_pFat32->GetSecCount());
	m_secList.m_strName = m_strTitle;

	//设置当前要显示的列表
	SetCurPath(_T("/"));

	if (NULL == m_pDlgFileAttr)
	{//属性对话框还没有创建
		m_pDlgFileAttr = new CFat32FileDlg( this );
		m_pDlgFileAttr->Create(CFat32FileDlg::IDD , AfxGetMainWnd());
		m_pDlgFileAttr->UpdateWindow();
		m_pDlgFileAttr->CenterWindow(NULL);
	}

	return TRUE;
}

void CFat32Doc::InitContentListHead()
{//开始初始化列表有

	CString  strHead;
	//设置图片列表
// 	m_pContentList->SetImageList(&(((CMainFrame*)::AfxGetMainWnd())->m_wndImageList)
// 		,TVSIL_NORMAL);
	
	//  ID  文件名 起始扇区号  总扇区数  实际大小  分配大小 创建时间 修改时间 访问时间 
	strHead.LoadString(IDS_ID);
	m_pContentList->InsertColumn( 0, strHead , LVCFMT_LEFT, 30 );//插入列
	
	strHead.LoadString(IDS_FILE_NAME);
	m_pContentList->InsertColumn( 1, strHead , LVCFMT_LEFT, 80 );
	
	strHead.LoadString(IDS_START_SECTOR);
	m_pContentList->InsertColumn( 2, strHead , LVCFMT_RIGHT, 70 );
	
	strHead.LoadString(IDS_SECTOR_COUNT);
	m_pContentList->InsertColumn( 3, strHead , LVCFMT_RIGHT, 60 );
	
	strHead.LoadString(IDS_REAL_SIZE);
	m_pContentList->InsertColumn( 4, strHead , LVCFMT_RIGHT, 60 );

	strHead.LoadString(IDS_ALLOC_SIZE);
	m_pContentList->InsertColumn( 5, strHead , LVCFMT_RIGHT, 60 );

	strHead.LoadString(IDS_CREATE_TIME);
	m_pContentList->InsertColumn( 6, strHead , LVCFMT_LEFT, 140 );

	strHead.LoadString(IDS_MODIFY_TIME);
	m_pContentList->InsertColumn( 7, strHead , LVCFMT_LEFT, 140 );

	strHead.LoadString(IDS_ACCESS_TIME);
	m_pContentList->InsertColumn( 8, strHead , LVCFMT_LEFT, 80 );
	
}

LONG_INT CFat32Doc::GetSecCount()
{
	LONG_INT liCnt = {0};
	liCnt.QuadPart = m_pFat32->GetSecCount();
	return liCnt;
}

BOOL CFat32Doc::ReadData( void* buf , PLONG_INT offset , BOOL isNext /*= TRUE*/ , DWORD size /*= SECTOR_SIZE*/ )
{
	if (NULL == m_pFat32)
		return FALSE;
	if (isNext)
	{
		*offset = m_pCurSecList->FixToNextSector(*offset);
	}else{
		*offset = m_pCurSecList->FixToPreSector(*offset);
	}
	return DR_OK == m_pFat32->ReadData(buf , DWORD(offset->QuadPart) , size) ;
}

void CFat32Doc::OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult)
{
	int			nItem ;
	CString		strTemp;
	CString		strPath;
	CString		strName;
	DFat32File  dfile;
	DRES		res = DR_OK;

	*pResult = 0;
	
	//这里还没有获得列表的指针
	if (m_pContentList == NULL) return ;
	nItem = m_pContentList->GetSelectionMark();
	if (-1 == nItem)//没有选择?
		return;
	if (IsSelFAT32File(m_pContentList , nItem , strTemp))
	{//双击的是文件
		return ;
	}

	//获得选择目录的路径
	strPath = GetSelPath(strName);

	//判断路径是否可以打开
	res = m_pFat32->OpenFile(strPath , &dfile);
	if (res != DR_OK)
	{//TODO 打开指定的文件或者目录失败
		strPath.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strPath.Replace(STR_POS  , strName );
		MessageBox(NULL , strPath , strTemp , MB_OK |MB_ICONWARNING );
	}else{
		//打开成功
		//要显示的数据的扇区号
		m_liCurSec.QuadPart = dfile.GetStartSec();
		dfile.Close();
		SetCurPath(strPath);
		//更新视图
		UpdateAllViews(NULL);
	}		
}

void CFat32Doc::OnClickContextList( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = 0;
	OnFat32PosData();
}

CString CFat32Doc::GetCurPath()
{
	return m_strCurPath;
}

void CFat32Doc::SetCurPath( CString path )
{
	m_strCurPath = path;

	m_bIsRun = FALSE;

	if (m_hThread != NULL && m_hThread != INVALID_HANDLE_VALUE)
	{//线程还没结束
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread , 500))
		{//超时 
			DWORD exitCode;
			if(GetExitCodeThread(m_hThread , &exitCode)){
				TerminateThread(m_hThread , exitCode);
			}
		}
	}

	//创建枚举线程
	CloseHandle(m_hThread);
	m_hThread = ::CreateThread(NULL , 0 , EnumFAT32File , this , 0 , NULL);
}

CRuntimeClass* CFat32Doc::GetInofViewClass()
{
	return RUNTIME_CLASS(CFat32InfoView);
}



void CFat32Doc::OnBnClickedPreSector()
{
	//重置默认扇区列表
	ReSetSectorList();

	if (0 != m_liCurSec.QuadPart )
	{//当前显示的不是第一扇区
		--m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedNextSector()
{
	//总扇区数
	DWORD dwSecCnt = m_pFat32->GetSecCount();

	//重置默认扇区列表
	ReSetSectorList();

	if (m_liCurSec.QuadPart < dwSecCnt - 1)
	{//当前不是最后一个扇区
		++m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedFirstSector()
{
	//重置默认扇区列表
	ReSetSectorList();

	if (0 != m_liCurSec.QuadPart )
	{//当前显示的不是第一扇区
		m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedLastSector()
{
	//总扇区数
	DWORD dwSecCnt = m_pFat32->GetSecCount();

	//重置默认扇区列表
	ReSetSectorList();

	if (m_liCurSec.QuadPart != dwSecCnt - 1)
	{//当前不是最后一个扇区
		m_liCurSec.QuadPart = dwSecCnt - 1;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedPreClust()
{
	//当前扇区号
	LONG_INT liCurSec = m_liCurSec;
	//当前扇区号所在的簇号
	DWORD dwCurClust = m_pFat32->SectToClust((DWORD)liCurSec.QuadPart);
	
	//重置默认扇区列表
	ReSetSectorList();

	if (0 == dwCurClust)
	{//无效的簇号
		return;
	}

	//前一簇
	--dwCurClust;
	m_liCurSec.QuadPart = m_pFat32->ClustToSect(dwCurClust);

	//更新所有的视图
	UpdateAllViews(NULL);

}

void CFat32Doc::OnBnClickedNextClust()
{
	//当前扇区号
	LONG_INT liCurSec = m_liCurSec;
	//当前扇区号所在的簇号
	DWORD dwCurClust = m_pFat32->SectToClust((DWORD)liCurSec.QuadPart);

	//重置默认扇区列表
	ReSetSectorList();

	if (0 == dwCurClust)//当前还在第二号簇之前
		dwCurClust = 2;
	else if (m_pFat32->GetMaxClustNum() == dwCurClust)
	{//已经是最后一簇了
		return ;
	}else
		++dwCurClust;
	m_liCurSec.QuadPart = m_pFat32->ClustToSect(dwCurClust);

	//更新所有的视图
	UpdateAllViews(NULL);
}

void CFat32Doc::OnBnClickedFirstClust()
{
	//当前扇区号
	LONG_INT liCurSec = m_liCurSec;
	//当前扇区号所在的簇号
	DWORD dwCurClust = m_pFat32->SectToClust((DWORD)liCurSec.QuadPart);

	//重置默认扇区列表
	ReSetSectorList();

	if ( 2 != dwCurClust )
	{
		m_liCurSec.QuadPart = m_pFat32->ClustToSect(2);

		//更新所有的视图
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedLastClust()
{
	//要计算最后一簇的簇号
	//总扇区数
	DWORD dwSecCnt = m_pFat32->GetSecCount();
	//数据区的扇区号
	DWORD firstClustSec = m_pFat32->ClustToSect(2);
	//每簇扇区数
	BYTE secPerClust = m_pFat32->GetSecPerClust();

	//重置默认扇区列表
	ReSetSectorList();

	if (0 == secPerClust)
	{//仿佛设备还没有打开
		return ;
	}

	//数据区的总簇数
	DWORD dwClustCnt = (dwSecCnt - firstClustSec) / secPerClust;

	//最后一簇的簇号
	(dwClustCnt += 2)--;

	m_liCurSec.QuadPart = m_pFat32->ClustToSect(dwClustCnt);

	//更新所有的视图
	UpdateAllViews(NULL);

}

void CFat32Doc::OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CRect	cRect;//客户区
	CPoint	p;	//鼠标当前位置
	CMenu	menu;
	CMenu*	pMenu;

	::GetWindowRect(AfxGetMainWnd()->GetSafeHwnd()/*m_pContentList->GetParent()->GetSafeHwnd()*//*AfxGetMainWnd()->GetSafeHwnd()*/ , &cRect);
	::GetCursorPos(&p);
	if(!cRect.PtInRect(p)) 
		return ;//只在列表空中显示快捷菜单

	//快捷菜单
	menu.LoadMenu(IDR_FAT32_FILE_MENU);
	pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN , p.x , p.y , AfxGetMainWnd()/*m_pContentList->GetParent()*//*AfxGetMainWnd()*/ , NULL);
}

void CFat32Doc::OnFat32PosData()
{
	CString		strPath;			//路径
	CString		strName;		//所选择的文件名
	CString		strTemp;
	LONG_INT	liStart = {0};
	DFat32File  dfile;
	DRES		res = DR_OK;

	//获得当前选择的文件的路径
	strPath = GetSelPath(strName);


	//判断路径是否可以打开
	res = m_pFat32->OpenFile(strPath , &dfile);
	if (res != DR_OK)
	{//TODO 打开指定的文件或者目录失败
		strPath.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strPath.Replace(STR_POS  , strName);

		MessageBox(NULL , strPath , strTemp , MB_OK |MB_ICONWARNING );
		return;
	}

	//要显示的数据的扇区号
	m_liCurSec.QuadPart = dfile.GetStartSec();

	if (0 == m_liCurSec.QuadPart)
	{//数据区不可能出现 为0的山区号
		//为空就直接使用磁盘的 
		ReSetSectorList();
	}else{
		SectorList* secList = new SectorList();
		secList->m_strName = strPath;  //区域名字
		secList->AddSector(m_liCurSec , m_pFat32->GetSecPerClust() );   //先使用一个簇
		if (FALSE == SetSectorListNoCopy(secList))
		{
			delete secList;
			secList = 0;
		}

		//创建线程来获取簇链
		m_bIsGetSeclistRun = FALSE;
		if (m_hGetSectorListThread != NULL && m_hGetSectorListThread != INVALID_HANDLE_VALUE)
		{//线程还没结束
			if(WAIT_TIMEOUT == WaitForSingleObject(m_hGetSectorListThread , 500))
			{//超时 
				DWORD exitCode;
				if(GetExitCodeThread(m_hGetSectorListThread , &exitCode)){
					TerminateThread(m_hGetSectorListThread , exitCode);
				}
			}
		}

		//创建枚举线程
		CloseHandle(m_hGetSectorListThread);
		m_hGetSectorListThread = ::CreateThread(NULL , 0 , GetFAT32FileSectorList , this , 0 , NULL);
		Sleep(50); //休息一下
	}


	dfile.Close();			
	//更新视图
	UpdateAllViews(NULL);
}

void CFat32Doc::OnFat32FileAttr()
{
	CString strName;
	//获得当前选择的文件的完整路径
	CString strSelPath = GetSelPath(strName);
	if (0 == strSelPath.GetLength())
	{//仿佛有点不对劲啊！
		return;
	}

	//设置需要显示的文件的路径
	if(FALSE == m_pDlgFileAttr->SetFilePath(strSelPath))
	{//设置成功
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_GET_FILE_ATTR_ERR);
		strMsg.Replace(STR_POS , strSelPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//重新显示文件信息对话框
	::ShowWindow(m_pDlgFileAttr->GetSafeHwnd() , SW_SHOW);
}

void CFat32Doc::OnFat32PosParaentDir()
{
	DFat32File		file;
	DRES			res = DR_OK;
	DWORD			parentIndex = 0;		//在父目录中的短文件名入口项的索引
	DWORD			dwClust;				//簇号
	DWORD			dwSector;
	LONG_INT		liSector;				//扇区号
	LONG_INT		liEnd = {0};
	int				iTemp;
	int				i;
	CString			strName;
	//当前列表的路径  也是所谓的父目录
	CString strCurPath = m_strCurPath;
	//获得当前选中的路径
	CString strSelPath = GetSelPath(strName);
	if (1 == strSelPath.GetLength() && IsPathSeparator(strSelPath.GetAt(0)))
	{//是根目录
		return;
	}

	//打开指定的文件
	res = m_pFat32->OpenFile(strSelPath , &file);

	if ( DR_OK != res )
	{//打开指定的文件失败
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_OPEN_FALIED);
		strMsg.Replace(STR_POS , strSelPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//获取选择文件在父目录中的位置
	parentIndex = file.GetParentIndex();
	file.Close();

	//打开父目录
	res = m_pFat32->OpenFile(strCurPath , &file);
	dwClust = file.GetStartClust();


	//要显示的数据的扇区号
	dwSector = m_pFat32->ClustToSect(dwClust);
	if (0 == dwSector)
	{//数据区不可能出现 为0的山区号
		//为空就直接使用磁盘的 
		ReSetSectorList();
	}
	else
	{
		SectorList* secList = new SectorList();
		secList->m_strName = strCurPath;  //区域名字
		secList->AddSector(dwSector , m_pFat32->GetSecPerClust() );   //先使用一个簇
		if(TRUE == SetSectorListNoCopy(secList))
		{
			//创建线程来获取簇链
			m_bIsGetSeclistRun = FALSE;
			if (m_hGetSectorListThread != NULL && m_hGetSectorListThread != INVALID_HANDLE_VALUE)
			{//线程还没结束
				if(WAIT_TIMEOUT == WaitForSingleObject(m_hGetSectorListThread , 500))
				{//超时 
					DWORD exitCode;
					if(GetExitCodeThread(m_hGetSectorListThread , &exitCode)){
						TerminateThread(m_hGetSectorListThread , exitCode);
					}
				}
			}

			//创建枚举线程
			CloseHandle(m_hGetSectorListThread);
			m_hGetSectorListThread = ::CreateThread(NULL , 0 , GetFAT32FileSectorList , this , 0 , NULL);
			Sleep(50); //休息一下
		}
		else
		{
			delete secList;
		}
	}

	//计算目录所在簇号  每一个目录入口32个字节

	//簇数
	iTemp = (parentIndex * 32) / (m_pFat32->GetSecPerClust() * SECTOR_SIZE);
	//查找具体的簇号
	for (i = 0 ; i < iTemp ; ++i)
		dwClust = m_pFat32->GetFATFromFAT1(dwClust);

	//扇区号
	liSector.QuadPart = m_pFat32->ClustToSect(dwClust);
	liSector.QuadPart +=((parentIndex * 32) / SECTOR_SIZE) % m_pFat32->GetSecPerClust();

	//设置当前扇区
	SetCurSector(liSector);
	
	//字节偏移
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += ((parentIndex % (SECTOR_SIZE / 32)) * 32 );
	liEnd.QuadPart = liSector.QuadPart + 32;

	//选择
	SetSel(liSector , liEnd);
}

void CFat32Doc::OnFat32ServeAs()
{//将文件的数据另外保存起来
	int i = 0;
	int len = 0;
	CString strName;
	//获得当前选中的文件
	CString strFilePath = GetSelPath(strName);
	CString strFileName;
	CString	strWrite;
	
	len = strFilePath.GetLength();
	for (i = len - 1 ; i > 0 && !IsPathSeparator(strFilePath.GetAt(i)); --i);
	strFileName = strFilePath.Mid( i + 1 );
	
	//文件选择对话框
	CFileDialog fDlg(FALSE , NULL , strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if(IDCANCEL == fDlg.DoModal())
	{//取消了 
		return;
	}

	//获得要写的路径名
	strWrite = fDlg.GetPathName();
	
	//开启一个文件复制进度条对话框

	CCopyProcessDlg ccpDlg(strFilePath , strWrite , this);
	ccpDlg.DoModal();
}

void CFat32Doc::OnUpdateFat32ServeAs(CCmdUI *pCmdUI)
{
	CString strTemp;
	CString strName;
	int nSelItem = m_pContentList->GetSelectionMark();
	if (-1 == nSelItem)
	{ //没有选择任何东西
		return ;
	}

	strName = m_pContentList->GetItemText(nSelItem , 1);

	if (_T('*') != strName.GetAt(0) && IsSelFAT32File(m_pContentList , nSelItem , strTemp))
	{//选择的是文件
		pCmdUI->Enable(TRUE);
	}else//选择的是目录
		pCmdUI->Enable(FALSE);
}

void CFat32Doc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (NULL != m_pDlgFileAttr)
	{//销毁窗口
		m_pDlgFileAttr->DestroyWindow();
		delete m_pDlgFileAttr;
	}

	m_pImgList->DeleteImageList();

	CDataDoc::OnCloseDocument();
}

CString CFat32Doc::GetSelPath(CString& strName)
{
	int			nItem = 0;
	CString		strTemp;
	//CString		strName;
	CString		strPath = _T("/");
	int			pathLen;		//原路径的长度

	if (m_pContentList == NULL)//这里还没有获得列表的指针?
		return strPath;
	nItem = m_pContentList->GetSelectionMark();
	if (-1 == nItem) {//没有选中任何有效数据
		return strPath;
	}

	//获得文件名
	strName = m_pContentList->GetItemText( nItem , 1 );
	strPath = m_strCurPath;
	pathLen = strPath.GetLength();

	if (!IsSelFAT32File(m_pContentList , nItem , strTemp ))
	{//选择的是目录

		if (0 == strName.Compare(_T(".")))
		{//进入的是当前目录

		}else if(0 == strName.Compare(_T("..")))
		{//要进入的是父目录 
			for ( --pathLen ; pathLen > 0 && !IsPathSeparator(strPath.GetAt(pathLen)) ; --pathLen);
			if (pathLen <= 0)  
				strPath = _T("/");  //这这情况应该不会出现
			else{
				strPath = strPath.Mid(0 , pathLen);
			}
		}else{
			//是普通的目录
			if (IsPathSeparator(strPath.GetAt(pathLen - 1)))
				strPath = strPath + strName;
			else
				strPath = strPath + _T("/") + strName;
		}
	} else {  //点击的是一个目录
		//是一个文件
		if (IsPathSeparator(strPath.GetAt(pathLen - 1)))
			strPath = strPath + strName;
		else
			strPath = strPath + _T("/") + strName;
	}

	return strPath;
}

void CFat32Doc::SetCurFile( CString strPath )
{
	//父目录
	CString strParent;
	int i = 0;
	int len = strPath.GetLength();
	DFat32File dfile;
	if (len == 0) return;
	else if(len == 1){
		//根目录
		if (m_strCurPath.GetLength() != 0)
		{//原路径不在根目录 
			m_pFat32->OpenFile(strPath  , &dfile);
			m_liCurSec.QuadPart = dfile.GetStartSec();
			dfile.Close();
			//更新视图
			UpdateAllViews(NULL);
			SetCurPath(strPath);
		}
		//return;
	}else {
		//非根目录 
		for(i = len-1 ; (i > 0) &&  !IsPathSeparator(strPath.GetAt(i))  ; --i);
		if (0 == i)
		{
			if (IsPathSeparator(strPath.GetAt(0)))
			{//父目录是目录
				strParent = _T("/");
			}else{
				CString strTitle;
				CString strMsg;
				strTitle.LoadString(IDS_PROMPT);
				strMsg.LoadString(IDS_INVALID_PATH);
				strMsg.Replace(STR_POS , strPath);
				::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
				return;
			}
		}else  //是一个常规目录
			strParent = strPath.Left(i);
		
		//可以先判断一下是否需要重新一下路径
		len = strParent.GetLength();
		i = len + 1;
		if ( len == m_strCurPath.GetLength())
		{
			TCHAR tc1 , tc2;
			for(i = 0 ; i < len ; ++i )
			{
				tc1 = strParent.GetAt(i);
				tc2 = m_strCurPath.GetAt(i);
				if ((tc1 == tc2) || (IsPathSeparator(tc1) && IsPathSeparator(tc2)))
					continue;
			}
			
		}/*else//新旧路径不同 */
			/*SetCurPath(strParent);*/
		if (i != len)
		{//新旧路径不同 
			m_pFat32->OpenFile(strParent  , &dfile);
			m_liCurSec.QuadPart = dfile.GetStartSec();
			dfile.Close();
			//更新视图
			UpdateAllViews(NULL);
			SetCurPath(strParent);
		}
	}

	//设置需要显示的文件的路径
	if(FALSE == m_pDlgFileAttr->SetFilePath(strPath))
	{//设置成功
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_GET_FILE_ATTR_ERR);
		strMsg.Replace(STR_POS , strPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//重新显示文件信息对话框
	::ShowWindow(m_pDlgFileAttr->GetSafeHwnd() , SW_SHOW);
}

void CFat32Doc::OnCheckDeleteFile()
{//查看已经删除来的文件
	m_bIsChkDelFileRun = FALSE;

	if (m_hChkDelFile != NULL && m_hChkDelFile != INVALID_HANDLE_VALUE)
	{//线程还没结束
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hChkDelFile , 500))
		{//超时 
			DWORD exitCode;
			if(GetExitCodeThread(m_hChkDelFile , &exitCode)){
				TerminateThread(m_hChkDelFile , exitCode);
			}
		}
	}

	//创建枚举线程
	CloseHandle(m_hChkDelFile);
	m_hChkDelFile = ::CreateThread(NULL , 0 , EnumDelFAT32File , this , 0 , NULL);
}

void CFat32Doc::OnUpdateCheckDeleteFile(CCmdUI *pCmdUI)
{
	//判断最后一条记录是否是表已经删除
	int cnt = m_pContentList->GetItemCount();
	CString strName = m_pContentList->GetItemText(cnt-1 , 1);
	if (strName.GetAt(0) == _T('*'))
	{//
		pCmdUI->Enable(FALSE);
	}else{
		pCmdUI->Enable(TRUE);
	}
}
