
#include "stdafx.h"
#include "7zdec.h"

#include "../../../CPP/Common/MyBuffer.h"

#define IS_PATH_SEPARATOR(x) ((x) == L'/' || (x) == L'\\')

BOOL _CreatePath(const std::wstring& str)
{
	LPWSTR name = (LPWSTR)str.c_str();
	LPCWSTR destPath = (LPCWSTR)name;

	for (int j = 0; name[j] != 0; ++j)
	{
		if (IS_PATH_SEPARATOR(name[j]))
		{
			name[j] = 0;
			if (_waccess(name, 0))
			{
				if (!CreateDirectoryW((LPCWSTR)name, NULL))
					return FALSE;
			}
			name[j] = L'/';
		}
	}

	if (_waccess(name, 0))
	{
		if (!CreateDirectoryW((LPCWSTR)name, NULL))
			return FALSE;
	}

	return TRUE;
}

class CBufInStream
{
	CByteBuffer Buf;
	UInt64 _pos;
public:
	CBufInStream(const Byte *data, size_t size)
		: _pos(0)
	{
		Buf.CopyFrom(data, size);
	}

	~CBufInStream()
	{
	}

	HRESULT Read(void *data, UInt32 size, UInt32 *processedSize)
	{
		if (processedSize)
			*processedSize = 0;
		if (size == 0)
			return S_OK;
		if (_pos >= Buf.Size())
			return S_OK;
		size_t rem = Buf.Size() - (size_t)_pos;
		if (rem > size)
			rem = (size_t)size;
		memcpy(data, (const Byte*)Buf + (size_t)_pos, rem);
		_pos += rem;
		if (processedSize)
			*processedSize = (UInt32)rem;
		return S_OK;
	}
	HRESULT Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
	{
		switch (seekOrigin)
		{
		case STREAM_SEEK_SET: break;
		case STREAM_SEEK_CUR: offset += _pos; break;
		case STREAM_SEEK_END: offset += Buf.Size(); break;
		default: return STG_E_INVALIDFUNCTION;
		}
		if (offset < 0)
			return HRESULT_WIN32_ERROR_NEGATIVE_SEEK;
		_pos = offset;
		if (newPosition)
			*newPosition = offset;
		return S_OK;
	}
};


typedef struct
{
	ISeekInStream s;
	CBufInStream* pBufStream;
} MemStream;

static SRes MemStrem_Read(void* pp, void* buf, size_t* size)
{
	MemStream *p = (MemStream*)pp;
	UInt32 processedSize = 0;
	if (SUCCEEDED(p->pBufStream->Read(buf, *size, &processedSize)))
	{
		*size = processedSize;
		return SZ_OK;
	}
	return SZ_ERROR_FAIL;
}

static SRes MemStrem_Seek(void *pp, Int64 *pos, ESzSeek origin)
{
	MemStream *p = (MemStream*)pp;

	Int64 offset = *pos;
	UInt64 newPos = 0;
	if (SUCCEEDED(p->pBufStream->Seek(offset, origin, &newPos)))
	{
		*pos = newPos;
		return SZ_OK;
	}

	return SZ_ERROR_FAIL;
}

class C7zDecExtracter
	: public I7zExtracter
	, private CMyUnknownImp
{
public:
	C7zDecExtracter();
	~C7zDecExtracter();

	MY_UNKNOWN_IMP1(I7zExtracter)

	STDMETHOD(Open7zFile)(LPCWSTR szPath) override;
	STDMETHOD(OpenBuf)(IN void* pBuf, IN UINT32 size) override;
	STDMETHOD(ExtractTo)(IN LPCWSTR szPath) override;
	STDMETHOD(NumFiles)(UINT32*) override;
	STDMETHOD(IsDir)(UINT32, BOOL*) override;
	STDMETHOD(GetFileName)(IN UINT32, OUT LPWSTR pBuf, IN OUT UINT32* pSize) override;

private:
	bool isOpened() const { return !sz7zFile.empty() || memStream.pBufStream != nullptr; };

private:
	CFileInStream archiveStream;

	MemStream memStream;

	CLookToRead lookStream;
	CSzArEx db;
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	std::wstring sz7zFile;
};


