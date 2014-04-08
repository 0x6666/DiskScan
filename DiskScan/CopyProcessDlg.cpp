// CopyProcessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DiskScan.h"
#include "CopyProcessDlg.h"
#include <afxcmn.h>
#include "Fat32Doc.h"
#include "NtfsDoc.h"


// CCopyProcessDlg 对话框

IMPLEMENT_DYNAMIC(CCopyProcessDlg, CDialog)

CCopyProcessDlg::CCopyProcessDlg(CString src , CString dis , CDataDoc* pDoc ,CWnd* pParent /*=NULL*/)
	: CDialog(CCopyProcessDlg::IDD, pParent)
	, m_pDoc(pDoc)
	, m_bIsCopying(FALSE)
	, m_hCopyThread(NULL)
{
	this->m_strFileToRead = src;
	this->m_strFileToWrite = dis;
}

CCopyProcessDlg::~CCopyProcessDlg()
{
}

void CCopyProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCopyProcessDlg, CDialog)
	ON_MESSAGE(DMSG_OPEN_FILE_FAILED , OnOpenFileFaild)
	ON_MESSAGE(DMSG_COPY_FILE_FAILED , OnCopyFileFaild)
	ON_MESSAGE(DMSG_COPY_SUCCESS , OnCopySuccess)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CCopyProcessDlg 消息处理程序

BOOL CCopyProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化各个控件
	CProgressCtrl* pProg = (CProgressCtrl*)this->GetDlgItem(IDC_COPY_PROGRESS);
	pProg->SetRange(0 , 100); //设置进度条的空间大小
	pProg->SetStep(1);

	CWnd* pBtn = this->GetDlgItem(IDOK);
	pBtn->EnableWindow(FALSE);
	pBtn = this->GetDlgItem(IDCANCEL);
	pBtn->EnableWindow(TRUE);

	//在这里判断是什么文件
	if(this->m_pDoc->IsKindOf(RUNTIME_CLASS(CFat32Doc))){//是FAT32文件
		m_hCopyThread = CreateThread(NULL , 0 , CopyFat32File , this , 0 , NULL);
	}else if (this->m_pDoc->IsKindOf(RUNTIME_CLASS(CNtfsDoc)))
	{//是NTFS文件
		m_hCopyThread = CreateThread(NULL , 0 , CopyNtfsFile , this , 0 , NULL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LRESULT CCopyProcessDlg::OnOpenFileFaild( WPARAM wParam , LPARAM lParam )
{//文件拷贝失败
	EndDialog(IDCANCEL);

	CString strTitle;
	CString	strMsg;
	strTitle.LoadString(IDS_PROMPT);
	strMsg.LoadString(IDS_OPEN_FALIED);
	strMsg.Replace(STR_POS , *((CString*)wParam));

	::MessageBox(AfxGetMainWnd()->GetSafeHwnd() , strMsg , strTitle , MB_OK | MB_ICONWARNING );

	return 0;
}

LRESULT CCopyProcessDlg::OnCopyFileFaild( WPARAM wParam , LPARAM lParam )
{
	EndDialog(IDCANCEL);

	CString strTitle;
	CString	strMsg;
	strTitle.LoadString(IDS_PROMPT);
	strMsg.LoadString(IDS_COPY_FAILED);

	::MessageBox(AfxGetMainWnd()->GetSafeHwnd() , strMsg , strTitle , MB_OK | MB_ICONWARNING );
	return 0;
}

LRESULT CCopyProcessDlg::OnCopySuccess( WPARAM wParam , LPARAM lParam )
{
	CString strMsg;

	CWnd* pBtn = this->GetDlgItem(IDOK);
	pBtn->EnableWindow(TRUE);


	//将取消按钮改为浏览 
	pBtn = this->GetDlgItem(IDCANCEL);
	pBtn->EnableWindow(TRUE);

	strMsg.LoadString(IDS_SCAN);
	pBtn->SetWindowText(strMsg);


	return 0;
}

void CCopyProcessDlg::OnBnClickedOk()
{
	OnOK();
}

void CCopyProcessDlg::OnBnClickedCancel()
{
	DWORD dwExitCode = 0;


	if (FALSE == m_bIsCopying )
	{//已经拷贝完了
		//打开资源管理器，定位文件，将这个工作放到线程中去是为了避免阻塞当前GUI线程
		CloseHandle(CreateThread(NULL , 0 , PosFileInExplore , &(this->m_strFileToWrite) , 0 , NULL));
		OnOK();
		return;
	}

	if ( NULL != m_hCopyThread || INVALID_HANDLE_VALUE != m_hCopyThread)
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hCopyThread , 1000))
		{//等待超时,也只有没办法的时候才能强行关闭线程,
			 if(FALSE != GetExitCodeThread(m_hCopyThread , &dwExitCode))
				TerminateThread(m_hCopyThread , dwExitCode);
		}
	}

	//如果存在的话就删除，现在可以删除了没有用了的文件
	::DeleteFile(this->m_strFileToWrite);

	OnCancel();
}
