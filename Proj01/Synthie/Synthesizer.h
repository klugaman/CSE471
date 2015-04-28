#pragma once
#include <list>
#include "Instrument.h"
#include "msxml2.h"
#include <vector>
#include "Note.h"
#include "Chorus.h"
#include "Flange.h"
#include "NoiseGate.h"
#include "Compression.h"
#include "PianoInstrumentFactory.h"

using namespace std;

class CSynthesizer
{
public:
	CSynthesizer(void);
	virtual ~CSynthesizer(void);
public:
	//! Number of audio channels
    int GetNumChannels() {return m_channels;}

	//! Sample rate in samples per second
    double GetSampleRate() {return m_sampleRate;}

	//! Sample period in seconds (1/samplerate)
    double GetSamplePeriod() {return m_samplePeriod;}
    
	//! Set the number of channels
    void SetNumChannels(int n) {m_channels = n;}

	//! Set the sample rate
    void SetSampleRate(double s) {m_sampleRate = s;  m_samplePeriod = 1.0 / s;}

	void OpenScore(CString & filename);

private:
    int		m_channels;
    double	m_sampleRate;
    double	m_samplePeriod;
	double m_time;
	double  m_bpm;                  //!< Beats per minute
    int     m_beatspermeasure;  //!< Beats per measure
    double  m_secperbeat;        //!< Seconds per beat
	int m_currentNote;          //!< The current note we are playing
    int m_measure;              //!< The current measure
    double m_beat;              //!< The current beat within the measure
public:
	void Start(void);
	bool Generate(double*);
	//! Get the time since we started generating audio
    double GetTime() {return m_time;}
	void Clear(void);

private:
    std::list<CInstrument *>  m_instruments;
	void XmlLoadScore(IXMLDOMNode * xml);
	void XmlLoadInstrument(IXMLDOMNode * xml);
	void XmlLoadNote(IXMLDOMNode * xml, std::wstring & instrument);
	std::vector<CNote> m_notes;

	CChorus m_chorus;
	CFlange m_flange;
	CNoiseGate m_noiseGate;
	CCompression m_compression;
    CPianoInstrumentFactory m_pianoinstfactory;

	bool m_chorusOn, m_flangeOn, m_noiseGateOn, m_compressorOn;
};

