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
	ID_INDICATOR_CUR_VIEW,	//��ǰ������ʾ������������
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

	//�ֻ���ListBar��ʾ�ĳ�ʼ��С
	CSize size(400 , 400);


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	//�����ʾ����������Ϣ״̬������
	m_wndStatusBar.SetPaneInfo( 1 , ID_INDICATOR_CUR_VIEW ,SBPS_NOBORDERS , 400);//���һ���µ�indicator 
	CString strTemp;
	strTemp.LoadString(ID_INDICATOR_CUR_VIEW);
	m_wndStatusBar.SetPaneText(1 ,  strTemp , TRUE); 


	//��������ʾ����ʾ���м�
	CenterWindow();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//����ܲ���ʾ�ļ��ı���
/*	cs.style&= ~FWS_ADDTOTITLE;*/
	//����ܵĳ�ʼ��С
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
	LONG_INT			off = {0};//�豸��ƫ��
	DRES				res = DR_OK;
	CMultiDocTemplate*	pTemp = NULL;
	CDocument*			pDoc  = NULL;
	POSITION			pos   = NULL;
	CString				sTitle;			//�Ѿ����ڵ��豸����
	int					index = 0;


	CDevVolumeDlg dlg;
	if( IDOK != dlg.DoModal()) //ѡ����һ����ȷ�豸����
		return;

	//���ѡ���˵��豸
	strDevName = dlg.GetSelDevName();
		
	//�жϴ򿪵���������̻��Ǿ�
	if (SEL_DISK == dlg.m_bSelDisk)
	{//���������
		//����ָ������ͼ
		pTemp =  ((CDiskScanApp*)AfxGetApp())->m_pDiskDocTemplate;//->OpenDocumentFile(
			//strDevName ,TRUE);
	}else if (SEL_VOLUME == dlg.m_bSelDisk)
	{//��һ���� �߼�������

		if ((res = DFat32::IsContainFat32Flag( strDevName , off )) == DR_OK)
		{//һ��FAT32��
			pTemp =  ((CDiskScanApp*)AfxGetApp())->m_pFat32DocTemplate ;//->OpenDocumentFile(
			//strDevName ,TRUE);

		}else if((res == DR_NO) &&
			( (res = DNtfs::IsContainNTFSFlag( strDevName , off )) == DR_OK))
		{//��һ��NTFS��
			pTemp = ((CDiskScanApp*)AfxGetApp())->m_pNtfsDocTemplate;//->OpenDocumentFile(
			//strDevName ,TRUE);

		}else if(res != DR_NO){
			//�豸����  
			sTemp.LoadString(IDS_OPEN_FALIED);
			sTemp.Replace(STR_POS , strDevName);
			strDevName.LoadString(IDS_ERROR);
			::MessageBox(NULL , sTemp  , strDevName  , MB_OK|MB_ICONERROR);
			return ;
		
		}else{
			//�����ǲ�֧��ֱ�Ӵ򿪵�����
			return ;
		}
	}else
		return ;


	//�豸����
	//strDevName	
	
	//����Ƿ����Ѿ�����
	pos = pTemp->GetFirstDocPosition();
	
	while( NULL != pos)
	{
		//����ĵ�ָ��
		pDoc = pTemp->GetNextDoc(pos);
		sTitle = pDoc->GetTitle();
		index = sTitle.Find(_T(" "));
		if (-1 != index)
			sTitle = sTitle.Mid(0 , index);
		
		if (0 == strDevName.CompareNoCase(sTitle))
		{//�ҵ���
			//	pDoc->se
			//�����ҵ��Ĵ���
			pos = pDoc->GetFirstViewPosition();
			this->MDIActivate(pDoc->GetNextView(pos)->GetParentFrame());
			
			return ;
		}
	}
	
	//û�еĻ��ʹ�һ���µ�
	pTemp->OpenDocumentFile( strDevName ,TRUE);

	return ;
}


