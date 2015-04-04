//////////////////////////////////////////////////////////////////////////
// Module:		Disk.cpp
// Author:		杨松
// Created:		2012年3月20日 星期二
// Purpose:		实现Disk声明的方法
// Comment:		这个类主要是包含了一下物理磁盘的操作方法
//////////////////////////////////////////////////////////////////////////

#include "disktool.h"
#include "stut_connf.h"


Disk::Disk(void)
: mPPartList(NULL)
, mUnPartSize(0) 
, mIsGetUnPartSec(FALSE)
, mDisk(INVALID_HANDLE_VALUE)
, mVolCnt(0)
, mSecPerTrack(0)
, mTracksPerCylinder(0)
, mMainVolCount(0)
{
	this->mExtPos.QuadPart = 0;
	mPartableSecCnt.QuadPart = 0;
	mDevName[0] = 0;
	mCylinders.QuadPart = 0;
}

Disk::~Disk(void)
{
/*	CloseDisk();*/
}

int Disk::GetDiskCount(void)
{
	HANDLE   hDrive = NULL;             //驱动设备
	int      nCount = 0;                //设备数量
	char     czhDriveName[30] = {0};    //设备名字
	int		 i		=	 0;

	//车老板说windows最多可以挂载0x80个物理磁盘
	//但是我在一个国外的源程序中看到他是以0x40为上限的
	for (i = 0 ; i < MAX_DISK_COUNT ; ++i)
	{
		sprintf(czhDriveName ,"%s%d",DISK_PRE_NAME , i);
		hDrive = CreateFileA(
			czhDriveName,						//要打开的设备 
			0,									//访问模式
			FILE_SHARE_READ | FILE_SHARE_WRITE,	//共享模式
			NULL,								//默认的安全属性
			OPEN_EXISTING,						//打开方式
			0,									//默认的文件的属性
			NULL);								//不复制任何文件属性
		if(hDrive == INVALID_HANDLE_VALUE)		//没有了   
			continue;
		else{  
			::CloseHandle(hDrive);
			++nCount;
		}
	}

	return nCount;
}

BOOL Disk::OpenDisk(int index)
{
	//序号无效则是是不
	if(index < 0)	return FALSE;

	char name[30] = {0};
	::sprintf(name ,"%s%d" ,DISK_PRE_NAME, index);
	
	return OpenDisk(name);
}

BOOL Disk::OpenDisk(char* czName)
{
	DWORD		  dwOutBytes = 0; 
	DISK_GEOMETRY mGeometry  = {0};
	BOOL		  res		 = FALSE;  //操作结构

	//如果已经打开了一个磁盘就不能在打开了
	if (czName == NULL || strlen(mDevName)) return FALSE;

	this->mDisk =   CreateFile( czName , GENERIC_READ | GENERIC_WRITE ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,  //共享模式
		NULL , OPEN_EXISTING , 0 , NULL);

	if(mDisk == INVALID_HANDLE_VALUE)  
		res = FALSE;                        //打开磁盘失败
	else{
	
		//计算磁盘的可分区的扇区数 
		res = ::DeviceIoControl(mDisk,		// 设备句柄
			IOCTL_DISK_GET_DRIVE_GEOMETRY,	// 取磁盘参数
			NULL , 0 , &mGeometry,
			sizeof(DISK_GEOMETRY),			// 输出数据缓冲区
			&dwOutBytes , (LPOVERLAPPED)NULL);			// 用同步I/O

		if(!res)  //获取磁盘的信息失败
		{
			::CloseHandle(mDisk);//关闭打开的磁盘设备
			mDisk = INVALID_HANDLE_VALUE;
			return res;
		}
		//计算磁盘的可分区的扇区数
		this->mPartableSecCnt.QuadPart = mGeometry.Cylinders.QuadPart \
			* mGeometry.TracksPerCylinder *  mGeometry.SectorsPerTrack;     

		strcpy(mDevName , czName);//先保存一下名字
		mSecPerTrack = mGeometry.SectorsPerTrack;
		mTracksPerCylinder = mGeometry.TracksPerCylinder;
		mCylinders.QuadPart = mGeometry.Cylinders.QuadPart;
		
		//获取磁盘的分区列表
		if(LoadPartList(/*hDisk*/)){
			//排序
			PDList(this->mPPartList)->SortList(ComparePart);
			//添加各个空隙节点
			MakeListContinue(/*hDisk*/);
			res = TRUE;
		}else{
			mDisk = INVALID_HANDLE_VALUE;
			mDevName[0] = 0;
			res = FALSE;
		}
	}
	return res;
}

