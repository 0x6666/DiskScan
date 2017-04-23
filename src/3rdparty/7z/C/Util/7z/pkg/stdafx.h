
#ifndef __7ZDEC_H__
#define __7ZDEC_H__

#include <string.h>
#include <string>

#include "../../../../CPP/Common/Common.h"
#include "../../../../CPP/Common/MyCom.h"

#include "../../7z.h"
#include "../../7zAlloc.h"
#include "../../7zBuf.h"
#include "../../7zCrc.h"
#include "../../7zFile.h"
#include "../../7zVersion.h"

#include <stdio.h>

#ifdef __HRESULT_FROM_WIN32
#define HRESULT_WIN32_ERROR_NEGATIVE_SEEK __HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK)
#else
#define HRESULT_WIN32_ERROR_NEGATIVE_SEEK   HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK)
#endif

#endif //__7ZDEC_H__