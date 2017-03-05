/***********************************************************************
 * FileName:	NtfsFle.cpp
 * Author:		杨松
 * Created:		2012年4月20日 星期五
 * Purpose:		实现NtfsFile的类的方法
 * Comment:		这个类的主要用途是抽象一个Ntfs卷上的文件，提供了各种方法
 *
 * Modify:		处理一个BUG。当遇到一个run中有两个虚拟簇，而目录需要读取
 *				第二个IndexBlock时(IndexBlock一般是4K，在我的电脑刚好一簇)
 *				获得逻辑簇号实际是第一个虚拟簇号的逻辑簇号，在这里需要加上
 *				实际需要虚拟粗号与当前run起始虚拟粗号只差,而簇数则是相加
 *
 *				读取文件时，为了操作简单，每次都将文件读写指针移动到簇的边
 *				界,然后读取一簇的数据，在计算这一个簇中有多少有效数据（也就
 *				是当前缓存中的有效数据偏移），如果这一次读取还满意有完毕的
 *				话就那么接下来的读取就没有不需要计算缓存簇内有效数据偏移了，
 *				那个换成簇内偏移是以一个变量存在，所以第一次读取之后就必须
 *				置为0,否则会整个读取将会失败。2012-07-04 08:42
 *
 ***********************************************************************/

#include "disktool.h"
#include "stut_connf.h"

#pragma warning(disable:4996)


DNtfsFile::DNtfsFile()
	: mAttrCnt(0)
	, mFS(0)
	, mFDTLen(0)
{
	this->mFilePointer.QuadPart = 0;
	this->mMftIdx.QuadPart = -1;
	mLIStartFDT.QuadPart = 0;
}

DRES DNtfsFile::InitRecode(DNtfs* fs ,LONG_INT mftIndex)
{
	DRES res = DR_OK;
	BYTE mRecodeBuf[MFT_RECODE_SIZE];//记录缓存 1K
	PFILE_RECODE_HEAD pfrh;
	
	if (this->mFS)//文件已经初始化了
		return DR_ALREADY_OPENDED;

	//读取指定的mft记录
	res = fs->ReadMFT(mRecodeBuf , &mftIndex);
	
	if (res != DR_OK){//读取数据失败
		this->mMftIdx.QuadPart = -1;
		return res;
	}
	
	this->mFS = fs;

	//初始化文件记录头部
	pfrh = PFILE_RECODE_HEAD(mRecodeBuf);
	this->mMftHeadBuf.resize(pfrh->FR_1stAttrOff, 0);
	memcpy(this->mMftHeadBuf.data(), mRecodeBuf, pfrh->FR_1stAttrOff );

	//读取数据成功
	this->mMftIdx = mftIndex;	
	//this->head = PFILE_RECODE_HEAD(this->mRecodeBuf);
	
	//还原每个扇区末尾处的数据USA/USN
	WORD usa = GetUSAItem(0);
	SetWORD(mRecodeBuf + 510 , usa);
	usa = GetUSAItem(1);
	SetWORD(mRecodeBuf + 1022 , usa);

// 	if (this->mAttrArr)
// 	{//似乎有有内存没有释放  
// 		free(this->mAttrArr);
// 		this->mAttrArr = NULL;
// 		this->mAttrCnt = 0;
// 	}

	//初始化属性列表
	return InitAttrList((BYTE*)mRecodeBuf);
}
WORD DNtfsFile::GetUSAItem(int index)
{
	//跟心序列号的偏移
	WORD offUSN = PFILE_RECODE_HEAD(mMftHeadBuf.data())->FR_USOff;
	offUSN = GetWORD(mMftHeadBuf.data() + offUSN + 2 + index * 2);
	return offUSN;
}

LONG_INT DNtfsFile::GetMftIndex()
{
	return this->mMftIdx;
}
BOOL DNtfsFile::IsFileValid()
{
	return this->mFS != NULL;
}
LONG_INT DNtfsFile::GetParentMftIndex()
{
	LONG_INT parent;
	DNtfsAttr nameAttr;
	DRES	 res = DR_OK;

	parent.QuadPart = -1;

	//查找文件名记录号
	res = this->FindAttribute(AD_FILE_NAME , &nameAttr );

	if (res !=  DR_OK) return parent; //失败了
		
	parent.QuadPart = (nameAttr.FNGetParentMftIndx().QuadPart << 16)>>16;
	return parent;
}

