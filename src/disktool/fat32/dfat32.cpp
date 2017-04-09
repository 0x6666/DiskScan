/***********************************************************************
 * FileName:	DFAT32.cpp
 * Author:		杨松
 * Created:		2012年3月20日 星期二
 * Purpose:		DFat32的实现
 * Comment:		这个类主要是包含了一些FAT32的操方法解，用于抽象一个FAT32的卷
 * 
 * Modified:	2012年5月13日 星期日		修复一个BUG。
 *				1.解决碰到短文件名刚好是8.3格式时而无法识别的问题。GetSegName
 *				中，文件名与扩展名分隔符"."的索引最大可以使8。
 *
 *				2.windows资源管理器不支持创建以"."开头的文件，但是实际上
 *				在windows中是存在这样的文件的,所以现在将原先不支持以点开
 *				头的文件，改为了支持"."开头的文件
 ***********************************************************************/
#include "stdafx.h"

DFat32::DFat32(void)
{
	mDev				= INVALID_HANDLE_VALUE;
	mDevName[0]		= 0;
	mFSOff.QuadPart	= 0;
/*	mIsInit			= FALSE;*/
	mSecPerClus		= 0;
	mResSec			= 0;
	mFATs				= 0;
	mSecsPerFAT		= 0;
	mSectors			= 0;
	m1stDirClut		= 0;
	mFSinfoSec		= 0;
	mIsViewChged		= 0;
	mViewSec			= 0;
	mMaxClust			= 0;
	memset(mView , 0 , SECTOR_SIZE);
}

DFat32::~DFat32(void)
{
}

DRES DFat32::OpenDev(const WCHAR* name, LONG_INT offset)
{
	if (!wcslen(name))
		return DR_INVALED_PARAM;

	if (wcslen(mDevName))
		return DR_ALREADY_OPENDED;

	wcscpy(mDevName, name);
	mFSOff = offset;

	//打开设备
	mDev = ::CreateFile(mDevName,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);

	if (mDev == INVALID_HANDLE_VALUE)
	{//打开设备失败
		wcscpy(mDevName, L"");
		return DR_OPEN_DEV_ERR;
	}

	char		buf[SECTOR_SIZE] = { 0 };			//数据缓存
	PFAT32_DBR	pDBR = PFAT32_DBR(buf);

	mSectors = 1;  //为了读取第一个扇区暂时设置为1
	DRES res = ReadData(buf, 0, SECTOR_SIZE);	//读取分区的第一个扇区的数据
	mSectors = 0;  //还原数据
	if(res)
		return res;

	if (MBR_END != pDBR->bsEndSig)//数据无效
		return DR_INIT_ERR;

	mSecPerClus = pDBR->bsSecPerClus;	//每簇扇区数
	m1stDirClut = pDBR->bsFirstDirEntry32;//第一个跟目录所在的簇号
	mFATs = pDBR->bsFATs;		//fat表数
	mFSinfoSec = pDBR->bsFsInfo32;	//FSinfo所在的扇区
	mResSec = pDBR->bsResSectors; //保留扇区数	
	mSecsPerFAT = pDBR->bsFATsecs32;	//每个FAT表所占的扇区数
	mSectors = pDBR->bsHugeSectors;//本分区的扇数
	mMaxClust = (mSectors - (mResSec + (mFATs * mSecsPerFAT))) / mSecPerClus + 2 - 1;

	//设备打开失败
	if(res)
		wcscpy(mDevName, L"");

	return res;
}

void DFat32::CloseDev()
{
	mDevName[0] = 0;
	CloseHandle(mDev);
	mDev = INVALID_HANDLE_VALUE;
	mFSOff.QuadPart = 0;
	mSecPerClus = 0;
	mResSec = 0;
	mFATs = 0;
	mSecsPerFAT = 0;
	mSectors = 0;
	m1stDirClut = 0;
	mFSinfoSec = 0;
	mIsViewChged = 0;
	mViewSec = 0;
	mMaxClust = 0;
	memset(mView, 0, SECTOR_SIZE);
}

