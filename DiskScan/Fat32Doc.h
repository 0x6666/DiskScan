#if !defined(AFX_FAT32DOC_H__D878FE6C_6CAD_45FC_A4A9_363C4ACAE473__INCLUDED_)
#define AFX_FAT32DOC_H__D878FE6C_6CAD_45FC_A4A9_363C4ACAE473__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataDoc.h"
#include "..\DiskTool\disktool.h"
#include "Fat32FileAttr.h"
// Fat32Doc.h : header file



/////////////////////////////////////////////////////////////////////////////
// CFat32Doc document

class CFat32Doc : public CDataDoc
{
protected:
	CFat32Doc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFat32Doc)

// Attributes
public:
	DFat32*		m_pFat32;		//FAT32的操作集合
	LONG_INT	m_liStartSec;	//文件系统在当前设备上的其实扇区
	CString		m_strCurPath;	//在内容列表中当前显示的数据的路径
	CString		m_strDevAreaIdx;//设备区域ID,如果当前打开的是一个卷的话此域为 "0"
	CString		m_strDevStartSec;//在设备上的起始扇区号,如果当前打开的是一个卷的话此域为 "0"

	HANDLE		m_hThread;//枚举所有文件
	volatile BOOL   m_bIsRun; //线程是否在运行  多线程处理

	HANDLE		m_hChkDelFile;//查看已经删除到了文件
	volatile BOOL   m_bIsChkDelFileRun; //线程是否在运行  多线程处理

	//获取扇区列表
	HANDLE		m_hGetSectorListThread;
	volatile BOOL	m_bIsGetSeclistRun;

	//图标列表
	CImageList* m_pImgList;




	CFat32FileDlg* m_pDlgFileAttr;//FAT32文件属性对话框
// Operations
public:
	//获得当前列表中要显示的路径
	CString GetCurPath();
	//设置当前要显示的类容
	void    SetCurPath(CString path);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFat32Doc)
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
	//初始化m_pContentList所指向的列表的表头
	//////////////////////////////////////////////////////////////////////////
	virtual	void InitContentListHead();
	
	//////////////////////////////////////////////////////////////////////////
	//获得当前文档所处理的设备的扇区总数,子类必须实现
	//////////////////////////////////////////////////////////////////////////
	virtual LONG_INT GetSecCount();
	
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
	//获得当前文档用来显示详细信息的视图类
	//return 运行时识别的类
	virtual CRuntimeClass* GetInofViewClass();

	//////////////////////////////////////////////////////////////////////////
	//设置当前选择的路径
	//param
	//		strPath	当前选择的路径
	virtual	void SetCurFile(CString strPath);

	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CFat32Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFat32Doc)

	//////////////////////////////////////////////////////////////////////////
	//此消息是m_pContentList的点击事件的相应函数也需要由子类去实现
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnClickContextList(NMHDR* pNMHDR, LRESULT* pResult);  
	
	//////////////////////////////////////////////////////////////////////////
	//此消息是m_pContentList的双击击事件的相应函数也需要由子类去实现
	//////////////////////////////////////////////////////////////////////////
	afx_msg /*virtual*/ void OnDbClickContextList(NMHDR* pNMHDR, LRESULT* pResult);

	//////////////////////////////////////////////////////////////////////////
	//在列表中右击了列表
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnRClickContextList(NMHDR *pNMHDR, LRESULT *pResult);

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

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//定位选择了的列表表象 到数据区域
	afx_msg void OnFat32PosData();
	//文件属性对话框
	afx_msg void OnFat32FileAttr();
	//定位到父目录
	afx_msg void OnFat32PosParaentDir();
	//文件另存为
	afx_msg void OnFat32ServeAs();
	afx_msg void OnUpdateFat32ServeAs(CCmdUI *pCmdUI);
	virtual void OnCloseDocument();

// 	//设置当前需要显示的扇区
// 	void SetCurSector(LONG_INT sector);

protected:
	//////////////////////////////////////////////////////////////////////////
	// 获得列表中选择的文件路径
	//param	
	//		strName 用户返回名字
	CString GetSelPath(CString& strName);

public:
	afx_msg void OnCheckDeleteFile();
	afx_msg void OnUpdateCheckDeleteFile(CCmdUI *pCmdUI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAT32DOC_H__D878FE6C_6CAD_45FC_A4A9_363C4ACAE473__INCLUDED_)