BOOL Disk::ReadSecter(/*HANDLE hDisk ,*/LONG_INT offert , PVOID buf  ,DWORD* dwReaded ,DWORD buflen)
{
	if(offert.QuadPart <  0 || buf == NULL || !dwReaded)
		return FALSE;
	
	offert.QuadPart *= SECTOR_SIZE;
	::SetFilePointer(mDisk , offert.LowPart ,  &(offert.HighPart ), FILE_BEGIN);
	return ::ReadFile(mDisk , buf, buflen , dwReaded , NULL);
}

DRES Disk::ReadData(void* buf , LONG_INT off , DWORD dwRead)
{
	DRES	 res		= DR_OK;
	DWORD	 dwReaded   = 0;
	LONG_INT offset		= {0};
	/*	HANDLE hDev = INVALID_HANDLE_VALUE;*/
	
	//设备没有打开
	if (0 == strlen(this->mDevName)) return DR_INIT_ERR;

	if (off.QuadPart >= this->mPartableSecCnt.QuadPart + this->mUnPartSize / SECTOR_SIZE)
	{//指针越界
		return DR_DEV_CTRL_ERR;
	}
	
	offset.QuadPart = off.QuadPart;						//读取数据的实际偏移
	offset.QuadPart *= SECTOR_SIZE;					//字节偏移
	
	//设置文件指针
	offset.LowPart = SetFilePointer(mDisk , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
	if (offset.LowPart == -1 && GetLastError() != NO_ERROR )
		res = DR_DEV_CTRL_ERR;
	
	//读取数据
	if(!res && !::ReadFile(mDisk , buf ,
		dwRead ,&dwReaded ,NULL) && dwReaded != dwRead)	
		res =  DR_DEV_IO_ERR;
	
	/*	CloseHandle(hDev);								//关闭已经打开的设备*/
	return res;
}

int Disk::GetBytePerSec()
{
	return 512;
}


int Disk::GetSectorPerTrack()
{
	//设备没有打开
	if (0 == strlen(this->mDevName)) return -1;

	return  mSecPerTrack;
}

DWORD Disk::GetTracksPerCylinder()
{
	//设备没有打开
	if (0 == strlen(this->mDevName)) return 0;

	return  mTracksPerCylinder;
}

LONG_INT Disk::GetCylinders()
{
	LONG_INT res;
	res.QuadPart = -1;
	//设备没有打开
	if (0 == strlen(this->mDevName)) return res;

	return  mCylinders;
}


DWORD Disk::GetUnPartableSize()
{
	if (0 == strlen(mDevName)) return 0;
	return mUnPartSize;
}

void clearDList(DList* pList)
{
	if (!pList || pList->GetCount() == 0)
		return;

	//要删除每一个节点
	int nCount = pList->GetCount();
	for (int i = 0; i < nCount; ++i)
	{
		PDPart pPart = (PDPart)pList->GetPart(i);
		delete pPart;
	}
	pList->Clear();
}

void Disk::CloseDisk(void)
{
	//清理一下名字
	mDevName[0] = 0;

	//释放分区链表
	if (mPPartList)
	{
		clearDList(mPPartList);
		delete mPPartList;
		mPPartList = NULL;
	}

	//关闭已经打开的设备
	CloseHandle(mDisk);
	mDisk = INVALID_HANDLE_VALUE;

}
BOOL Disk::IsDiskOpened()
{
	return ((strlen(this->mDevName) != 0) &&
		(mDisk != INVALID_HANDLE_VALUE || mDisk != NULL));
}

BOOL Disk::ListPartion(/*HANDLE hDisk ,*/ PVOID dp , LONG_INT dptoff , BOOL isFirstDPT ,DWORD* pLogicDir)
{
	//实际的EBR(MBR)  或分区的偏移
	LONG_INT o    = {0};
	PVOID	 pn	  = NULL;
	DPT		 dpt  = *PDPT(dp);
	PDList   list = PDList(this->mPPartList);

	//如果当前还在分析第一个DPT表中的表项是
	//那么EBR 或者 分区的物理偏移就在DPT的表象中
	//如果不是磁盘中的第一个DPT表了  
	//那么EBR的偏移就是当前DPT中的相对偏移加上扩展分区的偏移
	//而分区的偏移则是想对当前DPT的
	//扩展分区的偏移就是 第一个DPT表中的扩展分区表项的相对偏移
	if(isFirstDPT){
		o.QuadPart = dpt.mRelativeSectors;
		if(IsExtPart(&dpt))//第一个分区表中的扩展就是整个磁盘的扩展分区
			this->mExtPos.QuadPart = dpt.mRelativeSectors;
	}else{
		if(IsExtPart(&dpt))
			o.QuadPart = this->mExtPos.QuadPart + dpt.mRelativeSectors;
		else
			o.QuadPart = dptoff.QuadPart + dpt.mRelativeSectors;
	}

	if (IsExtPart(&dpt))               //判断要分析的分区是否是扩展分区
	{//当前要列举的
		int      i   = 0;
		EBR      ebr = {0};
		DWORD    dwReaded = 0;
		if(!ReadSecter(/*mDisk ,*/ o , &ebr ,&dwReaded))              //读取扩展扩展分区的EBR
			return FALSE;                      //读取EBR失败
		if(ebr.mEndSig != MBR_END || dwReaded != SECTOR_SIZE)
			return FALSE;
		
		//新建一个相应的节点添加到表中去
		pn = NewPart(NULL , &o  ,PART_EBR );
		if(NULL != pn)
			list->AddPart(PVOID(pn)); //将节点添加到链表中去
		

		//在EBR中 只有两条DPT有效 ，
		//第一条DPT 指向当前逻辑驱动器
		//第二条DPT 指向下一个逻辑分区的EBR
		for(i = 0 ; i < 4 ; ++i)               //迭代每一个EBR中的DPT表项
		{ 
			if(ebr.mDPT[i].mSectorCount == 0)
				continue;
			ListPartion(/*mDisk , */&(ebr.mDPT[i]) ,o  , FALSE , pLogicDir);
		}
	}
	else
	{
		//不是逻辑分区	
		//新建一个相应的节点添加到表中去
		pn = NewPart(&dpt , &o  , 0  , isFirstDPT , pLogicDir);
		if(NULL != pn) {
			list->AddPart(PVOID(pn)); //将节点添加到链表中去
			if (isFirstDPT) ++mMainVolCount;  //是主分区
		}
	}
	return TRUE;
}

BOOL Disk::LoadPartList(/*HANDLE hDisk*/)
{
	//已经加载了列表
	if(this->mPPartList)
		return FALSE;

	//创建链表
	this->mPPartList = new DList();

	//开始解析 整个物理磁盘   获得其分区链表
	LONG_INT	offert  = {0};
	MBR			mbr		= {0};      //磁盘的MBR
	int			i	    = 0 ;
	DWORD		dwRead	= 0;
	PDList		list	= PDList(this->mPPartList);
	PVOID		pn		= NULL;
	DWORD*		pLogicDir = NULL;		//逻辑驱动信息
	BOOL		res		= TRUE;		//一些操作结果 

	if (!ReadSecter(/*hDisk, */offert ,&mbr , &dwRead))          //读取磁盘的MBR
		return FALSE;                              //读取MBR失败

	//检查结束标记
	if(mbr.mEndSig != MBR_END || dwRead != SECTOR_SIZE)
		return FALSE;

	//获得逻辑驱动信息
	pLogicDir = GetLogicalDrives();

	//还要判断是否是DBR或者MBR  这里暂时默认为MBR


	
	//新建一个相应的节点添加到表中去
	if (pn = NewPart(NULL, &offert, PART_MBR))
		list->AddPart(pn); //将节点添加到链表中去
	
	res = TRUE;
	for (i = 0 ; res == TRUE && i < 4 ; ++i)              //每个分区表最多只有四条记录
	{
		if (!mbr.mDPT[i].mSectorCount)
			continue;                      //当前记录的分区长度为0

		res = ListPartion(&(mbr.mDPT[i]) , offert ,TRUE , pLogicDir);
	}

	if (res == FALSE)  //解析MBR中的分区表失败？？
	{
		res = TRUE;
		//开始解析第二个扇区的中的可能存在的分区信息
		char		extBuf[SECTOR_SIZE] = {0};	//一个扇区的缓冲空间
		PExtDPT		pExtDPT = NULL;//扩展分区表的起始地址
		LONG_INT	liSecOff;
		int			i = 0;
		int			flag	=	0;	//0表示第一个表项 
		
		liSecOff.QuadPart = 1;		//第一个要读取的是一号扇区
	
		if (!ReadSecter(liSecOff , extBuf , &dwRead , SECTOR_SIZE))
		{//读取数据失败
			res = FALSE;
		}

		if ((res != FALSE) && (PLONG_INT(PExtDPT(extBuf)->partName)->QuadPart != EXT_DPT_FLAG))
		{//不是一个扩展分区表
			res = FALSE;
		}

		if (TRUE == res )
		{//是一个有效的扩展分区表
			//需要删除原来已经建立好的链表
			//释放分区链表
			if(NULL != list){
				clearDList(list);
				delete list;
			}

			//重新分配链表空间
			list = new DList();
			this->mPPartList = list;

			//感觉MBR还是必须有的
			offert.QuadPart = 0 ;
			pn = NewPart(NULL , &offert  , PART_MBR );
			if(NULL != pn)
				list->AddPart(pn); //将节点添加到链表中去
		}

		while (TRUE == res)
		{
			for(i = 0 ; i < 16 ; ++i)
			{//遍历每一个表项
				pExtDPT = ((PExtDPT)extBuf) + i;
				if (IsExtPDPTEmpty(pExtDPT))
				{//这是一个空表项
					break;
				}
				
				if (0 != PLONG_INT(pExtDPT->partName)->QuadPart)
				{//是一个有效的表项
					offert.QuadPart = pExtDPT->startSector;
					pn = NewPart(NULL , &offert  , pExtDPT->partFormate  , FALSE , pLogicDir);
					if(NULL != pn) {
						PDPart(pn)->mSecCount.QuadPart = (flag == 0) ? (pExtDPT->sectorCount - 0xEC000000) : pExtDPT->sectorCount;
						list->AddPart(PVOID(pn)); //将节点添加到链表中去
					}
					flag = 1;
				}
			}

			if (16 != i)
			{//解析完毕
				break;
			}else{
				//需要读取下一个扇区进行测试
				++liSecOff.QuadPart;
				if (!ReadSecter(liSecOff , extBuf , &dwRead , SECTOR_SIZE))
				{//读取数据失败
					break;
				}
			}
		}
	}


	//需要的话就清理一下
	if (pLogicDir)
		delete[] pLogicDir;

	return TRUE;
}

int Disk::GetPartCount(void)
{
	return PDList(this->mPPartList)->GetCount();
}
DWORD Disk::GetVolumeCount()
{
	if (0 == strlen(mDevName)) return 0;
	return mVolCnt;
}
DWORD Disk::GetMainVolCount()
{
	if (0 == strlen(mDevName)) return 0;
	return mMainVolCount;
}


const Disk::PDPart Disk::GetPart( int index )
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//指定分区不存在
	if (!p)	return NULL;
	
	return p;	
}


