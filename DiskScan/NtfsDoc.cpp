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

//�жϵ�ǰ�б���ѡ����Ƿ���һ��NTFS�ļ�
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
	{//�ͷ���Ӧ����Դ
		delete this->m_pNtfs;
		this->m_pNtfs = NULL;
	}

	if (NULL != m_pImgList)
	{
		delete m_pImgList;
	}

// 	if (this->m_pEveIsRun)
// 	{
// 		m_pEveIsRun->SetEvent();  //�����ȳ�����
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
	//����򿪵���һ����Ļ��;�ֻ��һ����ĵ����� ��"\\\\?\\C:"
	//�����һ������Ļ�����������һ����������ֺ�һ��ƫ�ƣ���"\\\\.\\PhysicalDrive ABCDEF10"
	CString sPath = m_strOpenParam = lpszPathName;
	CString strTemp = _T("");
	CString strOff  =_T("");
	char denName[DEVICE_NAME_LEN + 1] = {0};
	DRES res = DR_OK;
	CView * view;
	WCHAR  volName[MAX_PATH] = {0};

	//����ͼ���б�
	m_pImgList->Create(12,12,ILC_COLORDDB|ILC_MASK , 0 , 1);
	//this->m_pImgList->Create(16 , 16 ,ILC_COLOR32|ILC_MASK , 0 , 4);
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_FILE);//�ļ�  0
	m_pImgList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_FOLDER);//Ŀ¼     1
	m_pImgList->Add(hIcon);	

	//�����Ҫ�������б�
	POSITION pos = GetFirstViewPosition();
	view = 	GetNextView(pos);
	this->m_pContentList = &((CChildFrm*)(view->GetParentFrame()))->m_DisList;
	//���б���г�ʼ��
	InitContentListHead();
	this->m_pContentList->SetImageList(m_pImgList , LVSIL_SMALL);

	//�򵥵����ݳ�ʼ��
	this->m_liCurSec.QuadPart = 0;
	this->m_liStartSec.QuadPart = 0;

	//��������о�Ĵ򿪴���

	//�ͷ����ߵĿո�
	sPath.TrimLeft();	
	sPath.TrimRight();

	//����豸������
	strTemp = GetPathParam(sPath , PT_DEVNAME);
	strOff = GetPathParam(sPath , PT_OFFSET);
	if (strOff.GetLength() != 0) //��ƫ�Ʋ���
		m_liStartSec = HexStrToLONG_INT(strOff);

	//Ҫ�򿪵��豸����
#ifdef  UNICODE
	UnicToMultyByte(strTemp , denName ,DEVICE_NAME_LEN + 1);
#else
	strcpy(denName , strTemp);
