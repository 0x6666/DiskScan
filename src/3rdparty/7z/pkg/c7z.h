
#ifndef __7Z_C7Z_H__
#define __7Z_C7Z_H__

#include "../7z.h"


class C7zCompresser
	: public I7zCompresser
	, private CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(I7zCompresser)

	C7zCompresser();
	~C7zCompresser();

	STDMETHOD(AddFile) (LPCWSTR szPath, LPCWSTR szInnerPath) override;
	STDMETHOD(Folder) (LPCWSTR szPath) override;
	STDMETHOD(Compress) (LPCWSTR szOutPath) override;
};


#endif __7Z_C7Z_H__
