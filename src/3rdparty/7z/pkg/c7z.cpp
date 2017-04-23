
#include "StdAfx.h"

#include "c7z.h"


C7zCompresser::C7zCompresser()
{

}

C7zCompresser::~C7zCompresser()
{

}

STDMETHODIMP C7zCompresser::AddFile(LPCWSTR szPath, LPCWSTR szInnerPath)
{
	return E_NOTIMPL;
}

STDMETHODIMP C7zCompresser::Folder(LPCWSTR szPath)
{
	return E_NOTIMPL;
}

STDMETHODIMP C7zCompresser::Compress(LPCWSTR szOutPath)
{
	return E_NOTIMPL;
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
