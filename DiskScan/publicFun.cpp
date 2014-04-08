#include "StdAfx.h"
#include "DiskScan.h"
#include "publicFun.h"
#include "DiskScanView.h"
#include "Fat32Doc.h"
#include "..\DiskTool\disktool.h"
#include "..\StrTool\StrTool.h"
#include "NtfsDoc.h"
#include "CopyProcessDlg.h"

#pragma warning(disable:4996) 

CString GetPartName( USHORT format , const char* cDevname , LONG_INT off)
{
	CString res;
	DFat32	fat32;
	DNtfs	ntfs;
	WCHAR	wBuf[MAX_NTFS_VOLUME_NAME_LEN + 1] = {0};
	char*	cBuf = (char*)wBuf; // len = 2*(MAX_NTFS_VOLUME_NAME_LEN + 1)

	switch(format)
	{
	case 0x01://fat32
	case 0x0B:
	case 0x0C:
	case 0x1B://	Hidden FAT32
	case 0x1C://	Hidden FAT32 partition (using LAB-mode INT 13 extension)
		//fName.LoadString(IDS_FAT32_PART);
		
		if(DR_OK == fat32.OpenDev(cDevname , off))
		{//打开设备成功
			fat32.GetVolumeName(cBuf , 2*(MAX_NTFS_VOLUME_NAME_LEN + 1));
			fat32.CloseDev();	//关闭已经打开的设备
			res = cBuf;
		}else
			res = GetPartFormatName(format);
		break;

	case 0x07://	HPFS/NTFS
	case 0x17://	Hidden HPFS/HTFS
		//fName.LoadString(IDS_NTFS_PART);
		if(DR_OK == ntfs.OpenDev(cDevname , &off))
		{//打开设备成功
			ntfs.GetVolumeName(wBuf , (MAX_NTFS_VOLUME_NAME_LEN + 1));
			ntfs.CloseDev();	//关闭已经打开的设备
			res = wBuf;
		}else
			res = GetPartFormatName(format);
		break; 

		//一下是我自己定义的几种类型
	case 0x100://			//MBR
		res.LoadString(IDS_MBR_NAME);
		break;
	case 0x101://			//EBR
		res.LoadString(IDS_EBR_NAME);
		break;
	case 0x102://			//没分区的空闲区域
		res.LoadString(IDS_EMPTY_PART_NAME);
		break;
	case 0x103:			//不能分区的区域
		res.LoadString(IDS_REMAIN_PART_NAME);
		break;

	default://其他情况就没办法了 
		res = GetPartFormatName(format);
	}

	return res;
}

