// NtfsInfoView.cpp : 实现文件
//

#include "stdafx.h"
#include "DiskScan.h"
#include "NtfsInfoView.h"


#pragma  warning(disable:4996)

// CNtfsInfoView

IMPLEMENT_DYNCREATE(CNtfsInfoView, CInfoView)

CNtfsInfoView::CNtfsInfoView()
	: CInfoView(CInfoView::IDD)
{

}

CNtfsInfoView::~CNtfsInfoView()
{
}

void CNtfsInfoView::DoDataExchange(CDataExchange* pDX)
{
	CInfoView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNtfsInfoView, CInfoView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CNtfsInfoView 诊断

#ifdef _DEBUG
void CNtfsInfoView::AssertValid() const
{
	CInfoView::AssertValid();
}

#ifndef _WIN32_WCE
void CNtfsInfoView::Dump(CDumpContext& dc) const
{
	CInfoView::Dump(dc);
}
#endif
#endif //_DEBUG


void CNtfsInfoView::OnInitialUpdate()
{
	//获得当前视图对应的文档对象
	CNtfsDoc*	pDoc = GetDocument();
	DNtfs*		pNtfs = pDoc->m_pNtfs.get();
	LONG_INT	size = {0};
	DWORD		dwSize = 0;
	CString		strTemp;
	WCHAR		volName[MAX_NTFS_VOLUME_NAME_LEN * 2 + 2] = {0};
	int			i = 0;

	CInfoView::OnInitialUpdate();

	//设备名字标签
	CRT_STATIC(m_wndSTName ,IDC_ST_DEV_NAME ,  IDS_DEV_NAME);

	//设备名字
	strTemp = pNtfs->GetDevName();  //设备的名字
	CRT_EDIT(m_wndETDevName , IDC_ET_DEV_NAME ,strTemp );

	//设备区域序号
	CRT_STATIC(m_wndSTDevAreaIdx ,IDC_ST_DEV_AREA_INDEX , IDS_DEV_AREA_INDEX );
	strTemp = GetPathParam(pDoc->m_strOpenParam , PT_INDEX);
	CRT_EDIT(m_wndETDevAreaIdx , IDC_ET_DEV_AREA_INDEX , strTemp.GetLength()?strTemp:_T("0"));

	//起始扇区号
	CRT_STATIC(m_wndSTStartSector ,IDC_ST_START_SECTOR , IDS_START_SECTOR );
	strTemp = GetPathParam(pDoc->m_strOpenParam , PT_OFFSET);
	CRT_EDIT(m_wndETStartSector , IDC_ET_START_SECTOR , strTemp.GetLength()?strTemp:_T("0"));

	//磁盘详细信息的框架
	CRT_STATIC_AREA(m_wndSTDevInfoArea , IDC_ST_DEV_INFO_AREA, IDS_DEV_INFO_AREA);

	//分区信息区域
	CRT_STATIC_AREA(m_wndSTPartInfoArea , IDC_ST_PART_INFO_AREA , IDS_PART_INFO);

	//分区名字
	CRT_STATIC(m_wndSTPartName , IDC_ST_PART_NAME , IDS_PART_NAME);
	pNtfs->GetVolumeName(volName , MAX_NTFS_VOLUME_NAME_LEN * 2 + 2);
	strTemp = volName;
	CRT_EDIT(m_wndETPartName , IDC_ET_PART_NAME , strTemp);


	//文件系统
	CRT_STATIC(m_wndSTFileSys , IDC_ST_FILE_SYSTEM , IDS_FILE_SYSTEM);
	strTemp.LoadString(IDS_NTFS_PART);
	CRT_EDIT(m_wndETFileSys , IDC_ET_FILE_SYSTEM , strTemp);

	//分区大小
	CRT_STATIC(m_wndSTPartSize , IDC_ST_PART_SIZE , IDS_PART_SIZE);
	size = pNtfs->GetSecCount();
	size.QuadPart *= SECTOR_SIZE;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETPartSizeByte , IDC_ET_PART_SIZE_BYTE , strTemp + _T("Byte"));
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETPartSize , IDC_ET_PART_SIZE , strTemp);

	//总扇区数
	CRT_STATIC(m_wndSTSectorCount , IDC_ST_SECTOR_COUNT , IDS_SECTOR_COUNT);
	size = pNtfs->GetSecCount();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETSectorCount , IDC_ET_SECTOR_COUNT , strTemp);

	//每簇扇区数
	CRT_STATIC(m_wndSTSectorPerClust , IDC_ST_SECTOR_PER_CLUST , IDS_SECTOR_PER_CLUST);
	strTemp.Format(_T("%X") , pNtfs->GetSecPerClust());
	CRT_EDIT(m_wndETSectorPerClust , IDC_ET_SECTOR_PER_CLUST , strTemp);

	//MFT的第一个簇号
	CRT_STATIC(m_wndSTMFTClust , IDC_ST_MFT_CLUST , IDS_MFT);
	size = pNtfs->GetClustForMFT();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETMFTClust , IDC_ET_MFT_CLUST , strTemp);

	//MFTMirr的第一个簇号
	CRT_STATIC(m_wndSTMFTMirrClust , IDC_ST_MFTMIRR_CLUST , IDS_MFTMIRR);
	size = pNtfs->GetClustForMFTMirr();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETMFTMirrClust , IDC_ET_MFTMIRR_CLUST , strTemp);


	//剩余扇区
	CRT_STATIC_AREA(m_wndSTRemainSectorArea , IDC_ST_REMAIN_SECTOR_ARER , IDS_REMAIN_PART_NAME);
	//剩余扇区的起始扇区号
	CRT_STATIC(m_wndSTRemainStartSector , IDC_ST_REMAIN_SECTOR_POS , IDS_START_SECTOR);
	dwSize =(DWORD)(pNtfs->GetSecCount().QuadPart % pNtfs->GetSecPerClust());
	size.QuadPart = pNtfs->GetSecCount().QuadPart - dwSize;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETRemainStartSector , IDC_ET_REMAIN_SECTOR_POS , strTemp);

	//剩余扇区数
	CRT_STATIC(m_wndSTRemainCount , IDC_ST_REMAIN_SECTOR_COUNT , IDS_REMAIN_SECTOR_COUNT);
	strTemp.Format(_T("%X") , dwSize);
	CRT_EDIT(m_wndETRemainCount , IDC_ET_REMAIN_SECTOR_COUNT , strTemp);

	//剩余扇区大小
	CRT_STATIC(m_wndSTRemainSize , IDC_ST_REMAIN_SECTOR_SIZE , IDS_REMAIN_SECTOR_SIZE);
	strTemp = GetSizeToString(dwSize * SECTOR_SIZE);
	CRT_EDIT(m_wndETRemainSize , IDC_ET_REMAIN_SECTOR_SIZE , strTemp);

	//数据浏览控制区域
	CRT_STATIC_AREA(m_wndSTDataScanArea , IDC_ST_DATA_SCAN_AREA , IDS_DATA_SCAN_AREA);

	//扇区浏览控制
	CRT_SCAN_CTRLS(m_wndSTSectorScan   , IDS_SECTOR_SCAN  , IDC_ST_SECTOR_SCAN ,	//标签
				   m_wndETSectorNum    , IDC_ET_SECTOR_NUM ,						//用于显示扇区号
				   m_wndBtnPreSector   , IDC_BTN_PRE_SECTOR ,						//前一个扇区
				   m_wndBtnNextSector  , IDC_BTN_NEXT_SECTOR ,						//下一个扇区
				   m_wndBtnFirstSector , IDC_BTN_FIRST_SECTOR ,						//第一个扇区
				   m_wndBtnLastSector  , IDC_BTN_LAST_SECTOR);						//最后一个扇区

	//簇浏览控制
	CRT_SCAN_CTRLS(	m_wndSTClustScan   , IDS_CLUST_SCAN  , IDC_ST_CLUST_SCAN ,	//标签
					m_wndETClustNum    , IDC_ET_CLUST_NUM ,						//用于显示扇区号
					m_wndBtnPreClust   , IDC_BTN_PRE_CLUST ,						//前一个扇区
					m_wndBtnNextClust  , IDC_BTN_NEXT_CLUST ,						//下一个扇区
					m_wndBtnFirstClust , IDC_BTN_FIRST_CLUST ,						//第一个扇区
					m_wndBtnLastClust  , IDC_BTN_LAST_CLUST);						//最后一个扇区

	//强行更新一次数据视图
	this->OnUpdate(NULL , 0 , NULL );
}


