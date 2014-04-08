#if !defined(AFX_FAT32DOC_H__D878FE6C_6CAD_45FC_A4A9_363C4ACAE473__INCLUDED_)
#define AFX_FAT32DOC_H__D878FE6C_6CAD_45FC_A4A9_363C4ACAE473__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataDoc.h"
#include "..\DiskTool\disktool.h"
#include "Fat32FileAttr.h"
// Fat32Doc.h : header file



/////////////////////////////////////////////////////////////////////////////
// CFat32Doc document

class CFat32Doc : public CDataDoc
{
protected:
	CFat32Doc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFat32Doc)

// Attributes
public:
	DFat32*		m_pFat32;		//FAT32�Ĳ�������
	LONG_INT	m_liStartSec;	//�ļ�ϵͳ�ڵ�ǰ�豸�ϵ���ʵ����
	CString		m_strCurPath;	//�������б��е�ǰ��ʾ�����ݵ�·��
	CString		m_strDevAreaIdx;//�豸����ID,�����ǰ�򿪵���һ����Ļ�����Ϊ "0"
	CString		m_strDevStartSec;//���豸�ϵ���ʼ������,�����ǰ�򿪵���һ����Ļ�����Ϊ "0"

	HANDLE		m_hThread;//ö�������ļ�
	volatile BOOL   m_bIsRun; //�߳��Ƿ�������  ���̴߳���

	HANDLE		m_hChkDelFile;//�鿴�Ѿ�ɾ�������ļ�
	volatile BOOL   m_bIsChkDelFileRun; //�߳��Ƿ�������  ���̴߳���

	//��ȡ�����б�
	HANDLE		m_hGetSectorListThread;
	volatile BOOL	m_bIsGetSeclistRun;

	//ͼ���б�
	CImageList* m_pImgList;




	CFat32FileDlg* m_pDlgFileAttr;//FAT32�ļ����ԶԻ���
// Operations
public:
	//��õ�ǰ�б���Ҫ��ʾ��·��
	CString GetCurPath();
	//���õ�ǰҪ��ʾ������
	void    SetCurPath(CString path);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFat32Doc)
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
	//		offset	�������豸���ϵ�����ƫ��
	//		isNext	���ָ���Ĳ������Ƿ�Ҫ����һ����Ч���� 
	//				TRUE ���offset���������ȡ��һ����Ч����
	//				FALSE ����������ڶ�ȡ��һ����Ч����
	//		size	��Ҫ��ȡ�����ݵĴ�С
	//return ��ȡ�����Ƿ�ɹ�
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL ReadData(void* buf , PLONG_INT offset , BOOL isNext = TRUE , DWORD size = SECTOR_SIZE);
	
	//////////////////////////////////////////////////////////////////////////
	//��õ�ǰ�ĵ�������ʾ��ϸ��Ϣ����ͼ��
	//return ����ʱʶ�����
	virtual CRuntimeClass* GetInofViewClass();

	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰѡ���·��
	//param
	//		strPath	��ǰѡ���·��
	virtual	void SetCurFile(CString strPath);

	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CFat32Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFat32Doc)

	//////////////////////////////////////////////////////////////////////////
	//����Ϣ��m_pContentList�ĵ���¼�����Ӧ����Ҳ��Ҫ������ȥʵ��
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnClickContextList(NMHDR* pNMHDR, LRESULT* pResult);  
	
	//////////////////////////////////////////////////////////////////////////
	//����Ϣ��m_pContentList��˫�����¼�����Ӧ����Ҳ��Ҫ������ȥʵ��
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult);

	//////////////////////////////////////////////////////////////////////////
	//���б����һ����б�
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult);

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

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//��λѡ���˵��б���� ����������
	afx_msg void OnFat32PosData();
	//�ļ����ԶԻ���
	afx_msg void OnFat32FileAttr();
	//��λ����Ŀ¼
	afx_msg void OnFat32PosParaentDir();
	//�ļ����Ϊ
	afx_msg void OnFat32ServeAs();
	afx_msg void OnUpdateFat32ServeAs(CCmdUI *pCmdUI);
	virtual void OnCloseDocument();

// 	//���õ�ǰ��Ҫ��ʾ������
// 	void SetCurSector(LONG_INT sector);

protected:
	//////////////////////////////////////////////////////////////////////////
	// ����б���ѡ����ļ�·��
	//param	
	//		strName �û���������
	CString GetSelPath(CString& strName);

public:
	afx_msg void OnCheckDeleteFile();
	afx_msg void OnUpdateCheckDeleteFile(CCmdUI *pCmdUI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAT32DOC_H__D878FE6C_6CAD_45FC_A4A9_363C4ACAE473__INCLUDED_)
