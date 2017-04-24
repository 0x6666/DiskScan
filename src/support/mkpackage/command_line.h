
#ifndef __MK_PACKAGE_H__
#define __MK_PACKAGE_H__

class CommandLine
{
public:
	CommandLine();

	LPCWSTR getCmd() const;
	const std::vector<LPCWSTR>* getCmdParams(LPCWSTR cmd);
};

#endif //__MK_PACKAGE_H__
