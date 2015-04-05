// NtfsFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DiskScan.h"
#include "NtfsFileDlg.h"


// CNtfsFileDlg 对话框

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

	//先不管有没有打开，关闭一下，避免资源泄露
	m_upFile->Close();
	//打开指定的文件
	res = pNtfs->OpenFile(strSelPath, m_upFile.get());
	if (DR_OK != res)
	{//打开指定的文件失败
		return FALSE;
	}

	m_strFilePath = strSelPath;
	//将数据重新显示出来
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
	DNtfsFile::PAttrItem pAttrItem = NULL;  //文件属性节点
	CString		strTemp;
	int			nTemp;
	DNtfsAttr	ntfsAttr;
	WCHAR		wBuf[100] = {0};
	LONG_INT	mft = {0};

	//显示路径
	pWnd = this->GetDlgItem(IDC_FILE_PATH);
	::SetWindowText(pWnd->GetSafeHwnd() , this->m_strFilePath);

	//获得文件名
	nCunt = m_strFilePath.GetLength();
	for ( i = nCunt - 1 ; ( i > 0 ) && !IsPathSeparator(m_strFilePath.GetAt(i)); --i );
	this->SetWindowText(m_strFilePath.Mid(1 + i));


	//获得文件属性数量
	nCunt = m_upFile->GetAttrCount();
	//加载属性列表
	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	pList->DeleteAllItems();
	for (i = 0 ; i < nCunt; ++i)
	{
		//获得属性对象
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
		
		//属性ID
		strTemp.Format(_T("%d") , pAttrItem->id);
		pList->InsertItem(i , strTemp);

		//属性类型 
		strTemp = GetNtfsAttrTypeName(pAttrItem->attrType);
		pList->SetItemText(i , 1 , strTemp );

		//属性名
		ntfsAttr.GetAttrName(wBuf , 100);
		strTemp = wBuf;
		pList->SetItemText(i , 2 , strTemp );

		//字节数
		nTemp = ntfsAttr.GetAllLen();
		strTemp.Format(_T("%X") , nTemp);
		pList->SetItemText(i , 3 , strTemp );

		//是否常驻
		if(FALSE == ntfsAttr.IsNonResident())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 4 , strTemp );
		}

		//压缩
		if(ntfsAttr.IsCompressed())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 5 , strTemp );
		}

		//稀疏
		if(ntfsAttr.IsSparse())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 6 , strTemp );
		}

		//加密
		if(ntfsAttr.IsEncrypted())
		{
			strTemp.LoadString(IDS_YES_HOOK);
			pList->SetItemText(i , 7 , strTemp );
		}
	}

	//更新dos属性
	UpdateDosAttr();

	//更新MFT记录号
	pWnd = this->GetDlgItem(IDC_MFT_NUM);
	mft = m_upFile->GetMftIndex();
	mft.HighPart?strTemp.Format(_T("%X%08X") , mft.HighPart , mft.LowPart):strTemp.Format(_T("%X") , mft.LowPart);
	pWnd->SetWindowText(strTemp);

	//更新父目录记录号
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
	
	//属性ID
	strTemp.LoadString(IDS_ATTR_ID);
	pList->InsertColumn(0 , strTemp , LVCFMT_LEFT , 50);

	//属性类型 
	strTemp.LoadString(IDS_ATTR_TYPE);
	pList->InsertColumn(1 , strTemp , LVCFMT_LEFT , 150);
	
	//属性名
	strTemp.LoadString(IDS_ATTR_NAME);
	pList->InsertColumn(2 , strTemp , LVCFMT_LEFT , 70);

	//字节数
	strTemp.LoadString(IDS_ATTR_SIZE);
	pList->InsertColumn(3 , strTemp , LVCFMT_LEFT , 90);

	//是否常驻
	strTemp.LoadString(IDS_ATTR_RESIDENT);
	pList->InsertColumn(4 , strTemp , LVCFMT_LEFT , 40);

	//压缩
	strTemp.LoadString(IDS_ATTR_COMPRESSED);
	pList->InsertColumn(5 , strTemp , LVCFMT_LEFT , 40);
	
	//稀疏
	strTemp.LoadString(IDS_ATTR_SPARSE);
	pList->InsertColumn(6 , strTemp , LVCFMT_LEFT , 40);
	
	//加密
	strTemp.LoadString(IDS_ATTR_ENCRYPTED);
	pList->InsertColumn(7 , strTemp , LVCFMT_LEFT , 40);

	dwStyle = pList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮
	dwStyle |= LVS_EX_GRIDLINES;	//网格线
	pList->SetExtendedStyle(dwStyle); //设置扩展风格

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNtfsFileDlg::OnNMClickNtfsFileAttrList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//处理列表的的点击事件 
	CString strTemp;
	LONG_INT liEnd = {0};
	DWORD	nAttrID = 0;
	int		nAttrCnt = 0;
	int		i;
	LONG_INT liSector = {0};
	DNtfsFile::PAttrItem pAttrItem = NULL;  //文件属性节点
	DNtfsAttr	ntfsAttr;
	CListCtrl* pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//获得属性ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);

	//获得具体的属性
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //这是不允许出现的情况

	//获得实际的扇区号
	liSector.QuadPart = this->m_pDoc->m_pNtfs->GetSectorOfMFTRecode(
		pAttrItem->mftIndex).QuadPart +  pAttrItem->off / SECTOR_SIZE;
	
	//设置当前扇区号
	this->m_pDoc->SetCurSector(liSector);

	//选择
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += (pAttrItem->off % SECTOR_SIZE);
	liEnd.QuadPart = liSector.QuadPart + ntfsAttr.GetAllLen();
	//设置选择区域
	this->m_pDoc->SetSel(liSector , liEnd);
}

