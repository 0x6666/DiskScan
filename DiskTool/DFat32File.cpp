//////////////////////////////////////////////////////////////////////////
// FileName:	DFat32File.cpp
// Author:		杨松
// Created:		2012年3月20日 星期二
// Purpose:		实现类所申明的函数DFat32File
// Comment:		本来没有必要这个类以及这个文件的，但是后来有程序的结构的调整
//				决定还是添加了这个类
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


	if (this->mFS == NULL) return NULL;	//当前文件根本就没有打开
	len = mPath.length();//文件路径的长度
	
	if (!len)
		return NULL;

	//就是跟目录
	if (len == 1)
		return mPath.c_str();

	//找最后一个,路径分割符
	const WCHAR* p = this->mPath.c_str() + len - 1;
	for (; !IsPathSeparator(*p); --p);

	return ++p;

}
BOOL DFat32File::IsDir()
{
	//检查属性对象
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
	DWORD pSec		= 0;			//扇区指针
	DWORD pClust	= 0;			//簇指针
	DWORD pSSec     = 0;			//本次的其实山区号
	BYTE  tBuf[SECTOR_SIZE] = {0};	//临时数据缓存
	DWORD pBuf		= 0;			//buf的写指针
	DWORD ptBuf		= 0;			//tBuf的读指针
	DWORD rLen		= 0;			//实际需要往buf里面写的数据大小
	int	  i			= 0;


	//文件根本就没有打开
	if (!mFS) return DR_NO_OPEN;

	if(!mFS || !buf || ! dwReaded )
		return DR_INVALED_PARAM;		//参数错误
	
	*dwReaded = 0;						//先清理一下
	
	if(this->mAttr & ATTR_DIRECTORY)		//不是文件
		return DR_IS_DIR;
	
	if(this->mStatus & F_ST_EOF)			//已经是文件尾了			
		return DR_OK;
	
	
	if(dwToRead > this->mFileSize - this->mPointer)//要读取的数据长度是否大于剩下的数据
		dwToRead = this->mFileSize - this->mPointer;//截断要读取的数据
	
	//文件内的簇号
	pClust = this->mPointer / (SECTOR_SIZE * this->mFS->mSecPerClus); 
	//计算当前的簇号
	for (i = pClust ,pClust = this->mStartClust; i != 0 ; --i )
	{
		pClust = mFS->GetFATFromFAT1(pClust);
		if(pClust == 1 || pClust >= mFS->mMaxClust)			//无效簇号
			return DR_INIT_ERR;
		if(pClust == 0xFFFFFFFF)//IO错误
			return DR_DEV_IO_ERR;
	}	
	
	//本次的起始扇区号
	pSSec = mFS->ClustToSect(pClust);
	//实际的山区号
	pSec = pSSec + \
		(this->mPointer / SECTOR_SIZE) % mFS->mSecPerClus;
	
	ptBuf = this->mPointer % SECTOR_SIZE;
	while (dwToRead > 0)
	{
		if((pSec != pSSec)&&!(pSec % mFS->mSecPerClus))
		{//到了下一簇
			//实际的簇号
			pClust = mFS->GetFATFromFAT1(pClust); 
// 			if (IsFATEnd(pClust))
// 			{//结束了
// 				this->mStatus |= F_ST_EOF;
// 				return DR_OK;
// 			}
			if(pClust == 1 || pClust >= mFS->mMaxClust)			//无效簇号
				return DR_INIT_ERR;
			if(pClust == 0xFFFFFFFF)//IO错误
				return DR_DEV_IO_ERR;
			
			//实际的扇区号
			pSec = mFS->ClustToSect(pClust) + \
				(this->mPointer / SECTOR_SIZE) % mFS->mSecPerClus;
		}
		
		res = mFS->ReadData(tBuf , pSec);//读取一个扇区的数据
		if(res) return res ;		//读取数据错误
		
		//需要的数据是否已经读取完了
		if(dwToRead < DWORD(SECTOR_SIZE -  ptBuf))
			rLen = USHORT(dwToRead);
		else
			rLen = SECTOR_SIZE -  ptBuf;
		
		::memcpy(buf + pBuf , tBuf + ptBuf , rLen);
		pBuf += rLen;		  //Buf的写指针
		*dwReaded += rLen;	  //已经读取了的数据
		ptBuf = 0;
		this->mPointer += rLen;//移动文件指针
		dwToRead -= rLen;	  //还需要读取的数据
		++pSec;				  //到了下一个扇区
	}
	//文件已经读取完了
	if(this->mPointer >= this->mFileSize)
		this->mStatus |= F_ST_EOF;
	
	return DR_OK;
}

DRES DFat32File::InitFile( PVOID entr ,const WCHAR* path ,DFat32 * fs )
{
	PDirEntry	entry = PDirEntry(entr);
	BYTE*		dir	  = NULL;
	//参数检查
	if (!entr || !path || ! fs) return DR_INVALED_PARAM;
	
	//开始创建文件对象
	dir = entry->mDir;
	this->mFS = fs;

	//一些属性的清理
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

	if(!dir)//跟目录
	{
		this->mAttr = ATTR_VOLUME_ID|ATTR_DIRECTORY;
		this->mStartClust = this->mFS->m1stDirClut;
		this->mFileSize = 0;
	}
	else
	{
		this->mAttr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;	//文件属性
		this->mStartClust = (PSDE(dir)->mFstClusHI << 16)|PSDE(dir)->mFstClusLO;//簇号
		this->mFileSize = PSDE(dir)->mFileSize;//文件的大小

		//时间
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
	BYTE  SecPerClust = 0; //每簇的扇区数
	DWORD ClustSize = 0;  //簇的大小(字节)

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
	
	//时间转化
	crtTime.QuadPart = mktime(&cTm);
	
	return crtTime;
}

LONG_INT DFat32File::GetWriteTime(void)
{
	struct tm cTm	  = {0};
	LONG_INT  crtTime = {0};
	
	//获得相关时间值
	cTm = mktime(this->mWrtDate , this->mWrtTime);

	//时间转化
	crtTime.QuadPart = mktime(&cTm);
	
	return crtTime;
}
LONG_INT DFat32File::GetAccessTime(void)
{
	struct tm cTm	  = {0};
	LONG_INT  crtTime = {0};
	
	cTm = mktime(this->mLstAccDate , 0);
	
	//时间转化
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
