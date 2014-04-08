// Fat32InfoView.cpp : 实现文件
//

#include "stdafx.h"
#include "DiskScan.h"
#include "Fat32InfoView.h"


#pragma warning(disable:4996)
// CFat32InfoView

IMPLEMENT_DYNCREATE(CFat32InfoView, CInfoView)

CFat32InfoView::CFat32InfoView()
	: CInfoView(CInfoView::IDD)
{

}

CFat32InfoView::~CFat32InfoView()
{
}

void CFat32InfoView::DoDataExchange(CDataExchange* pDX)
{
	CInfoView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFat32InfoView, CInfoView)
	ON_WM_SIZE()
END_MESSAGE_MAP()

CFat32Doc* CFat32InfoView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFat32Doc)));
	return (CFat32Doc*)m_pDocument;
}
// CFat32InfoView 诊断

#ifdef _DEBUG
void CFat32InfoView::AssertValid() const
{
	CInfoView::AssertValid();
}

#ifndef _WIN32_WCE
void CFat32InfoView::Dump(CDumpContext& dc) const
{
	CInfoView::Dump(dc);
}
#endif
#endif //_DEBUG


void CFat32InfoView::OnInitialUpdate()
{
	//获得当前视图对应的文档对象
	CFat32Doc*	pDoc = GetDocument();
	DFat32*		pFat32 = pDoc->m_pFat32;
	LONG_INT	size = {0};
	CString		strTemp;
	int			i = 0;

	CInfoView::OnInitialUpdate();

	//设备名字标签
	CRT_STATIC(m_wndSTName ,IDC_ST_DEV_NAME ,  IDS_DEV_NAME);

	//设备名字
	strTemp = pFat32->GetDevName();  //设备的名字
	CRT_EDIT(m_wndETDevName , IDC_ET_DEV_NAME ,strTemp );

	//设备区域序号
	CRT_STATIC(m_wndSTDevAreaIdx ,IDC_ST_DEV_AREA_INDEX , IDS_DEV_AREA_INDEX );
	CRT_EDIT(m_wndETDevAreaIdx , IDC_ET_DEV_AREA_INDEX , pDoc->m_strDevAreaIdx);

	//起始扇区号
	CRT_STATIC(m_wndSTStartSector ,IDC_ST_START_SECTOR , IDS_START_SECTOR );
	CRT_EDIT(m_wndETStartSector , IDC_ET_START_SECTOR , pDoc->m_strDevStartSec);

	//磁盘详细信息的框架
	CRT_STATIC_AREA(m_wndSTDevInfoArea , IDC_ST_DEV_INFO_AREA, IDS_DEV_INFO_AREA);

	//分区信息区域
	CRT_STATIC_AREA(m_wndSTPartInfoArea , IDC_ST_PART_INFO_AREA , IDS_PART_INFO);

	//分区名字
	CRT_STATIC(m_wndSTPartName , IDC_ST_PART_NAME , IDS_PART_NAME);
	pFat32->GetVolumeName(strTemp.GetBuffer(50) , 50);
	strTemp.ReleaseBuffer();
	CRT_EDIT(m_wndETPartName , IDC_ET_PART_NAME , strTemp);

	//文件系统
	CRT_STATIC(m_wndSTFileSys , IDC_ST_FILE_SYSTEM , IDS_FILE_SYSTEM);
	strTemp.LoadString(IDS_FAT32_PART);
	CRT_EDIT(m_wndETFileSys , IDC_ET_FILE_SYSTEM , strTemp);

	//分区大小
	CRT_STATIC(m_wndSTPartSize , IDC_ST_PART_SIZE , IDS_PART_SIZE);
	size.QuadPart = pFat32->GetSecCount();
	size.QuadPart *= SECTOR_SIZE;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETPartSizeByte , IDC_ET_PART_SIZE_BYTE , strTemp + _T("Byte"));
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETPartSize , IDC_ET_PART_SIZE , strTemp);

	//总扇区数
	CRT_STATIC(m_wndSTSectorCount , IDC_ST_SECTOR_COUNT , IDS_SECTOR_COUNT);
	size.QuadPart = pFat32->GetSecCount();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETSectorCount , IDC_ET_SECTOR_COUNT , strTemp);

	//保留扇区数 
	CRT_STATIC(m_wndSTReserveSector , IDC_ST_RESERVE_SECTOR , IDS_RESERVE_SECTOR);
	strTemp.Format(_T("%X") , pFat32->GetReserveSector());
	CRT_EDIT(m_wndETReserveSector , IDC_ET_RESERVE_SECTOR , strTemp);

	//每簇扇区数
	CRT_STATIC(m_wndSTSectorPerClust , IDC_ST_SECTOR_PER_CLUST , IDS_SECTOR_PER_CLUST);
	strTemp.Format(_T("%X") , pFat32->GetSecPerClust());
	CRT_EDIT(m_wndETSectorPerClust , IDC_ET_SECTOR_PER_CLUST , strTemp);

	//每FAT扇区数
	CRT_STATIC(m_wndSTSectorPerFAT , IDC_ST_SECTOR_PER_FAT , IDS_SECTOR_PER_FAT);
	strTemp.Format(_T("%X") ,pFat32->GetSectorPerFAT() );
	CRT_EDIT(m_wndETSectorPerFAT , IDC_ET_SECTOR_PER_FAT , strTemp);

	//第一个FAT扇区数
	CRT_STATIC(m_wndSTFAT1 , IDC_ST_FAT1 , IDS_FAT1);
	strTemp.Format(_T("%X") ,pFat32->GetReserveSector() );
	CRT_EDIT(m_wndETFAT1 , IDC_ET_FAT1 , strTemp);

	//第二个FAT位置
	CRT_STATIC(m_wndSTFAT2 , IDC_ST_FAT2 , IDS_FAT2);
	strTemp.Format(_T("%X") ,pFat32->GetReserveSector() + pFat32->GetSectorPerFAT() );
	CRT_EDIT(m_wndETFAT2 , IDC_ET_FAT2 , strTemp);

	//FSInfo
	CRT_STATIC(m_wndSTFSInfo , IDC_ST_FSINFO , IDS_FSINFO);
	strTemp.Format(_T("%X") ,pFat32->GetFSInfoSec());
	CRT_EDIT(m_wndETFSInfo , IDC_ET_FSINFO , strTemp);

	//跟目录扇区
	CRT_STATIC(m_wndSTRootDir , IDC_ST_ROOT_DIR , IDS_ROOT_DIR);
	strTemp.Format(_T("%X") ,pFat32->ClustToSect(pFat32->Get1stDirClust()));
	CRT_EDIT(m_wndETRootDir , IDC_ET_ROOT_DIR , strTemp);

	//剩余扇区
	CRT_STATIC_AREA(m_wndSTRemainSectorArea , IDC_ST_REMAIN_SECTOR_ARER , IDS_REMAIN_PART_NAME);
 	//剩余扇区的起始扇区号
	CRT_STATIC(m_wndSTRemainStartSector , IDC_ST_REMAIN_SECTOR_POS , IDS_START_SECTOR);
	strTemp.Format(_T("%X") , pFat32->GetSecCount() - pFat32->GetRemainSectorCnt());
	CRT_EDIT(m_wndETRemainStartSector , IDC_ET_REMAIN_SECTOR_POS , strTemp);

	//剩余扇区数
	CRT_STATIC(m_wndSTRemainCount , IDC_ST_REMAIN_SECTOR_COUNT , IDS_REMAIN_SECTOR_COUNT);
	strTemp.Format(_T("%X") , pFat32->GetRemainSectorCnt());
	CRT_EDIT(m_wndETRemainCount , IDC_ET_REMAIN_SECTOR_COUNT , strTemp);

	//剩余扇区大小
	CRT_STATIC(m_wndSTRemainSize , IDC_ST_REMAIN_SECTOR_SIZE , IDS_REMAIN_SECTOR_SIZE);
	strTemp = GetSizeToString(pFat32->GetRemainSectorCnt() * SECTOR_SIZE);
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


	//强制更新视图信息
	this->OnUpdate(NULL , 0 , NULL);
}



