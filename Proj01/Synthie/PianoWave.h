#pragma once
#include "audionode.h"
#include "audio/DirSoundSource.h"
#include <vector>

class CPianoWave :
	public CAudioNode
{
public:
	CPianoWave(void);
	~CPianoWave(void);

public:
    virtual void Start();
    virtual bool Generate();

    void SetFreq(double f) {m_freq = f;}
    void SetAmplitude(double a) {m_amp = a;}
	void SetSamples(std::vector<short> *s, int n) {m_samplewave = s; m_numsamples = n;}
	void PlayPedal(std::vector<short> *);


private:
	std::vector<short> *m_samplewave, *m_pedalwave;
	int m_pedalCount; //Will be used to traverse otes to add pedal noise
	int m_numsamples;
    double m_freq;
	double m_rate;
    double m_amp;
    double m_sample;
	double tmp;
	int count;
};