DRES DFat32::ReadData(void* buf , DWORD off , DWORD dwReadCnt)
{
	if (!IsDevOpened())
		return DR_NO_OPEN;

	//需要判断读取数据时是否越界
	if (off  >= mSectors )
	{//读取数据越界了
		return DR_DEV_CTRL_ERR;
	}

	LONG_INT offset;
	offset.QuadPart = mFSOff.QuadPart + off;						//读取数据的实际偏移
	offset.QuadPart *= SECTOR_SIZE;					//字节偏移

	DRES res = DR_OK;
	//设置文件指针
	offset.LowPart = SetFilePointer(mDev , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
	if (offset.LowPart == -1 && GetLastError() != NO_ERROR )
		res = DR_DEV_CTRL_ERR;

	//读取数据
	DWORD dwReaded = 0;
	if(!res && !::ReadFile(mDev , buf , dwReadCnt ,&dwReaded ,NULL) && dwReaded != dwReadCnt)	
		res =  DR_DEV_IO_ERR;

	return res;
}

DRES DFat32::WriteData(void* buf , DWORD off , DWORD dwWrite)
{
	LONG_INT offset = {0};
	offset.QuadPart = mFSOff.QuadPart + off; //读取数据的实际偏移
	offset.QuadPart *= SECTOR_SIZE; //字节偏移

	//设置文件指针
	DRES res = DR_OK;
	offset.LowPart = SetFilePointer(mDev , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
	if (offset.LowPart == -1 && GetLastError() != NO_ERROR )
		res = DR_DEV_CTRL_ERR;

	//读取数据
	DWORD dwWrited = 0;
	if(!res && !WriteFile(mDev , buf , dwWrite , &dwWrited ,NULL) && dwWrited != dwWrite)	
		res =  DR_DEV_IO_ERR;

	return res;
}
DWORD DFat32::GetFATFromFAT1(DWORD clust)
{
	DWORD fsect;

	if (clust < 2 || clust > mMaxClust)	//簇号检查
		return 1;

	fsect = mResSec;
	if (MoveView(fsect + (clust / (SECTOR_SIZE / 4))))//移动窗口到clust所在的扇区
		return 0xFFFFFFFF;

	return GetDWORD(mView + (clust % (SECTOR_SIZE / 4) *4)) & FAT_MASK;
}
DWORD DFat32::GetFATFromFAT2(DWORD clust)
{
	if (clust < 2 || clust > mMaxClust)	//簇号检查
		return 1;

	DWORD fsect = mResSec + mSecsPerFAT;
	if (MoveView(fsect + (clust / (SECTOR_SIZE / 4))))//移动窗口到clust所在的扇区
		return 0xFFFFFFFF;

	return GetDWORD(mView + (clust % (SECTOR_SIZE / 4) *4)) & FAT_MASK;
}

DWORD DFat32::ClustToSect(DWORD clust)
{
	if (!IsDevOpened())
		return 0;

	//无效的簇号?
	if (clust < 2 || clust > mMaxClust) 
		return 0;

	return ((clust - 2) * mSecPerClus) + mResSec + (mFATs * mSecsPerFAT);
}

DWORD DFat32::SectToClust(DWORD sector)
{
	if (!IsDevOpened())
		return 0;

	//无效扇区号
	if (sector < (mResSec + (mFATs * mSecsPerFAT)))
	{
		return 0;
	}

	//相对根目录的第一簇
	sector -= (mResSec + (mFATs * mSecsPerFAT));

	//获得实际的簇号
	return (sector/ mSecPerClus) + 2;
}

DRES DFat32::MoveView(DWORD sec)
{
	DWORD	vSect = mViewSec;
	DRES	res = DR_OK;

	if (vSect != sec) //要改变当前的数据窗口
	{
		if (mIsViewChged) //数据视图中的数据需要写会磁盘
		{
			if ((res = WriteData(mView, vSect ,SECTOR_SIZE)) != DR_OK)
				return res;

			mIsViewChged = FALSE;
			if (vSect < (mResSec + mSecsPerFAT)) //在FAT表区域
			{
				int nf;
				for (nf = mFATs; nf > 1; nf--) //将数据写到每一个FAT表中去
				{
					vSect += mSecsPerFAT;
					WriteData(mView, vSect  ,SECTOR_SIZE);
				}
			}
		}

		if (sec)
		{
			if ((res = ReadData(mView, sec, SECTOR_SIZE)) != DR_OK)
				return res;

			mViewSec = sec;
		}
	}

	return DR_OK;
}

DRES DFat32::PosEntry(PVOID entr , WORD index)
{
	PDirEntry entry = PDirEntry(entr);
	entry->mIndex = index;						//要定位的目录号
	DWORD clst = entry->mStartClust;					//起始簇号
	if (clst == 1 || clst > mMaxClust)	//检查起始簇号是否哈合法
		return DR_INIT_ERR;

	USHORT ic = SECTOR_SIZE / 32 * mSecPerClus;	//每簇的入口数
	while (index >= ic) //跟进到簇链
	{
		clst = GetFATFromFAT1( clst	); //获得下一簇的簇号
		if (clst == 0xFFFFFFFF)
			return DR_DEV_IO_ERR; //设备IO出错

		if (clst < 2 || clst > mMaxClust)//超出了范围
			return DR_INIT_ERR;

		index -= ic;
	}

	entry->mCurClust = clst; //当前簇号
	entry->mCurSect = ClustToSect( clst) + index / (SECTOR_SIZE / 32);	//扇区号
	entry->mDir = mView + (index % (SECTOR_SIZE / 32)) * 32;//定位index所指定的入口
	return DR_OK; //定位成功  只需在用的时候移动数据窗口了
}

DRES DFat32::GetSegName(const WCHAR** path, PVOID entr)
{
	PDirEntry	entry	= PDirEntry(entr);
	WCHAR*		lfn		= entry->mLFN;
	char*		sfn		= (char*)entry->mSFN;
/*	const WCHAR* p		= *path;*/
	const WCHAR* p		= NULL;
	WCHAR		w		= 0;	//Unicode字符缓存
	int			i		= 0;	//辅助变量
	int			lfni	= 0;	//lfn超尾
	int			sfni	= 0;	//sfn超尾
	int			doti	= 0;	//最后一个点的位置
	W_CHAR		w_w		= {0};		//用于字符转换的字符
	
	if (path[0][0] == '*')
	{//已经删除了的文件
		p = *path + 1;
		entry->mIsDelFile =  TRUE;
	}
	else
	{
		entry->mIsDelFile =  FALSE;
		p = *path;
	}

	entry->mStatus = 0;				//先清理一下状态
	::memset(entry->mLFN , 0 ,sizeof(WCHAR)*MAX_LFN);
	::memset(entry->mSFN , 0 ,11);

	for (i = 0; !IsPathSeparator(w = p[i]) && w  ; ++i)
	{
		//长文件名中空格和点可以在中间。但不可以在开始和末尾
		//文件名前面不可以有空格或者点
		if(!lfni && (w <= 0x20 /*|| w == '.'*/ ))	//windows资源管理器虽然不可创建以点开头的文件，但是他却是是却是存在的  2012-5-13
			continue;
		if(lfni > MAX_LFN) return DR_INVALID_NAME;	//文件名超过了255个字符
		if(IsSingleByteChar(w) && strchr("\"*:<>\?|", w))	return DR_INVALID_NAME;					//有非法字符

		lfn[lfni++] = w;
	}

	if(!w)	//到了最后一段
		entry->mStatus |= ST_LAST;
	*path = p + i +1;//返回剩下的路径

	lfn[lfni] = 0;	
	//去掉后面的空格和点
	for (; lfni > 0 &&(lfn[lfni-1]== 0x20 || lfn[lfni-1]== '.');--lfni)
		lfn[lfni-1] = 0;

	if(!lfni)//没有名字
		return DR_INVALID_NAME;

	//同时有大写和写的时候也只能用长文件名
	if(entry->mStatus & ST_UPPER && entry->mStatus & ST_LOWER)
		entry->mStatus |= ST_LFN;
	
	//长文件名创建完毕
	//开始多文件名

	//查找扩展名的位置  //第一个字符不可能是点
	for (doti = lfni-1; doti >= 0 && lfn[doti] != '.';--doti);  //windows 其实是可以存在一点开头的文件的     
	//doti==-1  没扩展名
	if (doti == -1)
	{
		doti = 0;
	}
	else if (entry->mIsDelFile == FALSE && doti >/*=*/ 8)	//超出了8.3命名法   //此处不得有等于 "."的最大位置是可以等于8的  2012-5-13
	{
		entry->mStatus |= ST_LFN;
	}
	else if (entry->mIsDelFile && doti >/*=*/ 7)
	{
		entry->mStatus |= ST_LFN;
	}

	//开始创建短文件名
	//不得小于0x20 以及"\"*+,./:;<=>\?[\\]|"
	//先来文件的名字

	sfni = 0;
	if (entry->mIsDelFile && !(entry->mStatus & ST_LFN)) //已经删除了的文件的短文件名第一个字节替换成 '*'
	{
		sfn[sfni++] = '*';
	}

	for (i = 0 ; i < lfni; ++i )
	{
		if(i == doti && doti) //该扩展名了
			break;

		w = lfn[i];
		if (IsSingleByteChar(w) && strchr(" .+,/;=[\\]", w))  //除掉长文件名中不可以的 就是" +,/;=[\\]"  有一个空格
		{//出现了非法字符  只能用长文件名
			entry->mStatus |= ST_LFN;
			continue;
		}
		
		//短文件名如果既有大写又有小写就只能用长文件名
		if(!(entry->mStatus & ST_LFN))
		{
			if (IsCharUpper((char)w))  entry->mStatus |= ST_UPPER;
			if (IsCharLower((char)w))  entry->mStatus |= ST_LOWER;
		}

		w_w.charw = WchrToUpper(w);		//需要的话先将字符转换成大小的形式
		w_w = ChrConvert(w_w , FALSE);	//在将字符转换成多字节字符
		if(w_w.char1){
			sfn[sfni++] = w_w.char1;
			if(sfni == 8) break;
		}
		sfn[sfni++] = w_w.char2;
		if(sfni == 8) break;
	}

	//填充名中的空格
	for (;sfni < 8;++sfni)	sfn[sfni] = 0x20;

	//该扩展名了
	if(doti){   //有扩展名
		for (i = doti +1 ; i < lfni ; ++i)
		{
			w = lfn[i];
			if (IsSingleByteChar(w) && strchr(" +,/;=[\\]", w))  //除掉长文件名中不可以的 就是" +,/;=[\\]"  有一个空格
			{//出现了非法字符  只能用长文件名
				entry->mStatus |= ST_LFN;
				continue;
			}
			w_w.charw = WchrToUpper(w);		//需要的话先将字符转换成大小的形式
			w_w = ChrConvert(w_w , FALSE);	//在将字符转换成多字节字符
			if(w_w.char1){
				sfn[sfni++] = w_w.char1;
				if(sfni == 11) break;
			}
			sfn[sfni++] = w_w.char2;
			if(sfni == 11) break;
		}
	}
	//扩展名不够的话填空格
	for (;sfni < 11;++sfni)	sfn[sfni] = 0x20;

	return DR_OK;
}

// DRES DFat32::GetDelSegName(const WCHAR** path, PVOID entr)
// {
// 	PDirEntry	entry	= PDirEntry(entr);
// 	WCHAR*		lfn		= entry->mLFN;
// 	char*		sfn		= (char*)entry->mSFN;
// 	const WCHAR* p		= (*path) + 1;   //跳过第一个字节  "*"
// 	WCHAR		w		= 0;	//Unicode字符缓存
// 	int			i		= 0;	//辅助变量
// 	int			lfni	= 0;	//lfn超尾
// 	int			sfni	= 0;	//sfn超尾
// 	int			doti	= 0;	//最后一个点的位置
// 	W_CHAR		w_w		= {0};	//用于字符转换的字符
// 
// 	entry->mStatus = 0;			//先清理一下状态
// 	::memset(entry->mLFN , 0 ,sizeof(WCHAR)*MAX_LFN);
// 	::memset(entry->mSFN , 0 ,11);
// 
// 	for (i = 0; !IsPathSeparator(w = p[i]) && w  ; ++i)
// 	{
// 		//长文件名中空格和点可以在中间。但不可以在开始和末尾
// 		//文件名前面不可以有空格或者点
// 		if(!lfni && (w <= 0x20 /*|| w == '.'*/ ))	//windows资源管理器虽然不可创建以点开头的文件，但是他却是是却是存在的  2012-5-13
// 			continue;
// 		if(lfni > MAX_LFN) return DR_INVALID_NAME;	//文件名超过了255个字符
// 		if(IsSingleByteChar(w) && strchr("\"*:<>\?|", w))	return DR_INVALID_NAME;					//有非法字符
// 
// 		lfn[lfni++] = w;
// 	}
// 	if(!w)	//到了最后一段
// 		entry->mStatus |= ST_LAST;
// 	*path = p + i +1;//返回剩下的路径
// 
// 	lfn[lfni] = 0;	
// 	//去掉后面的空格和点
// 	for (; lfni > 0 &&(lfn[lfni-1]== 0x20 || lfn[lfni-1]== '.');--lfni)
// 		lfn[lfni-1] = 0;
// 	if(!lfni)  return DR_INVALID_NAME;//没有名字
// 
// 	//同时有大写和写的时候也只能用长文件名
// 	if(entry->mStatus & ST_UPPER && entry->mStatus & ST_LOWER)
// 		entry->mStatus |= ST_LFN;
// 
// 	//长文件名创建完毕
// 	//开始多文件名
// 
// 	//查找扩展名的位置  //第一个字符不可能是点
// 	for (doti = lfni-1; doti >= 0 && lfn[doti] != '.';--doti);  //windows 其实是可以存在一点开头的文件的     
// 	//doti==-1  没扩展名
// 	if(doti == -1) 
// 		doti = 0;
// 	else if(doti >/*=*/ 8)	//超出了8.3命名法   //此处不得有等于 "."的最大位置是可以等于8的  2012-5-13
// 		entry->mStatus |= ST_LFN;
// 
// 	//开始创建短文件名
// 	//不得小于0x20 以及"\"*+,./:;<=>\?[\\]|"
// 	//先来文件的名字
// 	for (sfni = 0 ,i = 0 ; i < lfni; ++i )
// 	{
// 		if(i == doti && doti)  break;  //该扩展名了
// 		w = lfn[i];
// 		if (IsSingleByteChar(w) && strchr(" .+,/;=[\\]", w))  //除掉长文件名中不可以的 就是" +,/;=[\\]"  有一个空格
// 		{//出现了非法字符  只能用长文件名
// 			entry->mStatus |= ST_LFN;
// 			continue;
// 		}
// 
// 		//短文件名如果既有大写又有小写就只能用长文件名
// 		if(!(entry->mStatus & ST_LFN))
// 		{
// 			if (IsCharUpper((char)w))  entry->mStatus |= ST_UPPER;
// 			if (IsCharLower((char)w))  entry->mStatus |= ST_LOWER;
// 		}
// 
// 		w_w.charw = WchrToUpper(w);		//需要的话先将字符转换成大小的形式
// 		w_w = ChrConvert(w_w , FALSE);	//在将字符转换成多字节字符
// 		if(w_w.char1){
// 			sfn[sfni++] = w_w.char1;
// 			if(sfni == 8) break;
// 		}
// 		sfn[sfni++] = w_w.char2;
// 		if(sfni == 8) break;
// 	}
// 
// 	//填充名中的空格
// 	for (;sfni < 8;++sfni)	sfn[sfni] = 0x20;
// 
// 	//该扩展名了
// 	if(doti){   //有扩展名
// 		for (i = doti +1 ; i < lfni ; ++i)
// 		{
// 			w = lfn[i];
// 			if (IsSingleByteChar(w) && strchr(" +,/;=[\\]", w))  //除掉长文件名中不可以的 就是" +,/;=[\\]"  有一个空格
// 			{//出现了非法字符  只能用长文件名
// 				entry->mStatus |= ST_LFN;
// 				continue;
// 			}
// 			w_w.charw = WchrToUpper(w);		//需要的话先将字符转换成大小的形式
// 			w_w = ChrConvert(w_w , FALSE);	//在将字符转换成多字节字符
// 			if(w_w.char1){
// 				sfn[sfni++] = w_w.char1;
// 				if(sfni == 11) break;
// 			}
// 			sfn[sfni++] = w_w.char2;
// 			if(sfni == 11) break;
// 		}
// 	}
// 	//扩展名不够的话填空格
// 	for (;sfni < 11;++sfni)	sfn[sfni] = 0x20;
// 
// 	return DR_OK;
// }

DRES DFat32::GetDirEntry( const WCHAR* path ,PVOID entr)
{
	DRES		res  = DR_OK;
	BYTE*		dir  = NULL; 
	BYTE		attr = 0;
	PDirEntry	entry= PDirEntry(entr);

	if (IsPathSeparator(path[0]))	++path;	//跳过前面的分隔符
	entry->mStartClust = m1stDirClut;	//从根目录开始
	if(path[0] == 0){						//空路径也就一位置是当前分区本身
		res = PosEntry(entry , 0);			//定位到制定的入口所在的山区
		entry->mDir = 0;					//因为没有只想任何目录 而是当前分区 所以没有入口
	}else{									//一个指定的目录
		while (TRUE)
		{
			res = GetSegName(&path ,entry);	//获得path中的第一个路径段
		
			if(res)  break;					//文件名有问题
			
			if (entry->mIsDelFile)
			{//查找已经删除的了的入口
				res = FindDelEntry(entry);
			}else{
				//正常入口
				res = FindEntry(entry);			//在指定的扇区/簇中查找制定的入口
			}

			
			if(res != DR_OK)
			{//查找失败
				if(res == DR_FAT_EOF)   //收索粗炼已经结束  
					res = DR_NO_PATH;	//没有指定的路径
				break;
			}
			//此次成功
			if(entry->mStatus & ST_LAST)	//最后一次已经匹配了 找到了 
				break;

			dir = entry->mDir;
			attr = PSDE(dir)->mAttr;
			if(!(attr & ATTR_DIRECTORY)){	//不是一个目录 无法再更下去了
				res = DR_NO_PATH;
				break;
			}
			entry->mStartClust = (PSDE(dir)->mFstClusHI << 16)|PSDE(dir)->mFstClusLO;
		}
	}
	return res;
}

DRES DFat32::FindEntry(PVOID entr)
{
	DRES	res		= DR_OK;
	BYTE*	dir		= NULL;
	BYTE	flag	= 0;
	BYTE	attr	= 0;
	BYTE	chSum	= 0;		//短文件名校验和
	BYTE	order	= 0;		//目录项序号
	PDirEntry entry = PDirEntry(entr);

	res = PosEntry(entry , 0);		//重定位到第一个入口的位置 定位扇区，0目录的位置
	if (res) return res;			//从定位失败
	
	do{	//遍历目录中每一个入口
		res = MoveView(entry->mCurSect);
		if(res)  break;
		dir = entry->mDir;			//入口的位置
		flag = dir[0];
		if( flag == 0){				//到了目录的末尾
			res = DR_NO_FILE;
			break;		
		}
		attr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;//目录项的属性
		
		if (flag == 0xE5 || \
			((attr & ATTR_VOLUME_ID) && attr != ATTR_LONG_NAME)) {	//不是一个有效的入口
			order = 0xFF;
		} else {//是一个有效的入口
			if(attr == ATTR_LONG_NAME){//长文件名入口
				if(flag & 0x40)	{	//是一个长文件名序列的开始  也就是一个长文件名的最后一个部分
					chSum = PLDE(dir)->mChksum;//文件名的校验和
					flag &= 0xBF;	//去掉0x40的掩码  抠出序号
					order = flag;			
				}
				//计算下一个长名目录的序号
				order = (order == flag && chSum == PLDE(dir)->mChksum &&CompLFN(entry->mLFN ,dir))? order - 1 : 0xFF;

			}else{  //一个短文件名
				if(!order && chSum == ChkSum(dir))
					break;			//找到了匹配的长名序列
				order = 0xFF;
				if(!(entry->mStatus & ST_LFN)&& CompSFN((char*)entry->mSFN , (char*)dir)) break;
			}
		}
		res = NextEntry(entry);  //移到写一个入口
	} while (!res);

	return res;
}

DRES DFat32::FindDelEntry(PVOID entr)
{
	DRES		res		= DR_OK;
	BYTE*		dir		= NULL;
	BYTE		flag	= 0;
	BYTE		attr	= 0;
	USHORT		chSum	= 0xFFFF;	//这里的校验和为两个字节
									//主要是因为用此域做判断是否是一个新的长文件名的开始
									//应为实际的校验和是一个字节,也就是说醉倒为0xFF 
									//而我用0xFFFF表当前还没有匹配到一个长名目录项
	BYTE		order	= 0;		//目录项序号
	PDirEntry	entry = PDirEntry(entr);
	WCHAR		nambuf[MAX_LFN+1] = {0}; //文件名缓存
	WCHAR       path[MAX_PATH] = {0};	//路径缓存
	W_CHAR		w_w;					//用于计算校验和

	res = PosEntry(entry , 0);		//重定位到第一个入口的位置 定位扇区，0目录的位置
	if (res) return res;			//从定位失败

	do{	//遍历目录中每一个入口
		res = MoveView(entry->mCurSect);
		if(res)  break;				//出错
		dir = entry->mDir;	//入口的位置
		flag = dir[0];				//第一个字节的标志位
		if( flag == 0){				//到了目录的末尾
			res = DR_FAT_EOF;
			return DR_FAT_EOF;
		}
		attr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;//目录项的属性

		if (flag != 0xE5) {			//不是一个删除了的入口
			chSum = 0xFFFF;
		} else {//是一个有效的入口
			if(attr == ATTR_LONG_NAME){//长文件名入口

				if(chSum == 0xFFFF)	{
					//是一个长文件名序列的开始
					//也就是一个长文件名的最后一个部分
					chSum = PLDE(dir)->mChksum;//记录下校验和
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}
				//判断
				if(chSum == PLDE(dir)->mChksum)
				{//匹配到一个长文件名
					AppLFN(nambuf , dir);//取出文件名将其添加到缓存的前面
				}else{//匹配失败 , 放弃原有的匹配结果  ，进行一个新的匹配过程
					//清空缓存 ,进行下一次匹配
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存

					//进行下一次匹配
					chSum = PLDE(dir)->mChksum;
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
					AppLFN(nambuf , dir);//取出文件名将其添加到缓存的前面
				}

			}else{  //一个短文件名
				//短文件名入口的第一个字节设置了0xE5  已经无法计算校验和了
				//在这里先长文件名中还原，还原后再计算
				BYTE btBack;
				w_w.charw = nambuf[0];
				w_w = ChrConvert(w_w ,FALSE );
				btBack = dir[0];
				if (w_w.char1)//第一个字符是多字节字符
					dir[0] = w_w.char1;
				else		 //第一个字符是单字节字符
					dir[0] = w_w.char2;

				if(chSum == ChkSum(dir)){  
					 dir[0] = btBack;   //比较完了后就可以还原
					//长文件名比较
					if (0 == memcmp(nambuf , entry->mLFN , wcslen(entry->mLFN) ))
					{//找到了
						break;
					}
					chSum = 0xFFFF;
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
			
				}else{						//纯粹的短文件名入口
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存

//					(*(char*)dir) = '*';//删除标志
					SetBYTE(dir , '*');
					//SetSFN(nambuf ,dir);

					if(!(entry->mStatus & ST_LFN)&& CompSFN((char*)entry->mSFN , (char*)dir)) 
					{
						dir[0] = btBack; 					
						break;
					}
					dir[0] = btBack; 	
				}
			}
		}
		res = NextEntry( entry );  //移到下一个入口
		if (res == DR_FAT_EOF)    //到了粗炼尾部
		{
			return DR_FAT_EOF;
		}
	} while (!res);

	return res;
}

BOOL DFat32::CompLFN(const WCHAR* path, BYTE* dir)
{
	//长文件名目录入口中的每一个字符所在的字节位置
	static const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};
	BYTE	index = 0;
	WCHAR	wp	  = 0;
	WCHAR	wd	  = 0;
	int		i	  = 0;
	
	index = dir[0] & 0xBF;			//抠出序号
	index = (index - 1) *13;		//序号对应的第一个字符  每个目录最多十三个字符，序号从一开始 
	do{
		wd = (WCHAR)GetWORD(dir + LfnOfs[i]);
		if(wd)
		{		//已经到了整个路径的结尾
			wp = path[index++];
			wd = WchrToUpper(wd);
			wp = WchrToUpper(wp);
			if(wd != wp)		//匹配失败
				return FALSE;
		}else{	//已经到了整个路径的结尾
			if(1+i == 13)  //刚好最后一个字符是0
				break;
			wd = (WCHAR)GetWORD(dir + LfnOfs[++i]);
			if(wd != 0xFFFF)	return FALSE;
			else	return TRUE;
		}
	} while (++i < 13);				//匹配13次  一个入最多有十三个字符

	return TRUE;
}
//长文件名目录入口中的每一个字符所在的字节位置
static const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};
DRES DFat32::SetLFN( WCHAR* path, BYTE* dir)
{
	BYTE	index	= 0;
	int		i		= 0;

	index = dir[0] & 0xBF;			//抠出序号
	index = (index - 1) *13;		//序号对应的第一个字符  每个目录最多十三个字符，序号从一开始 
	do{
		if(!(path[index++] = (WCHAR)GetWORD(dir + LfnOfs[i])))
			break;		
	} while (++i < 13);				//匹配13次  一个入最多有十三个字符
	
	//文件名的末尾了
	//if((i == 13) && ((dir[0]&0xBF) == 1))
	//	path[index] = 0;

	return DR_OK;
}
DRES DFat32::AppLFN( WCHAR* c, BYTE* dir)
{
	WCHAR	temp  = 0;
	int		i	  = 0;
	size_t	len   = 0;//原有的数据长度
	WCHAR	buf[14] = {0};
	
	for(i = 0 ; i < 13 ; ++i)
	{
		temp = (WCHAR)GetWORD(dir + LfnOfs[i]);
		if (!temp)//结尾了
			break;
		buf[i] = temp;
	}
	
	//将元缓存中的数据后裔 i个字符
	len = wcslen(c);
	if(len) ::memmove(((BYTE*)c) + i * 2 , (BYTE*)c , len*2);
	memcpy((BYTE*)c , (BYTE*)buf , i*2);

	return DR_OK;
}
DRES DFat32::SetSFN( WCHAR* path, BYTE* dir)
{
	//多字节字符缓存
	char buf[20] = {0};
	int  bufi	 = 0;
	char a		 = 0;
	int  i		 = 0;
	BYTE nameCase = PSDE(dir)->mNameCase;

	//抠出名字部分
	if (nameCase & FNAME_LOWER_CASE){
		//文件名为小写
		for(; i < 8 ; ++i){ a = dir[i]; if(a!= 0x20) buf[bufi++] = ChrToLower(a); }
	}else{//文件名为大写
		for(; i < 8 ; ++i) { a = dir[i]; if(a!= 0x20) buf[bufi++] = a; }
	}

	//追加一个点
	buf[bufi++] = '.';

	//取出扩展名
	if (nameCase & FEXT_NAME_LOWER_CASE)
	{//扩展名为大写
		for( ; (i < 11) && ((a = dir[i]) != 0x20); ++i) buf[bufi++] = ChrToLower(a);	
	}else{
		//扩展名为小写
		for( ; (i < 11) && ((a = dir[i]) != 0x20); ++i) buf[bufi++] = a;
	}


	//收尾
	if(buf[bufi-1] == '.')	buf[bufi-1] = 0;  //没有扩展名
	else	buf[bufi] = 0;			//有扩展名

	MultyByteToUnic(buf , path , MAX_LFN+1);

	return DR_OK;
}
BOOL DFat32::CompSFN(const char* path1, const char* path2)
{
	for (int i = 0 ; i < 11 ;++i)
	{
		if(ChrToUpper(path1[i]) != ChrToUpper(path2[i]))
			return FALSE;
	}
	return TRUE;
}