LONG_INT DNtfsFile::GetCreateTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	
	//文件没有初始化
	if (!this->mFS) return tim;

	//获得标准信息
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //出错了

	//标准属性
	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_CreatTime.QuadPart / 10000000 - (__int64)NTFS_TIME_OFFSET;
	
	return tim;
}
LONG_INT DNtfsFile::GetAlteredTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	//文件没有初始化
	if (!this->mFS) return tim;

	//获得标准信息
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //出错了

	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_AlterTime.QuadPart / 10000000 - NTFS_TIME_OFFSET;

	return tim;
}
LONG_INT DNtfsFile::GetMFTChgTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	//文件没有初始化
	if (!this->mFS) return tim;

	//获得标准信息
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //出错了

	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_MFTChgTime.QuadPart / 10000000 - NTFS_TIME_OFFSET;

	return tim;
}
LONG_INT DNtfsFile::GetReadTime()
{
	LONG_INT tim = {0};
	DNtfsAttr attr;
	DRES	 res = DR_OK;
	PSTD_INFO psi = NULL;

	//文件没有初始化
	if (!this->mFS) return tim;

	//获得标准信息
	res = this->FindAttribute(AD_STANDARD_INFORMATION , &attr);
	if (res != DR_OK)  return tim;  //出错了

	psi = PSTD_INFO(attr.R_GetAttrBodyPtr());
	tim.QuadPart = psi->SI_ReadTime.QuadPart / 10000000 - NTFS_TIME_OFFSET;

	return tim;
}

BOOL DNtfsFile::IsDir()
{
	if (/*PFILE_RECODE_HEAD(this->mRecodeBuf)->FR_Flags == ATTR_DIRECTORY 
		|| */PFILE_RECODE_HEAD(mMftHeadBuf.data())->FR_Flags & 0x2
		)
		return TRUE;
	else 
		return FALSE;
}
DRES DNtfsFile::GetFileName(WCHAR* nameBuf , DWORD len , BYTE nameSpace)
{
	DNtfsAttr attr;
	DWORD	 atttIdx = 0;
	DWORD	 tAttr   = 0;
	DRES	 res     = DR_OK;
	DWORD	 nameLen = 0;
	
	//好像没初始化哦
	if (this->mFS == NULL) 	return DR_NO_OPEN;
	//检查参数
	if (!nameBuf ) return DR_INVALED_PARAM;
	wcscpy(nameBuf , L"");//先清空缓存



	if (nameSpace != NS__ALL) {
		while(1){
			res = FindAttribute(AD_FILE_NAME , &attr , &atttIdx);
			//没有指定文件名空间的文件名属性
			if (res != DR_OK)
				return DR_NO_FILE_NAME;

			if (attr.FNGetFileNameSpase() == nameSpace)
				break;//找到了相应的文件名
			++atttIdx;
		}
	}else{	//所有的都可以
		atttIdx = 0;
		tAttr = 0;

		while(1){
			res = FindAttribute(AD_FILE_NAME , &attr , &atttIdx);
			if (res == DR_NO)
				break;//没有指定的文件属性
			else if (res != DR_OK)
				return DR_NO_FILE_NAME;
			
			//先备份一下找到的文件名属性
			tAttr = atttIdx;

			if (attr.FNGetFileNameSpase() &/*==*/ NS_WIN32)  //不一定是必须是win32的命名空间 ，兼容就可以
				break;//找到了相应的文件名
			++atttIdx;
		}

		//仿佛一下的代码没有必要，一般的文件都支持WIN32
		//我好想还只发现NS_WIN32和NS_DOS_WIN32以及NS_DOS，
		//而且只要出现了NS_DOS很定会出现NS_WIN32或者NS_DOS_WIN32，
		//也就是说NS_DOS不会单独出现，但是下面的代码我没把握一定可以不要
		if (res == DR_NO && tAttr != 0)
		{//没有win32的,就任意的
// 			while(1){
// 				res = FindAttribute(AD_FILE_NAME , &attr , NULL);
// 				if (res != DR_OK)//没有文件名属性
// 					return DR_NO_FILE_NAME;
// 				else//找到了一盒文件名属性
// 					break;
// 			}
			res = FindAttribute(AD_FILE_NAME , &attr , &tAttr);
			if (res != DR_OK)//没有文件名属性
				return DR_NO_FILE_NAME;
// 			else//找到了一盒文件名属性
//  					break;
		}   
	}    

	//获得文件名的长度
	nameLen = attr.FNGetFileNameLen();

	//缓存区不够啊
	if (len < nameLen*2 +1)	return DR_BUF_OVER;
	attr.FNGetFileName(nameBuf);
	nameBuf[nameLen] = 0;

	return DR_OK;
}
DWORD DNtfsFile::GetDOSAttr()
{
	DNtfsAttr attr;
	if(DR_OK != this->FindAttribute(AD_FILE_NAME , &attr))
		return 0;
	else
		return PFILE_NAME(attr.R_GetAttrBodyPtr())->FN_DOSAttr;
}
DWORD DNtfsFile::GetAttrCount()
{
	return this->mAttrCnt;
}


