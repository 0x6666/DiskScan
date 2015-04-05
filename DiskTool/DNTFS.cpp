/***********************************************************************
 * FileName:	DNtfs.cpp
 * Author:		杨松
 * Created:		2012年4月20日 星期五
 * Purpose:		这是DNtfs的具体实现
 * Comment:		这个类是对一个Ntfs卷的抽象，提供对Ntfs操作的各种方法
 *
 * Modify:		在程序的运行期间发现有些目录的IndexBlock中的文件名区域中
 *				的DOS属性为零,也就是说在查找的过程中如果设置要查找的文件
 *				属性的话，可能会出错误，也就是说找不到，这次我将所有的带
 *				属性操作行数都改为了不带属性的函数，避免出现文件存在而找
 *				不到的情况.2012-05-21 16:34
 *
 *				在之前run的操作是直接写在NTFS文件类中，那样的话使用起来
 *				很受限,只在访问的时候通过判断是否是文件来决定初始化无名
 *				DATA属性中的RUN还是INDEX_ALLOCATION中的RUN，也就是说之处
 *				理了两种情况的RUN，而实际的RUN是可以被很多属性使用的，所
 *				以现在将RUN独立出来了，将其定义为DRun。2012-07-01 11:21。
 *
 *				在获取MFT记录的第一个扇区时，以前是直接通过记录号查找DBR
 *				中的指定的MFT，但是据说有些情况会出现MFT不会再一起的情况，
 *				所在在这里修改了查找MFT的算法。因为文件分配表其实也是一个
 *				文件,所以查找的话可以通过查找无名DATA属性的中RUN获得实际
 *				MFT记录所在的具体位置,这样也就避免了MFT一分为二的情况.当
 *				然，这种情况我没有见过，查过的资料也都说文件分配表示一个
 *				连续的空间,是预留的。2012-07-01 13:54。
 *
 *				优化MFT查找算法。在之前每一定位MFT记录是都要获取查找$MFT
 *				文件的无名DATA属性，这样的话效率会低了一点，现在是在打开
 *				设备是就将那个Run转换到MFT_BLOCK序列中去了，便于以后每一
 *				次查找时计算，这样的话只需获取一次$MFT无名属性Run了，且计
 *				算量减少了一些。2012-07-18 1:07.
 *
 ***********************************************************************/


#include "disktool.h"
#include "stut_connf.h"
#include "windows.h"

//取消意思按windows关于标注库的警告开关
#pragma warning(disable:4996)

//////////////////////////////////////////////////////////////////////
//用于查找文件句柄

//////////////////////////////////////////////////////////////////////
// typedef struct _FileFindHander{
// 	LONG_INT vcn;		//虚拟簇号  -1 表示indexRoot中的
// 	int index;			//在制定vcn入口索引
// }FIND_FILE_HANDER , *PFIND_FILE_HANDER;


//遍历类型  1  B+数遍历   2  Index_Allocation 遍历
#define  SEARCH_TYPE  2 


DNtfs::DNtfs()
	: mPMftBlock(NULL)
	, mDev(INVALID_HANDLE_VALUE)
	, mMftBlockCnt(0)
{
	mFSOff.QuadPart			= 0;
	mCluForMFTMirr.QuadPart	= 0;
	mCluForMFT.QuadPart		= 0;
}

DNtfs::~DNtfs()
{
}

DRES DNtfs::OpenDev(const WCHAR* devName, const PLONG_INT off)
{
	if (!devName || !off)
		return DR_INVALED_PARAM;

	if (!mDevName.empty())
		return DR_ALREADY_OPENDED;

	DRES		res = DR_OK;
	size_t		len = 0;
	char		buf[SECTOR_SIZE] = {0};
	PNTFS_DBR	pDBR = (PNTFS_DBR)buf;
	LONG_INT	temp = {0};

	mDevName = devName;
	//设备的偏移
	mFSOff = *off;

	//打开设备
	mDev = ::CreateFile(mDevName.c_str(),
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);
	if (mDev == INVALID_HANDLE_VALUE)
	{//打开设备失败
		mDevName.clear();
		mFSOff.QuadPart = 0;
		return DR_OPEN_DEV_ERR;
	}

	//读取dbr的数据
	temp.QuadPart = 0;
	mAllSec.QuadPart = 1;
	res = ReadData(buf , &temp , SECTOR_SIZE );	//读取分区的第一个扇区的数据
	mAllSec.QuadPart = 0;
	if(DR_OK != res)	return res;				//读取数据失败?
	if(pDBR->dbrEnd != MBR_END)//数据无效
		return DR_INIT_ERR;

	//读取MFT的起始逻辑簇号
	mSecPerClu		= pDBR->bpbSecPerClu;	//每簇的山区数
//	mResSec			= pDBR->bpbResSec;		//保留扇区数
	mAllSec			= pDBR->bpbAllSec;		//总的扇区数
	mCluForMFT		= pDBR->bpbCluForMFT;	//MFT
	mCluForMFTMirr	= pDBR->bpbCluForMFTMirr;//MFTMirr
	
	//初始化MFT块结构
	res = InitMFTBlock();

	if (res != DR_OK)
	{//操作失败
		mDevName.clear();
		mFSOff.QuadPart = 0;
		CloseHandle(mDev);
		mDev = INVALID_HANDLE_VALUE;
		if (NULL != mPMftBlock)
		{
			free(mPMftBlock);
			mPMftBlock = NULL;
		}
	}
	return res;
}

