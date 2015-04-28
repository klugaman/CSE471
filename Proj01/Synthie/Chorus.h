#pragma once

#include <vector>

class CChorus
{
public:
	CChorus(void);
	~CChorus(void);

	void setSampleRate(double s) {m_sampleRate = s;}

	void Process(double *frameIn, double *frameOut);
	void XmlLoad(IXMLDOMNode * xml);

private:
	double m_delay;
	double m_dry;
	double m_wet;
	double m_modulationRange;
	double m_modulationRate;
	int m_wrloc;
	double m_sampleRate;

	std::vector<double> m_queueL;
	std::vector<double> m_queueR;
};