DNtfsFile::PAttrItem DNtfsFile::GetAttr( DWORD index )
{
	if (NULL == this->mFS)
		return NULL; //仿佛这是一个无效的文件
	if ( mAttrCnt <= index)//索引越界?
		return NULL;

	return PAttrItem(mAttrArr.get()) + index;
}

DNtfsFile::PAttrItem DNtfsFile::FindAttribute( DWORD dwAttrType , const DWORD* startIdx /*= 0*/ )
{
	if (NULL == this->mFS)  //设备还没有打开
		return NULL;
	PAttrItem attr = mAttrArr.get();
	DWORD i = 0;

	for( i = (NULL != startIdx ? (*startIdx) : 0) ; i < mAttrCnt ; ++i )
	{
		if (attr[i].attrType == dwAttrType)
			return (attr + i);   //找到了指定的属性
	}

	if (i >= mAttrCnt)  //没有指定的属性
		return NULL;

	return NULL;
}


LONG_INT DNtfsFile::GetAllocSize()
{
	DNtfsAttr attr;
	LONG_INT off = {0};

	//获取无名数据属性
	if (DR_OK != this->FindNoNameDataAttr(&attr))
		return off;  //木有无名数据属性
	else
	{
		if(attr.IsNonResident())
		{////是非常驻属性
			return PNON_RESID_ATTR_HEAD(attr.GetAttrHeadPtr())->ATTR_AllocSize;
		}else{//常驻属性,没有分配大小也就是说只在MFT中分配了数据，数据区没有
			return off;//这里就返回0
		}
	}
//从文件名属性中获取文件的大小信息
// 	PAttrItem pAttr = this->FindAttribute(AD_FILE_NAME);
// 
// 	if (NULL == pAttr)
// 		return off;  //没有文件名属性
// 	else
// 	{
// 		attr.InitAttr(pAttr->attrDataPtr);
// 		return attr.FNGetAllocateSize();
// 	}
}
DRES DNtfsFile::ReadFile(char* buf, DWORD* dwReaded, DWORD dwToRead)
{
	//////////////////////////////////////////////////////////////////////////
	//文件的数据存储位置有两种，小文件一般会存储在MFT记录中，这个很简单，下面
	//的代码一眼就可以看清楚，而数据存储在其他数据区域的话，就有点麻烦，在无名
	//DATA属性中有一个Run列表，记录了每一个虚拟簇(VCN)所对应的逻辑簇号(LCN)，
	//也就是这里需要查找这两个Run链表,获得数据的实际位置,这个和目录有点类似，
	//只是目录的话,每一个扇区的末尾两个字节都会被替换成更新序列号(USN),而这里不
	//会，找到了就直接读取就是了，不要有什么顾忌。接下来就是具体的读取数据的过
	//程了。文件的数据读取方法大概是:我使用一个簇为数据缓存,每次读取数据都是先
	//将设备读写指针对齐到簇的边界,然后再去读一个簇(A)的大小,随之计算当前文件(F)
	//在A中的读写指针(FP)。在A中的数据FP以前的数据时无效的，FP以后的数菜蔬有效
	//的当然也需要计算本次读取的最后有一簇(B)中的有效位置(FEP),同样是读取一整簇，
	//FEP之前的数据有效，FEP之后的数据无效
	//////////////////////////////////////////////////////////////////////////
	DRES	 res	  = DR_OK;
	LONG_INT realSize = {0};	//文件的实际大小
	DNtfsAttr dataAttr;			//文件的数据属性
	LONG_INT curPtr   = {0};	//文件内的当前度写指针
	LONG_INT vcn	  = {0};	//当前读写文件的cvn 
	LONG_INT lcn	  = {0};	//当前读写文件的lcn
	LONG_INT clustCnt = {0};	//簇数
	LONG_INT secOff   = {0};
	BYTE	 SecPerClust = 0;
	DWORD	 dwClustSize = 0;	//每簇字节数
	std::vector<BYTE> dataBuf;
	DWORD	 dBufoff  = 0;		//dataBuf中写数据的偏移
	DWORD	 clustOff = 0;		//读取到的簇中的有效数据偏移(FP)
	DWORD	 toReadThisTime = 0;//本次独到的有效数据
	DWORD	 clustLeave = 0;

	if(!buf || ! dwReaded )	return DR_INVALED_PARAM;		//参数错误
	*dwReaded = 0;						//先清理一下

	if (!this->mFS) return DR_NO_OPEN;//文件实例化失败
	if(this->IsDir())	return DR_IS_DIR;	//不是文件

	//获得文件无名数据属性
	res = this->FindNoNameDataAttr(&dataAttr);
	if (res != DR_OK)	return DR_OK;	//系统数据文件

	//文件的实际大小
	realSize = this->GetRealSize();
	if (realSize.QuadPart == 0) return DR_OK;//木有数据可以读取
	if (realSize.QuadPart == this->mFilePointer.QuadPart) return DR_OK;

	//如果要读取的数超过了实际剩下的数据话，阶段要读取数据
	if(dwToRead > realSize.QuadPart - mFilePointer.QuadPart)//要读取的数据长度是否大于剩下的数据
		dwToRead = (DWORD)(realSize.QuadPart - mFilePointer.QuadPart);//截断要读取的数据

	//判断文件是否为常驻
	if (!dataAttr.IsNonResident())
	{//驻留属性 存放数据
		BYTE* dataPtr = dataAttr.R_GetAttrBodyPtr();
		dataPtr += DWORD(this->mFilePointer.QuadPart);
		memcpy(buf , dataPtr , dwToRead);
		this->mFilePointer.QuadPart += dwToRead;
		*dwReaded = dwToRead;
		return DR_OK;
	}

	//非驻留属性中存放数据

	curPtr		= this->mFilePointer;			//文件内的当前度写指针
	SecPerClust = this->mFS->GetSecPerClust();	//每簇扇区数
	dwClustSize = SecPerClust  * SECTOR_SIZE;	//每簇字节数
	dataBuf.resize(dwClustSize, 0);				//一簇作为换成空间
	dBufoff		= 0;							//dataBuf中写数据的偏移
	clustOff	= DWORD(curPtr.QuadPart % dwClustSize);//读取到的簇中的有效数据偏移(FP)
	toReadThisTime = 0;							//本次读到的有效数据

	//最后一簇有效数据指针(FEP)
	clustLeave = DWORD(dwClustSize - (curPtr.QuadPart + dwToRead)%dwClustSize);
	if (clustLeave == dwClustSize)  //刚好满一个簇后面没有什么留的
		clustLeave = 0;

	//当前读写的虚拟簇号
	vcn.QuadPart =  curPtr.QuadPart / dwClustSize;

	while(dwToRead > 0)
	{
		//获得虚拟簇号对应的逻辑簇号
		lcn = this->GetLCNByVCN(vcn , &clustCnt);
		if (lcn.QuadPart == -1) {//居然出错了
			return DR_INIT_ERR;
		}

		for (; clustCnt.QuadPart && dwToRead > 0;
			--clustCnt.QuadPart , ++vcn.QuadPart )
		{
			if (lcn.QuadPart == -2)
			{//稀疏文件
				memset(dataBuf.data(), 0, dwClustSize);
			}else{
				secOff.QuadPart = lcn.QuadPart * SecPerClust;
				res = this->mFS->ReadData(dataBuf.data(), &secOff , dwClustSize);
				if (res != DR_OK)
				{//这下麻烦大了
					return DR_INIT_ERR;
				}
				++lcn.QuadPart;
			}
			//复制读取到的有效数据
			if (dwToRead >= dwClustSize)//本次读取的数据有一簇
				toReadThisTime = dwClustSize - clustOff;
			else//本次读取的数据不到一簇
				toReadThisTime = dwClustSize - clustOff - clustLeave;
			
			//复制到有效的数据
			memcpy(buf + dBufoff , dataBuf.data() + clustOff , toReadThisTime );
			dwToRead -=toReadThisTime;
			dBufoff +=toReadThisTime;
			*dwReaded += toReadThisTime;
			curPtr.QuadPart += toReadThisTime;
			clustOff = 0;	//这是缓存簇内偏移，因为是以簇为一个读取单元，
							//所致只对第一次读取有效，读完第一次后如果再继
							//续读取的话文件指针式已经对齐到簇边界了，而这
							//个字段就必须设置为0，否则读取数据时就会出现问
							//题。2012-07-04 08:42
		}
	}
	//文件的当前读写指针 
	this->mFilePointer = curPtr;
	return DR_OK;
}
BOOL DNtfsFile::IsEOF()
{
	return (this->mFilePointer.QuadPart >= this->GetRealSize().QuadPart);
}
DRES DNtfsFile::SetFilePointer(LONG_INT off , BYTE dwMoveMethod)
{  
	LONG_INT realSize = this->GetRealSize();
	LONG_INT temp ;

	//设备根本就没有打开
	if (NULL == this->mFS) return DR_NO_OPEN;

	if (dwMoveMethod == FILE_POS_END)
	{//从文件为开始
		temp.QuadPart = realSize.QuadPart - off.QuadPart;
		if (temp.QuadPart < 0 )
		{//无效的移动
			return DR_INVALED_PARAM;
		}else{
			this->mFilePointer = temp;
			return DR_OK;
		}
	}else if(dwMoveMethod == FILE_POS_BEGIN){
		
		if (off.QuadPart < 0 || off.QuadPart >= realSize.QuadPart)
		{//无效的移动
			return DR_INVALED_PARAM;
		}else{
			this->mFilePointer = off;
			return DR_OK;
		}
	}else if(dwMoveMethod == FILE_POS_CURRENT){
		temp.QuadPart = this->mFilePointer.QuadPart + off.QuadPart;
		if (temp.QuadPart < 0 || temp.QuadPart > realSize.QuadPart)
		{//无效的移动
			return DR_INVALED_PARAM;
		}else{
			this->mFilePointer = temp;
			return DR_OK;
		}
	}else
		return DR_INVALED_PARAM;
}
LONG_INT DNtfsFile::GetRealSize()
{
	DNtfsAttr attr;
	LONG_INT off;
	off.QuadPart = 0;

	//获取无名数据属性
	if (DR_OK != this->FindNoNameDataAttr(&attr))
		return off;  //木有无名数据属性
	else
	{
		if(attr.IsNonResident())
		{////是非常驻属性
			return PNON_RESID_ATTR_HEAD(attr.GetAttrHeadPtr())->ATTR_ValidSize;
		}else{//常驻属性,
			off.QuadPart = PRESID_ATTR_HEAD(attr.GetAttrHeadPtr())->ATTR_DatSz;
			return off;
		}
	}


//从文件名中获取文件的大小信息
// 	PAttrItem pAttr = this->FindAttribute(AD_FILE_NAME);
// 	if (NULL == pAttr)
// 		return off;  //没有文件名属性
// 	else
// 	{
// 		attr.InitAttr(pAttr->attrDataPtr);
// 		return attr.FNGetRealSize();
// 	}

}
DRES DNtfsFile::FindAttribute(DWORD dwAttrType , VOID* att , DWORD* startIdx)
{
	//第一个属性在文件记DRES NtfsFile::InitAttrList( BYTE* attrBuf )
	//AD(this->mRecodeBuf)->FR_1stAttrOff;
	//DWORD dwLen = MFT_RECODE_SIZE - dwOff;//属性集合的允许长度
	DRES  res   = DR_NO;
	DWORD i = 0;
	PAttrItem pai = mAttrArr.get();
	DNtfsAttr* attr = (DNtfsAttr*)att;

	if (startIdx)
	{
		if (*startIdx >= this->mAttrCnt)
			return DR_INVALED_PARAM;
		else
			i = *startIdx;
	}

	//遍历属性表
	for (; i < this->mAttrCnt /*&& pai[i].off*/ ; ++i)
	{
		if (pai[i].attrType == dwAttrType)
		{
			//attr->InitAttr(this->mRecodeBuf + pai[i].off/* , &dwLen*/);
			attr->InitAttr(pai[i].attrDataBuf.data());
			//返回当前的索引
			
			if (startIdx) *startIdx = i;
			return DR_OK;
		}
	}
	return res;
}
DRES DNtfsFile::FindNoNameDataAttr( DNtfsAttr* attr )
{
	DRES		res = DR_NO;
	DWORD		i	= 0;
	PAttrItem	pai = mAttrArr.get();

	if (NULL == this->mFS) return DR_NO_OPEN;

	if (NULL == attr) return DR_INVALED_PARAM;

	//遍历属性表
	for (; i < this->mAttrCnt /*&& pai[i].off*/ ; ++i)
	{
		if (pai[i].attrType == AD_DATA )
		{
			//attr->InitAttr(this->mRecodeBuf + pai[i].off/* , &dwLen*/);	
			attr->InitAttr(pai[i].attrDataBuf.data());
			if (attr->GetNameLen()) 
				continue; //对不起我要的是无名数据属性
			//返回当前的索引
			return DR_OK;
		}
	}
	return res;
}
DRES DNtfsFile::InitAttrList(BYTE* attrBuf)
{
	//第一个属性在文件记录中的偏移
	PFILE_RECODE_HEAD	prh	   = PFILE_RECODE_HEAD(attrBuf);
	DWORD				dwOff  = prh->FR_1stAttrOff;
	int					i      = 0;
	int					attrLen= 0;
	PAttrItem			pai    = NULL;
	DNtfsAttr			attr;

	//先分配换从空间
	this->mAttrCnt = prh->FR_NxtAttrId;				//属性数量
	mAttrArr.reset(new AttrItem[prh->FR_NxtAttrId]);
	pai = mAttrArr.get();

	//遍历每一个属性
	while(GetDWORD(attrBuf + dwOff) != 0xFFFFFFFF){
		attr.InitAttr(attrBuf + dwOff/* , &dwLen*/);
		attrLen = attr.GetAllLen();			//当前属性的长度
		
		//当前属性在MFT中的偏移
		pai[i].mftIndex = this->mMftIdx;		//需要记得的是 在找到ATTRBUTE_LIST后需要更改这里的赋值，以保证数据正确
		pai[i].off		= (WORD)dwOff;			//当前属性在当前MFT中的偏移
		pai[i].attrType = attr.GetAttrType();	//属性类型
		pai[i].id		= attr.GetAttrID();		//属性id
		pai[i].attrDataBuf.resize(attrLen, 0);	//属性数据缓存区
		memcpy(pai[i].attrDataBuf.data(), attrBuf + dwOff , attrLen);
		//pai[i++].off = (WORD)dwOff;				//属性在缓冲中的偏移
		dwOff += attrLen;				//先一个属性的偏移
		++i;

#ifdef _DEBUG  
		//因为我还没在我的电脑上找到ATTRIBUTE_LIST，所以无法研究这个属性的具体情况
		//所以在这里就谈一下窗
		if (pai[i].attrType == AD_ATTRIBUTE_LIST)
		{
			MessageBoxA(NULL ,"找到了ATTRIBUTE_LIST属性" , "找到了ATTR_LIST" , MB_OK);
		}
#endif
	}
	//属性的总数
	this->mAttrCnt = i;

	return DR_OK;
}

