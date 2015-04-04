// NtfsInfoView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DiskScan.h"
#include "NtfsInfoView.h"
#include "..\DiskTool\disktool.h"


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


// CNtfsInfoView ���

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
	//��õ�ǰ��ͼ��Ӧ���ĵ�����
	CNtfsDoc*	pDoc = GetDocument();
	DNtfs*		pNtfs = pDoc->m_pNtfs.get();
	LONG_INT	size = {0};
	DWORD		dwSize = 0;
	CString		strTemp;
	WCHAR		volName[MAX_NTFS_VOLUME_NAME_LEN * 2 + 2] = {0};
	int			i = 0;

	CInfoView::OnInitialUpdate();

	//�豸���ֱ�ǩ
	CRT_STATIC(m_wndSTName ,IDC_ST_DEV_NAME ,  IDS_DEV_NAME);

	//�豸����
	strTemp = pNtfs->GetDevName();  //�豸������
	CRT_EDIT(m_wndETDevName , IDC_ET_DEV_NAME ,strTemp );

	//�豸�������
	CRT_STATIC(m_wndSTDevAreaIdx ,IDC_ST_DEV_AREA_INDEX , IDS_DEV_AREA_INDEX );
	strTemp = GetPathParam(pDoc->m_strOpenParam , PT_INDEX);
	CRT_EDIT(m_wndETDevAreaIdx , IDC_ET_DEV_AREA_INDEX , strTemp.GetLength()?strTemp:_T("0"));

	//��ʼ������
	CRT_STATIC(m_wndSTStartSector ,IDC_ST_START_SECTOR , IDS_START_SECTOR );
	strTemp = GetPathParam(pDoc->m_strOpenParam , PT_OFFSET);
	CRT_EDIT(m_wndETStartSector , IDC_ET_START_SECTOR , strTemp.GetLength()?strTemp:_T("0"));

	//������ϸ��Ϣ�Ŀ��
	CRT_STATIC_AREA(m_wndSTDevInfoArea , IDC_ST_DEV_INFO_AREA, IDS_DEV_INFO_AREA);

	//������Ϣ����
	CRT_STATIC_AREA(m_wndSTPartInfoArea , IDC_ST_PART_INFO_AREA , IDS_PART_INFO);

	//��������
	CRT_STATIC(m_wndSTPartName , IDC_ST_PART_NAME , IDS_PART_NAME);
	pNtfs->GetVolumeName(volName , MAX_NTFS_VOLUME_NAME_LEN * 2 + 2);
	strTemp = volName;
	CRT_EDIT(m_wndETPartName , IDC_ET_PART_NAME , strTemp);


	//�ļ�ϵͳ
	CRT_STATIC(m_wndSTFileSys , IDC_ST_FILE_SYSTEM , IDS_FILE_SYSTEM);
	strTemp.LoadString(IDS_NTFS_PART);
	CRT_EDIT(m_wndETFileSys , IDC_ET_FILE_SYSTEM , strTemp);

	//������С
	CRT_STATIC(m_wndSTPartSize , IDC_ST_PART_SIZE , IDS_PART_SIZE);
	size = pNtfs->GetSecCount();
	size.QuadPart *= SECTOR_SIZE;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETPartSizeByte , IDC_ET_PART_SIZE_BYTE , strTemp + _T("Byte"));
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETPartSize , IDC_ET_PART_SIZE , strTemp);

	//��������
	CRT_STATIC(m_wndSTSectorCount , IDC_ST_SECTOR_COUNT , IDS_SECTOR_COUNT);
	size = pNtfs->GetSecCount();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETSectorCount , IDC_ET_SECTOR_COUNT , strTemp);

	//ÿ��������
	CRT_STATIC(m_wndSTSectorPerClust , IDC_ST_SECTOR_PER_CLUST , IDS_SECTOR_PER_CLUST);
	strTemp.Format(_T("%X") , pNtfs->GetSecPerClust());
	CRT_EDIT(m_wndETSectorPerClust , IDC_ET_SECTOR_PER_CLUST , strTemp);

	//MFT�ĵ�һ���غ�
	CRT_STATIC(m_wndSTMFTClust , IDC_ST_MFT_CLUST , IDS_MFT);
	size = pNtfs->GetClustForMFT();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETMFTClust , IDC_ET_MFT_CLUST , strTemp);

	//MFTMirr�ĵ�һ���غ�
	CRT_STATIC(m_wndSTMFTMirrClust , IDC_ST_MFTMIRR_CLUST , IDS_MFTMIRR);
	size = pNtfs->GetClustForMFTMirr();
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETMFTMirrClust , IDC_ET_MFTMIRR_CLUST , strTemp);


	//ʣ������
	CRT_STATIC_AREA(m_wndSTRemainSectorArea , IDC_ST_REMAIN_SECTOR_ARER , IDS_REMAIN_PART_NAME);
	//ʣ����������ʼ������
	CRT_STATIC(m_wndSTRemainStartSector , IDC_ST_REMAIN_SECTOR_POS , IDS_START_SECTOR);
	dwSize =(DWORD)(pNtfs->GetSecCount().QuadPart % pNtfs->GetSecPerClust());
	size.QuadPart = pNtfs->GetSecCount().QuadPart - dwSize;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETRemainStartSector , IDC_ET_REMAIN_SECTOR_POS , strTemp);

	//ʣ��������
	CRT_STATIC(m_wndSTRemainCount , IDC_ST_REMAIN_SECTOR_COUNT , IDS_REMAIN_SECTOR_COUNT);
	strTemp.Format(_T("%X") , dwSize);
	CRT_EDIT(m_wndETRemainCount , IDC_ET_REMAIN_SECTOR_COUNT , strTemp);

	//ʣ��������С
	CRT_STATIC(m_wndSTRemainSize , IDC_ST_REMAIN_SECTOR_SIZE , IDS_REMAIN_SECTOR_SIZE);
	strTemp = GetSizeToString(dwSize * SECTOR_SIZE);
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

	//ǿ�и���һ��������ͼ
	this->OnUpdate(NULL , 0 , NULL );
}


