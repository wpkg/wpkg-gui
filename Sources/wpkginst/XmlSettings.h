#pragma once

#import <msxml.dll> named_guids

class CXmlSettings
{
public:
	CXmlSettings(void);
	virtual ~CXmlSettings(void);
private:
	MSXML::IXMLDOMDocumentPtr m_plDomDocument;
	MSXML::IXMLDOMNodeListPtr m_pNodeList;
public:
	void CreateInstance(void);
	void GetParameter(CString paramName, CString& paramValue);
	void Load(CString fileName);
	void GetParameterList(CString paramName, int& count);
	void GetParameter(int index, CString& paramName, CString& paramValue);
};
