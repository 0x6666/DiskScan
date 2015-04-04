/***********************************************************************
 * FileName:	run.cpp 
 * Author:		����
 * Created:		2012��7��1�� ������
 * Purpose:		����run�ľ���ʵ��
 * Comment:		������Ƕ�һ���ǳ�פ������run��һ�������ṩ�˶�run�ļ�
 *				����������.
 *
 * Modify:		�޸�BUG����֮ǰ����Run�ڵ�������ͨ����ǰ�ǳ�פ���ԵĽ���
 *				VCN��ȥ��ʼVCN��Ҳ����˵ÿһ��VCN�������һ��RunList�Ŀ�
 *				��,���ǲ�����ģ�֮ǰ��Ƶ���ÿһ��������LCN����һ��RunList
 *				Ҳ����˵���ǲ�������Ƶġ�������Ĭ�Ϸ���3��RunList��Ȼ��
 *				����Ҫ��ʱ���ڼ�����ӿռ䡣2012-07-18 1:18				
 *
 ***********************************************************************/


#include "disktool.h"
#include "stut_connf.h"
#include <assert.h>

DRun::DRun()
{
}


DRun::~DRun()
{
}


DRES DRun::InitRunList( DNtfsAttr* attr )
{
	DWORD		runOff	= 0;		//�����е�����ƫ��
	DWORD		temp	= 0;
	PRunHead	runHead = NULL;
	LONG_INT	lcn		= {0};
	BYTE*		run		= NULL;
	LONG_INT	dataBuf = {0};
	int			i		= 0;

	//����
	if (NULL == attr)
		return DR_INVALED_PARAM;
	//�Ѿ���ʼ�����˵ľ�ֱ��ɾ��
	if (!mRunList.empty())
		mRunList.clear();

	//ֻ�зǳ�פ���Բ��������б�
	if (!attr->IsNonResident()) 
		return DR_INIT_ERR;

// 	this->mRunCnt = (DWORD)(attr->NR_GetEndVCN().QuadPart - start.QuadPart + 1);
// 	this->mRunList = new RunList[this->mRunCnt];

	run = attr->NR_GetDataPtr();//���е���ʼ���ݵ�ַ

	//�Ҳ�֪��Run�ĵڶ����͵������ֶεĵ����������ռ���ٸ��ֽ�
	//����ȥ8���ֽڣ���Ӧ��û�����, 
	for (i = 0;; ++i)
	{
		runHead = PRunHead(run + runOff++);	//��ȡrun��ͷ��
		if (runHead->all == 0)
			break;	//�������˵�ǰ���Ե���������

		/*if (i == runCnt)    // 2012-07-18 1:18	
		{//�ڵ㲻����
			runCnt += 2;  //��������ڵ�Ŀռ�
			this->mRunList = (PRunList)realloc(mRunList, sizeof(RunList)* runCnt);
			runListPtr = PRunList(this->mRunList);
		}*/

		RunList tmpRunNode = { 0 };
		if (i == 0)//��һ��
			tmpRunNode.vcn = attr->NR_GetStartVCN();
		else//��ǰvcn��ǰһ��run�ڵ��vcn+����
			tmpRunNode.vcn.QuadPart =
			mRunList.back().vcn.QuadPart + mRunList.back().clustCnt.QuadPart;

		//����
		tmpRunNode.clustCnt.QuadPart = 0;//������һ��
		//��ȡ���ݴ���  length
		memcpy(&(tmpRunNode.clustCnt), run + runOff, runHead->length);
		runOff += runHead->length;

		if (runHead->offset == 0)
		{//ϡ���ļ���Run  ����ʾ��ǰvcnû������
			tmpRunNode.lcn.QuadPart = -1;
			continue;
		}

		//���һ���ַ��Ļ����±�
		temp = runOff + runHead->offset -1;

		//���lcn�����������һ������ �����������з�������ȡ��ȡ����
		dataBuf.QuadPart = (char)run[temp--];
		//һ�ζ�ȡʣ�µ��ֽ�
		for (; temp >= runOff; --temp)
			dataBuf.QuadPart = (dataBuf.QuadPart << 8) + GetBYTE(run + temp);

		//ʵ�ʵ�lcn
		lcn.QuadPart += dataBuf.QuadPart;

		tmpRunNode.lcn = lcn;
		mRunList.push_back(tmpRunNode);

		//��һ��run��λ��
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
		//�������غ�����Ӧ����ʼ�߼��غ�ʱ��Ӧ����
		//����ǰrun����ʼ�߼��غţ����ϣ���ǰrun��ʼ
		//����غ�������Ҫ����غ�ֻ���
		//�������Ļ������ǣ���ǰrun��������ȥ����ǰrun
		//��ʼ����غ�����������غ�ֻ��� 2012-05-21 15:12
		if (vcn.QuadPart >= mRunList[i].vcn.QuadPart &&
			vcn.QuadPart < mRunList[i].vcn.QuadPart + mRunList[i].clustCnt.QuadPart)
		{//��ǰvcn�ҵ���
			
			if (mRunList[i].lcn.QuadPart == -1)
			{//ϡ���
				lcn.QuadPart = -2;
				return lcn;
			}
			lcn.QuadPart = 
				mRunList[i].lcn.QuadPart +  //��ǰrun����ʵ�߼���
				(vcn.QuadPart - mRunList[i].vcn.QuadPart);//ʵ����Ҫ������غź���ʵ�ֺ�ֻ��
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