#pragma once
#include "InfoView.h"
#include "afxwin.h"
#include "DiskScanDoc.h"



// CDiskInfoView ������ͼ

class CDiskInfoView : public CInfoView
{
	DECLARE_DYNCREATE(CDiskInfoView)

protected:
	CDiskInfoView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CDiskInfoView();

public:
	enum { IDD = IDD_INFOVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();


private: //������ʾ��Ϣ�Ŀؼ�
	CStatic m_wndSTName;
	CEdit	m_wndETDevName;

	//���̿ɷ�����С
	CStatic m_wndSTCanPart;			//�ɷ�����С��ǩ
	CEdit	m_wndETCanPartByte;		//�ɷ�����С�ֽ���
	CEdit	m_wndETCanPartSize;		//�ɷ�����С�ߴ�

	//δ������С
	CStatic m_wndSTNoPart;			//δ������С��ǩ
	CEdit	m_wndETNoPartByte;		//δ������С�ֽ���
	CEdit	m_wndETNoPartSize;		//δ������С�ߴ�

	//ÿ�����ֽ���
	CStatic m_wndSTBytePerSector;	
	CEdit	m_wndETBytePerSector;

	//ÿ�ŵ������� 
	CStatic m_wndSTSectorPerTrack;
	CEdit	m_wndETSectorPerTrack;
	
	//ÿ����ŵ���
	CStatic m_wndSTTracksPerCylinder;
	CEdit	m_wndETTracksPerCylinder;

	//������
	CStatic m_wndSTCylinders;
	CEdit	m_wndETCylinders;


	//���̵���ϸ��Ϣ�Ŀ�ܣ����ֿ�ܱ�������CStatic�ģ�
	//��ʵ������Ҫʹ�õ�����BS_GROUPBOX��CButton������
	//������ʹ��CButton��,����������������ʹ��CButton֮
	//��û�����ط�����CStatic����
	CButton m_wndSTCanPartArea;

	//���ɷ�����Ϣ
	CButton	m_wndSTCanNotPartArea;

	//���̲��ɷ�����С
	CStatic m_wndSTCanNotPart;			//���ɷ�����С��ǩ
	CEdit	m_wndETCanNotPartByte;		//���ɷ�����С�ֽ���
	CEdit	m_wndETCanNotPartSize;		//���ɷ�����С�ߴ�

	//�߼���������
	CButton	m_wndSTLgcDrivArea;

	//�ܷ�����
	CStatic m_wndSTPartCnt;		//������
	CEdit	m_wndETPartCnt;	

	//��������
	CStatic m_wndSTMainPartCnt;
	CEdit	m_wndETMainPartCnt;

	//�߼�������
	CStatic m_wndSTLgcPartCnt;
	CEdit	m_wndETLgcPartCnt;



	//�������
	CButton	m_wndSTDataScanArea;

	//������
	CStatic m_wndSTSectorScan;
	CEdit	m_wndETSectorNum;

	//��/��һ����
	CButton m_wndBtnPreSector;	//��һ����
	CButton m_wndBtnNextSector;	//��һ����

	//��һ/�������
	CButton m_wndBtnFirstSector;//��һ����
	CButton m_wndBtnLastSector;	//���һ����

// 
// 	//һ���ַ��Ĵ�С
// 	CSize	m_szChar;
protected:
	// ���ÿؼ���λ��
	int RePositionCtrl(void);

	CDiskDoc* GetDocument();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnDestroy();
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//////////////////////////////////////////////////////////////////////////
	//�ڵ�ǰ�����༭���ϰ����˻س���
	//////////////////////////////////////////////////////////////////////////
	void OnEnterCurSector();
};


