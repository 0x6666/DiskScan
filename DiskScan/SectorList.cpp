#include "stdafx.h"
#include "SectorList.h"

SectorList::SectorList(void)
{
	m_pSecList = NULL;
	m_nListNodeCnt = 0;
}

SectorList::~SectorList(void)
{
	if (NULL != m_pSecList )	free(m_pSecList);
	m_pSecList = 0;
}

BOOL SectorList::AddSector( LONG_INT sector ,DWORD nSecCount /*= 1*/)
{
	LONG_INT secCnt;
	secCnt.QuadPart = nSecCount;
	return AddSector(sector , secCnt);
}

BOOL SectorList::AddSector( LONG_INT sector ,LONG_INT nSecCount )
{
	if (sector.QuadPart < 0) return FALSE;
	if (0 == nSecCount.QuadPart)		return FALSE;


	if (NULL != m_pSecList)
	{//序列是空
		m_pSecList = (PSECTOR_LIST_NODE)malloc(sizeof(SECTOR_LIST_NODE));
		memset(m_pSecList , 0 , sizeof(SECTOR_LIST_NODE));
		m_pSecList->mStartSector = sector;
		m_pSecList->mSectorCnt.QuadPart = nSecCount.QuadPart;
		this->m_nListNodeCnt = 1;
		return TRUE;
	}else{
		//序列中有数据
		int i;
		for (i = 0 ; i < m_nListNodeCnt; ++i )
		{
			//检查是否存在当前节点
			if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart &&
				m_pSecList[i].mStartSector.QuadPart + 
				m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
			{//存在
				if (nSecCount.QuadPart != 1)
				{//需要判断是否全部在
					if (sector.QuadPart + nSecCount.QuadPart > 
						m_pSecList[i].mStartSector.QuadPart + 
						m_pSecList[i].mSectorCnt.QuadPart )
					{//超尾了
						m_pSecList[i].mSectorCnt.QuadPart = 
							sector.QuadPart + nSecCount.QuadPart - 
							m_pSecList[i].mStartSector.QuadPart;
					}
				}
				return TRUE;  //添加成功
			}

			//检查是否是当前节点最后一扇区的下一个扇区
			if (sector.QuadPart == 
				m_pSecList[i].mStartSector.QuadPart + 
				m_pSecList[i].mSectorCnt.QuadPart)
			{//可以直接追加
				m_pSecList[i].mSectorCnt.QuadPart += nSecCount.QuadPart;
				return TRUE;
			}
		}

		//需要添加一个节点 
		this->m_pSecList = (PSECTOR_LIST_NODE)realloc(m_pSecList , (++m_nListNodeCnt) * sizeof(SECTOR_LIST_NODE));
		this->m_pSecList[i].mStartSector = sector;
		this->m_pSecList[i].mSectorCnt.QuadPart = nSecCount.QuadPart;
		return TRUE;
	}
}

BOOL SectorList::AddSector( DWORD sector ,DWORD nSecCount /*= 1*/ )
{
	LONG_INT secCnt , sec;
	sec.QuadPart = sector;
	secCnt.QuadPart = nSecCount;
	return AddSector(sec , secCnt);
}

BOOL SectorList::AddSector( DWORD sector ,LONG_INT nSecCount )
{
	LONG_INT sec;
	sec.QuadPart = sector;
	return AddSector(sec , nSecCount);
}

BOOL SectorList::IsSectorExist( LONG_INT sector )
{
	if(0 > sector.QuadPart) return FALSE;

	int i  = 0;
	for (i = 0 ; i < m_nListNodeCnt ; ++i)
	{
		//检查是否存在当前节点
		if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart &&
			m_pSecList[i].mStartSector.QuadPart + 
			m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
		{//存在
			return TRUE;  //添加成功
		}
	}
	return FALSE;
}

// LONG_INT SectorList::FixSector( LONG_INT sector )
// {
// 	return sector;
// }

LONG_INT SectorList::FixToNextSector( LONG_INT sector )
{
	LONG_INT  res = {-1};
	int		i = 0;
	if(0 > sector.QuadPart) return res;	//无效簇号
	if ( 0 == m_nListNodeCnt ) return res;	//没有簇号
	if (1 == m_nListNodeCnt)
	{//有一个簇号
		if (m_pSecList[0].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[0].mStartSector.QuadPart + 
			m_pSecList[0].mSectorCnt.QuadPart > sector.QuadPart)
		{//存在
			return sector;
		}else
			return res;  //不存在
	}

	//超过一个
	for(i = 0 ; i < m_nListNodeCnt - 1 ; ++i )
	{
		if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[i].mStartSector.QuadPart + 
			m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
		{//存在与当前节点
			return sector;
		}else if (m_pSecList[i + 1].mStartSector.QuadPart > sector.QuadPart)
		{//在当前节点和下一个节点之间 
			return m_pSecList[i + 1].mStartSector;
		}
	}
	return res;  //不存在
}

LONG_INT SectorList::FixToPreSector( LONG_INT sector )
{
	LONG_INT  res = {-1};
	int		i = 0;
	if ( 0 > sector.QuadPart )	return res;	//无效簇号
	if ( 0 == m_nListNodeCnt )	return res;	//没有簇号
	if ( 1 == m_nListNodeCnt )
	{//有一个簇号
		if (m_pSecList[0].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[0].mStartSector.QuadPart + 
			m_pSecList[0].mSectorCnt.QuadPart > sector.QuadPart)
		{//存在
			return sector;
		}else
			return res;  //不存在
	}

	//超过一个
	for(i = 0 ; i < m_nListNodeCnt - 1 ; ++i )
	{
		if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[i].mStartSector.QuadPart + 
			m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
		{//存在与当前节点
			return sector;
		}else if (m_pSecList[i + 1].mStartSector.QuadPart > sector.QuadPart)
		{//在当前节点和下一个节点之间 
			res.QuadPart = m_pSecList[i].mStartSector.QuadPart + m_pSecList[i].mSectorCnt.QuadPart - 1;
			return res;
		}
	}
	return res;  //不存在
}

SectorList* SectorList::Clone()
{
	SectorList* t_copy = new SectorList();
	
	if (0 == this->m_nListNodeCnt)
	{
		return t_copy;
	}

	t_copy->m_nListNodeCnt = this->m_nListNodeCnt;
	t_copy->m_pSecList = (PSECTOR_LIST_NODE)malloc(sizeof(SECTOR_LIST_NODE)*t_copy->m_nListNodeCnt);
	memcpy(t_copy->m_pSecList , this->m_pSecList , sizeof(SECTOR_LIST_NODE)*t_copy->m_nListNodeCnt);
	t_copy->m_strName = this->m_strName;
	return t_copy;
}

CString SectorList::GetName()
{
	return m_strName;
}

void SectorList::SetName( CString strName )
{
	this->m_strName = strName;
}

LONG_INT SectorList::GetStartSector()
{
	LONG_INT sec = {-1};
	if (0 >= this->m_nListNodeCnt ) return sec;

	return m_pSecList[0].mStartSector;
}