int CNtfsInfoView::RePositionCtrl()
{
	CRect	cr;			//当前视图的可客户区的大小
	CRect	cr2;		//自况内的客户区
	CRect	ctlRc1;
	CString strTemp;
	int		top = 5 ;		//控件的顶部位置
	int		topbak = 0;	//顶部位置的一个备份  可能在其他的地方需要使用
	int		width = 0 ;
	int btnW , btnH;
	//按钮的唯独
	btnW = this->m_szChar.cx * 2 + 2 * CTRL_X_GAP;
	btnH = CTRL_Y_GAP + m_szChar.cy;

	//获得当前视图的客户区大小
	::GetWindowRect(this->GetSafeHwnd() , &cr);
	cr.DeflateRect(11 , 7);		//视图中绘制控件的区域
	this->ScreenToClient(&cr);  //客户区中空间可以防止的区域
	cr2 = cr;
	cr2.DeflateRect(2 * CTRL_X_GAP , 0);	//子框中的控件挥之区域

	//调整第一个静态控件的位置
	CRect c1stRc;
	m_wndSTName.GetWindowRect(c1stRc);
	if (c1stRc.Height() == 0)
	{//是第一次调整位置 
		top += 3 * CTRL_Y_GAP;
		topbak = top;
	}else{
		this->ScreenToClient(c1stRc);
		top = c1stRc.top;
		topbak = top;
	}

	//设备名字
	MV_STATIC_NO_PRE(m_wndSTName , cr2 , top);
	MV_EDITE(m_wndSTName , m_wndETDevName  , cr2 , top);

 	//设备区域序号
 	MV_STATIC(m_wndETDevName , m_wndSTDevAreaIdx , cr2 , top);
 	MV_EDITE(m_wndSTDevAreaIdx , m_wndETDevAreaIdx , cr2, top);

	//起始扇区号
	MV_STATIC(m_wndETDevAreaIdx , m_wndSTStartSector , cr2 , top);
	MV_EDITE(m_wndSTStartSector , m_wndETStartSector , cr2 , top);

	//设备信息区域 
	MV_STATIC_AREA(m_wndETStartSector , m_wndSTDevInfoArea , topbak , top );
	
	top += 7 * CTRL_Y_GAP;
	topbak = top;

	//分区名字
	MV_STATIC_NO_PRE(m_wndSTPartName , cr2 , top);
	MV_EDITE(m_wndSTPartName ,m_wndETPartName ,   cr2 , top);

	//文件系统
	MV_STATIC(m_wndETPartName ,m_wndSTFileSys , cr2 , top);
	MV_EDITE(m_wndSTFileSys ,m_wndETFileSys ,   cr2 , top);

	//分区大小
	MV_STATIC(m_wndETFileSys ,m_wndSTPartSize , cr2 , top);
	MV_EDITE(m_wndSTPartSize ,m_wndETPartSizeByte ,   cr2 , top);
	m_wndETPartSizeByte.GetWindowRect(&ctlRc1);
	m_wndETPartSize.MoveWindow(cr2.left + CTRL_X_GAP , top += ctlRc1.Height() ,cr2.Width() - CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//总扇区数
	MV_STATIC(m_wndETPartSize ,m_wndSTSectorCount , cr2 , top);
	MV_EDITE(m_wndSTSectorCount ,m_wndETSectorCount ,   cr2 , top);

	//每簇扇区数
	MV_STATIC(m_wndETSectorCount ,m_wndSTSectorPerClust , cr2 , top);
	MV_EDITE(m_wndSTSectorPerClust ,m_wndETSectorPerClust ,   cr2 , top);

 	//MFT的第一个簇号
	MV_STATIC(m_wndETSectorPerClust ,m_wndSTMFTClust , cr2 , top);
	MV_EDITE(m_wndSTMFTClust ,m_wndETMFTClust ,   cr2 , top);

 	//MFTMirr的第一个簇号
	MV_STATIC(m_wndETMFTClust ,m_wndSTMFTMirrClust , cr2 , top);
	MV_EDITE(m_wndSTMFTMirrClust ,m_wndETMFTMirrClust ,   cr2 , top);

	//设备信息区域
	MV_STATIC_AREA(m_wndETMFTMirrClust , m_wndSTPartInfoArea  , topbak , top  );

	top += 7 * CTRL_Y_GAP;
	topbak = top;

	//剩余扇区的起始扇区号
	MV_STATIC_NO_PRE(m_wndSTRemainStartSector , cr2 , top);
	MV_EDITE(m_wndSTRemainStartSector ,m_wndETRemainStartSector , cr2 , top);

	//剩余扇区数
	MV_STATIC(m_wndETRemainStartSector ,m_wndSTRemainCount , cr2 , top);
	MV_EDITE(m_wndSTRemainCount ,m_wndETRemainCount ,   cr2 , top);

	//剩余扇区大小
	MV_STATIC(m_wndETRemainCount ,m_wndSTRemainSize , cr2 , top);
	MV_EDITE(m_wndSTRemainSize ,m_wndETRemainSize ,   cr2 , top);

	//分区信息显示区域
	MV_STATIC_AREA(m_wndETRemainSize , m_wndSTRemainSectorArea  , topbak , top  );

	top += (7 * CTRL_Y_GAP );
	topbak = top;

	//扇区浏览
	MV_SCAN_CTRLS(top , cr2 , btnW , btnH ,	m_wndSTSectorScan ,
		m_wndBtnFirstSector ,
		m_wndBtnLastSector , 
		m_wndBtnPreSector ,
		m_wndBtnNextSector ,
		m_wndETSectorNum);

	m_wndETSectorNum.GetWindowRect(&ctlRc1);
	top +=(ctlRc1.Height() + CTRL_Y_GAP);

	//簇浏览
	MV_SCAN_CTRLS(top , cr2 , btnW , btnH ,	m_wndSTClustScan ,
		m_wndBtnFirstClust ,
		m_wndBtnLastClust , 
		m_wndBtnPreClust ,
		m_wndBtnNextClust ,
		m_wndETClustNum);

	//磁盘信息的区域
	MV_STATIC_AREA( m_wndETClustNum  ,m_wndSTDataScanArea , topbak , top);

	//设置滚动条的滚动范围
	m_wndETClustNum.GetWindowRect(ctlRc1);
	SetScrollSizes(MM_TEXT , CSize( cr.Width() - cr2.Width() + 7 * (btnW + CTRL_X_GAP) ,
		top + ctlRc1.Height() + 2 * CTRL_Y_GAP ));
	return 0;
}

CNtfsDoc* CNtfsInfoView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNtfsDoc)));
	return (CNtfsDoc*)m_pDocument;
}

