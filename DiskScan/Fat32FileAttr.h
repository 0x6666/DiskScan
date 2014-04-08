#include "Fat32Doc.h"

#ifndef _CFAT32_FILE_ATTR_H_
#define _CFAT32_FILE_ATTR_H_


// CFat32FileAttr 对话框
class CFat32Doc;

class CFat32FileDlg : public CDialog
{
	DECLARE_DYNAMIC(CFat32FileDlg)

public:
	CFat32FileDlg(CFat32Doc* pDoc , CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFat32FileDlg();

// 对话框数据
	enum { IDD = IDD_FAT32_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//当前对话框对应的文件所在的文档
	CFat32Doc*  m_pDoc;
	//当前文件所对应的实际文件的路径
	CString		m_strFilePath;
	//FAT32的文件对象
	DFat32File*	m_pFile;
	//线程
	HANDLE		m_hThread;
	volatile BOOL		m_bIsThreadRun;

	//设置当前要显示的文件的路径
	BOOL SetFilePath(CString file);
	//将数据(重新)显示出来
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
