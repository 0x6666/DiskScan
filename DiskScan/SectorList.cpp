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
	{//�����ǿ�
		m_pSecList = (PSECTOR_LIST_NODE)malloc(sizeof(SECTOR_LIST_NODE));
		memset(m_pSecList , 0 , sizeof(SECTOR_LIST_NODE));
		m_pSecList->mStartSector = sector;
		m_pSecList->mSectorCnt.QuadPart = nSecCount.QuadPart;
		this->m_nListNodeCnt = 1;
		return TRUE;
	}else{
		//������������
		int i;
		for (i = 0 ; i < m_nListNodeCnt; ++i )
		{
			//����Ƿ���ڵ�ǰ�ڵ�
			if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart &&
				m_pSecList[i].mStartSector.QuadPart + 
				m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
			{//����
				if (nSecCount.QuadPart != 1)
				{//��Ҫ�ж��Ƿ�ȫ����
					if (sector.QuadPart + nSecCount.QuadPart > 
						m_pSecList[i].mStartSector.QuadPart + 
						m_pSecList[i].mSectorCnt.QuadPart )
					{//��β��
						m_pSecList[i].mSectorCnt.QuadPart = 
							sector.QuadPart + nSecCount.QuadPart - 
							m_pSecList[i].mStartSector.QuadPart;
					}
				}
				return TRUE;  //��ӳɹ�
			}

			//����Ƿ��ǵ�ǰ�ڵ����һ��������һ������
			if (sector.QuadPart == 
				m_pSecList[i].mStartSector.QuadPart + 
				m_pSecList[i].mSectorCnt.QuadPart)
			{//����ֱ��׷��
				m_pSecList[i].mSectorCnt.QuadPart += nSecCount.QuadPart;
				return TRUE;
			}
		}

		//��Ҫ���һ���ڵ� 
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
		//����Ƿ���ڵ�ǰ�ڵ�
		if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart &&
			m_pSecList[i].mStartSector.QuadPart + 
			m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
		{//����
			return TRUE;  //��ӳɹ�
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
	if(0 > sector.QuadPart) return res;	//��Ч�غ�
	if ( 0 == m_nListNodeCnt ) return res;	//û�дغ�
	if (1 == m_nListNodeCnt)
	{//��һ���غ�
		if (m_pSecList[0].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[0].mStartSector.QuadPart + 
			m_pSecList[0].mSectorCnt.QuadPart > sector.QuadPart)
		{//����
			return sector;
		}else
			return res;  //������
	}

	//����һ��
	for(i = 0 ; i < m_nListNodeCnt - 1 ; ++i )
	{
		if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[i].mStartSector.QuadPart + 
			m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
		{//�����뵱ǰ�ڵ�
			return sector;
		}else if (m_pSecList[i + 1].mStartSector.QuadPart > sector.QuadPart)
		{//�ڵ�ǰ�ڵ����һ���ڵ�֮�� 
			return m_pSecList[i + 1].mStartSector;
		}
	}
	return res;  //������
}

LONG_INT SectorList::FixToPreSector( LONG_INT sector )
{
	LONG_INT  res = {-1};
	int		i = 0;
	if ( 0 > sector.QuadPart )	return res;	//��Ч�غ�
	if ( 0 == m_nListNodeCnt )	return res;	//û�дغ�
	if ( 1 == m_nListNodeCnt )
	{//��һ���غ�
		if (m_pSecList[0].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[0].mStartSector.QuadPart + 
			m_pSecList[0].mSectorCnt.QuadPart > sector.QuadPart)
		{//����
			return sector;
		}else
			return res;  //������
	}

	//����һ��
	for(i = 0 ; i < m_nListNodeCnt - 1 ; ++i )
	{
		if (m_pSecList[i].mStartSector.QuadPart <= sector.QuadPart && 
			m_pSecList[i].mStartSector.QuadPart + 
			m_pSecList[i].mSectorCnt.QuadPart > sector.QuadPart)
		{//�����뵱ǰ�ڵ�
			return sector;
		}else if (m_pSecList[i + 1].mStartSector.QuadPart > sector.QuadPart)
		{//�ڵ�ǰ�ڵ����һ���ڵ�֮�� 
			res.QuadPart = m_pSecList[i].mStartSector.QuadPart + m_pSecList[i].mSectorCnt.QuadPart - 1;
			return res;
		}
	}
	return res;  //������
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
