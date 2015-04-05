// NtfsFileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DiskScan.h"
#include "NtfsFileDlg.h"


// CNtfsFileDlg �Ի���

IMPLEMENT_DYNAMIC(CNtfsFileDlg, CDialog)

CNtfsFileDlg::CNtfsFileDlg(CNtfsDoc* pDoc ,CWnd* pParent /*=NULL*/)
	: CDialog(CNtfsFileDlg::IDD, pParent)
	, m_pDoc(pDoc)
{
	m_upFile.reset(new DNtfsFile());

}

CNtfsFileDlg::~CNtfsFileDlg()
{
	if (m_upFile)
		m_upFile->Close();
}

void CNtfsFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNtfsFileDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_NTFS_FILE_ATTR_LIST, OnNMClickNtfsFileAttrList)
	ON_BN_CLICKED(IDC_CK_READ_ONLY, OnBnClickedCkReadOnly)
	ON_BN_CLICKED(IDC_CK_HIDDEN, OnBnClickedCkHidden)
	ON_BN_CLICKED(IDC_CK_ARCHIVE, OnBnClickedCkArchive)
	ON_BN_CLICKED(IDC_CK_SYSTEM, OnBnClickedCkSystem)
	ON_BN_CLICKED(IDC_CK_SPARSE, OnBnClickedCkSparse)
	ON_BN_CLICKED(IDC_CK_COMPRESSED, OnBnClickedCkCompressed)
	ON_BN_CLICKED(IDC_CK_ENCRYPTED, OnBnClickedCkEncrypted)
	ON_BN_CLICKED(IDC_CK_DIR, OnBnClickedCkDir)
	ON_BN_CLICKED(IDC_CK_TEMP, OnBnClickedCkTemp)
	ON_BN_CLICKED(IDC_CK_DEVICE, OnBnClickedCkDevice)
	ON_BN_CLICKED(IDC_CK_NORMAL, OnBnClickedCkNormal)
	ON_BN_CLICKED(IDC_CK_REPARSE_POINT, OnBnClickedCkReparsePoint)
	ON_COMMAND(ID_POS_STD_ATTR_HEAD, OnPosStdAttrHead)
	ON_COMMAND(ID_SEVER_ATTR, OnSeverAttr)
//	ON_UPDATE_COMMAND_UI(ID_SEVER_ATTR, &CNtfsFileDlg::OnUpdateSeverAttr)
	ON_COMMAND(ID_POS_STD_ATTR_DATA, OnPosStdAttrData)
	ON_NOTIFY(NM_RCLICK, IDC_NTFS_FILE_ATTR_LIST, OnNMRClickNtfsFileAttrList)
END_MESSAGE_MAP()

BOOL CNtfsFileDlg::SetFilePath( CString strSelPath )
{
	DNtfs*		pNtfs;
	DRES		res = DR_OK;
	
	pNtfs = m_pDoc->m_pNtfs.get();

	//�Ȳ�����û�д򿪣��ر�һ�£�������Դй¶
	m_upFile->Close();
	//��ָ�����ļ�
	res = pNtfs->OpenFile(strSelPath, m_upFile.get());
	if (DR_OK != res)
	{//��ָ�����ļ�ʧ��
		return FALSE;
	}

	m_strFilePath = strSelPath;
	//������������ʾ����
	UpdateFileData();

	return TRUE;
}