const Disk::PDPart Disk::GetPart( char letter )
{
	PDList list = PDList(this->mPPartList);
	int cnt = list->GetCount();
	PDPart pp = NULL;
	int i;

	for (i = 0 ; i < cnt ; ++i)
	{
		pp = (PDPart)list->GetPart(i);
		if (pp->mLogicalLetter == letter)
		{
			return pp;
		}
	}

	return NULL;
}


USHORT Disk::GetPartFormat(int index)
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//指定分区不存在
	if (!p)	return 0xFFFF;

	return p->mType;	
}

DWORD Disk::GetRelativeSectors(int index)
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//指定分区不存在
	if (!p) return 0xFFFFFFFF;

	return p->mRelativeSectors;
	//return p->GetRelativeSectors();	
}
LONG_INT Disk::GetPartSectorCount(int index)
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//指定分区不存在
	if (!p){
		LONG_INT li;
		li.QuadPart = -1;
		return li;
	}
	return p->mSecCount;
	//return p->GetSectorCount();	
}
LONG_INT Disk::GetPartOffset(int index)
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//指定分区不存在
	if (!p){
		LONG_INT li;
		li.QuadPart = -1;
		return li;
	}
	return p->mOffset;
	//return p->GetPartOffset();	
}


BOOL Disk::IsActivityPart( int index )
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//指定分区不存在
	if (!p){
		LONG_INT li;
		li.QuadPart = -1;
		return FALSE;
	}
	return p->mIsActivity;
	//return p->IsActivityPart();	
}


