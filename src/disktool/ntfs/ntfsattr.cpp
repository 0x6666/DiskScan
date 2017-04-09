/***********************************************************************
* FileName:	ntfsattr.cpp
* Author:		杨松
* Created:		2012年4月20日 星期五
* Purpose:		这是NtfsAttr的具体实现
* Comment:		这个类是对Ntfs文件的各种属性的一个抽象,不过我感觉这里基本
*				是一些垃圾代码
***********************************************************************/

#include "stdafx.h"

#define  STD_ATTR_HEAD_LEN 0x18

DNtfsAttr::DNtfsAttr() :mAttrBuf(NULL) {}
DNtfsAttr::~DNtfsAttr() {}

DRES DNtfsAttr::InitAttr(void* src)
{
	int off = 0;  //下一次在src中的读取缓存
				  //例行公务
	if (NULL == src)  return DR_INVALED_PARAM;

	//已经到了属性列表的末尾了
	if (GetDWORD(src) == 0xFFFFFFFF)    return DR_FAT_EOF;

	//先保留数据吧
	this->mAttrBuf = (BYTE*)src;

	return DR_OK;
}

DWORD DNtfsAttr::GetAttrType()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_Type;
}

DWORD DNtfsAttr::GetAllLen()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_Size;
}

BOOL DNtfsAttr::IsNonResident()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_NonResFlag == 1);
}

WORD DNtfsAttr::GetNameLen()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz;
}

BOOL DNtfsAttr::IsCompressed()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_Flags & 0x0001);
}

BOOL DNtfsAttr::IsEncrypted()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_Flags & 0x4000);
}

BOOL DNtfsAttr::IsSparse()
{
	return (PATTR_HEAD(this->mAttrBuf)->ATTR_Flags & 0x8000);
}

WORD DNtfsAttr::GetAttrID()
{
	return PATTR_HEAD(this->mAttrBuf)->ATTR_Id;
}

WORD DNtfsAttr::R_GetStdHeadLen()
{
	return STD_ATTR_HEAD_LEN + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
}

BYTE* DNtfsAttr::GetAttrHeadPtr()
{
	return this->mAttrBuf;
}

BYTE* DNtfsAttr::R_GetAttrBodyPtr()
{
	DWORD offset = STD_ATTR_HEAD_LEN + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
	return (BYTE*)(this->mAttrBuf + offset);
}

DWORD DNtfsAttr::R_GetAttrLen()
{
	return PRESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DatSz;
}

DWORD DNtfsAttr::R_GetAttrOff()
{
	return PRESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DatOff;
}

LONG_INT DNtfsAttr::NR_GetStartVCN()  //获得本属性数据流的开始虚拟簇号
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_StartVCN;
}

LONG_INT DNtfsAttr::NR_GetEndVCN()	//获得本属性数据流的结束虚拟簇号
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_EndVCN;
}

WORD	 DNtfsAttr::NR_GetDataOff()	//获得数据流描述相对于属性头的偏移，数据应该按双字对齐
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DataOff;
}

WORD     DNtfsAttr::NR_GetCmpSize()	//压缩单元的尺寸。压缩单元的尺寸必须是2的整数次幂，为0表示未压缩
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_CmpSize;
}

LONG_INT DNtfsAttr::NR_GetAllocSize() //属性记录数据块分配的空间的尺寸，该尺寸按簇尺寸对齐
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_AllocSize;
}

LONG_INT DNtfsAttr::NR_GetValidSize() //属性记录数据块的实际尺寸
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_ValidSize;
}

LONG_INT DNtfsAttr::NR_GetInitedSize()//属性记录数据块已经初始化数据的尺寸，到目前为止该值都与属性记录数据块分配的尺寸相同
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_InitedSize;
}

BYTE*	 DNtfsAttr::NR_GetDataPtr()
{
	return mAttrBuf + PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DataOff;//运行的其实数据地址
}

DWORD DNtfsAttr::NR_GetStdHeadLen()
{
	return PNON_RESID_ATTR_HEAD(this->mAttrBuf)->ATTR_DataOff;//运行的其实数据地址
}

