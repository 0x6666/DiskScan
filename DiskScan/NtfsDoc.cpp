// NtfsDoc.cpp : implementation file
//

#include "stdafx.h"
#include "diskscan.h"
#include "NtfsDoc.h"
#include "ChildFrm.h"
#include "NtfsInfoView.h"
#include "CopyProcessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//判断当前列表中选择的是否是一个NTFS文件
#define IsSelNTFSFile(pList , item , strTemp) (strTemp = \
	(pList)->GetItemText(item , 3 ),0 != strTemp.GetLength() )
/////////////////////////////////////////////////////////////////////////////
// CNtfsDoc

#pragma  warning(disable:4996)


IMPLEMENT_DYNCREATE(CNtfsDoc, CDataDoc)

CNtfsDoc::CNtfsDoc()
: m_strCurPath(_T(""))
, m_pNtfsFileDlg(NULL)
, m_hThread(NULL)
, m_bIsRun(FALSE)
{
	m_liStartSec.QuadPart = 0;
	m_pNtfs = new DNtfs();
	m_strOpenParam = _T("");
/*	m_pEveIsRun = new CEvent(FALSE , TRUE , NULL , NULL);*/
	m_pImgList = new CImageList();
}

CNtfsDoc::~CNtfsDoc()
{
	if (this->m_pNtfs)
	{//释放相应的资源
		delete this->m_pNtfs;
		this->m_pNtfs = NULL;
	}

	if (NULL != m_pImgList)
	{
		delete m_pImgList;
	}

// 	if (this->m_pEveIsRun)
// 	{
// 		m_pEveIsRun->SetEvent();  //还是先出发吧
// 		delete m_pEveIsRun;
// 	}
}


BEGIN_MESSAGE_MAP(CNtfsDoc, CDataDoc)
	//{{AFX_MSG_MAP(CNtfsDoc)
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
	ON_COMMAND(ID_NTFS_FILE_ATTR, OnNtfsFileAttr)
	ON_COMMAND(ID_NTFS_POS_MFT, OnNtfsPosMft)
	ON_COMMAND(ID_NTFS_POS_PARAENT_DIR, OnNtfsPosParaentDir)
	ON_COMMAND(ID_NTFS_SERVE_AS, OnNtfsServeAs)
	ON_UPDATE_COMMAND_UI(ID_NTFS_SERVE_AS, OnUpdateNtfsServeAs)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNtfsDoc diagnostics

#ifdef _DEBUG
void CNtfsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNtfsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


BOOL CNtfsDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//如果打开的是一个卷的话就就只是一个卷的的名字 如"\\\\?\\C:"
	//如果是一个物理的话传过来的是一个物理的名字和一个偏移，如"\\\\.\\PhysicalDrive ABCDEF10"
	CString sPath = m_strOpenParam = lpszPathName;
	CString strTemp = _T("");
	CString strOff  =_T("");
	char denName[DEVICE_NAME_LEN + 1] = {0};
	DRES res = DR_OK;
	CView * view;
	WCHAR  volName[MAX_PATH] = {0};

	//创建图标列表
	m_pImgList->Create(12,12,ILC_COLORDDB|ILC_MASK , 0 , 1);
	//this->m_pImgList->Create(16 , 16 ,ILC_COLOR32|ILC_MASK , 0 , 4);
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_FILE);//文件  0
	m_pImgList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_FOLDER);//目录     1
	m_pImgList->Add(hIcon);	

	//获得需要的内容列表
	POSITION pos = GetFirstViewPosition();
	view = 	GetNextView(pos);
	this->m_pContentList = &((CChildFrm*)(view->GetParentFrame()))->m_DisList;
	//对列表进行初始化
	InitContentListHead();
	this->m_pContentList->SetImageList(m_pImgList , LVSIL_SMALL);

	//简单的数据初始化
	this->m_liCurSec.QuadPart = 0;
	this->m_liStartSec.QuadPart = 0;

	//在这里进行卷的打开处理

	//释放两边的空格
	sPath.TrimLeft();	
	sPath.TrimRight();

	//获得设备的名字
	strTemp = GetPathParam(sPath , PT_DEVNAME);
	strOff = GetPathParam(sPath , PT_OFFSET);
	if (strOff.GetLength() != 0) //有偏移参数
		m_liStartSec = HexStrToLONG_INT(strOff);

	//要打开的设备名字