const char* Disk::GetDevName(void)
{
	return mDevName;
}

PVOID Disk::NewPart(PVOID dp, PLONG_INT off , int type , BOOL isMainPart /*= FALSE*/, DWORD* pLogicDri /*= NULL*/)
{
	if(!off)
		return NULL;

	PDPT		dpt = PDPT(dp);
	DWORD		i = 0;
	LONG_INT	offset = {0};

	//新建一个分区域节点
	PDPart pn = new DPart();
	memset(pn , 0 , sizeof(DPart) );
	pn->mVolIndex = -1;

	if(NULL != dpt)
	{
		//有DPT的数据
		pn->mIsActivity = (dpt->mGuidFlag == 0x80);
		pn->mRelativeSectors = dpt->mRelativeSectors;
		pn->mSecCount.QuadPart = dpt->mSectorCount;
		type = dpt->mPartType;
	}
	pn->mType		= type;				//节点的类型
	pn->mOffset		= *off;	            //节点变量的物理偏移地址
	pn->mIsMainPart = isMainPart;		//是否为主分区
	if (IsPartFS(type))
	{//当前添加的节点是一个文件系统
		pn->mVolIndex = ++this->mVolCnt;
		if (pLogicDri)
		{
			//当前区域在物理设备上的字节偏移
			offset = *off;
			offset.QuadPart *= SECTOR_SIZE;

			for(i = 0 ; i < 26 ; ++i)
			{
				if (PLOGCDRI(pLogicDri)[i].byteOffset.QuadPart == offset.QuadPart )
				{//匹配到了
					pn->mLogicalLetter = (char)PLOGCDRI(pLogicDri)[i].letter;
				}
			}
		}
	}
	else
	{
		pn->mVolIndex = -1;
	}

	return pn;
}

