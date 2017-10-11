#include "stdafx.h"
#include ".\xmlsettings.h"

CXmlSettings::CXmlSettings(void)
{
	IXMLDOMDocumentPtr x;
	
}

CXmlSettings::~CXmlSettings(void)
{
}

void CXmlSettings::CreateInstance(void)
{
	HRESULT hr = m_plDomDocument.CreateInstance(MSXML::CLSID_DOMDocument);
	if (FAILED(hr))
		throw _com_error(hr);
}

void CXmlSettings::GetParameter(CString paramName, CString& paramValue)
{
	_bstr_t bstrParamName;
	bstrParamName = paramName.AllocSysString();
	MSXML::IXMLDOMNodePtr node = m_plDomDocument->selectSingleNode(bstrParamName);

	CString str((char*)node->text);
	paramValue = str;
}

void CXmlSettings::GetParameterList(CString paramName, int& count)
{
	_bstr_t bstrParamName;
	bstrParamName = paramName.AllocSysString();
	
	m_pNodeList = m_plDomDocument->selectNodes(bstrParamName);
	count = m_pNodeList->length;
}

void CXmlSettings::GetParameter(int index, CString& paramName, CString& paramValue)
{
	MSXML::IXMLDOMNodePtr pChild = m_pNodeList->item[index];

	paramName = (char*)pChild->attributes->item[0]->text;
	paramValue = (char*)pChild->text;
}


void CXmlSettings::Load(CString fileName)
{
	variant_t vResult;
	_bstr_t bstrFileName;

	bstrFileName = fileName.AllocSysString();
	vResult = m_plDomDocument->load(bstrFileName);
	
}