int CFat32InfoView::RePositionCtrl()
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

	//调整第一个静态控件
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
	MV_EDITE(m_wndSTDevAreaIdx , m_wndETDevAreaIdx , cr2  , top);

	//起始扇区号
	MV_STATIC(m_wndETDevAreaIdx , m_wndSTStartSector , cr2, top);
	MV_EDITE(m_wndSTStartSector , m_wndETStartSector , cr2  , top);

	//设备信息区域
	MV_STATIC_AREA(m_wndETStartSector , m_wndSTDevInfoArea , topbak , top);

	top += 7 * CTRL_Y_GAP;
	topbak = top;
	
	//分区名字
	MV_STATIC_NO_PRE(m_wndSTPartName , cr2  , top);
	MV_EDITE(m_wndSTPartName ,m_wndETPartName ,   cr2  , top);

	//文件系统
	MV_STATIC(m_wndETPartName ,m_wndSTFileSys , cr2 , top);
	MV_EDITE(m_wndSTFileSys ,m_wndETFileSys ,   cr2  , top);

	//分区大小
	MV_STATIC(m_wndETPartName ,m_wndSTPartSize , cr2 , top);
	MV_EDITE(m_wndSTPartSize ,m_wndETPartSizeByte ,   cr2  , top);
	m_wndETPartSizeByte.GetWindowRect(&ctlRc1);
	m_wndETPartSize.MoveWindow(cr2.left + CTRL_X_GAP , top += ctlRc1.Height() ,cr2.Width() - CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//总扇区数
	MV_STATIC(m_wndETPartSize ,m_wndSTSectorCount , cr2 , top);
	MV_EDITE(m_wndSTSectorCount ,m_wndETSectorCount ,   cr2  , top);

	//保留扇区数
	MV_STATIC(m_wndETSectorCount ,m_wndSTReserveSector , cr2 , top);
	MV_EDITE(m_wndSTReserveSector ,m_wndETReserveSector ,   cr2  , top);

	//每簇扇区数
	MV_STATIC(m_wndETReserveSector ,m_wndSTSectorPerClust , cr2 , top);
	MV_EDITE(m_wndSTSectorPerClust ,m_wndETSectorPerClust ,   cr2  , top);

	//每FAT扇区数
	MV_STATIC(m_wndETSectorPerClust ,m_wndSTSectorPerFAT , cr2 , top);
	MV_EDITE(m_wndSTSectorPerFAT ,m_wndETSectorPerFAT ,   cr2  , top);

	//第一个FAT位置
	MV_STATIC(m_wndETSectorPerFAT ,m_wndSTFAT1 , cr2 , top);
	MV_EDITE(m_wndSTFAT1 ,m_wndETFAT1 ,   cr2  , top);

	//第二个FAT位置
	MV_STATIC(m_wndETFAT1 ,m_wndSTFAT2 , cr2 , top);
	MV_EDITE(m_wndSTFAT2 ,m_wndETFAT2 ,   cr2  , top);

	//FSInfo
	MV_STATIC(m_wndETFAT2 ,m_wndSTFSInfo , cr2 , top);
	MV_EDITE(m_wndSTFSInfo ,m_wndETFSInfo ,   cr2  , top);

	//跟目录扇区
	MV_STATIC(m_wndETFSInfo ,m_wndSTRootDir , cr2 , top);
	MV_EDITE(m_wndSTRootDir ,m_wndETRootDir ,   cr2  , top);

	//分区信息显示区域
	MV_STATIC_AREA(m_wndETRootDir , m_wndSTPartInfoArea , topbak , top );

	top += 7 * CTRL_Y_GAP;
	topbak = top;

	//剩余扇区的起始扇区号
	MV_STATIC_NO_PRE(m_wndSTRemainStartSector , cr2  , top);
	MV_EDITE(m_wndSTRemainStartSector ,m_wndETRemainStartSector ,   cr2  , top);
	
	//剩余扇区数
	MV_STATIC(m_wndETRemainStartSector ,m_wndSTRemainCount , cr2 , top);
	MV_EDITE(m_wndSTRemainCount ,m_wndETRemainCount ,   cr2  , top);

	//剩余扇区大小
	MV_STATIC(m_wndETRemainCount ,m_wndSTRemainSize , cr2 , top);
	MV_EDITE(m_wndSTRemainSize ,m_wndETRemainSize ,   cr2 , top);

	//分区信息显示区域
	MV_STATIC_AREA(m_wndETRemainSize , m_wndSTRemainSectorArea , topbak , top );

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

	//初始化滚动条的滚动范围
	m_wndETClustNum.GetWindowRect(ctlRc1);
	SetScrollSizes(MM_TEXT , CSize( 0 , top + ctlRc1.Height() +  2*CTRL_Y_GAP ));

	return 0;
}