DRES DNtfsAttr::GetAttrName(WCHAR* buf, int len0)
{
	int len = this->GetNameLen();
	if (0 == len) {  //没有文件名
		buf[0] = 0;
		return DR_NO;
	}
	if (len0 < len + 1)
	{//缓存不够
		buf[0] = 0;
		return DR_BUF_OVER;
	}

	memcpy(buf, (BYTE*)this->mAttrBuf + PATTR_HEAD(this->mAttrBuf)->ATTR_NamOff, len * 2);
	buf[len] = NULL;

	return DR_OK;
}

DWORD DNtfsAttr::FNGetFileNameLen()
{
	DWORD		offset = 0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
	PFILE_NAME  pfn = PFILE_NAME(this->mAttrBuf + offset);

	return pfn->FN_NameSize;
}

DRES  DNtfsAttr::FNGetFileName(WCHAR* buf)
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += (0x18 + 0x42); //除掉属性属性头的文件名长度偏移

							 //文件名长度
	DWORD len = FNGetFileNameLen();

	//复制文件名数据
	memcpy(buf, this->mAttrBuf + offset, len * 2);

	return DR_OK;
}

BYTE DNtfsAttr::FNGetFileNameSpase()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += 0x18; //除掉属性属性头的文件名长度偏移

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_NamSpace;
}

LONG_INT DNtfsAttr::FNGetParentMftIndx()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += 0x18; //除掉属性属性头的文件名长度偏移

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_ParentFR;
}

DWORD DNtfsAttr::FNGetFlags()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += 0x18; //除掉属性属性头的文件名长度偏移

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_DOSAttr;
}

LONG_INT DNtfsAttr::FNGetRealSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += 0x18; //除掉属性属性头的文件名长度偏移

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_ValidSize;
}

LONG_INT DNtfsAttr::FNGetAllocateSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += 0x18; //除掉属性属性头的文件名长度偏移

	PFILE_NAME pfn = PFILE_NAME(mAttrBuf + offset);

	return pfn->FN_AllocSize;
}

DWORD DNtfsAttr::IRGetAttrAttrType()
{
	DWORD offset = 0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_AttrType;
}
DWORD DNtfsAttr::IRGetIndexBlockSize()
{
	DWORD offset = 0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_EntrySz;
}

BOOL DNtfsAttr::IRIsLargeIndex()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += (0x18); //除掉属性属性头的属性名长度偏移

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return (pr->IR_IdxHead.IH_Flags == 0x01);
}

DWORD DNtfsAttr::IRGetIndexEntriesSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += (0x18); //除掉属性属性头的属性名长度偏移

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_IdxHead.IH_TalSzOfEntries;
}

DWORD DNtfsAttr::IRGetAlloIndexEntriesSize()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += (0x18); //除掉属性属性头的属性名长度偏移

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return pr->IR_IdxHead.IH_AllocSize;
}

BYTE* DNtfsAttr::IRGetFistEntry()
{
	DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
															  //文件名长度数据偏移
	offset += 0x18;//定位制定的字段的片位置(相对于属性的其实位置)

	PINDEX_ROOT pr = PINDEX_ROOT(mAttrBuf + offset);

	return (BYTE*)&(pr->IR_IdxHead) + pr->IR_IdxHead.IH_EntryOff;
}

