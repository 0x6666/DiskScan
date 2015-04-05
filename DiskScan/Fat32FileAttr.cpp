// Fat32FileAttr.cpp : 实现文件
//

#include "stdafx.h"
#include "DiskScan.h"
#include "Fat32FileAttr.h"


// CFat32FileAttr 对话框

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



// //设置当前要显示的文件的路径

BOOL CFat32FileDlg::SetFilePath( CString file )
{
	DFat32*		pDFat32;
	DRES		res = DR_OK;
	pDFat32 = this->m_pDoc->m_pFat32;

	this->m_pFile->Close();
	//打开指定的文件
	res = pDFat32->OpenFile(file , this->m_pFile);
	if (DR_OK != res)
	{//打开指定的文件失败
		return FALSE;
	}

	m_strFilePath = file;
	//将数据重新显示出来
	UpdateFileData();

	return TRUE;
}

// 刷新数据
int CFat32FileDlg::UpdateFileData(void)
{
	CString		strTitle;
	CWnd*		pWnd;
	BYTE		attr;
	int			nCunt = 0;
	int			i = 0;
	//显示路径
	pWnd = this->GetDlgItem(IDC_FILE_PATH);
	::SetWindowText(pWnd->GetSafeHwnd() , this->m_strFilePath);

	//获得文件名
	nCunt = m_strFilePath.GetLength();
	for ( i = nCunt - 1 ; i > 0 && !IsPathSeparator(m_strFilePath.GetAt(i)); --i );
	this->SetWindowText(m_strFilePath.Mid(1 + i));

	//获得此文件的属性
	attr = this->m_pFile->GetFileAttr();


	pWnd = this->GetDlgItem(IDC_CK_READ_ONLY);
	if (attr & ATTR_READ_ONLY)
	{//是只读文件/目录
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_HIDDEN);
	if (attr & ATTR_HIDDEN)
	{//是只读文件/目录
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_SYSTEM);
	if (attr & ATTR_SYSTEM)
	{//是只读文件/目录
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_VOL_ID);
	if (attr & ATTR_VOLUME_ID)
	{//是只读文件/目录
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_DIR);
	if (attr & ATTR_DIRECTORY)
	{//是只读文件/目录
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	pWnd = this->GetDlgItem(IDC_CK_ARTRIVE);
	if (attr & ATTR_ARCHIVE)
	{//是只读文件/目录
		((CButton*)pWnd)->SetCheck(TRUE);
	}else
		((CButton*)pWnd)->SetCheck(FALSE);

	//开启线程加载簇链
	this->m_bIsThreadRun = FALSE;  //通知停止线程
	//Sleep(200);	
	//等待应存在的线程结束
	if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread , 800))
	{//手动关闭线程无效 则则强行关闭
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
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮
	dwStyle |= LVS_EX_GRIDLINES;	//网格线
	pList->SetExtendedStyle(dwStyle); //设置扩展风格

	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT2_LIST);
	strTemp.LoadString(IDS_ID);
	pList->InsertColumn(0 , strTemp , LVCFMT_LEFT , 30);
	strTemp.LoadString(IDS_CLUST_NUM);
	pList->InsertColumn(1 , strTemp , LVCFMT_LEFT , 90);
	dwStyle = pList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮
	dwStyle |= LVS_EX_GRIDLINES;	//网格线
	pList->SetExtendedStyle(dwStyle); //设置扩展风格


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFat32FileDlg::OnNMClickFat1List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//列表控件
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT1_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	liTemp = {0};
	LONG_INT	liEnd = {0};

	//获得选择的簇号
	//选择了的数据
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//没有选择任何数据
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	//计算簇号所在的扇区
	liTemp.QuadPart = (dwClust / (SECTOR_SIZE / 4)) + pFat32->GetReserveSector();

	//扇区列表 
	SectorList secList;
	secList.AddSector(pFat32->GetReserveSector() , pFat32->GetSectorPerFAT());
	secList.m_strName.LoadString(IDS_FAT1);
	this->m_pDoc->SetSectorList(&secList);

	//移动到指定的扇区
	this->m_pDoc->SetCurSector(liTemp);

	//计算簇号所在扇区内偏移 
	liTemp.QuadPart *= SECTOR_SIZE;
	liTemp.QuadPart += (dwClust % (SECTOR_SIZE / 4) * 4);

	//选择指定的数据
	liEnd.QuadPart = liTemp.QuadPart + 4;
	this->m_pDoc->SetSel(liTemp ,liEnd );
}

void CFat32FileDlg::OnNMClickFat2List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//列表控件
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT2_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	liTemp = {0};
	LONG_INT	liEnd = {0};

	//获得选择的簇号
	//选择了的数据
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//没有选择任何数据
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	//计算簇号所在的扇区
	liTemp.QuadPart = (dwClust / (SECTOR_SIZE / 4)) + pFat32->GetReserveSector() + pFat32->GetSectorPerFAT();


	//扇区列表 
	SectorList secList;
	secList.AddSector(pFat32->GetReserveSector() + pFat32->GetSectorPerFAT() , pFat32->GetSectorPerFAT());
	secList.m_strName.LoadString(IDS_FAT2);
	this->m_pDoc->SetSectorList(&secList);


	//移动到指定的扇区
	this->m_pDoc->SetCurSector(liTemp);

	//计算簇号所在扇区内偏移 
	liTemp.QuadPart *= SECTOR_SIZE;
	liTemp.QuadPart += (dwClust % (SECTOR_SIZE / 4) * 4);

	//选择指定的数据
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
	{///选中
		::CheckDlgButton(this->m_hWnd , id , BST_UNCHECKED);
	}else{
		///没选中
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

	//列表控件
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT1_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	curSec = {0};

	//获得选择的簇号
	//选择了的数据
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//没有选择任何数据
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	SectorList secList;
	dwSector = pFat32->ClustToSect(dwClust) ;
	secList.AddSector(dwSector , pFat32->GetSecPerClust());
	secList.m_strName += this->m_pFile->GetFileName();
	secList.m_strName += (_T("  ") + strTemp);
	this->m_pDoc->SetSectorList(&secList);

	//移动到指定的扇区
	curSec.QuadPart = dwSector;
	this->m_pDoc->SetCurSector(curSec);

}

void CFat32FileDlg::OnNMDblclkFat2List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//列表控件
	CListCtrl*	pList = (CListCtrl*)this->GetDlgItem(IDC_FAT2_LIST);
	DFat32*		pFat32 = this->m_pDoc->m_pFat32;
	int			nItem = 0;
	DWORD		dwSector = 0;
	DWORD		dwClust  = 0;
	CString		strTemp;
	LONG_INT	curSec = {0};

	//获得选择的簇号
	//选择了的数据
	nItem = pList->GetSelectionMark();
	if (-1 == nItem) return ;	//没有选择任何数据
	strTemp = pList->GetItemText(nItem , 1);
	if (0 == strTemp.GetLength()) return ;
	dwClust = DWORD(HexStrToLONG_INT(strTemp).QuadPart);

	SectorList secList;
	dwSector = pFat32->ClustToSect(dwClust) ;
	secList.AddSector(dwSector , pFat32->GetSecPerClust());
	secList.m_strName += this->m_pFile->GetFileName();
	secList.m_strName += (_T("  ") + strTemp);
	this->m_pDoc->SetSectorList(&secList);

	//移动到指定的扇区
	curSec.QuadPart = dwSector;
	this->m_pDoc->SetCurSector(curSec);
}
