// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DiskScan.h"

#include "MainFrm.h"
#include "DevVolume.h"
#include "DataDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef BIF_NONEWFOLDERBUTTON
#define BIF_NONEWFOLDERBUTTON  0x0200 
#endif
#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE     0x0040  
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_1, OnFileOpen)
	ON_MESSAGE(DMSG_OPEN_NEW_DOC , OnOpenNewDoc)
	ON_MESSAGE(DMSG_SET_CUR_DATA_VIEW_NAME , OnSetCurDataViewName)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_OPEN_FILE_DIR, OnOpenFileDir)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CUR_VIEW,	//当前可以显示的数据区名字
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//现货的ListBar显示的初始大小
	CSize size(400 , 400);


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	//添加显示数据区的信息状态栏格子
	m_wndStatusBar.SetPaneInfo( 1 , ID_INDICATOR_CUR_VIEW ,SBPS_NOBORDERS , 400);//添加一个新的indicator 
	CString strTemp;
	strTemp.LoadString(ID_INDICATOR_CUR_VIEW);
	m_wndStatusBar.SetPaneText(1 ,  strTemp , TRUE); 


	//将窗口显示在显示器中间
	CenterWindow();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//主框架不显示文件的标题
/*	cs.style&= ~FWS_ADDTOTITLE;*/
	//主框架的初始大小
	cs.cx = 900;
	cs.cy = 700;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnFileOpen() 
{
	CString				strDevName;
	CString				sTemp;
	HANDLE				hDev  = INVALID_HANDLE_VALUE;
	LONG_INT			off = {0};//设备的偏移
	DRES				res = DR_OK;
	CMultiDocTemplate*	pTemp = NULL;
	CDocument*			pDoc  = NULL;
	POSITION			pos   = NULL;
	CString				sTitle;			//已经存在的设备名字
	int					index = 0;


	CDevVolumeDlg dlg;
	if( IDOK != dlg.DoModal()) //选择了一个正确设备名字
		return;

	//获得选择了的设备
	strDevName = dlg.GetSelDevName();
		
	//判断打开的是物理磁盘还是卷
	if (SEL_DISK == dlg.m_bSelDisk)
	{//是物理磁盘
		//创建指定的视图
		pTemp =  ((CDiskScanApp*)AfxGetApp())->m_pDiskDocTemplate;//->OpenDocumentFile(
			//strDevName ,TRUE);
	}else if (SEL_VOLUME == dlg.m_bSelDisk)
	{//是一个卷 逻辑驱动器

		if ((res = DFat32::IsContainFat32Flag( strDevName , off )) == DR_OK)
		{//一个FAT32卷
			pTemp =  ((CDiskScanApp*)AfxGetApp())->m_pFat32DocTemplate ;//->OpenDocumentFile(
			//strDevName ,TRUE);

		}else if((res == DR_NO) &&
			( (res = DNtfs::IsContainNTFSFlag( strDevName , off )) == DR_OK))
		{//是一个NTFS卷
			pTemp = ((CDiskScanApp*)AfxGetApp())->m_pNtfsDocTemplate;//->OpenDocumentFile(
			//strDevName ,TRUE);

		}else if(res != DR_NO){
			//设备错误  
			sTemp.LoadString(IDS_OPEN_FALIED);
			sTemp.Replace(STR_POS , strDevName);
			strDevName.LoadString(IDS_ERROR);
			::MessageBox(NULL , sTemp  , strDevName  , MB_OK|MB_ICONERROR);
			return ;
		
		}else{
			//这里是不支持直接打开的类型
			return ;
		}
	}else
		return ;


	//设备名字
	//strDevName	
	
	//检查是否是已经打开了
	pos = pTemp->GetFirstDocPosition();
	
	while( NULL != pos)
	{
		//获得文档指针
		pDoc = pTemp->GetNextDoc(pos);
		sTitle = pDoc->GetTitle();
		index = sTitle.Find(_T(" "));
		if (-1 != index)
			sTitle = sTitle.Mid(0 , index);
		
		if (0 == strDevName.CompareNoCase(sTitle))
		{//找到了
			//	pDoc->se
			//激活找到的窗口
			pos = pDoc->GetFirstViewPosition();
			this->MDIActivate(pDoc->GetNextView(pos)->GetParentFrame());
			
			return ;
		}
	}
	
	//没有的话就打开一个新的
	pTemp->OpenDocumentFile( strDevName ,TRUE);

	return ;
}


