#include "Fat32Doc.h"

#ifndef _CFAT32_FILE_ATTR_H_
#define _CFAT32_FILE_ATTR_H_


// CFat32FileAttr �Ի���
class CFat32Doc;

class CFat32FileDlg : public CDialog
{
	DECLARE_DYNAMIC(CFat32FileDlg)

public:
	CFat32FileDlg(CFat32Doc* pDoc , CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFat32FileDlg();

// �Ի�������
	enum { IDD = IDD_FAT32_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//��ǰ�Ի����Ӧ���ļ����ڵ��ĵ�
	CFat32Doc*  m_pDoc;
	//��ǰ�ļ�����Ӧ��ʵ���ļ���·��
	CString		m_strFilePath;
	//FAT32���ļ�����
	DFat32File*	m_pFile;
	//�߳�
	HANDLE		m_hThread;
	volatile BOOL		m_bIsThreadRun;

	//���õ�ǰҪ��ʾ���ļ���·��
	BOOL SetFilePath(CString file);
	//������(����)��ʾ����
	int UpdateFileData(void);
	afx_msg void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickFat1List(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickFat2List(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCkVolId();

	void   OnCheckBox(DWORD id);
	afx_msg void OnBnClickedCkReadOnly();
	afx_msg void OnBnClickedCkSystem();
	afx_msg void OnBnClickedCkDir();
	afx_msg void OnBnClickedCkArtrive();
	afx_msg void OnBnClickedCkHidden();
	afx_msg void OnNMDblclkFat1List(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkFat2List(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif
