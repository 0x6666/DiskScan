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
	//������ͼ�и����ؼ���λ��
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
	//��ʾ��Ϣ��Ҫ����
	//////////////////////////////////////////////////////////////////////////
	CFont m_Font;

	//һ���ַ���ʾ��Ҫ�Ŀռ��С
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
	// ���������ʾ��Ϣ����ͼ��
	//CRuntimeClass* GetInfoViewClass(void);
public:
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};



//#define  FIRST_CTRL_RECT CRect( 11 , 5 , 78 , 23)

//���ڿؼ��Ĳ���
//CStatic�ؼ��Ĵ���
#define CRT_STATIC(pST/*�ؼ�*/ , id /*�ؼ�ID*/ , title /*�ؼ�����ID*/) do{	\
		CString strTemp;													\
		strTemp.LoadString(title);											\
		pST.Create(strTemp  , WS_CHILD | WS_VISIBLE | SS_LEFT  ,			\
		/*CRect( 11 , 5 , 78 , 23)*/CRect(0,0,0,0), this , id );								\
		pST.SetFont(&m_Font , FALSE);										\
		}while(0)

//������Ͽ�(������ʾ��)
#define CRT_STATIC_AREA(st/*Ҫ�������Ŀؼ�*/ ,id/*�ؼ�ID*/ , title/*����*/) do{	\
			CString strTemp;													\
			strTemp.LoadString(title);											\
			(st).Create(strTemp , BS_CENTER | BS_GROUPBOX | WS_CHILD |			\
			WS_VISIBLE  , CRect( 1 , 1 , 1 , 1) , this  ,	id);				\
			(st).SetFont(&m_Font , FALSE);										\
			}while(0)

//������ʾ�ı༭��ؼ��Ĵ���
#define CRT_EDIT(pST/*�ؼ�*/ , id /*�ؼ�ID*/ , title /*�ؼ�����*/) do{	\
		(pST).Create(WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,	\
		CRect(85, 35, 165, 65), this , (id));							\
		(pST).SetFont(&m_Font);											\
		(pST).SetWindowText(title);										\
		}while(0)



//����������ƿؼ���
#define CRT_SCAN_CTRLS(st , title , stId , et , etId , pre , preid , next , nextId ,first ,firstid , last , lastid )  do{\
		CString  strTemp;														\
		strTemp.LoadString(title);/*��ǩ����*/									\
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


//�ƶ���̬�ؼ�
#define MV_STATIC(et /*��һ��Edit*/ , st /*Ҫ�ƶ��Ŀؼ�*/ , cr/*�ؼ������ͻ���*/ , top /*���ƿؼ��Ķ���λ��*/) do{	\
		CRect ctlRc1;													\
		CString strTemp;												\
		et.GetWindowRect(&ctlRc1);										\
		top += (ctlRc1.Height() + CTRL_Y_GAP);							\
		st.GetWindowText(strTemp);										\
		width = strTemp.GetLength() * this->m_szChar.cx;				\
		st.MoveWindow(cr.left /*+ CTRL_X_GAP*/ , top ,					\
		min(width + CTRL_X_GAP , cr.Width()) , m_szChar.cy , FALSE );	\
		}while(0)

//�ƶ�һ����̬�ؼ� û��ǰһ���༭�ؼ�������
#define MV_STATIC_NO_PRE(st /*Ҫ�ƶ��Ŀؼ�*/ ,cr/*�ؼ������ͻ���*/ , top /*���ƿؼ��Ķ���λ��*/)  do{	\
		CString strTemp;													\
		(st).GetWindowText(strTemp);										\
		width = strTemp.GetLength() * this->m_szChar.cx;					\
		(st).MoveWindow(cr.left  , top ,									\
		min(width + CTRL_X_GAP , cr.Width()) , m_szChar.cy , FALSE );		\
		}while(0)

//�ƶ������
#define MV_STATIC_AREA(et /*�����������Կؼ�*/ , st /*Ҫ�ƶ��Ŀؼ�*/ , topbak/*���ݵĶ���λ��*/ , top/*��һ�ؼ��Ķ���λ��*/) do{ \
		CRect ctlRc1;														\
		(et).GetWindowRect(&ctlRc1);										\
		(st).MoveWindow( cr.left , topbak - 3*CTRL_Y_GAP , cr.Width() ,		\
		ctlRc1.Height()*2 + top + 2 * CTRL_Y_GAP - topbak , FALSE  );		\
	}while(0)

//�ƶ���ʾ���ݵı༭��ؼ�
#define MV_EDITE(st/*��Ӧ��static�ؼ�*/ , et/*Ҫ�ƶ��ĵĿؼ�*/  , cr/*��ǰ�����ͻ���*/ , top /*�ؼ��Ķ���λ��*/) do{\
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


//�ƶ�����ؼ���
//�ƶ��ռ�ĵ�˳��  Ϊ st |<  >|   <   >   Edit
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
