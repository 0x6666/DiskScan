#if !defined(AFX_NTFSDOC_H__95A2B6E1_7B61_44DF_A00F_21E37DA3D086__INCLUDED_)
#define AFX_NTFSDOC_H__95A2B6E1_7B61_44DF_A00F_21E37DA3D086__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataDoc.h"
#include "..\DISKTOOL\disktool.h"	// Added by ClassView
#include "NtfsFileDlg.h"
#include <afxmt.h>
// NtfsDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNtfsDoc document

class CNtfsFileDlg;

class CNtfsDoc : public CDataDoc
{
protected:
	CNtfsDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNtfsDoc)

// Attributes
public:

// Operations
public:

	public:
//	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:

public:
	virtual ~CNtfsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNtfsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	//////////////////////////////////////////////////////////////////////////
	//打开一个指定的设备或者文档。这个方法不直接调用，由文档模板调用，我们只需
	//要调用对应模板的同名方法即可。参数lpszPathName其格式为
	//	devName [-IDX <索引> -OFF <偏移>]
	//	devName   要打开的设备的名字
	//	索引	  要打开的设备在磁盘上的区域序号  十六进制字符表示
	//  偏移	  要打开的设备在磁盘上的偏移      十六进制字符表示
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	//////////////////////////////////////////////////////////////////////////
	//向当前文档所处理的设备读取数据
	//param
	//		buf		数据缓存
	//		offset	数据在设备的上的扇区偏移
	//		isNext	如果指定的不存在是否要往下一个有效扇区 
	//				TRUE 如果offset不存在则读取下一个有效扇区
	//				FALSE 如果不存在在读取上一个有效扇区
	//		size	将要读取的数据的大小
	//return 读取数据是否成功
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL ReadData(void* buf , PLONG_INT offset , BOOL isNext = TRUE , DWORD size = SECTOR_SIZE);

	//////////////////////////////////////////////////////////////////////////
	//获得当前文档所处理的设备的扇区总数,子类必须实现
	//////////////////////////////////////////////////////////////////////////
	virtual LONG_INT GetSecCount();

	//////////////////////////////////////////////////////////////////////////
	//初始化m_pContentList所指向的列表的表头
	//////////////////////////////////////////////////////////////////////////
	virtual	void InitContentListHead();

	//////////////////////////////////////////////////////////////////////////
	//设置当前选择的路径
	//param
	//		strPath	当前选择的路径
	virtual	void SetCurFile(CString strPath);

	//////////////////////////////////////////////////////////////////////////
	//获得当前文档用来显示详细信息的视图类
	//return 运行时识别的类
	virtual CRuntimeClass* GetInofViewClass();

	//////////////////////////////////////////////////////////////////////////
	//设置当前列表要显示的路径
	//////////////////////////////////////////////////////////////////////////
	void SetCurPath( CString path );//设置当前要显示的路径


	//////////////////////////////////////////////////////////////////////////
	//此消息是m_pContentList的点击事件的相应函数也需要由子类去实现
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnClickContextList(NMHDR* pNMHDR, LRESULT* pResult);  
	
	//////////////////////////////////////////////////////////////////////////
	//此消息是m_pContentList的双击击事件的相应函数也需要由子类去实现
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult);

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示上一个扇区
	afx_msg void OnBnClickedPreSector();

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示下一个扇区
	afx_msg void OnBnClickedNextSector();

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示第一个扇区
	afx_msg void OnBnClickedFirstSector();

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示最后一个扇区
	afx_msg void OnBnClickedLastSector();

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示上一个簇
	afx_msg void OnBnClickedPreClust();

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示下一个扇区
	afx_msg void OnBnClickedNextClust();

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示第一个扇区
	afx_msg void OnBnClickedFirstClust();

	//////////////////////////////////////////////////////////////////////////
	//视图发来的消息，显示最后一个扇区
	afx_msg void OnBnClickedLastClust();

		//////////////////////////////////////////////////////////////////////////
	//在列表中右击了列表,需要显示快捷菜单
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult);


public:
	// Ntfs卷实例
	DNtfs* m_pNtfs;
	// 本文件系统的起始扇区
	LONG_INT m_liStartSec;
	// 当前列表显示的路径
	CString m_strCurPath;
	//文档打开时参数 
	CString m_strOpenParam;	

	//NTFS文件属性对话框
	CNtfsFileDlg* m_pNtfsFileDlg;

	//图标列表
	CImageList* m_pImgList;

	//读取文件列表线程
	HANDLE m_hThread;
	volatile BOOL   m_bIsRun; //线程是否在运行  多线程处理 
/*	CEvent*  m_pEveIsRun;*/

	//打开文件属性对话框
	afx_msg void OnNtfsFileAttr();
	//定位到MFT记录
	afx_msg void OnNtfsPosMft();
	//定位MFT记录表
	afx_msg void OnNtfsPosParaentDir();
	//文件另存为
	afx_msg void OnNtfsServeAs();
	afx_msg void OnUpdateNtfsServeAs(CCmdUI *pCmdUI);

	//获得列表中当前选中的文件/目录的路径
	CString GetSelPath(CString &strName);
	virtual void OnCloseDocument();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTFSDOC_H__95A2B6E1_7B61_44DF_A00F_21E37DA3D086__INCLUDED_)