void CNtfsFileDlg::UpdateFileData()
{
	CString		strTitle;
	CWnd*		pWnd;
	int			nCunt = 0;
	int			i = 0;
	CListCtrl*	pList;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //�ļ����Խڵ�
	CString		strTemp;
	int			nTemp;
	DNtfsAttr	ntfsAttr;
	WCHAR		wBuf[100] = {0};
	LONG_INT	mft = {0};

	//��ʾ·��
	pWnd = this->GetDlgItem(IDC_FILE_PATH);
	::SetWindowText(pWnd->GetSafeHwnd() , this->m_strFilePath);

	//����ļ���
	nCunt = m_strFilePath.GetLength();
	for ( i = nCunt - 1 ; ( i > 0 ) && !IsPathSeparator(m_strFilePath.GetAt(i)); --i );
	this->SetWindowText(m_strFilePath.Mid(1 + i));


	//����ļ���������
	nCunt = m_upFile->GetAttrCount();
	//���������б�
	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	pList->DeleteAllItems();
	for (i = 0 ; i < nCunt; ++i)
	{
		//������Զ���
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
		
		//����ID
		strTemp.Format(_T("%d") , pAttrItem->id);
		pList->InsertItem(i , strTemp);

		//�������� 
		strTemp = GetNtfsAttrTypeName(pAttrItem->attrType);
		pList->SetItemText(i , 1 , strTemp );

		//������
		ntfsAttr.GetAttrName(wBuf , 100);
		strTemp = wBuf;
		pList->SetItemText(i , 2 , strTemp );

		//�ֽ���
		nTemp = ntfsAttr.GetAllLen();
		strTemp.Format(_T("%X") , nTemp);
		pList->SetItemText(i , 3 , strTemp );

		//�Ƿ�פ
		if(FALSE == ntfsAttr.IsNonResident())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 4 , strTemp );
		}

		//ѹ��
		if(ntfsAttr.IsCompressed())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 5 , strTemp );
		}

		//ϡ��
		if(ntfsAttr.IsSparse())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 6 , strTemp );
		}

		//����
		if(ntfsAttr.IsEncrypted())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 7 , strTemp );
		}
	}

	//����dos����
	UpdateDosAttr();

	//����MFT��¼��
	pWnd = this->GetDlgItem(IDC_MFT_NUM);
	mft = m_upFile->GetMftIndex();
	mft.HighPart?strTemp.Format(_T("%X%08X") , mft.HighPart , mft.LowPart):strTemp.Format(_T("%X") , mft.LowPart);
	pWnd->SetWindowText(strTemp);

	//���¸�Ŀ¼��¼��
	pWnd = this->GetDlgItem(IDC_PARENT_MFT);
	mft = m_upFile->GetParentMftIndex();
	mft.HighPart?strTemp.Format(_T("%X%08X") , mft.HighPart , mft.LowPart):strTemp.Format(_T("%X") , mft.LowPart);
	pWnd->SetWindowText(strTemp);
}


BOOL CNtfsFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString	strTemp;
	DWORD dwStyle = 0; 

	CListCtrl* pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	
	//����ID
	strTemp.LoadString(IDS_ATTR_ID);
	pList->InsertColumn(0 , strTemp , LVCFMT_LEFT , 50);

	//�������� 
	strTemp.LoadString(IDS_ATTR_TYPE);
	pList->InsertColumn(1 , strTemp , LVCFMT_LEFT , 150);
	
	//������
	strTemp.LoadString(IDS_ATTR_NAME);
	pList->InsertColumn(2 , strTemp , LVCFMT_LEFT , 70);

	//�ֽ���
	strTemp.LoadString(IDS_ATTR_SIZE);
	pList->InsertColumn(3 , strTemp , LVCFMT_LEFT , 90);

	//�Ƿ�פ
	strTemp.LoadString(IDS_ATTR_RESIDENT);
	pList->InsertColumn(4 , strTemp , LVCFMT_LEFT , 40);

	//ѹ��
	strTemp.LoadString(IDS_ATTR_COMPRESSED);
	pList->InsertColumn(5 , strTemp , LVCFMT_LEFT , 40);
	
	//ϡ��
	strTemp.LoadString(IDS_ATTR_SPARSE);
	pList->InsertColumn(6 , strTemp , LVCFMT_LEFT , 40);
	
	//����
	strTemp.LoadString(IDS_ATTR_ENCRYPTED);
	pList->InsertColumn(7 , strTemp , LVCFMT_LEFT , 40);

	dwStyle = pList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и���
	dwStyle |= LVS_EX_GRIDLINES;	//������
	pList->SetExtendedStyle(dwStyle); //������չ���

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CNtfsFileDlg::OnNMClickNtfsFileAttrList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//�����б�ĵĵ���¼� 
	CString strTemp;
	LONG_INT liEnd = {0};
	DWORD	nAttrID = 0;
	int		nAttrCnt = 0;
	int		i;
	LONG_INT liSector = {0};
	DNtfsFile::PAttrItem pAttrItem = NULL;  //�ļ����Խڵ�
	DNtfsAttr	ntfsAttr;
	CListCtrl* pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//�������ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);

	//��þ��������
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //���ǲ�������ֵ����

	//���ʵ�ʵ�������
	liSector.QuadPart = this->m_pDoc->m_pNtfs->GetSectorOfMFTRecode(
		pAttrItem->mftIndex).QuadPart +  pAttrItem->off / SECTOR_SIZE;
	
	//���õ�ǰ������
	this->m_pDoc->SetCurSector(liSector);

	//ѡ��
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += (pAttrItem->off % SECTOR_SIZE);
	liEnd.QuadPart = liSector.QuadPart + ntfsAttr.GetAllLen();
	//����ѡ������
	this->m_pDoc->SetSel(liSector , liEnd);
}

