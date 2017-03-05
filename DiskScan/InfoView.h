#if !defined(AFX_INFOVIEW_H__AE0D456E_E45A_43C7_99C6_376A9C9A7FE9__INCLUDED_)
#define AFX_INFOVIEW_H__AE0D456E_E45A_43C7_99C6_376A9C9A7FE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInfoView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CInfoView : public CFormView
{
protected:
	CInfoView();           // protected constructor used by dynamic creation
	CInfoView(UINT nIDTemplate);
	DECLARE_DYNCREATE(CInfoView)

// Form Data
public:
	//{{AFX_DATA(CInfoView)
	enum { IDD = IDD_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

	//////////////////////////////////////////////////////////////////////////
	//调整视图中各个控件的位置
	//////////////////////////////////////////////////////////////////////////
	virtual int RePositionCtrl();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInfoView();

	//////////////////////////////////////////////////////////////////////////
	//显示信息需要字体
	//////////////////////////////////////////////////////////////////////////
	CFont m_Font;

	//一个字符显示需要的空间大小
	CSize m_szChar;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInfoView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// 获得用于显示信息的视图类
	//CRuntimeClass* GetInfoViewClass(void);
public:
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};



//#define  FIRST_CTRL_RECT CRect( 11 , 5 , 78 , 23)

//关于控件的操作
//CStatic控件的创建
#define CRT_STATIC(pST/*控件*/ , id /*控件ID*/ , title /*控件标题ID*/) do{	\
		CString strTemp;													\
		strTemp.LoadString(title);											\
		pST.Create(strTemp  , WS_CHILD | WS_VISIBLE | SS_LEFT  ,			\
		/*CRect( 11 , 5 , 78 , 23)*/CRect(0,0,0,0), this , id );								\
		pST.SetFont(&m_Font , FALSE);										\
		}while(0)

//创建组合框(区域显示框)
#define CRT_STATIC_AREA(st/*要啊创建的控件*/ ,id/*控件ID*/ , title/*标题*/) do{	\
			CString strTemp;													\
			strTemp.LoadString(title);											\
			(st).Create(strTemp , BS_CENTER | BS_GROUPBOX | WS_CHILD |			\
			WS_VISIBLE  , CRect( 1 , 1 , 1 , 1) , this  ,	id);				\
			(st).SetFont(&m_Font , FALSE);										\
			}while(0)

//用于显示的编辑框控件的创建
#define CRT_EDIT(pST/*控件*/ , id /*控件ID*/ , title /*控件标题*/) do{	\
		(pST).Create(WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,	\
		CRect(85, 35, 165, 65), this , (id));							\
		(pST).SetFont(&m_Font);											\
		(pST).SetWindowText(title);										\
		}while(0)



//创建浏览控制控件组
#define CRT_SCAN_CTRLS(st , title , stId , et , etId , pre , preid , next , nextId ,first ,firstid , last , lastid )  do{\
		CString  strTemp;														\
		strTemp.LoadString(title);/*标签名字*/									\
		st.Create(strTemp  , WS_CHILD | WS_VISIBLE | SS_CENTER ,				\
		CRect( 1 , 1 , 1 , 1) , this , stId );									\
		st.SetFont(&m_Font , FALSE);											\
		et.Create( ES_UPPERCASE | WS_CHILD | WS_VISIBLE | ES_CENTER	|WS_BORDER |\
		ES_AUTOHSCROLL, CRect(85, 35, 165, 65), this , etId);					\
		et.SetFont(&m_Font);													\
		pre.Create(_T("<") , BS_CENTER | WS_CHILD | WS_VISIBLE  , CRect( 1 , 1 ,\
		1 , 1) , this  ,	preid);												\
		pre.SetFont(&m_Font , FALSE);											\
		next.Create(_T(">") , BS_CENTER | WS_CHILD | WS_VISIBLE  , CRect( 1 , 1 ,\
		1 , 1) , this  ,	nextId);											\
		next.SetFont(&m_Font , FALSE);											\
		first.Create(_T("|<") , BS_CENTER | WS_CHILD | WS_VISIBLE  , CRect( 1 , 1 \
		, 1 , 1) , this  ,	firstid);											\
		first.SetFont(&m_Font , FALSE);											\
		last.Create(_T(">|") , BS_CENTER | WS_CHILD | WS_VISIBLE  , CRect( 1 , 1\
		, 1 , 1) , this  ,	lastid);											\
		last.SetFont(&m_Font , FALSE);											\
	} while (0)


