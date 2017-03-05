#pragma once
#include "NtfsDoc.h"


// CNtfsFileDlg 对话框

class CNtfsDoc;

class CNtfsFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CNtfsFileDlg)

public:
	CNtfsFileDlg(CNtfsDoc* pDoc , CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNtfsFileDlg();

// 对话框数据
	enum { IDD = IDD_NTFS_FILE };


	//NTFS文档类
	CNtfsDoc*	m_pDoc;
	//当前的文件
	std::unique_ptr<DNtfsFile> m_upFile;
	//当前文件路径
	CString		m_strFilePath;

	//////////////////////////////////////////////////////////////////////////
	//设置当前文件的路径
	//param
	//		strSelPath	文件路径
	//return 操作是否已成功
	//////////////////////////////////////////////////////////////////////////
	BOOL SetFilePath( CString strSelPath );

	//////////////////////////////////////////////////////////////////////////
	//更新要显示的数据
	void UpdateFileData();

	//////////////////////////////////////////////////////////////////////////
	//更新DOS属性
	void UpdateDosAttr();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnInitDialog();
	//////////////////////////////////////////////////////////////////////////
	//属性列表的点击事件
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
	afx_msg void OnPosStdAttrHead();	//在当前文件的属性区域选择当前属性的标准属性头
	afx_msg void OnSeverAttr();			//将当前属性保存到文件
//	afx_msg void OnUpdateSeverAttr(CCmdUI *pCmdUI);
	afx_msg void OnPosStdAttrData();	//在当前文件的属性区域选择当前属性的标准属性数据区域
	afx_msg void OnNMRClickNtfsFileAttrList(NMHDR *pNMHDR, LRESULT *pResult);
};
