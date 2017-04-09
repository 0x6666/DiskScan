// DevVolume.cpp : implementation file
//

#include "stdafx.h"
#include "diskscan.h"
#include "DevVolume.h"
#include "MainFrm.h"
#include "../DiskTool/disktool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDevVolume dialog


CDevVolumeDlg::CDevVolumeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDevVolumeDlg::IDD, pParent)
	, m_bSelDisk(0)
{
	//{{AFX_DATA_INIT(CDevVolume)
		// NOTE: the ClassWizard will add member initialization here
	m_pImageList.reset(new CImageList());
	//}}AFX_DATA_INIT
}
// CDevVolumeDlg::~CDevVolumeDlg()
// {
// 	delete m_pImageList;
// }

void CDevVolumeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevVolume)
	DDX_Control(pDX, IDC_DEV_AND_VOLUME, m_wndDevVolume);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDevVolumeDlg, CDialog)
	//{{AFX_MSG_MAP(CDevVolume)
	ON_NOTIFY(NM_DBLCLK, IDC_DEV_AND_VOLUME, OnDblclkDevAndVolume)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevVolume message handlers

BOOL CDevVolumeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//初始化图标列表
	InitImageList();

	//初始化控件
	this->InitTreeCtrl();

	//数控件需要展开显示
	this->m_wndDevVolume.Expand( this->m_hPhsycDisk , TVE_EXPAND);
	this->m_wndDevVolume.Expand( this->m_hLogicDrivre , TVE_EXPAND);
	
	return TRUE;
}

void CDevVolumeDlg::InitTreeCtrl()
{
	//设置图片列表
	this->m_wndDevVolume.SetImageList(m_pImageList.get(),TVSIL_NORMAL);

	CString	strInfo;
	int		i = 0;
	char	cDev;
	HANDLE	hDev = INVALID_HANDLE_VALUE;
	BOOL	res = FALSE;
	DISK_GEOMETRY geometry  = {0};
	DWORD	dwOutBytes = 0;
	CString	strTemp;
	UINT	devType;

	//添加根节点
	//物理存储设备的根节点
	strInfo.LoadString(IDS_PHYSC_DISK);
	this->m_hPhsycDisk = this->m_wndDevVolume.InsertItem(strInfo , 0 , 0 , TVI_ROOT);

	//获取每一个物理磁盘的的的名字
	for (i = 0 ; i < MAX_DISK_COUNT ; ++i)
	{
		strInfo.Format(_T("%s%d") , DISK_PRE_NAME , i);  //设备名字
		hDev = CreateFile( strInfo, 0, FILE_SHARE_READ |
			FILE_SHARE_WRITE , NULL, OPEN_EXISTING , 0 ,  NULL);								
		if(hDev == INVALID_HANDLE_VALUE) continue;	//当前设备没有   
		else{  

			//查询设备类型
			res = ::DeviceIoControl(hDev,		// 设备句柄
				IOCTL_DISK_GET_DRIVE_GEOMETRY,	// 取磁盘参数
				NULL , 0 , &geometry,
				sizeof(DISK_GEOMETRY),			// 输出数据缓冲区
				&dwOutBytes , (LPOVERLAPPED)NULL);// 用同步I/O
			::CloseHandle(hDev);

			//获得信息设备失败
			if (FALSE == res) continue;

			if (FixedMedia == geometry.MediaType)
			{//本地磁盘
				strTemp.LoadString(IDS_LOCAL_DISK);
				strInfo.Format(_T("%d\t%s") , i , strTemp);
			}else if (RemovableMedia == geometry.MediaType)
			{//可以动磁盘
				strTemp.LoadString(IDS_REMOVEABLE_DISK);
				strInfo.Format(_T("%d\t%s") , i , strTemp);
			}else{
				//未知设备
				strTemp.LoadString(IDS_UNKOWE_DISK);
				strInfo.Format(_T("%d\t%s") , i , strTemp);
			}
			
			this->m_wndDevVolume.InsertItem(strInfo , 1 , 1 ,m_hPhsycDisk );
		}
	}

	//逻辑驱动器
	strInfo.LoadString(IDS_LOGICAL_DRIVER);
	this->m_hLogicDrivre = this->m_wndDevVolume.InsertItem(strInfo , 0 , 0 , TVI_ROOT);

	//卷标缓存
	TCHAR volName[MAX_NTFS_VOLUME_NAME_LEN] = {0};
// 	PULARGE_INTEGER lpFreeBytesAvailable = {0};    // bytes available to caller
// 	PULARGE_INTEGER lpTotalNumberOfBytes = {0};    // bytes on disk
// 	PULARGE_INTEGER lpTotalNumberOfFreeBytes = {0}; // free bytes on disk
	int nImgIndex = 2;	//默认是本地磁盘

	//遍历每一个驱动
	for(cDev = 'A' ; cDev < 'Z' ; ++cDev)
	{
		strInfo.Format(_T("%s%c:") , _T("\\\\?\\") , cDev);
		hDev = CreateFile( strInfo , 0 , FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL , OPEN_EXISTING , 0 , NULL );
		if(hDev == INVALID_HANDLE_VALUE) continue;		//没有了   
		else{  
			::CloseHandle(hDev);

			//设备类型
			strInfo.Format(_T("%c:\\") , cDev);
			devType = GetDriveType(strInfo);

// 			//获得卷标
// 			if (GetDiskFreeSpaceExA(strInfo , lpFreeBytesAvailable ,
//  				lpTotalNumberOfBytes ,lpTotalNumberOfFreeBytes)// &&
// // 				(TRUE == GetVolumeInformation(strInfo , volName ,
// /*// 				MAX_NTFS_VOLUME_NAME_LEN , NULL , NULL , NULL , NULL , 0 ))*/)
// 			{
// 				strInfo += _T(" ");
// 				strInfo += volName;
// 			}

			switch(devType)
			{
			case DRIVE_REMOVABLE:	//可移动磁盘 如U盘、软盘等 		
				strTemp.LoadString(IDS_VOL_REMOVEABLE);
				nImgIndex = 7;
				break;
			case DRIVE_FIXED:		//本地磁盘 
				if (TRUE == GetVolumeInformation(strInfo , volName ,
					MAX_NTFS_VOLUME_NAME_LEN , NULL , NULL , NULL , NULL , 0 ))
				{
					strTemp = volName;
				}else{
					strTemp.LoadString(IDS_VOL_LOCAL);
				}
				nImgIndex = 2;
				break;
			case DRIVE_REMOTE:		//网盘 
				strTemp.LoadString(IDS_VOL_REMOTE);
				nImgIndex = 5;
				break;
			case DRIVE_CDROM:		//CD-ROM 
				strTemp.LoadString(IDS_VOL_CD_DVD);
				nImgIndex = 3;
				break;
			case DRIVE_RAMDISK:		//内存盘
				strTemp.LoadString(IDS_VOL_RAM_DISK);
				nImgIndex = 2;
				break;
			default:				//无法解析磁盘
				strTemp.LoadString(IDS_VOL_UNKNOW);
				nImgIndex = 4;
				break;
			}
			strInfo.Format(_T("%c:\t"), cDev);
			strInfo += strTemp;
			this->m_wndDevVolume.InsertItem(strInfo , nImgIndex , nImgIndex ,m_hLogicDrivre );
		}
	}  
}