LRESULT CMainFrame::OnOpenNewDoc(WPARAM wParam, LPARAM lParam)
{
	//要打开的设备的路径
	CString*			pPath = (CString*)wParam;
	CMultiDocTemplate*	pTemp = NULL;
	CDocument*			pDoc  = NULL;
	POSITION			pos   = NULL;
	CString				sDevName;		//设备名字	
	CString				sIndex;			//序号
	CString				sTitle;			//已经存在的设备名字
	int					index = 0;
	
	if (wParam == 0) return 0;

	//需要检查是否重复打开

	if (lParam == PART_FAT32)
	{//要的打开的是一个fat32卷
		pTemp = ((CDiskScanApp*)AfxGetApp())->m_pFat32DocTemplate;
	}else if(lParam == PART_NTFS){
		//一个ntfs卷
		pTemp = ((CDiskScanApp*)AfxGetApp())->m_pNtfsDocTemplate;//->OpenDocumentFile(
			//*pPath ,TRUE);
	}else{
		//不支持的设备
		return 0;
	}

	//设备名字
	sDevName = GetPathParam(*pPath , PT_DEVNAME);
	//获得序号
	sIndex = GetPathParam(*pPath , PT_INDEX);
	if (0 != sIndex.GetLength())
	{//组合名字
		sDevName += _T("->");
		sDevName += sIndex;
	}


	//检查是否是已经打开了
	pos = pTemp->GetFirstDocPosition();

	while( NULL != pos)
	{
		//获得文档指针
		pDoc = pTemp->GetNextDoc(pos);
		sTitle = pDoc->GetTitle();
		index = sTitle.Find(_T(" "));
		if (-1 != index)
			sTitle = sTitle.Mid(0 , index);

		if (0 == sDevName.CompareNoCase(sTitle))
		{//找到了
			//激活找到的窗口
			pos = pDoc->GetFirstViewPosition();
			this->MDIActivate(pDoc->GetNextView(pos)->GetParentFrame());
			
			return (LRESULT)(pDoc);
		}
	}

	//没有的话就打开一个新的
	return (LRESULT)pTemp->OpenDocumentFile( *pPath ,TRUE);
}


void CMainFrame::OnOpenFileDir()
{
	CString	strTemp;
	strTemp.LoadString(IDS_SELECT_FILE_DIR);
	BROWSEINFO bi = {0};
	char szPathName[MAX_PATH];
	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = szPathName;
	bi.lpszTitle = (LPCSTR)(LPCTSTR)strTemp;
	bi.ulFlags = BIF_NONEWFOLDERBUTTON | BIF_NEWDIALOGSTYLE | BIF_BROWSEFORCOMPUTER | BIF_BROWSEINCLUDEFILES; 
	CString str;
	CString strDir;  //选择的目录
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if(idl == NULL)
	{//没有选择文件或目录
		return;
	}
	//获取选择的文件或者目录路径
	SHGetPathFromIDList(idl, str.GetBuffer(MAX_PATH * 2));
	str.ReleaseBuffer();
	IMalloc * imalloc = 0;
	if (SUCCEEDED(SHGetMalloc(&imalloc)))
	{//相关资源的释放
		imalloc->Free (idl);
		imalloc->Release();
	}
	//判断是否是一个有效的路径
	if (str.GetLength() < 3 )
		return ;
	if (isLetter(str.GetAt(0)) && str.GetAt(1) == _T(':') && IsPathSeparator(str.GetAt(2)))
	{
		PosFileDir(str);
	}else{//无效路径
		strTemp.LoadString(IDS_PROMPT);
		strDir.LoadString(IDS_INVALID_PATH);
		strDir.Replace(STR_POS , str);
		::MessageBox(this->GetSafeHwnd() , strDir , strTemp , MB_OK);
	}
}

