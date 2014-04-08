#pragma once



class CDataDoc;
// CCopyProcessDlg �Ի���

class CCopyProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CCopyProcessDlg)

public:
	CCopyProcessDlg(CString src , CString dis , CDataDoc* pDoc , CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCopyProcessDlg();

// �Ի�������
	enum { IDD = IDD_PROGRESS };

	//�����Ƶ��ļ�
	CString m_strFileToRead;
	//��Ҫд���ļ�
	CString	m_strFileToWrite;
	//��Ӧ���ĵ�
	CDataDoc* m_pDoc;

	//�����߳�ͬ��
	volatile BOOL	m_bIsCopying;

	//�����߳�
	HANDLE m_hCopyThread;

	//////////////////////////////////////////////////////////////////////////
	//�ļ������߳����ļ���ʧ��
	//param
	//		wParma=>CString*	ʧ�ܵ�·��
	//////////////////////////////////////////////////////////////////////////
	afx_msg LRESULT OnOpenFileFaild(WPARAM wParam , LPARAM lParam);

	//////////////////////////////////////////////////////////////////////////
	//�����ļ�ʧ��
	//param
	//		wParam=>CString*	�����Ƶ��ļ�
	//		lParam=>CString*	��д���ļ�
	//////////////////////////////////////////////////////////////////////////
	afx_msg LRESULT OnCopyFileFaild(WPARAM wParam , LPARAM lParam);

	//////////////////////////////////////////////////////////////////////////
	//�ļ����ƽ���
	afx_msg LRESULT OnCopySuccess(WPARAM wParam , LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
