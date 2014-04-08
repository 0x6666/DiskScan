//////////////////////////////////////////////////////////////////////////
//���豸���ݵĴ�����ĵ�����һ����������
//��Щ������Ҫ��������ʵ��  
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATADOC_H__AA942ECF_EDFA_406B_80CF_528D0D592BB5__INCLUDED_)
#define AFX_DATADOC_H__AA942ECF_EDFA_406B_80CF_528D0D592BB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SectorList.h"
// DataDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataDoc document
class CHexDataView;

class CDataDoc : public CDocument
{
protected:
	CDataDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDataDoc)

// Attributes
public:
	//������ʾ�ռ���б��ָ�룬�˿ռ��ʵ����CChildFrame�ж���
	//����ΪCCtrlBar���ӿؼ�
	CListCtrl* m_pContentList;

	//��ǰ�ĵ���������豸������
	CString m_strDevName;

	//��ǰ��ʾ��������
	LONG_INT m_liCurSec;

	//ʮ��������ͼ
	CHexDataView* m_pHexView;
	
	//������ʾ�Ĵ���
	SectorList	m_secList;
	//��ǰ��Ч��������
	SectorList* m_pCurSecList;
// Operations
public:

	//////////////////////////////////////////////////////////////////////////
	//��õ�ǰҪ��ʾ��������,���಻��Ҫʵ��
	//////////////////////////////////////////////////////////////////////////
	LONG_INT GetCurSec(void);

// Overrides


	// ClassWizard generated virtual function overrides
protected:



	//{{AFX_VIRTUAL(KeyFrameViewer)
public:
	 virtual void Serialize(CArchive& ar);   // overridden for document i/o

	//////////////////////////////////////////////////////////////////////////
	//��ʮ��������ͼ�����õĸ��ĵ�ǰ�����ŷ���
	//param
	//		liCurSector	��ǰ������
	//		pSender		������
	//////////////////////////////////////////////////////////////////////////
	void ChangeCurSector(LONG_INT liCurSector , CView* pSender);
	
	
	//////////////////////////////////////////////////////////////////////////
	//�����ĵ��������ﻹ����Ҫ���
	//////////////////////////////////////////////////////////////////////////
//	virtual BOOL OnNewDocument();  //����������ڲ���Ҫ��

	//////////////////////////////////////////////////////////////////////////
	//�����˸���ĺ������������ñ���ʱ��������,һЩ�������ַ�
	//////////////////////////////////////////////////////////////////////////
	virtual void SetTitle(LPCTSTR lpszTitle);

	//////////////////////////////////////////////////////////////////////////
	//��ʼ��m_pContentList��ָ����б�ı�ͷ
	//////////////////////////////////////////////////////////////////////////
	virtual	void InitContentListHead();
	
	//////////////////////////////////////////////////////////////////////////
	//��õ�ǰ�ĵ���������豸����������,�������ʵ��
	//////////////////////////////////////////////////////////////////////////
	virtual LONG_INT GetSecCount();
	
	//////////////////////////////////////////////////////////////////////////
	//��ǰ�ĵ���������豸��ȡ����
	//param
	//		buf		���ݻ���
	//		offset	�������豸���ϵ�����ƫ��,��Ҫ����ʵ��������
	//		isNext	���ָ���Ĳ������Ƿ�Ҫ����һ����Ч���� 
	//				TRUE ���offset���������ȡ��һ����Ч����
	//				FALSE ����������ڶ�ȡ��һ����Ч����
	//		size	��Ҫ��ȡ�����ݵĴ�С
	//return ��ȡ�����Ƿ�ɹ�
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL ReadData(void* buf , PLONG_INT offset , BOOL isNext = TRUE , DWORD size = SECTOR_SIZE);

	//////////////////////////////////////////////////////////////////////////
	//����һ�����ຯ����������Ϊ��ǿ��bAddToMRU = FALSE�������ڻ����е��������
	//����"AfxGetFileTitle",��Ϊ�ڴ�Afx�����л�ķѺܾõ�ʱ�䡣�ӻ����е������
	//����Դ����Կ������������ֻ���������£�һ�����ĵ����⣨��ǰ��ǰ���Ѿ��Լ�
	//ʵ���ˣ��������ڡ����ʹ���ļ��б����һ����¼(������û��ʹ�ô˹���)
	//////////////////////////////////////////////////////////////////////////
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = FALSE);

	//////////////////////////////////////////////////////////////////////////
	//��õ�ǰ�豸������ 
	//////////////////////////////////////////////////////////////////////////
	virtual CString GetDevName();

	//////////////////////////////////////////////////////////////////////////
	//�����ʾ��ϸ��Ϣ����ͼ��
	//return ����ʾ��ϸ��Ϣ����ͼ��
	virtual CRuntimeClass* GetInofViewClass();

	//////////////////////////////////////////////////////////////////////////
	//����ѡ������
	//param
	//		start	ѡ����ֽ�
	//		end		ѡ��Ľ����ֽ�
	//////////////////////////////////////////////////////////////////////////
	virtual void SetSel(LONG_INT start , LONG_INT end);

	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰ��Ҫ��ʾ������
	//param
	//		sector	��Ҫ��ʾ��������
	virtual void SetCurSector(LONG_INT sector);

	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰѡ���·��
	//param
	//		strPath	��ǰѡ���·��
	virtual	void SetCurFile(CString strPath);

	//////////////////////////////////////////////////////////////////////////
	//�����������б�,��Ĭ��״̬
	virtual void ReSetSectorList();

	//////////////////////////////////////////////////////////////////////////
	//���ǵ�ǰ��ʾ���б�
	//param 
	//		pSecList ��Ҫ���õ������б� ���pSecList == NULL��return FALSE
	//return FALSE ����ʧ�� 
	//		 TRUE �����ɹ�
	virtual BOOL SetSectorList(SectorList* pSecList);

	//////////////////////////////////////////////////////////////////////////
	//���ǵ�ǰ��ʾ���б�,��SetSectorList������pSecList������ֱ��ʹ��
	//param 
	//		pSecList ��Ҫ���õ������б� ���pSecList == NULL ��return FALSE
	//			�˲������붯̬���䣬���Ҳ����ֶ��ͷţ��ڴ����л��ͷ�
	//return �޸��˵�ǰ���������� TRUE
	//		 FALSE û���޸������б�
	virtual BOOL SetSectorListNoCopy(SectorList* pSecList);



	//////////////////////////////////////////////////////////////////////////
	//���ʮ��������ͼ
	virtual CHexDataView* GetHexDataView();

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDataDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:

	//{{AFX_MSG(CDataDoc)
	
	//}}AFX_MSG(CDataDoc)

	DECLARE_MESSAGE_MAP()
// public:
// 	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATADOC_H__AA942ECF_EDFA_406B_80CF_528D0D592BB5__INCLUDED_)