#endif

	res = this->m_pNtfs->OpenDev(/*(LPCSTR)(LPCTSTR)strTemp*/denName , &m_liStartSec);
	if (res != DR_OK)
	{//TODO ���豸ʧ��
		sPath.LoadString(IDS_OPEN_FALIED);
		sPath.Replace(STR_POS , lpszPathName);
		strTemp.LoadString(IDS_ERROR);
		::MessageBox(NULL , sPath  , strTemp  , MB_OK|MB_ICONERROR);
		this->m_strTitle = _T("");
		return FALSE;
	}

	//���豸�ɹ�
	strTemp = GetPathParam(sPath , PT_INDEX);
	this->m_strTitle = denName;
	if (strTemp.GetLength() != 0)
	{
		this->m_strTitle += _T("->");
		this->m_strTitle += strTemp;
	}	

	//���Ͼ��
	this->m_strTitle += _T(" ");
	this->m_pNtfs->GetVolumeName( volName , MAX_PATH );
	this->m_strTitle += volName;

	//������ʾ��Χ
	this->m_secList.AddSector(0 , m_pNtfs->GetSecCount());
	this->m_secList.m_strName = m_strTitle;
	
	//this->SetSectorList(&m_secList);
	
	//���õ�ǰҪ��ʾ���б�
	SetCurPath(_T("/"));

	if (NULL == this->m_pNtfsFileDlg)
	{//���ԶԻ���û�д���
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
	//����ͼƬ�б�

	//  ID  �ļ��� MFT��¼ ʵ�ʴ�С  �����С ����ʱ�� MFT�޸�ʱ��  �����޸�ʱ�� ����ʱ�� 
	strHead.LoadString(IDS_ID);
	m_pContentList->InsertColumn( 0, strHead , LVCFMT_LEFT, 30 );//������

	//�ļ���
	strHead.LoadString(IDS_FILE_NAME);
	m_pContentList->InsertColumn( 1, strHead , LVCFMT_LEFT, 80 );

	//MFT��¼��
	strHead.LoadString(IDS_MFT_RECODE);
	m_pContentList->InsertColumn( 2, strHead , LVCFMT_RIGHT, 70 );

	//�ļ����ݵ�ʵ�ʴ�С
	strHead.LoadString(IDS_REAL_SIZE);
	m_pContentList->InsertColumn( 3, strHead , LVCFMT_RIGHT, 60 );

	//�ļ��ķ����С   ��������СΪ0������ʵ�ʴ�С����0�Ļ��ļ�������λ��פ
	strHead.LoadString(IDS_ALLOC_SIZE);
	m_pContentList->InsertColumn( 4, strHead , LVCFMT_RIGHT, 60 );

	//����ʱ��
	strHead.LoadString(IDS_CREATE_TIME);
	m_pContentList->InsertColumn( 5, strHead , LVCFMT_LEFT, 140 );

	//MFT�޸�ʱ�� IDS_MFT_MODIFY_TIME
	strHead.LoadString(IDS_MFT_MODIFY_TIME);
	m_pContentList->InsertColumn( 6, strHead , LVCFMT_LEFT, 140 );

	//�ļ����޸�ʱ��
	strHead.LoadString(IDS_FILE_MODIFY_TIME);
	m_pContentList->InsertColumn( 7, strHead , LVCFMT_LEFT, 140 );

	//����ʱ��
	strHead.LoadString(IDS_ACCESS_TIME);
	m_pContentList->InsertColumn( 8, strHead , LVCFMT_LEFT, 80 );
}

void CNtfsDoc::SetCurPath( CString path )
{
	this->m_strCurPath = path;

//	m_pEveIsRun->SetEvent();  //�����¼�  ��ʾ��Ҫ�˳������߳�

	if (m_hThread != NULL && m_hThread != INVALID_HANDLE_VALUE)
	{//�̻߳�û����
		m_bIsRun = FALSE;
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread , 500))
		{//��ʱ 
			DWORD exitCode;
			if(GetExitCodeThread(m_hThread , &exitCode)){
				TerminateThread(m_hThread , exitCode);
			}
		}
	}

	//���������߳�
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
	
	//���ﻹû�л���б��ָ��
	if (this->m_pContentList == NULL) return ;
	
	//����б�ĵ�һѡ����
	pos = m_pContentList->GetFirstSelectedItemPosition();
	if (pos == NULL){//û��ѡ���κ�����
		TRACE0("No items were selected!\n");
		*pResult = 0;
		return ;
	}
	
	//���ѡ�����������
	nItem = m_pContentList->GetNextSelectedItem(pos);
	sRealSize = m_pContentList->GetItemText(nItem , 3 );  //ͨ��ʵ�ʴ�С���ж��Ƿ������ļ�
	if (0 == sRealSize.GetLength())
	{//ѡ�����Ŀ¼
		path = this->m_strCurPath;
		iTemp = path.GetLength();
		
		if (iTemp < 1) return; //TODO  Ӧ������������
		
		//����ļ���
		strName = m_pContentList->GetItemText(nItem , 1 );
		if (0 == strName.Compare(_T(".")))
		{//������ǵ�ǰĿ¼
			
		}else if(0 == strName.Compare(_T("..")))
		{//Ҫ������Ǹ�Ŀ¼ 
			for ( --iTemp ; iTemp > 0 && !IsPathSeparator(path.GetAt(iTemp)) ; --iTemp);
			if (iTemp <= 0)  
				path = _T("/");  //�������Ӧ�ò������
			else{
				path = path.Mid(0 , iTemp);
			}
			
		}else{
			//��ҵ��ͨ���ļ�
			if (IsPathSeparator(path.GetAt(iTemp - 1)))
				path = path + strName;
			else
				path = path + _T("/") + strName;
		}

		//�ж�·���Ƿ���Դ�
		res = this->m_pNtfs->OpenFile((LPCTSTR)path , &nfile);
		if (res != DR_OK)
		{//TODO ��ָ�����ļ�����Ŀ¼ʧ��
			path.LoadString(IDS_OPEN_FALIED);
			sTemp.LoadString(IDS_PROMPT);
			path.Replace(STR_POS  , strName);

			MessageBox(NULL , path , sTemp , MB_OK |MB_ICONWARNING );
		}else{
			//�򿪳ɹ�
			//Ҫ��ʾ�����ݵ�������
			this->m_liCurSec = nfile.GetMftStartSec();
			nfile.Close();
			SetCurPath(path);

			//������ͼ
			UpdateAllViews(NULL);
		}		
	}	
}