DRES DNtfs::InitMFTBlock()
{
	DNtfsFile	mftFile;//当前卷的MFT文件
	DRES		res = DR_OK;
	DNtfsFile::PAttrItem pAttrItem = NULL;
	DNtfsAttr	ntfsAttr;
	DRun		run;
	DWORD		i;
	LONG_INT	liStartMft;	//起始MFT记录号

	//打开$MFT文件
	res = OpenFileW(&mftFile , SYS_FILE_MFT);
	if (res != DR_OK) return res;  //文件打开失败

	//获得无名数据属性 
	if(DR_OK != mftFile.FindNoNameDataAttr(&ntfsAttr))
	{//获取无名数据属性失败
		return DR_INNER_ERR;
	}

	//获得数据运行
	if(DR_OK != run.InitRunList(&ntfsAttr))
		return DR_INNER_ERR;

	mMftBlockCnt = DWORD(run.mRunList.size());  //快数量
	mPMftBlock = (PMFT_BLOCK)malloc(sizeof(MFT_BLOCK) * mMftBlockCnt);  //空间分配
	memset(mPMftBlock , 0 , sizeof(MFT_BLOCK) * mMftBlockCnt);

	for( i = 0 , liStartMft.QuadPart = 0 ;
		i < mMftBlockCnt ; ++i )
	{
		//起始记录号
		mPMftBlock[i].liStartMft = liStartMft;

		//起始扇区号
		mPMftBlock[i].liStartSector.QuadPart 
			= run.mRunList[i].lcn.QuadPart * mSecPerClu;
		
		//记录数
		mPMftBlock[i].liMftCnt.QuadPart
			= (run.mRunList[i].clustCnt.QuadPart * mSecPerClu) / SECTOR_PER_RECODE;

		//写一个MFT记录块的起始记录号
		liStartMft.QuadPart += mPMftBlock[i].liMftCnt.QuadPart;
	}

	return DR_OK;
}

BOOL DNtfs::IsDevOpened()
{
	return !mDevName.empty();
}

void DNtfs::CloseDev()
{
	//释放名字空间
	if (!mDevName.empty())
	{
		mDevName.clear();
		mFSOff.QuadPart = 0;
		CloseHandle(mDev);
		mDev = INVALID_HANDLE_VALUE;
	}

	if (NULL != mPMftBlock)
	{
		free(mPMftBlock);
		mMftBlockCnt = 0;
		mPMftBlock = NULL;
	}
}