void CNtfsFileDlg::UpdateDosAttr()
{
	DNtfsAttr	ntfsAttr;
	DRES		res		= DR_OK;
	DWORD		dwFlags = 0;
	CWnd*		pWnd	= NULL;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //文件属性节点

	//标准属性中的dos属性
	pAttrItem = m_upFile->FindAttribute(AD_STANDARD_INFORMATION);
	if ( NULL != pAttrItem)
	{
		ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
		dwFlags = ntfsAttr.SIGetFlags();
	}
	//文件名的DOS属性
	pAttrItem = m_upFile->FindAttribute(AD_FILE_NAME);
	if ( NULL != pAttrItem)
	{
		ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
		dwFlags |= ntfsAttr.FNGetFlags();
	}
	
	//只读
	pWnd = this->GetDlgItem(IDC_CK_READ_ONLY);
	if (dwFlags & ATTR_READ_ONLY)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//隐藏
	pWnd = this->GetDlgItem(IDC_CK_HIDDEN);
	if (dwFlags & ATTR_HIDDEN)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//归档
	pWnd = this->GetDlgItem(IDC_CK_ARCHIVE);
	if (dwFlags & ATTR_ARCHIVE)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//系统
	pWnd = this->GetDlgItem(IDC_CK_SYSTEM);
	if (dwFlags & ATTR_SYSTEM)
	{
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//稀疏
	pWnd = this->GetDlgItem(IDC_CK_SPARSE);
	if (dwFlags & ATTR_SPARES)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//压缩
	pWnd = this->GetDlgItem(IDC_CK_COMPRESSED);
	if (dwFlags & ATTR_COMPRESSED)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//加密
	pWnd = this->GetDlgItem(IDC_CK_ENCRYPTED);
	if (dwFlags & ATTR_ENCRYPTED)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//目录
	pWnd = this->GetDlgItem(IDC_CK_DIR);
	if (dwFlags & ATTR_DIRECTORY_INDEX)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//临时
	pWnd = this->GetDlgItem(IDC_CK_TEMP);
	if (dwFlags & ATTR_TEMPORARY)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//设备
	pWnd = this->GetDlgItem(IDC_CK_DEVICE);
	if (dwFlags & ATTR_DEVICE)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//常规
	pWnd = this->GetDlgItem(IDC_CK_NORMAL);
	if (dwFlags & ATTR_NORMAL)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

	//多分点
	pWnd = this->GetDlgItem(IDC_CK_REPARSE_POINT);
	if (dwFlags & ATTR_REPARSE_POINT)
		((CButton*)pWnd)->SetCheck(TRUE);
	else
		((CButton*)pWnd)->SetCheck(FALSE);

// #define ATTR_OFFLINE		0x1000		//Offline      1000000000000 
// #define ATTR_NOT_CONTENT_IDX 0x2000//没有索引的index  10000000000000
// #define ATTR_INDEX_VIEW		 0x20000000	//Index View

}

void CNtfsFileDlg::OnBnClickedCkReadOnly()
{
	OnCheckBox(IDC_CK_READ_ONLY);
}

void CNtfsFileDlg::OnCheckBox( DWORD id )
{
	if(BST_CHECKED == ::IsDlgButtonChecked(this->m_hWnd , id))
	{///选中
		::CheckDlgButton(this->m_hWnd , id , BST_UNCHECKED);
	}else{
		///没选中
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
	//处理列表的的点击事件
	CString		strTemp;
	DWORD		nAttrID = 0;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //文件属性节点
	DNtfsAttr	ntfsAttr;
	DWORD		nAttrCnt;
	DWORD		i = 0; 
	LONG_INT	liSector = {0};
	LONG_INT	liEnd = {0};

	//当前列表控件
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	//当前选中的列号
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//获得属性ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);
	
	//获得具体的属性
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //这是不允许出现的情况

	//获得实际的扇区号
	liSector.QuadPart = this->m_pDoc->m_pNtfs->GetSectorOfMFTRecode(
		pAttrItem->mftIndex).QuadPart +  pAttrItem->off / SECTOR_SIZE;

	//设置当前扇区号
	this->m_pDoc->SetCurSector(liSector);

	//选择
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += (pAttrItem->off % SECTOR_SIZE);

	if (ntfsAttr.IsNonResident())
	{//非常驻属性
		liEnd.QuadPart = liSector.QuadPart + ntfsAttr.NR_GetStdHeadLen();
	}else{
		//常驻属性
		liEnd.QuadPart = liSector.QuadPart + ntfsAttr.R_GetStdHeadLen();
	}

	//设置选择区域
	this->m_pDoc->SetSel(liSector , liEnd);
}

void CNtfsFileDlg::OnSeverAttr()
{
	DWORD		i = 0;
	int			len = 0;
	//获得当前选中的文件
	CString		strFilePath = this->m_strFilePath;
	CString		strFileName;
	CString		strWrite;
	CListCtrl*	pList = NULL;
	CString		strTemp;
	DWORD		nAttrID;
	HANDLE		hFile;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //文件属性节点
	DNtfsAttr	ntfsAttr;
	DWORD		nAttrCnt;
	DWORD		dwWriten;

	len = strFilePath.GetLength();
	for (i = len - 1 ; i > 0 && !IsPathSeparator(strFilePath.GetAt(i)); --i);
	strFileName = strFilePath.Mid( i + 1 );
	if (strFileName.GetLength() == 1 && (IsPathSeparator(strFileName.GetAt(0))))
	{//是根目录
		strFileName = _T("root");
	}
	//当前列表控件
	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	//当前选中的列号
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//获得属性ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);
	//属性类型
	strTemp = pList->GetItemText(nItem , 1);
	strFileName += (_T("_") + strTemp);
	//属性名
	strTemp = pList->GetItemText(nItem , 2);
	if (strTemp.GetLength())
	{
		strFileName += (_T("_") + strTemp);
	}
	


	//文件选择对话框
	CFileDialog fDlg(FALSE , NULL , strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if(IDCANCEL == fDlg.DoModal())
	{//取消了 
		return;
	}

	//获得要写的路径名
	strWrite = fDlg.GetPathName();

	//获取指定的属性
	//获得具体的属性
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //这是不允许出现的情况
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());


	//打开要写的文件
	hFile = ::CreateFile(strWrite , GENERIC_WRITE , FILE_SHARE_READ , NULL , CREATE_ALWAYS ,FILE_ATTRIBUTE_NORMAL , NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{//打开将要写的文件失败 
		CString strTitle;
		CString	strMsg;
		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_OPEN_FALIED);
		strMsg.Replace(STR_POS , strWrite);
		::MessageBox(this->GetSafeHwnd(), strMsg , strTitle , MB_OK | MB_ICONWARNING );
		return ;
	}

	//将数据写到文件
	if (FALSE == ::WriteFile(hFile, pAttrItem->attrDataBuf.data(),
		ntfsAttr.GetAllLen() , &dwWriten , NULL ))
	{//读取属性失败
		CString strTitle;
		CString	strMsg;
		strTitle.LoadString(IDS_PROMPT);
		strMsg.LoadString(IDS_READ_ATTR_FAILED);
		::MessageBox(this->GetSafeHwnd(), strMsg , strTitle , MB_OK | MB_ICONWARNING );		
		::CloseHandle(hFile);
		return ;
	}
	
	//复制成功
	CString strTitle;
	CString	strMsg;
	strTitle.LoadString(IDS_PROMPT);
	strMsg.LoadString(IDS_READ_ATTR_SUCCESS);
	::MessageBox(this->GetSafeHwnd(), strMsg , strTitle , MB_OK );	
	::CloseHandle(hFile);
}


void CNtfsFileDlg::OnPosStdAttrData()
{
	//处理列表的的点击事件
	CString		strTemp;
	DWORD		nAttrID = 0;
	DNtfsFile::PAttrItem pAttrItem = NULL;  //文件属性节点
	DNtfsAttr	ntfsAttr;
	DWORD		nAttrCnt;
	DWORD		i = 0; 
	LONG_INT	liSector = {0};
	LONG_INT	liEnd = {0};

	//当前列表控件
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_NTFS_FILE_ATTR_LIST);
	//当前选中的列号
	int nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;

	//获得属性ID
	strTemp = pList->GetItemText(nItem , 0);
	nAttrID = atoi((LPCSTR)(LPCTSTR)strTemp);

	//获得具体的属性
	nAttrCnt = m_upFile->GetAttrCount();
	for (i = 0 ; i < nAttrCnt ; ++i )
	{
		pAttrItem = m_upFile->GetAttr((DWORD)i);
		if (NULL == pAttrItem) ASSERT(FALSE);
		if (pAttrItem->id == nAttrID)
			break;
	}
	if (i == nAttrCnt) ASSERT(FALSE);   //这是不允许出现的情况

	//获得实际的扇区号
	liSector.QuadPart = this->m_pDoc->m_pNtfs->GetSectorOfMFTRecode(
		pAttrItem->mftIndex).QuadPart +  pAttrItem->off / SECTOR_SIZE;

	//设置当前扇区号
	this->m_pDoc->SetCurSector(liSector);

	//选择
	ntfsAttr.InitAttr(pAttrItem->attrDataBuf.data());
	liSector.QuadPart *= SECTOR_SIZE;
	liSector.QuadPart += (pAttrItem->off % SECTOR_SIZE);
	liEnd.QuadPart = liSector.QuadPart + ntfsAttr.GetAllLen();
	if (ntfsAttr.IsNonResident())
	{//非常驻属性
		liSector.QuadPart += ntfsAttr.NR_GetStdHeadLen();
	}else{
		//常驻属性
		liSector.QuadPart += ntfsAttr.R_GetStdHeadLen();
	}

	//设置选择区域
	this->m_pDoc->SetSel( liSector , liEnd );
}

void CNtfsFileDlg::OnNMRClickNtfsFileAttrList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CRect	cRect;//客户区
	CPoint	p;	//鼠标当前位置
	CMenu	menu;
	CMenu*	pMenu;

	::GetWindowRect(this->GetSafeHwnd()/*AfxGetMainWnd()->GetSafeHwnd()*/ , &cRect);
	::GetCursorPos(&p);
	if(!cRect.PtInRect(p)) 
		return ;//只在列表空中显示快捷菜单

	//快捷菜单
	menu.LoadMenu(IDR_NTFS_FILE_ATTR_LIST_MENU);
	pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN , p.x , p.y , this/*AfxGetMainWnd()*/ , NULL);
}