DRES DFat32::NextEntry(PVOID entr)
{
	PDirEntry	entry	= PDirEntry(entr); 
	WORD		i		= entry->mIndex + 1;
	DWORD		idxSec	= 0;//下一个入口所在的当前目录的扇区号
	DWORD		clust	= 0;

	if(!(i % (SECTOR_SIZE /32)))
	{								//需要进入下一个扇区了
		++entry->mCurSect;			//物理扇区下移
		idxSec = i / (SECTOR_SIZE /32);
		if(idxSec && !(idxSec % mSecPerClus))
		{
			clust = GetFATFromFAT1(entry->mCurClust);
			if(clust == 1)
				return DR_INIT_ERR;	//无效簇号
			if(clust == 0xFFFFFFFF)
				return DR_DEV_IO_ERR;//设备IO错误
			if(clust > mMaxClust)
				return DR_FAT_EOF;	//到了结尾了
			
			entry->mCurClust = clust;
			entry->mCurSect = ClustToSect(clust);
		}
	}
	entry->mIndex = i;
	entry->mDir = mView + (i % (SECTOR_SIZE / 32)) * 32;

	return DR_OK;
}

BYTE DFat32::ChkSum(BYTE* pFcbName)
{//源自 fatgen103.doc
	short FcbNameLen = 0;
	BYTE  Sum		 = 0;

	for (FcbNameLen=11; FcbNameLen!=0; FcbNameLen--) 
	{
		// NOTE: The operation is an unsigned char rotate right
		Sum = ((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + *pFcbName++;
	}
	return (Sum);
}

/*
DRES DFat32::OpenFileA(const char* path , DFat32File* file)
{
	//参数错误
	if (path == NULL || file == NULL) return DR_INVALED_PARAM;
	
	size_t len = strlen(path);
	std::vector<WCHAR> wPath(len + 1, 0);
	MultyByteToUnic(path , wPath.data(), len + 1);

	return OpenFileW(wPath.data(), file);
	
}
*/

DRES DFat32::OpenFileW(const WCHAR* path , DFat32File *file)
{
	if (!IsDevOpened())
		return DR_NO_OPEN;

	if (path == NULL || file == NULL)
		return DR_INVALED_PARAM;

	DirEntry entry;
	::memset(&entry , 0 , sizeof(entry));

	DRES res = GetDirEntry(path, &entry);
	if (res) //跟进失败
	{
		file->mFS = NULL;	//返回的文件句柄NULL
		return res;
	}

	file->mFS = this;
	//开始创建文件对象
	return NewFileHandle(file , &entry , path);
}

DRES DFat32::ListFile(DFat32File* fil, FIND_FILE listFun)
{
	if (!IsDevOpened())
		return DR_NO_OPEN;

	if (!fil || !listFun)
		return DR_INVALED_PARAM;		//参数错误
	if (!(fil->mAttr & ATTR_DIRECTORY))
		return DR_INVALED_PARAM;		//需要的是一个目录而不是一个文件

	DirEntry	entry;		//入口结构
	BYTE		chSum	= 0;		//短文件名校验和
	BYTE		order	= 0;		//目录项序号
	WCHAR		nambuf[MAX_LFN+1] = {0};

	entry.mStartClust = fil->mStartClust;//进入指定的目录的第一簇
	DRES res = PosEntry(&entry, 0);			//定位第一个山区的第一个入口结构
	if (res)
		return res;				//定位失败

	do{	//遍历目录中每一个入口
		res = MoveView(entry.mCurSect);
		if(res)  break;				//出错
		BYTE* dir = entry.mDir;			//入口的位置
		BYTE flag = dir[0];				//第一个字节的标志位
		if( flag == 0){				//到了目录的末尾
			(*listFun)(NULL);		//通知回调者查找完毕
			break;		
		}

		BYTE attr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;//目录项的属性

		if (flag == 0xE5||\
			((attr & ATTR_VOLUME_ID) && attr != ATTR_LONG_NAME)) {	//不是一个有效的入口
				order = 0xFF;
		} else {//是一个有效的入口
			if(attr == ATTR_LONG_NAME){//长文件名入口
				if(flag & 0x40)	{	//是一个长文件名序列的开始  也就是一个长文件名的最后一个部分
					chSum = PLDE(dir)->mChksum;//文件名的校验和
					flag &= 0xBF;	//去掉0x40的掩码  抠出序号
					order = flag;	
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}
				//计算下一个长名目录的序号
				if(order == flag && chSum == PLDE(dir)->mChksum)
				{//匹配到一个长文件名
					--order; 
					SetLFN(nambuf , dir);		//取出文件名
				}else{//匹配失败
					order = 0xFF;
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}

			}else{  //一个短文件名
				if(!order && chSum == ChkSum(dir)){
					(*listFun)(nambuf);		//找到一个长文件名目录对应的短文件名入口
				}else{						//纯粹的短文件名入口
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
					order = 0xFF;
					SetSFN(nambuf ,dir);
					(*listFun)(nambuf);		//找到一个长文件名	
				}
			}
		}
		res = NextEntry(&entry);  //移到下一个入口
		if (res == DR_FAT_EOF)    //到了粗炼尾部
		{
			(*listFun)(NULL);		//通知回调者查找完毕
			return DR_OK;
		}
	} while (!res);

	return res;
}

DRES DFat32::FindFile( DFat32File* dir , FINDER* finder , BOOL findDel /*= FALSE*/ )
{
	//先清理一下结果
	*finder = NULL;

	if (!IsDevOpened())
		return DR_NO_OPEN;

	//例行安全检查
	if (!dir || !finder)
		return DR_INVALED_PARAM;
	if (!dir->IsValid())
		return DR_INVALID_HANDLE;
	if (!dir->IsDir())
		return DR_IS_FILE;

	//创建一个查找句柄
	std::unique_ptr<Fat32FileFinder> pfff(new Fat32FileFinder());

	//是否是要查找已经删除了的文件
	pfff->isFindDel = findDel;
	wcscpy(pfff->path , dir->mPath.c_str());
	pfff->isEnd = FALSE;
// 	len = wcslen(pfff->path);
// 	//添加分割符
// 	if (!IsPathSeparator(pfff->path[len - 1]))
// 	{
// 		pfff->path[len] = PATH_SEPAR;
// 		pfff->path[len + 1] = 0;
// 	}

	pfff->entry.mStartClust = dir->mStartClust;//进入指定的目录的第一簇
	DRES res = PosEntry(&(pfff->entry), 0);			//定位第一个山区的第一个入口结构
	if (res)
	{
		//定位失败，这是文件句柄有问题
		return DR_INVALID_HANDLE;				//定位失败
	}

	//返回查找句柄
	*finder = FINDER(pfff.release());

	return DR_OK;
}

DRES DFat32::FindNextFileW( FINDER finder , DFat32File* file )
{
	if (!IsDevOpened())
		return DR_NO_OPEN;
	if (!finder && !file)
		return DR_INVALED_PARAM;

	//进行具体的查找
	PFat32FileFinder pfff = PFat32FileFinder(finder);
	if (pfff->isFindDel)
		return FindNextDelFile(finder, file);
	else
		return FindNextExistFile(finder, file);

	return DR_OK;
}

DRES DFat32::FindNextExistFile( FINDER find , DFat32File* file )
{
	PFat32FileFinder finder = PFat32FileFinder(find);//查询结构体
	if (finder->isEnd)
		return DR_FAT_EOF;

	DRES		res		= DR_OK;
	BYTE*		dir		= NULL;
	BYTE		flag	= 0;		//第一个字节的标准
	BYTE		attr	= 0;		//属性
	BYTE		chSum	= 0;		//短文件名校验和
	BYTE		order	= 0;		//目录项序号
	WCHAR		nambuf[MAX_LFN+1] = {0}; 
	size_t		len = 0;
	WCHAR		path[MAX_PATH + 1] = {0};

	do{	//遍历目录中每一个入口
		res = MoveView(finder->entry.mCurSect);
		if(res)  break;				//出错
		dir = finder->entry.mDir;	//入口的位置
		flag = dir[0];				//第一个字节的标志位
		if( flag == 0){				//到了目录的末尾
			res = DR_FAT_EOF;		//到了到了结尾
			finder->isEnd = TRUE;
			break;		
		}
		
		attr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;//目录项的属性
		
		if (flag == 0xE5||\
			((attr & ATTR_VOLUME_ID) && attr != ATTR_LONG_NAME)) {	//不是一个有效的入口
			order = 0xFF;
		} else {//是一个有效的入口

			if(attr == ATTR_LONG_NAME){//长文件名入口
				if(flag & 0x40)	{	//是一个长文件名序列的开始  也就是一个长文件名的最后一个部分
					chSum = PLDE(dir)->mChksum;//文件名的校验和
					flag &= 0xBF;	//去掉0x40的掩码  抠出序号
					order = flag;	
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}
				//计算下一个长名目录的序号
				if(order == flag && chSum == PLDE(dir)->mChksum)
				{//匹配到一个长文件名
					--order; 
					SetLFN(nambuf , dir);		//取出文件名
				}else{//匹配失败
					order = 0xFF;
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}
				
			}else{  //一个短文件名
				if(!order && chSum == ChkSum(dir)){
				//找到一个长文件名目录对应的短文件名入口

					//需要进行文件路径拼接
					wcscpy(path , finder->path);//父路径
					if((len = wcslen(path)) > 1 
						&& !IsPathSeparator(path[len - 1]))     //不是在 更目录中找
						wcscat(path , L"/");	//分隔符
					wcscat(path , nambuf);		//文件名

					//创建文件件句柄
					NewFileHandle(file , &(finder->entry) , path);
					
					//先进入下一次查找的位置
					res = NextEntry(&(finder->entry));  //移到下一个入口
					if (res == DR_FAT_EOF)
						finder->isEnd = TRUE;
					
					return DR_OK;

					//(*listFun)(nambuf);		
				}else{						//纯粹的短文件名入口
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
					order = 0xFF;
					SetSFN(nambuf ,dir);

					//好药进行文件路径拼接
					wcscpy(path , finder->path);//父路径
					if((len = wcslen(path)) > 1 
						&& !IsPathSeparator(path[len - 1]))     //不是在 更目录中找
						wcscat(path , L"/");	//分隔符
					wcscat(path , nambuf);		//文件名

					//创建文件句柄
					NewFileHandle(file , &(finder->entry) , path);

					//先进入下一次查找的位置
					res = NextEntry(&(finder->entry));  //移到下一个入口
					if (res == DR_FAT_EOF)
						finder->isEnd = TRUE;
					
					return DR_OK;
				}
			}
		}
		res = NextEntry(&(finder->entry));  //移到下一个入口
		if (res == DR_FAT_EOF)    //到了粗炼尾部
		{
			finder->isEnd = TRUE;
			return DR_FAT_EOF;
// 			res = DR_FAT_EOF
// /*			(*listFun)(NULL);		//通知回调者查找完毕*/
// 			return DR_OK;
		}
	} while (!res);
	
	return res;
}

DRES DFat32::FindNextDelFile( FINDER find , DFat32File* file )
{
	PFat32FileFinder finder = PFat32FileFinder(find);
	if (finder->isEnd)
		return DR_FAT_EOF;

	DRES		res		= DR_OK;	//操作结果
	BYTE*		dir		= NULL;		//在缓存中的入口结构
	BYTE		flag	= 0;		//第一个字节的标准
	BYTE		attr	= 0;		//属性
	USHORT		chSum	= 0xFFFF;	//这里的校验和为两个字节
									//主要是因为用此域做判断是否是一个新的长文件名的开始
									//应为实际的校验和是一个字节,也就是说醉倒为0xFF 
									//而我用0xFFFF表当前还没有匹配到一个长名目录项
	WCHAR		nambuf[MAX_LFN+1] = {0}; //文件名缓存
	WCHAR       path[MAX_PATH] = {0};	//路径缓存
	DFat32File	df;
	W_CHAR		w_w;					//用于计算校验和

	do{	//遍历目录中每一个入口
		res = MoveView(finder->entry.mCurSect);
		if(res)  break;				//出错
		dir = finder->entry.mDir;	//入口的位置
		flag = dir[0];				//第一个字节的标志位
		if( flag == 0){				//到了目录的末尾

			res = DR_FAT_EOF;
			finder->isEnd = TRUE;
			return DR_FAT_EOF;
		}
		attr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;//目录项的属性

		if (flag != 0xE5) {			//不是一个删除了的入口
			//order = 0xFF;
			chSum = 0xFFFF;
		} else {//是一个有效的入口
			if(attr == ATTR_LONG_NAME){//长文件名入口

				if(chSum == 0xFFFF)	{
					//是一个长文件名序列的开始
					//也就是一个长文件名的最后一个部分
					chSum = PLDE(dir)->mChksum;//记录下校验和
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}
				//判断
				if(chSum == PLDE(dir)->mChksum)
				{//匹配到一个长文件名
					AppLFN(nambuf , dir);//取出文件名将其添加到缓存的前面
				}else{//匹配失败 , 放弃原有的匹配结果  ，进行一个新的匹配过程
					//清空缓存 ,进行下一次匹配
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
					
					//进行下一次匹配
					chSum = PLDE(dir)->mChksum;
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
					AppLFN(nambuf , dir);//取出文件名将其添加到缓存的前面

				}

			}else{  //一个短文件名
				//短文件名入口的第一个字节设置了0xE5  已经无法计算校验和了
				//在这里先长文件名中还原，饭后再计算
				BYTE btBack = 0;		//备份第一个字节
				w_w.charw = nambuf[0];
				w_w = ChrConvert(w_w ,FALSE );
				btBack = dir[0];
				if (w_w.char1)//第一个字符是多字节字符
					dir[0] = w_w.char1;
				else		 //第一个字符是单字节字符
					dir[0] = w_w.char2;
				
				if(chSum == ChkSum(dir)){

					//创建文件句柄
					//设置删除标志
					wcscpy(path , finder->path);//父路径
					if(wcslen(path) != 1)       //不是在 更目录中找
						wcscat(path , L"/");	//分隔符
					wcscat(path , L"*");		//删除标志
					wcscat(path , nambuf);		//文件名

					//初始化返回的文件句柄
					NewFileHandle(file , &(finder->entry) , path);//创建句柄
					chSum = 0xFFFF;			//重置校验和
					
					//先提前进入下一个入口
					res = NextEntry( &(finder->entry) );  //移到下一个入口
					if (res == DR_FAT_EOF)    //到了粗炼尾部
						finder->isEnd = TRUE;
					
					dir[0] = btBack;		//还原缓存中的数据
					return DR_OK;				
// 					res = DR_OK;
// 
// 					break;
					//(*listFun)(df);		//找到一个长文件名目录对应的短文件名入口
				}else{						//纯粹的短文件名入口
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
					
					(*(char*)dir) = '*';//删除标志
					SetSFN(nambuf ,dir);

					wcscpy(path , finder->path);//父路径
					if(wcslen(path) != 1)     //不是在 更目录中找
						wcscat(path , L"/");	//分隔符
					wcscat(path , nambuf);		//文件名

					//初始化返回的文件句柄
					NewFileHandle(file , &(finder->entry) , path);//创建句柄
					chSum = 0xFFFF;			//重置校验和

					
					//先提前进入下一个入口
					res = NextEntry( &(finder->entry) );  //移到下一个入口
					if (res == DR_FAT_EOF)    //到了粗炼尾部
						finder->isEnd = TRUE;

					dir[0] = btBack;  //还原一下数据
					return DR_OK;
// 					res = DR_OK;
// 
// 					break;
// 
// 					//创建删除标志
// /*					(*listFun)(df);		//找到一个长文件名	*/
					
				}
			}
		}
		res = NextEntry( &(finder->entry) );  //移到下一个入口
		if (res == DR_FAT_EOF)    //到了粗炼尾部
		{
			finder->isEnd = TRUE;
// 			df.mFS = NULL;
// /*			(*listFun)(df);		//通知回调者查找完毕*/
			return DR_FAT_EOF;
		}
	} while (!res);

	return res;
}

void DFat32::FindClose( FINDER finder )
{
	if (finder)
		delete PFat32FileFinder(finder);
}

DRES DFat32::ListDelFile(DFat32File* fil, FIND_DEL_FILE listFun)
{
	if (!IsDevOpened())
		return DR_NO_OPEN;

	if (!fil || !listFun)
		return DR_INVALED_PARAM;		//参数错误
	if (!(fil->mAttr & ATTR_DIRECTORY))
		return DR_INVALED_PARAM;		//需要的是一个目录而不是一个文件

	DRES		res		= DR_OK;	//操作结果
	DirEntry	entry;		//入口结构
	BYTE*		dir		= NULL;		//在缓存中的入口结构
	BYTE		flag	= 0;		//第一个字节的标准
	BYTE		attr	= 0;		//属性
	USHORT		chSum	= 0xFFFF;	//这里的校验和为两个字节
									//主要是因为用此域做判断是否是一个新的长文件名的开始
									//应为实际的校验和是一个字节,也就是说醉倒为0xFF 
									//而我用0xFFFF表当前还没有匹配到一个长名目录项
	WCHAR		nambuf[MAX_LFN+1] = {0}; //文件名缓存
	WCHAR       path[MAX_PATH] = {0};	//路径缓存
	DFat32File	df;
	W_CHAR		w_w;					//用于计算校验和

	entry.mStartClust = fil->mStartClust;//进入指定的目录的第一簇
	res = PosEntry(&entry , 0);			//定位第一个山区的第一个入口结构
	if (res) return res;				//定位失败

	do{	//遍历目录中每一个入口
		res = MoveView(entry.mCurSect);
		if(res)  break;				//出错
		dir = entry.mDir;			//入口的位置
		flag = dir[0];				//第一个字节的标志位
		if( flag == 0){				//到了目录的末尾
			df.mFS = NULL;
			(*listFun)(df);		//通知回调者查找完毕
			break;		
		}
		attr = PSDE(dir)->mAttr & ATTR_FAT32_MASK;//目录项的属性

		if (flag != 0xE5) {			//不是一个删除了的入口
			//order = 0xFF;
			chSum = 0xFFFF;
		} else {//是一个有效的入口
			if(attr == ATTR_LONG_NAME){//长文件名入口

				if(chSum == 0xFFFF)	{
					//是一个长文件名序列的开始
					//也就是一个长文件名的最后一个部分
					chSum = PLDE(dir)->mChksum;//记录下校验和
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}
				//判断
				if(chSum == PLDE(dir)->mChksum)
				{//匹配到一个长文件名
					AppLFN(nambuf , dir);//取出文件名将其添加到缓存的前面
				}else{//匹配失败
					//清空缓存
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
				}

			}else{  //一个短文件名
				//短文件名入口的第一个字节设置了0xE5  已经无法计算校验和了
				//在这里先长文件名中还原，饭后再计算
				w_w.charw = nambuf[0];
				w_w = ChrConvert(w_w ,FALSE );
				if (w_w.char1)//第一个字符是多字节字符
					dir[0] = w_w.char1;
				else		 //第一个字符是单字节字符
					dir[0] = w_w.char2;
				
				if(chSum == ChkSum(dir)){

					//创建文件句柄
					//设置删除标志
					wcscpy(path , fil->mPath.c_str());//父路径
					if(wcslen(path) != 1)     //不是在 更目录中找
						wcscat(path , L"/");	//分隔符
					wcscat(path , L"*");		//删除标志
					wcscat(path , nambuf);		//文件名
					NewFileHandle(&df , &entry , path);//创建句柄

					(*listFun)(df);		//找到一个长文件名目录对应的短文件名入口
					chSum = 0xFFFF;			//重置校验和
				}else{						//纯粹的短文件名入口
					::memset(nambuf , 0 , sizeof(WCHAR)*(MAX_LFN+1));//清空名字缓存
					
					(*(char*)dir) = '*';//删除标志
					SetSFN(nambuf ,dir);

					wcscpy(path , fil->mPath.c_str());//父路径
					if(wcslen(path) != 1)     //不是在 更目录中找
						wcscat(path , L"/");	//分隔符
					wcscat(path , nambuf);		//文件名
					NewFileHandle(&df , &entry , path);//创建句柄

					//创建删除标志
					(*listFun)(df);		//找到一个长文件名	
					chSum = 0xFFFF;			//重置校验和
				}
			}
		}
		res = NextEntry(&entry);  //移到下一个入口
		if (res == DR_FAT_EOF)    //到了粗炼尾部
		{
			df.mFS = NULL;
			(*listFun)(df);		//通知回调者查找完毕
			return DR_OK;
		}
	} while (!res);

	return res;
}

DRES DFat32::NewFileHandle( DFat32File* file, PVOID entr ,const WCHAR* path )
{
	return file->InitFile(entr , path , this);
}

DWORD DFat32::GetSecCount()
{
	return mSectors;
}

BYTE DFat32::GetSecPerClust()
{
	return IsDevOpened() ? mSecPerClus : 0;
}

DRES DFat32::IsContainFat32Flag(const WCHAR* cDevName, LONG_INT offset)
{
	if (cDevName == NULL)
		return DR_INVALED_PARAM;

	//打开设备
	HANDLE hDev = ::CreateFile(cDevName,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								0,
								NULL);
	if (hDev == INVALID_HANDLE_VALUE) //打开设备失败
		return DR_OPEN_DEV_ERR;

	DRES res = DR_OK;
	if (offset.QuadPart > 0)
	{
		//设置文件指针
		offset.LowPart = SetFilePointer(hDev , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
		if (offset.LowPart == -1 && GetLastError() != NO_ERROR )
			res = DR_DEV_CTRL_ERR;
	}

	//读取数据
	DWORD dwReaded = 0;
	FAT32_DBR fDbr = {0};
	if((DR_OK == res)
		&& !::ReadFile(hDev, &fDbr, sizeof(FAT32_DBR), &dwReaded, NULL)
		&& (dwReaded != sizeof(FAT32_DBR)))
		res =  DR_DEV_IO_ERR;
	
	//不需要了
	CloseHandle(hDev);
	//设备相关操作失败
	if (res != DR_OK)
		return res;
	
	if ( ( fDbr.bsEndSig  != MBR_END)	//dbr结束标记检查 
		||(fDbr.bsBytesPerSec != 512)	//每扇区的字节数是512的倍数，一般是512 ，我这里也处理512的情况
		||(fDbr.bsFATs != 2)			//FAT表数一定是二
		||(fDbr.bsRootDirEnts != 0)		//根目录数一定是0  这个值是给fat12/16用的
		||(fDbr.bsSectors != 0)			//总扇区数  也是fat12/16用的
		||(fDbr.bsFATsecs != 0)			//fat表的扇区数  也是fat12/16用的
		||(fDbr.bsHugeSectors == 0))	//一个fat32卷的扇区数  一定部位0
		return DR_NO;

	//以上的都是可以很定的，也就是说一个正常的FAT32卷以上的条件都会满足
	//可很肯定的值还有一些，比如在DBR中的定义的设备类型(fDbr.bsMedia)会
	//在FAT表的第一个字节有一个相同的值,一般是0xF8。还有就是根目录中第一
	//个入口的是一个卷标的存放位置，肯定会含有ATTR_VOLUME_ID(0x08)属性

	//接下来是判断FAT32标记，实际上并不是所有的FAT32卷都是用这个标记，特
	//别是第三方驱动
	if ( ( fDbr.bsFSType[0] != 0x46)	//F
		||(fDbr.bsFSType[1] != 0x41)	//A
		||(fDbr.bsFSType[2] != 0x54)	//T
		||(fDbr.bsFSType[3] != 0x33)	//3
		||(fDbr.bsFSType[4] != 0x32)	//2
		||(fDbr.bsFSType[5] != 0x20)	//空格
		||(fDbr.bsFSType[6] != 0x20)	//空格
		||(fDbr.bsFSType[7] != 0x20))	//空格
		return DR_NO;

	return DR_OK;
}

DRES DFat32::GetVolumeName(OUT WCHAR* cNameBuf, IN DWORD len)
{
	if (!IsDevOpened())
		return DR_NO_OPEN;

	if (NULL == cNameBuf)
		return DR_INVALED_PARAM;

	wcscpy(cNameBuf, L"");

	UCHAR	buf[SECTOR_SIZE] = {0};
	PSDE	sde		= PSDE(buf);

	//第一个根目录的扇区号
	DRES res = ReadData(buf, ClustToSect(m1stDirClut), SECTOR_SIZE);
	if (DR_OK != res)
		return res;

	if (sde->mAttr & ATTR_VOLUME_ID)
	{
		int i = 0;
		for ( i = 0 ; i< 11 && buf[i] != 0x20 ; ++i );
		buf[i] = 0;

		if (MultyByteToUnic((char*)buf, NULL, 0) > len)
			return DR_BUF_OVER;

		MultyByteToUnic((char*)buf, cNameBuf, len);
	}
	else
	{
		wcscpy(cNameBuf , L"NO NAME");
		return DR_NO_FILE_NAME;
	}

	return res;
}

const WCHAR* DFat32::GetDevName()
{
	if (!IsDevOpened())
		return NULL;

	return mDevName;
}

USHORT DFat32::GetReserveSector()
{
	if (!IsDevOpened())
		return 0;

	return mResSec;
}

DWORD DFat32::GetSectorPerFAT()
{
	if (!IsDevOpened())
		return 0;

	return mSecsPerFAT;
}

USHORT DFat32::GetFSInfoSec()
{
	if (!IsDevOpened())
		return 0;

	return mFSinfoSec;
}

DWORD DFat32::Get1stDirClust()
{
	if (!IsDevOpened())
		return 0;

	return m1stDirClut;
}

DWORD DFat32::GetRemainSectorCnt()
{
	if (!IsDevOpened())
		return 0;

	//（总扇区数-第一个目录所在簇号扇区）%每簇扇区数
	return (mSectors - (mResSec + (mFATs * mSecsPerFAT))) % mSecPerClus;
}

DWORD DFat32::GetMaxClustNum()
{
	if (!IsDevOpened())
		return 0;

	return mMaxClust;
}
