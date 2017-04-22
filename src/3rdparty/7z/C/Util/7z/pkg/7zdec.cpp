

#include "7zdec.h"
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

class C7zDecExtracter
	: public I7zExtracter
	, private CMyUnknownImp
{
public:
	C7zDecExtracter();
	~C7zDecExtracter();

	MY_UNKNOWN_IMP1(I7zExtracter)

	STDMETHOD(Open7zFile)(LPCWSTR szPath) override;
	STDMETHOD(SetOutDirectory)(LPCWSTR szPath) override;
	STDMETHOD(NumFiles)(UINT32*) override;
	STDMETHOD(IsDir)(UINT32, BOOL*) override;
	STDMETHOD(GetFileName)(IN UINT32, OUT LPWSTR pBuf, IN OUT UINT32* pSize) override;

private:
	bool isOpened() const { return !sz7zFile.empty(); };

private:
	CFileInStream archiveStream;
	CLookToRead lookStream;
	CSzArEx db;
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	std::wstring sz7zFile;
};


C7zDecExtracter::C7zDecExtracter()
{
	allocImp.Alloc = SzAlloc;
	allocImp.Free = SzFree;

	allocTempImp.Alloc = SzAllocTemp;
	allocTempImp.Free = SzFreeTemp;
}

C7zDecExtracter::~C7zDecExtracter()
{
	if (isOpened())
	{
		SzArEx_Free(&db, &allocImp);
		File_Close(&archiveStream.file);
	}
}

STDMETHODIMP C7zDecExtracter::Open7zFile(LPCWSTR szPath)
{
	if (szPath == nullptr || wcslen(szPath) == 0)
		return E_INVALIDARG;

	if (isOpened())
		return E_FAIL;

	if (InFile_OpenW(&archiveStream.file, szPath))
	{
		//can not open input file
		return MK_E_CANTOPENFILE;
	}
	sz7zFile = szPath;

	FileInStream_CreateVTable(&archiveStream);
	LookToRead_CreateVTable(&lookStream, False);

	lookStream.realStream = &archiveStream.s;
	LookToRead_Init(&lookStream);

	CrcGenerateTable();

	SzArEx_Init(&db);

	WRes res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
	if (res != SZ_OK)
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP C7zDecExtracter::SetOutDirectory(LPCWSTR szPath)
{
	return E_NOTIMPL;
}

STDMETHODIMP C7zDecExtracter::NumFiles(UINT32* pNum)
{
	if (pNum == nullptr)
		return E_INVALIDARG;

	if (!isOpened())
		return E_FAIL;

	*pNum = db.NumFiles;

	return S_OK;
}

STDMETHODIMP C7zDecExtracter::IsDir(UINT32 idx, BOOL* bDir)
{
	if (!isOpened())
		return E_FAIL;

	if (bDir == nullptr || idx >= db.NumFiles)
		return E_INVALIDARG;

	*bDir = SzArEx_IsDir(&db, idx);

	return S_OK;
}

STDMETHODIMP C7zDecExtracter::GetFileName(IN UINT32 idx, OUT LPWSTR pBuf, IN OUT UINT32* pSize)
{
	if (!isOpened())
		return E_FAIL;

	if (pSize == nullptr || idx >= db.NumFiles)
		return E_INVALIDARG;

	size_t len = SzArEx_GetFileNameUtf16(&db, idx, NULL);
	if (len > *pSize)
	{
		*pSize = len;
		return S_FALSE;
	}

	if (pBuf == nullptr)
		return E_INVALIDARG;

	SzArEx_GetFileNameUtf16(&db, idx, (UInt16*)pBuf);

	return S_OK;;
}

HRESULT _7zCreateExtracter(I7zExtracter** extracter)
{
	if (!extracter)
		return E_INVALIDARG;

	auto ptr = new C7zDecExtracter();

	if (ptr == nullptr)
		return E_OUTOFMEMORY;

	ptr->AddRef();
	*extracter = ptr;

	return S_OK;
}

