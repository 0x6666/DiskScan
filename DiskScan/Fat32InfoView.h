#pragma once
#include "InfoView.h"
#include "Fat32Doc.h"



// CFat32InfoView ������ͼ

class CFat32InfoView : public CInfoView
{
	DECLARE_DYNCREATE(CFat32InfoView)

protected:
	CFat32InfoView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CFat32InfoView();

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

	//����������
	CStatic m_wndSTReserveSector;
	CEdit	m_wndETReserveSector;

	//ÿ��������
	CStatic m_wndSTSectorPerClust;
	CEdit	m_wndETSectorPerClust;

	//ÿFAT������
	CStatic m_wndSTSectorPerFAT;
	CEdit	m_wndETSectorPerFAT;

	//��һ��FATλ��
	CStatic m_wndSTFAT1;
	CEdit	m_wndETFAT1;

	//�ڶ���FATλ��
	CStatic m_wndSTFAT2;
	CEdit	m_wndETFAT2;

	//FSInfo
	CStatic m_wndSTFSInfo;
	CEdit	m_wndETFSInfo;

	//��Ŀ¼����
	CStatic m_wndSTRootDir;
	CEdit	m_wndETRootDir;


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

	//�ļ�ϵͳ
	CStatic m_wndSTFileSys;	
	CEdit	m_wndETFileSys;


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
	enum { IDD = IDD_INFOVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	//��õ�ǰ��ͼ��Ӧ���ĵ�
	CFat32Doc* GetDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual int RePositionCtrl();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnEnterCurSector();
	void OnEnterCurClust();
};


