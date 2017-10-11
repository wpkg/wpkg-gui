#include "stdafx.h"
#include ".\xmlsettings.h"
#include "atlenc.h"

#define KEY_LENGTH 40

static BYTE key[] = {
    0x50,
    0xF7,
    0x82,
    0x69,
    0xEA,
    0x2D,
    0xDD,
    0x2D,
    0x6A,
    0xB4,
    0x33,
    0x8F,
    0xD5,
    0xC7,
    0x90,
    0x9C,
    0x22,
    0x95,
    0x61,
    0xE5,
    0x65,
    0xF6,
    0xB0,
    0x4B,
    0x94,
    0x47,
    0xB0,
    0xBD,
    0x73,
    0x58,
    0x56,
    0x87,
    0x79,
    0x7B,
    0xE6,
    0xB0,
    0xD2,
    0x20,
    0x28,
	0xE1
};

CString CXmlSettings::Crypt(CString str)
{
	char* buffer;
	buffer = str.GetBuffer(1024);
	int length = str.GetLength();
	char destination[1024];
	int destinationLength = 1024;

	

	int idx = 0;

	for (int i = 0; i < length; i++)
    {
        buffer[i] ^= key[idx];
        idx++;
        if (idx > KEY_LENGTH - 1)
            idx = 0;
    }
 
	
	Base64Encode((BYTE*)buffer,length,destination,&destinationLength);

	if(destinationLength<1024)
		destination[destinationLength] = 0;

 	CString strResult(destination);

	return  strResult;
}

CString CXmlSettings::Decrypt(CString str)
{
	char* buffer;
	buffer = str.GetBuffer(1024);
	char destination[1024];
	int destinationLength = 1024;
	int length = str.GetLength();

	Base64Decode(buffer,length,(BYTE*)destination,&destinationLength);

	int idx = 0;

	for (int i = 0; i < destinationLength; i++)
    {
        destination[i] ^= key[idx];
        idx++;
        if (idx > KEY_LENGTH - 1)
            idx = 0;
    }

	if(destinationLength<1024)
		destination[destinationLength] = 0;

 	CString strResult(destination);

	return  strResult;
}



           
CXmlSettings::CXmlSettings(void)
{
	
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

void CXmlSettings::Save(CString fileName)
{
	
	_bstr_t bstrFileName = fileName.AllocSysString();
	m_plDomDocument->save(bstrFileName);
}

void CXmlSettings::Init()
{
	_bstr_t bstrConfiguration=(_bstr_t)(LPCTSTR)"configuration";
	MSXML::IXMLDOMProcessingInstructionPtr pi = m_plDomDocument->createProcessingInstruction("xml", "version='1.0' encoding='utf-8'"); 
	m_plDomDocument->appendChild(pi); 
	MSXML::IXMLDOMElementPtr pElement = m_plDomDocument->createElement(bstrConfiguration);
	m_pConfigurationNode = m_plDomDocument->appendChild(pElement);
}

void CXmlSettings::WriteParameter(CString paramName, CString paramValue)
{
	MSXML::IXMLDOMNodePtr pNode;
	MSXML::IXMLDOMElementPtr pElement;

	_bstr_t bstrParamName=(_bstr_t)(LPCTSTR)paramName;
	pElement = m_plDomDocument->createElement(bstrParamName);
	pNode = m_pConfigurationNode->appendChild(pElement);

	_bstr_t bstrParamValue=(_bstr_t)(LPCTSTR)paramValue;
	MSXML::IXMLDOMTextPtr pText = m_plDomDocument->createTextNode(bstrParamValue);
	pNode->appendChild(pText);

}

void CXmlSettings::WriteParameterEx(CString paramName, CString paramValue)
{
	MSXML::IXMLDOMNodePtr pNode;
	MSXML::IXMLDOMElementPtr pElement;
	MSXML::IXMLDOMAttributePtr pNewAttr;
	MSXML::IXMLDOMTextPtr pText;

	_bstr_t bstrNode = (_bstr_t)(LPCTSTR)"script-variable";
	pElement = m_plDomDocument->createElement(bstrNode);
	pNode = m_pConfigurationNode->appendChild(pElement);

	_bstr_t bstrAttrName = (_bstr_t)(LPCTSTR)"name";

	pNewAttr = m_plDomDocument->createAttribute(bstrAttrName);
	_bstr_t bstrAttrValue = (LPCTSTR)paramName;
	pNewAttr->PutnodeValue((_variant_t)bstrAttrValue);
	
	pNode->attributes->setNamedItem(pNewAttr);
	

	_bstr_t bstrParamValue=(_bstr_t)(LPCTSTR)paramValue;
	pText = m_plDomDocument->createTextNode(bstrParamValue);
	pNode->appendChild(pText);
	

}