BOOL Disk::ComparePart(VOID* p1, VOID* p2)
{
	return (PDPart(p1)->mOffset.QuadPart < PDPart(p2)->mOffset.QuadPart);
}



DWORD* Disk::GetLogicalDrives()
{
	//逻辑驱动信息
	DWORD		dwDris = 0;
	DWORD		temp = 0;
	PLOGCDRI	pLogic;
	char devNmae[MAX_PATH] = {0};
	size_t		len = strlen(mDevName);
	size_t		i = 0, nCnt = 0;
	DWORD		index = 0;
	char		csLogicN[] = "\\\\?\\A:";
	HANDLE		hDev = INVALID_HANDLE_VALUE;
	//STORAGE_DEVICE_NUMBER deviceInfo;
	DWORD		bytesReturned = 0;
	char		outBuf[100] = {0};		//数据查询时的输出Buf
	VOLUME_DISK_EXTENTS* pOutBuf = (VOLUME_DISK_EXTENTS*)outBuf;

	DWORD res = 0;

	strcpy(devNmae , mDevName);

	//17 == strlen("\\\\.\\PHYSICALDRIVE")
	//20 == strlen("\\\\.\\PHYSICALDRIVE127")  最多只有128(0x80)个磁盘
	if (len < 18 || len > 20) return NULL;
	
	//先全部转换陈大写
	for (i = 0; i < len; ++i)
	{
		devNmae[i] = toupper(devNmae[i]);
	}

	//获得当前设备的索引
	if(0 == sscanf(devNmae , "\\\\.\\PHYSICALDRIVE%d" , &index ))
		return NULL;  //获取设备索引失败

	//获得当前系统中所有的逻辑驱动器
	dwDris = ::GetLogicalDrives();
	
	if ( 0 == dwDris )
		return NULL;

	//分配空间
	pLogic = new LOGCDRI[27];  //最多26个逻辑驱动还有用与表示数组的结束
	memset(pLogic , 0 , sizeof(LOGCDRI)*27);

	//最多只有26个逻辑驱动器
	for ( i = 0 ; i < 26 ; ++i)
	{
		if (0 == (dwDris & 0x00000001))
		{//当前没有
			dwDris >>= 1;
			continue;
		}

		csLogicN[4] = char('A' + i);
		hDev = CreateFileA( csLogicN , 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,	//共享模式
			NULL ,OPEN_EXISTING , 0 , NULL);								//不复制任何文件属性
		if (hDev == INVALID_HANDLE_VALUE)
			continue;
		
		//获取每一个在当前磁盘上的每一个卷的偏移位置，以便于获取每一个分区的逻辑驱动字符
		if(0 == ::DeviceIoControl( hDev,
			IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
			NULL, 0 , pOutBuf ,	100,
			&bytesReturned,	NULL ))
		{//获取信息失败
			res = GetLastError();
			CloseHandle(hDev);
			continue;
		}

		if (pOutBuf->Extents[0].DiskNumber != index)
		{//不是当前设备的
			CloseHandle(hDev);
			continue;
		}

		//找到了一个属于当前设备的卷
		pLogic[nCnt].byteOffset.QuadPart = pOutBuf->Extents[0].StartingOffset.QuadPart;
		pLogic[nCnt++].letter = char('A' + i);
		CloseHandle(hDev);
	}

	return (DWORD*)pLogic;
}

