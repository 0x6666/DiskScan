// DiskInfoView.cpp : 实现文件
//

#include "stdafx.h"
#include "DiskScan.h"
#include "DiskInfoView.h"
#include "DiskScanDoc.h"

#pragma warning(disable:4996)


// CDiskInfoView

IMPLEMENT_DYNCREATE(CDiskInfoView, CInfoView)

CDiskInfoView::CDiskInfoView()
	: CInfoView(CInfoView::IDD)
{

}

CDiskInfoView::~CDiskInfoView()
{
}

void CDiskInfoView::DoDataExchange(CDataExchange* pDX)
{
	CInfoView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDiskInfoView, CInfoView)
	ON_WM_SIZE()
//	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDiskInfoView 诊断

#ifdef _DEBUG
void CDiskInfoView::AssertValid() const
{
	CInfoView::AssertValid();
}

#ifndef _WIN32_WCE
void CDiskInfoView::Dump(CDumpContext& dc) const
{
	CInfoView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDiskInfoView 消息处理程序

void CDiskInfoView::OnInitialUpdate()
{
	//获得当前视图对应的文档对象
	CDiskDoc*	pDoc = GetDocument();
	Disk*		pDisk = pDoc->m_pDisk;
	LONG_INT	size = {0};
	CString		strTemp;
	int			i = 0;
	int			partCnt = 0;
	Disk::PDPart pPart;

	CInfoView::OnInitialUpdate();

	//设备名字标签
	CRT_STATIC(m_wndSTName ,IDC_ST_DEV_NAME ,  IDS_DEV_NAME);

	//设备名字
	strTemp = pDoc->GetDevName();  //设备的名字
	CRT_EDIT(m_wndETDevName , IDC_ET_DEV_NAME ,strTemp );

	//磁盘详细信息的框架
	CRT_STATIC_AREA(m_wndSTCanPartArea ,IDC_ST_CAN_PART_AREA , IDS_CAN_PART_AREA);

	//可分区大小
	CRT_STATIC(m_wndSTCanPart ,IDC_ST_CAN_PART_SIZE ,  IDS_CAN_PART);

	//可分区大小字节数
	size = pDoc->m_pDisk->GetPartableSecCount();  //磁盘的可分配扇区数
	size.QuadPart *= SECTOR_SIZE;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETCanPartByte , IDC_ET_CAN_PART_BYTE ,strTemp + _T("Byte"));

	//可分区大小尺寸
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETCanPartSize , IDC_ET_CAN_PART_SIZE ,strTemp);

	//未分区大小
	CRT_STATIC(m_wndSTNoPart ,IDC_ST_NO_PART_SIZE ,  IDS_NO_PART);

	//未分区大小字节数
	partCnt = pDisk->GetPartCount();
	size.QuadPart = 0;
	for (i = 0 ;i < partCnt ; ++i )
	{
		pPart = pDisk->GetPart(i);
		if (NULL != pPart && PART_UN_PART ==  pPart->mType)
			size.QuadPart += pPart->mSecCount.QuadPart;
	}
	size.QuadPart *= SECTOR_SIZE;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETNoPartByte , IDC_ET_NO_PART_BYTE ,strTemp + _T("Byte"));

	//未分区大小尺寸
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETNoPartSize , IDC_ET_NO_PART_SIZE ,strTemp);

	//每扇区字节数 标签
	CRT_STATIC(m_wndSTBytePerSector ,IDC_ST_BYTE_PER_SECTOR ,  IDS_BYTE_PER_SECTOR);
	
	//每扇区字节数 大小
	strTemp.Format(_T("%X") , pDisk->GetBytePerSec());
	CRT_EDIT(m_wndETBytePerSector , IDC_ET_BYTE_PER_SECTOR ,strTemp);

	//每磁道扇区数 标签
	CRT_STATIC(m_wndSTSectorPerTrack ,IDC_ST_SECTOR_PER_TRACK ,  IDS_SECTOR_PER_TRACK);

	//每磁道扇区数 数值
	strTemp.Format(_T("%X") , pDisk->GetSectorPerTrack());
	CRT_EDIT(m_wndETSectorPerTrack , IDC_ET_SECTOR_PER_TRACK ,strTemp);

	//每柱面磁道数 标签
	CRT_STATIC(m_wndSTTracksPerCylinder ,IDC_ST_TRACK_PER_CYLINDER ,  IDS_TRACK_PER_CYLINDER);

	//每柱面磁道数 数值
	strTemp.Format(_T("%X") , pDisk->GetTracksPerCylinder());
	CRT_EDIT(m_wndETTracksPerCylinder , IDC_ET_TRACK_PER_CYLINDER ,strTemp);

	//柱面数 标签
	CRT_STATIC(m_wndSTCylinders ,IDC_ST_CYLINDERS ,  IDS_CYLINDERS);

	//柱面数 数值
	strTemp.Format(_T("%X") , pDisk->GetCylinders());
	CRT_EDIT(m_wndETCylinders , IDC_ET_CYLINDERS ,strTemp);

	//不分区区域信息
	CRT_STATIC_AREA(m_wndSTCanNotPartArea , IDC_ST_CAN_NOT_PART_AREA , IDS_CAN_NOT_PART_AREA);

	//不可分区大小标签
	CRT_STATIC(m_wndSTCanNotPart ,IDC_ST_CAN_NOT_PART_SIZE ,  IDS_CAN_NOT_PART);

	//可分区大小字节数
	strTemp.Format(_T("%X") ,pDoc->m_pDisk->GetUnPartableSize());
	CRT_EDIT(m_wndETCanNotPartByte , IDC_ET_CAN_NOT_PART_BYTE ,strTemp + _T("Byte"));

	//可分区大小尺寸
	strTemp = GetSizeToString(pDoc->m_pDisk->GetUnPartableSize());
	CRT_EDIT(m_wndETCanNotPartSize , IDC_ET_CAN_NOT_PART_SIZE ,strTemp);

	//逻辑驱动区域
	CRT_STATIC_AREA(m_wndSTLgcDrivArea , IDC_ST_LOGICAL_DRIVER_AREA , IDS_LOGICAL_DRIVER_AREA);

	//分区数 标签
	CRT_STATIC(m_wndSTPartCnt ,IDC_ST_LOGICAL_DRIVER_COUNT ,  IDS_LOGICAL_DRIVER_COUNT);

	//分区总数 数值
	strTemp.Format(_T("%X") , pDisk->GetVolumeCount()) ;
	CRT_EDIT(m_wndETPartCnt , IDC_ET_LOGICAL_DRIVER_COUNT ,strTemp);

 	//主分区数
	CRT_STATIC(m_wndSTMainPartCnt ,IDC_ST_MAIN_PART_COUNT ,  IDS_MAIN_PART_COUNT);

	//主分区数
	strTemp.Format(_T("%X") , pDisk->GetMainVolCount()) ;
	CRT_EDIT(m_wndETMainPartCnt , IDC_ET_MAIN_PART_COUNT ,strTemp);

 	//逻辑分区数
	CRT_STATIC(m_wndSTLgcPartCnt ,IDC_ST_LOGICAL_PART_COUNT ,  IDS_LOGICAL_PART_COUNT);
	
	//逻辑分区数
	strTemp.Format(_T("%X") , pDisk->GetVolumeCount() - pDisk->GetMainVolCount()) ;
	CRT_EDIT(m_wndETLgcPartCnt , IDC_ET_LOGICAL_PART_COUNT ,strTemp);

	//数据浏览控制区域
	CRT_STATIC_AREA(m_wndSTDataScanArea , IDC_ST_DATA_SCAN_AREA , IDS_DATA_SCAN_AREA);

	//扇区浏览控制
	CRT_SCAN_CTRLS(m_wndSTSectorScan   , IDS_SECTOR_SCAN  , IDC_ST_SECTOR_SCAN ,	//标签
				   m_wndETSectorNum    , IDC_ET_SECTOR_NUM ,						//用于显示扇区号
				   m_wndBtnPreSector   , IDC_BTN_PRE_SECTOR ,						//前一个扇区
				   m_wndBtnNextSector  , IDC_BTN_NEXT_SECTOR ,						//下一个扇区
				   m_wndBtnFirstSector , IDC_BTN_FIRST_SECTOR ,						//第一个扇区
				   m_wndBtnLastSector  , IDC_BTN_LAST_SECTOR);						//最后一个扇区


	//强行更新一次数据视图
	this->OnUpdate(NULL , 0 , NULL );
}