void CFat32InfoView::OnSize(UINT nType, int cx, int cy)
{
	CInfoView::OnSize(nType, cx, cy);
	if (IsWindow(m_wndSTName.GetSafeHwnd()))
		RePositionCtrl();
}

void CFat32InfoView::OnUpdate(CView* pSender, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	//更新属中需要更新的数据显示 
	CFat32Doc*	pDoc = this->GetDocument();
	LONG_INT	liCurSec = {0};
	CString		strTemp;
	DWORD		dwClust;

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
	dwClust = pDoc->m_pFat32->SectToClust((DWORD)liCurSec.QuadPart);
	strTemp.Format(_T("%X") , dwClust);
	this->m_wndETClustNum.SetWindowText(strTemp);

}

BOOL CFat32InfoView::PreTranslateMessage(MSG* pMsg)
{
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

void CFat32InfoView::OnEnterCurSector()
{
	CFat32Doc*	pDoc = this->GetDocument();
	CString		strCurSector;
	int			len = 0;
	int			i = 0;
	TCHAR		chr;
	CString		strCap;
	CString		strTemp;
	LONG_INT	liSec = {0};
	LONG_INT	liSecCnt = {0};
	BYTE		secPerClust = pDoc->m_pFat32->GetSecPerClust();

	//获得输入的扇区号
	m_wndETSectorNum.GetWindowText(strCurSector);
	strCurSector.TrimLeft();
	strCurSector.TrimRight();

	//重置为默认扇区列表
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

	liSecCnt.QuadPart = pDoc->m_pFat32->SectToClust(DWORD(liSec.QuadPart));

	//设置簇号
	liSecCnt.HighPart ? strTemp.Format(_T("%X%08X") , liSecCnt.HighPart , liSecCnt.LowPart):strTemp.Format(_T("%X") , liSecCnt.LowPart);
	this->m_wndETClustNum.SetWindowText(strTemp);

	pDoc->ChangeCurSector(liSec , this);

	return ;
}

void CFat32InfoView::OnEnterCurClust()
{
	CFat32Doc*	pDoc = this->GetDocument();
	CString		strCurClust;
	int			len = 0;
	int			i = 0;
	TCHAR		chr;
	CString		strCap;
	CString		strTemp;
	LONG_INT	liClust = {0};
	LONG_INT	liClustCnt = {0};
	BYTE		secPerClust = pDoc->m_pFat32->GetSecPerClust();

	//获得输入的扇区号
	m_wndETClustNum.GetWindowText(strCurClust);
	strCurClust.TrimLeft();
	strCurClust.TrimRight();

	//重置为默认扇区列表
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
	if (2 > liClust.QuadPart)
	{//无效的簇号
		strCap.LoadString(IDS_WARNING);
		strCurClust.LoadString(IDS_CLUST_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	//总的簇数
	liClustCnt.QuadPart = pDoc->m_pFat32->GetMaxClustNum();

	if (liClustCnt.QuadPart < liClust.QuadPart)
	{//越界了 
		strCap.LoadString(IDS_WARNING);
		strCurClust.LoadString(IDS_CLUST_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	liClustCnt.QuadPart = pDoc->m_pFat32->ClustToSect(DWORD(liClust.QuadPart));
	//设置扇区号
	liClustCnt.HighPart ? strTemp.Format(_T("%X%08X") , liClustCnt.HighPart , liClustCnt.LowPart):strTemp.Format(_T("%X") , liClustCnt.LowPart);
	this->m_wndETSectorNum.SetWindowText(strTemp);

	pDoc->ChangeCurSector(liClustCnt , this);

	return ;
}
