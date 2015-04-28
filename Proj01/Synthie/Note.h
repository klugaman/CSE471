#pragma once
#include <string>
#include "xmlhelp.h"

class CNote
{
public:
	CNote(void);
	virtual ~CNote(void);

	int Measure() const {return m_measure;}
    double Beat() const {return m_beat;}
    const std::wstring &Instrument() const {return m_instrument;}  
    IXMLDOMNode *Node() {return m_node;}
	void XmlLoad(IXMLDOMNode * xml, std::wstring & instrument);
	bool operator<(const CNote &b);

	bool hasEffect(int effect) {return m_effects[effect];}
	void setEffect(int effect) {m_effects[effect] = true;}

private:
    std::wstring m_instrument;
    int m_measure;
    double m_beat;
    CComPtr<IXMLDOMNode> m_node;

	bool m_effects[4];
};