void CNtfsInfoView::OnSize(UINT nType, int cx, int cy)
{
	CInfoView::OnSize(nType, cx, cy);
	
	if(IsWindow(m_wndSTName.GetSafeHwnd()))
		RePositionCtrl();
}

void CNtfsInfoView::OnUpdate(CView* pSender, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	//更新属中需要更新的数据显示 
	CNtfsDoc* pDoc = this->GetDocument();
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

	//簇号
	BYTE secPerClust = pDoc->m_pNtfs->GetSecPerClust();
	liCurSec.QuadPart /= secPerClust; 
	liCurSec.HighPart ? strTemp.Format(_T("%X%08X") , liCurSec.HighPart , liCurSec.LowPart):strTemp.Format(_T("%X") , liCurSec.LowPart);
	this->m_wndETClustNum.SetWindowText(strTemp);

}

BOOL CNtfsInfoView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{    
		CWnd* pFocus = GetFocus();
		if(&m_wndETSectorNum ==  pFocus)
		{//当前扇区编辑框上按了回车
			OnEnterCurSector();
		}

		if(&m_wndETClustNum ==  pFocus)
		{//当前簇号编辑框上按了回车
			OnEnterCurClust();
		}
	}
	return CInfoView::PreTranslateMessage(pMsg);
}

