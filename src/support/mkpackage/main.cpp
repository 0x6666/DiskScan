
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

	return 0;
}
