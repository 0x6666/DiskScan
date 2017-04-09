#include "stdafx.h"
#include "DataWnd.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable:4996)

//偏移区的名字
#define  OFFSET_NAME	_T("偏移")

//#define SCROLL_W			::GetSystemMetrics(SM_CXVSCROLL)
#define LINE_W				5   //分割线的宽度

#define HEAD_H				25		//顶部的高度
//#define DATA_H				(ALL_HEIGHT - HEAD_H)//数据区域的高度

//#define OFF_C_W				16		//偏移区的用11个字符的宽度
#define DATA_C_W			50		//(3*17)数据区的大小每个字节三个字符宽度 ，在家上三个空格)
#define CHAR_C_W			18		//字符区十六个字符

#define BG_COLOR			RGB(255,255,255)//背景颜色为白色
#define SP_LING_COLOR		RGB(236,233,216)//分割线的颜色
#define DATA_FONT_COLOR		RGB(0,0,0)		//数据颜色为黑色
#define OFFSET_FONT_COLOR	RGB(0,0,191)	//偏移文字的颜色

//此版本vc没有的宏 
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
	mEtData		= new CEdit;	//数据区
	mEtOffset	= new CEdit;	//偏移区
	mEtChar		= new CEdit;	//字符区
/*	mVScro		= new CScrollBar;//滚动条*/

	//数据缓存
	mDBuf	= new DATA_BUF[3];
	::memset(mDBuf , 0 ,3 * sizeof(DATA_BUF));

	mCurBuf			= -1;		//当前的缓存号
	mCount.QuadPart	= 0;		//总数据
	mLineCnt		= 0;
	mCurBufOff		= 0;

	mBKBrush.CreateSolidBrush(COLORREF(BG_COLOR)); 

	//mIsCharUnic = FALSE;	//默认为多字节字符形式
	mRadix		= 16;		//默认十六进制方式
/*	mVMaxLine	= 0;*/

	//各个区域的宽度  以字符数类衡量
	this->m_iOffCharCnt = 11;

	//开始时什么都没选中
	mSelEnd.QuadPart = -1;
	mSelStart.QuadPart = -1;
}

DataWnd::~DataWnd()
{
	delete mEtData;		//数据区
	delete mEtOffset;	//偏移区
	delete mEtChar;		//字符区
/*	delete mVScro;		//滚动条*/

	delete[] mDBuf;		//数据缓存

	mBKBrush.DeleteObject();
}

BOOL DataWnd::Create(RECT &rc,CWnd *pParent)
{
	BOOL bRes;
	GetLetterSize(pParent);	//借助父窗口获得字体的大小
	bRes = CWnd::Create(_T("STATIC") , 	NULL , WS_CHILD|SS_NOTIFY|WS_CHILD , rc , pParent , IDC_DATA_CTRL);
	
	if (bRes) InitControl(rc);
	return bRes;
}
void DataWnd::GetLetterSize(CWnd *pParent)
{
	//拷贝字体
	LOGFONT lf ,oldf;
	CFont f;
	pParent->GetFont()->GetLogFont(&lf);
	oldf = lf; //保持原有的父窗口字体

	_tcscpy(lf.lfFaceName , _T("宋体"));
	lf.lfHeight = 16;
	//lf.lfWidth = 10;
	this->mFont.DeleteObject();
	this->mFont.CreateFontIndirect(&lf);
	pParent->SetFont(&mFont ,FALSE);

	//获取字体的大小
	GetTextExtentPoint32(pParent->GetDC()->GetSafeHdc() ,_T("A") , 1 , &mSize);

	f.DeleteObject();
	f.CreateFontIndirect(&oldf);
	pParent->SetFont(&f ,FALSE);
}

BEGIN_MESSAGE_MAP(DataWnd, CWnd)
	//{{AFX_MSG_MAP(DataWnd)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()			//控件的背景颜色
	ON_WM_SIZE()				//空间的大小改变消息
/*	ON_WM_MOUSEWHEEL()*/
/*	ON_MESSAGE(WM_MOUSEWHEEL, OnMouseWheel)*/
/*	ON_WM_VSCROLL()				//垂直滚动条*/	
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
	
	//客户区的大小
