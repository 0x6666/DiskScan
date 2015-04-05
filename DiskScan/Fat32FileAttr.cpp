// Fat32FileAttr.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DiskScan.h"
#include "Fat32FileAttr.h"


// CFat32FileAttr �Ի���

IMPLEMENT_DYNAMIC(CFat32FileDlg, CDialog)

CFat32FileDlg::CFat32FileDlg( CFat32Doc* pDoc , CWnd* pParent /*=NULL*/)
	: CDialog(CFat32FileDlg::IDD, pParent)
	, m_pDoc(pDoc)
	, m_bIsThreadRun(FALSE)
	, m_hThread(0)
{
	m_pFile = new DFat32File();
}

CFat32FileDlg::~CFat32FileDlg()
{
	if (NULL != m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
	}
	if (INVALID_HANDLE_VALUE != m_hThread)
	{
		::CloseHandle(m_hThread);
	}
}

void CFat32FileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFat32FileDlg, CDialog)
	ON_BN_CLICKED(IDC_OK, OnOK)
	ON_NOTIFY(NM_CLICK, IDC_FAT1_LIST, OnNMClickFat1List)
	ON_NOTIFY(NM_CLICK, IDC_FAT2_LIST, OnNMClickFat2List)
	ON_BN_CLICKED(IDC_CK_VOL_ID, OnBnClickedCkVolId)
	ON_BN_CLICKED(IDC_CK_READ_ONLY,  OnBnClickedCkReadOnly)
	ON_BN_CLICKED(IDC_CK_SYSTEM,  OnBnClickedCkSystem)
	ON_BN_CLICKED(IDC_CK_DIR,  OnBnClickedCkDir)
	ON_BN_CLICKED(IDC_CK_ARTRIVE,  OnBnClickedCkArtrive)
	ON_BN_CLICKED(IDC_CK_HIDDEN,  OnBnClickedCkHidden)
	ON_NOTIFY(NM_DBLCLK, IDC_FAT1_LIST, OnNMDblclkFat1List)
	ON_NOTIFY(NM_DBLCLK, IDC_FAT2_LIST, OnNMDblclkFat2List)
END_MESSAGE_MAP()



// //���õ�ǰҪ��ʾ���ļ���·��

BOOL CFat32FileDlg::SetFilePath( CString file )
{
	DFat32*		pDFat32;
	DRES		res = DR_OK;
	pDFat32 = this->m_pDoc->m_pFat32;

	this->m_pFile->Close();
	//��ָ�����ļ�
	res = pDFat32->OpenFile(file , this->m_pFile);
	if (DR_OK != res)
	{//��ָ�����ļ�ʧ��
		return FALSE;
	}

	m_strFilePath = file;
	//������������ʾ����
	UpdateFileData();

	return TRUE;
}

// ˢ������
int CFat32FileDlg::UpdateFileData(void)
{
	CString		strTitle;
	CWnd*		pWnd;
	BYTE		attr;
	int			nCunt = 0;
	int			i = 0;
	//��ʾ·��
	pWnd = this->GetDlgItem(IDC_FILE_PATH);
	::SetWindowText(pWnd->GetSafeHwnd() , this->m_strFilePath);

	//����ļ���
	nCunt = m_strFilePath.GetLength();
	for ( i = nCunt - 1 ; i > 0 && !IsPathSeparator(m_strFilePath.GetAt(i)); --i );
	this->SetWindowText(m_strFilePath.Mid(1 + i));

	//��ô��ļ�������
	attr = this->m_pFile->GetFileAttr();


	pWnd = this->GetDlgItem(IDC_CK_READ_ONLY);
	if (attr & ATTR_READ_ONLY)
	{//��ֻ���ļ�/Ŀ¼
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_HIDDEN);
	if (attr & ATTR_HIDDEN)
	{//��ֻ���ļ�/Ŀ¼
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_SYSTEM);
	if (attr & ATTR_SYSTEM)
	{//��ֻ���ļ�/Ŀ¼
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_VOL_ID);
	if (attr & ATTR_VOLUME_ID)
	{//��ֻ���ļ�/Ŀ¼
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_DIR);
	if (attr & ATTR_DIRECTORY)
	{//��ֻ���ļ�/Ŀ¼
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_ARTRIVE);
	if (attr & ATTR_ARCHIVE)
	{//��ֻ���ļ�/Ŀ¼
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//�����̼߳��ش���
	this->m_bIsThreadRun = FALSE;  //ֹ֪ͨͣ�߳�
	//Sleep(200);	
	//�ȴ�Ӧ���ڵ��߳̽���
	if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread , 800))
	{//�ֶ��ر��߳���Ч ����ǿ�йر�
		DWORD dwExitCode = 0;
		if(GetExitCodeThread(m_hThread , &dwExitCode ))
			TerminateThread(m_hThread , dwExitCode);
	}
	::CloseHandle(m_hThread);
	m_hThread = ::CreateThread(NULL , 0 , GetFATClustList , this , 0 , NULL);

	return 0;
}

void CFat32FileDlg::OnOK()
{
	CDialog::OnOK();
}

