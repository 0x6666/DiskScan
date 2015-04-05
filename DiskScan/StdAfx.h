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

//添加相关的库文件
#include "../StrTool/StrTool.h"
#include "../DiskTool/disktool.h"

//#ifdef  _INCLUDE_LIB_  //避免库的重复加载  好像这玩意重复加载无所谓
//#define _INCLUDE_LIB_

//字符串中的要替换的名字的位置
//#define		NAME_POS	_T("[NAME]")
#define		HCAR_POS	_T("[CHAR]")
#define		STR_POS		_T("{_[STR_POS]_}")//一个字符串的位置 


//以下是ImageList中的图标
//具体情况看 void CMainFrame::InitImageList()
#define  ICOM_MAIN		0  //程序的主图标
#define  ICON_DISK		1  //磁盘的图标
#define  ICON_FILE		2  //文件
#define  ICON_FOLDER	3  //文件夹
#define  ICON_VOLUME	4  //卷


//自己定义的消息类型


//////////////////////////////////////////////////////////////////////////
//通知父窗口，需要填充数据缓存区，也是这十六进制控件的数据入口
//param
//	wParam==>BOOL
//				1	如果指定的簇号不存在的话返回下一个有效簇号
//				0	如果指定的簇号不存在的话返回上一个有效簇号
//	lParam==>(PDATA_BUF) 需要读取数据的扇区号，以及缓存
//////////////////////////////////////////////////////////////////////////
#define DATA_CTRL_MSG			WM_USER + 1		//此宏需要在实际的项目中具体设置

//////////////////////////////////////////////////////////////////////////
//通知父窗口十六进制控件需要的显示宽度已经改变了,
//param
//	wParam==>int			控件需要的新宽度
//////////////////////////////////////////////////////////////////////////
#define DATA_CHANGE_WIDTH		WM_USER + 2		//此宏需要在实际的项目中具体设置

//////////////////////////////////////////////////////////////////////////
//通知父窗口当前显示的数据的扇区号已经改变了
//param
//		wParam==>ULONG		扇区号的低四字节
//		lParam==>LONG		扇区号的搞低字节	可能为0
//////////////////////////////////////////////////////////////////////////
#define DATA_CHANGE_SECTOR		WM_USER + 3

//////////////////////////////////////////////////////////////////////////
//已经获得了不可分配的空间大小
//param
//		wParam	具体不可分配空间的大小
//////////////////////////////////////////////////////////////////////////
#define DMSG_GOT_UNPARTABLE_SEC	WM_USER + 4

// #define  PART_MBR		0x100			//MBR
// #define  PART_EBR		0x101			//EBR
// #define  PART_UN_PART	0x102			//没分区的空闲区域
// #define  PART_UNPARTBLE 0x103			//不能分区的区域
// #define	PART_FAT32		0x0C			//FAT32  
// #define  PART_NTFS		0x07			//NTFS
//////////////////////////////////////////////////////////////////////////
//通知主框架打开一个新的文档
//param
//		wParam=》CString* 用于打开文档的参数
//		lParam			  要打开的设备类型	PART_*  现在只支持 PART_FAT32
//////////////////////////////////////////////////////////////////////////
#define DMSG_OPEN_NEW_DOC	WM_USER + 5

//////////////////////////////////////////////////////////////////////////
//打开文件失败
//param
//		wParam=》CString* 文件路径
//////////////////////////////////////////////////////////////////////////
#define DMSG_OPEN_FILE_FAILED	WM_USER + 6

//////////////////////////////////////////////////////////////////////////
//复制文件失败 
//param
//		wParam=>CString*	被复制的文件
//		lParam=>CString*	被写的文件
//////////////////////////////////////////////////////////////////////////
#define DMSG_COPY_FILE_FAILED	WM_USER + 7

//////////////////////////////////////////////////////////////////////////
//文件拷贝结束
//////////////////////////////////////////////////////////////////////////
#define DMSG_COPY_SUCCESS		WM_USER + 8

//////////////////////////////////////////////////////////////////////////
//设置当前数据区视图的显示数据的名字
//param
//		wParam=>CString*	区域名字
#define DMSG_SET_CUR_DATA_VIEW_NAME WM_USER + 9



///打开设备时的参数类型 
#define PT_DEVNAME	0	//要打开的设备名字,此参数没有类型头
#define PT_INDEX	1	//本区域在磁盘上的区域位置序号 ， 参数头为 -idx
#define PT_OFFSET	2	//本区域在磁盘上的偏移,参数头为			   -off	
#define PT_LETTER	3	//本分区的盘符							   -let

//参数名字
#define PN_DEVNAME	_T("")		//设备名字
#define PN_INDEX	_T("-IDX ")	//索引	十进制
#define PN_OFFSET	_T("-OFF ")	//偏移  十六进制
#define PN_LETTER	_T("-LET ")	//盘符  一个分区的盘符


//信息视图中控件的之间的间隙
#define CTRL_X_GAP	4	//水平方向的间隙
#define CTRL_Y_GAP	8	//垂直方向的间隙


//字符类型判断
#define  isHexChar(x) (('0' <= (x) && '9' >= (x)) || ('a' <= (x) && 'f' >= (x)) || ('A' <= (x) && 'F' >= (x)))
//是数字
#define isDigit(x) ('0' <= (x) && '9' >= (x))
//是否是字符
#define isLetter(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))


//////////////////////////////////////////////////////////////////////////
//此宏决定当前编译的结果是适于WinNT，如果没定义此宏的话则适于WIn2K的
//定义为1时表示 WinNT，定义为0时则表示Win2K
//////////////////////////////////////////////////////////////////////////
#define FOR_NT	1

//#include <vld.h>

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0EFB6409_A313_4567_8F5C_93EC408EBA66__INCLUDED_)

