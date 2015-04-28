#pragma once
class CNoiseGate
{
public:
	CNoiseGate(void);
	~CNoiseGate(void);
	void Process(double * frameIn, double * frameOut);

	void XmlLoad(IXMLDOMNode * xml);

private:
	// Gate threshold
	int m_threshold;

	double m_dry;
	double m_wet;

	double m_gateL;
	double m_gateR;
};

