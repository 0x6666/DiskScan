// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0EFB6409_A313_4567_8F5C_93EC408EBA66__INCLUDED_)
#define AFX_STDAFX_H__0EFB6409_A313_4567_8F5C_93EC408EBA66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_WINNT 0x0502

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include "winioctl.h"

//�����صĿ��ļ�
#include "../StrTool/StrTool.h"
#include "../DiskTool/disktool.h"

//#ifdef  _INCLUDE_LIB_  //�������ظ�����  �����������ظ���������ν
//#define _INCLUDE_LIB_

//�ַ����е�Ҫ�滻�����ֵ�λ��
//#define		NAME_POS	_T("[NAME]")
#define		HCAR_POS	_T("[CHAR]")
#define		STR_POS		_T("{_[STR_POS]_}")//һ���ַ�����λ�� 


//������ImageList�е�ͼ��
//��������� void CMainFrame::InitImageList()
#define  ICOM_MAIN		0  //�������ͼ��
#define  ICON_DISK		1  //���̵�ͼ��
#define  ICON_FILE		2  //�ļ�
#define  ICON_FOLDER	3  //�ļ���
#define  ICON_VOLUME	4  //��


//�Լ��������Ϣ����


//////////////////////////////////////////////////////////////////////////
//֪ͨ�����ڣ���Ҫ������ݻ�������Ҳ����ʮ�����ƿؼ����������
//param
//	wParam==>BOOL
//				1	���ָ���ĴغŲ����ڵĻ�������һ����Ч�غ�
//				0	���ָ���ĴغŲ����ڵĻ�������һ����Ч�غ�
//	lParam==>(PDATA_BUF) ��Ҫ��ȡ���ݵ������ţ��Լ�����
//////////////////////////////////////////////////////////////////////////
#define DATA_CTRL_MSG			WM_USER + 1		//�˺���Ҫ��ʵ�ʵ���Ŀ�о�������

//////////////////////////////////////////////////////////////////////////
//֪ͨ������ʮ�����ƿؼ���Ҫ����ʾ����Ѿ��ı���,
//param
//	wParam==>int			�ؼ���Ҫ���¿��
//////////////////////////////////////////////////////////////////////////
#define DATA_CHANGE_WIDTH		WM_USER + 2		//�˺���Ҫ��ʵ�ʵ���Ŀ�о�������

//////////////////////////////////////////////////////////////////////////
//֪ͨ�����ڵ�ǰ��ʾ�����ݵ��������Ѿ��ı���
//param
//		wParam==>ULONG		�����ŵĵ����ֽ�
//		lParam==>LONG		�����ŵĸ���ֽ�	����Ϊ0
//////////////////////////////////////////////////////////////////////////
#define DATA_CHANGE_SECTOR		WM_USER + 3

//////////////////////////////////////////////////////////////////////////
//�Ѿ�����˲��ɷ���Ŀռ��С
//param
//		wParam	���岻�ɷ���ռ�Ĵ�С
//////////////////////////////////////////////////////////////////////////
#define DMSG_GOT_UNPARTABLE_SEC	WM_USER + 4

// #define  PART_MBR		0x100			//MBR
// #define  PART_EBR		0x101			//EBR
// #define  PART_UN_PART	0x102			//û�����Ŀ�������
// #define  PART_UNPARTBLE 0x103			//���ܷ���������
// #define	PART_FAT32		0x0C			//FAT32  
// #define  PART_NTFS		0x07			//NTFS
//////////////////////////////////////////////////////////////////////////
//֪ͨ����ܴ�һ���µ��ĵ�
//param
//		wParam=��CString* ���ڴ��ĵ��Ĳ���
//		lParam			  Ҫ�򿪵��豸����	PART_*  ����ֻ֧�� PART_FAT32
//////////////////////////////////////////////////////////////////////////
#define DMSG_OPEN_NEW_DOC	WM_USER + 5

//////////////////////////////////////////////////////////////////////////
//���ļ�ʧ��
//param
//		wParam=��CString* �ļ�·��
//////////////////////////////////////////////////////////////////////////
#define DMSG_OPEN_FILE_FAILED	WM_USER + 6

//////////////////////////////////////////////////////////////////////////
//�����ļ�ʧ�� 
//param
//		wParam=>CString*	�����Ƶ��ļ�
//		lParam=>CString*	��д���ļ�
//////////////////////////////////////////////////////////////////////////
#define DMSG_COPY_FILE_FAILED	WM_USER + 7

//////////////////////////////////////////////////////////////////////////
//�ļ���������
//////////////////////////////////////////////////////////////////////////
#define DMSG_COPY_SUCCESS		WM_USER + 8

//////////////////////////////////////////////////////////////////////////
//���õ�ǰ��������ͼ����ʾ���ݵ�����
//param
//		wParam=>CString*	��������
#define DMSG_SET_CUR_DATA_VIEW_NAME WM_USER + 9



///���豸ʱ�Ĳ������� 
#define PT_DEVNAME	0	//Ҫ�򿪵��豸����,�˲���û������ͷ
#define PT_INDEX	1	//�������ڴ����ϵ�����λ����� �� ����ͷΪ -idx
#define PT_OFFSET	2	//�������ڴ����ϵ�ƫ��,����ͷΪ			   -off	
#define PT_LETTER	3	//���������̷�							   -let

//��������
#define PN_DEVNAME	_T("")		//�豸����
#define PN_INDEX	_T("-IDX ")	//����	ʮ����
#define PN_OFFSET	_T("-OFF ")	//ƫ��  ʮ������
#define PN_LETTER	_T("-LET ")	//�̷�  һ���������̷�


//��Ϣ��ͼ�пؼ���֮��ļ�϶
#define CTRL_X_GAP	4	//ˮƽ����ļ�϶
#define CTRL_Y_GAP	8	//��ֱ����ļ�϶


//�ַ������ж�
#define  isHexChar(x) (('0' <= (x) && '9' >= (x)) || ('a' <= (x) && 'f' >= (x)) || ('A' <= (x) && 'F' >= (x)))
//������
#define isDigit(x) ('0' <= (x) && '9' >= (x))
//�Ƿ����ַ�
#define isLetter(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))


//////////////////////////////////////////////////////////////////////////
//�˺������ǰ����Ľ��������WinNT�����û����˺�Ļ�������WIn2K��
//����Ϊ1ʱ��ʾ WinNT������Ϊ0ʱ���ʾWin2K
//////////////////////////////////////////////////////////////////////////
#define FOR_NT	1

//#include <vld.h>

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0EFB6409_A313_4567_8F5C_93EC408EBA66__INCLUDED_)