int CDiskInfoView::RePositionCtrl(void)
{
	CRect	cr;			//当前视图的可客户区的大小
	CRect	cw;			//当前视图的窗口矩形
	CRect	ctlRc1;
	CString strTemp;
	int		top = 5 ;	//控件的顶部位置
	int		topbak = 0;	//顶部位置的一个备份  可能在其他的地方需要使用
	int		width = 0 ;
	int btnW , btnH;
	//按钮的唯独
	btnW = this->m_szChar.cx * 2 + 2 * CTRL_X_GAP;
	btnH = CTRL_Y_GAP + m_szChar.cy;

	TRACE0("需要查询配置文件，字体，大小");

	//获得当前视图的客户区大小
	::GetWindowRect(this->GetSafeHwnd() , &cw);
	cr = cw;
	cr.DeflateRect(11 , 7);
	this->ScreenToClient(&cr);  //客户区中空间可以防止的区域
	CRect cr2 = cr;
	cr2.DeflateRect(2 * CTRL_X_GAP , 0);

	//获得第一个控件的位置
	//IDC_ST_DEV_NAME
	CRect c1stRc;
	m_wndSTName.GetWindowRect(c1stRc);
	if (c1stRc.Height() == 0)
	{//第一次调用此函数
		m_wndSTName.MoveWindow(CRect( 11 , 5 , 78 , 23) , FALSE);
		top = 5;
	}else{
		this->ScreenToClient(&c1stRc);  //客户区中空间可以防止的区域
		top = c1stRc.top;
	}
 
	//调整设备名字控件的大小和位置
	MV_EDITE(m_wndSTName , m_wndETDevName  , cr , top);

	top += (6 * CTRL_Y_GAP );
	topbak = top ;

	//可分区大小标签
	MV_STATIC_NO_PRE(m_wndSTCanPart ,cr2 , top);
	MV_EDITE(m_wndSTCanPart ,m_wndETCanPartByte ,   cr2 , top);

	//可分区大小尺寸
	m_wndETCanPartByte.GetWindowRect(&ctlRc1);
	m_wndETCanPartSize.MoveWindow(cr.left + CTRL_X_GAP , top += ctlRc1.Height() , cr.Width() - 3 * CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//未分区
	MV_STATIC(m_wndETCanPartSize , m_wndSTNoPart , cr2 , top);
	MV_EDITE(m_wndSTNoPart ,m_wndETNoPartByte ,   cr2 , top);

	//未分区大小尺寸
	m_wndETNoPartByte.GetWindowRect(&ctlRc1);
	m_wndETNoPartSize.MoveWindow(cr.left + CTRL_X_GAP , top += ctlRc1.Height() , cr.Width() - 3 * CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//每扇区字节数
	MV_STATIC(m_wndETNoPartSize , m_wndSTBytePerSector , cr2 , top);
	MV_EDITE(m_wndSTBytePerSector ,m_wndETBytePerSector ,  cr2 , top);

	//每磁道扇区数
	MV_STATIC(m_wndETBytePerSector , m_wndSTSectorPerTrack , cr2 , top);
	MV_EDITE(m_wndSTSectorPerTrack ,m_wndETSectorPerTrack ,  cr2 , top);

	//每柱面磁道数 标签
	MV_STATIC(m_wndETSectorPerTrack , m_wndSTTracksPerCylinder , cr2 , top);
	MV_EDITE(m_wndSTTracksPerCylinder ,m_wndETTracksPerCylinder,  cr2 , top);

	//每柱面磁道数 标签
	MV_STATIC(m_wndETTracksPerCylinder , m_wndSTCylinders  , cr2 , top);
	MV_EDITE(m_wndSTCylinders ,m_wndETCylinders  ,  cr2 , top);

	//磁盘信息的区域
	MV_STATIC_AREA(m_wndETTracksPerCylinder , m_wndSTCanPartArea , topbak , top);

	//下一个框的起始位置
	top += (7 * CTRL_Y_GAP );
	topbak = top ;
	//不可分区大小
	MV_STATIC_NO_PRE(m_wndSTCanNotPart , cr2 , top);
	MV_EDITE(m_wndSTCanNotPart ,m_wndETCanNotPartByte ,   cr2 , top);

	//不可分区大小尺寸
	m_wndETCanNotPartByte.GetWindowRect(&ctlRc1);
	m_wndETCanNotPartSize.MoveWindow(cr.left + CTRL_X_GAP , top += ctlRc1.Height() ,cr.Width() - 3 * CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//不可分区区域
	MV_STATIC_AREA(m_wndETCanNotPartSize , m_wndSTCanNotPartArea , topbak , top);

 	//总分区数
	top += (7 * CTRL_Y_GAP );
	topbak = top;
	
	//不可分区大小标签
	MV_STATIC_NO_PRE(m_wndSTPartCnt , cr2 , top);
	MV_EDITE(m_wndSTPartCnt , m_wndETPartCnt , cr2 , top);

 	//主分区数
	MV_STATIC(m_wndETPartCnt , m_wndSTMainPartCnt , cr2 , top);
	MV_EDITE(m_wndSTMainPartCnt , m_wndETMainPartCnt , cr2 , top);

 	//逻辑分区数
	MV_STATIC(m_wndETMainPartCnt , m_wndSTLgcPartCnt , cr2 , top);
	MV_EDITE(m_wndSTLgcPartCnt , m_wndETLgcPartCnt , cr2 , top);

 	//逻辑驱动区域
	MV_STATIC_AREA(m_wndETLgcPartCnt , m_wndSTLgcDrivArea , topbak , top);


	top += (7 * CTRL_Y_GAP );
	topbak = top;

	//数据浏览
	MV_SCAN_CTRLS(top , cr2 , btnW , btnH ,	m_wndSTSectorScan ,
								m_wndBtnFirstSector ,
								m_wndBtnLastSector , 
								m_wndBtnPreSector ,
								m_wndBtnNextSector ,
								m_wndETSectorNum);

	//磁盘信息的区域
	MV_STATIC_AREA( m_wndETSectorNum,m_wndSTDataScanArea , topbak , top);

	//设置滚动条的滚动范围
	m_wndETLgcPartCnt.GetWindowRect(ctlRc1);
	SetScrollSizes(MM_TEXT , CSize( cr.Width() - cr2.Width() + 6 * (btnW + CTRL_X_GAP) , top + ctlRc1.Height() +  3*CTRL_Y_GAP ));

	return 0;
}

CDiskDoc* CDiskInfoView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDiskDoc)));
	return (CDiskDoc*)m_pDocument;
}


