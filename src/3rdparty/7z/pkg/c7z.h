
#ifndef __7Z_C7Z_H__
#define __7Z_C7Z_H__

#include "../7z.h"
#include <CPP/7zip/Archive/IArchive.h>
#include <vector>
#include <CPP/Common/MyString.h>

struct CDirItem
{
	UInt64 Size;
	FILETIME CTime;
	FILETIME ATime;
	FILETIME MTime;
	UString Name;
	FString LocalPath;
	UInt32 Attrib;

	bool isDir() const { return (Attrib & FILE_ATTRIBUTE_DIRECTORY) != 0; }
};

class C7zCompresser
	: public I7zCompresser
	, public IArchiveUpdateCallback
	, private CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(I7zCompresser)

	C7zCompresser();
	~C7zCompresser();


	INTERFACE_IArchiveUpdateCallback(override)

	STDMETHOD(AddFile) (LPCWSTR szPath, LPCWSTR szInnerPath) override;
	STDMETHOD(Folder) (LPCWSTR szPath) override;
	STDMETHOD(Compress) (LPCWSTR szOutPath) override;

private:
	CObjectVector<CDirItem> m_files;
};


#endif __7Z_C7Z_H__
