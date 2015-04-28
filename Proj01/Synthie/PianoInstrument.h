#pragma once
#include "instrument.h"
#include "PianoWave.h"
#include "Notes.h"
#include "PianoEnvelope.h"

class CPianoInstrument :
	public CInstrument
{
public:
	CPianoInstrument(void);
	~CPianoInstrument(void);
	std::wstring Name() const {return L"Piano";}

public:
    virtual void Start();
    virtual bool Generate();

    void SetFreq(double f) {m_pianowave.SetFreq(f);}
    void SetAmplitude(double a) {m_pianowave.SetAmplitude(a);}
    void SetDuration(double d) {m_ar.SetDuration(d);}
	void SetNote(CNote *note);
	void Sustain(bool s){m_ar.Sustain(s);}
	CPianoWave *GetPlayer() {return &m_pianowave;}
	void GetFrame(double frame[2]){frame[0] = m_frame[0]; frame[1] = m_frame[1];}


private:
    CPianoWave m_pianowave;
    double m_duration;
    double m_time;
	CPianoEnvelope m_ar;
};
