
#include "stdafx.h"
#include <stdio.h>
#include "command_line.h"

#include <Format7zR/7z.h>

int main(int argc, char** argv)
{
	CommandLine arg;

	auto cmd = arg.getCmd();
	const std::vector<LPCWSTR>* pkg = arg.getCmdParams(L"pkg");
	if (pkg && !pkg->empty())
	{
		auto path = (*pkg)[0];
		path = path;
	}

	CMyComPtr<I7zCompresser> spCmps;
	HRESULT hr = _Create7zCompresser(&spCmps);
	if (FAILED(hr))
		return 0;

	spCmps->AddFile(L"D:\\diskscan\\output\\DiskScand\\bin\\diskscan.exe", L"bin\\");
	spCmps->AddFile(L"D:\\diskscan\\output\\DiskScand\\bin\\disktool.dll", L"bin\\");
	spCmps->AddFile(L"D:\\diskscan\\output\\DiskScand\\bin\\strtool.dll", L"bin\\");

	spCmps->Compress(L"D:\\diskscan\\output\\DiskScand\\pkg\\bin.7z");

	return 0;
}
