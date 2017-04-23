
#include "stdafx.h"
#include "stdio.h"
#include <vector>

#include <7zdec/pkg/7zdec.h>

int main(int argc, char** argv)
{
	CMyComPtr<I7zExtracter> spExtracter;
	HRESULT hr = _7zCreateExtracter(&spExtracter);
	if (FAILED(hr))
		return 0;

	if (false)
	{
		hr = spExtracter->Open7zFile(L"f:/cmake.7z");
		if (FAILED(hr))
			return 0;
	}
	else
	{
		FILE* file = _wfopen(L"f:/cmake.7z", L"rb");
		fseek(file, 0, SEEK_END);
		fpos_t pos;
		fgetpos(file, &pos);
		fseek(file, 0, SEEK_SET);

		std::vector<char> vecBuf(pos, 0);
		size_t s = fread(vecBuf.data(), 1, pos, file);

		hr = spExtracter->OpenBuf(vecBuf.data(), pos);
		if (FAILED(hr))
			return 0;
	}

	hr = spExtracter->ExtractTo(L"f:/out");

	return 0;
}