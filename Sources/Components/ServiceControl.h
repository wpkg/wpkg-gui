#pragma once


class CServiceControl
{
public:
	CServiceControl(void);
	virtual ~CServiceControl(void);
	static CString ServiceControl(CString strServerName, BOOL bStop, CEdit* pEditProgressMessage);
	static void SetPreShutdownTimeout(DWORD dwTimeout);
};