//	::GetClientRect(this->GetSafeHwnd() , &rect);
	//::GetWindowRect()
	this->GetWindowRect(&rect);

	//各个区域的宽度
	int nOffW = mSize.cx * m_iOffCharCnt;//偏移区的宽度
	int nDatW = mSize.cx * DATA_C_W;

	//窗口的宽度
	//ww = rect.right - rect.left;
	//ww = min(ww , GetMinWidth());
	ww = GetMinWidth();


	//先将头部区域填白
	hHead = CreateRectRgn(0 , 0 , ww , HEAD_H);
	hb = CreateSolidBrush(BG_COLOR);
	FillRgn(dc.GetSafeHdc() , hHead , hb);
	DeleteObject(hHead);
	
	DrawHead(dc);		//绘制顶部的区域
	
	//开始画各个分割线
	HPEN pen = CreatePen(PS_SOLID , LINE_W ,COLORREF(SP_LING_COLOR));
	HGDIOBJ hAld = SelectObject(dc.GetSafeHdc() ,pen);
	dc.MoveTo( 0 , HEAD_H);
	dc.LineTo(ww ,HEAD_H);

	dc.MoveTo(nOffW + LINE_W  , 0);
	dc.LineTo(nOffW + LINE_W , rect.bottom - rect.top);

	dc.MoveTo( nOffW + nDatW +2 * LINE_W, 0);
	dc.LineTo(nOffW + nDatW +2 * LINE_W,rect.bottom - rect.top);
	
	
	//还原
	SelectObject(dc.GetSafeHdc() ,hAld);
	DeleteObject(pen);

}
void DataWnd::InitControl(RECT rc)
{	
	//整个窗口的大小
 	RECT wr;
// //	::GetClientRect(this->GetSafeHwnd() , &wr);
	this->GetWindowRect(&wr);	
	
	//wr.right = wr.left + this->GetMinWidth();

	RECT rect;
	//创建偏移区EDIT
	rect.left = 0;
	rect.right = mSize.cx * m_iOffCharCnt + 3;
	rect.top = HEAD_H + LINE_W;
	rect.bottom = /*ALL_HEIGHT*/wr.bottom - wr.top;
	this->mEtOffset->Create(
		WS_CHILD|WS_VISIBLE|ES_UPPERCASE|ES_READONLY/*|ES_NOHIDESEL*/|ES_MULTILINE|ES_CENTER
		,rect,this,IDC_ET_OFFSET);
	this->mEtOffset->SetFont(&mFont);
	this->mEtOffset->SetMargins(this->mSize.cx , this->mSize.cx); //两边都留了一个字符的狂度
	
	//创建数据区EDIT
	rect.left = rect.right + LINE_W ;
	rect.right = rect.left + mSize.cx * DATA_C_W;
	this->mEtData->Create(
		WS_CHILD|WS_VISIBLE|ES_UPPERCASE|ES_READONLY|ES_NOHIDESEL|ES_MULTILINE
		,rect,this,IDC_ET_DATA);
	this->mEtData->SetFont(&mFont);
	this->mEtData->SetMargins(mSize.cx , mSize.cx);
	
	//创建字符区EDIT
	rect.left = rect.right + LINE_W  ;
	rect.right = rect.left + mSize.cx * CHAR_C_W ;
	this->mEtChar->Create(
		WS_CHILD|WS_VISIBLE|ES_READONLY|ES_NOHIDESEL|ES_MULTILINE
		,rect,this,IDC_ET_CHAR);
	this->mEtChar->SetFont(&mFont);
	this->mEtChar->SetMargins(this->mSize.cx , this->mSize.cx);

// 	//垂直滚动条控件
// 	rc.right = GetMinWidth();
// 	this->mVScro->Create(
// 		WS_VISIBLE|WS_CHILD|SBS_VERT |SBS_RIGHTALIGN|SBS_RIGHTALIGN  ,rc,this ,IDC_VSCROLL);
// 	this->mVScro->ShowScrollBar(TRUE);
}
void DataWnd::ReSize(RECT rc)
{
	RECT rect;
	//创建偏移区EDIT
	rect.left = 0;
	rect.right = mSize.cx * m_iOffCharCnt + 3;
	rect.top = HEAD_H + LINE_W;
	rect.bottom = /*ALL_HEIGHT*/rc.bottom - rc.top;
	this->mEtOffset->MoveWindow(&rect , TRUE);

	//创建数据区EDIT
	rect.left = rect.right + LINE_W ;
	rect.right = rect.left + mSize.cx * DATA_C_W;
	this->mEtData->MoveWindow(&rect , TRUE);
	
	//创建字符区EDIT
	rect.left = rect.right + LINE_W  ;
	rect.right = rect.left + mSize.cx * CHAR_C_W ;
	this->mEtChar->MoveWindow(&rect , TRUE);
	
// 	//垂直滚动条控件
// 	rect.top = 0;
// 	rect.right = min(GetMinWidth() , rc.right - rc.left);
// 	rect.left = rect.right - SCROLL_W ;
// 	this->mVScro->MoveWindow(&rect , TRUE);
// 	this->mVScro->ShowScrollBar(TRUE);

	//重新绘制控件
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

	//设置文字的颜色
	SetTextColor(dc.GetSafeHdc() , OFFSET_FONT_COLOR);
	::SelectObject(dc.GetSafeHdc() , this->mFont.GetSafeHandle());
	
	y = (HEAD_H - mSize.cy )/2;
	x = (nOffw - mSize.cx * 4)/2;
	//左上角的偏移标志
	dc.TextOut(x , y ,OFFSET_NAME);
	
	for (i = 0 ; i < 16 ; ++i)
	{
		x = LINE_W+  nOffw + celLen * i + this->mSize.cx /2/*个人感觉这里不应该除2 ，但是除似乎好看一点*/;
		if(i >= 8)	x += this->mSize.cx ;
		x += (celLen - mSize.cx)/2;
		if(this->mRadix == 10)	buf.Format(_T("%d") , i);
		else buf.Format(_T("%X") , i);	//如果设置的不是十进制则不管其他的 统一十六进制(霸道的人生从不解释)
		dc.TextOut(x , y , buf);
	}
}
HBRUSH DataWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// TODO:  在此更改 DC 的任何属性
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
	//行数最多是两个扇区的行数
	ASSERT(lineCnt <= LINE_PER_SEC * 2);
	if(0 == lineCnt)	return;

	//当前使的换成区
	DATA_BUF* buf = mDBuf + mCurBuf;
	
	//要添加的第一行的行号(相对当前缓冲)
	UINT bfLnOff = this->mCurBufOff + this->mLineCnt;
	//要添加的第一行的实际行偏移（相对磁盘起始位置）
	LONG_INT off;
	off.QuadPart= buf->mOff.QuadPart * LINE_PER_SEC + bfLnOff;

	int no1;
	//判断最后一行是否搭到了下一个缓存区
	int no = (bfLnOff + lineCnt -1) / LINE_PER_SEC;

	if(no /*== 2*/){//需要的下下一个数据缓存
		no1 = (mCurBuf + no) % 3;//最后一行所在缓冲号
		no = (mCurBuf + no - 1) % 3;//最后一行所在的缓冲的前一个缓冲
		
		if(mDBuf[no1].mOff.QuadPart != mDBuf[no].mOff.QuadPart +1)
		{//下一个缓存区不是合法的数据  需要读取
			//设置扇区号
			mDBuf[no1].mOff.QuadPart = mDBuf[no].mOff.QuadPart +1;
			FillBuf(no1 , TRUE);//填充缓存
			if(  -1 == mDBuf[no1].mOff.QuadPart)//不在显示了
			{//没有下一个缓存区没有数据
				//需要修改行数

				//如果要添加的第一行就在下一个缓存的话，直接返回
				if (0 != bfLnOff / LINE_PER_SEC)
				{
					lineCnt = 0;
					this->UpdateData(FALSE);
					return ;
				}

				//当前换成有数据可以添加
				lineCnt = LINE_PER_SEC - mCurBufOff;
				if (!lineCnt)
					return;
			}
		}	
	}
  
	AppEndOff(off ,lineCnt);
	AppEndData(off ,lineCnt);
	AppEndChar(off ,lineCnt);
	//行数增加
	mLineCnt += lineCnt;
	this->UpdateData(FALSE);
}
BOOL DataWnd::AppHeadAll( int lineCnt )
{
	BOOL res = FALSE;
	//行数最多是2个扇区
	ASSERT(lineCnt <= 2 * LINE_PER_SEC);
	if(0 == lineCnt)	//没有什么要添加的
		return res;

	//当前的缓存区
	DATA_BUF* buf = mDBuf + mCurBuf;	
	int no = 0;
	
	if(mCurBufOff < lineCnt )
	{//需要使用上一个缓存区的数据
		no = (mCurBuf + 2) % 3;  //上一个缓冲区号
		if( mDBuf[no].mOff.QuadPart != buf->mOff.QuadPart - 1 )
		{//上一个缓存的数据还没有读取出来
			//设置扇区号
			mDBuf[no].mOff.QuadPart = buf->mOff.QuadPart - 1;
			FillBuf(no , FALSE);//填充缓存
			if(mDBuf[no].mOff.QuadPart == -1)//不在显示了
				return res;
		}	

		//当前扇区号有变化
		res = TRUE;
	}
	AppHeadOff(lineCnt);//在偏移区添加数据
	AppHeadData(lineCnt);
	AppHeadChar(lineCnt);
	//行数增加
	this->mLineCnt += lineCnt;
	UpdateData(FALSE);//数据重显
	
	return res;
}
void DataWnd::AppEndOff(LONG_INT start/*起始行号*/ ,int lineCnt/*行数*/)
{
	if(!lineCnt) return;
	
	CString buf , temp;
	int index = 0 ,nSC = 0, nEC = 0 ,i = 0;
	LONG_INT tempLi;
	start.QuadPart *= BYTE_PER_LINE;  //转换字节偏移
	
	for(i = 0 ;i < lineCnt ; ++i){

		if(this->mRadix == 10)
		{//十进制
			if (start.QuadPart % SEC_SIZE == 0 )
			{//一个山区的其实
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				temp.Format(_T("%d%d\r\n") , tempLi.HighPart , tempLi.LowPart);
				if (tempLi.QuadPart)
					temp.Format(_T("%d%d\r\n") , tempLi.HighPart , tempLi.LowPart);
				else
					temp.Format(_T("%d\r\n") ,  tempLi.LowPart);
			}else
				temp.Format(_T("%d%d\r\n") , start.HighPart , start.LowPart);
		}else{//十六进制
			if (start.QuadPart % SEC_SIZE == 0 )
			{//扇区号
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				if (tempLi.HighPart)//有高位
					temp.Format(_T("%X%X#\r\n") , tempLi.HighPart , tempLi.LowPart);
				else//没有高位
					temp.Format(_T("%X#\r\n") ,  tempLi.LowPart);
			}else //字节偏移
				temp.Format(_T("%X%08X\r\n") , start.HighPart , start.LowPart);
		}
		//追加将要显示的数据
		buf += temp;

		//判断是否需要改变窗口的大小
		if(start.QuadPart % SEC_SIZE != 0){
			temp.TrimRight();
			if (temp.GetLength() + 2 != m_iOffCharCnt )
				SetOffCharCnt(temp.GetLength());
		}
		//下面将要显示的偏移
		start.QuadPart += 16;
	}
	this->mOB += buf;
}
void DataWnd::AppEndData(LONG_INT start/*起始行号*/ ,int lineCnt/*行数*/)
{
	CString buf , temp;
	int index = 0 ,nSC = 0 , nEC = 0 , i = 0 , j = 0;
	BYTE b;
	DATA_BUF* dBuf = mDBuf + mCurBuf;		//当前缓存
	UINT bfLnOff = (UINT)(start.QuadPart -\
		dBuf->mOff.QuadPart * LINE_PER_SEC); //当前缓存的行偏移
	int no  = 0 ;//当前缓存号偏移


	for(i = 0 ;i < lineCnt ; ++i){
		no = (bfLnOff + i) / LINE_PER_SEC;
		no = (mCurBuf + no)%3;//下一个缓存号
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
void DataWnd::AppEndChar(LONG_INT start/*起始行号*/ ,int lineCnt/*行数*/)
{
	CString buf;
	int index = 0 ,nSC = 0 , nEC = 0 , i = 0 , j = 0;
	DATA_BUF* dBuf = mDBuf + mCurBuf;		//当前缓存
	UINT bfLnOff = (UINT)(start.QuadPart -\
		dBuf->mOff.QuadPart * LINE_PER_SEC); //当前缓存的行偏移
	int no  = 0 ;//当前缓存号偏移
	W_CHAR ww;


	BYTE temp[18] = {0};		
	for(i = 0 ;i < lineCnt ; ++i){
		no = (bfLnOff + i) / LINE_PER_SEC;
		no = (mCurBuf + no)%3;//下一个缓存号

		::memcpy(temp , mDBuf[no].mBuf + ((bfLnOff + i) % LINE_PER_SEC)*BYTE_PER_LINE , BYTE_PER_LINE);
		temp[16] = 0;//无效编码,便于判断
		temp[17] = 0;//无效编码，便于判断

		//if(mIsCharUnic){//CString的+=有编码转换的功能
		//	buf+=(WCHAR*)temp;				
	//	}else{
		for (j = 0 ; j < 16 ; ++j)
		{
			ww.char1 = temp[j];
			ww.char2 = temp[j+1];
			if (IsValidCode(ww , CP_GBK)){ //接下来是一个有效的gbk编码
				++j;
				continue;
			}

			//不是有效的gbk编码时判断是不是ascll码
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

	//要添加的第一个字节偏移
	start.QuadPart =BYTE_PER_LINE*(mDBuf[mCurBuf].mOff.QuadPart * LINE_PER_SEC + mCurBufOff - lineCnt);  //转换字节偏移

	for(i = 0 ;i < lineCnt ; ++i){
		if(this->mRadix == 10)
		{
			if (start.QuadPart % SEC_SIZE == 0 )
			{//一个山区的其实
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				if (tempLi.QuadPart)
					temp.Format(_T("%d%d\r\n") , tempLi.HighPart , tempLi.LowPart);
				else
					temp.Format(_T("%d\r\n") , tempLi.LowPart);
			}else
				temp.Format(_T("%d%d\r\n") , start.HighPart , start.LowPart);
		}else{
			if (start.QuadPart % SEC_SIZE == 0 )
			{//扇区号
				tempLi.QuadPart = start.QuadPart / SEC_SIZE;
				if (tempLi.HighPart)//有高位
					temp.Format(_T("%X%X#\r\n") , tempLi.HighPart , tempLi.LowPart);
				else//没有高位
					temp.Format(_T("%X#\r\n") ,  tempLi.LowPart);
			}else
				temp.Format(_T("%X%08X\r\n") , start.HighPart , start.LowPart);
		}
		start.QuadPart += 16;
		buf +=temp;
	}
	this->mOB= buf + mOB;    //将数据添加到头部
}
void DataWnd::AppHeadData(int lineCnt)
{
	CString buf ,temp; 
	int i = 0 , j = 0 ,no = 0;
	BYTE b;
	DATA_BUF* dBuf = mDBuf + mCurBuf;		//当前缓存
	UINT bfLnOff = (UINT)(mDBuf[mCurBuf].mOff.QuadPart * LINE_PER_SEC \
		+ mCurBufOff - lineCnt)%LINE_PER_SEC; //实际的行号

	for(i = 0 ;i < lineCnt ; ++i){
		//当前的要添加的行所在缓存号
		no = (lineCnt - i > mCurBufOff)?(mCurBuf + 2)%3:mCurBuf;

		for (j = 0 ; j < BYTE_PER_LINE ; ++j)
		{//取出一行中的每一个字节
			b = mDBuf[no].mBuf[((bfLnOff + i) % LINE_PER_SEC)*BYTE_PER_LINE + j];
			temp.Format(_T("%02X ") , b);
			if(j == 7)
				temp+=_T(" ");
			buf += temp;
		}
		//去掉最后的空格
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

	DATA_BUF* dBuf = mDBuf + mCurBuf;		//当前缓存
	UINT bfLnOff = (UINT)(mDBuf[mCurBuf].mOff.QuadPart * LINE_PER_SEC \
		+ mCurBufOff - lineCnt)%LINE_PER_SEC; //实际的行号

	for(i = 0 ;i < lineCnt ; ++i){
		//当前的要添加的行所在缓存号
		no = (lineCnt - i > mCurBufOff)?(mCurBuf + 2)%3:mCurBuf;

		::memcpy(temp , mDBuf[no].mBuf + ((bfLnOff + i) % LINE_PER_SEC)*BYTE_PER_LINE , BYTE_PER_LINE);
		temp[16] = 0;
		temp[17] = 0;

		//if(mIsCharUnic){//CString的+=有编码转换的功能
		//	buf+=(WCHAR*)temp;				
		//	}else{
		for (j = 0 ; j < 16 ; ++j)
		{
			ww.char1 = temp[j];
			ww.char2 = temp[j+1];
			if (IsValidCode(ww , CP_GBK)){ //接下来是一个有效的gbk编码
				++j;
				continue;
			}
			
			//不是有效的gbk编码时判断是不是ascll码
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
// 	//绘制控件边框
 	::GetClientRect(this->GetSafeHwnd() , &rect);
// 	this->GetWindowRect(&rect);
// 		
// 	//正则可以显示出来的行数
// 	h = rect.Height() - HEAD_H - LINE_W;
// 	mVMaxLine = h/mSize.cy;
	
	//调整偏移区的大小
	if (::IsWindow(this->mEtOffset->GetSafeHwnd()))
		ReSize(rect);

}
BOOL DataWnd::PreTranslateMessage(MSG* pMsg)
{
	//static bool isFirst = true;
	//屏蔽ESC键
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
		return TRUE;

	//没有加载过数据的话 就什么都不做
	if (mCurBuf == -1) 		return CWnd::PreTranslateMessage(pMsg);

	//获得当前鼠标的位置
	int xPos = GET_X_LPARAM(pMsg->lParam); 
	int yPos = GET_Y_LPARAM(pMsg->lParam); 
	CRect rect;
	
	if (pMsg->message == WM_MOUSEWHEEL)		//鼠标滚轮消息
	{
		if(!OnMouseWheel(pMsg))
			return CWnd::PreTranslateMessage(pMsg);
		else 
			return TRUE;
	}
	
	//按住左键移动
	if( (pMsg->wParam & MK_LBUTTON) && (pMsg->message == WM_MOUSEMOVE))
	{
		if (isFirst)
			return CWnd::PreTranslateMessage(pMsg);
// #ifdef _DEBUG
// 		AfxMessageBox(_T("鼠标移动"));
// #endif // _DEBUG

		//光标在子控件的位置
		mEtData->GetWindowRect(&rect);
		if(rect.PtInRect(pMsg->pt)){
			if(!OnDataLBtnMsMv(pMsg))
				return CWnd::PreTranslateMessage(pMsg);
			else  return TRUE;
		}

		mEtChar->GetWindowRect(&rect);
		if (rect.PtInRect(pMsg->pt))
		{	//在字符显示区
			//将字符区的选择同步给数据区
			SyncSel(2 , true);

			//if(!OnCharLBtnMsMv(pMsg))
			return CWnd::PreTranslateMessage(pMsg);
			//else return TRUE;
		}

// 		mEtOffset->GetWindowRect(&rect);
// 		if (rect.PtInRect(pMsg->pt))
// 		{	//在偏移区
// 
// 		}
	}
	
	//鼠标左按键事件
	if(pMsg->message == WM_LBUTTONDOWN ||pMsg->message == WM_LBUTTONDBLCLK )
	{
// 		CWnd * wndFocus = GetFocus();
// 		if (this != wndFocus &&
// 			mEtChar != wndFocus &&
// 			mEtData != wndFocus &&
// 			mEtOffset != wndFocus)
// 			return TRUE;

		isFirst = false;

		//贯标在子控件的位置
		mEtData->GetWindowRect(&rect);
		if(rect.PtInRect(pMsg->pt)){
			if(!OnDataLBtnDown(pMsg)) 
				return CWnd::PreTranslateMessage(pMsg);
			else
				return TRUE;
		}

		mEtChar->GetWindowRect(&rect);
		if (rect.PtInRect(pMsg->pt))
		{	//在字符显示区
			if(!OnCharLBtnDown(pMsg)) 
				return CWnd::PreTranslateMessage(pMsg);
			else
				return TRUE;
		}
// 		mEtOffset->GetWindowRect(&rect);
// 		if (rect.PtInRect(pMsg->pt))
// 		{	//在偏移区
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
// 	//滚动条信息
// 	SCROLLINFO scif;
// 	int i = 0;
// 	//获得当前滚动条的信息
// 	pSBar->GetScrollInfo(&scif,SIF_ALL);   
// 	i = scif.nPos;
// 	switch (nSBCode)   
// 	{  
// 	case SB_BOTTOM:		//滚动到末尾
// 		//pSBar->sroScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMax));   
// 		scif.nPos = scif.nMax;
// 		
// 		break;   
// 	case SB_TOP:		//滚到顶部 
// 		scif.nPos = 0;
// 		break;   
// 	case SB_LINEUP:		//上面一行 
// 		scif.nPos = scif.nPos -2;
// 		if(scif.nPos < scif.nMin)
// 			scif.nPos  = scif.nMin;
// 		break;   
// 	case SB_LINEDOWN:	//下一行
// 		scif.nPos = scif.nPos +2;
// 		if (scif.nPos > scif.nMax)     
// 			scif.nPos = scif.nMax;   
// 		break; 
// 	case SB_PAGEUP:		//上一页
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
// 	//先删后添
// //	int zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
// 	int lc = 0 ;  //行数
// 	int toDel = 0;
// 
// 	AfxMessageBox(_T("收到了了消息"));
// 	
// 	//还没有加载过数据
// 	if (mCurBuf == -1)
// 		return TRUE;
// 	
// 	if(zDelta < 0){   //页面往上移
// 		//判断是否可以往上移
// 		if(mLineCnt <= this->mVMaxLine) //滚不动了
// 			return TRUE;
// 		lc = (zDelta / (-120)) * 2 ;
// 		lc = 3;
// 		DelHeadLn(lc);
// 		this->mLineCnt -= lc;
// 		this->mCurBufOff += lc;
// 		if(mCurBufOff / LINE_PER_SEC > 0){	//该显示下一个缓存区了？？
// 			mCurBuf = (mCurBuf + 1) % 3;	//下一个缓存
// 			mCurBufOff %= LINE_PER_SEC;			//线的偏移
// 		}
// 		AppEndAll(lc);
// 		
// 	}else if(zDelta > 0)
// 	{//页面往下移
// 		DATA_BUF* dBuf = mDBuf + mCurBuf;
// 		if(dBuf->mOff.QuadPart <= 0 && mCurBufOff == 0)//不能往下滚了
// 			return TRUE;
// 		lc = (zDelta / 120) * 2 ;
// 		AfxMessageBox("出错了");
// 		
// 		lc = 3;
// 		if(!lc)	return TRUE;//没有要滚动的数据
// 		
// 		
// 		//删除尾部的几行数据
// 		toDel = (mLineCnt + lc)%(LINE_PER_SEC * 2);//要删除的行数
// 		if(toDel){//数据超过了 两个扇区的缓存   需要删除
// 			DelTailLn(toDel);
// 			mLineCnt -= toDel;
// 		}
// 		AppHeadAll(lc);		//更新各个显示区
// 		//计算当前缓存的偏移
// 		mCurBufOff -= lc;
// 		if (mCurBufOff < 0)
// 		{//到了上一个缓存
// 			mCurBuf = (mCurBuf + 2)%3;//上一个缓存号
// 			mCurBufOff += LINE_PER_SEC; //上一个缓存中偏移
// 		}
// 	}
// 	return TRUE;
// 	
// 	return DataWnd::OnMouseWheel(nFlags, zDelta, pt);
// }

BOOL DataWnd::OnMouseWheel(MSG* pMsg)
{
	//先删后添
	short	zDelta =  (short) HIWORD(pMsg->wParam);
	int		lc = 0 ;  //行数
	int		toDel = 0;
	DATA_BUF* dBuf = NULL;

	BOOL isCurSecChg = FALSE;		//当前扇区是否有变化

	//还没有加载过数据
	if (-1 == mCurBuf || 0 == zDelta)
		return TRUE;
	
	if(zDelta < 0){   //页面往上移  需要显示后面的数据
		//判断是否可以往上移
		
		//需要查询配置文件最小可以显示的行数
		TRACE0("需要查询最少可以显示的行数");

		if(mLineCnt <= 5) //滚不动了
			return TRUE;
		lc = (zDelta / (-WHEEL_DELTA)) * 2 ;

		DelHeadLn(lc);
		this->mLineCnt -= lc;
		this->mCurBufOff += lc;
		if(mCurBufOff / LINE_PER_SEC > 0)//该显示下一个缓存区了？？
		{					
			mCurBuf = (mCurBuf + 1) % 3;	//下一个缓存
			mCurBufOff %= LINE_PER_SEC;		//线的偏移
			isCurSecChg = TRUE;				//当前扇号有变化
		}

		//在后面添加数据
		AppEndAll(lc);
		
	}else if(zDelta > 0)   //需要显示前面的数据
	{//页面往下移
		dBuf = mDBuf + mCurBuf;  //当前缓存

		//要移动的行数
		lc = (zDelta / WHEEL_DELTA) * 2 ;
		
		if(mStartSector.QuadPart == dBuf->mOff.QuadPart )
		{//不能往下滚了
			
			if(mCurBufOff == 0) //已经没有什么要显示的了
				return TRUE;

			if (mCurBufOff < lc)
			{//在第一个扇区最大滚动的行数有限
				lc = mCurBufOff;
			}
		}
		

		if(0 == lc)//没有要滚动的数据
			return TRUE;
				
		//删除尾部的几行数据
		
		if (mLineCnt + lc > LINE_PER_SEC * 2)
		{//有需要的删除的
			toDel = (mLineCnt + lc)%(LINE_PER_SEC * 2);//要删除的行数
			if(toDel){//数据超过了 两个扇区的缓存   需要删除
				DelTailLn(toDel);
				mLineCnt -= toDel;
			}
		}

		AppHeadAll(lc);//更新各个显示区		
		//计算当前缓存的偏移
		mCurBufOff -= lc;
		if (mCurBufOff < 0)
		{//到了上一个缓存
			mCurBuf = (mCurBuf + 2) % 3;//上一个缓存号
			mCurBufOff += LINE_PER_SEC; //上一个缓存中偏移
			isCurSecChg = TRUE;
		}
	}

	
	if (isCurSecChg)//有需要的话向父窗口发送消息
	{
		dBuf = mDBuf + mCurBuf;  //当前缓存
		this->GetParent()->SendMessage(DATA_CHANGE_SECTOR , dBuf->mOff.LowPart , dBuf->mOff.HighPart);
	}


	ReSel();
	return TRUE;
}

BOOL DataWnd::OnDataLBtnMsMv(MSG* pMsg)
{
	//次函数我表示很郁闷   
	//先是当前鼠标的位置  xPos yPos
	//再通过此点获得字符的偏移
	//之后就是计算字符在行中的偏移
	//一行可以显示 16个字节，每个字节用两个字符表示
	//每显示一个字节后后加一个空格，也就相当于一个字节需要3个字符个显示空间 
	//显示了8个字节之后用一个空格隔开
	//行尾是 \r\n  
	//也就是说每16个字节需要 16*3+2 = 50个字符空间
	//更具以上的描述就可以计算选择文本的方式了


	//当前贯标在设备客户区的位置
	int xPos = GET_X_LPARAM(pMsg->lParam); 
	int yPos = GET_Y_LPARAM(pMsg->lParam); 

	CPoint   myPt(xPos , yPos); 
	int	n = mEtData->CharFromPos(myPt); 
	int ncIdx = LOWORD(n); //光标所在的字符的位置
	int nSs , nEs;//具体选择的位置

	//获得当前选择的区域
	mEtData->GetSel(nSs ,nEs);
 	if((abs(ncIdx - nSs) > abs(nEs - ncIdx ))//当前鼠标更靠近已选择的最后一个字符
		||((nSs == nEs)&&(ncIdx > nEs)))	 //还没有选择，但是鼠标在插入点的后面
 	{//贯标靠近被选中的最后一个字符

		if(nSs%50 < 3*8)//已选择的字符在一行的前半行
			nSs = nSs/50*50 + (nSs%50+1)/3*3;
		else			//已选择的字符在一行的后半部分
			nSs = nSs/50*50 + nSs%50/3*3+1;

		if(ncIdx%50 < 3*8)		//当前鼠标在一行的前半部分
			nEs = ncIdx/50*50 + ncIdx%50/3*3+2 ;
		else if(ncIdx%50 > 3*8)	//当前鼠标在一行的后半部分
			nEs =ncIdx/50*50 + (ncIdx%50+2)/3*3;
		else					//中间
			nEs =ncIdx/50*50 + (ncIdx%50-1)/3*3 +2;

 	}
 	else 
	if((abs(ncIdx - nSs) < abs(nEs - ncIdx) + 1) || ((nSs == nEs)&&(ncIdx < nSs))
		)
	{//光标靠近被选中的第一个字符

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

	//设置选择的字符
	if(nEs >= nSs){
		//设置当前选择项
 		mEtData->SetSel(nSs ,  nEs , FALSE);

		//保存一下选择位置
		SeveSelPos(nSs , nEs);

 		//同步到字符区
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
// 	int nSs , nEs;//具体选择的位置
// 
// 	//获得当前选择的区域
// 	mEtData->GetSel(nSs ,nEs);
// 	
// 	if((abs(ncIdx - nSs) > abs(nEs - ncIdx ))||((nSs == nEs)&&(ncIdx > nEs)))
// 	{//贯标靠近被选中的最后一个字符
// 		nEs = ncIdx;
// 	}else  if((abs(ncIdx - nSs) < abs(nEs - ncIdx))||((nSs == nEs)&&(ncIdx < nSs)))
// 	{//贯标靠近被选中的第一个字符
// 		nSs = ncIdx;
// 	}else
// 		return TRUE;
// 
// 	mEtData->GetSel(nSs ,nEs);
// 	SyncSel(2);//同步选择数据区
// 
// 	return FALSE;
// 
// }
BOOL DataWnd::OnCharLBtnDown(MSG *pMsg)
{
	int xPos = GET_X_LPARAM(pMsg->lParam); 
	int yPos = GET_Y_LPARAM(pMsg->lParam);
	
	::SetFocus(mEtChar->GetSafeHwnd());
	//在数据区移动
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
	//在数据区移动
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

	//取消选择区域的选择
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
	//去掉偏移的几行
	for(i = 0 ; i < lnSize ; ++i)
		dC = this->mOB.Find(_T("\r\n") ,dC) + 2;
	mOB = mOB.Mid(dC);
	
	//去掉数据的几行
	dC = 0;
	for(i = 0 ; i < lnSize ; ++i)
		dC = this->mDB.Find(_T("\r\n") ,dC) + 2;
	mDB = mDB.Mid(dC);

	//去掉数据的几行
	dC = 0;
	for(i = 0 ; i < lnSize ; ++i)
		dC = this->mCB.Find(_T("\r\n") ,dC) + 2;
	mCB = mCB.Mid(dC);
}

void DataWnd::DelTailLn(int lnC)
{
	ASSERT(lnC > 0);
	//去掉偏移的几行
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
	//设置设置扇区总数
	this->mCount.QuadPart = liSecCount.QuadPart;
	this->mStartSector = liSecStart;
// 	//设置滚动条信息
// 	SCROLLINFO scif;
// 	scif.cbSize = sizeof(SCROLLINFO);
// 	scif.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
// 	scif.nMax = 3000;
// 	scif.nMin = 0;
// 	scif.nPos = 0;
// 	this->mVScro->SetScrollInfo(&scif);

	//设置当前显示的扇区
	return SetCurSec(liSecStart);
}

BOOL DataWnd::SetCurSec( LONG_INT liSec )
{
	//扇区号不合法
	if((liSec.QuadPart >= mCount.QuadPart)||(liSec.QuadPart < 0))
		return FALSE;

	//总的扇区数
	::memset(this->mDBuf , 0 , sizeof(DATA_BUF)*3);//清理缓存
	this->mDBuf[0].mOff.QuadPart = liSec.QuadPart;
	this->mCurBufOff = 0;
	this->mCurBuf = 0;
	this->mLineCnt = 0;

	//发送消息获取数据
	FillBuf(0 , TRUE);				//填充一号缓存
	if (this->mDBuf[0].mOff.QuadPart == -1)//获取指定的山区数据失败
		return FALSE;

	//现在清理所有已经显示了的数据
	this->mDB = _T("");
	this->mOB = _T("");
	this->mCB = _T("");

	this->AppEndAll( LINE_PER_SEC * 2  );//初始更显 两个扇区

	//通知一下当前扇区号已经重新设置了
	//在这里不直接发往窗口的原因是，第一次调用时会断言错误，所以直接发给自己
	//因为字节没有处理这个消息，所以如果父窗口可用的话，父窗口会处理的
	::SendMessage( this->GetSafeHwnd(), DATA_CHANGE_SECTOR , liSec.LowPart , liSec.HighPart);

	//重新选择数据
	ReSel();
	return TRUE;
}
void  DataWnd::SyncSel(int type , bool isMsgCall)
{
	int nS , nE  , t;
	if(type == 1)
	{//同步数据数据区到字符区
		this->mEtData->GetSel(nS ,nE);
		t = nS;
		nS = nS/50 *18 + (nS%50 )/3;//计算起始位置在字符去的位置
		
		if (t == nE)//没有选择的数据
			nE = nS;
		else{

			//计算借宿位置在字符区的位置
			if(nE%50 > 3*8)
				nE = nE/50 *18 + (nE%50+1)/3;
			else
				nE = nE/50 *18 + (nE%50+2)/3;
		}
		
		//在字符去选择相应的区域
		this->mEtChar->SetSel(nS , nE);

	}else if (type == 2)
	{//同步字符区到数据区
		this->mEtChar->GetSel(nS ,nE);
		t = nS;
		
		//计算在数据区的选择区起始位置
		if (nS%18 < 8)
			nS = nS/18*50 + nS%18*3;
		else
			nS = nS/18*50 + nS%18*3 +1;

		//计算在数据区的选择区结束位置
		if(t == nE){//没有选择数据  只是定位
			nE = nS;
		}else{
			if (nE%18 <= 8)
				nE = nE/18*50 + nE%18*3 -1;
			else
				nE = nE/18*50 + nE%18*3;
		}
		
		//选择数据区域的数据
		this->mEtData->SetSel(nS , nE);

		//保存一下选择区域的位置
		if (isMsgCall)  //消息驱动的就需要保存一下
			SeveSelPos(nS , nE);
	}
}

int DataWnd::GetMinWidth()
{
	return mSize.cx * (DATA_C_W + CHAR_C_W + m_iOffCharCnt)+ 2 * LINE_W /*- (LINE_W )*/;
}

// void DataWnd::DrawSpLine(void)
// {
// 	//缓存区中没有数据
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
	{//需要改变宽度
		m_iOffCharCnt = cnt + 2;
		this->GetWindowRect(&rect);
		this->ReSize(rect);

		this->GetParent()->SendMessage(DATA_CHANGE_WIDTH , GetMinWidth() , 0);
		//通知父窗口改变窗口的大小

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
	{//需要交换一下
		temp  = end;
		end   = start;
		start = temp;
	}

	//先保存一下选择了的区域
	this->mSelStart = start;
	this->mSelEnd	= end;

	//设置一下选择区域
	ReSel();
}

void DataWnd::ReSel()
{
	if (this->mSelStart.QuadPart < 0 || this->mSelEnd.QuadPart < 0) 
		return ;

	LONG_INT cStart = {0};//当前显示的第一字节的实际偏移
	LONG_INT start = {0};	
	LONG_INT end   = {0};

	//当前显示的第一个字节的实际偏移
	cStart.QuadPart = mDBuf[mCurBuf].mOff.QuadPart * SEC_SIZE + BYTE_PER_LINE * mCurBufOff;
	start = cStart;
	//当前显示的最后一个字节
	end.QuadPart = start.QuadPart + mLineCnt * BYTE_PER_LINE;

	if ( end.QuadPart < this->mSelStart.QuadPart ||
		start.QuadPart > this->mSelEnd.QuadPart) 
		return ;//在已经显示了的地方没有什么可以选择的


	//在显示区域中需要选择的位置
	if ( this->mSelStart.QuadPart > start.QuadPart )	start = this->mSelStart;
	if (this->mSelEnd.QuadPart <  end.QuadPart)         end = this->mSelEnd;

	//转换到当前显示区的相对位置
	start.QuadPart = start.QuadPart - cStart.QuadPart;
	end.QuadPart   = end.QuadPart - cStart.QuadPart;
	
	//每一行添加两个字节  (换行)
	start.QuadPart = (start.QuadPart / 0x10) * 18 + start.QuadPart % 0x10;
	end.QuadPart   = (end.QuadPart   / 0x10) * 18 + end.QuadPart % 0x10;

	//选择字符区
	this->mEtChar->SetSel((int)start.QuadPart , (int)end.QuadPart);
	SyncSel(2 , false);  //同步到字符区
}

void DataWnd::SeveSelPos( int nSs , int nEs )
{
	//保存选择的区域位置
	LONG_INT cStart = {0};
	//当前显示的第一个字节的实际偏移
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