BOOL Disk::MakeListContinue(/*HANDLE hDisk*/)
{
	//给拍好序的链表添加各个空隙节点
	int			nCount	= this->GetPartCount();
	LONG_INT	temp	= {0};
	LONG_INT	offset	= {0};
	PDPart		t1		= NULL;
	PDPart		pn		= NULL;
	PDList		list	= PDList(mPPartList);
	LONG_INT	unPartCount = {0};
	PDPart		t2		= NULL;
	GET_LENGTH_INFORMATION gli = {0};
	DWORD		dwOutBytes = 0;
	BOOL		res		= FALSE;

	if(nCount)	t1		= PDPart(list->GetPart(0));

	for (int i = 1 ; i < nCount ; ++ i )
	{
		t2 = PDPart(list->GetPart(i));
		if(t1->mSecCount.QuadPart)
		{		//有分区大小
			temp.QuadPart =t2->mOffset.QuadPart 
				- t1->mOffset.QuadPart - t1->mSecCount.QuadPart;
			if(temp.QuadPart)
			{//有空隙
				offset.QuadPart = t1->mOffset.QuadPart + t1->mSecCount.QuadPart;
				pn = (PDPart)NewPart(NULL , &offset  , PART_UN_PART );
				if(pn){
					pn->mSecCount = temp;				
					list->InsertNode( i  , pn);
				}
				++nCount;
				++i;
			}
		}
		else	//没有分区大小
		{
			t1->mSecCount.QuadPart = t2->mOffset.QuadPart - t1->mOffset.QuadPart;
		}
		t1 = t2;
	}

	//添加一个未分区的区域大小节点
	t2 = PDPart(list->GetPart(nCount-1));      //获得最后一个节点
	offset.QuadPart = t2->mSecCount.QuadPart + t2->mOffset.QuadPart;		    //没有分区的区域物理偏移
	unPartCount.QuadPart = this->mPartableSecCnt.QuadPart - offset.QuadPart;//不可分区的区域大小
	if(unPartCount.QuadPart > 0 )				    //这里默认当中是LAB模式
	{//后面有没有分区的区域
		if (pn = (PDPart)NewPart(NULL, &offset, PART_UN_PART))
		{
			pn->mSecCount = unPartCount;
			list->AddPart( pn);
		}
	}

	//计算磁盘的不可分区空间大小
	res = ::DeviceIoControl(mDisk , IOCTL_DISK_GET_LENGTH_INFO , NULL ,
		0 , &gli , sizeof(gli) , &dwOutBytes,   NULL);			// 用同步I/O
	if(res)
	{//成功获取磁盘的大小
		//不可分区的扇区数
		mUnPartSize = (DWORD)(gli.Length.QuadPart  - mPartableSecCnt.QuadPart * SECTOR_SIZE);

		//创建一个新的节点
		if (pn = (PDPart)NewPart(NULL, &mPartableSecCnt, PART_UNPARTBLE))
		{
			pn->mSecCount.QuadPart = mUnPartSize / SECTOR_SIZE;
			PDList(this->mPPartList)->AddPart( pn);
			mIsGetUnPartSec = TRUE;
		}
	}

	return TRUE;
}
// 
// BOOL Disk::GetUnPartSecCount(DWORD * secCount)
// {
// 	//DOS / Windows 系统下,基本分区必须以柱面为单位划分
// 	LONG_INT	low		= this->mPartableSecCnt;
// 	char		buf[SECTOR_SIZE] = {0};
// 	BOOL		bres	= FALSE;
// 	HANDLE		hDisk	= NULL;
// 	DWORD		dwRead	= 0;
// 	PDPart		pn		= NULL;
// 	LONG_INT	offert	= {0};
// 
// 	//如果已经打开了一个磁盘就不能在打开了
// 	if(!strlen(mDevName))	return FALSE;//没有初始化过
// 
// 
// 	//检查是否已经查找了不可分配的空间 
// 	if (mIsGetUnPartSec) {//已经获得了不可分配的空间大小
// 
// 		if (secCount)//需要返回数据
// 			*secCount = this->mUnPartSecCnt;
// 		return TRUE;
// 	}
// 
// 
// 	//打开一个新的句柄，如果不这样的就算降磁盘函数方在一个独立的
// 	//线程里面的话也会阻塞程序
// 
// 	hDisk =   CreateFile(mDevName ,	//要打开的设备 
// 		GENERIC_READ | GENERIC_WRITE,		 //访问模式
// 		FILE_SHARE_READ | FILE_SHARE_WRITE,  //共享模式
// 		NULL , OPEN_EXISTING, 0 , NULL);								 //不复制任何文件属性
// 	if(hDisk == INVALID_HANDLE_VALUE)
// 		return FALSE;						 //打开设备失败
// 	
// 	
// 	//              穷举
// 	
// 	//计算字节偏移
// 	offert.QuadPart = low.QuadPart * SECTOR_SIZE; //字节偏移
// 	//移动读写指针
// 	::SetFilePointer(hDisk , offert.LowPart ,  &(offert.HighPart ), FILE_BEGIN);
// 
// 	do{
// 		bres = ::ReadFile(hDisk , buf, SECTOR_SIZE , &dwRead , NULL);
// 		//bres = this->ReadSecter(/*hDisk ,*/low ,buf , &dwRead , SECTOR_SIZE);
// 	}while(bres && dwRead == SECTOR_SIZE && ++low.QuadPart);
// 	//不需要了时候即使关闭句柄
// 	CloseHandle(hDisk);
// 	
// 	//不可分区的山区总数
// 	this->mUnPartSecCnt= DWORD(low.QuadPart - this->mPartableSecCnt.QuadPart);
// 	
// 	if (secCount)//需要返回数据
// 		*secCount = mUnPartSecCnt;
// 	
// 	pn = (PDPart)NewPart(NULL , &mPartableSecCnt  , PART_UNPARTBLE );
// 	if(pn){
// 		pn->mSecCount.QuadPart = mUnPartSecCnt;				
// 		PDList(this->mPPartList)->AddPart( pn);
// 		mIsGetUnPartSec = TRUE;
// 	}else{
// 		return FALSE;
// 	}
// 
// 	return TRUE;
// 	//计算磁盘的不可分区空间大小
// // 	DWORD dwUnPartCnt = 0;
// // 	if(ComputeUnPartSecCont(&dwUnPartCnt))
// // 	{
// // 
// // 	}
// 
// 
// //				穷举
// 
// /*				折半查找
// 
// #define COM_TYPE 1	//1 是以读取数据计算  SECTOR_SIZE 以设置指针计算
// //经测试 设置以设置指针计算是不可行的，不管怎么样设置指针都会正确 结果返回一个柱面的大小
// //但是读取山区我的方式是可行的 至少和WinHex的结果一致
// 
// 	LONG_INT high = mSecCount;
// 	high.QuadPart +=(mGeometry.SectorsPerTrack * mGeometry.TracksPerCylinder -1);
// 	LONG_INT mid;
// 	DWORD res;
// 	while(high >= low){
// 		if(high == low){					//找到最后一个扇区了
// 			CloseHandle(hDisk);
// 			*secCount = DWORD(low.QuadPart - mSecCount.QuadPart);
// 			return TRUE;
// 		}
// 		mid.QuadPart =( high + low).QuadPart / 2u;
// #if COM_TYPE == SECTOR_SIZE   //以设置指针计算
// 		mid.QuadPart *= SECTOR_SIZE;
// 		mid.QuadPart += (SECTOR_SIZE -1);
// 		res  = SetFilePointer(hDisk , mid.LowPart ,&mid.HighPart ,FILE_BEGIN);
// #elif COM_TYPE == 1 //是以读取数据计算
// 		bres = this->ReadSecter(hDisk ,mid ,buf , &dwRead , SECTOR_SIZE);
// #endif
// 		if (
// #if COM_TYPE == SECTOR_SIZE //以设置指针计算
// 			GetLastError()!=NO_ERROR && res == INVALID_SET_FILE_POINTER
// #elif COM_TYPE == 1 //是以读取数据计算
// 			!bres && dwRead != SECTOR_SIZE
// #endif
// 			){
// 			high.QuadPart=mid.QuadPart / COM_TYPE -1;
// 		}else
// 			low.QuadPart=mid.QuadPart / COM_TYPE + 1;
// 	}
// 				折半查找*/
// 
// // 	//操作失败
// // 	CloseHandle(hDisk);
// // 	return FALSE;   //折半是此处返回FAlSE  
// }

LONG_INT Disk::GetPartableSecCount()
{
	LONG_INT li = {0};
	if (0 == strlen(this->mDevName))
		li.QuadPart = -1;
	li.QuadPart = this->mPartableSecCnt.QuadPart;
	return li;
}
LONG_INT Disk::GetSecCount()
{
	LONG_INT li = {0};
	if (0 == strlen(this->mDevName))
		li.QuadPart = -1;
	li.QuadPart = this->mPartableSecCnt.QuadPart + this->mUnPartSize / SECTOR_SIZE;
	return li;
}
// DWORD Disk::GetUnPartableSecCount()
// {
// 	if (0 == strlen(this->mDevName)) return 0;
// 	return this->mUnPartSecCnt;
// }