CString GetPartFormatName(USHORT format)
{
	CString fName;
	switch(format)
	{
	case 0x0://空  Microsoft 不可以使用
		fName.LoadString(IDS_NO_USE_PART);
		break;

	case 0x11://	Hidden FAT12
		fName.LoadString(IDS_HID_FAT12_PART);
		break;

	case 0x04://	FAT16 <32M
	case 0x06://	FAT16
	case 0x0E://	Win95 FAT16
	case 0x14://	Hidden FAT16 <32G
	case 0x16://	Hidden FAT16
		fName.LoadString(IDS_FAT16_PART);
		break;

	case 0x01://fat32
	case 0x0B:
	case 0x0C:
	case 0x1B://	Hidden FAT32
	case 0x1C://	Hidden FAT32 partition (using LAB-mode INT 13 extension)
		fName.LoadString(IDS_FAT32_PART);
		break;
	
	case 0x02://XENIX root
		fName.LoadString(IDS_XENIX_ROOT_PART);
		break;
	case 0x03://	XENIX usr
		fName.LoadString(IDS_XENIX_USR_PART);
		break;

	case 0x05://	Externded  扩展分区
	case 0x0F://	Win95 externded（>8GB）
		fName.LoadString(IDS_EXT_PART);
		break;

	case 0x07://	HPFS/NTFS
	case 0x17://	Hidden HPFS/HTFS
		fName.LoadString(IDS_NTFS_PART);
		break;
	
	case 0x08://	AIX
		fName.LoadString(IDS_AIX_PART);
		break;
	case 0x09://	AIX bootable
		fName.LoadString(IDS_AIX_BOOTABLE_PART);
		break;
	case 0x0A://	OS/2 Boot Manage
		fName.LoadString(IDS_SO2_BOOT_MG_PART);
		break;
	case 0x10://	OPUS
		fName.LoadString(IDS_OPUS_PART);
		break;
	case 0x12://	Compaq diagnost
		fName.LoadString(IDS_COMPAQ_DIAG_PART);
		break;
	case 0x18://	AST Windows swap
		fName.LoadString(IDS_AST_WINDOWS_SWAP_PART);
		break;
	case 0x1E://	Hidden LAB VFAT partition
		fName.LoadString(IDS_HID_VFAT_PART);
		break;
	case 0x24://	NEC DOS
		fName.LoadString(IDS_NEC_DOS_PART);
		break;
	case 0x3C://	Partition Magic
		fName.LoadString(IDS_PART_MAGIC_PART);
		break;
	case 0x40://	Venix 80286
		fName.LoadString(IDS_VENIX_80286_PART);
		break;
	case 0x41://	PPC PreP Boot	
		fName.LoadString(IDS_PPC_PREP_PART);
		break;
	case 0x42://	SFS
		fName.LoadString(IDS_SFS_PART);
		break;
	case 0x4D://	QNX4.x
	case 0x4E://	QNX4.x 2nd part
	case 0x4F://	QNX4.x 3rd part
		fName.LoadString(IDS_QNX4_PART);
		break;
	case 0x50://	Ontrack DM
	case 0x51://	Ontrack DM6 Aux
	case 0x53://	Ontrack DM6 Aux
	case 0x54://	Ontrack DM6
		fName.LoadString(IDS_ONTRACK_DM_PART);
		break;
	case 0x52://	CP/M
		fName.LoadString(IDS_CP_M_PART);
		break;
	case 0x55://	EZ-Drive
		fName.LoadString(IDS_EZ_DRIVE_PART);
		break;
	case 0x56://	Golden Bow
		fName.LoadString(IDS_GOLDEN_BOW_PART);
		break;

	case 0x5C://	Priam Edick
		fName.LoadString(IDS_PRIAM_EDICK_PART);
		break;

	case 0x61://	Speed Stor
	case 0xE4://	SpeedStor
	case 0xF1://	SpeedStor
	case 0xF4://	SpeedStor
		fName.LoadString(IDS_SPEED_STOR_PART);
		break;
	case 0x63://	GNU HURD or Sys
		fName.LoadString(IDS_GNU_HURD_PART);
		break;
	case 0x64://	Novell Notware	
	case 0x65://	Novell Notware
		fName.LoadString(IDS_NOVELL_NOTWARE_PART);
		break;
	case 0x70://	Disk Secure Mult
		fName.LoadString(IDS_DISK_SECURE_MULT_PART);
		break;

	case 0x75://	PC/IX		
		fName.LoadString(IDS_PC_IX_PART);
		break;

	case 0x80: //Old Minix
		fName.LoadString(IDS_OLD_MINIX_PART);
		break;
	case 0x81://	Minix/Old Linux
		fName.LoadString(IDS_MINIX_OLD_LINUX_PART);
		break;

	case 0x82:///	Linux Swap
		fName.LoadString(IDS_LINUX_SWAP_PART);
		break;
	case 0x83://Linux
		fName.LoadString(IDS_LINUX_PART);
		break;
	case 0x84://	OS/2 hidden C:
		fName.LoadString(IDS_OS2_HID_C_PART);
		break;
	case 0x85://	Linux externded
		fName.LoadString(IDS_LINUX_EXT_PART);
		break;

	case 0x86://	NTFS volume set
	case 0x87://	NTFS volume set
		fName.LoadString(IDS_NTFS_VOLUM_SET_PART);
		break;
	
	case 0x8E://Linux LVM
		fName.LoadString(IDS_LINUX_LVM);
		break;

	case 0x93://	Amoeba
		fName.LoadString(IDS_AMOEBA_PART);
		break;
	case 0x94://	Amoeba BBT
		fName.LoadString(IDS_AMOEBA_BBT_PART);
		break;
	case 0xA0://	IBM thinkpad hidden partition
		fName.LoadString(IDS_THINKPAD_HID_PART);
		break;
	case 0xA5://	BSD/386
		fName.LoadString(IDS_BSD_386_PART);
		break;

	case 0xA6://	Open BSD
		fName.LoadString(IDS_OPEN_BSD_PART);
		break;
	case 0xA7://	NextSTEP
		fName.LoadString(IDS_NEXT_STEP_PART);
		break;
	case 0xB7://	BSDI fs
		fName.LoadString(IDS_BSDI_FS_PART);
		break;
	case 0xB8://	BSDI swap
		fName.LoadString(IDS_BSDI_SWAP_PART);
		break;
	case 0xBE://	Solaris Boot
		fName.LoadString(IDS_SOLAIS_BOOT_PART);
		break;
	
	case 0xC0://	DR-DOS/Novell DOS secured 
	case 0xC1://	DR-DOS/Novell DOS secured
	case 0xC4://	DR-DOS/Novell DOS secured
	case 0xC6://	DR-DOS/Novell DOS secured
		fName.LoadString(IDS_DR_OS_NOVELL_PART);
		break;
	
	case 0xC7://	Syrinx
		fName.LoadString(IDS_SYRINX_PART);
		break;
	case 0xDB://	CP/M/CTOS
		fName.LoadString(IDS_CP_M_CTOS_PART);
		break;
	case 0xE1://	DOS access
		fName.LoadString(IDS_DOS_ACCESS_PART);
		break;
	case 0xE3://	DOS R/O
		fName.LoadString(IDS_DOS_R0_PART);
		break;

	case 0xEB://	BeOS fs
		fName.LoadString(IDS_BEOS_FS_PART);		
		break;

	case 0xF2://	DOS 3.3+  secondary
		fName.LoadString(IDS_DOS_3_3_PART);
		break;
	case 0xFE://	LAN step
		fName.LoadString(IDS_LAN_STEP_PART);
		break;
	case 0xFF://	BBT
		fName.LoadString(IDS_BBT_PART);
		break;

		//一下是我自己定义的几种类型
	case 0x100://			//MBR
		fName.LoadString(IDS_MBR_PART);
		break;
	case 0x101://			//EBR
		fName.LoadString(IDS_EBR_PART);
		break;
	case 0x102://			//没分区的空闲区域
		fName.LoadString(IDS_EMPTY_PART);
		break;
	case 0x103:			//不能分区的区域
		fName.LoadString(IDS_REMAIN_PART);
		break;
	
	default:
		fName.LoadString(IDS_UNKNOW_PART);
	}
	return fName;
}

