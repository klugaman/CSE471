#include "StdAfx.h"
#include "NoiseGate.h"


CNoiseGate::CNoiseGate(void)
{
	m_threshold = 0;
	m_dry = 0;
	m_wet = 0;
	m_gateL = 1;
	m_gateR = 1;
}


CNoiseGate::~CNoiseGate(void)
{
}

void CNoiseGate::Process(double * frameIn, double * frameOut)
{
	if(frameIn[0] < m_threshold && frameIn[0] > -m_threshold)
	{
		m_gateL -= .005;
		if(m_gateL < 0)
			m_gateL = 0;
	}
	else
	{
		m_gateL += .005;
		if(m_gateL > 1)
			m_gateL = 1;
	}
	frameOut[0] = m_dry * frameIn[0] + (m_wet * frameIn[0] * m_gateL);

	if(frameIn[1] < m_threshold && frameIn[1] > -m_threshold)
	{
		m_gateR -= .005;
		if(m_gateR < 0)
			m_gateR = 0;
	}
	else
	{
		m_gateR += .005;
		if(m_gateR > 1)
			m_gateR = 1;
	}
	frameOut[1] = m_dry * frameIn[1] + (m_wet * frameIn[1] * m_gateR);
}

void CNoiseGate::XmlLoad(IXMLDOMNode * xml)
{
	 // Get a list of all attribute nodes and the
    // length of that list
    CComPtr<IXMLDOMNamedNodeMap> attributes;
    xml->get_attributes(&attributes);
    long len;
    attributes->get_length(&len);

    // Loop over the list of attributes
    for(int i=0;  i<len;  i++)
    {
        // Get attribute i
        CComPtr<IXMLDOMNode> attrib;
        attributes->get_item(i, &attrib);

        // Get the name of the attribute
        CComBSTR name;
        attrib->get_nodeName(&name);

        // Get the value of the attribute.  
        CComVariant value;
        attrib->get_nodeValue(&value);

        if(name == "threshold")
        {
            value.ChangeType(VT_I4);
            m_threshold = value.intVal;
        }
        else if(name == "wet")
        {
            value.ChangeType(VT_R8);
            m_wet = value.dblVal;
        }
		else if(name == "dry")
		{
			value.ChangeType(VT_R8);
			m_dry = value.dblVal;
		}
    }
}
