#pragma once

class CParameters
{
private:
	CString m_strCommandLine;
	CString m_strParDelimiter;
	CString m_strValDelimiter;
	CStringArray m_array;
protected:
	virtual CString GetValue(CString& line);
	virtual CString GetName(CString& line);

public:
	CParameters(void);
	virtual ~CParameters(void);
	void SetCommandLine(CString command);
	void Compute(void);
	CString GetParameter(CString paramName);
	
};
