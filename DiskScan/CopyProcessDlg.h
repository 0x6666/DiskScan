#pragma once



class CDataDoc;
// CCopyProcessDlg 对话框

class CCopyProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CCopyProcessDlg)

public:
	CCopyProcessDlg(CString src , CString dis , CDataDoc* pDoc , CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCopyProcessDlg();

// 对话框数据
	enum { IDD = IDD_PROGRESS };

	//将复制的文件
	CString m_strFileToRead;
	//将要写的文件
	CString	m_strFileToWrite;
	//对应的文档
	CDataDoc* m_pDoc;

	//用于线程同步
	volatile BOOL	m_bIsCopying;

	//拷贝线程
	HANDLE m_hCopyThread;

	//////////////////////////////////////////////////////////////////////////
	//文件拷贝线程中文件打开失败
	//param
	//		wParma=>CString*	失败的路径
	//////////////////////////////////////////////////////////////////////////
	afx_msg LRESULT OnOpenFileFaild(WPARAM wParam , LPARAM lParam);

	//////////////////////////////////////////////////////////////////////////
	//复制文件失败
	//param
	//		wParam=>CString*	被复制的文件
	//		lParam=>CString*	被写的文件
	//////////////////////////////////////////////////////////////////////////
	afx_msg LRESULT OnCopyFileFaild(WPARAM wParam , LPARAM lParam);

	//////////////////////////////////////////////////////////////////////////
	//文件复制结束
	afx_msg LRESULT OnCopySuccess(WPARAM wParam , LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