CString GetSizeToString(LONG_INT len)
{
	//参数不合格
	CString strSize(_T("0"));

//	len.QuadPart *= SECTOR_SIZE;
	
	if(len.QuadPart < (LONGLONG)1024)
	{////小于1K
		strSize.Format(_T("%dB") , len.QuadPart);
	}else if (len.QuadPart < (LONGLONG)1024 * 1024)
	{////小于1M
		strSize.Format(_T("%.2fKB") , len.QuadPart / 1024.0);
	} 
	else if (len.QuadPart < (__int64)1024 * 1024 * 1024)
	{//小于1G
		strSize.Format(_T("%.2fMB") , len.QuadPart / 1024.0 / 1024.0);
	} 
	else if (len.QuadPart < (__int64)1024 * 1024 * 1024 * 1024)
	{//小于1T
		strSize.Format(_T("%.2fGB") , len.QuadPart / 1024.0 / 1024.0/1024.0);
	} 
	else 
	{
		strSize.Format(_T("%.2fTB") , len.QuadPart / 1024.0 / 1024.0/1024.0/1024.0);
	}
	
//  	strSize.TrimRight(_T('0'));
//  	strSize.TrimRight(_T('.'));
//  	if (strSize.GetLength() == 0) strSize = _T("0");
	
	return strSize;
}


CString GetSizeToString(DWORD sectorCount)
{
	LONG_INT len;
	len.QuadPart = sectorCount;
	return GetSizeToString(len);
}
LONG_INT HexStrToLONG_INT(CString str)
{
	LONG_INT li = {0};
	int len = str.GetLength();
	char a = 0;
	int i = 0;
	
	//超出了范围
	if (len == 0 || len > 16 )
		return li;
	
	//获取每一个字符
	for (i = 0 ; i < len; ++i)
	{
		li.QuadPart <<= 4;
		a = (char)str.GetAt(i);
		if ( a >= '0' && a <= '9' )
		{//数字
			li.QuadPart |= (0x0F & ( a - '0'));
		}else if(a >= 'a' && a <= 'f')
		{//小写字符
			li.QuadPart |= (0x0F & ( a - 'a' + 10));
		}else if(a >= 'A' && a <= 'F')
		{//大写字符
			li.QuadPart |= (0x0F & ( a - 'A' + 10));
		}else{//出现了不和发的字符
			li.QuadPart >>= 4;//还原
			return li;
		}
	}
	return li;
}

//CDiskScanView
DWORD WINAPI GetUnPartableSectorCntThread(PVOID wParam)
{
	CHexDataView* pView = (CHexDataView*)wParam;
	DWORD size = 0;
	CDataDoc * pDoc = pView->GetDocument();
	
	
	//TODO  错误处理,  收到了一个不应该的的文档
	if (!pDoc->IsKindOf(RUNTIME_CLASS(CDiskDoc)))
		return 0;
	

	//开始获取不可分配的扇区数
	if(((CDiskDoc*)pDoc)->GetUnPartSecCount(&size))
	{//获取成功
		if (::IsWindow(pView->GetSafeHwnd()))
		{
			pView->SendMessage(DMSG_GOT_UNPARTABLE_SEC , (WPARAM)size , 0 );
		}
		
	}else{
		//TODO  计算磁盘的不可分区的大小失败
	}

	return 0;	
}



CString GetParamByName(CString pathName , TCHAR* pName)
{
	int index = 0 , index2 = 0;
	CString upCase;
	CString res = _T("");
	//获得路径的大写形式
	upCase = pathName;
	upCase.MakeUpper();
	
	
	index  = upCase.Find(pName);
	if (index == -1)
		res = _T("");	//没有索引参数
	else{//找到了参数
		
		//获取参数
		index2 = upCase.Find(_T(" ") , index);
		
		if (index2 == -1)//没有参数实体
			res = _T("");
		else{//有参数实体
			pathName = pathName.Mid(index2);  //获得参数数据部分
			pathName.TrimLeft();
			if (pathName.GetAt(0) == _T('-'))  //没有当前参数数据
				res = _T("");
			else{
				index = pathName.Find(_T(" "));
				if (index == -1)  //接下来没有参数了
					res = pathName;
				else{
					res = pathName.Mid(0 , index);
					res.TrimRight();
				}
			}	
		}
	}
	return res;
}


CString GetPathParam( CString pathName , int paramType )
{
	CString res;
	CString upCase;		//路径大写形式
	CString strTmp;		
	int index = 0 , index2 = 0;
	
	pathName.TrimRight();
	pathName.TrimLeft();
	
	if (!pathName.GetLength()){
		//TODO 出现了无效的路径
		return _T("");
	}
	
	//获得路径的大写形式
	upCase = pathName;
	upCase.MakeUpper();
	
	switch(paramType){
	case PT_DEVNAME:	//获得设备的名字
		index  = upCase.Find(_T(" "));
		if (index == -1)
			res = pathName;	//没有参数
		else
			res  = pathName.Mid( 0 , index);
		break;
	case PT_INDEX:		//获得要打开的区域在设备上的区域序号
		//index  = upCase.Find();
		res = GetParamByName(pathName , PN_INDEX);
		break;
	case PT_OFFSET:		//获得区域的偏移
		index  = upCase.Find(PN_OFFSET);
		res = GetParamByName(pathName , PN_OFFSET);
		break;
	case PT_LETTER:		//盘符
		index  = upCase.Find(PN_LETTER);
		res = GetParamByName(pathName , PN_LETTER);
		break;
	default://出现了没有的参数类型
		return _T("");
	}
	return res;
}