void CNtfsInfoView::OnEnterCurSector()
{
	CNtfsDoc*	pDoc = this->GetDocument();
	CString		strCurSector;
	int			len = 0;
	int			i = 0;
	TCHAR		chr;
	CString		strCap;
	CString		strTemp;
	LONG_INT	liSec = {0};
	LONG_INT	liSecCnt = {0};
	BYTE		secPerClust = pDoc->m_pNtfs->GetSecPerClust();

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


	liSecCnt = liSec;
	liSecCnt.QuadPart /= secPerClust;
	//设置簇号
	liSecCnt.HighPart ? strTemp.Format(_T("%X%08X") , liSecCnt.HighPart , liSecCnt.LowPart):strTemp.Format(_T("%X") , liSecCnt.LowPart);
	this->m_wndETClustNum.SetWindowText(strTemp);

	pDoc->ChangeCurSector(liSec , this);

	return ;
}

void CNtfsInfoView::OnEnterCurClust()
{
	CNtfsDoc*	pDoc = this->GetDocument();
	CString		strCurClust;
	int			len = 0;
	int			i = 0;
	TCHAR		chr;
	CString		strCap;
	CString		strTemp;
	LONG_INT	liClust = {0};
	LONG_INT	liClustCnt = {0};
	BYTE		secPerClust = pDoc->m_pNtfs->GetSecPerClust();

	//获得输入的扇区号
	m_wndETClustNum.GetWindowText(strCurClust);
	strCurClust.TrimLeft();
	strCurClust.TrimRight();

	//重置到默认扇区链表
	pDoc->ReSetSectorList();

	len = strCurClust.GetLength();
	if (0 == len)
		return ;	//没有任何数据

	for(i = 0 ; i < len ; ++i)
	{
		chr = strCurClust.GetAt(i);
		if (!isHexChar(chr))
		{//无效字符 
			strCap.LoadString(IDS_WARNING);
			strCurClust.LoadString(IDS_INVALID_CHAR);
			strTemp.Format(_T("%c") , chr);
			strCurClust.Replace(HCAR_POS , strTemp);
			::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
			return ;
		}
	}

	//进行数据转换
	liClust = HexStrToLONG_INT(strCurClust);
	liClustCnt.QuadPart = pDoc->GetSecCount().QuadPart / secPerClust;

	if (liClustCnt.QuadPart <= liClust.QuadPart)
	{//越界了 
		strCap.LoadString(IDS_WARNING);
		strCurClust.LoadString(IDS_CLUST_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	liClustCnt = liClust;
	liClustCnt.QuadPart *= secPerClust;
	//设置扇区号
	liClustCnt.HighPart ? strTemp.Format(_T("%X%08X") , liClustCnt.HighPart , liClustCnt.LowPart):strTemp.Format(_T("%X") , liClustCnt.LowPart);
	this->m_wndETSectorNum.SetWindowText(strTemp);

	pDoc->ChangeCurSector(liClustCnt , this);

	return ;
}

