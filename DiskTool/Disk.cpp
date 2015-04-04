//////////////////////////////////////////////////////////////////////////
// Module:		Disk.cpp
// Author:		����
// Created:		2012��3��20�� ���ڶ�
// Purpose:		ʵ��Disk�����ķ���
// Comment:		�������Ҫ�ǰ�����һ��������̵Ĳ�������
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
	HANDLE   hDrive = NULL;             //�����豸
	int      nCount = 0;                //�豸����
	char     czhDriveName[30] = {0};    //�豸����
	int		 i		=	 0;

	//���ϰ�˵windows�����Թ���0x80���������
	//��������һ�������Դ�����п���������0x40Ϊ���޵�
	for (i = 0 ; i < MAX_DISK_COUNT ; ++i)
	{
		sprintf(czhDriveName ,"%s%d",DISK_PRE_NAME , i);
		hDrive = CreateFileA(
			czhDriveName,						//Ҫ�򿪵��豸 
			0,									//����ģʽ
			FILE_SHARE_READ | FILE_SHARE_WRITE,	//����ģʽ
			NULL,								//Ĭ�ϵİ�ȫ����
			OPEN_EXISTING,						//�򿪷�ʽ
			0,									//Ĭ�ϵ��ļ�������
			NULL);								//�������κ��ļ�����
		if(hDrive == INVALID_HANDLE_VALUE)		//û����   
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
	//�����Ч�����ǲ�
	if(index < 0)	return FALSE;

	char name[30] = {0};
	::sprintf(name ,"%s%d" ,DISK_PRE_NAME, index);
	
	return OpenDisk(name);
}

