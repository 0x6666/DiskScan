
#include "StdAfx.h"
#include "c7z.h"

#include <CPP/7zip/Common/FileStreams.h>
#include <CPP/7zip/Archive/7z/7zHandler.h>
#include <CPP/7zip/Archive/IArchive.h>
#include <CPP/Windows/PropVariant.h>
#include <CPP/Windows/FileFind.h>
#include <CPP/Common/IntToString.h>
#include <CPP/Common/MyCom.h>

#define IS_SPT(x) ((x) == L'/' || (x) == L'\\')

STDAPI CreateObject(const GUID *clsid, const GUID *iid, void **outObject);

DEFINE_GUID(CLSID_CFormat7z,
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);

C7zCompresser::C7zCompresser()
{

}

C7zCompresser::~C7zCompresser()
{

}

STDMETHODIMP C7zCompresser::AddFile(LPCWSTR szLocalPath, LPCWSTR szInnerPath)
{
	FString localPath = us2fs(szLocalPath);

	NWindows::NFile::NFind::CFileInfo fi;
	if (!fi.Find(localPath) || fi.IsDir())
		return E_FAIL;

	UString fileName = szLocalPath;
	int len = wcslen(szLocalPath);
	int i = len - 1;
	for (; i >= 0 && !IS_SPT(szLocalPath[i]); --i);
	if (i >= 0)
	{
		UString tmp = szLocalPath;
		fileName = tmp.RightPtr(len - i - 1);
	}

	UString innerPath;
	if (szInnerPath == nullptr)
	{
		innerPath = fileName;
	}
	else
	{
		innerPath = szInnerPath;
		int len = wcslen(szInnerPath);
		if (IS_SPT(szInnerPath[len - 1]))
			innerPath += fileName;
	}

	CDirItem di;
	di.Attrib = fi.Attrib;
	di.Size = fi.Size;
	di.CTime = fi.CTime;
	di.ATime = fi.ATime;
	di.MTime = fi.MTime;
	di.Name = innerPath;
	di.LocalPath = localPath;
	m_files.Add(di);
	return S_OK;
}

STDMETHODIMP C7zCompresser::Folder(LPCWSTR szPath)
{
	return E_NOTIMPL;
}

STDMETHODIMP C7zCompresser::SetTotal(UInt64 total)
{
	return S_OK;
}

STDMETHODIMP C7zCompresser::SetCompleted(const UInt64 *completeValue)
{
	return S_OK;
}

STDMETHODIMP C7zCompresser::Compress(LPCWSTR szOutPath)
{
	CMyComPtr<COutFileStream> outFileStream(new COutFileStream);
	if (!outFileStream->Create(us2fs(szOutPath), true))
		return E_FAIL;

	CMyComPtr<IOutArchive> outArchive = new NArchive::N7z::CHandler;
	//CMyComPtr<IOutArchive> outArchive;
	//if (CreateObject(&CLSID_CFormat7z, &IID_IOutArchive, (void **)&outArchive) != S_OK)
	//	return E_FAIL;

	return outArchive->UpdateItems(outFileStream, m_files.Size(), this);
}

STDMETHODIMP C7zCompresser::GetUpdateItemInfo(UInt32 /* index */,
	Int32 *newData, Int32 *newProperties, UInt32 *indexInArchive)
{
	if (newData)
		*newData = BoolToInt(true);
	if (newProperties)
		*newProperties = BoolToInt(true);
	if (indexInArchive)
		*indexInArchive = (UInt32)(Int32)-1;
	return S_OK;
}

STDMETHODIMP C7zCompresser::GetProperty(UInt32 index, PROPID propID, PROPVARIANT *value)
{
	NWindows::NCOM::CPropVariant prop;

	if (propID == kpidIsAnti)
	{
		prop = false;
		prop.Detach(value);
		return S_OK;
	}

	{
		const CDirItem &dirItem = m_files[index];
		switch (propID)
		{
		case kpidPath:  prop = dirItem.Name; break;
		case kpidIsDir:  prop = dirItem.isDir(); break;
		case kpidSize:  prop = dirItem.Size; break;
		case kpidAttrib:  prop = dirItem.Attrib; break;
		case kpidCTime:  prop = dirItem.CTime; break;
		case kpidATime:  prop = dirItem.ATime; break;
		case kpidMTime:  prop = dirItem.MTime; break;
		}
	}
	prop.Detach(value);
	return S_OK;
}


STDMETHODIMP C7zCompresser::GetStream(UInt32 index, ISequentialInStream **inStream)
{
	const CDirItem &dirItem = m_files[index];

	if (dirItem.isDir())
		return S_OK;

	{
		CMyComPtr<CInFileStream> inStreamLoc(new CInFileStream);
		FString path = dirItem.LocalPath;
		if (!inStreamLoc->Open(path))
		{
			DWORD sysError = ::GetLastError();
			return E_FAIL;
		}

		*inStream = inStreamLoc.Detach();
	}
	return S_OK;
}

STDMETHODIMP C7zCompresser::SetOperationResult(Int32 operationResult)
{
	return S_OK;
}

HRESULT _Create7zCompresser(I7zCompresser** compresser)
{
	if (!compresser)
		return E_INVALIDARG;

	auto ptr = new C7zCompresser();

	if (ptr == nullptr)
		return E_OUTOFMEMORY;

	ptr->AddRef();
	*compresser = ptr;

	return S_OK;
}
