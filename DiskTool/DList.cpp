/***********************************************************************
 * Module:		DiskDll.dll
 * FileName��	DList.cpp
 * Author:		����
 * Created:		2012-3-20
 * Purpose:		һ��˫�������ʵ��
 *
 * Modefied:	2012-05-31 �޸�һ��BUG��֮ǰ�������в���һ���ڵ�ʱ��û��
 *				�����ڵ����������ܵ���ģ���ڴ���ʴ���
 ***********************************************************************/


#include "disktool.h"
#include "stut_connf.h"

DList::DList()
{
	this->mCount = 0;
	::memset(&mHead , 0 , sizeof(mHead));
	this->mPTail = &mHead;
}

DList::~DList()
{
	//Ҫɾ��ÿһ���ڵ�
	Node* node = mHead.mNext;
	Node* temp = node;
	while (temp)				//
	{
		node = temp->mNext;
		delete temp;
		temp = node;
	}
	this->mCount  = 0;
}

int DList::AddPart(PVOID data)
{
	PNode node = NULL;

	if(!data) return FALSE;		//��������ȷ 
	
	node = new Node;		//�½�һ���ڵ�
	node->mData = data;

	this->mPTail->mNext = node;		
	this->mPTail = node;
	this->mPTail->mNext = NULL;

	++this->mCount;

	return TRUE;
}

VOID* DList::GetPart(int index)
{
	int		i	 = 0; 
	PNode	temp = 0; 

	//����Խ����
	if(index < 0 || index >= mCount )
		return NULL;

	temp = this->mHead.mNext;
	for(i = 0; i != index ;++i) temp = temp->mNext;

	return temp->mData;

}
PVOID DList::DeletePart(int index)
{
	int		i	= 0; 
	PNode	pre = &this->mHead;
	PNode	res = NULL;
	PVOID	data = NULL;

	//����Խ��
	if(index < 0 || index >= mCount )
		return NULL;
	
	for(i = 0; i != index ;++i) pre = pre->mNext;
	
	//Ҫɾ���Ľڵ�
	res = pre->mNext;
	pre->mNext = res->mNext;

	if(!res->mNext)
	{///Ҫɾ���������һ��
		this->mPTail = pre;	
	}
	
	//�����ݼ�
	--mCount;
	data = res->mData;
	delete res ;

	return data;
}

int DList::GetCount(void)
{
	return this->mCount;
}

BOOL DList::SortList(NODE_COMPARE cmpFun)
{
	if(!mHead.mNext)
		return FALSE;
	//�������ʵ������
	mHead.mNext = InsertSort(mHead.mNext , cmpFun);
	//����βָ��
	this->mPTail = &mHead;
	while(mPTail->mNext) mPTail = mPTail->mNext;

	return TRUE;
}

DList::Node* DList::InsertSort(Node * head , NODE_COMPARE cmpFun)
{
	Node *first;	//Ϊԭ����ʣ������ֱ�Ӳ�������Ľڵ�ͷָ��
	Node *t;		//��ʱָ�����������ڵ�
	Node *p;		//��ʱָ�����
	Node *q;		//��ʱָ�����

	first = head->mNext;	//ԭ����ʣ������ֱ�Ӳ�������Ľڵ�����
	head->mNext = NULL;		//ֻ����һ���ڵ���������������

	while (first != NULL)	//����ʣ�����������
	{
		//ע�⣺����for����������ֱ�Ӳ�������˼��ĵط�
		for (t=first, q=head; ((q!=NULL) && (*cmpFun)(q->mData , t->mData));
			p=q, q=q->mNext); //����ڵ��������������Ҳ����λ��

		//�˳�forѭ���������ҵ��˲����λ��
		first = first->mNext; //���������еĽڵ��뿪���Ա������뵽���������� 

		if (q == head) //���ڵ�һ���ڵ�֮ǰ
			head = t;    
		else //p��q��ǰ��
			p->mNext = t;   

		t->mNext = q; //��ɲ��붯��
	}
	return head;
}

BOOL DList::InsertNode(int index, VOID* data)
{
	Node*	node = NULL;
	int		i	 = 0; 
	PNode	temp = NULL;

	//����Խ����  ����Ҫ��ӵ����ݲ�����
	if(index < 0 || index > mCount || !data)
		return FALSE;
	
	node = new Node;
	node->mData = data;
	node->mNext = NULL;

	temp = &mHead;
	for(i = 0; i != index ;++i) temp = temp->mNext;
	node->mNext = temp->mNext;
	temp->mNext = node;

	//�����˽ڵ�����Ҫ������Ҳ����һ��   2012-5-31 15:42
	++mCount;

	return TRUE;
}
