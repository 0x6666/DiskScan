#pragma once
#include "InfoView.h"
#include "NtfsDoc.h"



// CNtfsInfoView ������ͼ

class CNtfsInfoView : public CInfoView
{
	DECLARE_DYNCREATE(CNtfsInfoView)

protected:
	CNtfsInfoView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CNtfsInfoView();

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

	//��õ�ǰ��ͼ��Ӧ���ĵ�
	CNtfsDoc* GetDocument();

	//��ʼ����ͼ���Լ����������ؼ�
	virtual void OnInitialUpdate();

	//////////////////////////////////////////////////////////////////////////
	//������ͼ�и����ؼ���λ��
	//////////////////////////////////////////////////////////////////////////
	virtual int RePositionCtrl();

protected:
	//�豸��Ϣ����
	CButton m_wndSTDevInfoArea;

	//�豸����
	CStatic m_wndSTName;
	CEdit	m_wndETDevName;

	//�豸�������
	CStatic m_wndSTDevAreaIdx;
	CEdit	m_wndETDevAreaIdx;

	//��ʼ������
	CStatic m_wndSTStartSector;
	CEdit	m_wndETStartSector;

	//������Ϣ����
	CButton m_wndSTPartInfoArea;
	//��������
	CStatic m_wndSTPartName;
	CEdit	m_wndETPartName;

	//������С
	CStatic m_wndSTPartSize;
	CEdit	m_wndETPartSizeByte;
	CEdit	m_wndETPartSize;

	//��������
	CStatic m_wndSTSectorCount;
	CEdit	m_wndETSectorCount;

	//ÿ��������
	CStatic m_wndSTSectorPerClust;
	CEdit	m_wndETSectorPerClust;

	//MFT�ĵ�һ���غ�
	CStatic m_wndSTMFTClust;
	CEdit	m_wndETMFTClust;

	//MFTMirr�ĵ�һ���غ�
	CStatic m_wndSTMFTMirrClust;
	CEdit	m_wndETMFTMirrClust;


	//�ļ�ϵͳ
	CStatic m_wndSTFileSys;	
	CEdit	m_wndETFileSys;


	//ʣ������
	CButton m_wndSTRemainSectorArea;

	//ʣ����������ʼ������
	CStatic m_wndSTRemainStartSector;	
	CEdit	m_wndETRemainStartSector;

	//ʣ��������
	CStatic m_wndSTRemainCount;	
	CEdit	m_wndETRemainCount;

	//ʣ��������С
	CStatic m_wndSTRemainSize;	
	CEdit	m_wndETRemainSize;


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


	//�غ����
	CStatic m_wndSTClustScan;
	CEdit	m_wndETClustNum;

	//��/��һ��
	CButton m_wndBtnPreClust;	//��һ��
	CButton m_wndBtnNextClust;	//��һ��

	//��һ/����
	CButton m_wndBtnFirstClust; //��һ��
	CButton m_wndBtnLastClust;	//���һ��


public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//////////////////////////////////////////////////////////////////////////
	//�ڵ�ǰ�����༭���ϰ����˻س���
	//////////////////////////////////////////////////////////////////////////
	void OnEnterCurSector();

	//////////////////////////////////////////////////////////////////////////
	//�ڵ�ǰ�غű༭���ϰ����˻س���
	//////////////////////////////////////////////////////////////////////////
	void OnEnterCurClust();
};