int CNtfsInfoView::RePositionCtrl()
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

	//������һ����̬�ؼ���λ��
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
 	MV_EDITE(m_wndSTDevAreaIdx , m_wndETDevAreaIdx , cr2, top);

	//��ʼ������
	MV_STATIC(m_wndETDevAreaIdx , m_wndSTStartSector , cr2 , top);
	MV_EDITE(m_wndSTStartSector , m_wndETStartSector , cr2 , top);

	//�豸��Ϣ���� 
	MV_STATIC_AREA(m_wndETStartSector , m_wndSTDevInfoArea , topbak , top );
	
	top += 7 * CTRL_Y_GAP;
	topbak = top;

	//��������
	MV_STATIC_NO_PRE(m_wndSTPartName , cr2 , top);
	MV_EDITE(m_wndSTPartName ,m_wndETPartName ,   cr2 , top);

	//�ļ�ϵͳ
	MV_STATIC(m_wndETPartName ,m_wndSTFileSys , cr2 , top);
	MV_EDITE(m_wndSTFileSys ,m_wndETFileSys ,   cr2 , top);

	//������С
	MV_STATIC(m_wndETFileSys ,m_wndSTPartSize , cr2 , top);
	MV_EDITE(m_wndSTPartSize ,m_wndETPartSizeByte ,   cr2 , top);
	m_wndETPartSizeByte.GetWindowRect(&ctlRc1);
	m_wndETPartSize.MoveWindow(cr2.left + CTRL_X_GAP , top += ctlRc1.Height() ,cr2.Width() - CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//��������
	MV_STATIC(m_wndETPartSize ,m_wndSTSectorCount , cr2 , top);
	MV_EDITE(m_wndSTSectorCount ,m_wndETSectorCount ,   cr2 , top);

	//ÿ��������
	MV_STATIC(m_wndETSectorCount ,m_wndSTSectorPerClust , cr2 , top);
	MV_EDITE(m_wndSTSectorPerClust ,m_wndETSectorPerClust ,   cr2 , top);

 	//MFT�ĵ�һ���غ�
	MV_STATIC(m_wndETSectorPerClust ,m_wndSTMFTClust , cr2 , top);
	MV_EDITE(m_wndSTMFTClust ,m_wndETMFTClust ,   cr2 , top);

 	//MFTMirr�ĵ�һ���غ�
	MV_STATIC(m_wndETMFTClust ,m_wndSTMFTMirrClust , cr2 , top);
	MV_EDITE(m_wndSTMFTMirrClust ,m_wndETMFTMirrClust ,   cr2 , top);

	//�豸��Ϣ����
	MV_STATIC_AREA(m_wndETMFTMirrClust , m_wndSTPartInfoArea  , topbak , top  );

	top += 7 * CTRL_Y_GAP;
	topbak = top;

	//ʣ����������ʼ������
	MV_STATIC_NO_PRE(m_wndSTRemainStartSector , cr2 , top);
	MV_EDITE(m_wndSTRemainStartSector ,m_wndETRemainStartSector , cr2 , top);

	//ʣ��������
	MV_STATIC(m_wndETRemainStartSector ,m_wndSTRemainCount , cr2 , top);
	MV_EDITE(m_wndSTRemainCount ,m_wndETRemainCount ,   cr2 , top);

	//ʣ��������С
	MV_STATIC(m_wndETRemainCount ,m_wndSTRemainSize , cr2 , top);
	MV_EDITE(m_wndSTRemainSize ,m_wndETRemainSize ,   cr2 , top);

	//������Ϣ��ʾ����
	MV_STATIC_AREA(m_wndETRemainSize , m_wndSTRemainSectorArea  , topbak , top  );

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

	//���ù������Ĺ�����Χ
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
	//����������Ҫ���µ�������ʾ 
	CNtfsDoc* pDoc = this->GetDocument();
	LONG_INT liCurSec = {0};
	CString strTemp;

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
	BYTE secPerClust = pDoc->m_pNtfs->GetSecPerClust();
	liCurSec.QuadPart /= secPerClust; 
	liCurSec.HighPart ? strTemp.Format(_T("%X%08X") , liCurSec.HighPart , liCurSec.LowPart):strTemp.Format(_T("%X") , liCurSec.LowPart);
	this->m_wndETClustNum.SetWindowText(strTemp);

}

