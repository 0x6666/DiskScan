
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

	spCmps->AddFile(L"E:/Code/C++/diskscan/output/DiskScan/bin/diskscan.exe", L"bin\\");
	spCmps->AddFile(L"E:/Code/C++/diskscan/output/DiskScan/bin/disktool.dll", L"bin\\");
	spCmps->AddFile(L"E:/Code/C++/diskscan/output/DiskScan/bin/strtool.dll", L"bin\\");

	spCmps->Compress(L"E:/Code/C++/diskscan/output/DiskScan/pkg/bin.7z");

	return 0;
}