LRESULT CMainFrame::OnOpenNewDoc(WPARAM wParam, LPARAM lParam)
{
	//Ҫ�򿪵��豸��·��
	CString*			pPath = (CString*)wParam;
	CMultiDocTemplate*	pTemp = NULL;
	CDocument*			pDoc  = NULL;
	POSITION			pos   = NULL;
	CString				sDevName;		//�豸����	
	CString				sIndex;			//���
	CString				sTitle;			//�Ѿ����ڵ��豸����
	int					index = 0;
	
	if (wParam == 0) return 0;

	//��Ҫ����Ƿ��ظ���

	if (lParam == PART_FAT32)
	{//Ҫ�Ĵ򿪵���һ��fat32��
		pTemp = ((CDiskScanApp*)AfxGetApp())->m_pFat32DocTemplate;
	}else if(lParam == PART_NTFS){
		//һ��ntfs��
		pTemp = ((CDiskScanApp*)AfxGetApp())->m_pNtfsDocTemplate;//->OpenDocumentFile(
			//*pPath ,TRUE);
	}else{
		//��֧�ֵ��豸
		return 0;
	}

	//�豸����
	sDevName = GetPathParam(*pPath , PT_DEVNAME);
	//������
	sIndex = GetPathParam(*pPath , PT_INDEX);
	if (0 != sIndex.GetLength())
	{//�������
		sDevName += _T("->");
		sDevName += sIndex;
	}


	//����Ƿ����Ѿ�����
	pos = pTemp->GetFirstDocPosition();

	while( NULL != pos)
	{
		//����ĵ�ָ��
		pDoc = pTemp->GetNextDoc(pos);
		sTitle = pDoc->GetTitle();
		index = sTitle.Find(_T(" "));
		if (-1 != index)
			sTitle = sTitle.Mid(0 , index);

		if (0 == sDevName.CompareNoCase(sTitle))
		{//�ҵ���
			//�����ҵ��Ĵ���
			pos = pDoc->GetFirstViewPosition();
			this->MDIActivate(pDoc->GetNextView(pos)->GetParentFrame());
			
			return (LRESULT)(pDoc);
		}
	}

	//û�еĻ��ʹ�һ���µ�
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
	CString strDir;  //ѡ���Ŀ¼
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if(idl == NULL)
	{//û��ѡ���ļ���Ŀ¼
		return;
	}
	//��ȡѡ����ļ�����Ŀ¼·��
	SHGetPathFromIDList(idl, str.GetBuffer(MAX_PATH * 2));
	str.ReleaseBuffer();
	IMalloc * imalloc = 0;
	if (SUCCEEDED(SHGetMalloc(&imalloc)))
	{//�����Դ���ͷ�
		imalloc->Free (idl);
		imalloc->Release();
	}
	//�ж��Ƿ���һ����Ч��·��
	if (str.GetLength() < 3 )
		return ;
	if (isLetter(str.GetAt(0)) && str.GetAt(1) == _T(':') && IsPathSeparator(str.GetAt(2)))
	{
		PosFileDir(str);
	}else{//��Ч·��
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
	//����̷�
	strVol += path.GetAt(0);
	strVol += _T(":");
	CString strPath = path.Mid(2);	
	BOOL res;
	
	//��ѯָ�����̷�����һ���豸��
	hVol = CreateFile(strVol , GENERIC_READ | GENERIC_WRITE,  
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL ,  
		OPEN_EXISTING,  0 , NULL);        
	if (hVol == INVALID_HANDLE_VALUE)
	{//���豸ʧ��
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
	CloseHandle(hVol);  //���ʹ���Ѿ������
	if (FALSE == res)
	{////��ȡ��Ϣʧ�� 
		strPath.LoadString(IDS_GET_DEV_INTO_ERR);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , strPath  , strTemp  , MB_OK|MB_ICONERROR);
		return ;
	}
	
	//��ȷ��ȡ�������豸��
	Disk disk;
	if(FALSE == disk.OpenDisk(numb.DeviceNumber))
	{//�򿪴���ʧ��
		strPath.LoadString(IDS_OPEN_FALIED);
		strPath.Replace(STR_POS , strVol);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , strPath  , strTemp  , MB_OK|MB_ICONERROR);
	}

	//���ָ���ľ�
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
	{//��ȡ����ʧ��
		strPath.LoadString(IDS_OPEN_FALIED);
		strPath.Replace(STR_POS , strVol);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , strPath  , strTemp  , MB_OK|MB_ICONERROR);
	}

	CString strParam;
	//���豸����Ϣ����
	strParam = disk.GetDevName() ;  //�豸������

	//���
	strTemp.Format(_T(" %d ") , i );
	strParam += _T(" ");
	strParam += PN_INDEX;		//�������� ��Ӱ����
	strParam += strTemp;	//���������

	//ƫ��
	strParam += PN_OFFSET;
	strParam += _T(" ");	
	pPart->mOffset.HighPart?(strTemp.Format(_T("%X%X") , 
		pPart->mOffset.HighPart , pPart->mOffset.LowPart)):(strTemp.Format(_T("%X") , pPart->mOffset.LowPart));
	strParam += strTemp;

	//�̷�
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
	//��ָ�����豸
	CDataDoc* pDoc = (CDataDoc*)OnOpenNewDoc(DWORD(&strParam) , type);

	//���õ�ǰ·��
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