BOOL CFat32FileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString	strTemp;
	DWORD dwStyle = 0; 
	CListCtrl* pList = (CListCtrl*)this->GetDlgItem(IDC_FAT1_LIST);
	strTemp.LoadString(IDS_ID);
	pList->InsertColumn(0 , strTemp , LVCFMT_LEFT , 30);
	strTemp.LoadString(IDS_CLUST_NUM);
	pList->InsertColumn(1 , strTemp , LVCFMT_LEFT , 90);
	dwStyle = pList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и���
	dwStyle |= LVS_EX_GRIDLINES;	//������
	pList->SetExtendedStyle(dwStyle); //������չ���

	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT2_LIST);
	strTemp.LoadString(IDS_ID);
	pList->InsertColumn(0 , strTemp , LVCFMT_LEFT , 30);
	strTemp.LoadString(IDS_CLUST_NUM);
	pList->InsertColumn(1 , strTemp , LVCFMT_LEFT , 90);
	dwStyle = pList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и���
	dwStyle |= LVS_EX_GRIDLINES;	//������
	pList->SetExtendedStyle(dwStyle); //������չ���


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CFat32FileDlg::OnNMClickFat1List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//�б�ؼ�
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT1_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	liTemp = {0};
	LONG_INT	liEnd = {0};

	//���ѡ��Ĵغ�
	//ѡ���˵�����
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//û��ѡ���κ�����
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	//����غ����ڵ�����
	liTemp.QuadPart = (dwClust / (SECTOR_SIZE / 4)) + pFat32->GetReserveSector();

	//�����б� 
	SectorList secList;
	secList.AddSector(pFat32->GetReserveSector() , pFat32->GetSectorPerFAT());
	secList.m_strName.LoadString(IDS_FAT1);
	this->m_pDoc->SetSectorList(&secList);

	//�ƶ���ָ��������
	this->m_pDoc->SetCurSector(liTemp);

	//����غ�����������ƫ�� 
	liTemp.QuadPart *= SECTOR_SIZE;
	liTemp.QuadPart += (dwClust % (SECTOR_SIZE / 4) * 4);

	//ѡ��ָ��������
	liEnd.QuadPart = liTemp.QuadPart + 4;
	this->m_pDoc->SetSel(liTemp ,liEnd );
}

void CFat32FileDlg::OnNMClickFat2List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//�б�ؼ�
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT2_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	liTemp = {0};
	LONG_INT	liEnd = {0};

	//���ѡ��Ĵغ�
	//ѡ���˵�����
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//û��ѡ���κ�����
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	//����غ����ڵ�����
	liTemp.QuadPart = (dwClust / (SECTOR_SIZE / 4)) + pFat32->GetReserveSector() + pFat32->GetSectorPerFAT();


	//�����б� 
	SectorList secList;
	secList.AddSector(pFat32->GetReserveSector() + pFat32->GetSectorPerFAT() , pFat32->GetSectorPerFAT());
	secList.m_strName.LoadString(IDS_FAT2);
	this->m_pDoc->SetSectorList(&secList);


	//�ƶ���ָ��������
	this->m_pDoc->SetCurSector(liTemp);

	//����غ�����������ƫ�� 
	liTemp.QuadPart *= SECTOR_SIZE;
	liTemp.QuadPart += (dwClust % (SECTOR_SIZE / 4) * 4);

	//ѡ��ָ��������
	liEnd.QuadPart = liTemp.QuadPart + 4;
	this->m_pDoc->SetSel(liTemp ,liEnd );
}


BOOL CFat32FileDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CFat32FileDlg::OnCheckBox( DWORD id )
{
	if(BST_CHECKED == ::IsDlgButtonChecked(this->m_hWnd , id))
	{///ѡ��
		::CheckDlgButton(this->m_hWnd , id , BST_UNCHECKED);
	}else{
		///ûѡ��
		::CheckDlgButton(this->m_hWnd , id , BST_CHECKED);
	}
}

void CFat32FileDlg::OnBnClickedCkVolId()
{
	OnCheckBox(IDC_CK_VOL_ID);
}
void CFat32FileDlg::OnBnClickedCkReadOnly()
{
	OnCheckBox(IDC_CK_READ_ONLY);
}

void CFat32FileDlg::OnBnClickedCkSystem()
{
	OnCheckBox(IDC_CK_SYSTEM);
}

void CFat32FileDlg::OnBnClickedCkDir()
{
	OnCheckBox(IDC_CK_DIR);
}

void CFat32FileDlg::OnBnClickedCkArtrive()
{
	OnCheckBox(IDC_CK_ARTRIVE);
}

void CFat32FileDlg::OnBnClickedCkHidden()
{
	OnCheckBox(IDC_CK_HIDDEN);
}

void CFat32FileDlg::OnNMDblclkFat1List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//�б�ؼ�
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT1_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	curSec = {0};

	//���ѡ��Ĵغ�
	//ѡ���˵�����
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//û��ѡ���κ�����
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	SectorList secList;
	dwSector = pFat32->ClustToSect(dwClust) ;
	secList.AddSector(dwSector , pFat32->GetSecPerClust());
	secList.m_strName += this->m_pFile->GetFileName();
	secList.m_strName += (_T("  ") + strTemp);
	this->m_pDoc->SetSectorList(&secList);

	//�ƶ���ָ��������
	curSec.QuadPart = dwSector;
	this->m_pDoc->SetCurSector(curSec);

}

void CFat32FileDlg::OnNMDblclkFat2List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//�б�ؼ�
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT2_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	curSec = {0};

	//���ѡ��Ĵغ�
	//ѡ���˵�����
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//û��ѡ���κ�����
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	SectorList secList;
	dwSector = pFat32->ClustToSect(dwClust) ;
	secList.AddSector(dwSector , pFat32->GetSecPerClust());
	secList.m_strName += this->m_pFile->GetFileName();
	secList.m_strName += (_T("  ") + strTemp);
	this->m_pDoc->SetSectorList(&secList);

	//�ƶ���ָ��������
	curSec.QuadPart = dwSector;
	this->m_pDoc->SetCurSector(curSec);
}