DWORD WINAPI EnumFAT32File(PVOID wParam)
{
	CFat32Doc*	pDoc	= (CFat32Doc*)wParam;
	CListCtrl*	pList	= pDoc->m_pContentList;
	DFat32*		pFat32	= pDoc->m_pFat32;
	CString     curPath = pDoc->m_strCurPath;
	DFat32File  root , file;
	WCHAR		path[MAX_PATH + 1] = {0};
	FINDER		Finder;
	int			i = 0;
	CString		strTemp = _T("");
	DWORD		dwTemp = 0;
	time_t      tTemp = 0;
	struct tm   tmTemp = {0};
/*	LONG_INT    liTemp = {0};*/

	DRES		res = DR_OK;

	//先清理一下现有的数据
	pList->DeleteAllItems();

	pDoc->m_bIsRun = TRUE;

	//要打开的设备名字
#ifdef  _UNICODE
	wcscpy(path , (LPCTSTR)curPath);
#else
	MultyByteToUnic((LPCTSTR)curPath , path , MAX_PATH + 1);
#endif

//  ID  文件名 起始扇区号  总扇区数  大小 创建时间 修改时间 访问时间
	
	res = pFat32->OpenFileW( path , &root);

	if (res != DR_OK)
	{//TODO  打开指定的目录失败
		return 0;
	}
	res = pFat32->FindFile(&root  , &Finder );
	if (res != DR_OK)
	{//TODO  初始化查找失败
		root.Close();
		return 0;
	}

	while (res == DR_OK)
	{
		if ( pDoc->m_bIsRun == FALSE)
		{//需要退出了
			//释放需要释放的资源
			pFat32->FindClose(Finder);
		
			return 0;
		}

		res = pFat32->FindNextFileW(Finder , &file  );
		if (res == DR_FAT_EOF)//TODO超找完毕了
			break;
		
		if (res != DR_OK)//TODO 超找失败了
			break;
				
		//  ID  文件名 起始扇区号  总扇区数  实际大小 分配大小 创建时间 修改时间 访问时间

		strTemp.Format(_T("%d") , i);//ID
		if (!file.IsDir()){  //文件
			pList->InsertItem( i  , strTemp , 0 );
		}else{				//目录
			pList->InsertItem( i  , strTemp , 1);
		}
		//文件名
		strTemp = file.GetFileName();
		pList->SetItemText(i , 1  , strTemp);//名字

		//起始扇区号
		dwTemp = file.GetStartSec();
		strTemp.Format(_T("%X") , dwTemp);
		pList->SetItemText(i , 2  , strTemp);//其实扇区号

		if (!file.IsDir())
		{//目录是没有文件大小的
			//占用的扇区数  如果是目录的话值为-1  便于以后检查
			strTemp.Format(_T("%X") , file.GetSecCount());
			pList->SetItemText(i , 3  , strTemp);//占用的扇区数
		
			//文件的大小
			dwTemp = file.GetFileSize();
			strTemp = GetSizeToString(dwTemp);
			pList->SetItemText(i , 4  , strTemp);//文件的大小

			//文件的分配大小
			dwTemp = file.GetSecCount() * SECTOR_SIZE;
			strTemp = GetSizeToString(dwTemp);
			pList->SetItemText(i , 5  , strTemp);//占用的扇区数
		
		}

		//文件的创建时间
		tTemp = (time_t)file.GetCreateTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 6  , strTemp);//占用的扇区数


		//文件的修改时间
		tTemp = (time_t)file.GetWriteTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 7  , strTemp);//占用的扇区数

		//文件的访问时间
		tTemp = (time_t)file.GetAccessTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday);
		pList->SetItemText(i , 8  , strTemp);//占用的扇区数


		file.Close();
		++i;
	}

	//释放需要释放的资源
	pFat32->FindClose(Finder);

	return 0;
}


DWORD WINAPI EnumDelFAT32File(PVOID wParam)
{
	CFat32Doc*	pDoc	= (CFat32Doc*)wParam;
	CListCtrl*	pList	= pDoc->m_pContentList;
	DFat32*		pFat32	= pDoc->m_pFat32;
	CString     curPath = pDoc->m_strCurPath;
	DFat32File  root , file;
	WCHAR		path[MAX_PATH + 1] = {0};
	FINDER		Finder;
	int			i = 0;
	CString		strTemp = _T("");
	DWORD		dwTemp = 0;
	time_t      tTemp = 0;
	struct tm   tmTemp = {0};
	/*	LONG_INT    liTemp = {0};*/

	DRES		res = DR_OK;

	//先清理一下现有的数据
	//pList->DeleteAllItems();

	pDoc->m_bIsRun = TRUE;

	//要打开的设备名字
#ifdef  _UNICODE
	wcscpy(path , (LPCTSTR)curPath);
#else
	MultyByteToUnic((LPCTSTR)curPath , path , MAX_PATH + 1);
#endif

	//  ID  文件名 起始扇区号  总扇区数  大小 创建时间 修改时间 访问时间

	res = pFat32->OpenFileW( path , &root);

	if (res != DR_OK)
	{//TODO  打开指定的目录失败
		return 0;
	}
	res = pFat32->FindFile(&root  , &Finder , TRUE );
	if (res != DR_OK)
	{//TODO  初始化查找失败
		root.Close();
		return 0;
	}
	i = pList->GetItemCount();

	while (res == DR_OK)
	{
		if ( pDoc->m_bIsRun == FALSE)
		{//需要退出了
			//释放需要释放的资源
			pFat32->FindClose(Finder);

			return 0;
		}

		res = pFat32->FindNextFileW(Finder , &file  );
		if (res == DR_FAT_EOF)//TODO超找完毕了
			break;

		if (res != DR_OK)//TODO 超找失败了
			break;

		//  ID  文件名 起始扇区号  总扇区数  实际大小 分配大小 创建时间 修改时间 访问时间

		strTemp.Format(_T("%d") , i);//ID
		pList->InsertItem( i  , strTemp ,1 );

		//文件名
		strTemp = file.GetFileName();
		pList->SetItemText(i , 1  , strTemp);//名字

		//起始扇区号
		dwTemp = file.GetStartSec();
		strTemp.Format(_T("%X") , dwTemp);
		pList->SetItemText(i , 2  , strTemp);//其实扇区号

		if (!file.IsDir())
		{//目录是没有文件大小的
			//占用的扇区数  如果是目录的话值为-1  便于以后检查
			strTemp.Format(_T("%X") , file.GetSecCount());
			pList->SetItemText(i , 3  , strTemp);//占用的扇区数

			//文件的大小
			dwTemp = file.GetFileSize();
			strTemp = GetSizeToString(dwTemp);
			pList->SetItemText(i , 4  , strTemp);//文件的大小

			//文件的分配大小
			dwTemp = file.GetSecCount() * SECTOR_SIZE;
			strTemp = GetSizeToString(dwTemp);
			pList->SetItemText(i , 5  , strTemp);//占用的扇区数

		}

		//文件的创建时间
		tTemp = (time_t)file.GetCreateTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 6  , strTemp);//占用的扇区数


		//文件的修改时间
		tTemp = (time_t)file.GetWriteTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 7  , strTemp);//占用的扇区数

		//文件的访问时间
		tTemp = (time_t)file.GetAccessTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday);
		pList->SetItemText(i , 8  , strTemp);//占用的扇区数


		file.Close();
		++i;
	}

	//释放需要释放的资源
	pFat32->FindClose(Finder);

	return 0;
}