#ifdef  UNICODE
	UnicToMultyByte(strTemp , denName ,DEVICE_NAME_LEN + 1);
#else
	strcpy(denName , strTemp);
#endif

	res = this->m_pNtfs->OpenDev(/*(LPCSTR)(LPCTSTR)strTemp*/denName , &m_liStartSec);
	if (res != DR_OK)
	{//TODO 打开设备失败
		sPath.LoadString(IDS_OPEN_FALIED);
		sPath.Replace(STR_POS , lpszPathName);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , sPath  , strTemp  , MB_OK|MB_ICONERROR);
		this->m_strTitle = _T("");
		return FALSE;
	}

	//打开设备成功
	strTemp = GetPathParam(sPath , PT_INDEX);
	this->m_strTitle = denName;
	if (strTemp.GetLength() != 0)
	{
		this->m_strTitle += _T("->");
		this->m_strTitle += strTemp;
	}	

	//加上卷标
	this->m_strTitle += _T(" ");
	this->m_pNtfs->GetVolumeName( volName , MAX_PATH );
	this->m_strTitle += volName;

	//设置显示范围
	this->m_secList.AddSector(0 , m_pNtfs->GetSecCount());
	this->m_secList.m_strName = m_strTitle;
	
	//this->SetSectorList(&m_secList);
	
	//设置当前要显示的列表
	SetCurPath(_T("/"));

	if (NULL == this->m_pNtfsFileDlg)
	{//属性对话框还没有创建
		m_pNtfsFileDlg = new CNtfsFileDlg( this );
		m_pNtfsFileDlg->Create(CNtfsFileDlg::IDD , AfxGetMainWnd());
		m_pNtfsFileDlg->UpdateWindow();
		m_pNtfsFileDlg->CenterWindow(NULL);
	}

	return TRUE;
}

BOOL CNtfsDoc::ReadData( void* buf , PLONG_INT offset , BOOL isNext/* = TRUE*/ , DWORD size /*= SECTOR_SIZE*/ )
{
	if (this->m_pNtfs == NULL)
		return FALSE;

	if (isNext)
	{
		*offset = this->m_pCurSecList->FixToNextSector(*offset);
	}else{
		*offset = this->m_pCurSecList->FixToPreSector(*offset);
	}
	return DR_OK == this->m_pNtfs->ReadData(buf , offset , size) ;
}

LONG_INT CNtfsDoc::GetSecCount()
{
	LONG_INT liCnt = {0};
	
	if (this->m_pNtfs)
		return this->m_pNtfs->GetSecCount();
	else
		return liCnt;
}