void CDiskInfoView::OnSize(UINT nType, int cx, int cy)
{
	CInfoView::OnSize(nType, cx, cy);

	//窗口大小调整时 则重新计算各个空间的位置 
	if(IsWindow(m_wndSTName.GetSafeHwnd()))
		RePositionCtrl();
}
void CDiskInfoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* /*pHint*/)
{
	//更新属中需要更新的数据显示 
	CDiskDoc* pDoc = this->GetDocument();
	LONG_INT liCurSec = {0};
	CString strTemp;

	if (!IsWindow(m_wndETSectorNum.GetSafeHwnd()))
		return ;

	if (this == pSender)  //自己出发的更新？
		return ;

	//当前扇区号显示出来
	liCurSec = pDoc->GetCurSec();

	//将要显示的数据转换成字符串
	liCurSec.HighPart ? strTemp.Format(_T("%X%08X") , liCurSec.HighPart , liCurSec.LowPart):strTemp.Format(_T("%X") , liCurSec.LowPart);
	this->m_wndETSectorNum.SetWindowText(strTemp);

}

BOOL CDiskInfoView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{    
		if(&m_wndETSectorNum ==  GetFocus())
		{//当前扇区编辑框上按了回车
			OnEnterCurSector();
		}
	}

	return CInfoView::PreTranslateMessage(pMsg);
}

