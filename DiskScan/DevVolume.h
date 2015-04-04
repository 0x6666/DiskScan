#if !defined(AFX_DEVVOLUME_H__35BAF1AF_9B90_44C6_80F8_17D69E94B21B__INCLUDED_)
#define AFX_DEVVOLUME_H__35BAF1AF_9B90_44C6_80F8_17D69E94B21B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DevVolume.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDevVolume dialog
#define SEL_DISK	1	//ѡ����һ������
#define SEL_VOLUME	2	//ѡ����һ������

class CDevVolumeDlg : public CDialog
{
// Construction
public:
	
	CDevVolumeDlg(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CDevVolume)
	enum { IDD = IDD_DVE_VOLUME };

	//////////////////////////////////////////////////////////////////////////
	//���ѡ���˵��豸����
	//////////////////////////////////////////////////////////////////////////
	CString GetSelDevName();
	CString		m_strSelItem;

	int			m_bSelDisk;	//0 ���� ��SEL_DISKѡ���˴��� �� SEL_VOLUMEѡ���˷���
private:
	CTreeCtrl	m_wndDevVolume;
	//}}AFX_DATA
	HTREEITEM	m_hPhsycDisk;
	HTREEITEM	m_hLogicDrivre;

	//ͼƬ�б�
	std::unique_ptr<CImageList> m_pImageList;
	//��ʼ��ͼ���б�
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
