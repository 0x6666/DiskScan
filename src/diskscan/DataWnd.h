
/////////////////////////////////////////////////////////////////////////////
// DataWnd window
#include "stdafx.h"
#include "DiskScan.h"
/*#include "DataType.h"*/
#include "..\DISKTOOL\disktool.h"	// Added by ClassView
#ifndef	_DATA_CTRL_
#define	_DATA_CTRL_

#define SEC_SIZE			0x200		//一个山区的字节数
#define BYTE_PER_LINE		0x10		//一行可以显示的字节数	
#define LINE_PER_SEC		(SEC_SIZE/BYTE_PER_LINE)//每扇区的行数

//////////////////////////////////////////////////////////////////////////
//系消息的值需要在实际的情况下给定
//起作用是，请求数据，也是这个控件的数据入口
//param
//	wParam==>(LONG_INT*)	刚显示完毕的数据扇区号,如果是SetDataSec后第
//							第一次发送此消息则此域为-1。此域也用于返回
//							接下来要显示的的数据的扇区号。如果不在显示了
//							则再此域返回 -1
//	lParam==>(BYTE*)		在此填入512字节的数据也就是wParam返回的山区号
//////////////////////////////////////////////////////////////////////////
//#define DATA_CTRL_MSG		WM_USER + 1000		//此宏需要在实际的项目中具体设置

//////////////////////////////////////////////////////////////////////////
//通知父窗口本空间的宽度需要已经改变了,
//param
//	wParam==>int			窗口的新宽度
//////////////////////////////////////////////////////////////////////////
//#define DATA_CHANGE_WIDTH		WM_USER + 1001		//此宏需要在实际的项目中具体设置

//////////////////////////////////////////////////////////////////////////
//通知父窗口当前显示的数据的扇区号已经改变了
//param
//		wParam		扇区号的低四字节
//		lParam		扇区号的搞低字节	可能为0
//////////////////////////////////////////////////////////////////////////
//#define DATA_CHANGE_SECTOR		WM_USER + 1002


class DataWnd : public CWnd
{
public:
	void SetSecCount(LONG_INT cnt);
	void SetStartSector(LONG_INT start);
	int GetMinWidth();
	/**********************************************************************
	构造一个数据控件
	***********************************************************************/
	DataWnd();

	/**********************************************************************
	实际创建出数控件
	param
		rc		空间所要显示在父窗口的位置
		pParent	控件的父窗口 会接受各种请求消息
	return
		 是否创建成功
	***********************************************************************/
	BOOL Create(RECT &rc,CWnd *pParent);

	/**********************************************************************
	设置要显示的数据的区域(扇区),在此方法中会发送一次数据请求消息（DATA_CTRL_MSG）
	param
		liSecStart		要显示的第一个扇区的扇区号
		liSecCount		总的扇区数
	return
		操作是否成功
	***********************************************************************/
	BOOL SetDataSec(LONG_INT liSecStart ,LONG_INT liSecCount);

	/**********************************************************************
	设置当前要显示的扇区 此函数会发送DATA_CTRL_MSG消息 请求数据,此函数的调用
	必须先调用SetDataSec，否则一定失败,如果指定的扇区号越界了也必定失败
	param
		liSec		要显示的扇区号
	return
		操作是否成功
	***********************************************************************/
	BOOL SetCurSec(LONG_INT liSec);

	//////////////////////////////////////////////////////////////////////////
	//设置选择区域
	//param
	//		start	选择的字节
	//		end		选择的结束字节
	//////////////////////////////////////////////////////////////////////////
	void SetSel(LONG_INT start , LONG_INT end);

	virtual ~DataWnd();

	typedef struct _tagDATA_BUF{
		BYTE		mBuf[SEC_SIZE];	//数据缓存 一个扇区
		LONG_INT	mOff;			//当前扇区的偏移
	}DATA_BUF , *PDATA_BUF;
protected:

	//////////////////////////////////////////////////////////////////////////
	//更具mSelStart和mSelEnd重新选择一次，一般是显示内容改变了之后会调用的
	//////////////////////////////////////////////////////////////////////////
	void ReSel();

