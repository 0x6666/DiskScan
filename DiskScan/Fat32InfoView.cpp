// Fat32InfoView.cpp : ʵ���ļ�
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
// CFat32InfoView ���

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
	//��õ�ǰ��ͼ��Ӧ���ĵ�����
	CFat32Doc*	pDoc = GetDocument();
	DFat32*		pFat32 = pDoc->m_pFat32;
	LONG_INT	size = {0};
	CString		strTemp;
	int			i = 0;

	CInfoView::OnInitialUpdate();

	//�豸���ֱ�ǩ
	CRT_STATIC(m_wndSTName ,IDC_ST_DEV_NAME ,  IDS_DEV_NAME);

	//�豸����
	strTemp = pFat32->GetDevName();  //�豸������
	CRT_EDIT(m_wndETDevName , IDC_ET_DEV_NAME ,strTemp );

	//�豸�������
	CRT_STATIC(m_wndSTDevAreaIdx ,IDC_ST_DEV_AREA_INDEX , IDS_DEV_AREA_INDEX );
	CRT_EDIT(m_wndETDevAreaIdx , IDC_ET_DEV_AREA_INDEX , pDoc->m_strDevAreaIdx);

	//��ʼ������
	CRT_STATIC(m_wndSTStartSector ,IDC_ST_START_SECTOR , IDS_START_SECTOR );
	CRT_EDIT(m_wndETStartSector , IDC_ET_START_SECTOR , pDoc->m_strDevStartSec);

	//������ϸ��Ϣ�Ŀ��
	CRT_STATIC_AREA(m_wndSTDevInfoArea , IDC_ST_DEV_INFO_AREA, IDS_DEV_INFO_AREA);

	//������Ϣ����
	CRT_STATIC_AREA(m_wndSTPartInfoArea , IDC_ST_PART_INFO_AREA , IDS_PART_INFO);

	//��������
	CRT_STATIC(m_wndSTPartName , IDC_ST_PART_NAME , IDS_PART_NAME);
	pFat32->GetVolumeName(strTemp.GetBuffer(50) , 50);
	strTemp.ReleaseBuffer();
	CRT_EDIT(m_wndETPartName , IDC_ET_PART_NAME , strTemp);

	//�ļ�ϵͳ
	CRT_STATIC(m_wndSTFileSys , IDC_ST_FILE_SYSTEM , IDS_FILE_SYSTEM);
	strTemp.LoadString(IDS_FAT32_PART);
	CRT_EDIT(m_wndETFileSys , IDC_ET_FILE_SYSTEM , strTemp);

	//������С
	CRT_STATIC(m_wndSTPartSize , IDC_ST_PART_SIZE , IDS_PART_SIZE);
	size.QuadPart = pFat32->GetSecCount();
	size.QuadPart *= SECTOR_SIZE;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETPartSizeByte , IDC_ET_PART_SIZE_BYTE , strTemp + _T("Byte"));
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETPartSize , IDC_ET_PART_SIZE , strTemp);

	//��������
	CRT_STATIC(m_wndSTSectorCount , IDC_ST_SECTOR_COUNT , IDS_SECTOR_COUNT);
	size.QuadPart = pFat32->GetSecCount();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETSectorCount , IDC_ET_SECTOR_COUNT , strTemp);

	//���������� 
	CRT_STATIC(m_wndSTReserveSector , IDC_ST_RESERVE_SECTOR , IDS_RESERVE_SECTOR);
	strTemp.Format(_T("%X") , pFat32->GetReserveSector());
	CRT_EDIT(m_wndETReserveSector , IDC_ET_RESERVE_SECTOR , strTemp);

	//ÿ��������
	CRT_STATIC(m_wndSTSectorPerClust , IDC_ST_SECTOR_PER_CLUST , IDS_SECTOR_PER_CLUST);
	strTemp.Format(_T("%X") , pFat32->GetSecPerClust());
	CRT_EDIT(m_wndETSectorPerClust , IDC_ET_SECTOR_PER_CLUST , strTemp);

	//ÿFAT������
	CRT_STATIC(m_wndSTSectorPerFAT , IDC_ST_SECTOR_PER_FAT , IDS_SECTOR_PER_FAT);
	strTemp.Format(_T("%X") ,pFat32->GetSectorPerFAT() );
	CRT_EDIT(m_wndETSectorPerFAT , IDC_ET_SECTOR_PER_FAT , strTemp);

	//��һ��FAT������
	CRT_STATIC(m_wndSTFAT1 , IDC_ST_FAT1 , IDS_FAT1);
	strTemp.Format(_T("%X") ,pFat32->GetReserveSector() );
	CRT_EDIT(m_wndETFAT1 , IDC_ET_FAT1 , strTemp);

	//�ڶ���FATλ��
	CRT_STATIC(m_wndSTFAT2 , IDC_ST_FAT2 , IDS_FAT2);
	strTemp.Format(_T("%X") ,pFat32->GetReserveSector() + pFat32->GetSectorPerFAT() );
	CRT_EDIT(m_wndETFAT2 , IDC_ET_FAT2 , strTemp);

	//FSInfo
	CRT_STATIC(m_wndSTFSInfo , IDC_ST_FSINFO , IDS_FSINFO);
	strTemp.Format(_T("%X") ,pFat32->GetFSInfoSec());
	CRT_EDIT(m_wndETFSInfo , IDC_ET_FSINFO , strTemp);

	//��Ŀ¼����
	CRT_STATIC(m_wndSTRootDir , IDC_ST_ROOT_DIR , IDS_ROOT_DIR);
	strTemp.Format(_T("%X") ,pFat32->ClustToSect(pFat32->Get1stDirClust()));
	CRT_EDIT(m_wndETRootDir , IDC_ET_ROOT_DIR , strTemp);

	//ʣ������
	CRT_STATIC_AREA(m_wndSTRemainSectorArea , IDC_ST_REMAIN_SECTOR_ARER , IDS_REMAIN_PART_NAME);
 	//ʣ����������ʼ������
	CRT_STATIC(m_wndSTRemainStartSector , IDC_ST_REMAIN_SECTOR_POS , IDS_START_SECTOR);
	strTemp.Format(_T("%X") , pFat32->GetSecCount() - pFat32->GetRemainSectorCnt());
	CRT_EDIT(m_wndETRemainStartSector , IDC_ET_REMAIN_SECTOR_POS , strTemp);

	//ʣ��������
	CRT_STATIC(m_wndSTRemainCount , IDC_ST_REMAIN_SECTOR_COUNT , IDS_REMAIN_SECTOR_COUNT);
	strTemp.Format(_T("%X") , pFat32->GetRemainSectorCnt());
	CRT_EDIT(m_wndETRemainCount , IDC_ET_REMAIN_SECTOR_COUNT , strTemp);

	//ʣ��������С
	CRT_STATIC(m_wndSTRemainSize , IDC_ST_REMAIN_SECTOR_SIZE , IDS_REMAIN_SECTOR_SIZE);
	strTemp = GetSizeToString(pFat32->GetRemainSectorCnt() * SECTOR_SIZE);
	CRT_EDIT(m_wndETRemainSize , IDC_ET_REMAIN_SECTOR_SIZE , strTemp);


	//���������������
	CRT_STATIC_AREA(m_wndSTDataScanArea , IDC_ST_DATA_SCAN_AREA , IDS_DATA_SCAN_AREA);

	//�����������
	CRT_SCAN_CTRLS(m_wndSTSectorScan   , IDS_SECTOR_SCAN  , IDC_ST_SECTOR_SCAN ,	//��ǩ
		m_wndETSectorNum    , IDC_ET_SECTOR_NUM ,						//������ʾ������
		m_wndBtnPreSector   , IDC_BTN_PRE_SECTOR ,						//ǰһ������
		m_wndBtnNextSector  , IDC_BTN_NEXT_SECTOR ,						//��һ������
		m_wndBtnFirstSector , IDC_BTN_FIRST_SECTOR ,						//��һ������
		m_wndBtnLastSector  , IDC_BTN_LAST_SECTOR);						//���һ������

	//���������
	CRT_SCAN_CTRLS(	m_wndSTClustScan   , IDS_CLUST_SCAN  , IDC_ST_CLUST_SCAN ,	//��ǩ
		m_wndETClustNum    , IDC_ET_CLUST_NUM ,						//������ʾ������
		m_wndBtnPreClust   , IDC_BTN_PRE_CLUST ,						//ǰһ������
		m_wndBtnNextClust  , IDC_BTN_NEXT_CLUST ,						//��һ������
		m_wndBtnFirstClust , IDC_BTN_FIRST_CLUST ,						//��һ������
		m_wndBtnLastClust  , IDC_BTN_LAST_CLUST);						//���һ������


	//ǿ�Ƹ�����ͼ��Ϣ
	this->OnUpdate(NULL , 0 , NULL);
}



