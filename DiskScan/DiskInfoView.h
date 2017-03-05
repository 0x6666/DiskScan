#pragma once
#include "InfoView.h"
#include "afxwin.h"
#include "DiskScanDoc.h"



// CDiskInfoView 窗体视图

class CDiskInfoView : public CInfoView
{
	DECLARE_DYNCREATE(CDiskInfoView)

protected:
	CDiskInfoView();           // 动态创建所使用的受保护的构造函数
	virtual ~CDiskInfoView();

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

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();


private: //各种显示信息的控件
	CStatic m_wndSTName;
	CEdit	m_wndETDevName;

	//磁盘可分区大小
	CStatic m_wndSTCanPart;			//可分区大小标签
	CEdit	m_wndETCanPartByte;		//可分区大小字节数
	CEdit	m_wndETCanPartSize;		//可分区大小尺寸

	//未分区大小
	CStatic m_wndSTNoPart;			//未分区大小标签
	CEdit	m_wndETNoPartByte;		//未分区大小字节数
	CEdit	m_wndETNoPartSize;		//未分区大小尺寸

	//每扇区字节数
	CStatic m_wndSTBytePerSector;	
	CEdit	m_wndETBytePerSector;

	//每磁道扇区数 
	CStatic m_wndSTSectorPerTrack;
	CEdit	m_wndETSectorPerTrack;
	
	//每柱面磁道数
	CStatic m_wndSTTracksPerCylinder;
	CEdit	m_wndETTracksPerCylinder;

	//柱面数
	CStatic m_wndSTCylinders;
	CEdit	m_wndETCylinders;


	//磁盘的详细信息的框架，这种框架本来属于CStatic的，
	//但实际上需要使用的属性BS_GROUPBOX是CButton的所以
	//在这里使用CButton了,不过除了在声明是使用CButton之
	//外没其他地方都当CStatic处理
	CButton m_wndSTCanPartArea;

	//不可分区信息
	CButton	m_wndSTCanNotPartArea;

	//磁盘不可分区大小
	CStatic m_wndSTCanNotPart;			//不可分区大小标签
	CEdit	m_wndETCanNotPartByte;		//不可分区大小字节数
	CEdit	m_wndETCanNotPartSize;		//不可分区大小尺寸

	//逻辑驱动区域
	CButton	m_wndSTLgcDrivArea;

	//总分区数
	CStatic m_wndSTPartCnt;		//分区数
	CEdit	m_wndETPartCnt;	

	//主分区数
	CStatic m_wndSTMainPartCnt;
	CEdit	m_wndETMainPartCnt;

	//逻辑分区数
	CStatic m_wndSTLgcPartCnt;
	CEdit	m_wndETLgcPartCnt;



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

// 
// 	//一个字符的大小
// 	CSize	m_szChar;
protected:
	// 重置控件的位置
	int RePositionCtrl(void);

	CDiskDoc* GetDocument();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnDestroy();
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//////////////////////////////////////////////////////////////////////////
	//在当前扇区编辑框上按下了回车键
	//////////////////////////////////////////////////////////////////////////
	void OnEnterCurSector();
};