//移动静态控件
#define MV_STATIC(et /*上一个Edit*/ , st /*要移动的控件*/ , cr/*控件所属客户区*/ , top /*绘制控件的顶部位置*/) do{	\
		CRect ctlRc1;													\
		CString strTemp;												\
		et.GetWindowRect(&ctlRc1);										\
		top += (ctlRc1.Height() + CTRL_Y_GAP);							\
		st.GetWindowText(strTemp);										\
		width = strTemp.GetLength() * this->m_szChar.cx;				\
		st.MoveWindow(cr.left /*+ CTRL_X_GAP*/ , top ,					\
		min(width + CTRL_X_GAP , cr.Width()) , m_szChar.cy , FALSE );	\
		}while(0)

//移动一个静态控件 没有前一个编辑控件作参数
#define MV_STATIC_NO_PRE(st /*要移动的控件*/ ,cr/*控件所属客户区*/ , top /*绘制控件的顶部位置*/)  do{	\
		CString strTemp;													\
		(st).GetWindowText(strTemp);										\
		width = strTemp.GetLength() * this->m_szChar.cx;					\
		(st).MoveWindow(cr.left  , top ,									\
		min(width + CTRL_X_GAP , cr.Width()) , m_szChar.cy , FALSE );		\
		}while(0)

//移动区域框
#define MV_STATIC_AREA(et /*区域外的最后以控件*/ , st /*要移动的控件*/ , topbak/*备份的顶部位置*/ , top/*上一控件的顶部位置*/) do{ \
		CRect ctlRc1;														\
		(et).GetWindowRect(&ctlRc1);										\
		(st).MoveWindow( cr.left , topbak - 3*CTRL_Y_GAP , cr.Width() ,		\
		ctlRc1.Height()*2 + top + 2 * CTRL_Y_GAP - topbak , FALSE  );		\
	}while(0)

//移动显示数据的编辑框控件
#define MV_EDITE(st/*对应的static控件*/ , et/*要移动的的控件*/  , cr/*当前所属客户区*/ , top /*控件的顶部位置*/) do{\
		CRect ctlRc1;														\
		CString	strTemp;													\
		int width;															\
		st.GetWindowRect(&ctlRc1);											\
		et.GetWindowText(strTemp);											\
		width = strTemp.GetLength() * this->m_szChar.cx;					\
		if (ctlRc1.Width() + width + CTRL_X_GAP <= cr.Width())				\
			et.MoveWindow(ctlRc1.Width() + cr.left+ CTRL_X_GAP , top ,		\
			cr.Width() - ctlRc1.Width() - CTRL_X_GAP, ctlRc1.Height() , FALSE );\
		else																\
			et.MoveWindow(cr.left , top += ctlRc1.Height() ,				\
			cr.Width() - CTRL_X_GAP, ctlRc1.Height() , FALSE );				\
		}while(0)


//移动浏览控件组
//移动空间的的顺序  为 st |<  >|   <   >   Edit
#define MV_SCAN_CTRLS(top , cr , btnW ,btnH , st , first , last , pre , next ,et ) do {\
	CRect ctlRc1;				 										\
	st.MoveWindow(cr.left  , top , cr.Width() , m_szChar.cy , FALSE );	\
	st.GetWindowRect(&ctlRc1);											\
	top += ctlRc1.Height() + CTRL_Y_GAP;								\
	first.MoveWindow(cr.left , top , btnW , btnH , FALSE);				\
	last.MoveWindow(cr.right - btnW , top , btnW , btnH , FALSE);		\
	pre.MoveWindow(cr.left + btnW + CTRL_X_GAP , top , btnW , btnH , FALSE);\
	next.MoveWindow(cr.right - 2 * btnW - CTRL_X_GAP , top , btnW , btnH , FALSE);\
	et.MoveWindow(cr.left + 2 * (btnW + CTRL_X_GAP) , top ,cr.Width() - 4 * (btnW + CTRL_X_GAP) , btnH , FALSE);\
	} while (0)



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOVIEW_H__AE0D456E_E45A_43C7_99C6_376A9C9A7FE9__INCLUDED_)