DWORD WINAPI EnumNtfsFile(PVOID wParam)
{
	CNtfsDoc*	pDoc	= (CNtfsDoc*)wParam;
	CListCtrl*	pList	= pDoc->m_pContentList;
	DNtfs*		pNtfs	= pDoc->m_pNtfs;
	CString     curPath = pDoc->m_strCurPath;
	DNtfsFile    file;
	WCHAR		path[MAX_PATH + 1] = {0};
	FINDER		Finder;
	int			i = 0;
	CString		strTemp = _T("");
	DWORD		dwTemp = 0;
	time_t      tTemp = 0;
	struct tm   tmTemp = {0};
	LONG_INT	liMft = {0};
	LONG_INT	liTemp = {0};

	DRES		res = DR_OK;

	//先清理一下现有的数据
	pList->DeleteAllItems();
	pDoc->m_bIsRun = TRUE;

	if (1 < curPath.GetLength()) //非根目录
	{
		res = pNtfs->OpenFileA(curPath , &file );
		if (DR_OK != res) return 0;//出错了
		
		//在Ntfs的目录中是没有dot(.)和dotdot(..)目录的,在这里我为了方便自己添加
		//dot
		strTemp.Format(_T("%d") , i);	//ID
		pList->InsertItem( i  , strTemp ,1 );
		pList->SetItemText(i , 1  , _T("."));//名字
		
		//MFT记录
		liMft = file.GetMftIndex();
		liMft.HighPart == 0 ? strTemp.Format(_T("%X") , liMft.LowPart) :strTemp.Format(_T("%X%X") ,liMft.HighPart , liMft.LowPart) ;
		pList->SetItemText(i++ , 2  , strTemp);//其实扇区号
		
		//dotdot
		strTemp.Format(_T("%d") , i);	//ID
		pList->InsertItem( i  , strTemp ,1 );
		pList->SetItemText(i , 1  , _T(".."));//名字
		
		//MFT记录
		liMft = file.GetParentMftIndex();
		liMft.HighPart == 0 ? strTemp.Format(_T("%X") , liMft.LowPart) :strTemp.Format(_T("%X%X") ,liMft.HighPart , liMft.LowPart) ;
		pList->SetItemText(i++ , 2  , strTemp);//其实扇区号
		file.Close();
	}


	//开始一个查找过程
	res = pNtfs->FindFile(curPath  , &Finder );
	if (res != DR_OK)
	{//TODO  初始化查找失败
		return 0;
	}
	
	
	while (res == DR_OK)
	{
		if (pDoc->m_bIsRun == FALSE)
		{//外面已经通知结束了
			pNtfs->CloseFind(Finder);
			return 0;
		}
// 		if (WAIT_OBJECT_0 == WaitForSingleObject(pDoc->m_pEveIsRun->m_hObject , 0))
// 		{//已经出发了  需要返回
// 			pDoc->m_pEveIsRun->ResetEvent();
// 			pNtfs->CloseFind(Finder);
// 			return 0;
// 		}

		res = pNtfs->FindNext(Finder , &liMft  );
		if (res == DR_FAT_EOF)//TODO超找完毕了
			break;
		
		if (res != DR_OK)//TODO 超找失败了
			break;

		res = pNtfs->OpenFileW(&file , liMft);
		//if (res != DR_OK)			
		
//  ID  文件名 MFT记录 实际大小  分配大小 创建时间 MFT修改时间  数据修改时间 访问时间 
		strTemp.Format(_T("%d") , i);	//ID

		if (file.IsDir())
		{
			pList->InsertItem( i  , strTemp ,1 );
		}else{
			pList->InsertItem( i  , strTemp ,0 );
		}
		

		//文件名
		file.GetFileName( path , MAX_PATH + 1 );
		strTemp = path;
		pList->SetItemText(i , 1  , strTemp);//名字

		//MFT记录
		liMft.HighPart == 0 ? strTemp.Format(_T("%X") , liMft.LowPart) :strTemp.Format(_T("%X%X") ,liMft.HighPart , liMft.LowPart) ;
		pList->SetItemText(i , 2  , strTemp);//其实扇区号

		if (!file.IsDir())
		{//目录是没有文件大小的
			//实际大小
			strTemp = GetSizeToString(file.GetRealSize());
			//liTemp.HighPart == 0 ? strTemp.Format(_T("%X") , liTemp.LowPart) :strTemp.Format(_T("%X%X") ,liTemp.HighPart , liTemp.LowPart) ;
			pList->SetItemText(i , 3  , strTemp);//占用的扇区数
		
			//文件的分配大下
		//	dwTemp = file.GetFileSize();
			strTemp = GetSizeToString(file.GetAllocSize());
			pList->SetItemText(i , 4  , strTemp);//文件的大小

// 			//文件的分配大小
// 			dwTemp = file.GetSecCount() * SECTOR_SIZE;
// 			strTemp = GetSizeToString(dwTemp);
// 			pList->SetItemText(i , 5  , strTemp);//占用的扇区数
		
		}

		//文件的创建时间
		tTemp = (time_t)file.GetCreateTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 5  , strTemp);//占用的扇区数


		//MFT修改时间
		tTemp = (time_t)file.GetMFTChgTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 6  , strTemp);//占用的扇区数

		//数据修改时间
		tTemp = (time_t)file.GetAlteredTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 7  , strTemp);//占用的扇区数

		//访问时间
		tTemp = (time_t)file.GetReadTime().QuadPart;
		tmTemp = *localtime(&tTemp);
		strTemp.Format(_T("%d-%02d-%02d %02d:%02d:%02d")
			,tmTemp.tm_year + 1900 , tmTemp.tm_mon + 1 , tmTemp.tm_mday ,
			tmTemp.tm_hour , tmTemp.tm_min , tmTemp.tm_sec);
		pList->SetItemText(i , 8  , strTemp);//占用的扇区数


		file.Close();
		++i;
	}

	//释放需要释放的资源
	pNtfs->CloseFind(Finder);
