#pragma once
#include "NtfsDoc.h"


// CNtfsFileDlg �Ի���

class CNtfsDoc;

class CNtfsFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CNtfsFileDlg)

public:
	CNtfsFileDlg(CNtfsDoc* pDoc , CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNtfsFileDlg();

// �Ի�������
	enum { IDD = IDD_NTFS_FILE };


	//NTFS�ĵ���
	CNtfsDoc*	m_pDoc;
	//��ǰ���ļ�
	DNtfsFile*	m_pFile;	
	//��ǰ�ļ�·��
	CString		m_strFilePath;

	//////////////////////////////////////////////////////////////////////////
	//���õ�ǰ�ļ���·��
	//param
	//		strSelPath	�ļ�·��
	//return �����Ƿ��ѳɹ�
	//////////////////////////////////////////////////////////////////////////
	BOOL SetFilePath( CString strSelPath );

	//////////////////////////////////////////////////////////////////////////
	//����Ҫ��ʾ������
	void UpdateFileData();

	//////////////////////////////////////////////////////////////////////////
	//����DOS����
	void UpdateDosAttr();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnInitDialog();
	//////////////////////////////////////////////////////////////////////////
	//�����б�ĵ���¼�
	afx_msg void OnNMClickNtfsFileAttrList(NMHDR *pNMHDR, LRESULT *pResult);
	void   OnCheckBox(DWORD id);
	afx_msg void OnBnClickedCkReadOnly();
	afx_msg void OnBnClickedCkHidden();
	afx_msg void OnBnClickedCkArchive();
	afx_msg void OnBnClickedCkSystem();
	afx_msg void OnBnClickedCkSparse();
	afx_msg void OnBnClickedCkCompressed();
	afx_msg void OnBnClickedCkEncrypted();
	afx_msg void OnBnClickedCkDir();
	afx_msg void OnBnClickedCkTemp();
	afx_msg void OnBnClickedCkDevice();
	afx_msg void OnBnClickedCkNormal();
	afx_msg void OnBnClickedCkReparsePoint();
	afx_msg void OnPosStdAttrHead();	//�ڵ�ǰ�ļ�����������ѡ��ǰ���Եı�׼����ͷ
	afx_msg void OnSeverAttr();			//����ǰ���Ա��浽�ļ�
//	afx_msg void OnUpdateSeverAttr(CCmdUI *pCmdUI);
	afx_msg void OnPosStdAttrData();	//�ڵ�ǰ�ļ�����������ѡ��ǰ���Եı�׼������������
	afx_msg void OnNMRClickNtfsFileAttrList(NMHDR *pNMHDR, LRESULT *pResult);
};