int CFat32InfoView::RePositionCtrl()
{
	CRect	cr;			//��ǰ��ͼ�Ŀɿͻ����Ĵ�С
	CRect	cr2;		//�Կ��ڵĿͻ���
	CRect	ctlRc1;
	CString strTemp;
	int		top = 5 ;		//�ؼ��Ķ���λ��
	int		topbak = 0;	//����λ�õ�һ������  �����������ĵط���Ҫʹ��
	int		width = 0 ;
	int btnW , btnH;
	//��ť��Ψ��
	btnW = this->m_szChar.cx * 2 + 2 * CTRL_X_GAP;
	btnH = CTRL_Y_GAP + m_szChar.cy;

	//��õ�ǰ��ͼ�Ŀͻ�����С
	::GetWindowRect(this->GetSafeHwnd() , &cr);
	cr.DeflateRect(11 , 7);		//��ͼ�л��ƿؼ�������
	this->ScreenToClient(&cr);  //�ͻ����пռ���Է�ֹ������
	cr2 = cr;
	cr2.DeflateRect(2 * CTRL_X_GAP , 0);	//�ӿ��еĿؼ���֮����

	//������һ����̬�ؼ�
	CRect c1stRc;
	m_wndSTName.GetWindowRect(c1stRc);
	if (c1stRc.Height() == 0)
	{//�ǵ�һ�ε���λ�� 
		top += 3 * CTRL_Y_GAP;
		topbak = top;
	}else{
		this->ScreenToClient(c1stRc);
		top = c1stRc.top;
		topbak = top;
	}


	//�豸����
	MV_STATIC_NO_PRE(m_wndSTName , cr2 , top);
	MV_EDITE(m_wndSTName , m_wndETDevName  , cr2 , top);

	//�豸�������
	MV_STATIC(m_wndETDevName , m_wndSTDevAreaIdx , cr2 , top);
	MV_EDITE(m_wndSTDevAreaIdx , m_wndETDevAreaIdx , cr2  , top);

	//��ʼ������
	MV_STATIC(m_wndETDevAreaIdx , m_wndSTStartSector , cr2, top);
	MV_EDITE(m_wndSTStartSector , m_wndETStartSector , cr2  , top);

	//�豸��Ϣ����
	MV_STATIC_AREA(m_wndETStartSector , m_wndSTDevInfoArea , topbak , top);

	top += 7 * CTRL_Y_GAP;
	topbak = top;
	
	//��������
	MV_STATIC_NO_PRE(m_wndSTPartName , cr2  , top);
	MV_EDITE(m_wndSTPartName ,m_wndETPartName ,   cr2  , top);

	//�ļ�ϵͳ
	MV_STATIC(m_wndETPartName ,m_wndSTFileSys , cr2 , top);
	MV_EDITE(m_wndSTFileSys ,m_wndETFileSys ,   cr2  , top);

	//������С
	MV_STATIC(m_wndETPartName ,m_wndSTPartSize , cr2 , top);
	MV_EDITE(m_wndSTPartSize ,m_wndETPartSizeByte ,   cr2  , top);
	m_wndETPartSizeByte.GetWindowRect(&ctlRc1);
	m_wndETPartSize.MoveWindow(cr2.left + CTRL_X_GAP , top += ctlRc1.Height() ,cr2.Width() - CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//��������
	MV_STATIC(m_wndETPartSize ,m_wndSTSectorCount , cr2 , top);
	MV_EDITE(m_wndSTSectorCount ,m_wndETSectorCount ,   cr2  , top);

	//����������
	MV_STATIC(m_wndETSectorCount ,m_wndSTReserveSector , cr2 , top);
	MV_EDITE(m_wndSTReserveSector ,m_wndETReserveSector ,   cr2  , top);

	//ÿ��������
	MV_STATIC(m_wndETReserveSector ,m_wndSTSectorPerClust , cr2 , top);
	MV_EDITE(m_wndSTSectorPerClust ,m_wndETSectorPerClust ,   cr2  , top);

	//ÿFAT������
	MV_STATIC(m_wndETSectorPerClust ,m_wndSTSectorPerFAT , cr2 , top);
	MV_EDITE(m_wndSTSectorPerFAT ,m_wndETSectorPerFAT ,   cr2  , top);

	//��һ��FATλ��
	MV_STATIC(m_wndETSectorPerFAT ,m_wndSTFAT1 , cr2 , top);
	MV_EDITE(m_wndSTFAT1 ,m_wndETFAT1 ,   cr2  , top);

	//�ڶ���FATλ��
	MV_STATIC(m_wndETFAT1 ,m_wndSTFAT2 , cr2 , top);
	MV_EDITE(m_wndSTFAT2 ,m_wndETFAT2 ,   cr2  , top);

	//FSInfo
	MV_STATIC(m_wndETFAT2 ,m_wndSTFSInfo , cr2 , top);
	MV_EDITE(m_wndSTFSInfo ,m_wndETFSInfo ,   cr2  , top);

	//��Ŀ¼����
	MV_STATIC(m_wndETFSInfo ,m_wndSTRootDir , cr2 , top);
	MV_EDITE(m_wndSTRootDir ,m_wndETRootDir ,   cr2  , top);

	//������Ϣ��ʾ����
	MV_STATIC_AREA(m_wndETRootDir , m_wndSTPartInfoArea , topbak , top );

	top += 7 * CTRL_Y_GAP;
	topbak = top;

	//ʣ����������ʼ������
	MV_STATIC_NO_PRE(m_wndSTRemainStartSector , cr2  , top);
	MV_EDITE(m_wndSTRemainStartSector ,m_wndETRemainStartSector ,   cr2  , top);
	
	//ʣ��������
	MV_STATIC(m_wndETRemainStartSector ,m_wndSTRemainCount , cr2 , top);
	MV_EDITE(m_wndSTRemainCount ,m_wndETRemainCount ,   cr2  , top);

	//ʣ��������С
	MV_STATIC(m_wndETRemainCount ,m_wndSTRemainSize , cr2 , top);
	MV_EDITE(m_wndSTRemainSize ,m_wndETRemainSize ,   cr2 , top);

	//������Ϣ��ʾ����
	MV_STATIC_AREA(m_wndETRemainSize , m_wndSTRemainSectorArea , topbak , top );

	top += (7 * CTRL_Y_GAP );
	topbak = top;

	//�������
	MV_SCAN_CTRLS(top , cr2 , btnW , btnH ,	m_wndSTSectorScan ,
		m_wndBtnFirstSector ,
		m_wndBtnLastSector , 
		m_wndBtnPreSector ,
		m_wndBtnNextSector ,
		m_wndETSectorNum);

	m_wndETSectorNum.GetWindowRect(&ctlRc1);
	top +=(ctlRc1.Height() + CTRL_Y_GAP);

	//�����
	MV_SCAN_CTRLS(top , cr2 , btnW , btnH ,	m_wndSTClustScan ,
		m_wndBtnFirstClust ,
		m_wndBtnLastClust , 
		m_wndBtnPreClust ,
		m_wndBtnNextClust ,
		m_wndETClustNum);

	//������Ϣ������
	MV_STATIC_AREA( m_wndETClustNum  ,m_wndSTDataScanArea , topbak , top);

	//��ʼ���������Ĺ�����Χ
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
	//����������Ҫ���µ�������ʾ 
	CFat32Doc*	pDoc = this->GetDocument();
	LONG_INT	liCurSec = {0};
	CString		strTemp;
	DWORD		dwClust;

	if (!IsWindow(m_wndETSectorNum.GetSafeHwnd()))
		return ;

	if (this == pSender)  //�Լ������ĸ��£�
		return ;

	//��ǰ��������ʾ����
	liCurSec = pDoc->GetCurSec();

	//��Ҫ��ʾ������ת�����ַ���
	liCurSec.HighPart ? strTemp.Format(_T("%X%08X") , liCurSec.HighPart , liCurSec.LowPart):strTemp.Format(_T("%X") , liCurSec.LowPart);
	this->m_wndETSectorNum.SetWindowText(strTemp);

	//�غ�
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
		{//��ǰ�����༭���ϰ��˻س�

			OnEnterCurSector();
		}

		if(&m_wndETClustNum ==  pFocus)
		{//��ǰ�غű༭���ϰ��˻س�
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

	//��������������
	m_wndETSectorNum.GetWindowText(strCurSector);
	strCurSector.TrimLeft();
	strCurSector.TrimRight();

	//����ΪĬ�������б�
	pDoc->ReSetSectorList();

	len = strCurSector.GetLength();
	if (0 == len)
		return ;	//û���κ�����

	for(i = 0 ; i < len ; ++i)
	{
		chr = strCurSector.GetAt(i);
		if (!isHexChar(chr))
		{//��Ч�ַ� 
			strCap.LoadString(IDS_WARNING);
			strCurSector.LoadString(IDS_INVALID_CHAR);
			strTemp.Format(_T("%c") , chr);
			strCurSector.Replace(HCAR_POS , strTemp);
			::MessageBox(this->GetSafeHwnd() , strCurSector , strCap , MB_OK | MB_ICONWARNING );
			return ;
		}
	}

	//��������ת��
	liSec = HexStrToLONG_INT(strCurSector);
	liSecCnt = pDoc->GetSecCount();
	if (liSecCnt.QuadPart <= liSec.QuadPart)
	{//Խ���� 
		strCap.LoadString(IDS_WARNING);
		strCurSector.LoadString(IDS_SECTOR_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurSector , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	liSecCnt.QuadPart = pDoc->m_pFat32->SectToClust(DWORD(liSec.QuadPart));

	//���ôغ�
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

	//��������������
	m_wndETClustNum.GetWindowText(strCurClust);
	strCurClust.TrimLeft();
	strCurClust.TrimRight();

	//����ΪĬ�������б�
	pDoc->ReSetSectorList();

	len = strCurClust.GetLength();
	if (0 == len)
		return ;	//û���κ�����

	for(i = 0 ; i < len ; ++i)
	{
		chr = strCurClust.GetAt(i);
		if (!isHexChar(chr))
		{//��Ч�ַ� 
			strCap.LoadString(IDS_WARNING);
			strCurClust.LoadString(IDS_INVALID_CHAR);
			strTemp.Format(_T("%c") , chr);
			strCurClust.Replace(HCAR_POS , strTemp);
			::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
			return ;
		}
	}

	//��������ת��
	liClust = HexStrToLONG_INT(strCurClust);
	if (2 > liClust.QuadPart)
	{//��Ч�Ĵغ�
		strCap.LoadString(IDS_WARNING);
		strCurClust.LoadString(IDS_CLUST_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	//�ܵĴ���
	liClustCnt.QuadPart = pDoc->m_pFat32->GetMaxClustNum();

	if (liClustCnt.QuadPart < liClust.QuadPart)
	{//Խ���� 
		strCap.LoadString(IDS_WARNING);
		strCurClust.LoadString(IDS_CLUST_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	liClustCnt.QuadPart = pDoc->m_pFat32->ClustToSect(DWORD(liClust.QuadPart));
	//����������
	liClustCnt.HighPart ? strTemp.Format(_T("%X%08X") , liClustCnt.HighPart , liClustCnt.LowPart):strTemp.Format(_T("%X") , liClustCnt.LowPart);
	this->m_wndETSectorNum.SetWindowText(strTemp);

	pDoc->ChangeCurSector(liClustCnt , this);

	return ;
}
