#pragma once

class CRunProcess
{
public:
	CRunProcess(void);
	virtual ~CRunProcess(void);
	static void CreateProcess(HANDLE hToken, char* commandLine);
};
