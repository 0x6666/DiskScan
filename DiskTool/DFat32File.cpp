//////////////////////////////////////////////////////////////////////////
// FileName:	DFat32File.cpp
// Author:		����
// Created:		2012��3��20�� ���ڶ�
// Purpose:		ʵ�����������ĺ���DFat32File
// Comment:		����û�б�Ҫ������Լ�����ļ��ģ����Ǻ����г���Ľṹ�ĵ���
//				������������������
//////////////////////////////////////////////////////////////////////////

#include "disktool.h"
#include "stut_connf.h"
#include "time.h"

DFat32File::DFat32File()
: mAttr(0)
, mStatus(0)
, mStartClust(0)
, mFileSize(0)
, mPointer(0)
, mIndex(0) 
, mFS(0)
, mCrtDate(0)
, mCrtTime(0)
, mLstAccDate(0)
, mWrtDate(0)
, mWrtTime(0)
{
}


DFat32File::~DFat32File()
{

}

const WCHAR* DFat32File::GetFileName()
{
	size_t len = 0;


	if (this->mFS == NULL) return NULL;	//��ǰ�ļ�������û�д�
	len = mPath.length();//�ļ�·���ĳ���
	
	if (!len)
		return NULL;

	//���Ǹ�Ŀ¼
	if (len == 1)
		return mPath.c_str();

	//�����һ��,·���ָ��
	const WCHAR* p = this->mPath.c_str() + len - 1;
	for (; !IsPathSeparator(*p); --p);

	return ++p;

}
BOOL DFat32File::IsDir()
{
	//������Զ���
	return (this->mAttr & ATTR_DIRECTORY);
}

BOOL DFat32File::IsEOF()
{
	return (this->mStatus & F_ST_EOF);
}

BOOL DFat32File::IsValid()
{
	return (this->mFS != NULL);	
}

void DFat32File::Close()
{
	mPath.clear();
	mFS = NULL;
}

DRES DFat32File::ReadFile( char* buf, DWORD* dwReaded, DWORD dwToRead /*= SECTOR_SIZE*/ )
{
	DRES  res		= DR_OK;
	DWORD pSec		= 0;			//����ָ��
	DWORD pClust	= 0;			//��ָ��
	DWORD pSSec     = 0;			//���ε���ʵɽ����
	BYTE  tBuf[SECTOR_SIZE] = {0};	//��ʱ���ݻ���
	DWORD pBuf		= 0;			//buf��дָ��
	DWORD ptBuf		= 0;			//tBuf�Ķ�ָ��
	DWORD rLen		= 0;			//ʵ����Ҫ��buf����д�����ݴ�С
	int	  i			= 0;


	//�ļ�������û�д�
	if (!mFS) return DR_NO_OPEN;

	if(!mFS || !buf || ! dwReaded )
		return DR_INVALED_PARAM;		//��������
	
	*dwReaded = 0;						//������һ��
	
	if(this->mAttr & ATTR_DIRECTORY)		//�����ļ�
		return DR_IS_DIR;
	
	if(this->mStatus & F_ST_EOF)			//�Ѿ����ļ�β��			
		return DR_OK;
	
	
	if(dwToRead > this->mFileSize - this->mPointer)//Ҫ��ȡ�����ݳ����Ƿ����ʣ�µ�����
		dwToRead = this->mFileSize - this->mPointer;//�ض�Ҫ��ȡ������
	
	//�ļ��ڵĴغ�
	pClust = this->mPointer / (SECTOR_SIZE * this->mFS->mSecPerClus); 
	//���㵱ǰ�Ĵغ�
	for (i = pClust ,pClust = this->mStartClust; i != 0 ; --i )
	{
		pClust = mFS->GetFATFromFAT1(pClust);
		if(pClust == 1 || pClust >= mFS->mMaxClust)			//��Ч�غ�
			return DR_INIT_ERR;
		if(pClust == 0xFFFFFFFF)//IO����
			return DR_DEV_IO_ERR;
	}	
	
	//���ε���ʼ������
	pSSec = mFS->ClustToSect(pClust);
	//ʵ�ʵ�ɽ����
	pSec = pSSec + \
		(this->mPointer / SECTOR_SIZE) % mFS->mSecPerClus;
	
	ptBuf = this->mPointer % SECTOR_SIZE;
	while (dwToRead > 0)
	{
		if((pSec != pSSec)&&!(pSec % mFS->mSecPerClus))
		{//������һ��
			//ʵ�ʵĴغ�
			pClust = mFS->GetFATFromFAT1(pClust); 
// 			if (IsFATEnd(pClust))
// 			{//������
// 				this->mStatus |= F_ST_EOF;
// 				return DR_OK;
// 			}
			if(pClust == 1 || pClust >= mFS->mMaxClust)			//��Ч�غ�
				return DR_INIT_ERR;
			if(pClust == 0xFFFFFFFF)//IO����
				return DR_DEV_IO_ERR;
			
			//ʵ�ʵ�������
			pSec = mFS->ClustToSect(pClust) + \
				(this->mPointer / SECTOR_SIZE) % mFS->mSecPerClus;
		}
		
		res = mFS->ReadData(tBuf , pSec);//��ȡһ������������
		if(res) return res ;		//��ȡ���ݴ���
		
		//��Ҫ�������Ƿ��Ѿ���ȡ����
		if(dwToRead < DWORD(SECTOR_SIZE -  ptBuf))
			rLen = USHORT(dwToRead);
		else
			rLen = SECTOR_SIZE -  ptBuf;
		
		::memcpy(buf + pBuf , tBuf + ptBuf , rLen);
		pBuf += rLen;		  //Buf��дָ��
		*dwReaded += rLen;	  //�Ѿ���ȡ�˵�����
		ptBuf = 0;
		this->mPointer += rLen;//�ƶ��ļ�ָ��
		dwToRead -= rLen;	  //����Ҫ��ȡ������
		++pSec;				  //������һ������
	}
	//�ļ��Ѿ���ȡ����
	if(this->mPointer >= this->mFileSize)
		this->mStatus |= F_ST_EOF;
	
	return DR_OK;
}

