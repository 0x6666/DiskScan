
/////////////////////////////////////////////////////////////////////////////
// DataWnd window
#include "stdafx.h"
#include "DiskScan.h"
/*#include "DataType.h"*/
#include "..\DISKTOOL\disktool.h"	// Added by ClassView
#ifndef	_DATA_CTRL_
#define	_DATA_CTRL_

#define SEC_SIZE			0x200		//һ��ɽ�����ֽ���
#define BYTE_PER_LINE		0x10		//һ�п�����ʾ���ֽ���	
#define LINE_PER_SEC		(SEC_SIZE/BYTE_PER_LINE)//ÿ����������

//////////////////////////////////////////////////////////////////////////
//ϵ��Ϣ��ֵ��Ҫ��ʵ�ʵ�����¸���
//�������ǣ��������ݣ�Ҳ������ؼ����������
//param
//	wParam==>(LONG_INT*)	����ʾ��ϵ�����������,�����SetDataSec���
//							��һ�η��ʹ���Ϣ�����Ϊ-1������Ҳ���ڷ���
//							������Ҫ��ʾ�ĵ����ݵ������š����������ʾ��
//							���ٴ��򷵻� -1
//	lParam==>(BYTE*)		�ڴ�����512�ֽڵ�����Ҳ����wParam���ص�ɽ����
//////////////////////////////////////////////////////////////////////////
//#define DATA_CTRL_MSG		WM_USER + 1000		//�˺���Ҫ��ʵ�ʵ���Ŀ�о�������

//////////////////////////////////////////////////////////////////////////
//֪ͨ�����ڱ��ռ�Ŀ����Ҫ�Ѿ��ı���,
//param
//	wParam==>int			���ڵ��¿��
//////////////////////////////////////////////////////////////////////////
//#define DATA_CHANGE_WIDTH		WM_USER + 1001		//�˺���Ҫ��ʵ�ʵ���Ŀ�о�������

//////////////////////////////////////////////////////////////////////////
//֪ͨ�����ڵ�ǰ��ʾ�����ݵ��������Ѿ��ı���
//param
//		wParam		�����ŵĵ����ֽ�
//		lParam		�����ŵĸ���ֽ�	����Ϊ0
//////////////////////////////////////////////////////////////////////////
//#define DATA_CHANGE_SECTOR		WM_USER + 1002


class DataWnd : public CWnd
{
public:
	void SetSecCount(LONG_INT cnt);
	void SetStartSector(LONG_INT start);
	int GetMinWidth();
	/**********************************************************************
	����һ�����ݿؼ�
	***********************************************************************/
	DataWnd();

	/**********************************************************************
	ʵ�ʴ��������ؼ�
	param
		rc		�ռ���Ҫ��ʾ�ڸ����ڵ�λ��
		pParent	�ؼ��ĸ����� ����ܸ���������Ϣ
	return
		 �Ƿ񴴽��ɹ�
	***********************************************************************/
	BOOL Create(RECT &rc,CWnd *pParent);

	/**********************************************************************
	����Ҫ��ʾ�����ݵ�����(����),�ڴ˷����лᷢ��һ������������Ϣ��DATA_CTRL_MSG��
	param
		liSecStart		Ҫ��ʾ�ĵ�һ��������������
		liSecCount		�ܵ�������
	return
		�����Ƿ�ɹ�
	***********************************************************************/
	BOOL SetDataSec(LONG_INT liSecStart ,LONG_INT liSecCount);

	/**********************************************************************
	���õ�ǰҪ��ʾ������ �˺����ᷢ��DATA_CTRL_MSG��Ϣ ��������,�˺����ĵ���
	�����ȵ���SetDataSec������һ��ʧ��,���ָ����������Խ����Ҳ�ض�ʧ��
	param
		liSec		Ҫ��ʾ��������
	return
		�����Ƿ�ɹ�
	***********************************************************************/
	BOOL SetCurSec(LONG_INT liSec);

	//////////////////////////////////////////////////////////////////////////
	//����ѡ������
	//param
	//		start	ѡ����ֽ�
	//		end		ѡ��Ľ����ֽ�
	//////////////////////////////////////////////////////////////////////////
	void SetSel(LONG_INT start , LONG_INT end);

	virtual ~DataWnd();

	typedef struct _tagDATA_BUF{
		BYTE		mBuf[SEC_SIZE];	//���ݻ��� һ������
		LONG_INT	mOff;			//��ǰ������ƫ��
	}DATA_BUF , *PDATA_BUF;
protected:

