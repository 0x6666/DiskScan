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

//�ж�ѡ����Ƿ����ļ�
//pList �б�ؼ�
//item	�б���
//strTemp �����ַ���
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
	//�ͷ���Ҫ�ͷŵ���Դ
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
	//����򿪵���һ����Ļ��;�ֻ��һ����ĵ����� ��"\\\\?\\C:"
	//�����һ������Ļ�����������һ����������ֺ�һ��ƫ�ƣ���"\\\\.\\PhysicalDrive ABCDEF10"
	CString sPath = lpszPathName;
	CString strTemp = _T("");
	CString strOff = _T("");
	DRES res = DR_OK;
	CView * view;

	//����ͼ���б�
	m_pImgList->Create(12,12,ILC_COLORDDB|ILC_MASK , 0 , 1);
	//m_pImgList->Create(16 , 16 ,ILC_COLOR32|ILC_MASK , 0 , 4);
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_FILE);//�ļ�  0
	m_pImgList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_FOLDER);//Ŀ¼     1
	m_pImgList->Add(hIcon);	


	//�����Ҫ�������б�
	POSITION pos = GetFirstViewPosition();
	view = 	GetNextView(pos);
	m_pContentList = &((CChildFrm*)(view->GetParentFrame()))->m_DisList;
	//���б���г�ʼ��
	InitContentListHead();
	m_pContentList->SetImageList(m_pImgList , LVSIL_SMALL);

	//�򵥵����ݳ�ʼ��
	m_liCurSec.QuadPart = 0;
	m_liStartSec.QuadPart = 0;

	//��������о�Ĵ򿪴���

	//�ͷ����ߵĿո�
	sPath.TrimLeft();
	sPath.TrimRight();

	//����豸������
	strTemp = GetPathParam(sPath , PT_DEVNAME);
	strOff = GetPathParam(sPath , PT_OFFSET);
	if (strOff.GetLength() != 0) //��ƫ�Ʋ���
	{
		m_liStartSec = HexStrToLONG_INT(strOff);
		m_strDevStartSec = strOff;
	}
//Ҫ�򿪵��豸����
	m_strTitle = strTemp;
	res = m_pFat32->OpenDev(strTemp.GetBuffer(), m_liStartSec);
	if (res != DR_OK)
	{//TODO ���豸ʧ��
		sPath.LoadString(IDS_OPEN_FALIED);
		sPath.Replace(STR_POS , lpszPathName);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , sPath  , strTemp  , MB_OK|MB_ICONERROR);
		m_strTitle = _T("");
		return FALSE;
	}

	//���豸�ɹ�
	strTemp = GetPathParam(sPath , PT_INDEX);
	if (strTemp.GetLength() != 0)
	{
		m_strTitle += _T("->");
		m_strTitle += strTemp;
		m_strDevAreaIdx = strTemp;
	}	
	
	//���Ͼ��
	m_strTitle += _T(" ");
	WCHAR volName[DEVICE_NAME_LEN + 1] = { 0 };
	m_pFat32->GetVolumeName(volName, DEVICE_NAME_LEN + 1);
	m_strTitle += volName;

	m_secList.AddSector(0 , m_pFat32->GetSecCount());
	m_secList.m_strName = m_strTitle;

	//���õ�ǰҪ��ʾ���б�
	SetCurPath(_T("/"));

	if (NULL == m_pDlgFileAttr)
	{//���ԶԻ���û�д���
		m_pDlgFileAttr = new CFat32FileDlg( this );
		m_pDlgFileAttr->Create(CFat32FileDlg::IDD , AfxGetMainWnd());
		m_pDlgFileAttr->UpdateWindow();
		m_pDlgFileAttr->CenterWindow(NULL);
	}

	return TRUE;
}