void CNtfsDoc::InitContentListHead()
{
	CString  strHead;
	//设置图片列表

	//  ID  文件名 MFT记录 实际大小  分配大小 创建时间 MFT修改时间  数据修改时间 访问时间 
	strHead.LoadString(IDS_ID);
	m_pContentList->InsertColumn( 0, strHead , LVCFMT_LEFT, 30 );//插入列

	//文件名
	strHead.LoadString(IDS_FILE_NAME);
	m_pContentList->InsertColumn( 1, strHead , LVCFMT_LEFT, 80 );

	//MFT记录号
	strHead.LoadString(IDS_MFT_RECODE);
	m_pContentList->InsertColumn( 2, strHead , LVCFMT_RIGHT, 70 );

	//文件数据的实际大小
	strHead.LoadString(IDS_REAL_SIZE);
	m_pContentList->InsertColumn( 3, strHead , LVCFMT_RIGHT, 60 );

	//文件的分配大小   如果分配大小为0，但是实际大小不是0的话文件的数据位常驻
	strHead.LoadString(IDS_ALLOC_SIZE);
	m_pContentList->InsertColumn( 4, strHead , LVCFMT_RIGHT, 60 );

	//创建时间
	strHead.LoadString(IDS_CREATE_TIME);
	m_pContentList->InsertColumn( 5, strHead , LVCFMT_LEFT, 140 );

	//MFT修改时间 IDS_MFT_MODIFY_TIME
	strHead.LoadString(IDS_MFT_MODIFY_TIME);
	m_pContentList->InsertColumn( 6, strHead , LVCFMT_LEFT, 140 );

	//文件的修改时间
	strHead.LoadString(IDS_FILE_MODIFY_TIME);
	m_pContentList->InsertColumn( 7, strHead , LVCFMT_LEFT, 140 );

	//访问时间
	strHead.LoadString(IDS_ACCESS_TIME);
	m_pContentList->InsertColumn( 8, strHead , LVCFMT_LEFT, 80 );
}

void CNtfsDoc::SetCurPath( CString path )
{
	this->m_strCurPath = path;

//	m_pEveIsRun->SetEvent();  //设置事件  表示需要退出现有线程

	if (m_hThread != NULL && m_hThread != INVALID_HANDLE_VALUE)
	{//线程还没结束
		m_bIsRun = FALSE;
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread , 500))
		{//超时 
			DWORD exitCode;
			if(GetExitCodeThread(m_hThread , &exitCode)){
				TerminateThread(m_hThread , exitCode);
			}
		}
	}

	//创建美剧线程
	m_bIsRun = TRUE;
	//m_pEveIsRun->ResetEvent();
	CloseHandle(m_hThread);
	m_hThread = ::CreateThread(NULL , 0 , EnumNtfsFile , this , 0 , NULL);
}
void CNtfsDoc::OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION	pos;
	int			nItem ;
	CString		sRealSize;
	CString		path;
	CString		strName;
	CString     sTemp;
	int			iTemp = 0;
	LONG_INT	liStart = {0};
	DNtfsFile   nfile;
	DRES		res = DR_OK;

	
	*pResult = 0;
	
	//这里还没有获得列表的指针
	if (this->m_pContentList == NULL) return ;
	
	//获得列表的第一选中项
	pos = m_pContentList->GetFirstSelectedItemPosition();
	if (pos == NULL){//没有选择任何数据
		TRACE0("No items were selected!\n");
		*pResult = 0;
		return ;
	}
	
	//获得选中中项的索引
	nItem = m_pContentList->GetNextSelectedItem(pos);
	sRealSize = m_pContentList->GetItemText(nItem , 3 );  //通过实际大小来判断是否是问文件
	if (0 == sRealSize.GetLength())
	{//选择的是目录
		path = this->m_strCurPath;
		iTemp = path.GetLength();
		
		if (iTemp < 1) return; //TODO  应该是有问题了
		
		//获得文件名
		strName = m_pContentList->GetItemText(nItem , 1 );
		if (0 == strName.Compare(_T(".")))
		{//进入的是当前目录
			
		}else if(0 == strName.Compare(_T("..")))
		{//要进入的是父目录 
			for ( --iTemp ; iTemp > 0 && !IsPathSeparator(path.GetAt(iTemp)) ; --iTemp);
			if (iTemp <= 0)  
				path = _T("/");  //这这情况应该不会出现
			else{
				path = path.Mid(0 , iTemp);
			}
			
		}else{
			//事业普通的文件
			if (IsPathSeparator(path.GetAt(iTemp - 1)))
				path = path + strName;
			else
				path = path + _T("/") + strName;
		}

		//判断路径是否可以打开
		res = this->m_pNtfs->OpenFile((LPCTSTR)path , &nfile);
		if (res != DR_OK)
		{//TODO 打开指定的文件或者目录失败
			path.LoadString(IDS_OPEN_FALIED);
			sTemp.LoadString(IDS_PROMPT);
			path.Replace(STR_POS  , strName);

			MessageBox(NULL , path , sTemp , MB_OK |MB_ICONWARNING );
		}else{
			//打开成功
			//要显示的数据的扇区号
			this->m_liCurSec = nfile.GetMftStartSec();
			nfile.Close();
			SetCurPath(path);

			//更新视图
			UpdateAllViews(NULL);
		}		
	}	
}

