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
	
	//��ʼ��ͼ���б�
	InitImageList();

	//��ʼ���ؼ�
	this->InitTreeCtrl();

	//���ؼ���Ҫչ����ʾ
	this->m_wndDevVolume.Expand( this->m_hPhsycDisk , TVE_EXPAND);
	this->m_wndDevVolume.Expand( this->m_hLogicDrivre , TVE_EXPAND);
	
	return TRUE;
}

void CDevVolumeDlg::InitTreeCtrl()
{
	//����ͼƬ�б�
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

	//��Ӹ��ڵ�
	//����洢�豸�ĸ��ڵ�
	strInfo.LoadString(IDS_PHYSC_DISK);
	this->m_hPhsycDisk = this->m_wndDevVolume.InsertItem(strInfo , 0 , 0 , TVI_ROOT);

	//��ȡÿһ��������̵ĵĵ�����
	for (i = 0 ; i < MAX_DISK_COUNT ; ++i)
	{
		strInfo.Format(_T("%s%d") , DISK_PRE_NAME , i);  //�豸����
		hDev = CreateFile( strInfo, 0, FILE_SHARE_READ |
			FILE_SHARE_WRITE , NULL, OPEN_EXISTING , 0 ,  NULL);								
		if(hDev == INVALID_HANDLE_VALUE) continue;	//��ǰ�豸û��   
		else{  

			//��ѯ�豸����
			res = ::DeviceIoControl(hDev,		// �豸���
				IOCTL_DISK_GET_DRIVE_GEOMETRY,	// ȡ���̲���
				NULL , 0 , &geometry,
				sizeof(DISK_GEOMETRY),			// ������ݻ�����
				&dwOutBytes , (LPOVERLAPPED)NULL);// ��ͬ��I/O
			::CloseHandle(hDev);

			//�����Ϣ�豸ʧ��
			if (FALSE == res) continue;

			if (FixedMedia == geometry.MediaType)
			{//���ش���
				strTemp.LoadString(IDS_LOCAL_DISK);
				strInfo.Format(_T("%d\t%s") , i , strTemp);
			}else if (RemovableMedia == geometry.MediaType)
			{//���Զ�����
				strTemp.LoadString(IDS_REMOVEABLE_DISK);
				strInfo.Format(_T("%d\t%s") , i , strTemp);
			}else{
				//δ֪�豸
				strTemp.LoadString(IDS_UNKOWE_DISK);
				strInfo.Format(_T("%d\t%s") , i , strTemp);
			}
			
			this->m_wndDevVolume.InsertItem(strInfo , 1 , 1 ,m_hPhsycDisk );
		}
	}

	//�߼�������
	strInfo.LoadString(IDS_LOGICAL_DRIVER);
	this->m_hLogicDrivre = this->m_wndDevVolume.InsertItem(strInfo , 0 , 0 , TVI_ROOT);

	//��껺��
	TCHAR volName[MAX_NTFS_VOLUME_NAME_LEN] = {0};
// 	PULARGE_INTEGER lpFreeBytesAvailable = {0};    // bytes available to caller
// 	PULARGE_INTEGER lpTotalNumberOfBytes = {0};    // bytes on disk
// 	PULARGE_INTEGER lpTotalNumberOfFreeBytes = {0}; // free bytes on disk
	int nImgIndex = 2;	//Ĭ���Ǳ��ش���

	//����ÿһ������
	for(cDev = 'A' ; cDev < 'Z' ; ++cDev)
	{
		strInfo.Format(_T("%s%c:") , _T("\\\\?\\") , cDev);
		hDev = CreateFile( strInfo , 0 , FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL , OPEN_EXISTING , 0 , NULL );
		if(hDev == INVALID_HANDLE_VALUE) continue;		//û����   
		else{  
			::CloseHandle(hDev);

			//�豸����
			strInfo.Format(_T("%c:\\") , cDev);
			devType = GetDriveType(strInfo);

// 			//��þ��
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
			case DRIVE_REMOVABLE:	//���ƶ����� ��U�̡����̵� 		
				strTemp.LoadString(IDS_VOL_REMOVEABLE);
				nImgIndex = 7;
				break;
			case DRIVE_FIXED:		//���ش��� 
				if (TRUE == GetVolumeInformation(strInfo , volName ,
					MAX_NTFS_VOLUME_NAME_LEN , NULL , NULL , NULL , NULL , 0 ))
				{
					strTemp = volName;
				}else{
					strTemp.LoadString(IDS_VOL_LOCAL);
				}
				nImgIndex = 2;
				break;
			case DRIVE_REMOTE:		//���� 
				strTemp.LoadString(IDS_VOL_REMOTE);
				nImgIndex = 5;
				break;
			case DRIVE_CDROM:		//CD-ROM 
				strTemp.LoadString(IDS_VOL_CD_DVD);
				nImgIndex = 3;
				break;
			case DRIVE_RAMDISK:		//�ڴ���
				strTemp.LoadString(IDS_VOL_RAM_DISK);
				nImgIndex = 2;
				break;
			default:				//�޷���������
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
		//���ѡ���˵��豸����
		CString strSel = this->m_wndDevVolume.GetItemText(hitem);
		strSel = strSel.Left(strSel.Find(_T('\t')));

		if (hParent == m_hPhsycDisk)
		{//����洢�豸
			this->m_strSelItem.Format(_T("%s%s") , DISK_PRE_NAME , (LPCSTR)(LPCTSTR)strSel );
			this->m_bSelDisk = SEL_DISK;
		}else if (hParent == m_hLogicDrivre)
		{//�߼�������
			this->m_strSelItem.Format(_T("%s%s") , "\\\\?\\" , (LPCSTR)(LPCTSTR)strSel );
			this->m_bSelDisk = SEL_VOLUME;
		}

		//�رնԻ���
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
		//���ѡ���˵��豸����
		CString strSel = this->m_wndDevVolume.GetItemText(hitem);
		strSel = strSel.Left(strSel.Find(_T('\t')));

		if (hParent == m_hPhsycDisk)
		{//����洢�豸
			this->m_strSelItem.Format(_T("%s%s") , DISK_PRE_NAME , (LPCSTR)(LPCTSTR)strSel );
			this->m_bSelDisk = SEL_DISK;
		}else if (hParent == m_hLogicDrivre)
		{//�߼�������
			this->m_strSelItem.Format(_T("%s%s") , "\\\\?\\" , (LPCSTR)(LPCTSTR)strSel );
			this->m_bSelDisk = SEL_VOLUME;
		}

		//�رնԻ���
		EndDialog(IDOK); 
	}else
	{//ѡ��Ĳ���һ����Ч���豸����
		CString strInfo;
		CString strTitle;
		strInfo.LoadString(IDS_NOT_VALIDE_NAME);
		strTitle.LoadString(IDS_PROMPT);
		::MessageBox(this->GetSafeHwnd() , strInfo , strTitle , MB_OK|MB_ICONWARNING);
	}
}

void CDevVolumeDlg::InitImageList()
{
	//����Ҫʹ�õ�ͼ���б�
	this->m_pImageList->Create(16 , 16 ,ILC_COLOR32|ILC_MASK , 0 , 4);
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	//�������ͼ��  0
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDR_DISKSCTYPE);		//����ͼ��    1
	m_pImageList->Add(hIcon);	
	hIcon = AfxGetApp()->LoadIcon(IDR_VOLTYPE);			//��  2
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_DVD_ROM);			//dvd_rom 3
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_UNKNOW_VOL);		//δ֪�豸 4
	m_pImageList->Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_REMOTE_VOL);		//������� 5 
	m_pImageList->Add(hIcon);							
	hIcon = AfxGetApp()->LoadIcon(IDI_FLOPY_VOL);		//���� 6 
	m_pImageList->Add(hIcon);	
	hIcon = AfxGetApp()->LoadIcon(IDI_REMOVABLE_VOL);	//���ƶ��豸 7 
	m_pImageList->Add(hIcon);	
}

CString CDevVolumeDlg::GetSelDevName()
{
	return m_strSelItem;
}
