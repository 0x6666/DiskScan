#pragma once
#include "InfoView.h"
#include "Fat32Doc.h"



// CFat32InfoView 窗体视图

class CFat32InfoView : public CInfoView
{
	DECLARE_DYNCREATE(CFat32InfoView)

protected:
	CFat32InfoView();           // 动态创建所使用的受保护的构造函数
	virtual ~CFat32InfoView();

	//设备信息区域
	CButton m_wndSTDevInfoArea;

	//设备名字
	CStatic m_wndSTName;
	CEdit	m_wndETDevName;

	//设备区域序号
	CStatic m_wndSTDevAreaIdx;
	CEdit	m_wndETDevAreaIdx;

	//起始扇区号
	CStatic m_wndSTStartSector;
	CEdit	m_wndETStartSector;

	//分区信息区域
	CButton m_wndSTPartInfoArea;

	//分区名字
	CStatic m_wndSTPartName;
	CEdit	m_wndETPartName;

	//分区大小
	CStatic m_wndSTPartSize;
	CEdit	m_wndETPartSizeByte;
	CEdit	m_wndETPartSize;

	//总扇区数
	CStatic m_wndSTSectorCount;
	CEdit	m_wndETSectorCount;

	//保留扇区数
	CStatic m_wndSTReserveSector;
	CEdit	m_wndETReserveSector;

	//每簇扇区数
	CStatic m_wndSTSectorPerClust;
	CEdit	m_wndETSectorPerClust;

	//每FAT扇区数
	CStatic m_wndSTSectorPerFAT;
	CEdit	m_wndETSectorPerFAT;

	//第一个FAT位置
	CStatic m_wndSTFAT1;
	CEdit	m_wndETFAT1;

	//第二个FAT位置
	CStatic m_wndSTFAT2;
	CEdit	m_wndETFAT2;

	//FSInfo
	CStatic m_wndSTFSInfo;
	CEdit	m_wndETFSInfo;

	//跟目录扇区
	CStatic m_wndSTRootDir;
	CEdit	m_wndETRootDir;


	//剩余扇区
	CButton m_wndSTRemainSectorArea;

	//剩余扇区的起始扇区号
	CStatic m_wndSTRemainStartSector;	
	CEdit	m_wndETRemainStartSector;

	//剩余扇区数
	CStatic m_wndSTRemainCount;	
	CEdit	m_wndETRemainCount;

	//剩余扇区大小
	CStatic m_wndSTRemainSize;	
	CEdit	m_wndETRemainSize;

	//文件系统
	CStatic m_wndSTFileSys;	
	CEdit	m_wndETFileSys;


	//数据浏览
	CButton	m_wndSTDataScanArea;

	//扇区号
	CStatic m_wndSTSectorScan;
	CEdit	m_wndETSectorNum;

	//上/下一扇区
	CButton m_wndBtnPreSector;	//上一扇区
	CButton m_wndBtnNextSector;	//下一扇区

	//第一/最后扇区
	CButton m_wndBtnFirstSector;//第一扇区
	CButton m_wndBtnLastSector;	//最后一扇区

	//簇号浏览
	CStatic m_wndSTClustScan;
	CEdit	m_wndETClustNum;

	//上/下一簇
	CButton m_wndBtnPreClust;	//上一簇
	CButton m_wndBtnNextClust;	//下一簇

	//第一/最后簇
	CButton m_wndBtnFirstClust; //第一簇
	CButton m_wndBtnLastClust;	//最后一簇

public:
	enum { IDD = IDD_INFOVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//获得当前视图对应的文档
	CFat32Doc* GetDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual int RePositionCtrl();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnEnterCurSector();
	void OnEnterCurClust();
};


