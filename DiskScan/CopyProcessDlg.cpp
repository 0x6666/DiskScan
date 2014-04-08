// CopyProcessDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DiskScan.h"
#include "CopyProcessDlg.h"
#include <afxcmn.h>
#include "Fat32Doc.h"
#include "NtfsDoc.h"


// CCopyProcessDlg �Ի���

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


// CCopyProcessDlg ��Ϣ�������

BOOL CCopyProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//��ʼ�������ؼ�
	CProgressCtrl* pProg = (CProgressCtrl*)this->GetDlgItem(IDC_COPY_PROGRESS);
	pProg->SetRange(0 , 100); //���ý������Ŀռ��С
	pProg->SetStep(1);

	CWnd* pBtn = this->GetDlgItem(IDOK);
	pBtn->EnableWindow(FALSE);
	pBtn = this->GetDlgItem(IDCANCEL);
	pBtn->EnableWindow(TRUE);

	//�������ж���ʲô�ļ�
	if(this->m_pDoc->IsKindOf(RUNTIME_CLASS(CFat32Doc))){//��FAT32�ļ�
		m_hCopyThread = CreateThread(NULL , 0 , CopyFat32File , this , 0 , NULL);
	}else if (this->m_pDoc->IsKindOf(RUNTIME_CLASS(CNtfsDoc)))
	{//��NTFS�ļ�
		m_hCopyThread = CreateThread(NULL , 0 , CopyNtfsFile , this , 0 , NULL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

LRESULT CCopyProcessDlg::OnOpenFileFaild( WPARAM wParam , LPARAM lParam )
{//�ļ�����ʧ��
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


	//��ȡ����ť��Ϊ��� 
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
	{//�Ѿ���������
		//����Դ����������λ�ļ�������������ŵ��߳���ȥ��Ϊ�˱���������ǰGUI�߳�
		CloseHandle(CreateThread(NULL , 0 , PosFileInExplore , &(this->m_strFileToWrite) , 0 , NULL));
		OnOK();
		return;
	}

	if ( NULL != m_hCopyThread || INVALID_HANDLE_VALUE != m_hCopyThread)
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hCopyThread , 1000))
		{//�ȴ���ʱ,Ҳֻ��û�취��ʱ�����ǿ�йر��߳�,
			 if(FALSE != GetExitCodeThread(m_hCopyThread , &dwExitCode))
				TerminateThread(m_hCopyThread , dwExitCode);
		}
	}

	//������ڵĻ���ɾ�������ڿ���ɾ����û�����˵��ļ�
	::DeleteFile(this->m_strFileToWrite);

	OnCancel();
}