C7zDecExtracter::C7zDecExtracter()
{
	memStream.pBufStream = nullptr;

	allocImp.Alloc = SzAlloc;
	allocImp.Free = SzFree;

	allocTempImp.Alloc = SzAllocTemp;
	allocTempImp.Free = SzFreeTemp;

	CrcGenerateTable();
	LookToRead_CreateVTable(&lookStream, False);
}

C7zDecExtracter::~C7zDecExtracter()
{
	if (isOpened())
	{
		if (memStream.pBufStream != nullptr)
		{
			delete memStream.pBufStream;
		}
		else
		{
			SzArEx_Free(&db, &allocImp);
			File_Close(&archiveStream.file);
		}
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

	lookStream.realStream = &archiveStream.s;
	LookToRead_Init(&lookStream);

	SzArEx_Init(&db);

	WRes res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
	if (res != SZ_OK)
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP C7zDecExtracter::OpenBuf(IN void* pBuf, IN UINT32 size)
{
	if (pBuf == nullptr || size == 0)
		return E_INVALIDARG;

	if (isOpened())
		return E_FAIL;

	memStream.pBufStream = new CBufInStream((const Byte *)pBuf, size);
	memStream.s.Read = MemStrem_Read;
	memStream.s.Seek = MemStrem_Seek;

	lookStream.realStream = &memStream.s;
	LookToRead_Init(&lookStream);

	SzArEx_Init(&db);

	WRes res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
	if (res != SZ_OK)
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP C7zDecExtracter::ExtractTo(LPCWSTR szPath)
{
	if (szPath == nullptr || szPath[0] == 0)
		return E_INVALIDARG;

	if (!isOpened())
		return E_FAIL;

	std::wstring path(szPath);
	if (!IS_PATH_SEPARATOR(path[path.length() - 1]))
		path += L'/';

	UInt32 blockIndex = 0xFFFFFFFF;/* it can have any value before first call (if outBuffer = 0) */
	Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
	size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */
	SRes res = S_OK;

	for (UInt32 i = 0; i < db.NumFiles; ++i)
	{
		size_t offset = 0;
		size_t outSizeProcessed = 0;

		WCHAR pathBuf[MAX_PATH] = { 0 };
		SzArEx_GetFileNameUtf16(&db, i, (UInt16*)pathBuf);

		std::wstring fullPath = path + pathBuf;

		if (SzArEx_IsDir(&db, i))
		{
			if (!_CreatePath(path + pathBuf))
			{
				res = SZ_ERROR_FAIL;
				break;
			}
			continue;
		}
		else
		{
			LPWSTR _name = (LPWSTR)fullPath.c_str();
			for (int i = fullPath.length() - 1; i > 0; --i)
			{
				if (IS_PATH_SEPARATOR(_name[i]))
				{
					_name[i] = 0;
					if (!_CreatePath(_name))
						res = SZ_ERROR_FAIL;
					_name[i] = L'/';
					break;
				}
			}
		}

		res = SzArEx_Extract(&db, &lookStream.s, i,
			&blockIndex, &outBuffer, &outBufferSize,
			&offset, &outSizeProcessed,
			&allocImp, &allocTempImp);

		if (res != SZ_OK)
			break;

		CSzFile outFile;
		size_t processedSize;

		if (OutFile_OpenW(&outFile, fullPath.c_str()))
		{
			res = SZ_ERROR_FAIL;
			break;
		}

		processedSize = outSizeProcessed;

		if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
		{
			res = SZ_ERROR_FAIL;
			break;
		}

		if (SzBitWithVals_Check(&db.MTime, i))
		{
			const CNtfsFileTime *t = db.MTime.Vals + i;
			FILETIME mTime;
			mTime.dwLowDateTime = t->Low;
			mTime.dwHighDateTime = t->High;
			SetFileTime(outFile.handle, NULL, NULL, &mTime);
		}

		if (File_Close(&outFile))
		{
			res = SZ_ERROR_FAIL;
			break;
		}

		if (SzBitWithVals_Check(&db.Attribs, i))
			SetFileAttributesW(fullPath.c_str(), db.Attribs.Vals[i]);
	}

	IAlloc_Free(&allocImp, outBuffer);

	return res == SZ_OK ? S_OK : E_FAIL;
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

	return S_OK;
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