void CNtfsFileDlg::UpdateDosAttr()
{
	DNtfsAttr	ntfsAttr;
	DRES		res		= DR_OK;
	DWORD		dwFlags = 0;
	CWnd*		pWnd	= NULL;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //�ļ����Խڵ�

	//��׼�����е�dos����
	pAttrItem = m_upFile->FindAttribute(AD_STANDARD_INFORMATION);
	if ( NULL != pAttrItem)
	{
		ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
		dwFlags = ntfsAttr.SIGetFlags();
	}
	//�ļ�����DOS����
	pAttrItem = m_upFile->FindAttribute(AD_FILE_NAME);
	if ( NULL != pAttrItem)
	{
		ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
		dwFlags |= ntfsAttr.FNGetFlags();
	}
	
	//ֻ��
	pWnd = this->GetDlgItem(IDC_CK_READ_ONLY);
	if (dwFlags & ATTR_READ_ONLY)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//����
	pWnd = this->GetDlgItem(IDC_CK_HIDDEN);
	if (dwFlags & ATTR_HIDDEN)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//�鵵
	pWnd = this->GetDlgItem(IDC_CK_ARCHIVE);
	if (dwFlags & ATTR_ARCHIVE)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//ϵͳ
	pWnd = this->GetDlgItem(IDC_CK_SYSTEM);
	if (dwFlags & ATTR_SYSTEM)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//ϡ��
	pWnd = this->GetDlgItem(IDC_CK_SPARSE);
	if (dwFlags & ATTR_SPARES)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//ѹ��
	pWnd = this->GetDlgItem(IDC_CK_COMPRESSED);
	if (dwFlags & ATTR_COMPRESSED)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//����
	pWnd = this->GetDlgItem(IDC_CK_ENCRYPTED);
	if (dwFlags & ATTR_ENCRYPTED)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//Ŀ¼
	pWnd = this->GetDlgItem(IDC_CK_DIR);
	if (dwFlags & ATTR_DIRECTORY_INDEX)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//��ʱ
	pWnd = this->GetDlgItem(IDC_CK_TEMP);
	if (dwFlags & ATTR_TEMPORARY)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//�豸
	pWnd = this->GetDlgItem(IDC_CK_DEVICE);
	if (dwFlags & ATTR_DEVICE)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//����
	pWnd = this->GetDlgItem(IDC_CK_NORMAL);
	if (dwFlags & ATTR_NORMAL)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//��ֵ�
	pWnd = this->GetDlgItem(IDC_CK_REPARSE_POINT);
	if (dwFlags & ATTR_REPARSE_POINT)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

// #define ATTR_OFFLINE		0x1000		//Offline      1000000000000 
// #define ATTR_NOT_CONTENT_IDX 0x2000//û��������index  10000000000000
// #define ATTR_INDEX_VIEW		 0x20000000	//Index View

}

void CNtfsFileDlg::OnBnClickedCkReadOnly()
{
	OnCheckBox(IDC_CK_READ_ONLY);
}

void CNtfsFileDlg::OnCheckBox( DWORD id )
{
	if(BST_CHECKED == ::IsDlgButtonChecked(this->m_hWnd , id))
	{///ѡ��
		::CheckDlgButton(this->m_hWnd , id , BST_UNCHECKED);
	}else{
		///ûѡ��
		::CheckDlgButton(this->m_hWnd , id , BST_CHECKED);
	}
}

