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

DRun::DRun()
: mRunList(NULL)
, mRunCnt(0)
{
}


DRun::~DRun()
{
}


DRES DRun::InitRunList( DNtfsAttr* attr )
{
	LONG_INT	start	= {0};
	DWORD		runOff	= 0;		//�����е�����ƫ��
	DWORD		temp	= 0;
	PRunHead	runHead = NULL;
	LONG_INT	lcn		= {0};
	BYTE*		run		= NULL;
	PRunList	runListPtr = NULL;
	LONG_INT	dataBuf = {0};
	int			i		= 0;
	DWORD		runCnt;

	//����
	if (NULL == attr)
		return DR_INVALED_PARAM;
	//�Ѿ���ʼ�����˵ľ�ֱ��ɾ��
	if (NULL != this->mRunList)		
		delete[] this->mRunList;

	//ֻ�зǳ�פ���Բ��������б�
	if (!attr->IsNonResident()) 
		return DR_INIT_ERR;

	//runlist�Ľڵ�����
	start = attr->NR_GetStartVCN();
// 	this->mRunCnt = (DWORD)(attr->NR_GetEndVCN().QuadPart - start.QuadPart + 1);
// 	this->mRunList = new RunList[this->mRunCnt];

	//��Ĭ����3���ڵ�  2012-07-18 1:18	
	runCnt = 3;
	this->mRunList = (PRunList)malloc(sizeof(RunList) * runCnt);
	memset(mRunList , 0 , sizeof(RunList) * runCnt);

	run	   = attr->NR_GetDataPtr();//���е���ʼ���ݵ�ַ

	runListPtr = PRunList(this->mRunList);

	//�Ҳ�֪��Run�ĵڶ����͵������ֶεĵ����������ռ���ٸ��ֽ�
	//����ȥ8���ֽڣ���Ӧ��û�����, 
	for (i = 0 ;; ++i , ++mRunCnt)
	{	
		runHead = PRunHead(run + runOff++);  //��ȡrun��ͷ��
		if (runHead->all == 0)
			break;			     //�������˵�ǰ���Ե���������

		if (i == runCnt)    // 2012-07-18 1:18	
		{//�ڵ㲻����
			runCnt += 2;  //��������ڵ�Ŀռ�
			this->mRunList = (PRunList)realloc(mRunList ,sizeof(RunList) * runCnt);
			runListPtr = PRunList(this->mRunList);
		}

		if (i == 0)//��һ��
			runListPtr[i].vcn = start;
		else//��ǰvcn��ǰһ��run�ڵ��vcn+����
			runListPtr[i].vcn.QuadPart = 
			runListPtr[i-1].vcn.QuadPart + runListPtr[i-1].clustCnt.QuadPart;

		//����
		runListPtr[i].clustCnt.QuadPart = 0;//������һ��
		//��ȡ���ݴ���  length
		memcpy(&(runListPtr[i].clustCnt) , run + runOff , runHead->length);
		runOff += runHead->length;

		if (runHead->offset == 0)   
		{//ϡ���ļ���Run  ����ʾ��ǰvcnû������
			runListPtr[i].lcn.QuadPart = -1;
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

		runListPtr[i].lcn = lcn;

		//��һ��run��λ��
		runOff += runHead->offset;
	}

	return DR_OK;
}



LONG_INT DRun::GetLCNByVCN( LONG_INT vcn , PLONG_INT clustCnt )
{
	LONG_INT	lcn = {0};
	DWORD		i	= 0;
	PRunList	runListPtr = NULL;
	DNtfsAttr	attr;

	runListPtr = PRunList(this->mRunList);
	lcn.QuadPart = -1;

	for (i = 0 ; i < this->mRunCnt ; ++i)
	{
		//�������غ�����Ӧ����ʼ�߼��غ�ʱ��Ӧ����
		//����ǰrun����ʼ�߼��غţ����ϣ���ǰrun��ʼ
		//����غ�������Ҫ����غ�ֻ���
		//�������Ļ������ǣ���ǰrun��������ȥ����ǰrun
		//��ʼ����غ�����������غ�ֻ��� 2012-05-21 15:12
		if (vcn.QuadPart >= runListPtr[i].vcn.QuadPart &&
			vcn.QuadPart < runListPtr[i].vcn.QuadPart + runListPtr[i].clustCnt.QuadPart)
		{//��ǰvcn�ҵ���
			
			if (runListPtr[i].lcn.QuadPart == -1)
			{//ϡ���
				lcn.QuadPart = -2;
				return lcn;				
			}
			lcn.QuadPart = 
				runListPtr[i].lcn.QuadPart +  //��ǰrun����ʵ�߼���
				(vcn.QuadPart - runListPtr[i].vcn.QuadPart);//ʵ����Ҫ������غź���ʵ�ֺ�ֻ��
			if (clustCnt) clustCnt->QuadPart 
				= runListPtr[i].clustCnt.QuadPart -
				(vcn.QuadPart - runListPtr[i].vcn.QuadPart);
			break;
		}
	}
	return lcn;
}

void DRun::Close()
{
	if (NULL != this->mRunList)
	{
		free(this->mRunList);
		this->mRunList = NULL;
		this->mRunCnt = 0;
	}
}