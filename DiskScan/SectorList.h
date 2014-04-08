//////////////////////////////////////////////////////////////////////////
//���ڱ�ʾһ�����������У�������п��ܲ���������
#ifndef _SECTOR_LIST_H_
#define _SECTOR_LIST_H_


class SectorList
{
public:
	/************************************************************************/
	/* һ����������������                                                   */
	/************************************************************************/
	typedef struct _tagSECTOR_LIST_NODE{
		LONG_INT	mStartSector;	//��ǰ��������ʼ������
		LONG_INT	mSectorCnt;		//��ǰ�ڵ������������
	}SECTOR_LIST_NODE ,*PSECTOR_LIST_NODE;

	//������������
	PSECTOR_LIST_NODE m_pSecList;
	//��ȥ���е���������
	int				  m_nListNodeCnt;
	CString			  m_strName;//��ǰ�����б�������


	SectorList(void);
	~SectorList(void);

public:  
	//////////////////////////////////////////////////////////////////////////
	//������������һ��������
	//param
	//		sector	��Ҫ���ӵ�������
	//		nSecCount Ҫ���ӵ��������� 
	//return �����Ƿ�ɹ�
	//		����ʧ��ԭ���� ����������(����),����nSecCount=0
	//////////////////////////////////////////////////////////////////////////
	BOOL AddSector(LONG_INT sector ,DWORD nSecCount = 1);
	BOOL AddSector(LONG_INT sector ,LONG_INT nSecCount);
	BOOL AddSector(DWORD sector ,DWORD nSecCount = 1);
	BOOL AddSector(DWORD sector ,LONG_INT nSecCount);


	//////////////////////////////////////////////////////////////////////////
	//�ж�ָ���������Ƿ����
	//param
	//		sector	��Ҫ�жϵ�������
	//////////////////////////////////////////////////////////////////////////
	BOOL IsSectorExist(LONG_INT sector);

	//////////////////////////////////////////////////////////////////////////
	//������������޸������ţ���������  1��2��3  7��8��9  13,14,15  ��������
	//��6�򷵻�7������������12�򷵻�13���������4�򷵻�3���������10�򷵻�9��
	//param
	//		sector	���޸���������
	//return ��Ҫ�޸���������  -1�޸�ʧ��
	//////////////////////////////////////////////////////////////////////////
/*	LONG_INT FixSector(LONG_INT sector);*/

	//////////////////////////////////////////////////////////////////////////
	//������������޸�����һ����Ч�����ţ���������  1��2��3  7��8��9  13,14,15
	//����������6�򷵻�7������������12�򷵻�13��
	//param
	//		sector	���޸���������
	//return ��Ҫ�޸��������� -1�޸�ʧ��
	//////////////////////////////////////////////////////////////////////////
	LONG_INT FixToNextSector(LONG_INT sector);

	//////////////////////////////////////////////////////////////////////////
	//������������޸�ǰһ����Ч�����ţ���������  1��2��3  7��8��9  13,14,15  
	//�����������������4�򷵻�3���������10�򷵻�9��
	//param
	//		sector	���޸���������
	//return ��Ҫ�޸��������� -1�޸�ʧ��
	//////////////////////////////////////////////////////////////////////////
	LONG_INT FixToPreSector(LONG_INT sector);

	//////////////////////////////////////////////////////////////////////////
	//����Լ���һ������
	SectorList* GetCopy();

	//////////////////////////////////////////////////////////////////////////
	//��õ�ǰ��ʾ���������е�����
	CString GetName();
	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰ�������е�����
	void  SetName(CString strName);

	//////////////////////////////////////////////////////////////////////////
	//������������е���ʼ������ 
	//return ��ʼ������ ��������ڵĻ�����-1
	LONG_INT GetStartSector();

};

#endif