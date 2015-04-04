#if !defined(AFX_DEVVOLUME_H__35BAF1AF_9B90_44C6_80F8_17D69E94B21B__INCLUDED_)
#define AFX_DEVVOLUME_H__35BAF1AF_9B90_44C6_80F8_17D69E94B21B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DevVolume.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDevVolume dialog
#define SEL_DISK	1	//选择了一个磁盘
#define SEL_VOLUME	2	//选择了一个分区

class CDevVolumeDlg : public CDialog
{
// Construction
public:
	
	CDevVolumeDlg(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CDevVolume)
	enum { IDD = IDD_DVE_VOLUME };

	//////////////////////////////////////////////////////////////////////////
	//获得选择了的设备名字
	//////////////////////////////////////////////////////////////////////////
	CString GetSelDevName();
	CString		m_strSelItem;

	int			m_bSelDisk;	//0 错误 ，SEL_DISK选择了磁盘 ， SEL_VOLUME选择了分区
private:
	CTreeCtrl	m_wndDevVolume;
	//}}AFX_DATA
	HTREEITEM	m_hPhsycDisk;
	HTREEITEM	m_hLogicDrivre;

	//图片列表
	std::unique_ptr<CImageList> m_pImageList;
	//初始化图标列表
	void InitImageList();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevVolume)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitTreeCtrl();

	// Generated message map functions
	//{{AFX_MSG(CDevVolume)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkDevAndVolume(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVVOLUME_H__35BAF1AF_9B90_44C6_80F8_17D69E94B21B__INCLUDED_)