BOOL CNtfsInfoView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

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

	//��������������
	m_wndETSectorNum.GetWindowText(strCurSector);
	strCurSector.TrimLeft();
	strCurSector.TrimRight();

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


	liSecCnt = liSec;
	liSecCnt.QuadPart /= secPerClust;
	//���ôغ�
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

	//��������������
	m_wndETClustNum.GetWindowText(strCurClust);
	strCurClust.TrimLeft();
	strCurClust.TrimRight();

	//���õ�Ĭ����������
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
	liClustCnt.QuadPart = pDoc->GetSecCount().QuadPart / secPerClust;

	if (liClustCnt.QuadPart <= liClust.QuadPart)
	{//Խ���� 
		strCap.LoadString(IDS_WARNING);
		strCurClust.LoadString(IDS_CLUST_NUM_ERR);
		::MessageBox(this->GetSafeHwnd() , strCurClust , strCap , MB_OK | MB_ICONWARNING );
		return ;
	}

	liClustCnt = liClust;
	liClustCnt.QuadPart *= secPerClust;
	//����������
	liClustCnt.HighPart ? strTemp.Format(_T("%X%08X") , liClustCnt.HighPart , liClustCnt.LowPart):strTemp.Format(_T("%X") , liClustCnt.LowPart);
	this->m_wndETSectorNum.SetWindowText(strTemp);

	pDoc->ChangeCurSector(liClustCnt , this);

	return ;
}