	//////////////////////////////////////////////////////////////////////////
	//保存选择的选择区域的位置信息
	//param
	//		nSs	在数据区的选择的起始位置 
	//		nEs	在数据区的选择的结束位置
	//////////////////////////////////////////////////////////////////////////
	void SeveSelPos(int nSs , int nEs);

	//////////////////////////////////////////////////////////////////////////
	//向三个空间追加lineCnt行数据
	//param
	//		lineCnt	要添加的行数
	//////////////////////////////////////////////////////////////////////////
	void AppEndAll(int lineCnt);

	//////////////////////////////////////////////////////////////////////////
	//在头部压入指定的行数,执行了此函数后需要执行UpdateData(0)将数据显示出来
	//param
	//		lineCnt	要在头部添加的行数
	//return	是否改变了当前缓存号
	//////////////////////////////////////////////////////////////////////////
	BOOL AppHeadAll(int lineCnt);
	
	//初始化各个控件
	void InitControl(RECT rc);				
	
	//更新偏移缓存区
	//start		起始行号
	//lineCnt	行数
	void AppEndOff(LONG_INT start ,int lineCnt);

	void AppHeadOff(int lineCnt);
	void AppHeadData(int lineCnt);
	void AppHeadChar(int lineCnt);
	
	//更新数据显示区
	//start		起始行号
	//lineCnt	行数
	void AppEndData(LONG_INT start/*起始行号*/ ,int lineCnt/*行数*/);	
	void AppEndChar(LONG_INT start/*起始行号*/ ,int lineCnt/*行数*/);	//更新偏移缓存区
	void DrawHead(CPaintDC& dc);	//绘制顶部的标题
	BOOL FillBuf(int num , BOOL isNext);//填充缓存  0,1，2   isNext不存在是否获取下一个扇区
	void GetLetterSize(CWnd *pParent);//借助父窗口获得字体的大小
	//////////////////////////////////////////////////////////////////////////
	//删除又不的指定行数
	//param
	//		lnC	要删除的行数 
	//return	知否当前扇区号有变
	//////////////////////////////////////////////////////////////////////////
	void DelHeadLn(int lnC);//删除顶部 lnC 行
	void DelTailLn(int lnC);//删除尾部 lnC 行
	void SyncSel(int type , bool isMsgCall);//同步选择框的选择内容  isMsgCall是否是在消息驱动调用的
	void ReSize(RECT rc);//重置空间的位置
/*	void DrawSpLine(void);//绘制山区分割线*/
// Attributes
protected:

	BOOL	isFirst;	//是否当打开窗口,似的话一些响应时无法操作的

	CEdit*	mEtData;	//数据区
	CEdit*	mEtOffset;	//偏移区
	CEdit*	mEtChar;	//字符区
	CBrush	mBKBrush;	//背景刷子
	CFont	mFont;		//显示的字体

	//三个空间关联的数据变量
	CString	mDB;		
	CString	mOB;
	CString	mCB;
/*	CScrollBar* mVScro; //垂直滚动条*/

	SIZE	mSize;		//一个大写字符 "A"的绘制出来后的长和宽
	int		mRadix;		//偏移的显示方式  10 十进制  16  十六进制
	//int		mVMaxLine;	//可以显示的行数
	PDATA_BUF  mDBuf;
	int		mCurBuf;		//当前的缓存号 (第一行所在的缓存号 base 0)
	int		mCurBufOff;		//第一行在mCurBuf号缓存中的行号
	UINT		mLineCnt;		//当前控件中的行数不一定全部显示出来了  一般两个扇区

	LONG_INT	mStartSector;	//可以显示的第一个扇区号
	LONG_INT	mCount;			//总数据  扇区数


	LONG_INT mSelStart;		//选择的其实位置
	LONG_INT mSelEnd;		//选择的结束位置
protected:
	int m_iOffCharCnt;
	BOOL OnCharLBtnDown(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
/*	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);*/
/*	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);*/
	BOOL OnMouseWheel(MSG* pMsg);
	BOOL OnDataLBtnMsMv(MSG* pMsg);
	//BOOL OnCharLBtnMsMv(MSG* pMsg);
	BOOL OnDataLBtnDown(MSG* pMsg);
private:
	void SetOffCharCnt(int cnt);	//设置偏移区的字符宽度字符数
};

#endif 