DRES DNtfs::ReadData(void* buf , PLONG_INT off , DWORD dwReadCnt , BOOL isOffSec)
{
	DRES	 res = DR_OK;
	DWORD	 dwReaded = 0;
	LONG_INT offset;

	//设备根本就没有打开
	if (mDevName.empty())
		return DR_NO_OPEN;

	if (isOffSec) //扇区偏移
	{
		if (off->QuadPart >= mAllSec.QuadPart)
			return DR_DEV_CTRL_ERR; //越界了

		offset.QuadPart = mFSOff.QuadPart + off->QuadPart;	//读取数据的实际偏移
		offset.QuadPart *= SECTOR_SIZE;		//字节偏移
	}
	else //字节偏移
	{
		if (off->QuadPart / SECTOR_SIZE >= mAllSec.QuadPart)
			return DR_DEV_CTRL_ERR; //越界了

		offset.QuadPart = mFSOff.QuadPart * SECTOR_SIZE;
		offset.QuadPart += off->QuadPart;
	}

	//设置文件指针
	offset.LowPart = SetFilePointer(mDev , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
	if (offset.LowPart == -1 && GetLastError() != NO_ERROR )
		res = DR_DEV_CTRL_ERR;

	//读取数据
	if(!res && !::ReadFile(mDev , buf , dwReadCnt ,&dwReaded ,NULL) && dwReaded != dwReadCnt)	
		res =  DR_DEV_IO_ERR;

/*	CloseHandle(hDev);								//关闭已经打开的设备*/
	return res;
}

DRES DNtfs::ReadMFT(void* buf , PLONG_INT index)
{
	//先计算一下偏移吧
 	LONG_INT off;

	if (index->QuadPart != 0)
	{//只有第一个文件需要查找，否则不需要查找
		off =GetSectorOfMFTRecode(*index);
	}else
	{
		off = mCluForMFT;
		off.QuadPart *= mSecPerClu;	//MFT其实扇区号
/*		off.QuadPart += index->QuadPart * RECODE_PER_SEC;*/
	}
	
	//读取指定的数据
	return ReadData(buf , &off , MFT_RECODE_SIZE , TRUE);
}

int DNtfs::FileNameCmp(const WCHAR * src1,const WCHAR * src2 , int len , BOOL caseSensitiv)
{
	int i = 0;

	if (caseSensitiv)
	{
		for (i = 0 ;  i < len ; ++i)
			if (src1[i] == src2[i]) continue;  //当前字符相等
			else return  src1[i] - src2[i];       //当前字符不相等
	}else{
		WCHAR w1 ,w2;

		for (i = 0 ;  i < len ; ++i){
			w1 = WchrToUpper(src1[i]);
			w2 = WchrToUpper(src2[i]);
			if (w1 == w2) continue;  //当前字符相等
			else return  w1 - w2;       //当前字符不相等
		}
	}

	return 0;
}

DRES DNtfs::OpenFileW(DNtfsFile *file , LONG_INT idx)
{
	if (!file)
		return DR_INVALED_PARAM;

	//设备根本就没有打开
	if (mDevName.empty())
		return DR_NO_OPEN;

	return file->InitRecode(this , idx);
}

DRES DNtfs::OpenFileW(DNtfsFile *file , int idx)
{
	LONG_INT i;
	i.QuadPart = idx;
	return OpenFileW( file , i);
}
/*

DRES DNtfs::OpenFileA(const char* path , DNtfsFile *file/ * , DWORD attr* / / *= ATTR_NTFS_MASK* /)
{
	WCHAR wPath[MAX_PATH + 1] = {0};
	if (path == NULL)return DR_INVALED_PARAM;

	//编码转换
	MultyByteToUnic(path , wPath , MAX_PATH + 1);

	//调用Unicode接口
	return OpenFileW(wPath , file / *, attr* /);
}
*/

DRES DNtfs::OpenFileW(const WCHAR* path , DNtfsFile *file /*,DWORD attr*/)
{
	size_t		nameLen		= 0;	//文件名的总长度
	DWORD		nameSegLen	= 0;	//文件名的一部分的长度
	const WCHAR*  name		= path; //当前文件名
	DWORD		i			= 0;
/*	DWORD		attrT		= attr;	//临时的属性值*/
	LONG_INT	mftIdx		= {0};
	DRES		res			= DR_OK;//各个操作结果

	LONG_INT	liFDTOff	= {0};
	WORD		fdtLen		= 0;

	//文件系统是否已经打开了?
	if (mDevName.empty())
		return DR_NO_OPEN;

	//安检
	if (!path || ! file /*|| !attr*/)
		return DR_INVALED_PARAM;
	
	nameLen = wcslen(path);//文件名的总长度
	name    = path; //当前文件名
/*	attrT   = attr;			//临时的属性值*/

// 	//是目录的话
// 	if (attr & ATTR_DIRECTORY)
// 		attrT |= ATTR_DIRECTORY_INDEX;
	
	//根目录
	mftIdx.QuadPart = SYS_FILE_ROOT;

	while(name[0]){
		//遍历文件路径的每一段
		for(i = 0 ; name[0] ;++i ){
			if(IsPathSeparator(name[i]))  ++name;//去掉文件名前面的分隔符额
			else  break;		
		}
		//查找完毕
		if (!name[0] )  break;

		//计算当前路径段的长度
		for (nameSegLen = 0 ; 
			!IsPathSeparator(name[nameSegLen]) && name[nameSegLen] ;
			++nameSegLen);

/*		if (name[nameSegLen]) attrT = attr | ATTR_DIRECTORY |ATTR_DIRECTORY_INDEX;//是问假名*/

		//在目录查找文件
		//遍历所有的vcn对应的block
		//res = FindItemByName(mftIdx , name ,nameSegLen , &mftIdx ,  attrT);
		//收索b+树
		res = FindItemByName2(mftIdx , name ,nameSegLen , &mftIdx  , &liFDTOff , &fdtLen);
		//出错了
		if (res != DR_OK) return res;
		name += nameSegLen;
	}
	res = OpenFileW(file ,mftIdx);
	if (DR_OK == res)
	{
		file->mLIStartFDT = liFDTOff;
		file->mFDTLen = fdtLen;
	}
	
	return res ;
}

DRES DNtfs::FindItemByName(LONG_INT dir , const  WCHAR* name, int len , PLONG_INT mftIdx , DWORD attr)
{
 	DRES			res			= DR_OK;
	LONG_INT		parentMft	= {0};	//被查找的目录的mft记录号
 	DWORD			blockSize	= 0;
 	BYTE*			blockEnd	= 0;
	INDEX_ENTRY*	indexEntry	= NULL;
	LONG_INT		vcn			= {0};	//虚拟簇号
	LONG_INT		lcn			= {0};	//逻辑簇号
	std::vector<BYTE> indexBlockBuf;//索引缓存区的其实地址
 	PFILE_NAME		fn			= NULL; //文件属性指针
	DNtfsFile		root;
 	DNtfsAttr		attrRoot;
 	DNtfsAttr		attrAllocation;
	DNtfsAttr		attrBitMp;		//为土属性


	//安检
	if (!name || len <= 0 || len > 255 || !mftIdx) return DR_INVALED_PARAM; 

	if (dir.QuadPart == -1)  //在指定的目录中查找
		parentMft.QuadPart = SYS_FILE_ROOT;
	else
		parentMft = dir;


	//获得指定的附文件记录
	res = OpenFileW(&root , parentMft);
	
	//获得跟属性
	res = root.FindAttribute(AD_INDEX_ROOT , &attrRoot);
	//出问题
	if (res != DR_OK)	return DR_INIT_ERR;
	//索引块的大小
	blockSize = attrRoot.IRGetIndexBlockSize();
	//索引表的第一个入口的地址
	indexEntry = PINDEX_ENTRY(attrRoot.IRGetFistEntry());
	//索引表的结束位置
	blockEnd = (BYTE*)indexEntry + attrRoot.IRGetAlloIndexEntriesSize();

	//遍历整个索引表
	for (;(BYTE*)indexEntry != blockEnd 
		;indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size))
	{
		if ((BYTE*)indexEntry + 0x10 > blockEnd)//出错了 0x10是INDEX_RNTRY头部的大小
		{
			root.Close();
			return  DR_INIT_ERR;
		}
		//标记了最后入口的Entry不会含有名字.也不会有孩子节点的指针
		if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //到了结尾了
			break;  //跳出后开始检查vcn

		fn = PFILE_NAME(indexEntry->IE_Stream);  //获取文件名属性
		if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;			//文件名的长度不匹配
		if (!FileNameCmp(fn->FN_FileName ,  name , len , FALSE ))
		{//找到了
			mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16)>>16; //去掉高两个字节的mft序列号
			root.Close();
			return DR_OK;   //找到了就直接返回
		}else
			continue;
	}

	//获得相应的属性
	res = root.FindAttribute(AD_BITMAP , &attrBitMp);
	if (res == DR_NO) return DR_NO_FILE;  //没有位图属性   	
	res = root.FindAttribute(AD_INDEX_ALLOCATION , &attrAllocation);
	if (res == DR_NO) return DR_NO_FILE;  //没有vcn->lcn映射表属性
	
	root.Close();		//不需要用了


	//先申请一块索引缓存区的缓存
	indexBlockBuf.resize(blockSize, 0);

	int bitCnt = attrBitMp.R_GetAttrLen() * 8;  //bitmap中的bit数
	vcn.QuadPart = 0;//其实vcn

	//遍历整个B+树
	while (1)
	{
		//获得下一个vcn
		for( ; vcn.QuadPart < bitCnt ; ++vcn.QuadPart) {
			if (attrBitMp.BMIsBitSet(vcn , this))
				break;
		}
		if (vcn.QuadPart == bitCnt){
			res = DR_NO_FILE;//没有找到文件		
			break;
		}

		//获得逻辑簇号
		lcn = root.GetLCNByVCN(vcn , NULL);
		//lcn = attrAllocation.IAGetLCNByVCN(&vcn , NULL);
		lcn.QuadPart *= mSecPerClu; 
		ReadData(indexBlockBuf.data(), &lcn, blockSize, TRUE);//读取指定的数据

		//线恢复usa
		PINDEX_BLOCK_HEAD ibh = PINDEX_BLOCK_HEAD(indexBlockBuf.data());

		//获得usa的数量  
		int		usaCnt = ibh->IB_USNSz ;
		WORD	us	   = 0;
		DWORD	offUsa = ibh->IB_USOff;
		int		i	   = 0;
		DWORD	usn = GetWORD(indexBlockBuf.data() + offUsa);

		for (i = 1 ; i < usaCnt ; ++i){
			us = GetWORD(indexBlockBuf.data() + offUsa + i * 2);
			SetWORD(indexBlockBuf.data() + 512 * i - 2, us);
		}

		//对应虚拟簇号
		if (vcn.QuadPart != ibh->IB_VCN.QuadPart)
		{//虚拟簇号不匹配
			res = DR_INIT_ERR;
			break;
		}

		//索引表的第一个入口的地址
		indexEntry = PINDEX_ENTRY((BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_EntryOff);
		//索引表的结束位置
		blockEnd = (BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_AllocSize;

		//遍历整个索引表
		for (;(BYTE*)indexEntry != blockEnd ; 
			indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size))
		{
			if ((BYTE*)indexEntry + 0x10 > blockEnd)
			{//出错了 0x10是INDEX_RNTRY头部的大小
				res =  DR_INIT_ERR;
				break;
			}

			//标记了最后入口的Entry不会含有名字.也不会有孩子节点的指针
			if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //到了结尾了
				break;  //跳出后开始检查vcn

			fn = PFILE_NAME(indexEntry->IE_Stream);
			//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
			if (!FileNameCmp(fn->FN_FileName ,  name , len , FALSE ))
			{//找到了
				if(!(fn->FN_DOSAttr & attr)) 		continue;	//属性不服
				mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16)>>16; //去掉高两个字节的mft序列号
				return DR_OK;   //找到了就直接返回
			}else
				continue;
		}

		//下一个簇号
		++vcn.QuadPart;
	}

	return res;
}