//	pNtfs->FindClose(Finder);

	return 0;
}

DWORD WINAPI GetFATClustList( PVOID wParam )
{
	//fat32文件属性对话框
	CFat32FileDlg* dlg = (CFat32FileDlg*)wParam;
	CListCtrl*		pList = NULL;
	DFat32File*		pFile = dlg->m_pFile;
	DFat32*			pFat32 = dlg->m_pDoc->m_pFat32;
	DWORD			dwStartClust = 0;
	DWORD			dwClust = 0;
	DWORD			dIndex = 0;
	CString			strTemp;

	//起始簇号
	dwStartClust = pFile->GetStartClust();
	//表明线程已经在使用
	dlg->m_bIsThreadRun = TRUE;
	
	//FAT1 簇链
	pList = (CListCtrl*)(dlg->GetDlgItem(IDC_FAT1_LIST));
	
	//先清理一下空间
	pList->DeleteAllItems();

	//获得起始簇号
	dwClust = dwStartClust;
	dIndex = 0;
	while ((0 != dwClust) && !IsFATEnd(dwClust))
	{
		if (FALSE == dlg->m_bIsThreadRun)  //仿佛在线程外面需要关闭线程了
			return 0 ;
		strTemp.Format(_T("%d") , dIndex);
		pList->InsertItem(dIndex , strTemp );
		strTemp.Format(_T("%08X") , dwClust);
		pList->SetItemText(dIndex , 1 , strTemp);
		
		dwClust = pFat32->GetFATFromFAT1(dwClust);
		if (1 == dwClust || 0xFFFFFFFF == dwClust)
		{//无效簇号
			break;
		}
		++dIndex;
	}

	//FAT1 簇链
	pList = (CListCtrl*)(dlg->GetDlgItem(IDC_FAT2_LIST));
	//先清理一下空间
	pList->DeleteAllItems();
	//获得起始簇号
	dwClust = dwStartClust;
	dIndex = 0;
	while ((0 != dwClust) && !IsFATEnd(dwClust))
	{
		if (FALSE == dlg->m_bIsThreadRun)  //仿佛在线程外面需要关闭线程了
			return 0 ;
		strTemp.Format(_T("%d") , dIndex);
		pList->InsertItem(dIndex , strTemp );
		strTemp.Format(_T("%08X") , dwClust);
		pList->SetItemText(dIndex , 1 , strTemp);

		dwClust = pFat32->GetFATFromFAT2(dwClust);
		if (1 == dwClust || 0xFFFFFFFF == dwClust)
		{//无效簇号
			break;
		}
		++dIndex;
	}
	
	//关闭线程的占用
	dlg->m_bIsThreadRun = FALSE;

	return 0;
}

