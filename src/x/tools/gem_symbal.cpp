
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <vector>
#include <assert.h>
#include <regex>

#ifndef ASSERT
#define  ASSERT assert
#endif //ASSERT

#pragma comment(lib,"Shell32.lib")

struct CLData
{
	LPCWSTR cmd;
	std::vector<LPCWSTR> vecParams;
};

class CommandLine
{
public:
	CommandLine() {}

	LPCWSTR getCmd() const
	{
		const auto& cmds = getCmds();
		ASSERT(!cmds.empty());
		return cmds[0].cmd;
	}

	const std::vector<LPCWSTR>* getCmdParams(LPCWSTR cmd) const
	{
		const auto& cmds = getCmds();
		ASSERT(!cmds.empty());

		for (const CLData& data : cmds)
		{
			if (wcscmp(data.cmd, cmd) == 0)
				return &data.vecParams;
		}

		return nullptr;
	}

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
};

class ChildProcess
{
	enum { BUFSIZE = 4096 };
public:
	ChildProcess(const std::wstring& cmd)
		: m_hChildStd_OUT_Rd(NULL)
		, m_hChildStd_OUT_Wr(NULL)
		, g_hChildStd_IN_Rd(NULL)
		, g_hChildStd_IN_Wr(NULL)
		, m_cmd(cmd)
	{
	}

	~ChildProcess()
	{
		CloseHandle(g_hChildStd_IN_Rd);
		CloseHandle(g_hChildStd_IN_Wr);
		CloseHandle(m_hChildStd_OUT_Rd);
		CloseHandle(m_hChildStd_OUT_Wr);
	}

	bool Run()
	{
		SECURITY_ATTRIBUTES saAttr;

		// Set the bInheritHandle flag so pipe handles are inherited. 
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		// Create a pipe for the child process's STDOUT. 
		if (!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0))
			return false;

		// Create a pipe for the child process's STDIN. 
		if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
			return false;

		// Create the child process. 
		HANDLE hProc = NULL;
		if (CreateChildProcess(&hProc) == false)
			return false;

		// Read from pipe that is the standard output for child process. 
		ReadFromPipe(hProc);

		return true;
	}

	const std::string& GetRes() {
		return m_res;
	}
private:

	// Create a child process that uses the previously created pipes for STDIN and STDOUT.
	bool CreateChildProcess(HANDLE* pProcess)
	{
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFOW siStartInfo;
		BOOL bSuccess = FALSE;

		// Set up members of the PROCESS_INFORMATION structure. 
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

		// Set up members of the STARTUPINFO structure. 
		// This structure specifies the STDIN and STDOUT handles for redirection.
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFOW));
		siStartInfo.cb = sizeof(STARTUPINFOW);
		siStartInfo.hStdError = m_hChildStd_OUT_Wr;
		siStartInfo.hStdOutput = m_hChildStd_OUT_Wr;
		siStartInfo.hStdInput = g_hChildStd_IN_Rd;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

		std::vector<WCHAR> szCmdLine(m_cmd.length() + 1, 0);
		wcscpy(szCmdLine.data(), m_cmd.c_str());

		// Create the child process. 
		bSuccess = CreateProcessW(NULL,
			szCmdLine.data(),     // command line 
			NULL,          // process security attributes 
			NULL,          // primary thread security attributes 
			TRUE,          // handles are inherited 
			0,             // creation flags 
			NULL,          // use parent's environment 
			NULL,          // use parent's current directory 
			&siStartInfo,  // STARTUPINFO pointer 
			&piProcInfo);  // receives PROCESS_INFORMATION 

		// If an error occurs, exit the application. 
		if (!bSuccess)
		{
			printf("create child proc failed %S\n", szCmdLine.data());
			return false;
		}
		else
		{
			// Close handles to the child process and its primary thread.
			// Some applications might keep these handles to monitor the status
			// of the child process, for example. 
			CloseHandle(piProcInfo.hThread);
			if (pProcess)
				*pProcess = piProcInfo.hProcess;
			else
				CloseHandle(piProcInfo.hProcess);
		}

		return true;
	}

	// Read output from the child process's pipe for STDOUT
	// and write to the parent process's pipe for STDOUT. 
	// Stop when there is no more data. 
	void ReadFromPipe(HANDLE hProc)
	{
		DWORD dwRead;
		CHAR chBuf[BUFSIZE + 1] = { 0 };
		BOOL bSuccess = FALSE;

		DWORD dwExitCode = 0;
		while (GetExitCodeProcess(hProc, &dwExitCode))
		{
			for (; GetFileSize(m_hChildStd_OUT_Rd, 0) > 0;)
			{
				bSuccess = ReadFile(m_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
				if (!bSuccess || dwRead == 0) break;

				chBuf[dwRead] = 0;
				m_res += chBuf;
				//printf(chBuf);
			}

			//如果子进程结束，退出循环
			if (dwExitCode != STILL_ACTIVE)
				break;
		}
		CloseHandle(hProc);
	}

private:
	HANDLE g_hChildStd_IN_Rd;
	HANDLE g_hChildStd_IN_Wr;
	HANDLE m_hChildStd_OUT_Rd;
	HANDLE m_hChildStd_OUT_Wr;
	std::wstring m_cmd;
	std::string m_res;
};


#define DEF_PARAM(arg, name)				\
std::wstring arg;	{						\
CommandLine cmdline;						\
const std::vector<LPCWSTR>* param = cmdline.getCmdParams(L##name);	\
if (param == nullptr)						\
{											\
	printf("no " name " \n");				\
	return 1;								\
}											\
else										\
{											\
	arg = (*param)[0];						\
}}

template<class func>
void eachline(const std::string &s, const std::string &seperator, func _func)
{
	typedef std::string::size_type string_size;
	string_size i = 0;

	while (i != s.size())
	{
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0)
		{
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
			{
				if (s[i] == seperator[x])
				{
					++i;
					flag = 0;
					break;
				}
			}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0)
		{
			for (string_size x = 0; x < seperator.size(); ++x)
			{
				if (s[j] == seperator[x])
				{
					flag = 1;
					break;
				}
			}
			if (flag == 0)
				++j;
		}

		if (i != j)
		{
			_func(s.substr(i, j - i));
			//result.push_back();
			i = j;
		}
	}
}


int main(int argc, char** argv)
{
	DEF_PARAM(libPath, "lib");
	DEF_PARAM(dumpbinPath, "dumpbin");
	DEF_PARAM(outpath, "out");

	std::wstring cmd;
	cmd += L"\"";
	cmd += dumpbinPath;
	cmd += L"\" ";
	cmd += L"/linkermember:1 ";
	cmd += libPath;

	ChildProcess proc(cmd);
	if (!proc.Run())
	{
		printf("can't get symbal \n");
		return 1;
	}

	//TODO  check timestamp

	std::wstring symbalpath = outpath;//libPath.substr(0, libPath.length() - 4) + L".c";
	FILE* pFile = _wfopen(symbalpath.c_str(), L"w+");
	if (pFile == nullptr)
	{
		printf("can't open '%S' \n", symbalpath.c_str());
		return 1;
	}

	fprintf(pFile, "// This file is created automatically, please do not modify it!\n\n");

	std::regex pattern("\\s+.*\\s([\?_]+.*)");
	eachline(proc.GetRes(), "\r\n", [&](const std::string& line) {
		std::smatch sm;
		if (std::regex_match(line, sm, pattern) && sm.size() == 2)
		{
			fprintf(pFile, "#pragma comment(linker, \"/include:%s\")\n", sm[1].str().c_str());
		}
	});

	fclose(pFile);

	return 0;
}
