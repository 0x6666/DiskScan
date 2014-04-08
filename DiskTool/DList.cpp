/***********************************************************************
 * Module:		DiskDll.dll
 * FileName：	DList.cpp
 * Author:		杨松
 * Created:		2012-3-20
 * Purpose:		一个双端链表的实现
 *
 * Modefied:	2012-05-31 修复一个BUG。之前在链表中插入一个节点时，没有
 *				递增节点总数，可能导致模块内存访问错误。
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
	//要删除每一个节点
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

	if(!data) return FALSE;		//参数不正确 
	
	node = new Node;		//新建一个节点
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

	//索引越界了
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

	//索引越界
	if(index < 0 || index >= mCount )
		return NULL;
	
	for(i = 0; i != index ;++i) pre = pre->mNext;
	
	//要删除的节点
	res = pre->mNext;
	pre->mNext = res->mNext;

	if(!res->mNext)
	{///要删除的是最后一个
		this->mPTail = pre;	
	}
	
	//数量递减
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
	//对链表的实体排序
	mHead.mNext = InsertSort(mHead.mNext , cmpFun);
	//处理尾指针
	this->mPTail = &mHead;
	while(mPTail->mNext) mPTail = mPTail->mNext;

	return TRUE;
}

DList::Node* DList::InsertSort(Node * head , NODE_COMPARE cmpFun)
{
	Node *first;	//为原链表剩下用于直接插入排序的节点头指针
	Node *t;		//临时指针变量：插入节点
	Node *p;		//临时指针变量
	Node *q;		//临时指针变量

	first = head->mNext;	//原链表剩下用于直接插入排序的节点链表
	head->mNext = NULL;		//只含有一个节点的链表的有序链表

	while (first != NULL)	//遍历剩下无序的链表
	{
		//注意：这里for语句就是体现直接插入排序思想的地方
		for (t=first, q=head; ((q!=NULL) && (*cmpFun)(q->mData , t->mData));
			p=q, q=q->mNext); //无序节点在有序链表中找插入的位置

		//退出for循环，就是找到了插入的位置
		first = first->mNext; //无序链表中的节点离开，以便它插入到有序链表中 

		if (q == head) //插在第一个节点之前
			head = t;    
		else //p是q的前驱
			p->mNext = t;   

		t->mNext = q; //完成插入动作
	}
	return head;
}

BOOL DList::InsertNode(int index, VOID* data)
{
	Node*	node = NULL;
	int		i	 = 0; 
	PNode	temp = NULL;

	//索引越界了  或者要添加的数据不存在
	if(index < 0 || index > mCount || !data)
		return FALSE;
	
	node = new Node;
	node->mData = data;
	node->mNext = NULL;

	temp = &mHead;
	for(i = 0; i != index ;++i) temp = temp->mNext;
	node->mNext = temp->mNext;
	temp->mNext = node;

	//插入了节点是需要将总数也递增一下   2012-5-31 15:42
	++mCount;

	return TRUE;
}
