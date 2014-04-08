// DiskInfoView.cpp : ʵ���ļ�
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


// CDiskInfoView ���

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


// CDiskInfoView ��Ϣ�������

void CDiskInfoView::OnInitialUpdate()
{
	//��õ�ǰ��ͼ��Ӧ���ĵ�����
	CDiskDoc*	pDoc = GetDocument();
	Disk*		pDisk = pDoc->m_pDisk;
	LONG_INT	size = {0};
	CString		strTemp;
	int			i = 0;
	int			partCnt = 0;
	Disk::PDPart pPart;

	CInfoView::OnInitialUpdate();

	//�豸���ֱ�ǩ
	CRT_STATIC(m_wndSTName ,IDC_ST_DEV_NAME ,  IDS_DEV_NAME);

	//�豸����
	strTemp = pDoc->GetDevName();  //�豸������
	CRT_EDIT(m_wndETDevName , IDC_ET_DEV_NAME ,strTemp );

	//������ϸ��Ϣ�Ŀ��
	CRT_STATIC_AREA(m_wndSTCanPartArea ,IDC_ST_CAN_PART_AREA , IDS_CAN_PART_AREA);

	//�ɷ�����С
	CRT_STATIC(m_wndSTCanPart ,IDC_ST_CAN_PART_SIZE ,  IDS_CAN_PART);

	//�ɷ�����С�ֽ���
	size = pDoc->m_pDisk->GetPartableSecCount();  //���̵Ŀɷ���������
	size.QuadPart *= SECTOR_SIZE;
	size.HighPart == 0 ? strTemp.Format(_T("%X") ,size.LowPart) : strTemp.Format(_T("%X%08X") , size.HighPart , size.LowPart);
	CRT_EDIT(m_wndETCanPartByte , IDC_ET_CAN_PART_BYTE ,strTemp + _T("Byte"));

	//�ɷ�����С�ߴ�
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETCanPartSize , IDC_ET_CAN_PART_SIZE ,strTemp);

	//δ������С
	CRT_STATIC(m_wndSTNoPart ,IDC_ST_NO_PART_SIZE ,  IDS_NO_PART);

	//δ������С�ֽ���
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

	//δ������С�ߴ�
	strTemp = GetSizeToString(size);
	CRT_EDIT(m_wndETNoPartSize , IDC_ET_NO_PART_SIZE ,strTemp);

	//ÿ�����ֽ��� ��ǩ
	CRT_STATIC(m_wndSTBytePerSector ,IDC_ST_BYTE_PER_SECTOR ,  IDS_BYTE_PER_SECTOR);
	
	//ÿ�����ֽ��� ��С
	strTemp.Format(_T("%X") , pDisk->GetBytePerSec());
	CRT_EDIT(m_wndETBytePerSector , IDC_ET_BYTE_PER_SECTOR ,strTemp);

	//ÿ�ŵ������� ��ǩ
	CRT_STATIC(m_wndSTSectorPerTrack ,IDC_ST_SECTOR_PER_TRACK ,  IDS_SECTOR_PER_TRACK);

	//ÿ�ŵ������� ��ֵ
	strTemp.Format(_T("%X") , pDisk->GetSectorPerTrack());
	CRT_EDIT(m_wndETSectorPerTrack , IDC_ET_SECTOR_PER_TRACK ,strTemp);

	//ÿ����ŵ��� ��ǩ
	CRT_STATIC(m_wndSTTracksPerCylinder ,IDC_ST_TRACK_PER_CYLINDER ,  IDS_TRACK_PER_CYLINDER);

	//ÿ����ŵ��� ��ֵ
	strTemp.Format(_T("%X") , pDisk->GetTracksPerCylinder());
	CRT_EDIT(m_wndETTracksPerCylinder , IDC_ET_TRACK_PER_CYLINDER ,strTemp);

	//������ ��ǩ
	CRT_STATIC(m_wndSTCylinders ,IDC_ST_CYLINDERS ,  IDS_CYLINDERS);

	//������ ��ֵ
	strTemp.Format(_T("%X") , pDisk->GetCylinders());
	CRT_EDIT(m_wndETCylinders , IDC_ET_CYLINDERS ,strTemp);

	//������������Ϣ
	CRT_STATIC_AREA(m_wndSTCanNotPartArea , IDC_ST_CAN_NOT_PART_AREA , IDS_CAN_NOT_PART_AREA);

	//���ɷ�����С��ǩ
	CRT_STATIC(m_wndSTCanNotPart ,IDC_ST_CAN_NOT_PART_SIZE ,  IDS_CAN_NOT_PART);

	//�ɷ�����С�ֽ���
	strTemp.Format(_T("%X") ,pDoc->m_pDisk->GetUnPartableSize());
	CRT_EDIT(m_wndETCanNotPartByte , IDC_ET_CAN_NOT_PART_BYTE ,strTemp + _T("Byte"));

	//�ɷ�����С�ߴ�
	strTemp = GetSizeToString(pDoc->m_pDisk->GetUnPartableSize());
	CRT_EDIT(m_wndETCanNotPartSize , IDC_ET_CAN_NOT_PART_SIZE ,strTemp);

	//�߼���������
	CRT_STATIC_AREA(m_wndSTLgcDrivArea , IDC_ST_LOGICAL_DRIVER_AREA , IDS_LOGICAL_DRIVER_AREA);

	//������ ��ǩ
	CRT_STATIC(m_wndSTPartCnt ,IDC_ST_LOGICAL_DRIVER_COUNT ,  IDS_LOGICAL_DRIVER_COUNT);

	//�������� ��ֵ
	strTemp.Format(_T("%X") , pDisk->GetVolumeCount()) ;
	CRT_EDIT(m_wndETPartCnt , IDC_ET_LOGICAL_DRIVER_COUNT ,strTemp);

 	//��������
	CRT_STATIC(m_wndSTMainPartCnt ,IDC_ST_MAIN_PART_COUNT ,  IDS_MAIN_PART_COUNT);

	//��������
	strTemp.Format(_T("%X") , pDisk->GetMainVolCount()) ;
	CRT_EDIT(m_wndETMainPartCnt , IDC_ET_MAIN_PART_COUNT ,strTemp);

 	//�߼�������
	CRT_STATIC(m_wndSTLgcPartCnt ,IDC_ST_LOGICAL_PART_COUNT ,  IDS_LOGICAL_PART_COUNT);
	
	//�߼�������
	strTemp.Format(_T("%X") , pDisk->GetVolumeCount() - pDisk->GetMainVolCount()) ;
	CRT_EDIT(m_wndETLgcPartCnt , IDC_ET_LOGICAL_PART_COUNT ,strTemp);

	//���������������
	CRT_STATIC_AREA(m_wndSTDataScanArea , IDC_ST_DATA_SCAN_AREA , IDS_DATA_SCAN_AREA);

	//�����������
	CRT_SCAN_CTRLS(m_wndSTSectorScan   , IDS_SECTOR_SCAN  , IDC_ST_SECTOR_SCAN ,	//��ǩ
				   m_wndETSectorNum    , IDC_ET_SECTOR_NUM ,						//������ʾ������
				   m_wndBtnPreSector   , IDC_BTN_PRE_SECTOR ,						//ǰһ������
				   m_wndBtnNextSector  , IDC_BTN_NEXT_SECTOR ,						//��һ������
				   m_wndBtnFirstSector , IDC_BTN_FIRST_SECTOR ,						//��һ������
				   m_wndBtnLastSector  , IDC_BTN_LAST_SECTOR);						//���һ������


	//ǿ�и���һ��������ͼ
	this->OnUpdate(NULL , 0 , NULL );
}

