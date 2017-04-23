
#ifndef __7Z_H__
#define __7Z_H__

#include "CPP/Common/MyCom.h"
#include "CPP/Common/MyUnknown.h"

// {93A0559B-01F7-4BEC-B2BB-E9BCEEC2AC1C}
DEFINE_GUID(IID_I7zCompress, 0x93a0559b, 0x1f7, 0x4bec, 0xb2, 0xbb, 0xe9, 0xbc, 0xee, 0xc2, 0xac, 0x1c);

interface /*__declspec(novtable)*/ I7zCompress : public IUnknown
{
	STDMETHOD(AddFile) (LPCWSTR szPath, LPCWSTR szInnerPath) PURE;
	STDMETHOD(Folder) (LPCWSTR szPath) PURE;
	STDMETHOD(Compress) (LPCWSTR szOutPath) PURE;
	virtual ~I7zCompress() {};
};


// {A4D4EB39-C2B2-4DD5-BC40-108E02A1FABA}
DEFINE_GUID(IID_I7zExtracter, 0xa4d4eb39, 0xc2b2, 0x4dd5, 0xbc, 0x40, 0x10, 0x8e, 0x2, 0xa1, 0xfa, 0xba);

interface /*__declspec(novtable)*/ I7zExtracter : public IUnknown
{
	STDMETHOD(Open7zFile)(IN LPCWSTR szPath) PURE;
	STDMETHOD(OpenBuf)(IN void* pBuf, IN UINT32 size) PURE;
	STDMETHOD(ExtractTo)(IN LPCWSTR szPath) PURE;
	STDMETHOD(NumFiles)(OUT UINT32*) PURE;
	STDMETHOD(IsDir)(IN UINT32, OUT BOOL*) PURE;
	STDMETHOD(GetFileName)(IN UINT32, OUT LPWSTR pBuf, IN OUT UINT32* pSize) PURE;
};


#endif //__7Z_H__