void CDiskInfoView::OnEnterCurSector()
{
	CDiskDoc*	pDoc = this->GetDocument();
	CString		strCurSector;
	int			len = 0;
	int			i = 0;
	TCHAR		chr;
	CString		strCap;
	CString		strTemp;
	LONG_INT	liSec = {0};
	LONG_INT	liSecCnt = {0};


	//获得输入的扇区号
	m_wndETSectorNum.GetWindowText(strCurSector);
	strCurSector.TrimLeft();
	strCurSector.TrimRight();
	
	pDoc->ReSetSectorList();

	len = strCurSector.GetLength();
	if (0 == len)
		return ;	//没有任何数据
	
	for(i = 0 ; i < len ; ++i)
	{
		chr = strCurSector.GetAt(i);
		if (!isHexChar(chr))
		{//无效字符 
			strCap.LoadString(IDS_WARNING);
			strCurSector.LoadString(IDS_INVALID_CHAR);
			strTemp.Format(_T("%c") , chr);
			strCurSector.Replace(HCAR_POS , strTemp);
			::MessageBox(this->GetSafeHwnd() , strCurSector , strCap , MB_OK | MB_ICONWARNING );
			return ;
		}
	}

	//进行数据转换
	liSec = HexStrToLONG_INT(strCurSector);
	liSecCnt = pDoc->GetSecCount();
	if (liSecCnt.QuadPart <= liSec.QuadPart)
	{//越界了 
		strCap.LoadString(IDS_WARNING);
		strCurSector.LoadString(IDS_SECTOR_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurSector , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	pDoc->ChangeCurSector(liSec , this);

	return ;
}