DWORD WINAPI CopyFat32File( PVOID wParam )
{
	CCopyProcessDlg*	pDlg = (CCopyProcessDlg*)wParam;
	DFat32File			dFile;//FAT32的文件
	HANDLE				hFile;//将要写的文件
	CProgressCtrl*		pProg;//进度条
	CStatic*			pStatic;		//进度控制
	DRES				res = DR_OK;
	char				buf[SECTOR_SIZE * 10] = {0};//文件数据缓存
	DWORD				dwSize = 0;		//文件的总大小
	DWORD				dwRead = 0;		//文件已经读取了的大小
	DWORD				dwReaded = 0;	//一次读取的数据
	DWORD				dwWrite = 0;
	double				fRate = 0;		//复制比例
	CString				strTemp;
	DFat32*				pFAT32;

	//获得进度条的指针
	pProg = (CProgressCtrl*)pDlg->GetDlgItem(IDC_COPY_PROGRESS );
	//获得进度信息的控件
	pStatic = (CStatic*)pDlg->GetDlgItem(IDC_ST_PROGRESS );

	pFAT32 = ((CFat32Doc*)pDlg->m_pDoc)->m_pFat32;
	//打开将要读取文件
	res = pFAT32->OpenFile(pDlg->m_strFileToRead , &dFile);
	if ( DR_OK != res )
	{//打开文件失败
		::SendMessage(pDlg->GetSafeHwnd() , DMSG_OPEN_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToRead) , 0);
		return 0;
	}

	//打开要写的文件
	hFile = ::CreateFile(pDlg->m_strFileToWrite , GENERIC_WRITE , FILE_SHARE_READ , NULL , CREATE_ALWAYS ,FILE_ATTRIBUTE_NORMAL , NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{//打开将要写的文件失败 
		::SendMessage(pDlg->GetSafeHwnd() , DMSG_OPEN_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToWrite) , 0);
		dFile.Close();
		return 0;
	}

	//开始拷贝
	pDlg->m_bIsCopying = TRUE;

	dwSize = dFile.GetFileSize();
	dwRead = 0;
	//开始复制文件
	while( (DR_OK == res) && !dFile.IsEOF() )
	{
		if (pDlg->m_bIsCopying == FALSE)
		{//已经在外面关闭了
			dFile.Close();
			CloseHandle(hFile);
			return 0;
		}
		res = dFile.ReadFile(buf , &dwReaded , SECTOR_SIZE * 10 );
		if(FALSE == WriteFile(hFile , buf, dwReaded , &dwWrite , NULL ))
		{//写文件失败
			::SendMessage(pDlg->GetSafeHwnd() , DMSG_COPY_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToRead) , (WPARAM)&(pDlg->m_strFileToWrite));
			dFile.Close();
			::CloseHandle(hFile);
			pDlg->m_bIsCopying = FALSE;
			return 0;
		}
		
		//总的读取了的文件
		dwRead += dwReaded;
		fRate = (dwRead/(dwSize * 1.0)) * 100;
		strTemp.Format(_T("(%s/%s) %.2f%%") , GetSizeToString(dwRead) , GetSizeToString(dwSize) , fRate);
		pStatic->SetWindowText(strTemp);
		pProg->SetPos((int)fRate);
	}

	if (dFile.IsEOF())
	{//文件复制结束  
		::PostMessage(pDlg->GetSafeHwnd() , DMSG_COPY_SUCCESS , 0 , 0);
	}else{
		//复制文件失败
		::SendMessage(pDlg->GetSafeHwnd() , DMSG_COPY_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToRead) , (WPARAM)&(pDlg->m_strFileToWrite));
	}

	//释放一些资源
	dFile.Close();
	CloseHandle(hFile);
	pDlg->m_bIsCopying = FALSE;

	return 0;
}

DWORD WINAPI CopyNtfsFile( PVOID wParam )
{
	CCopyProcessDlg*	pDlg = (CCopyProcessDlg*)wParam;
	DNtfsFile			dFile;	//NTFS的文件
	HANDLE				hFile;	//将要写的文件
	CProgressCtrl*		pProg;	//进度条
	CStatic*			pStatic;//进度控制
	DRES				res = DR_OK;
	char				buf[SECTOR_SIZE * 10] = {0};//文件数据缓存
	LONG_INT			dwSize = {0};	//文件的总大小
	DWORD				dwRead = 0;		//文件已经读取了的大小
	DWORD				dwReaded = 0;	//一次读取的数据
	DWORD				dwWrite = 0;
	double				fRate = 0;		//复制比例
	CString				strTemp;
	DNtfs*				pNtfs;

	//获得进度条的指针
	pProg = (CProgressCtrl*)pDlg->GetDlgItem(IDC_COPY_PROGRESS );
	//获得进度信息的控件
	pStatic = (CStatic*)pDlg->GetDlgItem(IDC_ST_PROGRESS );

	pNtfs = ((CNtfsDoc*)pDlg->m_pDoc)->m_pNtfs;
	//打开将要读取文件
	res = pNtfs->OpenFileA((LPCSTR)(LPCTSTR)pDlg->m_strFileToRead , &dFile);
	if ( DR_OK != res )
	{//打开文件失败
		::SendMessage(pDlg->GetSafeHwnd() , DMSG_OPEN_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToRead) , 0);
		return 0;
	}

	//打开要写的文件
	hFile = ::CreateFile(pDlg->m_strFileToWrite , GENERIC_WRITE , FILE_SHARE_READ , NULL , CREATE_ALWAYS ,FILE_ATTRIBUTE_NORMAL , NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{//打开将要写的文件失败 
		::SendMessage(pDlg->GetSafeHwnd() , DMSG_OPEN_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToWrite) , 0);
		dFile.Close();
		return 0;
	}

	//开始拷贝
	pDlg->m_bIsCopying = TRUE;

	//获得文件的实际大小
	dwSize = dFile.GetRealSize();
	dwRead = 0;
	//开始复制文件
	while( (DR_OK == res) && !dFile.IsEOF() )
	{
		if (pDlg->m_bIsCopying == FALSE)
		{//已经在外面关闭了
			dFile.Close();
			CloseHandle(hFile);
			return 0;
		}
		res = dFile.ReadFile(buf , &dwReaded , SECTOR_SIZE * 10 );
		if(FALSE == WriteFile(hFile , buf, dwReaded , &dwWrite , NULL ))
		{//写文件失败
			::SendMessage(pDlg->GetSafeHwnd() , DMSG_COPY_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToRead) , (WPARAM)&(pDlg->m_strFileToWrite));
			dFile.Close();
			::CloseHandle(hFile);
			pDlg->m_bIsCopying = FALSE;
			return 0;
		}

		//总的读取了的文件
		dwRead += dwReaded;
		fRate = (dwRead/(dwSize.QuadPart * 1.0)) * 100;
		strTemp.Format(_T("(%s/%s) %.2f%%") , GetSizeToString(dwRead) , GetSizeToString(dwSize) , fRate);
		pStatic->SetWindowText(strTemp);
		pProg->SetPos((int)fRate);
	}

	if (dFile.IsEOF())
	{//文件复制结束  
		::PostMessage(pDlg->GetSafeHwnd() , DMSG_COPY_SUCCESS , 0 , 0);
	}else{
		//复制文件失败
		::SendMessage(pDlg->GetSafeHwnd() , DMSG_COPY_FILE_FAILED ,(WPARAM)&(pDlg->m_strFileToRead) , (WPARAM)&(pDlg->m_strFileToWrite));
	}

	//释放一些资源
	dFile.Close();
	CloseHandle(hFile);
	pDlg->m_bIsCopying = FALSE;

	return 0;
}