void CNtfsDoc::OnClickContextList( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = 0;
	
	//定位到MFT记录位置
	OnNtfsPosMft();
}

CString CNtfsDoc::GetSelPath(CString &strName)
{
	int			nItem = 0;
	CString		strTemp;
	//CString		strName;
	CString		strPath = _T("/");
	int			nLen = 0;
		 
	//这里还没有获得列表的指针
	if (this->m_pContentList == NULL) return strPath;
	//获得选中中项的索引
	nItem = m_pContentList->GetSelectionMark();
	if(-1 == nItem) return strPath;//没有选择任何一行

	//获得选择了的文件名
	strName = m_pContentList->GetItemText(nItem , 1 );

	//原路径
	strPath = this->m_strCurPath;
	//原路径的长度
	nLen = strPath.GetLength();

	if (!IsSelNTFSFile(m_pContentList , nItem , strTemp))
	{//选择的是目录

		if (0 == strName.Compare(_T(".")))
		{//进入的是当前目录
			
		}else if(0 == strName.Compare(_T("..")))
		{//要进入的是父目录 
			for ( --nLen ; nLen > 0 && !IsPathSeparator(strPath.GetAt(nLen)) ; --nLen);
			if (nLen <= 0)  
				strPath = _T("/");  //这这情况应该不会出现
			else{
				strPath = strPath.Mid(0 , nLen);
			}
		}else{
			//是普通的目录
			if (IsPathSeparator(strPath.GetAt(nLen - 1)))
				strPath = strPath + strName;
			else
				strPath = strPath + _T("/") + strName;
		}
	} else {  //点击的是一个文件 
		//是一个文件
		if (IsPathSeparator(strPath.GetAt(nLen - 1)))
			strPath = strPath + strName;
		else
			strPath = strPath + _T("/") + strName;
	}

	return strPath;
}

CRuntimeClass* CNtfsDoc::GetInofViewClass()
{
	return RUNTIME_CLASS(CNtfsInfoView);
}