// DRES DNtfsFile::InitRunList(VOID* att)
// {
// 	NtfsAttr*	attr	= (NtfsAttr*)att;
// 	LONG_INT	start	= {0};
// 	DWORD		runOff	= 0;		//运行中的数据偏移
// 	DWORD		temp	= 0;
// 	PRunHead	runHead = NULL;
// 	LONG_INT	lcn		= {0};
// 	BYTE*		run		= NULL;
// 	PRunList	runListPtr = NULL;
// 	LONG_INT	dataBuf = {0};
// 	int			i		= 0;
// 
// 	//安检
// 	if (!attr) return DR_INVALED_PARAM;
// 	//已经初始化好了
// 	if (this->mRunList)		return DR_OK;
// 	
// 	//只有非常驻属性才有运行列表
// 	if (!attr->IsNonResident()) return DR_INIT_ERR;
// 
// 	//runlist的节点总数
// 	start = attr->NR_GetStartVCN();
// 	this->mRunCnt = (DWORD)(attr->NR_GetEndVCN().QuadPart - start.QuadPart + 1);
// 	this->mRunList = new RunList[this->mRunCnt];
// 
// 	run	   = attr->NR_GetDataPtr();//运行的其实数据地址
// 
// /*	LONG_INT allCnt = start;*/
// 	runListPtr = PRunList(this->mRunList);
// 
// 	//我不知道Run的第二个和第三个字段的的数据最大能占多少个字节
// 	//这里去8个字节，这应该没问题吧, 
// 	for (i = 0 ;; ++i)
// 	{	
// 		runHead = PRunHead(run + runOff++);  //读取run的头部
// 		if (runHead->all == 0)
// 			break;			     //遍历完了当前属性的所有运行
// 
// 		if (i == 0)//当前VCN
// 			runListPtr[i].vcn = start;
// 		else//当前vcn是前一个run节点的vcn+簇数
// 			runListPtr[i].vcn.QuadPart = 
// 			runListPtr[i-1].vcn.QuadPart + runListPtr[i-1].clustCnt.QuadPart;
// 		
// 		//簇数
// 		runListPtr[i].clustCnt.QuadPart = 0;//先清理一下
// 		//读取数据簇数  length
// 		memcpy(&(runListPtr[i].clustCnt) , run + runOff , runHead->length);
// 		runOff += runHead->length;
// 
// 		if (runHead->offset == 0)   
// 		{//稀疏文件的Run  ，表示当前vcn没有数据
// 			runListPtr[i].lcn.QuadPart = 0;
// 			continue;;
// 		}
// 
// 		//最后一个字符的缓存下标
// 		temp = runOff + runHead->offset -1;
// 
// 		//获得lcn，这个可能是一个负数 ，所以先做有符号数读取读取出来
// 		dataBuf.QuadPart = (char)run[temp--];
// 		//一次读取剩下的字节
// 		for (; temp >= runOff; --temp)
// 			dataBuf.QuadPart = (dataBuf.QuadPart << 8) + GetBYTE(run + temp);
// 
// 		//实际的lcn
// 		lcn.QuadPart += dataBuf.QuadPart;
// 	
// 		runListPtr[i].lcn = lcn;
// 
// 		//下一个run的位置
// 		runOff += runHead->offset;
// 	}
// 
// 	return DR_OK;
// }
// 