BYTE DNtfs::GetSecPerClust()
{
	return mSecPerClu;
}

LONG_INT DNtfs::GetSecCount()
{
	return mAllSec;
}

DRES DNtfs::GetVolumeName(WCHAR * nameBuf , DWORD bufLen)
{
	//安检
	if (!nameBuf)
		return DR_INVALED_PARAM;

	//文件系统是否已经打开了?
	if (mDevName.empty())
		return DR_NO_OPEN;

	DNtfsFile file;
	//打开卷文件
	DRES res = OpenFileW(&file, SYS_FILE_VOLUME);
	if (res != DR_OK)
		return res;

	DNtfsAttr attr;
	//查找文件名属性
	res = file.FindAttribute(AD_VOLUME_NAME , &attr);
	if (res != DR_OK)
	{
		file.Close();
		return DR_NO_FILE_NAME;
	}

	//获得名字的字符数
	DWORD len = attr.R_GetAttrLen();
	if (bufLen <= len) //缓存不够
	{
		file.Close();
		return DR_BUF_OVER;
	}

	//拷贝文件名
	memcpy(nameBuf , attr.R_GetAttrBodyPtr() , len);
	nameBuf[len/2] = 0;

	//收尾的时候记得要关闭已经打开的文件
	file.Close();
	return res;
}

DRES DNtfs::FindItemByName2(LONG_INT dir , const  WCHAR* name, int len , PLONG_INT mftIdx , PLONG_INT pLIStartFDT , WORD* fdtLen/* , DWORD attr*/)
{
	//安检
	if (!name || len <= 0 || len > 255 || !mftIdx)
		return DR_INVALED_PARAM;

	DRES		res = DR_OK;
	LONG_INT	parentMft;	//被查找的目录的mft记录号
	DNtfsFile	root;
	DNtfsAttr	attrRoot;
	LONG_INT	vcn;
	WCHAR		nameBuf[255] = {0};
	BYTE*		blockEnd = 0;
	INDEX_ENTRY* indexEntry;
	PFILE_NAME	fn = NULL;			//文件属性指针
	DNtfsFile::PAttrItem pAttrItem;	//文件属性

	if (dir.QuadPart == -1)  //在指定的目录中查找
		parentMft.QuadPart = SYS_FILE_ROOT;
	else
		parentMft = dir;

	//获得被查找的目录
	res = OpenFileW(&root , parentMft);
	if (res != DR_OK) return res;  //查找文件失败了
	if (!root.IsDir()){//这不是一个目录
		root.Close();
		return DR_IS_FILE;
	}
	//获得跟属性
	pAttrItem = root.FindAttribute(AD_INDEX_ROOT);
	//res = root.FindAttribute(AD_INDEX_ROOT , &attrRoot);
	if (NULL == pAttrItem){//查找属性失败
		root.Close();
		return DR_INIT_ERR;
	}
	attrRoot.InitAttr(pAttrItem->attrDataBuf.data());
	

	//索引表的第一个入口的地址
	indexEntry = PINDEX_ENTRY(attrRoot.IRGetFistEntry());
	//索引表的结束位置
	blockEnd = (BYTE*)indexEntry + attrRoot.IRGetAlloIndexEntriesSize();

	//遍历整个索引表
	for (;(BYTE*)indexEntry < blockEnd 
		;indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size) )
	{
		if (indexEntry->IE_Flags & INDEX_ENTRY_NODE) {//是一个子节点
			
			if (!(indexEntry->IE_Flags & INDEX_ENTRY_END))
			{//当前索引区没有结束  需要和当前的节点比较
				fn = PFILE_NAME(indexEntry->IE_Stream);  //获取文件名属性
				memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
				nameBuf[fn->FN_NameSize] = 0;

				//和文件名进行比较
				//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
				int flag = FileNameCmp(nameBuf , name , len  , FALSE);
				if (flag == 0)
				{ //就是当前的节点
/*					if(!(fn->FN_DOSAttr & attr)) 		continue;	//属性不服*/
					mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;
					
					if (NULL != pLIStartFDT)
					{//计算FDT的起始位置

						//计算当前MFT的偏移
						pLIStartFDT->QuadPart = GetSectorOfMFTRecode(parentMft).QuadPart * SECTOR_SIZE;
						//当前MFT的INDEX_ROOT属性
						pLIStartFDT->QuadPart += pAttrItem->off;
						//INDEX_ROOT中的当前入口的偏移
						pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - pAttrItem->attrDataBuf.data());
						
						//当前FDT的长度
						if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
					}

					root.Close();
					return DR_OK;
				}else if (flag > 0)
				{//在这里进入vcn
					//获得索引表中的空表项所设定的虚拟簇号
					vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
					res = WalkNode(&root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
					if (res == DR_OK){ //查找成功 
						root.Close();					
						return DR_OK;   
					}    
					else if (res == DR_NO) //当前block没有找到
						continue;
					else					//出错了
						break;

				}else  //flag < 0 
				{//这里不可进入  下一个可进入
					continue;
				}
			}else
			{//已是最后一个indexEntry ，但是有vcn
				//获得索引表中的空表项所设定的虚拟簇号
				vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
				res = WalkNode(&root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
				root.Close();
				return res;//最后一个不管是白与否直接跳出
			}
		}

		if (indexEntry->IE_Flags & INDEX_ENTRY_END) { //到了结尾了	
			res = DR_NO;
			break;  //跳出后开始检查vcn
		}
		
		//如果执行到这里了的话就表示这里是一个普通的节点
		fn = PFILE_NAME(indexEntry->IE_Stream);  //获取文件名属性
		memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
		nameBuf[fn->FN_NameSize] = 0;
		//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
		if (!FileNameCmp(nameBuf , name  , len , FALSE))
		{//找到了
/*			if(!(fn->FN_DOSAttr & attr)) 		continue;	//属性不服*/
			mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;

			if (NULL != pLIStartFDT)
			{//计算FDT的起始位置

				//计算当前MFT的偏移
				pLIStartFDT->QuadPart = GetSectorOfMFTRecode(parentMft).QuadPart * SECTOR_SIZE;
				//当前MFT的INDEX_ROOT属性
				pLIStartFDT->QuadPart += pAttrItem->off;
				//INDEX_ROOT中的当前入口的偏移
				pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - pAttrItem->attrDataBuf.data());

				//当前FDT的长度
				if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
			}

			root.Close();
			return DR_OK;
		}
	}
	root.Close();
	return res;
}

