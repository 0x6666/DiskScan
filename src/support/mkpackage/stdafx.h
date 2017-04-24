#ifndef __MK_PACKAGE_STDAFX_H__
#define __MK_PACKAGE_STDAFX_H__

#include <vector>
#include <assert.h>

#include <windows.h>

#ifndef ASSERT
# ifdef _DEBUG
#  define ASSERT assert
# else
#  define ASSERT
# endif
#endif

#ifndef WCHAR
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
#endif

#endif //__MK_PACKAGE_STDAFX_H__
