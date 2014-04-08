#include "stdafx.h"
#include "DataWnd.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable:4996)

//ƫ����������
#define  OFFSET_NAME	_T("ƫ��")

//#define SCROLL_W			::GetSystemMetrics(SM_CXVSCROLL)
#define LINE_W				5   //�ָ��ߵĿ��

#define HEAD_H				25		//�����ĸ߶�
//#define DATA_H				(ALL_HEIGHT - HEAD_H)//��������ĸ߶�

//#define OFF_C_W				16		//ƫ��������11���ַ��Ŀ��
#define DATA_C_W			50		//(3*17)�������Ĵ�Сÿ���ֽ������ַ���� ���ڼ��������ո�)
#define CHAR_C_W			18		//�ַ���ʮ�����ַ�

#define BG_COLOR			RGB(255,255,255)//������ɫΪ��ɫ
#define SP_LING_COLOR		RGB(236,233,216)//�ָ��ߵ���ɫ
#define DATA_FONT_COLOR		RGB(0,0,0)		//������ɫΪ��ɫ
#define OFFSET_FONT_COLOR	RGB(0,0,191)	//ƫ�����ֵ���ɫ

//�˰汾vcû�еĺ� 
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

/////////////////////////////////////////////////////////////////////////////
// DataWnd
DataWnd::DataWnd()
{
	isFirst		= TRUE;
	mEtData		= new CEdit;	//������
	mEtOffset	= new CEdit;	//ƫ����
	mEtChar		= new CEdit;	//�ַ���
/*	mVScro		= new CScrollBar;//������*/

	//���ݻ���
	mDBuf	= new DATA_BUF[3];
	::memset(mDBuf , 0 ,3 * sizeof(DATA_BUF));

	mCurBuf			= -1;		//��ǰ�Ļ����
	mCount.QuadPart	= 0;		//������
	mLineCnt		= 0;
	mCurBufOff		= 0;

	mBKBrush.CreateSolidBrush(COLORREF(BG_COLOR)); 

	//mIsCharUnic = FALSE;	//Ĭ��Ϊ���ֽ��ַ���ʽ
	mRadix		= 16;		//Ĭ��ʮ�����Ʒ�ʽ
/*	mVMaxLine	= 0;*/

	//��������Ŀ��  ���ַ��������
	this->m_iOffCharCnt = 11;

	//��ʼʱʲô��ûѡ��
	mSelEnd.QuadPart = -1;
	mSelStart.QuadPart = -1;
}

DataWnd::~DataWnd()
{
	delete mEtData;		//������
	delete mEtOffset;	//ƫ����
	delete mEtChar;		//�ַ���
/*	delete mVScro;		//������*/

	delete[] mDBuf;		//���ݻ���

	mBKBrush.DeleteObject();
}

BOOL DataWnd::Create(RECT &rc,CWnd *pParent)
{
	BOOL bRes;
	GetLetterSize(pParent);	//���������ڻ������Ĵ�С
	bRes = CWnd::Create(_T("STATIC") , 	NULL , WS_CHILD|SS_NOTIFY|WS_CHILD , rc , pParent , IDC_DATA_CTRL);
	
	if (bRes) InitControl(rc);
	return bRes;
}
void DataWnd::GetLetterSize(CWnd *pParent)
{
	//��������
	LOGFONT lf ,oldf;
	CFont f;
	pParent->GetFont()->GetLogFont(&lf);
	oldf = lf; //����ԭ�еĸ���������

	_tcscpy(lf.lfFaceName , _T("����"));
	lf.lfHeight = 16;
	//lf.lfWidth = 10;
	this->mFont.DeleteObject();
	this->mFont.CreateFontIndirect(&lf);
	pParent->SetFont(&mFont ,FALSE);

	//��ȡ����Ĵ�С
	GetTextExtentPoint32(pParent->GetDC()->GetSafeHdc() ,_T("A") , 1 , &mSize);

	f.DeleteObject();
	f.CreateFontIndirect(&oldf);
	pParent->SetFont(&f ,FALSE);
}

BEGIN_MESSAGE_MAP(DataWnd, CWnd)
	//{{AFX_MSG_MAP(DataWnd)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()			//�ؼ��ı�����ɫ
	ON_WM_SIZE()				//�ռ�Ĵ�С�ı���Ϣ
/*	ON_WM_MOUSEWHEEL()*/
/*	ON_MESSAGE(WM_MOUSEWHEEL, OnMouseWheel)*/
/*	ON_WM_VSCROLL()				//��ֱ������*/	
	//}}AFX_MSG_MAP(DataWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DataWnd message handlers