DRES DNtfs::WalkNode(DNtfsFile* root , LONG_INT vcn , const  WCHAR* name, int len , PLONG_INT mftIdx  , PLONG_INT pLIStartFDT , WORD* fdtLen/* , DWORD attr*/)
{
	LONG_INT	lcn			= {0};//逻辑簇号
	DRES		res			= DR_NO;
	DNtfsAttr	attrAllocation;
	DNtfsAttr	attrRoot;
	INDEX_ENTRY* indexEntry	= NULL;
	BYTE*		blockEnd	= 0;
	PFILE_NAME	fn			= NULL;        //文件属性指针
	WCHAR		nameBuf[255] = {0};

	//获得跟属性
	res = root->FindAttribute(AD_INDEX_ROOT , &attrRoot);
	//出问题
	if (res != DR_OK)	return DR_INIT_ERR;

	//获得INDEX_ALLOCATION属性
	res = root->FindAttribute( AD_INDEX_ALLOCATION, &attrAllocation);
	//错问题了
	if (res != DR_OK)	return DR_INIT_ERR;  //内部错误

	//索引块的大小
	DWORD blockSize = attrRoot.IRGetIndexBlockSize();

	//先申请一块索引缓存区的缓存
	std::vector<BYTE> indexBlockBuf(blockSize, 0);

	//获得逻辑簇号
	lcn = root->GetLCNByVCN(vcn , NULL);
	//lcn = attrAllocation.IAGetLCNByVCN(&vcn , NULL);
	lcn.QuadPart *= mSecPerClu; 
	ReadData(indexBlockBuf.data(), &lcn, blockSize, TRUE);
	
	//先保存一下index_block的物理位置
	if (NULL != pLIStartFDT)
		pLIStartFDT->QuadPart = lcn.QuadPart * SECTOR_SIZE;

	//线恢复usa
	PINDEX_BLOCK_HEAD ibh = PINDEX_BLOCK_HEAD(indexBlockBuf.data());

	//获得usa的数量  
	int		usaCnt	= ibh->IB_USNSz ;
	WORD	us		= 0;
	DWORD	offUsa	=  ibh->IB_USOff;
	int		i		= 0;
	DWORD	tusn, usn = GetWORD(indexBlockBuf.data() + offUsa);
	for (i = 1 ; i < usaCnt ; ++i)
	{
		us = GetWORD(indexBlockBuf.data() + offUsa + i * 2);
		tusn = GetWORD(indexBlockBuf.data() + 512 * i - 2);
		SetWORD(indexBlockBuf.data() + 512 * i - 2, us);
	}

	if (vcn.QuadPart != ibh->IB_VCN.QuadPart)
	{
		//虚拟簇号不匹配
		return DR_INIT_ERR;
	}

	//索引表的第一个入口的地址
	indexEntry = PINDEX_ENTRY((BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_EntryOff);
	//索引表的结束位置
	blockEnd = (BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_AllocSize;

	//遍历整个索引表
	for (;(BYTE*)indexEntry < blockEnd ; 
		indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size) )
	{
		//标记了最后入口的Entry不会含有名字.也不会有孩子节点的指针
		if (indexEntry->IE_Flags & INDEX_ENTRY_NODE) {//是一个子节点

			if (!(indexEntry->IE_Flags & INDEX_ENTRY_END))
			{//当前索引区没有结束  需要和当前的节点比较
				fn = PFILE_NAME(indexEntry->IE_Stream);  //获取文件名属性
				memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
				nameBuf[fn->FN_NameSize] = 0;

				//和文件名进行比较
				//if(fn->FN_NameSize != len || !(fn->FN_DOSAttr & attr)) 		continue;	
				int flag = FileNameCmp(nameBuf , name  ,len , FALSE);
				if (flag == 0)
				{ //就是当前的节点
/*					if(!(fn->FN_DOSAttr & attr)) 		continue;	//属性不服*/
					mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;

					if (NULL != pLIStartFDT)
					{//计算FDT的起始位置

						//INDEX_ROOT中的当前入口的偏移
						pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - indexBlockBuf.data());

						//当前FDT的长度
						if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
					}

					res = DR_OK;
					break;
				}else if (flag > 0)
				{//在这里进入vcn
					//获得索引表中的空表项所设定的虚拟簇号
					vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
					res = WalkNode(root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
					if (res /*!=*/== DR_NO)
						continue;
					else //找到或者出错了
						break;
				}else  //flag < 0 
				{//这里不可进入  需要查下一个
					continue;
				}
			}else//最后的一个节点  需要跳出去
			{
				vcn = *PLONG_INT((BYTE*)indexEntry + indexEntry->IE_Size - 8);
				res = WalkNode(root  , vcn , name , len , mftIdx , pLIStartFDT , fdtLen);
				break;
			}
		}

		if (indexEntry->IE_Flags & INDEX_ENTRY_END) { //到了结尾了	
			res = DR_NO;
			break;  //跳出后开始检查vcn
		}

		//如果执行到这里了的话就表示这里是一个普通的节点
		fn = PFILE_NAME(indexEntry->IE_Stream);  //获取文件名属性
		memcpy(nameBuf , fn->FN_FileName , fn->FN_NameSize*2);
		nameBuf[fn->FN_NameSize] = 0;
		if(fn->FN_NameSize != len /*|| !(fn->FN_DOSAttr & attr)*/) 		continue;	
		if (!FileNameCmp(nameBuf , name , len  , FALSE))
		{//找到了
			mftIdx->QuadPart = (indexEntry->IE_FR.QuadPart << 16 )>> 16;

			if (NULL != pLIStartFDT)
			{//计算FDT的起始位置

				//INDEX_ROOT中的当前入口的偏移
				pLIStartFDT->QuadPart += (((BYTE*)indexEntry) - indexBlockBuf.data());

				//当前FDT的长度
				if (NULL != fdtLen) *fdtLen = indexEntry->IE_Size;
			}

			res = DR_OK;
			break;
		}
	}

	return res;
}

DRES DNtfs::FindFile(DNtfsFile* root, FINDER* hFin)
{
	*hFin = NULL;

	//设备根本就没有打开
	if (mDevName.empty())
		return DR_NO_OPEN;

	//安检
	if (root == NULL || hFin == NULL)
		return DR_INVALED_PARAM;
	if (!root->IsDir())
		return DR_IS_FILE;	//需要的是目录

	//内存分配
	std::unique_ptr<FIND_FILE_HANDER> hFind(new FIND_FILE_HANDER());
	hFind->vcn.QuadPart = -1;
	hFind->index = 0;

	//打开指定的文件目录
	DRES res = OpenFileW(&hFind->dir, root->GetMftIndex());
	//无效的路径
	if (res == DR_NO)
		return DR_INVALID_NAME;
	if (res != DR_OK)
		return DR_INIT_ERR;		//其他的初始化错误或者内部错粗

	//要返回的句柄
	*hFin = FINDER(hFind.release());
	
	return DR_OK;
}

DRES DNtfs::FindFile(const char* root, FINDER* hFind)
{
	WCHAR wPath[MAX_PATH + 1] = {0};
	//安检
	if (root == NULL || hFind == NULL) return DR_INVALED_PARAM;

	MultyByteToUnic(root, wPath, MAX_PATH + 1);

	return FindFile(wPath, hFind);
}

DRES DNtfs::FindFile(const WCHAR* path , FINDER* hFin /*,PLONG_INT mftIndx*/)
{
	*hFin = NULL;

	//安检
	if (!path || !hFin /*|| !mftIndx */)
		return DR_INVALED_PARAM;

	if (mDevName.empty())
		return DR_NO_OPEN;		//系统海眉头初始化

	//清零查找句柄
	std::unique_ptr<FIND_FILE_HANDER> hFind(new FIND_FILE_HANDER());
	hFind->vcn.QuadPart = -1;
	hFind->index = 0;

	//打开指定的文件目录
	DRES res = OpenFileW(path, &hFind->dir /*, ATTR_DIRECTORY|ATTR_DIRECTORY_INDEX*/);
	//无效的路径
	if (res == DR_NO)
		return DR_INVALID_NAME;
	if (res != DR_OK)
		return DR_INIT_ERR;		//其他的初始化错误或者内部错粗

	//要返回的句柄
	*hFin = FINDER(hFind.release());

	return res;
}

DRES DNtfs::FindNext(/*PFIND_FILE_HANDER*/FINDER hFin ,PLONG_INT mftIndx)
{
	//一个入口的其起始位置
	PINDEX_ENTRY	indexEntry;
	BYTE*			blockEnd;
	PFILE_NAME      fn;
	DNtfsAttr		attrRoot;
	DNtfsAttr		attrBitMp;
	//NtfsAttr		attrAllocation;
	DRES			res;
	int				index;
	int				bitCnt; 
	std::vector<BYTE> indexBlockBuf;
	DWORD			blockSize;
	LONG_INT		lcn;
	PFIND_FILE_HANDER hFind = (PFIND_FILE_HANDER)hFin;

	//检查是否已经初始化了文件洗系统
	if (mDevName.empty())
		return DR_NO_OPEN;
	if (!hFind || !mftIndx)
		return DR_INVALED_PARAM;
	if (!hFind->dir.IsFileValid())
		return DR_INVALID_HANDLE;//无效的查找句柄

	if (!hFind || !mftIndx)
		return DR_INVALED_PARAM;

	if (!hFind->dir.IsFileValid())
		return DR_INVALID_HANDLE;//无效的查找句柄

	//获得跟属性
	res = hFind->dir.FindAttribute(AD_INDEX_ROOT , &attrRoot);
	//出问题
	if (res != DR_OK)
		return DR_INIT_ERR;

	//索引表的第一个入口的地址
	indexEntry = PINDEX_ENTRY(attrRoot.IRGetFistEntry());
	//索引表的结束位置
	blockEnd = (BYTE*)indexEntry + attrRoot.IRGetAlloIndexEntriesSize();

	//遍历整个索引表
	if (hFind->vcn.QuadPart == -1)//mft记录中查找
	{
		for (index = 0;(BYTE*)indexEntry < blockEnd
			;indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size) 
			, ++index )
		{
			if ((BYTE*)indexEntry + 0x10 > blockEnd || 
				(BYTE*)indexEntry + indexEntry->IE_DataSize > blockEnd)
			{//超出了缓存区
				return DR_INIT_ERR;
			}
			//标记了最后入口的Entry不会含有名字.也不会有孩子节点的指针
			if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //到了结尾了
				break;  //跳出后开始检查vcn

			//还没到指定的位置
			if (index < hFind->index) continue;

			fn = PFILE_NAME(indexEntry->IE_Stream);  //获取文件名属性
			if (fn->FN_NamSpace == NS_DOS)//不要DOS的名字
			{//指定的位置的入口时一个dos入口
				++hFind->index;
				continue;
			}
			//找到了
			mftIndx->QuadPart = indexEntry->IE_FR.QuadPart << 16 >> 16;
			++hFind->index;//下一个查找的目录
			return DR_OK;
		}
		
		//mft记录中没有数据
		res = hFind->dir.FindAttribute(AD_BITMAP , &attrBitMp);
		if (res == DR_NO)
		{//没有Bitmap属性 ,也就是查找完了
			return DR_FAT_EOF;
		}

		//获得第一个vcn
		bitCnt = attrBitMp.R_GetAttrLen() * 8;  //bitmap中的bit数
		hFind->vcn.QuadPart = 0;
		for(; hFind->vcn.QuadPart < bitCnt ; ++hFind->vcn.QuadPart)
		{
			if (attrBitMp.BMIsBitSet(hFind->vcn , this))
				break;
		}
		if (hFind->vcn.QuadPart == bitCnt)
			return DR_FAT_EOF;//没有找到有效的vcn
		else
			hFind->index = 0;//找到了第一个vcn
	}
	
	//rootIndex中已经找过了,接下来找indexBlock区域

	//获得缓存区的大小
	blockSize = attrRoot.IRGetIndexBlockSize();
	//先申请一块索引缓存区的缓存
	indexBlockBuf.resize(blockSize, 0);

	//获得位图属性
	res = hFind->dir.FindAttribute(AD_BITMAP , &attrBitMp);
	if (res == DR_NO) return DR_INIT_ERR;  //没有位图属性   
	//获得vcn=》lvn 对照表属性
// 	res = hFind->dir.FindAttribute(AD_INDEX_ALLOCATION , &attrAllocation);
// 	if (res == DR_NO) return DR_INIT_ERR;  //没有vcn->lcn映射表属性


	//遍历整个B+树
	while (1)
	{
		//获得逻辑簇号
		lcn = hFind->dir.GetLCNByVCN(hFind->vcn , NULL);
		//lcn = attrAllocation.IAGetLCNByVCN(&hFind->vcn , NULL);
		lcn.QuadPart *= mSecPerClu; 
		ReadData(indexBlockBuf.data(), &lcn, blockSize, TRUE);//读取指定的数据

		//线恢复usa
		PINDEX_BLOCK_HEAD ibh = PINDEX_BLOCK_HEAD(indexBlockBuf.data());

		//获得usa的数量  
		int		usaCnt = ibh->IB_USNSz ;
		WORD	us = 0;
		DWORD	offUsa =  ibh->IB_USOff;
		int		i = 0;
		DWORD	usn = GetWORD(indexBlockBuf.data() + offUsa);
		for (i = 1 ; i < usaCnt ; ++i){
			us = GetWORD(indexBlockBuf.data() + offUsa + i * 2);
			SetWORD(indexBlockBuf.data() + 512 * i - 2, us);
		}

		//对应虚拟簇号
		if (hFind->vcn.QuadPart != ibh->IB_VCN.QuadPart)
		{//虚拟簇号不匹配
			res = DR_INIT_ERR;
			break;
		}

		//索引表的第一个入口的地址
		indexEntry = PINDEX_ENTRY((BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_EntryOff);
		//索引表的结束位置
		blockEnd = (BYTE*)&ibh->IB_indexHead + ibh->IB_indexHead.IH_AllocSize;

		index = 0;///缓存区中的索引
		//遍历整个索引表
		for (;(BYTE*)indexEntry != blockEnd ; 
			indexEntry = PINDEX_ENTRY((BYTE*)indexEntry + indexEntry->IE_Size)
			,++index)
		{
			if ((BYTE*)indexEntry + 0x10 > blockEnd || 
				(BYTE*)indexEntry + indexEntry->IE_DataSize > blockEnd)
			{//出错了 0x10是INDEX_RNTRY头部的大小
				res =  DR_INIT_ERR;
				break;
			}

			//标记了最后入口的Entry不会含有名字.也不会有孩子节点的指针
			if (indexEntry->IE_Flags & INDEX_ENTRY_END)  //到了结尾了,需要检查下一个vcn
				break;  //跳出后开始检查vcn

			//还没到指定的位置
			if (index < hFind->index) continue;

			fn = PFILE_NAME(indexEntry->IE_Stream);  //获取文件名属性
			if (fn->FN_NamSpace == NS_DOS)//不要纯DOS的名字
			{//指定的位置的入口时一个dos入口
				++hFind->index;
				continue;
			}
			//找到了
			mftIndx->QuadPart = indexEntry->IE_FR.QuadPart << 16 >> 16;
			++hFind->index;//下一个查找的目录
			res = DR_OK;
			break;
		}

		if(indexEntry->IE_Flags & INDEX_ENTRY_END)//是应为碰到了indexEntry的结束才跳出来的
		{
			//下一个簇号
			++hFind->vcn.QuadPart;
			hFind->index = index =0;

			bitCnt = attrBitMp.R_GetAttrLen() * 8;  //bitmap中的bit数
			for(; hFind->vcn.QuadPart < bitCnt ; ++hFind->vcn.QuadPart) {
				if (attrBitMp.BMIsBitSet(hFind->vcn , this))
					break;
			}
			if (hFind->vcn.QuadPart == bitCnt)
			{//没有了有效的vcn
				res = DR_FAT_EOF;
				break;
			}
		}else
			break;
	}

	return res;
}

void DNtfs::CloseFind(/*PFIND_FILE_HANDER*/FINDER hFin)
{
	PFIND_FILE_HANDER  hFind =  (PFIND_FILE_HANDER)hFin;
	if (hFind && hFind->dir.IsFileValid())
	{
		hFind->dir.Close();
		delete hFind;
	}
}

DRES DNtfs::IsContainNTFSFlag(const WCHAR* cDevName, LONG_INT offset)
{
	//参数错误
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

	DRES		res = DR_OK;
	if (offset.QuadPart > 0)
	{//偏移
		//设置文件指针
		offset.LowPart = ::SetFilePointer(hDev , offset.LowPart , PLONG(&(offset.HighPart)) ,FILE_BEGIN );
		if (offset.LowPart == -1 && ::GetLastError() != NO_ERROR )
			res = DR_DEV_CTRL_ERR;
	}

	//读取数据
	NTFS_DBR	nDbr = { 0 };
	DWORD		dwReaded = 0;
	if(DR_OK == res && !::ReadFile(hDev , &nDbr , sizeof(NTFS_DBR) 
		,&dwReaded ,NULL) && dwReaded != sizeof(NTFS_DBR))	
		res =  DR_DEV_IO_ERR;
	
	//不需要了
	CloseHandle(hDev);
	//设备相关操作失败
	if (res != DR_OK) return res;

	//有NTFS的技术是没有公开的，所以很多的东西基本是大牛们猜+测的
	if ( ( nDbr.dbrEnd != MBR_END)				//DBR的结束标记
		||(nDbr.bpbBytePerSec != SECTOR_SIZE))	//扇区大小我之处里512的
		return DR_NO;

	//NTFS标记
	//4E 54 46 53 20  20 20 20
	if ( ( nDbr.dbrOemName[0] != 0x4E)	//N
		||(nDbr.dbrOemName[1] != 0x54)	//T
		||(nDbr.dbrOemName[2] != 0x46)	//F
		||(nDbr.dbrOemName[3] != 0x53)	//S
		||(nDbr.dbrOemName[4] != 0x20)	//空格
		||(nDbr.dbrOemName[5] != 0x20)	//空格
		||(nDbr.dbrOemName[6] != 0x20)	//空格
		||(nDbr.dbrOemName[7] != 0x20))	//空格
		return DR_NO;

	//可以跑到这里的话应该是没问题的
	return DR_OK;
}

const WCHAR* DNtfs::GetDevName()
{
	return mDevName.c_str();
}

LONG_INT DNtfs::GetClustForMFT()
{
	LONG_INT clust = {0};
	if (mDevName.empty())
		return clust;

	return mCluForMFT;
}

LONG_INT DNtfs::GetClustForMFTMirr()
{
	LONG_INT clust = {0};
	if (mDevName.empty())
		return clust;

	return mCluForMFTMirr;
}


LONG_INT DNtfs::GetSectorOfMFTRecode( LONG_INT mft )
{
#define _USE_MFT_BLOCK_ 1 

#if _USE_MFT_BLOCK_ == 0 //不使用MFT_BLOCK 来处理MFT记录
	LONG_INT	liSector = {-1};
	DNtfsFile	mftFile;//当前卷的MFT文件
	DRES		res = DR_OK;
	DNtfsFile::PAttrItem pAttrItem = NULL;
	DNtfsAttr	ntfsAttr;
	LONG_INT	vcn = {-1};
	DRun		run;

	//设备还没有打开的？
	if (NULL == mDevName) return liSector;
	res = OpenFileW(&mftFile , SYS_FILE_MFT);
	if (res != DR_OK) liSector;  //文件打开失败

	//查找位图属性
	pAttrItem = mftFile.FindAttribute(AD_BITMAP);
	if (NULL == pAttrItem) return liSector;//这种情况是不允许的

	//初始化属性
	ntfsAttr.InitAttr(pAttrItem->attrDataPtr);
	//判断指定的MFT是否在使用
	if(FALSE == ntfsAttr.BMIsBitSet(mft , this ))
		return liSector;//指定的文件记录不存在
	

	//获得无名数据属性 
	if(DR_OK != mftFile.FindNoNameDataAttr(&ntfsAttr))
	{//获取属性失败
		return liSector;
	}

	//获得数据运行
	if(DR_OK != run.InitRunList(&ntfsAttr))
		return liSector;

	//计算文件所在vcn
	vcn.QuadPart = (mft.QuadPart * SECTOR_PER_RECODE) / GetSecPerClust();

	//查询LCN
	vcn = run.GetLCNByVCN(vcn , NULL);
	run.Close();
	if (-1 == vcn.QuadPart)
		return liSector;

	//计算簇内扇区偏移
	liSector.QuadPart = (vcn.QuadPart * mSecPerClu) + ((mft.QuadPart * SECTOR_PER_RECODE) % mSecPerClu);

	//资源还是需要释放的
	mftFile.Close();
	return liSector;
#elif _USE_MFT_BLOCK_ == 1   //使用MFT_BLOCK 来处理MFT记录  2012-07-18 1:07
	LONG_INT	liSector = {-1};
	DRES		res = DR_OK;
	DNtfsAttr	ntfsAttr;
	DWORD		i;
	LONG_INT	curMft;	//在当前块的相对MFT记录号

	//设备还没有打开的？
	if (mDevName.empty())
		return liSector;
// 	res = OpenFileW(&mftFile , SYS_FILE_MFT);
// 	if (res != DR_OK) liSector;  //文件打开失败
//
// 	//查找位图属性
// 	pAttrItem = mftFile.FindAttribute(AD_BITMAP);
// 	if (NULL == pAttrItem) return liSector;//这种情况是不允许的
// 
// 	//初始化属性
// 	ntfsAttr.InitAttr(pAttrItem->attrDataPtr);
// 	//判断指定的MFT是否在使用
// 	if(FALSE == ntfsAttr.BMIsBitSet(mft , this ))
// 		return liSector;//指定的文件记录不存在


	//遍历每一个MFT_BLOCK
	for(i = 0 ; i < mMftBlockCnt; ++i)
	{
		if (( mPMftBlock[i].liStartMft.QuadPart <= mft.QuadPart ) &&
			( mPMftBlock[i].liStartMft.QuadPart + 
			  mPMftBlock[i].liMftCnt.QuadPart > mft.QuadPart ) )
		{//要查找的MFT扩在这一块了
			curMft.QuadPart = mft.QuadPart - mPMftBlock[i].liStartMft.QuadPart;
			liSector.QuadPart = mPMftBlock[i].liStartSector.QuadPart 
				+ curMft.QuadPart * SECTOR_PER_RECODE;
			return liSector;
		}
	}

	return liSector;
#endif
}