BOOL Disk::OpenDisk(char* czName)
{
	DWORD		  dwOutBytes = 0; 
	DISK_GEOMETRY mGeometry  = {0};
	BOOL		  res		 = FALSE;  //�����ṹ

	//����Ѿ�����һ�����̾Ͳ����ڴ���
	if (czName == NULL || strlen(mDevName)) return FALSE;

	this->mDisk =   CreateFile( czName , GENERIC_READ | GENERIC_WRITE ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,  //����ģʽ
		NULL , OPEN_EXISTING , 0 , NULL);

	if(mDisk == INVALID_HANDLE_VALUE)  
		res = FALSE;                        //�򿪴���ʧ��
	else{
	
		//������̵Ŀɷ����������� 
		res = ::DeviceIoControl(mDisk,		// �豸���
			IOCTL_DISK_GET_DRIVE_GEOMETRY,	// ȡ���̲���
			NULL , 0 , &mGeometry,
			sizeof(DISK_GEOMETRY),			// ������ݻ�����
			&dwOutBytes , (LPOVERLAPPED)NULL);			// ��ͬ��I/O

		if(!res)  //��ȡ���̵���Ϣʧ��
		{
			::CloseHandle(mDisk);//�رմ򿪵Ĵ����豸
			mDisk = INVALID_HANDLE_VALUE;
			return res;
		}
		//������̵Ŀɷ�����������
		this->mPartableSecCnt.QuadPart = mGeometry.Cylinders.QuadPart \
			* mGeometry.TracksPerCylinder *  mGeometry.SectorsPerTrack;     

		strcpy(mDevName , czName);//�ȱ���һ������
		mSecPerTrack = mGeometry.SectorsPerTrack;
		mTracksPerCylinder = mGeometry.TracksPerCylinder;
		mCylinders.QuadPart = mGeometry.Cylinders.QuadPart;
		
		//��ȡ���̵ķ����б�
		if(LoadPartList(/*hDisk*/)){
			//����
			PDList(this->mPPartList)->SortList(ComparePart);
			//��Ӹ�����϶�ڵ�
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
	
	//�豸û�д�
	if (0 == strlen(this->mDevName)) return DR_INIT_ERR;

	if (off.QuadPart >= this->mPartableSecCnt.QuadPart + this->mUnPartSize / SECTOR_SIZE)
	{//ָ��Խ��
		return DR_DEV_CTRL_ERR;
	}
	
	offset.QuadPart = off.QuadPart;						//��ȡ���ݵ�ʵ��ƫ��
	offset.QuadPart *= SECTOR_SIZE;					//�ֽ�ƫ��
	
	//�����ļ�ָ��
	offset.LowPart = SetFilePointer(mDisk , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
	if (offset.LowPart == -1 && GetLastError() != NO_ERROR )
		res = DR_DEV_CTRL_ERR;
	
	//��ȡ����
	if(!res && !::ReadFile(mDisk , buf ,
		dwRead ,&dwReaded ,NULL) && dwReaded != dwRead)	
		res =  DR_DEV_IO_ERR;
	
	/*	CloseHandle(hDev);								//�ر��Ѿ��򿪵��豸*/
	return res;
}

int Disk::GetBytePerSec()
{
	return 512;
}


int Disk::GetSectorPerTrack()
{
	//�豸û�д�
	if (0 == strlen(this->mDevName)) return -1;

	return  mSecPerTrack;
}

DWORD Disk::GetTracksPerCylinder()
{
	//�豸û�д�
	if (0 == strlen(this->mDevName)) return 0;

	return  mTracksPerCylinder;
}

LONG_INT Disk::GetCylinders()
{
	LONG_INT res;
	res.QuadPart = -1;
	//�豸û�д�
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

	//Ҫɾ��ÿһ���ڵ�
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
	//����һ������
	mDevName[0] = 0;

	//�ͷŷ�������
	if (mPPartList)
	{
		clearDList(mPPartList);
		delete mPPartList;
		mPPartList = NULL;
	}

	//�ر��Ѿ��򿪵��豸
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
	//ʵ�ʵ�EBR(MBR)  �������ƫ��
	LONG_INT o    = {0};
	PVOID	 pn	  = NULL;
	DPT		 dpt  = *PDPT(dp);
	PDList   list = PDList(this->mPPartList);

	//�����ǰ���ڷ�����һ��DPT���еı�����
	//��ôEBR ���� ����������ƫ�ƾ���DPT�ı�����
	//������Ǵ����еĵ�һ��DPT����  
	//��ôEBR��ƫ�ƾ��ǵ�ǰDPT�е����ƫ�Ƽ�����չ������ƫ��
	//��������ƫ��������Ե�ǰDPT��
	//��չ������ƫ�ƾ��� ��һ��DPT���е���չ������������ƫ��
	if(isFirstDPT){
		o.QuadPart = dpt.mRelativeSectors;
		if(IsExtPart(&dpt))//��һ���������е���չ�����������̵���չ����
			this->mExtPos.QuadPart = dpt.mRelativeSectors;
	}else{
		if(IsExtPart(&dpt))
			o.QuadPart = this->mExtPos.QuadPart + dpt.mRelativeSectors;
		else
			o.QuadPart = dptoff.QuadPart + dpt.mRelativeSectors;
	}

	if (IsExtPart(&dpt))               //�ж�Ҫ�����ķ����Ƿ�����չ����
	{//��ǰҪ�оٵ�
		int      i   = 0;
		EBR      ebr = {0};
		DWORD    dwReaded = 0;
		if(!ReadSecter(/*mDisk ,*/ o , &ebr ,&dwReaded))              //��ȡ��չ��չ������EBR
			return FALSE;                      //��ȡEBRʧ��
		if(ebr.mEndSig != MBR_END || dwReaded != SECTOR_SIZE)
			return FALSE;
		
		//�½�һ����Ӧ�Ľڵ���ӵ�����ȥ
		pn = NewPart(NULL , &o  ,PART_EBR );
		if(NULL != pn)
			list->AddPart(PVOID(pn)); //���ڵ���ӵ�������ȥ
		

		//��EBR�� ֻ������DPT��Ч ��
		//��һ��DPT ָ��ǰ�߼�������
		//�ڶ���DPT ָ����һ���߼�������EBR
		for(i = 0 ; i < 4 ; ++i)               //����ÿһ��EBR�е�DPT����
		{ 
			if(ebr.mDPT[i].mSectorCount == 0)
				continue;
			ListPartion(/*mDisk , */&(ebr.mDPT[i]) ,o  , FALSE , pLogicDir);
		}
	}
	else
	{
		//�����߼�����	
		//�½�һ����Ӧ�Ľڵ���ӵ�����ȥ
		pn = NewPart(&dpt , &o  , 0  , isFirstDPT , pLogicDir);
		if(NULL != pn) {
			list->AddPart(PVOID(pn)); //���ڵ���ӵ�������ȥ
			if (isFirstDPT) ++mMainVolCount;  //��������
		}
	}
	return TRUE;
}

BOOL Disk::LoadPartList(/*HANDLE hDisk*/)
{
	//�Ѿ��������б�
	if(this->mPPartList)
		return FALSE;

	//��������
	this->mPPartList = new DList();

	//��ʼ���� �����������   ������������
	LONG_INT	offert  = {0};
	MBR			mbr		= {0};      //���̵�MBR
	int			i	    = 0 ;
	DWORD		dwRead	= 0;
	PDList		list	= PDList(this->mPPartList);
	PVOID		pn		= NULL;
	DWORD*		pLogicDir = NULL;		//�߼�������Ϣ
	BOOL		res		= TRUE;		//һЩ������� 

	if (!ReadSecter(/*hDisk, */offert ,&mbr , &dwRead))          //��ȡ���̵�MBR
		return FALSE;                              //��ȡMBRʧ��

	//���������
	if(mbr.mEndSig != MBR_END || dwRead != SECTOR_SIZE)
		return FALSE;

	//����߼�������Ϣ
	pLogicDir = GetLogicalDrives();

	//��Ҫ�ж��Ƿ���DBR����MBR  ������ʱĬ��ΪMBR


	
	//�½�һ����Ӧ�Ľڵ���ӵ�����ȥ
	if (pn = NewPart(NULL, &offert, PART_MBR))
		list->AddPart(pn); //���ڵ���ӵ�������ȥ
	
	res = TRUE;
	for (i = 0 ; res == TRUE && i < 4 ; ++i)              //ÿ�����������ֻ��������¼
	{
		if (!mbr.mDPT[i].mSectorCount)
			continue;                      //��ǰ��¼�ķ�������Ϊ0

		res = ListPartion(&(mbr.mDPT[i]) , offert ,TRUE , pLogicDir);
	}

	if (res == FALSE)  //����MBR�еķ�����ʧ�ܣ���
	{
		res = TRUE;
		//��ʼ�����ڶ����������еĿ��ܴ��ڵķ�����Ϣ
		char		extBuf[SECTOR_SIZE] = {0};	//һ�������Ļ���ռ�
		PExtDPT		pExtDPT = NULL;//��չ���������ʼ��ַ
		LONG_INT	liSecOff;
		int			i = 0;
		int			flag	=	0;	//0��ʾ��һ������ 
		
		liSecOff.QuadPart = 1;		//��һ��Ҫ��ȡ����һ������
	
		if (!ReadSecter(liSecOff , extBuf , &dwRead , SECTOR_SIZE))
		{//��ȡ����ʧ��
			res = FALSE;
		}

		if ((res != FALSE) && (PLONG_INT(PExtDPT(extBuf)->partName)->QuadPart != EXT_DPT_FLAG))
		{//����һ����չ������
			res = FALSE;
		}

		if (TRUE == res )
		{//��һ����Ч����չ������
			//��Ҫɾ��ԭ���Ѿ������õ�����
			//�ͷŷ�������
			if(NULL != list){
				clearDList(list);
				delete list;
			}

			//���·�������ռ�
			list = new DList();
			this->mPPartList = list;

			//�о�MBR���Ǳ����е�
			offert.QuadPart = 0 ;
			pn = NewPart(NULL , &offert  , PART_MBR );
			if(NULL != pn)
				list->AddPart(pn); //���ڵ���ӵ�������ȥ
		}

		while (TRUE == res)
		{
			for(i = 0 ; i < 16 ; ++i)
			{//����ÿһ������
				pExtDPT = ((PExtDPT)extBuf) + i;
				if (IsExtPDPTEmpty(pExtDPT))
				{//����һ���ձ���
					break;
				}
				
				if (0 != PLONG_INT(pExtDPT->partName)->QuadPart)
				{//��һ����Ч�ı���
					offert.QuadPart = pExtDPT->startSector;
					pn = NewPart(NULL , &offert  , pExtDPT->partFormate  , FALSE , pLogicDir);
					if(NULL != pn) {
						PDPart(pn)->mSecCount.QuadPart = (flag == 0) ? (pExtDPT->sectorCount - 0xEC000000) : pExtDPT->sectorCount;
						list->AddPart(PVOID(pn)); //���ڵ���ӵ�������ȥ
					}
					flag = 1;
				}
			}

			if (16 != i)
			{//�������
				break;
			}else{
				//��Ҫ��ȡ��һ���������в���
				++liSecOff.QuadPart;
				if (!ReadSecter(liSecOff , extBuf , &dwRead , SECTOR_SIZE))
				{//��ȡ����ʧ��
					break;
				}
			}
		}
	}


	//��Ҫ�Ļ�������һ��
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
	//ָ������������
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
	//ָ������������
	if (!p)	return 0xFFFF;

	return p->mType;	
}

DWORD Disk::GetRelativeSectors(int index)
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//ָ������������
	if (!p) return 0xFFFFFFFF;

	return p->mRelativeSectors;
	//return p->GetRelativeSectors();	
}
LONG_INT Disk::GetPartSectorCount(int index)
{
	PDPart p = PDPart(PDList(this->mPPartList)->GetPart(index));
	//ָ������������
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
	//ָ������������
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
	//ָ������������
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

	//�½�һ��������ڵ�
	PDPart pn = new DPart();
	memset(pn , 0 , sizeof(DPart) );
	pn->mVolIndex = -1;

	if(NULL != dpt)
	{
		//��DPT������
		pn->mIsActivity = (dpt->mGuidFlag == 0x80);
		pn->mRelativeSectors = dpt->mRelativeSectors;
		pn->mSecCount.QuadPart = dpt->mSectorCount;
		type = dpt->mPartType;
	}
	pn->mType		= type;				//�ڵ������
	pn->mOffset		= *off;	            //�ڵ����������ƫ�Ƶ�ַ
	pn->mIsMainPart = isMainPart;		//�Ƿ�Ϊ������
	if (IsPartFS(type))
	{//��ǰ��ӵĽڵ���һ���ļ�ϵͳ
		pn->mVolIndex = ++this->mVolCnt;
		if (pLogicDri)
		{
			//��ǰ�����������豸�ϵ��ֽ�ƫ��
			offset = *off;
			offset.QuadPart *= SECTOR_SIZE;

			for(i = 0 ; i < 26 ; ++i)
			{
				if (PLOGCDRI(pLogicDri)[i].byteOffset.QuadPart == offset.QuadPart )
				{//ƥ�䵽��
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
	//�߼�������Ϣ
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
	char		outBuf[100] = {0};		//���ݲ�ѯʱ�����Buf
	VOLUME_DISK_EXTENTS* pOutBuf = (VOLUME_DISK_EXTENTS*)outBuf;

	DWORD res = 0;

	strcpy(devNmae , mDevName);

	//17 == strlen("\\\\.\\PHYSICALDRIVE")
	//20 == strlen("\\\\.\\PHYSICALDRIVE127")  ���ֻ��128(0x80)������
	if (len < 18 || len > 20) return NULL;
	
	//��ȫ��ת���´�д
	for (i = 0; i < len; ++i)
	{
		devNmae[i] = toupper(devNmae[i]);
	}

	//��õ�ǰ�豸������
	if(0 == sscanf(devNmae , "\\\\.\\PHYSICALDRIVE%d" , &index ))
		return NULL;  //��ȡ�豸����ʧ��

	//��õ�ǰϵͳ�����е��߼�������
	dwDris = ::GetLogicalDrives();
	
	if ( 0 == dwDris )
		return NULL;

	//����ռ�
	pLogic = new LOGCDRI[27];  //���26���߼��������������ʾ����Ľ���
	memset(pLogic , 0 , sizeof(LOGCDRI)*27);

	//���ֻ��26���߼�������
	for ( i = 0 ; i < 26 ; ++i)
	{
		if (0 == (dwDris & 0x00000001))
		{//��ǰû��
			dwDris >>= 1;
			continue;
		}

		csLogicN[4] = char('A' + i);
		hDev = CreateFileA( csLogicN , 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,	//����ģʽ
			NULL ,OPEN_EXISTING , 0 , NULL);								//�������κ��ļ�����
		if (hDev == INVALID_HANDLE_VALUE)
			continue;
		
		//��ȡÿһ���ڵ�ǰ�����ϵ�ÿһ�����ƫ��λ�ã��Ա��ڻ�ȡÿһ���������߼������ַ�
		if(0 == ::DeviceIoControl( hDev,
			IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
			NULL, 0 , pOutBuf ,	100,
			&bytesReturned,	NULL ))
		{//��ȡ��Ϣʧ��
			res = GetLastError();
			CloseHandle(hDev);
			continue;
		}

		if (pOutBuf->Extents[0].DiskNumber != index)
		{//���ǵ�ǰ�豸��
			CloseHandle(hDev);
			continue;
		}

		//�ҵ���һ�����ڵ�ǰ�豸�ľ�
		pLogic[nCnt].byteOffset.QuadPart = pOutBuf->Extents[0].StartingOffset.QuadPart;
		pLogic[nCnt++].letter = char('A' + i);
		CloseHandle(hDev);
	}

	return (DWORD*)pLogic;
}

BOOL Disk::MakeListContinue(/*HANDLE hDisk*/)
{
	//���ĺ����������Ӹ�����϶�ڵ�
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
		{		//�з�����С
			temp.QuadPart =t2->mOffset.QuadPart 
				- t1->mOffset.QuadPart - t1->mSecCount.QuadPart;
			if(temp.QuadPart)
			{//�п�϶
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
		else	//û�з�����С
		{
			t1->mSecCount.QuadPart = t2->mOffset.QuadPart - t1->mOffset.QuadPart;
		}
		t1 = t2;
	}

	//���һ��δ�����������С�ڵ�
	t2 = PDPart(list->GetPart(nCount-1));      //������һ���ڵ�
	offset.QuadPart = t2->mSecCount.QuadPart + t2->mOffset.QuadPart;		    //û�з�������������ƫ��
	unPartCount.QuadPart = this->mPartableSecCnt.QuadPart - offset.QuadPart;//���ɷ����������С
	if(unPartCount.QuadPart > 0 )				    //����Ĭ�ϵ�����LABģʽ
	{//������û�з���������
		if (pn = (PDPart)NewPart(NULL, &offset, PART_UN_PART))
		{
			pn->mSecCount = unPartCount;
			list->AddPart( pn);
		}
	}

	//������̵Ĳ��ɷ����ռ��С
	res = ::DeviceIoControl(mDisk , IOCTL_DISK_GET_LENGTH_INFO , NULL ,
		0 , &gli , sizeof(gli) , &dwOutBytes,   NULL);			// ��ͬ��I/O
	if(res)
	{//�ɹ���ȡ���̵Ĵ�С
		//���ɷ�����������
		mUnPartSize = (DWORD)(gli.Length.QuadPart  - mPartableSecCnt.QuadPart * SECTOR_SIZE);

		//����һ���µĽڵ�
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
// 	//DOS / Windows ϵͳ��,������������������Ϊ��λ����
// 	LONG_INT	low		= this->mPartableSecCnt;
// 	char		buf[SECTOR_SIZE] = {0};
// 	BOOL		bres	= FALSE;
// 	HANDLE		hDisk	= NULL;
// 	DWORD		dwRead	= 0;
// 	PDPart		pn		= NULL;
// 	LONG_INT	offert	= {0};
// 
// 	//����Ѿ�����һ�����̾Ͳ����ڴ���
// 	if(!strlen(mDevName))	return FALSE;//û�г�ʼ����
// 
// 
// 	//����Ƿ��Ѿ������˲��ɷ���Ŀռ� 
// 	if (mIsGetUnPartSec) {//�Ѿ�����˲��ɷ���Ŀռ��С
// 
// 		if (secCount)//��Ҫ��������
// 			*secCount = this->mUnPartSecCnt;
// 		return TRUE;
// 	}
// 
// 
// 	//��һ���µľ��������������ľ��㽵���̺�������һ��������
// 	//�߳�����Ļ�Ҳ����������
// 
// 	hDisk =   CreateFile(mDevName ,	//Ҫ�򿪵��豸 
// 		GENERIC_READ | GENERIC_WRITE,		 //����ģʽ
// 		FILE_SHARE_READ | FILE_SHARE_WRITE,  //����ģʽ
// 		NULL , OPEN_EXISTING, 0 , NULL);								 //�������κ��ļ�����
// 	if(hDisk == INVALID_HANDLE_VALUE)
// 		return FALSE;						 //���豸ʧ��
// 	
// 	
// 	//              ���
// 	
// 	//�����ֽ�ƫ��
// 	offert.QuadPart = low.QuadPart * SECTOR_SIZE; //�ֽ�ƫ��
// 	//�ƶ���дָ��
// 	::SetFilePointer(hDisk , offert.LowPart ,  &(offert.HighPart ), FILE_BEGIN);
// 
// 	do{
// 		bres = ::ReadFile(hDisk , buf, SECTOR_SIZE , &dwRead , NULL);
// 		//bres = this->ReadSecter(/*hDisk ,*/low ,buf , &dwRead , SECTOR_SIZE);
// 	}while(bres && dwRead == SECTOR_SIZE && ++low.QuadPart);
// 	//����Ҫ��ʱ��ʹ�رվ��
// 	CloseHandle(hDisk);
// 	
// 	//���ɷ�����ɽ������
// 	this->mUnPartSecCnt= DWORD(low.QuadPart - this->mPartableSecCnt.QuadPart);
// 	
// 	if (secCount)//��Ҫ��������
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
// 	//������̵Ĳ��ɷ����ռ��С
// // 	DWORD dwUnPartCnt = 0;
// // 	if(ComputeUnPartSecCont(&dwUnPartCnt))
// // 	{
// // 
// // 	}
// 
// 
// //				���
// 
// /*				�۰����
// 
// #define COM_TYPE 1	//1 ���Զ�ȡ���ݼ���  SECTOR_SIZE ������ָ�����
// //������ ����������ָ������ǲ����еģ�������ô������ָ�붼����ȷ �������һ������Ĵ�С
// //���Ƕ�ȡɽ���ҵķ�ʽ�ǿ��е� ���ٺ�WinHex�Ľ��һ��
// 
// 	LONG_INT high = mSecCount;
// 	high.QuadPart +=(mGeometry.SectorsPerTrack * mGeometry.TracksPerCylinder -1);
// 	LONG_INT mid;
// 	DWORD res;
// 	while(high >= low){
// 		if(high == low){					//�ҵ����һ��������
// 			CloseHandle(hDisk);
// 			*secCount = DWORD(low.QuadPart - mSecCount.QuadPart);
// 			return TRUE;
// 		}
// 		mid.QuadPart =( high + low).QuadPart / 2u;
// #if COM_TYPE == SECTOR_SIZE   //������ָ�����
// 		mid.QuadPart *= SECTOR_SIZE;
// 		mid.QuadPart += (SECTOR_SIZE -1);
// 		res  = SetFilePointer(hDisk , mid.LowPart ,&mid.HighPart ,FILE_BEGIN);
// #elif COM_TYPE == 1 //���Զ�ȡ���ݼ���
// 		bres = this->ReadSecter(hDisk ,mid ,buf , &dwRead , SECTOR_SIZE);
// #endif
// 		if (
// #if COM_TYPE == SECTOR_SIZE //������ָ�����
// 			GetLastError()!=NO_ERROR && res == INVALID_SET_FILE_POINTER
// #elif COM_TYPE == 1 //���Զ�ȡ���ݼ���
// 			!bres && dwRead != SECTOR_SIZE
// #endif
// 			){
// 			high.QuadPart=mid.QuadPart / COM_TYPE -1;
// 		}else
// 			low.QuadPart=mid.QuadPart / COM_TYPE + 1;
// 	}
// 				�۰����*/
// 
// // 	//����ʧ��
// // 	CloseHandle(hDisk);
// // 	return FALSE;   //�۰��Ǵ˴�����FAlSE  
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