void DataWnd::OnPaint() 
{
	CPaintDC dc(this);
	
	RECT rect ;
	int h = 0 , ww = 0;
	HRGN hHead;
	HBRUSH hb;
	
	//�ͻ����Ĵ�С
//	::GetClientRect(this->GetSafeHwnd() , &rect);
	//::GetWindowRect()
	this->GetWindowRect(&rect);

	//��������Ŀ��
	int nOffW = mSize.cx * m_iOffCharCnt;//ƫ�����Ŀ��
	int nDatW = mSize.cx * DATA_C_W;

	//���ڵĿ��
	//ww = rect.right - rect.left;
	//ww = min(ww , GetMinWidth());
	ww = GetMinWidth();


	//�Ƚ�ͷ���������
	hHead = CreateRectRgn(0 , 0 , ww , HEAD_H);
	hb = CreateSolidBrush(BG_COLOR);
	FillRgn(dc.GetSafeHdc() , hHead , hb);
	DeleteObject(hHead);
	
	DrawHead(dc);		//���ƶ���������
	
	//��ʼ�������ָ���
	HPEN pen = CreatePen(PS_SOLID , LINE_W ,COLORREF(SP_LING_COLOR));
	HGDIOBJ hAld = SelectObject(dc.GetSafeHdc() ,pen);
	dc.MoveTo( 0 , HEAD_H);
	dc.LineTo(ww ,HEAD_H);

	dc.MoveTo(nOffW + LINE_W  , 0);
	dc.LineTo(nOffW + LINE_W , rect.bottom - rect.top);

	dc.MoveTo( nOffW + nDatW +2 * LINE_W, 0);
	dc.LineTo(nOffW + nDatW +2 * LINE_W,rect.bottom - rect.top);
	
	
	//��ԭ
	SelectObject(dc.GetSafeHdc() ,hAld);
	DeleteObject(pen);

}
void DataWnd::InitControl(RECT rc)
{	
	//�������ڵĴ�С
 	RECT wr;
// //	::GetClientRect(this->GetSafeHwnd() , &wr);
	this->GetWindowRect(&wr);	
	
	//wr.right = wr.left + this->GetMinWidth();

	RECT rect;
	//����ƫ����EDIT
	rect.left = 0;
	rect.right = mSize.cx * m_iOffCharCnt + 3;
	rect.top = HEAD_H + LINE_W;
	rect.bottom = /*ALL_HEIGHT*/wr.bottom - wr.top;
	this->mEtOffset->Create(
		WS_CHILD|WS_VISIBLE|ES_UPPERCASE|ES_READONLY/*|ES_NOHIDESEL*/|ES_MULTILINE|ES_CENTER
		,rect,this,IDC_ET_OFFSET);
	this->mEtOffset->SetFont(&mFont);
	this->mEtOffset->SetMargins(this->mSize.cx , this->mSize.cx); //���߶�����һ���ַ��Ŀ��
	
	//����������EDIT
	rect.left = rect.right + LINE_W ;
	rect.right = rect.left + mSize.cx * DATA_C_W;
	this->mEtData->Create(
		WS_CHILD|WS_VISIBLE|ES_UPPERCASE|ES_READONLY|ES_NOHIDESEL|ES_MULTILINE
		,rect,this,IDC_ET_DATA);
	this->mEtData->SetFont(&mFont);
	this->mEtData->SetMargins(mSize.cx , mSize.cx);
	
	//�����ַ���EDIT
	rect.left = rect.right + LINE_W  ;
	rect.right = rect.left + mSize.cx * CHAR_C_W ;
	this->mEtChar->Create(
		WS_CHILD|WS_VISIBLE|ES_READONLY|ES_NOHIDESEL|ES_MULTILINE
		,rect,this,IDC_ET_CHAR);
	this->mEtChar->SetFont(&mFont);
	this->mEtChar->SetMargins(this->mSize.cx , this->mSize.cx);

// 	//��ֱ�������ؼ�
// 	rc.right = GetMinWidth();
// 	this->mVScro->Create(
// 		WS_VISIBLE|WS_CHILD|SBS_VERT |SBS_RIGHTALIGN|SBS_RIGHTALIGN  ,rc,this ,IDC_VSCROLL);
// 	this->mVScro->ShowScrollBar(TRUE);
}
void DataWnd::ReSize(RECT rc)
{
	RECT rect;
	//����ƫ����EDIT
	rect.left = 0;
	rect.right = mSize.cx * m_iOffCharCnt + 3;
	rect.top = HEAD_H + LINE_W;
	rect.bottom = /*ALL_HEIGHT*/rc.bottom - rc.top;
	this->mEtOffset->MoveWindow(&rect , TRUE);

	//����������EDIT
	rect.left = rect.right + LINE_W ;
	rect.right = rect.left + mSize.cx * DATA_C_W;
	this->mEtData->MoveWindow(&rect , TRUE);
	
	//�����ַ���EDIT
	rect.left = rect.right + LINE_W  ;
	rect.right = rect.left + mSize.cx * CHAR_C_W ;
	this->mEtChar->MoveWindow(&rect , TRUE);
	
// 	//��ֱ�������ؼ�
// 	rect.top = 0;
// 	rect.right = min(GetMinWidth() , rc.right - rc.left);
// 	rect.left = rect.right - SCROLL_W ;
// 	this->mVScro->MoveWindow(&rect , TRUE);
// 	this->mVScro->ShowScrollBar(TRUE);

	//���»��ƿؼ�
	this->Invalidate(FALSE);
}
void DataWnd::DoDataExchange(CDataExchange* pDX) 
{
	CWnd::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ET_CHAR, mCB);
	DDX_Text(pDX, IDC_ET_OFFSET, mOB);
	DDX_Text(pDX, IDC_ET_DATA, mDB);
}

void DataWnd::DrawHead(CPaintDC& dc)
{
	int x = 0 ,y = 0;
	int celLen = this->mSize.cx * 3;
	CString buf;
	int i = 0;
	int nOffw = mSize.cx * m_iOffCharCnt;

	//�������ֵ���ɫ
	SetTextColor(dc.GetSafeHdc() , OFFSET_FONT_COLOR);
	::SelectObject(dc.GetSafeHdc() , this->mFont.GetSafeHandle());
	
	y = (HEAD_H - mSize.cy )/2;
	x = (nOffw - mSize.cx * 4)/2;
	//���Ͻǵ�ƫ�Ʊ�־
	dc.TextOut(x , y ,OFFSET_NAME);
	
	for (i = 0 ; i < 16 ; ++i)
	{
		x = LINE_W+  nOffw + celLen * i + this->mSize.cx /2/*���˸о����ﲻӦ�ó�2 �����ǳ��ƺ��ÿ�һ��*/;
		if(i >= 8)	x += this->mSize.cx ;
		x += (celLen - mSize.cx)/2;
		if(this->mRadix == 10)	buf.Format(_T("%d") , i);
		else buf.Format(_T("%X") , i);	//������õĲ���ʮ�����򲻹������� ͳһʮ������(�Ե��������Ӳ�����)
		dc.TextOut(x , y , buf);
	}
}
HBRUSH DataWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// TODO:  �ڴ˸��� DC ���κ�����
	int id = pWnd-> GetDlgCtrlID();
	if(id==IDC_ET_OFFSET 
		||id == IDC_ET_CHAR
		||id == IDC_ET_DATA) 
	{   
		pDC-> SetBkColor(COLORREF(BG_COLOR)); 
		return   (HBRUSH) mBKBrush.GetSafeHandle(); 
	}else{ 
		return CWnd::OnCtlColor(pDC,   pWnd,   nCtlColor); 
	}
}