void CFat32Doc::InitContentListHead()
{//��ʼ��ʼ���б���

	CString  strHead;
	//����ͼƬ�б�
// 	m_pContentList->SetImageList(&(((CMainFrame*)::AfxGetMainWnd())->m_wndImageList)
// 		,TVSIL_NORMAL);
	
	//  ID  �ļ��� ��ʼ������  ��������  ʵ�ʴ�С  �����С ����ʱ�� �޸�ʱ�� ����ʱ�� 
	strHead.LoadString(IDS_ID);
	m_pContentList->InsertColumn( 0, strHead , LVCFMT_LEFT, 30 );//������
	
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
	
	//���ﻹû�л���б��ָ��
	if (m_pContentList == NULL) return ;
	nItem = m_pContentList->GetSelectionMark();
	if (-1 == nItem)//û��ѡ��?
		return;
	if (IsSelFAT32File(m_pContentList , nItem , strTemp))
	{//˫�������ļ�
		return ;
	}

	//���ѡ��Ŀ¼��·��
	strPath = GetSelPath(strName);

	//�ж�·���Ƿ���Դ�
	res = m_pFat32->OpenFile(strPath , &dfile);
	if (res != DR_OK)
	{//TODO ��ָ�����ļ�����Ŀ¼ʧ��
		strPath.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strPath.Replace(STR_POS  , strName );
		MessageBox(NULL , strPath , strTemp , MB_OK |MB_ICONWARNING );
	}else{
		//�򿪳ɹ�
		//Ҫ��ʾ�����ݵ�������
		m_liCurSec.QuadPart = dfile.GetStartSec();
		dfile.Close();
		SetCurPath(strPath);
		//������ͼ
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
	{//�̻߳�û����
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread , 500))
		{//��ʱ 
			DWORD exitCode;
			if(GetExitCodeThread(m_hThread , &exitCode)){
				TerminateThread(m_hThread , exitCode);
			}
		}
	}

	//����ö���߳�
	CloseHandle(m_hThread);
	m_hThread = ::CreateThread(NULL , 0 , EnumFAT32File , this , 0 , NULL);
}

CRuntimeClass* CFat32Doc::GetInofViewClass()
{
	return RUNTIME_CLASS(CFat32InfoView);
}



