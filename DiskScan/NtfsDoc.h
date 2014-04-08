#if !defined(AFX_NTFSDOC_H__95A2B6E1_7B61_44DF_A00F_21E37DA3D086__INCLUDED_)
#define AFX_NTFSDOC_H__95A2B6E1_7B61_44DF_A00F_21E37DA3D086__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataDoc.h"
#include "..\DISKTOOL\disktool.h"	// Added by ClassView
#include "NtfsFileDlg.h"
#include <afxmt.h>
// NtfsDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNtfsDoc document

class CNtfsFileDlg;

class CNtfsDoc : public CDataDoc
{
protected:
	CNtfsDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNtfsDoc)

// Attributes
public:

// Operations
public:

	public:
//	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:

public:
	virtual ~CNtfsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNtfsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	//////////////////////////////////////////////////////////////////////////
	//��һ��ָ�����豸�����ĵ������������ֱ�ӵ��ã����ĵ�ģ����ã�����ֻ��
	//Ҫ���ö�Ӧģ���ͬ���������ɡ�����lpszPathName���ʽΪ
	//	devName [-IDX <����> -OFF <ƫ��>]
	//	devName   Ҫ�򿪵��豸������
	//	����	  Ҫ�򿪵��豸�ڴ����ϵ��������  ʮ�������ַ���ʾ
	//  ƫ��	  Ҫ�򿪵��豸�ڴ����ϵ�ƫ��      ʮ�������ַ���ʾ
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	//////////////////////////////////////////////////////////////////////////
	//��ǰ�ĵ���������豸��ȡ����
	//param
	//		buf		���ݻ���
	//		offset	�������豸���ϵ�����ƫ��
	//		isNext	���ָ���Ĳ������Ƿ�Ҫ����һ����Ч���� 
	//				TRUE ���offset���������ȡ��һ����Ч����
	//				FALSE ����������ڶ�ȡ��һ����Ч����
	//		size	��Ҫ��ȡ�����ݵĴ�С
	//return ��ȡ�����Ƿ�ɹ�
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL ReadData(void* buf , PLONG_INT offset , BOOL isNext = TRUE , DWORD size = SECTOR_SIZE);

	//////////////////////////////////////////////////////////////////////////
	//��õ�ǰ�ĵ���������豸����������,�������ʵ��
	//////////////////////////////////////////////////////////////////////////
	virtual LONG_INT GetSecCount();

	//////////////////////////////////////////////////////////////////////////
	//��ʼ��m_pContentList��ָ����б�ı�ͷ
	//////////////////////////////////////////////////////////////////////////
	virtual	void InitContentListHead();

	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰѡ���·��
	//param
	//		strPath	��ǰѡ���·��
	virtual	void SetCurFile(CString strPath);

	//////////////////////////////////////////////////////////////////////////
	//��õ�ǰ�ĵ�������ʾ��ϸ��Ϣ����ͼ��
	//return ����ʱʶ�����
	virtual CRuntimeClass* GetInofViewClass();

	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰ�б�Ҫ��ʾ��·��
	//////////////////////////////////////////////////////////////////////////
	void SetCurPath( CString path );//���õ�ǰҪ��ʾ��·��


	//////////////////////////////////////////////////////////////////////////
	//����Ϣ��m_pContentList�ĵ���¼�����Ӧ����Ҳ��Ҫ������ȥʵ��
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnClickContextList(NMHDR* pNMHDR, LRESULT* pResult);  
	
	//////////////////////////////////////////////////////////////////////////
	//����Ϣ��m_pContentList��˫�����¼�����Ӧ����Ҳ��Ҫ������ȥʵ��
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult);

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ��һ������
	afx_msg void OnBnClickedPreSector();

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ��һ������
	afx_msg void OnBnClickedNextSector();

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ��һ������
	afx_msg void OnBnClickedFirstSector();

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ���һ������
	afx_msg void OnBnClickedLastSector();

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ��һ����
	afx_msg void OnBnClickedPreClust();

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ��һ������
	afx_msg void OnBnClickedNextClust();

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ��һ������
	afx_msg void OnBnClickedFirstClust();

	//////////////////////////////////////////////////////////////////////////
	//��ͼ��������Ϣ����ʾ���һ������
	afx_msg void OnBnClickedLastClust();

		//////////////////////////////////////////////////////////////////////////
	//���б����һ����б�,��Ҫ��ʾ��ݲ˵�
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult);


public:
	// Ntfs��ʵ��
	DNtfs* m_pNtfs;
	// ���ļ�ϵͳ����ʼ����
	LONG_INT m_liStartSec;
	// ��ǰ�б���ʾ��·��
	CString m_strCurPath;
	//�ĵ���ʱ���� 
	CString m_strOpenParam;	

	//NTFS�ļ����ԶԻ���
	CNtfsFileDlg* m_pNtfsFileDlg;

	//ͼ���б�
	CImageList* m_pImgList;

	//��ȡ�ļ��б��߳�
	HANDLE m_hThread;
	volatile BOOL   m_bIsRun; //�߳��Ƿ�������  ���̴߳��� 
/*	CEvent*  m_pEveIsRun;*/

	//���ļ����ԶԻ���
	afx_msg void OnNtfsFileAttr();
	//��λ��MFT��¼
	afx_msg void OnNtfsPosMft();
	//��λMFT��¼��
	afx_msg void OnNtfsPosParaentDir();
	//�ļ����Ϊ
	afx_msg void OnNtfsServeAs();
	afx_msg void OnUpdateNtfsServeAs(CCmdUI *pCmdUI);

	//����б��е�ǰѡ�е��ļ�/Ŀ¼��·��
	CString GetSelPath(CString &strName);
	virtual void OnCloseDocument();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTFSDOC_H__95A2B6E1_7B61_44DF_A00F_21E37DA3D086__INCLUDED_)
