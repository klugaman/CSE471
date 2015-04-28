#pragma once
class CCompression
{
public:
	CCompression(void);
	~CCompression(void);

	void Process(double *frameIn, double *frameOut);
	void XmlLoad(IXMLDOMNode *xml);

private:
	int m_threshold;
	double m_wet;
	double m_dry;
	double m_gateL;
	double m_gateR;

	// Ratio at which we are compressing the signal x:1
	double m_ratio;
};