void CNtfsFileDlg::OnBnClickedCkHidden()
{
	OnCheckBox(IDC_CK_HIDDEN);
}

void CNtfsFileDlg::OnBnClickedCkArchive()
{
	OnCheckBox(IDC_CK_ARCHIVE);
}

void CNtfsFileDlg::OnBnClickedCkSystem()
{
	OnCheckBox(IDC_CK_SYSTEM);
}

void CNtfsFileDlg::OnBnClickedCkSparse()
{
	OnCheckBox(IDC_CK_SPARSE);
}

void CNtfsFileDlg::OnBnClickedCkCompressed()
{
	OnCheckBox(IDC_CK_COMPRESSED);
}

void CNtfsFileDlg::OnBnClickedCkEncrypted()
{
	OnCheckBox(IDC_CK_ENCRYPTED);
}

void CNtfsFileDlg::OnBnClickedCkDir()
{
	OnCheckBox(IDC_CK_DIR);
}

void CNtfsFileDlg::OnBnClickedCkTemp()
{
	OnCheckBox(IDC_CK_TEMP);
}

void CNtfsFileDlg::OnBnClickedCkDevice()
{
	OnCheckBox(IDC_CK_DEVICE);
}

void CNtfsFileDlg::OnBnClickedCkNormal()
{
	OnCheckBox(IDC_CK_NORMAL);
}

void CNtfsFileDlg::OnBnClickedCkReparsePoint()
{
	OnCheckBox(IDC_CK_REPARSE_POINT);
}

void CNtfsFileDlg::OnPosStdAttrHead()
{
	//�����б�ĵĵ���¼�
	CString		strTemp;
	DWORD		nAttrID = 0;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //�ļ����Խڵ�
	DNtfsAttr	ntfsAttr;
	DWORD		nAttrCnt;
	DWORD		i = 0; 
	LONG_INT	liSector = {0};
	LONG_INT	liEnd = {0};

	//��ǰ�б�ؼ�
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	//��ǰѡ�е��к�
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//�������ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);
	
	//��þ��������
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //���ǲ�������ֵ����

	//���ʵ�ʵ�������
	liSector.QuadPart = this->m_pDoc->m_pNtfs->GetSectorOfMFTRecode(
		pAttrItem->mftIndex).QuadPart +  pAttrItem->off / SECTOR_SIZE;

	//���õ�ǰ������
	this->m_pDoc->SetCurSector(liSector);

	//ѡ��
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += (pAttrItem->off % SECTOR_SIZE);

	if (ntfsAttr.IsNonResident())
	{//�ǳ�פ����
		liEnd.QuadPart = liSector.QuadPart + ntfsAttr.NR_GetStdHeadLen();
	}else{
		//��פ����
		liEnd.QuadPart = liSector.QuadPart + ntfsAttr.R_GetStdHeadLen();
	}

	//����ѡ������
	this->m_pDoc->SetSel(liSector , liEnd);
}