	//////////////////////////////////////////////////////////////////////////
	//����mSelStart��mSelEnd����ѡ��һ�Σ�һ������ʾ���ݸı���֮�����õ�
	//////////////////////////////////////////////////////////////////////////
	void ReSel();

	//////////////////////////////////////////////////////////////////////////
	//����ѡ���ѡ�������λ����Ϣ
	//param
	//		nSs	����������ѡ�����ʼλ�� 
	//		nEs	����������ѡ��Ľ���λ��
	//////////////////////////////////////////////////////////////////////////
	void SeveSelPos(int nSs , int nEs);

	//////////////////////////////////////////////////////////////////////////
	//�������ռ�׷��lineCnt������
	//param
	//		lineCnt	Ҫ��ӵ�����
	//////////////////////////////////////////////////////////////////////////
	void AppEndAll(int lineCnt);

	//////////////////////////////////////////////////////////////////////////
	//��ͷ��ѹ��ָ��������,ִ���˴˺�������Ҫִ��UpdateData(0)��������ʾ����
	//param
	//		lineCnt	Ҫ��ͷ����ӵ�����
	//return	�Ƿ�ı��˵�ǰ�����
	//////////////////////////////////////////////////////////////////////////
	BOOL AppHeadAll(int lineCnt);
	
	//��ʼ�������ؼ�
	void InitControl(RECT rc);				
	
	//����ƫ�ƻ�����
	//start		��ʼ�к�
	//lineCnt	����
	void AppEndOff(LONG_INT start ,int lineCnt);

	void AppHeadOff(int lineCnt);
	void AppHeadData(int lineCnt);
	void AppHeadChar(int lineCnt);
	
	//����������ʾ��
	//start		��ʼ�к�
	//lineCnt	����
	void AppEndData(LONG_INT start/*��ʼ�к�*/ ,int lineCnt/*����*/);	
	void AppEndChar(LONG_INT start/*��ʼ�к�*/ ,int lineCnt/*����*/);	//����ƫ�ƻ�����
	void DrawHead(CPaintDC& dc);	//���ƶ����ı���
	BOOL FillBuf(int num , BOOL isNext);//��仺��  0,1��2   isNext�������Ƿ��ȡ��һ������
	void GetLetterSize(CWnd *pParent);//���������ڻ������Ĵ�С
	//////////////////////////////////////////////////////////////////////////
	//ɾ���ֲ���ָ������
	//param
	//		lnC	Ҫɾ�������� 
	//return	֪��ǰ�������б�
	//////////////////////////////////////////////////////////////////////////
	void DelHeadLn(int lnC);//ɾ������ lnC ��
	void DelTailLn(int lnC);//ɾ��β�� lnC ��
	void SyncSel(int type , bool isMsgCall);//ͬ��ѡ����ѡ������  isMsgCall�Ƿ�������Ϣ�������õ�
	void ReSize(RECT rc);//���ÿռ��λ��
/*	void DrawSpLine(void);//����ɽ���ָ���*/
// Attributes
protected:

	BOOL	isFirst;	//�Ƿ񵱴򿪴���,�ƵĻ�һЩ��Ӧʱ�޷�������

	CEdit*	mEtData;	//������
	CEdit*	mEtOffset;	//ƫ����
	CEdit*	mEtChar;	//�ַ���
	CBrush	mBKBrush;	//����ˢ��
	CFont	mFont;		//��ʾ������

	//�����ռ���������ݱ���
	CString	mDB;		
	CString	mOB;
	CString	mCB;
/*	CScrollBar* mVScro; //��ֱ������*/

	SIZE	mSize;		//һ����д�ַ� "A"�Ļ��Ƴ�����ĳ��Ϳ�
	int		mRadix;		//ƫ�Ƶ���ʾ��ʽ  10 ʮ����  16  ʮ������
	//int		mVMaxLine;	//������ʾ������
	PDATA_BUF  mDBuf;
	int		mCurBuf;		//��ǰ�Ļ���� (��һ�����ڵĻ���� base 0)
	int		mCurBufOff;		//��һ����mCurBuf�Ż����е��к�
	UINT		mLineCnt;		//��ǰ�ؼ��е�������һ��ȫ����ʾ������  һ����������

	LONG_INT	mStartSector;	//������ʾ�ĵ�һ��������
	LONG_INT	mCount;			//������  ������


	LONG_INT mSelStart;		//ѡ�����ʵλ��
	LONG_INT mSelEnd;		//ѡ��Ľ���λ��
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
	void SetOffCharCnt(int cnt);	//����ƫ�������ַ�����ַ���
};

#endif 