void CFat32Doc::OnBnClickedPreSector()
{
	//����Ĭ�������б�
	ReSetSectorList();

	if (0 != m_liCurSec.QuadPart )
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		--m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedNextSector()
{
	//��������
	DWORD dwSecCnt = m_pFat32->GetSecCount();

	//����Ĭ�������б�
	ReSetSectorList();

	if (m_liCurSec.QuadPart < dwSecCnt - 1)
	{//��ǰ�������һ������
		++m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedFirstSector()
{
	//����Ĭ�������б�
	ReSetSectorList();

	if (0 != m_liCurSec.QuadPart )
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedLastSector()
{
	//��������
	DWORD dwSecCnt = m_pFat32->GetSecCount();

	//����Ĭ�������б�
	ReSetSectorList();

	if (m_liCurSec.QuadPart != dwSecCnt - 1)
	{//��ǰ�������һ������
		m_liCurSec.QuadPart = dwSecCnt - 1;
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedPreClust()
{
	//��ǰ������
	LONG_INT liCurSec = m_liCurSec;
	//��ǰ���������ڵĴغ�
	DWORD dwCurClust = m_pFat32->SectToClust((DWORD)liCurSec.QuadPart);
	
	//����Ĭ�������б�
	ReSetSectorList();

	if (0 == dwCurClust)
	{//��Ч�Ĵغ�
		return;
	}

	//ǰһ��
	--dwCurClust;
	m_liCurSec.QuadPart = m_pFat32->ClustToSect(dwCurClust);

	//�������е���ͼ
	UpdateAllViews(NULL);

}

void CFat32Doc::OnBnClickedNextClust()
{
	//��ǰ������
	LONG_INT liCurSec = m_liCurSec;
	//��ǰ���������ڵĴغ�
	DWORD dwCurClust = m_pFat32->SectToClust((DWORD)liCurSec.QuadPart);

	//����Ĭ�������б�
	ReSetSectorList();

	if (0 == dwCurClust)//��ǰ���ڵڶ��Ŵ�֮ǰ
		dwCurClust = 2;
	else if (m_pFat32->GetMaxClustNum() == dwCurClust)
	{//�Ѿ������һ����
		return ;
	}else
		++dwCurClust;
	m_liCurSec.QuadPart = m_pFat32->ClustToSect(dwCurClust);

	//�������е���ͼ
	UpdateAllViews(NULL);
}

void CFat32Doc::OnBnClickedFirstClust()
{
	//��ǰ������
	LONG_INT liCurSec = m_liCurSec;
	//��ǰ���������ڵĴغ�
	DWORD dwCurClust = m_pFat32->SectToClust((DWORD)liCurSec.QuadPart);

	//����Ĭ�������б�
	ReSetSectorList();

	if ( 2 != dwCurClust )
	{
		m_liCurSec.QuadPart = m_pFat32->ClustToSect(2);

		//�������е���ͼ
		UpdateAllViews(NULL);
	}
}

void CFat32Doc::OnBnClickedLastClust()
{
	//Ҫ�������һ�صĴغ�
	//��������
	DWORD dwSecCnt = m_pFat32->GetSecCount();
	//��������������
	DWORD firstClustSec = m_pFat32->ClustToSect(2);
	//ÿ��������
	BYTE secPerClust = m_pFat32->GetSecPerClust();

	//����Ĭ�������б�
	ReSetSectorList();

	if (0 == secPerClust)
	{//�·��豸��û�д�
		return ;
	}

	//���������ܴ���
	DWORD dwClustCnt = (dwSecCnt - firstClustSec) / secPerClust;

	//���һ�صĴغ�
	(dwClustCnt += 2)--;

	m_liCurSec.QuadPart = m_pFat32->ClustToSect(dwClustCnt);

	//�������е���ͼ
	UpdateAllViews(NULL);

}

void CFat32Doc::OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CRect	cRect;//�ͻ���
	CPoint	p;	//��굱ǰλ��
	CMenu	menu;
	CMenu*	pMenu;

	::GetWindowRect(AfxGetMainWnd()->GetSafeHwnd()/*m_pContentList->GetParent()->GetSafeHwnd()*//*AfxGetMainWnd()->GetSafeHwnd()*/ , &cRect);
	::GetCursorPos(&p);
	if(!cRect.PtInRect(p)) 
		return ;//ֻ���б������ʾ��ݲ˵�

	//��ݲ˵�
	menu.LoadMenu(IDR_FAT32_FILE_MENU);
	pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN , p.x , p.y , AfxGetMainWnd()/*m_pContentList->GetParent()*//*AfxGetMainWnd()*/ , NULL);
}

void CFat32Doc::OnFat32PosData()
{
	CString		strPath;			//·��
	CString		strName;		//��ѡ����ļ���
	CString		strTemp;
	LONG_INT	liStart = {0};
	DFat32File  dfile;
	DRES		res = DR_OK;

	//��õ�ǰѡ����ļ���·��
	strPath = GetSelPath(strName);


	//�ж�·���Ƿ���Դ�
	res = m_pFat32->OpenFile(strPath , &dfile);
	if (res != DR_OK)
	{//TODO ��ָ�����ļ�����Ŀ¼ʧ��
		strPath.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strPath.Replace(STR_POS  , strName);

		MessageBox(NULL , strPath , strTemp , MB_OK |MB_ICONWARNING );
		return;
	}

	//Ҫ��ʾ�����ݵ�������
	m_liCurSec.QuadPart = dfile.GetStartSec();

	if (0 == m_liCurSec.QuadPart)
	{//�����������ܳ��� Ϊ0��ɽ����
		//Ϊ�վ�ֱ��ʹ�ô��̵� 
		ReSetSectorList();
	}else{
		SectorList* secList = new SectorList();
		secList->m_strName = strPath;  //��������
		secList->AddSector(m_liCurSec , m_pFat32->GetSecPerClust() );   //��ʹ��һ����
		if (FALSE == SetSectorListNoCopy(secList))
		{
			delete secList;
			secList = 0;
		}

		//�����߳�����ȡ����
		m_bIsGetSeclistRun = FALSE;
		if (m_hGetSectorListThread != NULL && m_hGetSectorListThread != INVALID_HANDLE_VALUE)
		{//�̻߳�û����
			if(WAIT_TIMEOUT == WaitForSingleObject(m_hGetSectorListThread , 500))
			{//��ʱ 
				DWORD exitCode;
				if(GetExitCodeThread(m_hGetSectorListThread , &exitCode)){
					TerminateThread(m_hGetSectorListThread , exitCode);
				}
			}
		}

		//����ö���߳�
		CloseHandle(m_hGetSectorListThread);
		m_hGetSectorListThread = ::CreateThread(NULL , 0 , GetFAT32FileSectorList , this , 0 , NULL);
		Sleep(50); //��Ϣһ��
	}


	dfile.Close();			
	//������ͼ
	UpdateAllViews(NULL);
}

void CFat32Doc::OnFat32FileAttr()
{
	CString strName;
	//��õ�ǰѡ����ļ�������·��
	CString strSelPath = GetSelPath(strName);
	if (0 == strSelPath.GetLength())
	{//�·��е㲻�Ծ�����
		return;
	}

	//������Ҫ��ʾ���ļ���·��
	if(FALSE == m_pDlgFileAttr->SetFilePath(strSelPath))
	{//���óɹ�
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_GET_FILE_ATTR_ERR);
		strMsg.Replace(STR_POS , strSelPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//������ʾ�ļ���Ϣ�Ի���
	::ShowWindow(m_pDlgFileAttr->GetSafeHwnd() , SW_SHOW);
}

void CFat32Doc::OnFat32PosParaentDir()
{
	DFat32File		file;
	DRES			res = DR_OK;
	DWORD			parentIndex = 0;		//�ڸ�Ŀ¼�еĶ��ļ�������������
	DWORD			dwClust;				//�غ�
	DWORD			dwSector;
	LONG_INT		liSector;				//������
	LONG_INT		liEnd = {0};
	int				iTemp;
	int				i;
	CString			strName;
	//��ǰ�б��·��  Ҳ����ν�ĸ�Ŀ¼
	CString strCurPath = m_strCurPath;
	//��õ�ǰѡ�е�·��
	CString strSelPath = GetSelPath(strName);
	if (1 == strSelPath.GetLength() && IsPathSeparator(strSelPath.GetAt(0)))
	{//�Ǹ�Ŀ¼
		return;
	}

	//��ָ�����ļ�
	res = m_pFat32->OpenFile(strSelPath , &file);

	if ( DR_OK != res )
	{//��ָ�����ļ�ʧ��
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_OPEN_FALIED);
		strMsg.Replace(STR_POS , strSelPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//��ȡѡ���ļ��ڸ�Ŀ¼�е�λ��
	parentIndex = file.GetParentIndex();
	file.Close();

	//�򿪸�Ŀ¼
	res = m_pFat32->OpenFile(strCurPath , &file);
	dwClust = file.GetStartClust();


	//Ҫ��ʾ�����ݵ�������
	dwSector = m_pFat32->ClustToSect(dwClust);
	if (0 == dwSector)
	{//�����������ܳ��� Ϊ0��ɽ����
		//Ϊ�վ�ֱ��ʹ�ô��̵� 
		ReSetSectorList();
	}
	else
	{
		SectorList* secList = new SectorList();
		secList->m_strName = strCurPath;  //��������
		secList->AddSector(dwSector , m_pFat32->GetSecPerClust() );   //��ʹ��һ����
		if(TRUE == SetSectorListNoCopy(secList))
		{
			//�����߳�����ȡ����
			m_bIsGetSeclistRun = FALSE;
			if (m_hGetSectorListThread != NULL && m_hGetSectorListThread != INVALID_HANDLE_VALUE)
			{//�̻߳�û����
				if(WAIT_TIMEOUT == WaitForSingleObject(m_hGetSectorListThread , 500))
				{//��ʱ 
					DWORD exitCode;
					if(GetExitCodeThread(m_hGetSectorListThread , &exitCode)){
						TerminateThread(m_hGetSectorListThread , exitCode);
					}
				}
			}

			//����ö���߳�
			CloseHandle(m_hGetSectorListThread);
			m_hGetSectorListThread = ::CreateThread(NULL , 0 , GetFAT32FileSectorList , this , 0 , NULL);
			Sleep(50); //��Ϣһ��
		}
		else
		{
			delete secList;
		}
	}

	//����Ŀ¼���ڴغ�  ÿһ��Ŀ¼���32���ֽ�

	//����
	iTemp = (parentIndex * 32) / (m_pFat32->GetSecPerClust() * SECTOR_SIZE);
	//���Ҿ���Ĵغ�
	for (i = 0 ; i < iTemp ; ++i)
		dwClust = m_pFat32->GetFATFromFAT1(dwClust);

	//������
	liSector.QuadPart = m_pFat32->ClustToSect(dwClust);
	liSector.QuadPart +=((parentIndex * 32) / SECTOR_SIZE) % m_pFat32->GetSecPerClust();

	//���õ�ǰ����
	SetCurSector(liSector);
	
	//�ֽ�ƫ��
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += ((parentIndex % (SECTOR_SIZE / 32)) * 32 );
	liEnd.QuadPart = liSector.QuadPart + 32;

	//ѡ��
	SetSel(liSector , liEnd);
}

void CFat32Doc::OnFat32ServeAs()
{//���ļ����������Ᵽ������
	int i = 0;
	int len = 0;
	CString strName;
	//��õ�ǰѡ�е��ļ�
	CString strFilePath = GetSelPath(strName);
	CString strFileName;
	CString	strWrite;
	
	len = strFilePath.GetLength();
	for (i = len - 1 ; i > 0 && !IsPathSeparator(strFilePath.GetAt(i)); --i);
	strFileName = strFilePath.Mid( i + 1 );
	
	//�ļ�ѡ��Ի���
	CFileDialog fDlg(FALSE , NULL , strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if(IDCANCEL == fDlg.DoModal())
	{//ȡ���� 
		return;
	}

	//���Ҫд��·����
	strWrite = fDlg.GetPathName();
	
	//����һ���ļ����ƽ������Ի���

	CCopyProcessDlg ccpDlg(strFilePath , strWrite , this);
	ccpDlg.DoModal();
}

void CFat32Doc::OnUpdateFat32ServeAs(CCmdUI *pCmdUI)
{
	CString strTemp;
	CString strName;
	int nSelItem = m_pContentList->GetSelectionMark();
	if (-1 == nSelItem)
	{ //û��ѡ���κζ���
		return ;
	}

	strName = m_pContentList->GetItemText(nSelItem , 1);

	if (_T('*') != strName.GetAt(0) && IsSelFAT32File(m_pContentList , nSelItem , strTemp))
	{//ѡ������ļ�
		pCmdUI->Enable(TRUE);
	}else//ѡ�����Ŀ¼
		pCmdUI->Enable(FALSE);
}

void CFat32Doc::OnCloseDocument()
{
	// TODO: �ڴ����ר�ô����/����û���
	if (NULL != m_pDlgFileAttr)
	{//���ٴ���
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
	int			pathLen;		//ԭ·���ĳ���

	if (m_pContentList == NULL)//���ﻹû�л���б��ָ��?
		return strPath;
	nItem = m_pContentList->GetSelectionMark();
	if (-1 == nItem) {//û��ѡ���κ���Ч����
		return strPath;
	}

	//����ļ���
	strName = m_pContentList->GetItemText( nItem , 1 );
	strPath = m_strCurPath;
	pathLen = strPath.GetLength();

	if (!IsSelFAT32File(m_pContentList , nItem , strTemp ))
	{//ѡ�����Ŀ¼

		if (0 == strName.Compare(_T(".")))
		{//������ǵ�ǰĿ¼

		}else if(0 == strName.Compare(_T("..")))
		{//Ҫ������Ǹ�Ŀ¼ 
			for ( --pathLen ; pathLen > 0 && !IsPathSeparator(strPath.GetAt(pathLen)) ; --pathLen);
			if (pathLen <= 0)  
				strPath = _T("/");  //�������Ӧ�ò������
			else{
				strPath = strPath.Mid(0 , pathLen);
			}
		}else{
			//����ͨ��Ŀ¼
			if (IsPathSeparator(strPath.GetAt(pathLen - 1)))
				strPath = strPath + strName;
			else
				strPath = strPath + _T("/") + strName;
		}
	} else {  //�������һ��Ŀ¼
		//��һ���ļ�
		if (IsPathSeparator(strPath.GetAt(pathLen - 1)))
			strPath = strPath + strName;
		else
			strPath = strPath + _T("/") + strName;
	}

	return strPath;
}

void CFat32Doc::SetCurFile( CString strPath )
{
	//��Ŀ¼
	CString strParent;
	int i = 0;
	int len = strPath.GetLength();
	DFat32File dfile;
	if (len == 0) return;
	else if(len == 1){
		//��Ŀ¼
		if (m_strCurPath.GetLength() != 0)
		{//ԭ·�����ڸ�Ŀ¼ 
			m_pFat32->OpenFile(strPath  , &dfile);
			m_liCurSec.QuadPart = dfile.GetStartSec();
			dfile.Close();
			//������ͼ
			UpdateAllViews(NULL);
			SetCurPath(strPath);
		}
		//return;
	}else {
		//�Ǹ�Ŀ¼ 
		for(i = len-1 ; (i > 0) &&  !IsPathSeparator(strPath.GetAt(i))  ; --i);
		if (0 == i)
		{
			if (IsPathSeparator(strPath.GetAt(0)))
			{//��Ŀ¼��Ŀ¼
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
		}else  //��һ������Ŀ¼
			strParent = strPath.Left(i);
		
		//�������ж�һ���Ƿ���Ҫ����һ��·��
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
			
		}/*else//�¾�·����ͬ */
			/*SetCurPath(strParent);*/
		if (i != len)
		{//�¾�·����ͬ 
			m_pFat32->OpenFile(strParent  , &dfile);
			m_liCurSec.QuadPart = dfile.GetStartSec();
			dfile.Close();
			//������ͼ
			UpdateAllViews(NULL);
			SetCurPath(strParent);
		}
	}

	//������Ҫ��ʾ���ļ���·��
	if(FALSE == m_pDlgFileAttr->SetFilePath(strPath))
	{//���óɹ�
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_GET_FILE_ATTR_ERR);
		strMsg.Replace(STR_POS , strPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//������ʾ�ļ���Ϣ�Ի���
	::ShowWindow(m_pDlgFileAttr->GetSafeHwnd() , SW_SHOW);
}

void CFat32Doc::OnCheckDeleteFile()
{//�鿴�Ѿ�ɾ�������ļ�
	m_bIsChkDelFileRun = FALSE;

	if (m_hChkDelFile != NULL && m_hChkDelFile != INVALID_HANDLE_VALUE)
	{//�̻߳�û����
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hChkDelFile , 500))
		{//��ʱ 
			DWORD exitCode;
			if(GetExitCodeThread(m_hChkDelFile , &exitCode)){
				TerminateThread(m_hChkDelFile , exitCode);
			}
		}
	}

	//����ö���߳�
	CloseHandle(m_hChkDelFile);
	m_hChkDelFile = ::CreateThread(NULL , 0 , EnumDelFAT32File , this , 0 , NULL);
}

void CFat32Doc::OnUpdateCheckDeleteFile(CCmdUI *pCmdUI)
{
	//�ж����һ����¼�Ƿ��Ǳ��Ѿ�ɾ��
	int cnt = m_pContentList->GetItemCount();
	CString strName = m_pContentList->GetItemText(cnt-1 , 1);
	if (strName.GetAt(0) == _T('*'))
	{//
		pCmdUI->Enable(FALSE);
	}else{
		pCmdUI->Enable(TRUE);
	}
}