void CNtfsFileDlg::OnSeverAttr()
{
	DWORD		i = 0;
	int			len = 0;
	//��õ�ǰѡ�е��ļ�
	CString		strFilePath = this->m_strFilePath;
	CString		strFileName;
	CString		strWrite;
	CListCtrl*	pList = NULL;
	CString		strTemp;
	DWORD		nAttrID;
	HANDLE		hFile;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //�ļ����Խڵ�
	DNtfsAttr	ntfsAttr;
	DWORD		nAttrCnt;
	DWORD		dwWriten;

	len = strFilePath.GetLength();
	for (i = len - 1 ; i > 0 && !IsPathSeparator(strFilePath.GetAt(i)); --i);
	strFileName = strFilePath.Mid( i + 1 );
	if (strFileName.GetLength() == 1 && (IsPathSeparator(strFileName.GetAt(0))))
	{//�Ǹ�Ŀ¼
		strFileName = _T("root");
	}
	//��ǰ�б�ؼ�
	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	//��ǰѡ�е��к�
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//�������ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);
	//��������
	strTemp = pList->GetItemText(nItem , 1);
	strFileName += (_T("_") + strTemp);
	//������
	strTemp = pList->GetItemText(nItem , 2);
	if (strTemp.GetLength())
	{
		strFileName += (_T("_") + strTemp);
	}
	


	//�ļ�ѡ��Ի���
	CFileDialog fDlg(FALSE , NULL , strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if(IDCANCEL == fDlg.DoModal())
	{//ȡ���� 
		return;
	}

	//���Ҫд��·����
	strWrite = fDlg.GetPathName();

	//��ȡָ��������
	//��þ��������
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //���ǲ�������ֵ����
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());


	//��Ҫд���ļ�
	hFile = ::CreateFile(strWrite , GENERIC_WRITE , FILE_SHARE_READ , NULL , CREATE_ALWAYS ,FILE_ATTRIBUTE_NORMAL , NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{//�򿪽�Ҫд���ļ�ʧ�� 
		CString strTitle;
		CString	strMsg;
		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_OPEN_FALIED);
		strMsg.Replace(STR_POS , strWrite);
		::MessageBox(this->GetSafeHwnd(), strMsg , strTitle , MB_OK | MB_ICONWARNING );
		return ;
	}

	//������д���ļ�
	if (FALSE == ::WriteFile(hFile, pAttrItem->attrDataBuf.data(),
		ntfsAttr.GetAllLen() , &dwWriten , NULL ))
	{//��ȡ����ʧ��
		CString strTitle;
		CString	strMsg;
		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_READ_ATTR_FAILED);
		::MessageBox(this->GetSafeHwnd(), strMsg , strTitle , MB_OK | MB_ICONWARNING );		
		::CloseHandle(hFile);
		return ;
	}
	
	//���Ƴɹ�
	CString strTitle;
	CString	strMsg;
	strTitle.LoadString(IDS_PROMPT);
	strMsg.LoadString(IDS_READ_ATTR_SUCCESS);
	::MessageBox(this->GetSafeHwnd(), strMsg , strTitle , MB_OK );	
	::CloseHandle(hFile);
}


void CNtfsFileDlg::OnPosStdAttrData()
{
	//�����б�ĵĵ���¼�
	CString		strTemp;
	DWORD		nAttrID = 0;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //�ļ����Խڵ�
	DNtfsAttr	ntfsAttr;
	DWORD		nAttrCnt;
	DWORD		i = 0; 
	LONG_INT	liSector = {0};
	LONG_INT	liEnd = {0};

	//��ǰ�б�ؼ�
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	//��ǰѡ�е��к�
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//�������ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);

	//��þ��������
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //���ǲ�������ֵ����

	//���ʵ�ʵ�������
	liSector.QuadPart = this->m_pDoc->m_pNtfs->GetSectorOfMFTRecode(
		pAttrItem->mftIndex).QuadPart +  pAttrItem->off / SECTOR_SIZE;

	//���õ�ǰ������
	this->m_pDoc->SetCurSector(liSector);

	//ѡ��
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += (pAttrItem->off % SECTOR_SIZE);
	liEnd.QuadPart = liSector.QuadPart + ntfsAttr.GetAllLen();
	if (ntfsAttr.IsNonResident())
	{//�ǳ�פ����
		liSector.QuadPart += ntfsAttr.NR_GetStdHeadLen();
	}else{
		//��פ����
		liSector.QuadPart += ntfsAttr.R_GetStdHeadLen();
	}

	//����ѡ������
	this->m_pDoc->SetSel( liSector , liEnd );
}

void CNtfsFileDlg::OnNMRClickNtfsFileAttrList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CRect	cRect;//�ͻ���
	CPoint	p;	//��굱ǰλ��
	CMenu	menu;
	CMenu*	pMenu;

	::GetWindowRect(this->GetSafeHwnd()/*AfxGetMainWnd()->GetSafeHwnd()*/ , &cRect);
	::GetCursorPos(&p);
	if(!cRect.PtInRect(p)) 
		return ;//ֻ���б������ʾ��ݲ˵�

	//��ݲ˵�
	menu.LoadMenu(IDR_NTFS_FILE_ATTR_LIST_MENU);
	pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN , p.x , p.y , this/*AfxGetMainWnd()*/ , NULL);
}