DRES DFat32File::InitFile( PVOID entr ,const WCHAR* path ,DFat32 * fs )
{
	PDirEntry	entry = PDirEntry(entr);
	BYTE*		dir	  = NULL;
	//�������
	if (!entr || !path || ! fs) return DR_INVALED_PARAM;
	
	//��ʼ�����ļ�����
	dir = entry->mDir;
	this->mFS = fs;

	//һЩ���Ե�����
	mAttr		= 0;
	mStatus		= 0;
	mStartClust = 0 ;
	mFileSize	= 0;
	mPointer	= 0;
	mIndex		= 0;
	mCrtDate	= 0;
	mCrtTime	= 0;
	mLstAccDate = 0;
	mWrtDate	= 0;
	mWrtTime	= 0;

	mPath = path;

	if(!dir)//��Ŀ¼
	{
		this->mAttr = ATTR_VOLUME_ID|ATTR_DIRECTORY;
		this->mStartClust = this->mFS->m1stDirClut;
		this->mFileSize = 0;
	}
	else
	{
		this->mAttr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;	//�ļ�����
		this->mStartClust = (PSDE(dir)->mFstClusHI << 16)|PSDE(dir)->mFstClusLO;//�غ�
		this->mFileSize = PSDE(dir)->mFileSize;//�ļ��Ĵ�С

		//ʱ��
		mCrtDate	= PSDE(dir)->mCrtDate;
		mCrtTime	= PSDE(dir)->mCrtTime;
		mLstAccDate = PSDE(dir)->mLstAccDate;
		mWrtDate	= PSDE(dir)->mWrtDate;
		mWrtTime	= PSDE(dir)->mWrtTime;
	}

	return DR_OK;
}


DWORD DFat32File::GetStartSec()
{
	return this->mFS != NULL ? this->mFS->ClustToSect(this->mStartClust) : 0 ;
}


DWORD DFat32File::GetStartClust()
{
	return this->mFS != NULL ? this->mStartClust : 0 ;
}

DWORD DFat32File::GetSecCount()
{
	BYTE  SecPerClust = 0; //ÿ�ص�������
	DWORD ClustSize = 0;  //�صĴ�С(�ֽ�)

	if (this->mFS == NULL || this->mFileSize == 0 ) return 0;

	SecPerClust = this->mFS->GetSecPerClust();
	ClustSize = SecPerClust * SECTOR_SIZE;

	return ((mFileSize + ClustSize -1) / ClustSize ) * SecPerClust;
}

DWORD DFat32File::GetFileSize()
{
	return this->mFS != NULL ? this->mFileSize : 0 ;
}

LONG_INT DFat32File::GetCreateTime(void)
{
	struct tm cTm	  = {0};
	LONG_INT  crtTime = {0};

	cTm = mktime(this->mCrtDate , this->mCrtTime);
	
	//ʱ��ת��
	crtTime.QuadPart = mktime(&cTm);
	
	return crtTime;
}

LONG_INT DFat32File::GetWriteTime(void)
{
	struct tm cTm	  = {0};
	LONG_INT  crtTime = {0};
	
	//������ʱ��ֵ
	cTm = mktime(this->mWrtDate , this->mWrtTime);

	//ʱ��ת��
	crtTime.QuadPart = mktime(&cTm);
	
	return crtTime;
}
LONG_INT DFat32File::GetAccessTime(void)
{
	struct tm cTm	  = {0};
	LONG_INT  crtTime = {0};
	
	cTm = mktime(this->mLstAccDate , 0);
	
	//ʱ��ת��
	crtTime.QuadPart = mktime(&cTm);
	
	return crtTime;
}

BYTE DFat32File::GetFileAttr()
{
	return mAttr;
}

DWORD DFat32File::GetParentIndex()
{
	if (this->mFS == NULL)
		return 0xFFFFFFFF;

	return mIndex;
}
