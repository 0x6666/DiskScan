
#ifndef __DISKTOOL_H__
#define __DISKTOOL_H__

#include <assert.h>
#include "string.h"
#include "stdio.h"
#include "time.h"

#ifdef _MSC_VER  //是vc编译器
#include "windows.h"
#include "Winioctl.h"
#endif


#include <strtool/strtool.h>

#include "disktool.h"
#include "stut_connf.h"

#ifdef _UNICODE
#define _T(x)       L ## x
#else
#define _T(x)       x
#endif //_UNICODE

#endif //__DISKTOOL_H__