int CDiskInfoView::RePositionCtrl(void)
{
	CRect	cr;			//��ǰ��ͼ�Ŀɿͻ����Ĵ�С
	CRect	cw;			//��ǰ��ͼ�Ĵ��ھ���
	CRect	ctlRc1;
	CString strTemp;
	int		top = 5 ;	//�ؼ��Ķ���λ��
	int		topbak = 0;	//����λ�õ�һ������  �����������ĵط���Ҫʹ��
	int		width = 0 ;
	int btnW , btnH;
	//��ť��Ψ��
	btnW = this->m_szChar.cx * 2 + 2 * CTRL_X_GAP;
	btnH = CTRL_Y_GAP + m_szChar.cy;

	TRACE0("��Ҫ��ѯ�����ļ������壬��С");

	//��õ�ǰ��ͼ�Ŀͻ�����С
	::GetWindowRect(this->GetSafeHwnd() , &cw);
	cr = cw;
	cr.DeflateRect(11 , 7);
	this->ScreenToClient(&cr);  //�ͻ����пռ���Է�ֹ������
	CRect cr2 = cr;
	cr2.DeflateRect(2 * CTRL_X_GAP , 0);

	//��õ�һ���ؼ���λ��
	//IDC_ST_DEV_NAME
	CRect c1stRc;
	m_wndSTName.GetWindowRect(c1stRc);
	if (c1stRc.Height() == 0)
	{//��һ�ε��ô˺���
		m_wndSTName.MoveWindow(CRect( 11 , 5 , 78 , 23) , FALSE);
		top = 5;
	}else{
		this->ScreenToClient(&c1stRc);  //�ͻ����пռ���Է�ֹ������
		top = c1stRc.top;
	}
 
	//�����豸���ֿؼ��Ĵ�С��λ��
	MV_EDITE(m_wndSTName , m_wndETDevName  , cr , top);

	top += (6 * CTRL_Y_GAP );
	topbak = top ;

	//�ɷ�����С��ǩ
	MV_STATIC_NO_PRE(m_wndSTCanPart ,cr2 , top);
	MV_EDITE(m_wndSTCanPart ,m_wndETCanPartByte ,   cr2 , top);

	//�ɷ�����С�ߴ�
	m_wndETCanPartByte.GetWindowRect(&ctlRc1);
	m_wndETCanPartSize.MoveWindow(cr.left + CTRL_X_GAP , top += ctlRc1.Height() , cr.Width() - 3 * CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//δ����
	MV_STATIC(m_wndETCanPartSize , m_wndSTNoPart , cr2 , top);
	MV_EDITE(m_wndSTNoPart ,m_wndETNoPartByte ,   cr2 , top);

	//δ������С�ߴ�
	m_wndETNoPartByte.GetWindowRect(&ctlRc1);
	m_wndETNoPartSize.MoveWindow(cr.left + CTRL_X_GAP , top += ctlRc1.Height() , cr.Width() - 3 * CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//ÿ�����ֽ���
	MV_STATIC(m_wndETNoPartSize , m_wndSTBytePerSector , cr2 , top);
	MV_EDITE(m_wndSTBytePerSector ,m_wndETBytePerSector ,  cr2 , top);

	//ÿ�ŵ�������
	MV_STATIC(m_wndETBytePerSector , m_wndSTSectorPerTrack , cr2 , top);
	MV_EDITE(m_wndSTSectorPerTrack ,m_wndETSectorPerTrack ,  cr2 , top);

	//ÿ����ŵ��� ��ǩ
	MV_STATIC(m_wndETSectorPerTrack , m_wndSTTracksPerCylinder , cr2 , top);
	MV_EDITE(m_wndSTTracksPerCylinder ,m_wndETTracksPerCylinder,  cr2 , top);

	//ÿ����ŵ��� ��ǩ
	MV_STATIC(m_wndETTracksPerCylinder , m_wndSTCylinders  , cr2 , top);
	MV_EDITE(m_wndSTCylinders ,m_wndETCylinders  ,  cr2 , top);

	//������Ϣ������
	MV_STATIC_AREA(m_wndETTracksPerCylinder , m_wndSTCanPartArea , topbak , top);

	//��һ�������ʼλ��
	top += (7 * CTRL_Y_GAP );
	topbak = top ;
	//���ɷ�����С
	MV_STATIC_NO_PRE(m_wndSTCanNotPart , cr2 , top);
	MV_EDITE(m_wndSTCanNotPart ,m_wndETCanNotPartByte ,   cr2 , top);

	//���ɷ�����С�ߴ�
	m_wndETCanNotPartByte.GetWindowRect(&ctlRc1);
	m_wndETCanNotPartSize.MoveWindow(cr.left + CTRL_X_GAP , top += ctlRc1.Height() ,cr.Width() - 3 * CTRL_X_GAP , ctlRc1.Height() , FALSE );

	//���ɷ�������
	MV_STATIC_AREA(m_wndETCanNotPartSize , m_wndSTCanNotPartArea , topbak , top);

 	//�ܷ�����
	top += (7 * CTRL_Y_GAP );
	topbak = top;
	
	//���ɷ�����С��ǩ
	MV_STATIC_NO_PRE(m_wndSTPartCnt , cr2 , top);
	MV_EDITE(m_wndSTPartCnt , m_wndETPartCnt , cr2 , top);

 	//��������
	MV_STATIC(m_wndETPartCnt , m_wndSTMainPartCnt , cr2 , top);
	MV_EDITE(m_wndSTMainPartCnt , m_wndETMainPartCnt , cr2 , top);

 	//�߼�������
	MV_STATIC(m_wndETMainPartCnt , m_wndSTLgcPartCnt , cr2 , top);
	MV_EDITE(m_wndSTLgcPartCnt , m_wndETLgcPartCnt , cr2 , top);

 	//�߼���������
	MV_STATIC_AREA(m_wndETLgcPartCnt , m_wndSTLgcDrivArea , topbak , top);


	top += (7 * CTRL_Y_GAP );
	topbak = top;

	//�������
	MV_SCAN_CTRLS(top , cr2 , btnW , btnH ,	m_wndSTSectorScan ,
								m_wndBtnFirstSector ,
								m_wndBtnLastSector , 
								m_wndBtnPreSector ,
								m_wndBtnNextSector ,
								m_wndETSectorNum);

	//������Ϣ������
	MV_STATIC_AREA( m_wndETSectorNum,m_wndSTDataScanArea , topbak , top);

	//���ù������Ĺ�����Χ
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

	//���ڴ�С����ʱ �����¼�������ռ��λ�� 
	if(IsWindow(m_wndSTName.GetSafeHwnd()))
		RePositionCtrl();
}
void CDiskInfoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* /*pHint*/)
{
	//����������Ҫ���µ�������ʾ 
	CDiskDoc* pDoc = this->GetDocument();
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

}

BOOL CDiskInfoView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{    
		if(&m_wndETSectorNum ==  GetFocus())
		{//��ǰ�����༭���ϰ��˻س�
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

	pDoc->ChangeCurSector(liSec , this);

	return ;
}
