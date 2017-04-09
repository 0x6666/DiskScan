/***********************************************************************
 * FileName:	run.cpp 
 * Author:		杨松
 * Created:		2012年7月1日 星期日
 * Purpose:		这是run的具体实现
 * Comment:		这个类是对一个非常驻属性中run的一个抽象，提供了对run的简
 *				单操作方法.
 *
 * Modify:		修复BUG。在之前计算Run节点数，是通过当前非常驻属性的结束
 *				VCN减去起始VCN，也就是说每一个VCN都会分配一个RunList的空
 *				间,这是不合理的，之前设计的是每一块连续的LCN分配一个RunList
 *				也就是说那是不符合设计的。现在是默认分配3个RunList，然后
 *				在需要的时候在继续添加空间。2012-07-18 1:18				
 *
 ***********************************************************************/

#include "stdafx.h"


DRun::DRun()
{
}


DRun::~DRun()
{
}


DRES DRun::InitRunList( DNtfsAttr* attr )
{
	DWORD		runOff	= 0;		//运行中的数据偏移
	DWORD		temp	= 0;
	PRunHead	runHead = NULL;
	LONG_INT	lcn		= {0};
	BYTE*		run		= NULL;
	LONG_INT	dataBuf = {0};
	int			i		= 0;

	//安检
	if (NULL == attr)
		return DR_INVALED_PARAM;
	//已经初始化好了的就直接删除
	if (!mRunList.empty())
		mRunList.clear();

	//只有非常驻属性才有运行列表
	if (!attr->IsNonResident()) 
		return DR_INIT_ERR;

// 	this->mRunCnt = (DWORD)(attr->NR_GetEndVCN().QuadPart - start.QuadPart + 1);
// 	this->mRunList = new RunList[this->mRunCnt];

	run = attr->NR_GetDataPtr();//运行的起始数据地址

	//我不知道Run的第二个和第三个字段的的数据最大能占多少个字节
	//这里去8个字节，这应该没问题吧, 
	for (i = 0;; ++i)
	{
		runHead = PRunHead(run + runOff++);	//读取run的头部
		if (runHead->all == 0)
			break;	//遍历完了当前属性的所有运行

		/*if (i == runCnt)    // 2012-07-18 1:18	
		{//节点不够了
			runCnt += 2;  //添加两个节点的空间
			this->mRunList = (PRunList)realloc(mRunList, sizeof(RunList)* runCnt);
			runListPtr = PRunList(this->mRunList);
		}*/

		RunList tmpRunNode = { 0 };
		if (i == 0)//第一个
			tmpRunNode.vcn = attr->NR_GetStartVCN();
		else//当前vcn是前一个run节点的vcn+簇数
			tmpRunNode.vcn.QuadPart =
			mRunList.back().vcn.QuadPart + mRunList.back().clustCnt.QuadPart;

		//簇数
		tmpRunNode.clustCnt.QuadPart = 0;//先清理一下
		//读取数据簇数  length
		memcpy(&(tmpRunNode.clustCnt), run + runOff, runHead->length);
		runOff += runHead->length;

		if (runHead->offset == 0)
		{//稀疏文件的Run  ，表示当前vcn没有数据
			tmpRunNode.lcn.QuadPart = -1;
			continue;
		}

		//最后一个字符的缓存下标
		temp = runOff + runHead->offset -1;

		//获得lcn，这个可能是一个负数 ，所以先做有符号数读取读取出来
		dataBuf.QuadPart = (char)run[temp--];
		//一次读取剩下的字节
		for (; temp >= runOff; --temp)
			dataBuf.QuadPart = (dataBuf.QuadPart << 8) + GetBYTE(run + temp);

		//实际的lcn
		lcn.QuadPart += dataBuf.QuadPart;

		tmpRunNode.lcn = lcn;
		mRunList.push_back(tmpRunNode);

		//下一个run的位置
		runOff += runHead->offset;
	}

	return DR_OK;
}



LONG_INT DRun::GetLCNByVCN( LONG_INT vcn , PLONG_INT clustCnt )
{
	LONG_INT	lcn = {0};
	DNtfsAttr	attr;

	lcn.QuadPart = -1;
	for (size_t i = 0; i < mRunList.size(); ++i)
	{
		//获得虚拟簇号所对应的起始逻辑簇号时，应该是
		//（当前run的起始逻辑簇号）加上（当前run起始
		//虚拟簇号与所需要虚拟簇号只差）。
		//而簇数的话，则是（当前run簇数）减去（当前run
		//起始虚拟簇号与所需虚拟簇号只差）。 2012-05-21 15:12
		if (vcn.QuadPart >= mRunList[i].vcn.QuadPart &&
			vcn.QuadPart < mRunList[i].vcn.QuadPart + mRunList[i].clustCnt.QuadPart)
		{//当前vcn找到了
			
			if (mRunList[i].lcn.QuadPart == -1)
			{//稀疏簇
				lcn.QuadPart = -2;
				return lcn;
			}
			lcn.QuadPart = 
				mRunList[i].lcn.QuadPart +  //当前run的其实逻辑号
				(vcn.QuadPart - mRunList[i].vcn.QuadPart);//实际需要粗虚拟簇号和其实粗豪只差
			if (clustCnt)
				clustCnt->QuadPart = mRunList[i].clustCnt.QuadPart - (vcn.QuadPart - mRunList[i].vcn.QuadPart);
			break;
		}
	}
	return lcn;
}

void DRun::Close()
{
	mRunList.clear();
}