void CMainFrame::PosFileDir( CString path )
{
	CString strTemp;
	HANDLE hVol;
	CString strVol  = _T("\\\\?\\");
	//获得盘符
	strVol += path.GetAt(0);
	strVol += _T(":");
	CString strPath = path.Mid(2);	
	BOOL res;
	
	//查询指定的盘符在哪一个设备上
	hVol = CreateFile(strVol , GENERIC_READ | GENERIC_WRITE,  
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL ,  
		OPEN_EXISTING,  0 , NULL);        
	if (hVol == INVALID_HANDLE_VALUE)
	{//打开设备失败
		strPath.LoadString(IDS_OPEN_FALIED);
		strPath.Replace(STR_POS , strVol);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , strPath  , strTemp  , MB_OK|MB_ICONERROR);
		return ;
	}

	STORAGE_DEVICE_NUMBER numb = {0};
	DWORD	readed = 0;
	res = DeviceIoControl( hVol , 
		IOCTL_STORAGE_GET_DEVICE_NUMBER , NULL , 0 ,
		&numb ,	sizeof(numb) , &readed , NULL );
	CloseHandle(hVol);  //你的使命已经完成了
	if (FALSE == res)
	{////获取信息失败 
		strPath.LoadString(IDS_GET_DEV_INTO_ERR);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , strPath  , strTemp  , MB_OK|MB_ICONERROR);
		return ;
	}
	
	//正确获取了物理设备号
	Disk disk;
	if(FALSE == disk.OpenDisk(numb.DeviceNumber))
	{//打开磁盘失败
		strPath.LoadString(IDS_OPEN_FALIED);
		strPath.Replace(STR_POS , strVol);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , strPath  , strTemp  , MB_OK|MB_ICONERROR);
	}

	//获得指定的卷
	Disk::PDPart pPart = NULL;
	int  i = 0;
	char letter = path.GetAt(0);
	int  cnt = disk.GetPartCount();
	for ( ; i < cnt ; ++i)
	{
		pPart = disk.GetPart(i);
		if (pPart->mLogicalLetter == letter)
			break;
	}
	
	if (i == cnt)
	{//获取数据失败
		strPath.LoadString(IDS_OPEN_FALIED);
		strPath.Replace(STR_POS , strVol);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , strPath  , strTemp  , MB_OK|MB_ICONERROR);
	}

	CString strParam;
	//打开设备的消息参数
	strParam = disk.GetDevName() ;  //设备的名字

	//序号
	strTemp.Format(_T(" %d ") , i );
	strParam += _T(" ");
	strParam += PN_INDEX;		//接下来是 缩影参数
	strParam += strTemp;	//添加了索引

	//偏移
	strParam += PN_OFFSET;
	strParam += _T(" ");	
	pPart->mOffset.HighPart?(strTemp.Format(_T("%X%X") , 
		pPart->mOffset.HighPart , pPart->mOffset.LowPart)):(strTemp.Format(_T("%X") , pPart->mOffset.LowPart));
	strParam += strTemp;

	//盘符
	strTemp.Format(_T("%c") , path.GetAt(0));
	strParam += _T(" ");	
	strParam += PN_LETTER;
	strParam += _T(" ");	
	strParam += strTemp;

	DWORD type;
	if (IsFAT32fs(pPart->mType))
	{
		type = PART_FAT32;
	}else if (IsNTFSfs(pPart->mType))
	{
		type = PART_NTFS;
	}
	//打开指定的设备
	CDataDoc* pDoc = (CDataDoc*)OnOpenNewDoc(DWORD(&strParam) , type);

	//设置当前路径
	if (NULL != pDoc) pDoc->SetCurFile(strPath);

	disk.CloseDisk();
}

LRESULT CMainFrame::OnSetCurDataViewName( WPARAM wParam, LPARAM )
{
	int index = this->m_wndStatusBar.CommandToIndex(ID_INDICATOR_CUR_VIEW);
	ASSERT(-1 != index);
	m_wndStatusBar.SetPaneText(index , *(CString*)wParam , TRUE);
	return 0;
}
