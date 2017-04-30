
#include "stdafx.h"
#include "command_line.h"

struct CLData
{
	LPCWSTR cmd;
	std::vector<LPCWSTR> vecParams;
};

static const std::vector<CLData>& getCmds()
{
	static std::vector<CLData> cmds;
	if (cmds.empty())
	{
		int argCount = 0;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argCount);

		CLData data;
		data.cmd = argv[0];

		for (int i = 1; i < argCount; ++i)
		{
			if (argv[i][0] == L'-')
			{
				cmds.push_back(data);
				data = CLData();
				data.cmd = &argv[i][1];
			}
			else
			{
				data.vecParams.push_back(argv[i]);
			}
		}
		cmds.push_back(data);
		cmds.shrink_to_fit();
	}
	return cmds;
}


CommandLine::CommandLine()
{

}

LPCWSTR CommandLine::getCmd() const
{
	const auto& cmds = getCmds();
	ASSERT(!cmds.empty());
	return cmds[0].cmd;
}

const std::vector<LPCWSTR>* CommandLine::getCmdParams(LPCWSTR cmd)
{
	const auto& cmds = getCmds();
	ASSERT(!cmds.empty());

	for(const CLData& data : cmds)
	{
		if (wcscmp(data.cmd, cmd) == 0)
			return &data.vecParams;
	}

	return nullptr;
}