void CNtfsDoc::OnBnClickedPreSector()
{
	ReSetSectorList();
	/*AfxMessageBox(_T("上一扇区"));*/
	if (0 != this->m_liCurSec.QuadPart )
	{//当前显示的不是第一扇区
		--this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedNextSector()
{
	/*	AfxMessageBox(_T("下一扇区"));*/
	//总扇区数
	LONG_INT liSecCnt = this->m_pNtfs->GetSecCount();

	ReSetSectorList();

	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//当前不是最后一个扇区
		++this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedFirstSector()
{
	ReSetSectorList();
	/*A
	fxMessageBox(_T("第一个扇区"));*/
	if (0 != this->m_liCurSec.QuadPart )
	{//当前显示的不是第一扇区
		this->m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedLastSector()
{
	/*	AfxMessageBox(_T("最后一个扇区"));*/
	//总扇区数
	LONG_INT liSecCnt = this->m_pNtfs->GetSecCount();

	ReSetSectorList();

	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//当前不是最后一个扇区
		this->m_liCurSec.QuadPart = liSecCnt.QuadPart - 1;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedPreClust()
{
	BYTE secPerClust = this->m_pNtfs->GetSecPerClust();
	LONG_INT liClust = {0};
	liClust.QuadPart = this->m_liCurSec.QuadPart /secPerClust; //簇号

	ReSetSectorList();

	if (0 !=  liClust.QuadPart)
	{//当前显示的不是第一扇区
		this->m_liCurSec.QuadPart = (--liClust.QuadPart) *  secPerClust;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedNextClust()
{
	BYTE secPerClust = this->m_pNtfs->GetSecPerClust();
	LONG_INT liClust = {0};			//当前所在的簇号
	LONG_INT liClustCnt = {0};		//簇数
	liClust.QuadPart = this->m_liCurSec.QuadPart /secPerClust; //簇号
	liClustCnt.QuadPart = m_pNtfs->GetSecCount().QuadPart / secPerClust;

	ReSetSectorList();

	if ( liClust.QuadPart < liClustCnt.QuadPart - 1)
	{//当前显示的不是第一扇区
		this->m_liCurSec.QuadPart = (++liClust.QuadPart) *  secPerClust;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedFirstClust()
{
	this->ReSetSectorList();
	if (0 != this->m_liCurSec.QuadPart )
	{//当前显示的不是第一扇区
		this->m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedLastClust()
{
	BYTE secPerClust = this->m_pNtfs->GetSecPerClust();
	LONG_INT liClust = {0};			//当前所在的簇号
	LONG_INT liClustCnt = {0};		//簇数
	liClust.QuadPart = this->m_liCurSec.QuadPart /secPerClust; //簇号
	liClustCnt.QuadPart = m_pNtfs->GetSecCount().QuadPart / secPerClust;

	ReSetSectorList();

	if ( liClust.QuadPart < liClustCnt.QuadPart)
	{//当前显示的不是第一扇区
		this->m_liCurSec.QuadPart = (--liClustCnt.QuadPart) *  secPerClust;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CRect	cRect;//客户区
	CPoint	p;	//鼠标当前位置
	CMenu	menu;
	CMenu*	pMenu;

	::GetWindowRect(AfxGetMainWnd()->GetSafeHwnd()/*this->m_pContentList->GetParent()->GetSafeHwnd()*/ , &cRect);
	::GetCursorPos(&p);
	if(!cRect.PtInRect(p)) 
		return ;//只在列表空中显示快捷菜单

	//快捷菜单
	menu.LoadMenu(IDR_NTFS_FILE_MENU);
	pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN , p.x , p.y , AfxGetMainWnd()/*this->m_pContentList->GetParent()*/ , NULL);
}

void CNtfsDoc::OnNtfsFileAttr()
{
	DRES res = DR_OK;
	DNtfsFile   nfile;
	CString		strTemp;
	//获得当前选择的文件的完整路径
	CString strName;
	CString strSelPath = this->GetSelPath(strName);
	if (0 == strSelPath.GetLength())
	{//仿佛有点不对劲啊！
		return;
	}

	//设置需要显示的文件的路径
	if(FALSE == this->m_pNtfsFileDlg->SetFilePath(strSelPath))
	{//设置成功
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_GET_FILE_ATTR_ERR);
		strMsg.Replace(STR_POS , strSelPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//判断路径是否可以打开
	res = this->m_pNtfs->OpenFileA((LPCSTR)(LPCTSTR)strSelPath , &nfile);
	if (res != DR_OK)
	{//TODO 打开指定的文件或者目录失败
		strName.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strName.Replace(STR_POS  , strSelPath);

		MessageBox(NULL , strName , strTemp , MB_OK |MB_ICONWARNING );
		return;
	}

	//扇区列表
	SectorList secList;
	secList.AddSector(nfile.GetMftStartSec() , 2 );  //一般每一个MFT记录是1K(2个扇区)
	secList.m_strName = strSelPath;
	this->SetSectorList(&secList);

	//重新显示文件信息对话框
	::ShowWindow(m_pNtfsFileDlg->GetSafeHwnd() , SW_SHOW);
}


void CNtfsDoc::OnNtfsPosMft()
{
	DNtfsFile   nfile;
	DRES		res = DR_OK;
	CString		strPath;
	CString		strTemp;
	CString		strName;
	
	//获得当前选中的文件或目录的路径
	strPath = GetSelPath(strName);

	//判断路径是否可以打开
	res = this->m_pNtfs->OpenFile((LPCTSTR)strPath , &nfile);
	if (res != DR_OK)
	{//TODO 打开指定的文件或者目录失败
		strName.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strName.Replace(STR_POS  , strPath);
		
		MessageBox(NULL , strName , strTemp , MB_OK |MB_ICONWARNING );
		return;
	}

	//要显示的数据的扇区号
	this->m_liCurSec = nfile.GetMftStartSec();

	//扇区列表
	SectorList secList;
	secList.AddSector(m_liCurSec , 2 );  //一般每一个MFT记录是1K(2个扇区)
	secList.m_strName = strPath;
	this->SetSectorList(&secList);

	nfile.Close();			
	//更新视图
	UpdateAllViews(NULL);
}

void CNtfsDoc::OnNtfsPosParaentDir()
{
 	DNtfsFile		file;
 	DRES			res = DR_OK;
 	LONG_INT		liSector;				//扇区号
	LONG_INT		liEnd;
 	CString			strName;
	//当前列表的路径  也是所谓的父目录
	CString strCurPath = this->m_strCurPath;
	//获得当前选中的路径
	CString strSelPath = GetSelPath(strName);
	if (1 == strSelPath.GetLength() && IsPathSeparator(strSelPath.GetAt(0)))
	{//是根目录
		return;
	}

	//打开指定的文件
	res = this->m_pNtfs->OpenFileA(strSelPath , &file);

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

	//全盘查看模式
	ReSetSectorList();

	liSector = file.GetFDTOffset();
	liSector.QuadPart /= SECTOR_SIZE;
	//设置当前扇区
	this->SetCurSector(liSector);

	//字节偏移
	liSector = file.GetFDTOffset();
	liEnd.QuadPart = liSector.QuadPart + file.GetFDTLen();

	//选择
	this->SetSel(liSector , liEnd);
}


void CNtfsDoc::OnNtfsServeAs()
{
	int i = 0;
	int len = 0;
	CString	strName;
	//获得当前选中的文件
	CString strFilePath = this->GetSelPath(strName);
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


void CNtfsDoc::OnUpdateNtfsServeAs(CCmdUI *pCmdUI)
{//只有文件才可另存为 
	CString strTemp;
	int nSelItem = this->m_pContentList->GetSelectionMark();
	if (-1 == nSelItem)
	{ //没有选择任何东西
		return ;
	}

	if (IsSelNTFSFile(m_pContentList , nSelItem , strTemp))
	{//选择的是文件
		pCmdUI->Enable(TRUE);
	}else//选择的是目录
		pCmdUI->Enable(FALSE);
}


void CNtfsDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类

	if(NULL != m_pNtfsFileDlg)
	{
		delete m_pNtfsFileDlg;
	}

	if (NULL != m_pImgList)
	{
		m_pImgList->DeleteImageList();
	}

	CDataDoc::OnCloseDocument();
}

void CNtfsDoc::SetCurFile( CString strPath )
{
	//父目录
	CString strParent;
	int i = 0;
	int len = strPath.GetLength();
	DNtfsFile   nfile;
	if (len == 0) return;
	else if(len == 1){
		//根目录
		if (this->m_strCurPath.GetLength() != 1 )
		{//不同  本来就不在根目录
			SetCurPath(strPath);
			this->m_pNtfs->OpenFile(strPath  , &nfile);
			this->m_liCurSec = nfile.GetMftStartSec();
			nfile.Close();
			//更新视图
			UpdateAllViews(NULL);
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
		i = len + 1; //使i != len  
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
			
			this->m_pNtfs->OpenFile(strParent  , &nfile);
			this->m_liCurSec = nfile.GetMftStartSec();
			nfile.Close();
			//更新视图
			UpdateAllViews(NULL);
			SetCurPath(strParent);
		}
	}

	//设置需要显示的文件的路径
	if(FALSE == this->m_pNtfsFileDlg->SetFilePath(strPath))
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
	::ShowWindow(this->m_pNtfsFileDlg->GetSafeHwnd() , SW_SHOW);
}