LONG_INT DNtfsFile::GetLCNByVCN(LONG_INT vcn , PLONG_INT clustCnt)
{
	DRES		res = DR_OK;
	LONG_INT	lcn = {0};
	DWORD		i	= 0;
	DNtfsAttr	attr;

	lcn.QuadPart = -1;	
	
	if (!m_upRun)
	{
		//还没有初始化Runlist
		m_upRun.reset(new DRun());

		if (this->IsDir())
		{//是目录的话
			res = this->FindAttribute(AD_INDEX_ALLOCATION , &attr);
			if (res != DR_OK) return lcn;//没有指定的属性
			res = m_upRun->InitRunList(&attr);

			if (res != DR_OK) return lcn;//没有指定的属性
		}else{
			res = this->FindNoNameDataAttr(&attr);
			if (res != DR_OK) return lcn;//没有指定的属性
			if (!attr.IsNonResident()) return lcn;//不是非常驻属性  没有Runlist

			res = m_upRun->InitRunList(&attr);
			if (res != DR_OK) return lcn;//没有指定的属性
		}
	}

	//获得运行列表
	lcn = this->m_upRun->GetLCNByVCN(vcn, clustCnt);

	return lcn;
}

void DNtfsFile::Close()
{
	mAttrArr.reset();
	m_upRun.reset();
	mMftHeadBuf.clear();
	mFS = NULL;
	mMftIdx.QuadPart = 0;
}

LONG_INT DNtfsFile::GetMftStartSec()
{
	LONG_INT liSec = {-1};

	//当前文件并没有打开
	if (this->mFS == NULL) return liSec;


	return this->mFS->GetSectorOfMFTRecode(this->mMftIdx);

	//先计算一下偏移吧
// 	liSec = this->mFS->mCluForMFT;
// 	liSec.QuadPart *= mFS->mSecPerClu;	//MFT其实扇区号
// 	liSec.QuadPart += this->mMftIdx.QuadPart * RECODE_PER_SEC;
// 
// 	return  liSec;
}


LONG_INT DNtfsFile::GetFDTOffset()
{
	return mLIStartFDT;
}

WORD DNtfsFile::GetFDTLen()
{
	return mFDTLen;
}

