#pragma once

#import <msxml.dll> named_guids

class CXmlSettings
{
public:
	static CString Crypt(CString str);
	static  CString Decrypt(CString str);

	CXmlSettings(void);
	virtual ~CXmlSettings(void);
private:
	MSXML::IXMLDOMDocumentPtr m_plDomDocument;
	MSXML::IXMLDOMNodeListPtr m_pNodeList;
	MSXML::IXMLDOMNodePtr m_pConfigurationNode;
public:
	void CreateInstance(void);
	void GetParameter(CString paramName, CString& paramValue);
	void Load(CString fileName);
	void GetParameterList(CString paramName, int& count);
	void GetParameter(int index, CString& paramName, CString& paramValue);

	void Save(CString fileName);
	void Init();
	void WriteParameter(CString paramName, CString paramValue);
	void WriteParameterEx(CString paramName, CString paramValue);
	
	
};