void CDevVolumeDlg::OnDblclkDevAndVolume(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hitem = this->m_wndDevVolume.GetSelectedItem();
	HTREEITEM hParent;
	if (hParent = m_wndDevVolume.GetParentItem(hitem))
	{
		//获得选在了的设备名字
		CString strSel = this->m_wndDevVolume.GetItemText(hitem);
		strSel = strSel.Left(strSel.Find(_T('\t')));

		if (hParent == m_hPhsycDisk)
		{
			//物理存储设备
			this->m_strSelItem.Format(_T("%s%s") , DISK_PRE_NAME , (LPCSTR)(LPCTSTR)strSel );
			this->m_bSelDisk = SEL_DISK;
		}
		else if (hParent == m_hLogicDrivre)
		{
			//逻辑驱动器
			this->m_strSelItem.Format(_T("%s%s"), _T("\\\\?\\"), strSel );
			this->m_bSelDisk = SEL_VOLUME;
		}

		//关闭对话框
 		EndDialog(IDOK);
	}
	
	*pResult = 0;
}

void CDevVolumeDlg::OnOK() 
{
	HTREEITEM hitem = this->m_wndDevVolume.GetSelectedItem();
	HTREEITEM hParent;

	if (hParent = m_wndDevVolume.GetParentItem(hitem))
	{
		//获得选在了的设备名字
		CString strSel = this->m_wndDevVolume.GetItemText(hitem);
		strSel = strSel.Left(strSel.Find(_T('\t')));

		if (hParent == m_hPhsycDisk)
		{//物理存储设备
			this->m_strSelItem.Format(_T("%s%s") , DISK_PRE_NAME , (LPCSTR)(LPCTSTR)strSel );
			this->m_bSelDisk = SEL_DISK;
		}else if (hParent == m_hLogicDrivre)
		{//逻辑驱动器
			this->m_strSelItem.Format(_T("%s%s") , "\\\\?\\" , (LPCSTR)(LPCTSTR)strSel );
			this->m_bSelDisk = SEL_VOLUME;
		}

		//关闭对话框
		EndDialog(IDOK); 
	}else
	{//选择的不是一个有效的设备名字
		CString strInfo;
		CString strTitle;
		strInfo.LoadString(IDS_NOT_VALIDE_NAME);
		strTitle.LoadString(IDS_PROMPT);
		::MessageBox(this->GetSafeHwnd() , strInfo , strTitle , MB_OK|MB_ICONWARNING);
	}
}

void CDevVolumeDlg::InitImageList()
{
	//程序要使用的图标列表
	this->m_pImageList->Create(16 , 16 ,ILC_COLOR32|ILC_MASK , 0 , 4);
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	//程序的主图标  0
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDR_DISKSCTYPE);		//磁盘图标    1
	m_pImageList->Add(hIcon);	
	hIcon = AfxGetApp()->LoadIcon(IDR_VOLTYPE);			//卷  2
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_DVD_ROM);			//dvd_rom 3
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_UNKNOW_VOL);		//未知设备 4
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_REMOTE_VOL);		//网络磁盘 5 
	m_pImageList->Add(hIcon);							
	hIcon = AfxGetApp()->LoadIcon(IDI_FLOPY_VOL);		//软盘 6 
	m_pImageList->Add(hIcon);	
	hIcon = AfxGetApp()->LoadIcon(IDI_REMOVABLE_VOL);	//可移动设备 7 
	m_pImageList->Add(hIcon);	
}

CString CDevVolumeDlg::GetSelDevName()
{
	return m_strSelItem;
}