DWORD WINAPI PosFileInExplore( PVOID wParam )
{
// 	CString strTemp = _T("  /select , ");
// 	strTemp += *((CString*)wParam);
// 	ShellExecute(AfxGetMainWnd()->GetSafeHwnd() ,NULL, _T("explorer.exe") ,  strTemp  ,   NULL,   SW_SHOWNORMAL);
	
	CString strTemp = _T("explorer   /n , /select, ");
	strTemp += *((CString*)wParam);
	WinExec((LPCSTR)(LPCTSTR)strTemp ,  SW_SHOWNORMAL );
	return 0;
}

CString	GetNtfsAttrTypeName( DWORD attrType)
{
	CString strType;

	switch (attrType)
	{
	case AD_STANDARD_INFORMATION:  //标准属性
		strType.LoadString(IDS_AD_STANDARD_INFORMATION);
		break;
	case AD_ATTRIBUTE_LIST:	//属性列表
		strType.LoadString(IDS_AD_ATTRIBUTE_LIST);
		break;
	case AD_FILE_NAME://文件名
		strType.LoadString(IDS_AD_FILE_NAME);
		break; 

#if FOR_NT == 1 
	case AD_VOLUME_VERSION://卷版本//WinNT
		strType.LoadString(IDS_AD_VOLUME_VERSION);
		break;

#elif FOR_NT == 0 
	case AD_OBJECT_ID://对象ID //Win2K
		strType.LoadString(IDS_AD_OBJECT_ID);
		break;
#endif // FOR_NT
 
	case AD_SECURITY_DESCRIPTOR://安全描述
		strType.LoadString(IDS_AD_SECURITY_DESCRIPTOR);
		break;
	case AD_VOLUME_NAME://卷名
		strType.LoadString(IDS_AD_VOLUME_NAME);
		break;
	case AD_VOLUME_INFORMATION://卷信息
		strType.LoadString(IDS_AD_VOLUME_INFORMATION);
		break;
	case AD_DATA://数据属性
		strType.LoadString(IDS_AD_DATA);
		break;
	case AD_INDEX_ROOT:
		strType.LoadString(IDS_AD_INDEX_ROOT);
		break;
	case AD_INDEX_ALLOCATION:
		strType.LoadString(IDS_AD_INDEX_ALLOCATION);
		break;
	case AD_BITMAP:
		strType.LoadString(IDS_AD_BITMAP);
		break;

#if FOR_NT == 1 
	case AD_SYMBOL_LINK://WinNT
		strType.LoadString(IDS_AD_SYMBOL_LINK);
		break;

#elif FOR_NT == 0 
	case AD_REPARSE_POINT://Win2K
		strType.LoadString(IDS_AD_REPARSE_POINT)
		break;
#endif // FOR_NT

	case AD_EA_INFORMATION:
		strType.LoadString(IDS_AD_EA_INFORMATION);
		break;
	case AD_EA:
		strType.LoadString(IDS_AD_EA);
		break;
	case AD_PROPERTY_SET://WinNT
		strType.LoadString(IDS_AD_PROPERTY_SET);
		break;
	case AD_LOGGED_UNTILITY_STREAM://Win2K
		strType.LoadString(IDS_AD_LOGGED_UNTILITY_STREAM);
		break;
	default:
		strType = _T("");
	}
	return strType;
}

DWORD WINAPI GetFAT32FileSectorList( PVOID wParam )
{
	CFat32Doc*		pDoc = (CFat32Doc*)wParam;
	DFat32*			pFat32 = pDoc->m_pFat32;
	DWORD			dwClust = 0;
	SectorList*     pSecList = pDoc->m_pCurSecList;
	DWORD			dwSecPerClust = pFat32->GetSecPerClust();

	//起始簇号
	dwClust = pFat32->SectToClust( DWORD(pDoc->m_secList.GetStartSector().QuadPart) );
	//表明线程已经在使用
	pDoc->m_bIsGetSeclistRun = TRUE;

	while ((0 != dwClust) && !IsFATEnd(dwClust))
	{
		if (FALSE == pDoc->m_hGetSectorListThread)  //仿佛在线程外面需要关闭线程了
			return 0 ;
		//获得下一簇
		dwClust = pFat32->GetFATFromFAT1(dwClust);
		if (1 == dwClust || 0xFFFFFFFF == dwClust)
		{//无效簇号
			break;
		}
		
		//添加一个簇的数据
		pSecList->AddSector(pFat32->ClustToSect(dwClust) , dwSecPerClust);
	}

	//标记线程退出
	pDoc->m_bIsGetSeclistRun = FALSE;

	return 0 ;
}