void DataWnd::AppEndAll(int lineCnt)
{
	//�����������������������
	ASSERT(lineCnt <= LINE_PER_SEC * 2);
	if(0 == lineCnt)	return;

	//��ǰʹ�Ļ�����
	DATA_BUF* buf = mDBuf + mCurBuf;
	
	//Ҫ��ӵĵ�һ�е��к�(��Ե�ǰ����)
	UINT bfLnOff = this->mCurBufOff + this->mLineCnt;
	//Ҫ��ӵĵ�һ�е�ʵ����ƫ�ƣ���Դ�����ʼλ�ã�
	LONG_INT off;
	off.QuadPart= buf->mOff.QuadPart * LINE_PER_SEC + bfLnOff;

	int no1;
	//�ж����һ���Ƿ�����һ��������
	int no = (bfLnOff + lineCnt -1) / LINE_PER_SEC;

	if(no /*== 2*/){//��Ҫ������һ�����ݻ���
		no1 = (mCurBuf + no) % 3;//���һ�����ڻ����
		no = (mCurBuf + no - 1) % 3;//���һ�����ڵĻ����ǰһ������
		
		if(mDBuf[no1].mOff.QuadPart != mDBuf[no].mOff.QuadPart +1)
		{//��һ�����������ǺϷ�������  ��Ҫ��ȡ
			//����������
			mDBuf[no1].mOff.QuadPart = mDBuf[no].mOff.QuadPart +1;
			FillBuf(no1 , TRUE);//��仺��
			if(  -1 == mDBuf[no1].mOff.QuadPart)//������ʾ��
			{//û����һ��������û������
				//��Ҫ�޸�����

				//���Ҫ��ӵĵ�һ�о�����һ������Ļ���ֱ�ӷ���
				if (0 != bfLnOff / LINE_PER_SEC)
				{
					lineCnt = 0;
					this->UpdateData(FALSE);
					return ;
				}

				//��ǰ���������ݿ������
				lineCnt = LINE_PER_SEC - mCurBufOff;
				if (!lineCnt)
					return;
			}
		}	
	}
  
	AppEndOff(off ,lineCnt);
	AppEndData(off ,lineCnt);
	AppEndChar(off ,lineCnt);
	//��������
	mLineCnt += lineCnt;
	this->UpdateData(FALSE);
}
BOOL DataWnd::AppHeadAll( int lineCnt )
{
	BOOL res = FALSE;
	//���������2������
	ASSERT(lineCnt <= 2 * LINE_PER_SEC);
	if(0 == lineCnt)	//û��ʲôҪ��ӵ�
		return res;

	//��ǰ�Ļ�����
	DATA_BUF* buf = mDBuf + mCurBuf;	
	int no = 0;
	
	if(mCurBufOff < lineCnt )
	{//��Ҫʹ����һ��������������
		no = (mCurBuf + 2) % 3;  //��һ����������
		if( mDBuf[no].mOff.QuadPart != buf->mOff.QuadPart - 1 )
		{//��һ����������ݻ�û�ж�ȡ����
			//����������
			mDBuf[no].mOff.QuadPart = buf->mOff.QuadPart - 1;
			FillBuf(no , FALSE);//��仺��
			if(mDBuf[no].mOff.QuadPart == -1)//������ʾ��
				return res;
		}	

		//��ǰ�������б仯
		res = TRUE;
	}
	AppHeadOff(lineCnt);//��ƫ�����������
	AppHeadData(lineCnt);
	AppHeadChar(lineCnt);
	//��������
	this->mLineCnt += lineCnt;
	UpdateData(FALSE);//��������
	
	return res;
}
void DataWnd::AppEndOff(LONG_INT start/*��ʼ�к�*/ ,int lineCnt/*����*/)
{
	if(!lineCnt) return;
	
	CString buf , temp;
	int index = 0 ,nSC = 0, nEC = 0 ,i = 0;
	LONG_INT tempLi;
	start.QuadPart *= BYTE_PER_LINE;  //ת���ֽ�ƫ��
	
	for(i = 0 ;i < lineCnt ; ++i){

		if(this->mRadix == 10)
		{//ʮ����
			if (start.QuadPart % SEC_SIZE == 0 )
			{//һ��ɽ������ʵ
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				temp.Format(_T("%d%d\r\n") , tempLi.HighPart , tempLi.LowPart);
				if (tempLi.QuadPart)
					temp.Format(_T("%d%d\r\n") , tempLi.HighPart , tempLi.LowPart);
				else
					temp.Format(_T("%d\r\n") ,  tempLi.LowPart);
			}else
				temp.Format(_T("%d%d\r\n") , start.HighPart , start.LowPart);
		}else{//ʮ������
			if (start.QuadPart % SEC_SIZE == 0 )
			{//������
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				if (tempLi.HighPart)//�и�λ
					temp.Format(_T("%X%X#\r\n") , tempLi.HighPart , tempLi.LowPart);
				else//û�и�λ
					temp.Format(_T("%X#\r\n") ,  tempLi.LowPart);
			}else //�ֽ�ƫ��
				temp.Format(_T("%X%08X\r\n") , start.HighPart , start.LowPart);
		}
		//׷�ӽ�Ҫ��ʾ������
		buf += temp;

		//�ж��Ƿ���Ҫ�ı䴰�ڵĴ�С
		if(start.QuadPart % SEC_SIZE != 0){
			temp.TrimRight();
			if (temp.GetLength() + 2 != m_iOffCharCnt )
				SetOffCharCnt(temp.GetLength());
		}
		//���潫Ҫ��ʾ��ƫ��
		start.QuadPart += 16;
	}
	this->mOB += buf;
}
void DataWnd::AppEndData(LONG_INT start/*��ʼ�к�*/ ,int lineCnt/*����*/)
{
	CString buf , temp;
	int index = 0 ,nSC = 0 , nEC = 0 , i = 0 , j = 0;
	BYTE b;
	DATA_BUF* dBuf = mDBuf + mCurBuf;		//��ǰ����
	UINT bfLnOff = (UINT)(start.QuadPart -\
		dBuf->mOff.QuadPart * LINE_PER_SEC); //��ǰ�������ƫ��
	int no  = 0 ;//��ǰ�����ƫ��


	for(i = 0 ;i < lineCnt ; ++i){
		no = (bfLnOff + i) / LINE_PER_SEC;
		no = (mCurBuf + no)%3;//��һ�������
		//buf+=_T(" ");
		for (j = 0 ; j < BYTE_PER_LINE ; ++j)
		{
			b = mDBuf[no].mBuf[((bfLnOff + i) % LINE_PER_SEC)*BYTE_PER_LINE + j];
			temp.Format(_T("%02X ") , b);
			if(j == 7)
				temp+=_T(" ");
			buf += temp;
		}
		buf.TrimRight();
		buf+=_T("\r\n");
	}

	this->mDB += buf;
}
void DataWnd::AppEndChar(LONG_INT start/*��ʼ�к�*/ ,int lineCnt/*����*/)
{
	CString buf;
	int index = 0 ,nSC = 0 , nEC = 0 , i = 0 , j = 0;
	DATA_BUF* dBuf = mDBuf + mCurBuf;		//��ǰ����
	UINT bfLnOff = (UINT)(start.QuadPart -\
		dBuf->mOff.QuadPart * LINE_PER_SEC); //��ǰ�������ƫ��
	int no  = 0 ;//��ǰ�����ƫ��
	W_CHAR ww;


	BYTE temp[18] = {0};		
	for(i = 0 ;i < lineCnt ; ++i){
		no = (bfLnOff + i) / LINE_PER_SEC;
		no = (mCurBuf + no)%3;//��һ�������

		::memcpy(temp , mDBuf[no].mBuf + ((bfLnOff + i) % LINE_PER_SEC)*BYTE_PER_LINE , BYTE_PER_LINE);
		temp[16] = 0;//��Ч����,�����ж�
		temp[17] = 0;//��Ч���룬�����ж�

		//if(mIsCharUnic){//CString��+=�б���ת���Ĺ���
		//	buf+=(WCHAR*)temp;				
	//	}else{
		for (j = 0 ; j < 16 ; ++j)
		{
			ww.char1 = temp[j];
			ww.char2 = temp[j+1];
			if (IsValidCode(ww , CP_GBK)){ //��������һ����Ч��gbk����
				++j;
				continue;
			}

			//������Ч��gbk����ʱ�ж��ǲ���ascll��
			if(temp[j] < 0x20 || temp[j] > 0x7E)	
				temp[j] =  '.';

// 			if(!IsValidCode(ww , CP_GBK)/*IsGBKHead(temp[j]) && !IsGBKTail(temp[j+1])*/)
// 				temp[j] =  '.';
		}
// 		if((!IsGBKHead(temp[14])&&temp[15] >= 0xFF)||IsGBKHead(temp[15]) )
// 			temp[15] =  '.';

		temp[16] = 0;
		buf+=(char*)temp;	
	//	}
		buf+=_T("\r\n");
	}

	this->mCB += buf;

}
void DataWnd::AppHeadOff(int lineCnt)
{
	LONG_INT start , tempLi;
	CString buf , temp;
	int i = 0;

	//Ҫ��ӵĵ�һ���ֽ�ƫ��
	start.QuadPart =BYTE_PER_LINE*(mDBuf[mCurBuf].mOff.QuadPart * LINE_PER_SEC + mCurBufOff - lineCnt);  //ת���ֽ�ƫ��

	for(i = 0 ;i < lineCnt ; ++i){
		if(this->mRadix == 10)
		{
			if (start.QuadPart % SEC_SIZE == 0 )
			{//һ��ɽ������ʵ
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				if (tempLi.QuadPart)
					temp.Format(_T("%d%d\r\n") , tempLi.HighPart , tempLi.LowPart);
				else
					temp.Format(_T("%d\r\n") , tempLi.LowPart);
			}else
				temp.Format(_T("%d%d\r\n") , start.HighPart , start.LowPart);
		}else{
			if (start.QuadPart % SEC_SIZE == 0 )
			{//������
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				if (tempLi.HighPart)//�и�λ
					temp.Format(_T("%X%X#\r\n") , tempLi.HighPart , tempLi.LowPart);
				else//û�и�λ
					temp.Format(_T("%X#\r\n") ,  tempLi.LowPart);
			}else
				temp.Format(_T("%X%08X\r\n") , start.HighPart , start.LowPart);
		}
		start.QuadPart += 16;
		buf +=temp;
	}
	this->mOB= buf + mOB;    //��������ӵ�ͷ��
}
void DataWnd::AppHeadData(int lineCnt)
{
	CString buf ,temp; 
	int i = 0 , j = 0 ,no = 0;
	BYTE b;
	DATA_BUF* dBuf = mDBuf + mCurBuf;		//��ǰ����
	UINT bfLnOff = (UINT)(mDBuf[mCurBuf].mOff.QuadPart * LINE_PER_SEC \
		+ mCurBufOff - lineCnt)%LINE_PER_SEC; //ʵ�ʵ��к�

	for(i = 0 ;i < lineCnt ; ++i){
		//��ǰ��Ҫ��ӵ������ڻ����
		no = (lineCnt - i > mCurBufOff)?(mCurBuf + 2)%3:mCurBuf;

		for (j = 0 ; j < BYTE_PER_LINE ; ++j)
		{//ȡ��һ���е�ÿһ���ֽ�
			b = mDBuf[no].mBuf[((bfLnOff + i) % LINE_PER_SEC)*BYTE_PER_LINE + j];
			temp.Format(_T("%02X ") , b);
			if(j == 7)
				temp+=_T(" ");
			buf += temp;
		}
		//ȥ�����Ŀո�
		buf.TrimRight();
		buf+=_T("\r\n");
	}
	this->mDB= buf + mDB;
}
void DataWnd::AppHeadChar(int lineCnt)
{
	CString buf;
	int i = 0 , j = 0 , no = 0;
	BYTE temp[18] = {0};	
	W_CHAR ww;

	DATA_BUF* dBuf = mDBuf + mCurBuf;		//��ǰ����
	UINT bfLnOff = (UINT)(mDBuf[mCurBuf].mOff.QuadPart * LINE_PER_SEC \
		+ mCurBufOff - lineCnt)%LINE_PER_SEC; //ʵ�ʵ��к�

	for(i = 0 ;i < lineCnt ; ++i){
		//��ǰ��Ҫ��ӵ������ڻ����
		no = (lineCnt - i > mCurBufOff)?(mCurBuf + 2)%3:mCurBuf;

		::memcpy(temp , mDBuf[no].mBuf + ((bfLnOff + i) % LINE_PER_SEC)*BYTE_PER_LINE , BYTE_PER_LINE);
		temp[16] = 0;
		temp[17] = 0;

		//if(mIsCharUnic){//CString��+=�б���ת���Ĺ���
		//	buf+=(WCHAR*)temp;				
		//	}else{
		for (j = 0 ; j < 16 ; ++j)
		{
			ww.char1 = temp[j];
			ww.char2 = temp[j+1];
			if (IsValidCode(ww , CP_GBK)){ //��������һ����Ч��gbk����
				++j;
				continue;
			}
			
			//������Ч��gbk����ʱ�ж��ǲ���ascll��
			if(temp[j] < 0x20 || temp[j] > 0x7E)	
				temp[j] =  '.';
// 			if((temp[j] < 0x20 || temp[j] > 0x7E)&&(!IsGBKHead(temp[j])))	
// 				temp[j] =  '.';
// 			if(IsGBKHead(temp[j]) && !IsGBKTail(temp[j+1]))
// 				temp[j] =  '.';
		}
// 		if((!IsGBKHead(temp[14])&&temp[15] >= 0xFF)||IsGBKHead(temp[15]) )
// 			temp[15] =  '.';
		temp[16] = 0;
		buf+=(char*)temp;	
		//	}
		buf+=_T("\r\n");
	}
	this->mCB= buf + mCB;
}
void DataWnd::OnSize(UINT nType, int cx, int cy)
{
  	CRect rect;
//  	int h = 0;
// 	
// 	//���ƿؼ��߿�
 	::GetClientRect(this->GetSafeHwnd() , &rect);
// 	this->GetWindowRect(&rect);
// 		
// 	//���������ʾ����������
// 	h = rect.Height() - HEAD_H - LINE_W;
// 	mVMaxLine = h/mSize.cy;
	
	//����ƫ�����Ĵ�С
	if (::IsWindow(this->mEtOffset->GetSafeHwnd()))
		ReSize(rect);

}
BOOL DataWnd::PreTranslateMessage(MSG* pMsg)
{
	//static bool isFirst = true;
	//����ESC��
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
		return TRUE;

	//û�м��ع����ݵĻ� ��ʲô������
	if (mCurBuf == -1) 		return CWnd::PreTranslateMessage(pMsg);

	//��õ�ǰ����λ��
	int xPos = GET_X_LPARAM(pMsg->lParam); 
	int yPos = GET_Y_LPARAM(pMsg->lParam); 
	CRect rect;
	
	if (pMsg->message == WM_MOUSEWHEEL)		//��������Ϣ
	{
		if(!OnMouseWheel(pMsg))
			return CWnd::PreTranslateMessage(pMsg);
		else 
			return TRUE;
	}
	
	//��ס����ƶ�
	if( (pMsg->wParam & MK_LBUTTON) && (pMsg->message == WM_MOUSEMOVE))
	{
		if (isFirst)
			return CWnd::PreTranslateMessage(pMsg);
// #ifdef _DEBUG
// 		AfxMessageBox(_T("����ƶ�"));
// #endif // _DEBUG

		//������ӿؼ���λ��
		mEtData->GetWindowRect(&rect);
		if(rect.PtInRect(pMsg->pt)){
			if(!OnDataLBtnMsMv(pMsg))
				return CWnd::PreTranslateMessage(pMsg);
			else  return TRUE;
		}

		mEtChar->GetWindowRect(&rect);
		if (rect.PtInRect(pMsg->pt))
		{	//���ַ���ʾ��
			//���ַ�����ѡ��ͬ����������
			SyncSel(2 , true);

			//if(!OnCharLBtnMsMv(pMsg))
			return CWnd::PreTranslateMessage(pMsg);
			//else return TRUE;
		}

// 		mEtOffset->GetWindowRect(&rect);
// 		if (rect.PtInRect(pMsg->pt))
// 		{	//��ƫ����
// 
// 		}
	}
	
	//����󰴼��¼�
	if(pMsg->message == WM_LBUTTONDOWN ||pMsg->message == WM_LBUTTONDBLCLK )
	{
// 		CWnd * wndFocus = GetFocus();
// 		if (this != wndFocus &&
// 			mEtChar != wndFocus &&
// 			mEtData != wndFocus &&
// 			mEtOffset != wndFocus)
// 			return TRUE;

		isFirst = false;

		//������ӿؼ���λ��
		mEtData->GetWindowRect(&rect);
		if(rect.PtInRect(pMsg->pt)){
			if(!OnDataLBtnDown(pMsg)) 
				return CWnd::PreTranslateMessage(pMsg);
			else
				return TRUE;
		}

		mEtChar->GetWindowRect(&rect);
		if (rect.PtInRect(pMsg->pt))
		{	//���ַ���ʾ��
			if(!OnCharLBtnDown(pMsg)) 
				return CWnd::PreTranslateMessage(pMsg);
			else
				return TRUE;
		}
// 		mEtOffset->GetWindowRect(&rect);
// 		if (rect.PtInRect(pMsg->pt))
// 		{	//��ƫ����
// 
// 		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}
// 
// void DataWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pSBar)
// {
// 	if(pSBar != this->mVScro) return;
// 
// 	//��������Ϣ
// 	SCROLLINFO scif;
// 	int i = 0;
// 	//��õ�ǰ����������Ϣ
// 	pSBar->GetScrollInfo(&scif,SIF_ALL);   
// 	i = scif.nPos;
// 	switch (nSBCode)   
// 	{  
// 	case SB_BOTTOM:		//������ĩβ
// 		//pSBar->sroScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMax));   
// 		scif.nPos = scif.nMax;
// 		
// 		break;   
// 	case SB_TOP:		//�������� 
// 		scif.nPos = 0;
// 		break;   
// 	case SB_LINEUP:		//����һ�� 
// 		scif.nPos = scif.nPos -2;
// 		if(scif.nPos < scif.nMin)
// 			scif.nPos  = scif.nMin;
// 		break;   
// 	case SB_LINEDOWN:	//��һ��
// 		scif.nPos = scif.nPos +2;
// 		if (scif.nPos > scif.nMax)     
// 			scif.nPos = scif.nMax;   
// 		break; 
// 	case SB_PAGEUP:		//��һҳ
// 		scif.nPos -= 5;
// 		if (scif.nPos<scif.nMin)
// 			scif.nPos = scif.nMin;    
// 		break;   
// 	case SB_PAGEDOWN:   
// 		scif.nPos += 5; 
// 		if (scif.nPos > scif.nMax)
// 			scif.nPos = scif.nMax;
// 		break;   
// 	case SB_THUMBTRACK:  
// 		scif.nPos = nPos;   
// 		break;   
// 	}
// 	if(i != scif.nPos)
// 		pSBar->SetScrollPos(scif.nPos);
// 
// 	CWnd::OnVScroll(nSBCode, nPos, pSBar);
// }
// 
// BOOL DataWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
// {
// 	//��ɾ����
// //	int zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
// 	int lc = 0 ;  //����
// 	int toDel = 0;
// 
// 	AfxMessageBox(_T("�յ�������Ϣ"));
// 	
// 	//��û�м��ع�����
// 	if (mCurBuf == -1)
// 		return TRUE;
// 	
// 	if(zDelta < 0){   //ҳ��������
// 		//�ж��Ƿ����������
// 		if(mLineCnt <= this->mVMaxLine) //��������
// 			return TRUE;
// 		lc = (zDelta / (-120)) * 2 ;
// 		lc = 3;
// 		DelHeadLn(lc);
// 		this->mLineCnt -= lc;
// 		this->mCurBufOff += lc;
// 		if(mCurBufOff / LINE_PER_SEC > 0){	//����ʾ��һ���������ˣ���
// 			mCurBuf = (mCurBuf + 1) % 3;	//��һ������
// 			mCurBufOff %= LINE_PER_SEC;			//�ߵ�ƫ��
// 		}
// 		AppEndAll(lc);
// 		
// 	}else if(zDelta > 0)
// 	{//ҳ��������
// 		DATA_BUF* dBuf = mDBuf + mCurBuf;
// 		if(dBuf->mOff.QuadPart <= 0 && mCurBufOff == 0)//�������¹���
// 			return TRUE;
// 		lc = (zDelta / 120) * 2 ;
// 		AfxMessageBox("������");
// 		
// 		lc = 3;
// 		if(!lc)	return TRUE;//û��Ҫ����������
// 		
// 		
// 		//ɾ��β���ļ�������
// 		toDel = (mLineCnt + lc)%(LINE_PER_SEC * 2);//Ҫɾ��������
// 		if(toDel){//���ݳ����� ���������Ļ���   ��Ҫɾ��
// 			DelTailLn(toDel);
// 			mLineCnt -= toDel;
// 		}
// 		AppHeadAll(lc);		//���¸�����ʾ��
// 		//���㵱ǰ�����ƫ��
// 		mCurBufOff -= lc;
// 		if (mCurBufOff < 0)
// 		{//������һ������
// 			mCurBuf = (mCurBuf + 2)%3;//��һ�������
// 			mCurBufOff += LINE_PER_SEC; //��һ��������ƫ��
// 		}
// 	}
// 	return TRUE;
// 	
// 	return DataWnd::OnMouseWheel(nFlags, zDelta, pt);
// }

BOOL DataWnd::OnMouseWheel(MSG* pMsg)
{
	//��ɾ����
	short	zDelta =  (short) HIWORD(pMsg->wParam);
	int		lc = 0 ;  //����
	int		toDel = 0;
	DATA_BUF* dBuf = NULL;

	BOOL isCurSecChg = FALSE;		//��ǰ�����Ƿ��б仯

	//��û�м��ع�����
	if (-1 == mCurBuf || 0 == zDelta)
		return TRUE;
	
	if(zDelta < 0){   //ҳ��������  ��Ҫ��ʾ���������
		//�ж��Ƿ����������
		
		//��Ҫ��ѯ�����ļ���С������ʾ������
		TRACE0("��Ҫ��ѯ���ٿ�����ʾ������");

		if(mLineCnt <= 5) //��������
			return TRUE;
		lc = (zDelta / (-WHEEL_DELTA)) * 2 ;

		DelHeadLn(lc);
		this->mLineCnt -= lc;
		this->mCurBufOff += lc;
		if(mCurBufOff / LINE_PER_SEC > 0)//����ʾ��һ���������ˣ���
		{					
			mCurBuf = (mCurBuf + 1) % 3;	//��һ������
			mCurBufOff %= LINE_PER_SEC;		//�ߵ�ƫ��
			isCurSecChg = TRUE;				//��ǰ�Ⱥ��б仯
		}

		//�ں����������
		AppEndAll(lc);
		
	}else if(zDelta > 0)   //��Ҫ��ʾǰ�������
	{//ҳ��������
		dBuf = mDBuf + mCurBuf;  //��ǰ����

		//Ҫ�ƶ�������
		lc = (zDelta / WHEEL_DELTA) * 2 ;
		
		if(mStartSector.QuadPart == dBuf->mOff.QuadPart )
		{//�������¹���
			
			if(mCurBufOff == 0) //�Ѿ�û��ʲôҪ��ʾ����
				return TRUE;

			if (mCurBufOff < lc)
			{//�ڵ�һ����������������������
				lc = mCurBufOff;
			}
		}
		

		if(0 == lc)//û��Ҫ����������
			return TRUE;
				
		//ɾ��β���ļ�������
		
		if (mLineCnt + lc > LINE_PER_SEC * 2)
		{//����Ҫ��ɾ����
			toDel = (mLineCnt + lc)%(LINE_PER_SEC * 2);//Ҫɾ��������
			if(toDel){//���ݳ����� ���������Ļ���   ��Ҫɾ��
				DelTailLn(toDel);
				mLineCnt -= toDel;
			}
		}

		AppHeadAll(lc);//���¸�����ʾ��		
		//���㵱ǰ�����ƫ��
		mCurBufOff -= lc;
		if (mCurBufOff < 0)
		{//������һ������
			mCurBuf = (mCurBuf + 2) % 3;//��һ�������
			mCurBufOff += LINE_PER_SEC; //��һ��������ƫ��
			isCurSecChg = TRUE;
		}
	}

	
	if (isCurSecChg)//����Ҫ�Ļ��򸸴��ڷ�����Ϣ
	{
		dBuf = mDBuf + mCurBuf;  //��ǰ����
		this->GetParent()->SendMessage(DATA_CHANGE_SECTOR , dBuf->mOff.LowPart , dBuf->mOff.HighPart);
	}


	ReSel();
	return TRUE;
}

BOOL DataWnd::OnDataLBtnMsMv(MSG* pMsg)
{
	//�κ����ұ�ʾ������   
	//���ǵ�ǰ����λ��  xPos yPos
	//��ͨ���˵����ַ���ƫ��
	//֮����Ǽ����ַ������е�ƫ��
	//һ�п�����ʾ 16���ֽڣ�ÿ���ֽ��������ַ���ʾ
	//ÿ��ʾһ���ֽں���һ���ո�Ҳ���൱��һ���ֽ���Ҫ3���ַ�����ʾ�ռ� 
	//��ʾ��8���ֽ�֮����һ���ո����
	//��β�� \r\n  
	//Ҳ����˵ÿ16���ֽ���Ҫ 16*3+2 = 50���ַ��ռ�
	//�������ϵ������Ϳ��Լ���ѡ���ı��ķ�ʽ��


	//��ǰ������豸�ͻ�����λ��
	int xPos = GET_X_LPARAM(pMsg->lParam); 
	int yPos = GET_Y_LPARAM(pMsg->lParam); 

	CPoint   myPt(xPos , yPos); 
	int	n = mEtData->CharFromPos(myPt); 
	int ncIdx = LOWORD(n); //������ڵ��ַ���λ��
	int nSs , nEs;//����ѡ���λ��

	//��õ�ǰѡ�������
	mEtData->GetSel(nSs ,nEs);
 	if((abs(ncIdx - nSs) > abs(nEs - ncIdx ))//��ǰ����������ѡ������һ���ַ�
		||((nSs == nEs)&&(ncIdx > nEs)))	 //��û��ѡ�񣬵�������ڲ����ĺ���
 	{//��꿿����ѡ�е����һ���ַ�

		if(nSs%50 < 3*8)//��ѡ����ַ���һ�е�ǰ����
			nSs = nSs/50*50 + (nSs%50+1)/3*3;
		else			//��ѡ����ַ���һ�еĺ�벿��
			nSs = nSs/50*50 + nSs%50/3*3+1;

		if(ncIdx%50 < 3*8)		//��ǰ�����һ�е�ǰ�벿��
			nEs = ncIdx/50*50 + ncIdx%50/3*3+2 ;
		else if(ncIdx%50 > 3*8)	//��ǰ�����һ�еĺ�벿��
			nEs =ncIdx/50*50 + (ncIdx%50+2)/3*3;
		else					//�м�
			nEs =ncIdx/50*50 + (ncIdx%50-1)/3*3 +2;

 	}
 	else 
	if((abs(ncIdx - nSs) < abs(nEs - ncIdx) + 1) || ((nSs == nEs)&&(ncIdx < nSs))
		)
	{//��꿿����ѡ�еĵ�һ���ַ�

		if (nEs%50 < 3*8)
			nEs = nEs/50*50 + (nEs%50-1)/3*3+2;
		else if(nEs%50> 3*8)
			nEs = nEs/50*50 + (nEs%50)/3*3;
		else
			nEs = nEs/50*50 + nEs%50-1;
		
		if(ncIdx%50 < 3*8)
			nSs = ncIdx/50*50 + ncIdx%50/3*3;
		else 
			nSs = ncIdx/50*50 + ncIdx%50/3*3+1;
	}
	else
		return FALSE;

	//����ѡ����ַ�
	if(nEs >= nSs){
		//���õ�ǰѡ����
 		mEtData->SetSel(nSs ,  nEs , FALSE);

		//����һ��ѡ��λ��
		SeveSelPos(nSs , nEs);

 		//ͬ�����ַ���
		SyncSel(1 , true);
	}

	return FALSE;
}

// BOOL DataWnd::OnCharLBtnMsMv(MSG* pMsg)
// {
// 	int xPos = GET_X_LPARAM(pMsg->lParam); 
// 	int yPos = GET_Y_LPARAM(pMsg->lParam); 
// 
// 	CPoint   myPt(xPos , yPos); 
// 	int	n = mEtChar->CharFromPos(myPt); 
// 	int ncIdx = LOWORD(n); 
// 	int nSs , nEs;//����ѡ���λ��
// 
// 	//��õ�ǰѡ�������
// 	mEtData->GetSel(nSs ,nEs);
// 	
// 	if((abs(ncIdx - nSs) > abs(nEs - ncIdx ))||((nSs == nEs)&&(ncIdx > nEs)))
// 	{//��꿿����ѡ�е����һ���ַ�
// 		nEs = ncIdx;
// 	}else  if((abs(ncIdx - nSs) < abs(nEs - ncIdx))||((nSs == nEs)&&(ncIdx < nSs)))
// 	{//��꿿����ѡ�еĵ�һ���ַ�
// 		nSs = ncIdx;
// 	}else
// 		return TRUE;
// 
// 	mEtData->GetSel(nSs ,nEs);
// 	SyncSel(2);//ͬ��ѡ��������
// 
// 	return FALSE;
// 
// }
BOOL DataWnd::OnCharLBtnDown(MSG *pMsg)
{
	int xPos = GET_X_LPARAM(pMsg->lParam); 
	int yPos = GET_Y_LPARAM(pMsg->lParam);
	
	::SetFocus(mEtChar->GetSafeHwnd());
	//���������ƶ�
	CPoint   myPt(xPos , yPos); 
	int	n = mEtChar->CharFromPos(myPt); 
	int  ncIdx = LOWORD(n); 
	
	
	mEtChar->SetSel( ncIdx ,  ncIdx , TRUE);
	SyncSel(2 , true);
	return FALSE;
}
BOOL DataWnd::OnDataLBtnDown(MSG* pMsg)
{
	int xPos = GET_X_LPARAM(pMsg->lParam); 
	int yPos = GET_Y_LPARAM(pMsg->lParam);

	::SetFocus(mEtData->GetSafeHwnd());
	//���������ƶ�
	CPoint   myPt(xPos , yPos); 
	int	n = mEtData->CharFromPos(myPt); 
	int  ncIdx = LOWORD(n); 

	if((ncIdx%50 < 3*8-1)&&(ncIdx%50%3 == 2))
		++ncIdx;
	else if (ncIdx%50 == 3*8-1)
		ncIdx += 2;
	else if ((ncIdx%50 > 3*8-1)&&(ncIdx%50%3 == 0))
		++ncIdx;

	mEtData->SetSel( ncIdx ,  ncIdx , TRUE);

	//ȡ��ѡ�������ѡ��
	this->mSelEnd.QuadPart = -1;
	this->mSelStart.QuadPart = -1;


	SyncSel(1 , true);
	return TRUE;
}

BOOL DataWnd::FillBuf(int num , BOOL isNext)
{
	::SendMessage(GetParent()->GetSafeHwnd() ,DATA_CTRL_MSG , WPARAM(isNext) , LPARAM(mDBuf + num) );
	return TRUE;
}

void DataWnd::DelHeadLn(int lnSize)
{
	int i , dC = 0;
	//ȥ��ƫ�Ƶļ���
	for(i = 0 ; i < lnSize ; ++i)
		dC = this->mOB.Find(_T("\r\n") ,dC) + 2;
	mOB = mOB.Mid(dC);
	
	//ȥ�����ݵļ���
	dC = 0;
	for(i = 0 ; i < lnSize ; ++i)
		dC = this->mDB.Find(_T("\r\n") ,dC) + 2;
	mDB = mDB.Mid(dC);

	//ȥ�����ݵļ���
	dC = 0;
	for(i = 0 ; i < lnSize ; ++i)
		dC = this->mCB.Find(_T("\r\n") ,dC) + 2;
	mCB = mCB.Mid(dC);
}

void DataWnd::DelTailLn(int lnC)
{
	ASSERT(lnC > 0);
	//ȥ��ƫ�Ƶļ���
	int i = 0;
	int index = 0;
	for(i = 0 ; i <= lnC ; ++i )
		mOB = mOB.Left(mOB.ReverseFind(_T('\r')));
	mOB += _T("\r\n");

	for(i = 0 ; i <= lnC ; ++i )
		mCB = mCB.Left(mCB.ReverseFind(_T('\r')));
	mCB += _T("\r\n");

	for(i = 0 ; i <= lnC ; ++i )
		mDB = mDB.Left(mDB.ReverseFind(_T('\r')));
	mDB += _T("\r\n");
}

BOOL DataWnd::SetDataSec( LONG_INT liSecStart ,LONG_INT liSecCount )
{
	//����������������
	this->mCount.QuadPart = liSecCount.QuadPart;
	this->mStartSector = liSecStart;
// 	//���ù�������Ϣ
// 	SCROLLINFO scif;
// 	scif.cbSize = sizeof(SCROLLINFO);
// 	scif.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
// 	scif.nMax = 3000;
// 	scif.nMin = 0;
// 	scif.nPos = 0;
// 	this->mVScro->SetScrollInfo(&scif);

	//���õ�ǰ��ʾ������
	return SetCurSec(liSecStart);
}

BOOL DataWnd::SetCurSec( LONG_INT liSec )
{
	//�����Ų��Ϸ�
	if((liSec.QuadPart >= mCount.QuadPart)||(liSec.QuadPart < 0))
		return FALSE;

	//�ܵ�������
	::memset(this->mDBuf , 0 , sizeof(DATA_BUF)*3);//������
	this->mDBuf[0].mOff.QuadPart = liSec.QuadPart;
	this->mCurBufOff = 0;
	this->mCurBuf = 0;
	this->mLineCnt = 0;

	//������Ϣ��ȡ����
	FillBuf(0 , TRUE);				//���һ�Ż���
	if (this->mDBuf[0].mOff.QuadPart == -1)//��ȡָ����ɽ������ʧ��
		return FALSE;

	//�������������Ѿ���ʾ�˵�����
	this->mDB = _T("");
	this->mOB = _T("");
	this->mCB = _T("");

	this->AppEndAll( LINE_PER_SEC * 2  );//��ʼ���� ��������

	//֪ͨһ�µ�ǰ�������Ѿ�����������
	//�����ﲻֱ�ӷ������ڵ�ԭ���ǣ���һ�ε���ʱ����Դ�������ֱ�ӷ����Լ�
	//��Ϊ�ֽ�û�д��������Ϣ��������������ڿ��õĻ��������ڻᴦ���
	::SendMessage( this->GetSafeHwnd(), DATA_CHANGE_SECTOR , liSec.LowPart , liSec.HighPart);

	//����ѡ������
	ReSel();
	return TRUE;
}
void  DataWnd::SyncSel(int type , bool isMsgCall)
{
	int nS , nE  , t;
	if(type == 1)
	{//ͬ���������������ַ���
		this->mEtData->GetSel(nS ,nE);
		t = nS;
		nS = nS/50 *18 + (nS%50 )/3;//������ʼλ�����ַ�ȥ��λ��
		
		if (t == nE)//û��ѡ�������
			nE = nS;
		else{

			//�������λ�����ַ�����λ��
			if(nE%50 > 3*8)
				nE = nE/50 *18 + (nE%50+1)/3;
			else
				nE = nE/50 *18 + (nE%50+2)/3;
		}
		
		//���ַ�ȥѡ����Ӧ������
		this->mEtChar->SetSel(nS , nE);

	}else if (type == 2)
	{//ͬ���ַ�����������
		this->mEtChar->GetSel(nS ,nE);
		t = nS;
		
		//��������������ѡ������ʼλ��
		if (nS%18 < 8)
			nS = nS/18*50 + nS%18*3;
		else
			nS = nS/18*50 + nS%18*3 +1;

		//��������������ѡ��������λ��
		if(t == nE){//û��ѡ������  ֻ�Ƕ�λ
			nE = nS;
		}else{
			if (nE%18 <= 8)
				nE = nE/18*50 + nE%18*3 -1;
			else
				nE = nE/18*50 + nE%18*3;
		}
		
		//ѡ���������������
		this->mEtData->SetSel(nS , nE);

		//����һ��ѡ�������λ��
		if (isMsgCall)  //��Ϣ�����ľ���Ҫ����һ��
			SeveSelPos(nS , nE);
	}
}

int DataWnd::GetMinWidth()
{
	return mSize.cx * (DATA_C_W + CHAR_C_W + m_iOffCharCnt)+ 2 * LINE_W /*- (LINE_W )*/;
}

// void DataWnd::DrawSpLine(void)
// {
// 	//��������û������
// 	if (this->mDBuf[this->mCurBuf].mOff.QuadPart == -1)
// 		return;
// 	
// 	if (!::IsWindow(this->mEtChar->GetSafeHwnd()))
// 		return;
// 
// 	int top = (SEC_SIZE - this->mCurBufOff) * mSize.cy - 1;
// 	CDC* dc = mEtData->GetDC();
// 	CRect rect;
// 	mEtData->GetWindowRect(&rect);
// 
// 	HPEN pen = CreatePen(PS_SOLID , LINE_W ,COLORREF(SP_LING_COLOR));
// 	HGDIOBJ hAld = SelectObject(dc->GetSafeHdc() ,pen);
// 	
// 	dc->MoveTo( 0 , top);
// 	dc->LineTo(rect.Width() , top);
// 
// 
// 	SelectObject(dc->GetSafeHdc() ,hAld);
// 	::DeleteObject(pen);
// }

void DataWnd::SetOffCharCnt( int cnt )
{
	CRect rect;
	if (this->m_iOffCharCnt != cnt + 2)
	{//��Ҫ�ı���
		m_iOffCharCnt = cnt + 2;
		this->GetWindowRect(&rect);
		this->ReSize(rect);

		this->GetParent()->SendMessage(DATA_CHANGE_WIDTH , GetMinWidth() , 0);
		//֪ͨ�����ڸı䴰�ڵĴ�С

	}

}

void DataWnd::SetSecCount(LONG_INT cnt)
{
	this->mCount = cnt;
}

void DataWnd::SetSel( LONG_INT start , LONG_INT end )
{
	LONG_INT	temp;
	if (0 > start.QuadPart || 0 > end.QuadPart)
		return ;
	
	if (start.QuadPart > end.QuadPart)
	{//��Ҫ����һ��
		temp  = end;
		end   = start;
		start = temp;
	}

	//�ȱ���һ��ѡ���˵�����
	this->mSelStart = start;
	this->mSelEnd	= end;

	//����һ��ѡ������
	ReSel();
}

void DataWnd::ReSel()
{
	if (this->mSelStart.QuadPart < 0 || this->mSelEnd.QuadPart < 0) 
		return ;

	LONG_INT cStart = {0};//��ǰ��ʾ�ĵ�һ�ֽڵ�ʵ��ƫ��
	LONG_INT start = {0};	
	LONG_INT end   = {0};

	//��ǰ��ʾ�ĵ�һ���ֽڵ�ʵ��ƫ��
	cStart.QuadPart = mDBuf[mCurBuf].mOff.QuadPart * SEC_SIZE + BYTE_PER_LINE * mCurBufOff;
	start = cStart;
	//��ǰ��ʾ�����һ���ֽ�
	end.QuadPart = start.QuadPart + mLineCnt * BYTE_PER_LINE;

	if ( end.QuadPart < this->mSelStart.QuadPart ||
		start.QuadPart > this->mSelEnd.QuadPart) 
		return ;//���Ѿ���ʾ�˵ĵط�û��ʲô����ѡ���


	//����ʾ��������Ҫѡ���λ��
	if ( this->mSelStart.QuadPart > start.QuadPart )	start = this->mSelStart;
	if (this->mSelEnd.QuadPart <  end.QuadPart)         end = this->mSelEnd;

	//ת������ǰ��ʾ�������λ��
	start.QuadPart = start.QuadPart - cStart.QuadPart;
	end.QuadPart   = end.QuadPart - cStart.QuadPart;
	
	//ÿһ����������ֽ�  (����)
	start.QuadPart = (start.QuadPart / 0x10) * 18 + start.QuadPart % 0x10;
	end.QuadPart   = (end.QuadPart   / 0x10) * 18 + end.QuadPart % 0x10;

	//ѡ���ַ���
	this->mEtChar->SetSel((int)start.QuadPart , (int)end.QuadPart);
	SyncSel(2 , false);  //ͬ�����ַ���
}

void DataWnd::SeveSelPos( int nSs , int nEs )
{
	//����ѡ�������λ��
	LONG_INT cStart = {0};
	//��ǰ��ʾ�ĵ�һ���ֽڵ�ʵ��ƫ��
	cStart.QuadPart = mDBuf[mCurBuf].mOff.QuadPart * SEC_SIZE + BYTE_PER_LINE * mCurBufOff;

	nSs = ( nSs / 50 ) * 48 + nSs % 50 ;
	nEs = ( nEs / 50 ) * 48 + nEs % 50;
	this->mSelStart.QuadPart = (nSs + 2) / 3 + cStart.QuadPart;
	this->mSelEnd.QuadPart   = (nEs + 2) / 3 + cStart.QuadPart;

}

void DataWnd::SetStartSector( LONG_INT start )
{
	this->mStartSector = start;
}