void CNtfsDoc::OnClickContextList( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = 0;
	
	//��λ��MFT��¼λ��
	OnNtfsPosMft();
}

CString CNtfsDoc::GetSelPath(CString &strName)
{
	int			nItem = 0;
	CString		strTemp;
	//CString		strName;
	CString		strPath = _T("/");
	int			nLen = 0;
		 
	//���ﻹû�л���б��ָ��
	if (this->m_pContentList == NULL) return strPath;
	//���ѡ�����������
	nItem = m_pContentList->GetSelectionMark();
	if(-1 == nItem) return strPath;//û��ѡ���κ�һ��

	//���ѡ���˵��ļ���
	strName = m_pContentList->GetItemText(nItem , 1 );

	//ԭ·��
	strPath = this->m_strCurPath;
	//ԭ·���ĳ���
	nLen = strPath.GetLength();

	if (!IsSelNTFSFile(m_pContentList , nItem , strTemp))
	{//ѡ�����Ŀ¼

		if (0 == strName.Compare(_T(".")))
		{//������ǵ�ǰĿ¼
			
		}else if(0 == strName.Compare(_T("..")))
		{//Ҫ������Ǹ�Ŀ¼ 
			for ( --nLen ; nLen > 0 && !IsPathSeparator(strPath.GetAt(nLen)) ; --nLen);
			if (nLen <= 0)  
				strPath = _T("/");  //�������Ӧ�ò������
			else{
				strPath = strPath.Mid(0 , nLen);
			}
		}else{
			//����ͨ��Ŀ¼
			if (IsPathSeparator(strPath.GetAt(nLen - 1)))
				strPath = strPath + strName;
			else
				strPath = strPath + _T("/") + strName;
		}
	} else {  //�������һ���ļ� 
		//��һ���ļ�
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
	/*AfxMessageBox(_T("��һ����"));*/
	if (0 != this->m_liCurSec.QuadPart )
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		--this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedNextSector()
{
	/*	AfxMessageBox(_T("��һ����"));*/
	//��������
	LONG_INT liSecCnt = this->m_pNtfs->GetSecCount();

	ReSetSectorList();

	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//��ǰ�������һ������
		++this->m_liCurSec.QuadPart;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedFirstSector()
{
	ReSetSectorList();
	/*A
	fxMessageBox(_T("��һ������"));*/
	if (0 != this->m_liCurSec.QuadPart )
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		this->m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedLastSector()
{
	/*	AfxMessageBox(_T("���һ������"));*/
	//��������
	LONG_INT liSecCnt = this->m_pNtfs->GetSecCount();

	ReSetSectorList();

	if (this->m_liCurSec.QuadPart != liSecCnt.QuadPart - 1)
	{//��ǰ�������һ������
		this->m_liCurSec.QuadPart = liSecCnt.QuadPart - 1;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedPreClust()
{
	BYTE secPerClust = this->m_pNtfs->GetSecPerClust();
	LONG_INT liClust = {0};
	liClust.QuadPart = this->m_liCurSec.QuadPart /secPerClust; //�غ�

	ReSetSectorList();

	if (0 !=  liClust.QuadPart)
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		this->m_liCurSec.QuadPart = (--liClust.QuadPart) *  secPerClust;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedNextClust()
{
	BYTE secPerClust = this->m_pNtfs->GetSecPerClust();
	LONG_INT liClust = {0};			//��ǰ���ڵĴغ�
	LONG_INT liClustCnt = {0};		//����
	liClust.QuadPart = this->m_liCurSec.QuadPart /secPerClust; //�غ�
	liClustCnt.QuadPart = m_pNtfs->GetSecCount().QuadPart / secPerClust;

	ReSetSectorList();

	if ( liClust.QuadPart < liClustCnt.QuadPart - 1)
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		this->m_liCurSec.QuadPart = (++liClust.QuadPart) *  secPerClust;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedFirstClust()
{
	this->ReSetSectorList();
	if (0 != this->m_liCurSec.QuadPart )
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		this->m_liCurSec.QuadPart = 0;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnBnClickedLastClust()
{
	BYTE secPerClust = this->m_pNtfs->GetSecPerClust();
	LONG_INT liClust = {0};			//��ǰ���ڵĴغ�
	LONG_INT liClustCnt = {0};		//����
	liClust.QuadPart = this->m_liCurSec.QuadPart /secPerClust; //�غ�
	liClustCnt.QuadPart = m_pNtfs->GetSecCount().QuadPart / secPerClust;

	ReSetSectorList();

	if ( liClust.QuadPart < liClustCnt.QuadPart)
	{//��ǰ��ʾ�Ĳ��ǵ�һ����
		this->m_liCurSec.QuadPart = (--liClustCnt.QuadPart) *  secPerClust;
		UpdateAllViews(NULL);
	}
}

void CNtfsDoc::OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CRect	cRect;//�ͻ���
	CPoint	p;	//��굱ǰλ��
	CMenu	menu;
	CMenu*	pMenu;

	::GetWindowRect(AfxGetMainWnd()->GetSafeHwnd()/*this->m_pContentList->GetParent()->GetSafeHwnd()*/ , &cRect);
	::GetCursorPos(&p);
	if(!cRect.PtInRect(p)) 
		return ;//ֻ���б������ʾ��ݲ˵�

	//��ݲ˵�
	menu.LoadMenu(IDR_NTFS_FILE_MENU);
	pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN , p.x , p.y , AfxGetMainWnd()/*this->m_pContentList->GetParent()*/ , NULL);
}

void CNtfsDoc::OnNtfsFileAttr()
{
	DRES res = DR_OK;
	DNtfsFile   nfile;
	CString		strTemp;
	//��õ�ǰѡ����ļ�������·��
	CString strName;
	CString strSelPath = this->GetSelPath(strName);
	if (0 == strSelPath.GetLength())
	{//�·��е㲻�Ծ�����
		return;
	}

	//������Ҫ��ʾ���ļ���·��
	if(FALSE == this->m_pNtfsFileDlg->SetFilePath(strSelPath))
	{//���óɹ�
		CString strTitle;
		CString strMsg;

		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_GET_FILE_ATTR_ERR);
		strMsg.Replace(STR_POS , strSelPath);
		::MessageBox(NULL , strMsg , strTitle , MB_OK|MB_ICONWARNING);
		return;
	}

	//�ж�·���Ƿ���Դ�
	res = this->m_pNtfs->OpenFileA((LPCSTR)(LPCTSTR)strSelPath , &nfile);
	if (res != DR_OK)
	{//TODO ��ָ�����ļ�����Ŀ¼ʧ��
		strName.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strName.Replace(STR_POS  , strSelPath);

		MessageBox(NULL , strName , strTemp , MB_OK |MB_ICONWARNING );
		return;
	}

	//�����б�
	SectorList secList;
	secList.AddSector(nfile.GetMftStartSec() , 2 );  //һ��ÿһ��MFT��¼��1K(2������)
	secList.m_strName = strSelPath;
	this->SetSectorList(&secList);

	//������ʾ�ļ���Ϣ�Ի���
	::ShowWindow(m_pNtfsFileDlg->GetSafeHwnd() , SW_SHOW);
}


void CNtfsDoc::OnNtfsPosMft()
{
	DNtfsFile   nfile;
	DRES		res = DR_OK;
	CString		strPath;
	CString		strTemp;
	CString		strName;
	
	//��õ�ǰѡ�е��ļ���Ŀ¼��·��
	strPath = GetSelPath(strName);

	//�ж�·���Ƿ���Դ�
	res = this->m_pNtfs->OpenFile((LPCTSTR)strPath , &nfile);
	if (res != DR_OK)
	{//TODO ��ָ�����ļ�����Ŀ¼ʧ��
		strName.LoadString(IDS_OPEN_FALIED);
		strTemp.LoadString(IDS_PROMPT);
		strName.Replace(STR_POS  , strPath);
		
		MessageBox(NULL , strName , strTemp , MB_OK |MB_ICONWARNING );
		return;
	}

	//Ҫ��ʾ�����ݵ�������
	this->m_liCurSec = nfile.GetMftStartSec();

	//�����б�
	SectorList secList;
	secList.AddSector(m_liCurSec , 2 );  //һ��ÿһ��MFT��¼��1K(2������)
	secList.m_strName = strPath;
	this->SetSectorList(&secList);

	nfile.Close();			
	//������ͼ
	UpdateAllViews(NULL);
}

void CNtfsDoc::OnNtfsPosParaentDir()
{
 	DNtfsFile		file;
 	DRES			res = DR_OK;
 	LONG_INT		liSector;				//������
	LONG_INT		liEnd;
 	CString			strName;
	//��ǰ�б��·��  Ҳ����ν�ĸ�Ŀ¼
	CString strCurPath = this->m_strCurPath;
	//��õ�ǰѡ�е�·��
	CString strSelPath = GetSelPath(strName);
	if (1 == strSelPath.GetLength() && IsPathSeparator(strSelPath.GetAt(0)))
	{//�Ǹ�Ŀ¼
		return;
	}

	//��ָ�����ļ�
	res = this->m_pNtfs->OpenFileA(strSelPath , &file);

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

	//ȫ�̲鿴ģʽ
	ReSetSectorList();

	liSector = file.GetFDTOffset();
	liSector.QuadPart /= SECTOR_SIZE;
	//���õ�ǰ����
	this->SetCurSector(liSector);

	//�ֽ�ƫ��
	liSector = file.GetFDTOffset();
	liEnd.QuadPart = liSector.QuadPart + file.GetFDTLen();

	//ѡ��
	this->SetSel(liSector , liEnd);
}


void CNtfsDoc::OnNtfsServeAs()
{
	int i = 0;
	int len = 0;
	CString	strName;
	//��õ�ǰѡ�е��ļ�
	CString strFilePath = this->GetSelPath(strName);
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


void CNtfsDoc::OnUpdateNtfsServeAs(CCmdUI *pCmdUI)
{//ֻ���ļ��ſ����Ϊ 
	CString strTemp;
	int nSelItem = this->m_pContentList->GetSelectionMark();
	if (-1 == nSelItem)
	{ //û��ѡ���κζ���
		return ;
	}

	if (IsSelNTFSFile(m_pContentList , nSelItem , strTemp))
	{//ѡ������ļ�
		pCmdUI->Enable(TRUE);
	}else//ѡ�����Ŀ¼
		pCmdUI->Enable(FALSE);
}


void CNtfsDoc::OnCloseDocument()
{
	// TODO: �ڴ����ר�ô����/����û���

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
	//��Ŀ¼
	CString strParent;
	int i = 0;
	int len = strPath.GetLength();
	DNtfsFile   nfile;
	if (len == 0) return;
	else if(len == 1){
		//��Ŀ¼
		if (this->m_strCurPath.GetLength() != 1 )
		{//��ͬ  �����Ͳ��ڸ�Ŀ¼
			SetCurPath(strPath);
			this->m_pNtfs->OpenFile(strPath  , &nfile);
			this->m_liCurSec = nfile.GetMftStartSec();
			nfile.Close();
			//������ͼ
			UpdateAllViews(NULL);
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
		i = len + 1; //ʹi != len  
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
			
			this->m_pNtfs->OpenFile(strParent  , &nfile);
			this->m_liCurSec = nfile.GetMftStartSec();
			nfile.Close();
			//������ͼ
			UpdateAllViews(NULL);
			SetCurPath(strParent);
		}
	}

	//������Ҫ��ʾ���ļ���·��
	if(FALSE == this->m_pNtfsFileDlg->SetFilePath(strPath))
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
	::ShowWindow(this->m_pNtfsFileDlg->GetSafeHwnd() , SW_SHOW);
}