LONG_INT DNtfsAttr::IAGetLCNByVCN(LONG_INT* vcn, PLONG_INT ClustCnt)
{
	//逻辑簇号
	LONG_INT lcn = { 0 };
	LONG_INT start = this->NR_GetStartVCN();//其实虚拟簇号
	LONG_INT end = this->NR_GetEndVCN();  //借宿虚拟簇号
	BYTE*	 run = this->mAttrBuf + this->NR_GetDataOff();//运行的其实数据地址
	DWORD	 runOff = 0;		//运行中的数据偏移
	DWORD	 temp = 0;
	PRunHead runHead = NULL;

	//我不知道Run的第二个和第三个字段的的数据最大能占多少个字节
	//这里去8个字节，这应该没问题吧, 
	LONG_INT dataBuf = { 0 };

	//越界了  返回0  
	if (vcn->QuadPart > end.QuadPart || vcn->QuadPart < start.QuadPart) {
		dataBuf.QuadPart = 0;
		return dataBuf;
	}
	//循环控制
	int i = (int)(vcn->QuadPart - start.QuadPart) + 1;
	for (; i != 0; --i)
	{
		runHead = PRunHead(run + runOff++);  //读取run的头部
		if (runHead->all == 0)
			break;			     //遍历完了当前属性的所有运行

		if (ClustCnt) {//需要读取 簇数
			ClustCnt->QuadPart = 0;//先清理一下
								   //读取数据簇数  length
			memcpy(ClustCnt, run + runOff, runHead->length);
		}
		runOff += runHead->length;

		//最后一个字符的缓存下标
		temp = runOff + runHead->offset - 1;

		//蝴蝶lcn这个可能是一个负数 ，所以先做有符号数读取读取出来
		dataBuf.QuadPart = (char)run[temp--];
		//一次读取meiyi8ge剩下的字节
		for (; temp >= runOff; --temp)
			dataBuf.QuadPart = (dataBuf.QuadPart << 8) + GetBYTE(run + temp);

		//实际的lcn
		lcn.QuadPart += dataBuf.QuadPart;
		//下一个run的位置
		runOff += runHead->offset;
	}

	return lcn;
}

BOOL DNtfsAttr::BMIsBitSet(LONG_INT bit, DNtfs* fs)
{
	BYTE	bt = 0;
	DWORD	byteCnt = 0;

	//要判断当前属性是否是常驻的
	if (this->IsNonResident())
	{//非常驻的
		DRun run;				//获取运行列表
		if (DR_OK != run.InitRunList(this))
			return FALSE;

		LONG_INT	vcn;
		DWORD		secNum = 0;//簇中的扇区偏移
		DWORD		byteNum = 0;//扇区中的字节偏移
		char		buf[SECTOR_SIZE] = { 0 };//扇区缓存
		DRES		res = DR_OK;

		//计算指定的位所在的vcn
		vcn.QuadPart = ((bit.QuadPart / 8) / SECTOR_SIZE) / fs->GetSecPerClust();

		//查询LCV
		vcn = run.GetLCNByVCN(vcn, NULL);
		run.Close();//好，你的使命已经完成了

		if (-1 == vcn.QuadPart)
			return FALSE;  //获取lcn失败

						   //计算所在LCN中的扇区号
		secNum = ((bit.QuadPart / 8) / SECTOR_SIZE) % fs->GetSecPerClust();

		//计算扇区的字节偏移
		byteNum = (bit.QuadPart / 8) % SECTOR_SIZE;

		//计算位偏移
		byteCnt = bit.QuadPart % 8;

		//读取数据
		vcn.QuadPart *= fs->GetSecPerClust();
		vcn.QuadPart += secNum;
		res = fs->ReadData(buf, &vcn, SECTOR_SIZE, TRUE);
		if (DR_OK != res) return FALSE;

		//取出指定的字节
		bt = GetBYTE(buf + byteNum);

	}
	else {//常驻的
		DWORD offset = PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
																  //文件名长度数据偏移
		offset += 0x18;//定位制定的字段的片位置(相对于属性的其实位置)

					   //计算字节数偏移
		byteCnt = DWORD(bit.QuadPart / 8);

		//获得指定为所在的字节
		bt = GetBYTE(mAttrBuf + offset + byteCnt);
		//位偏移
		byteCnt = bit.QuadPart % 8;
	}

	bt = bt >> byteCnt;
	bt = bt << 7;
	return (bt != 0);
}


DWORD DNtfsAttr::SIGetFlags()
{
	DWORD		offset = 0x18 + PATTR_HEAD(this->mAttrBuf)->ATTR_NamSz * 2;//属性名尺寸(字节)
	PSTD_INFO  psi = PSTD_INFO(this->mAttrBuf + offset);

	return psi->SI_DOSAttr;
}
