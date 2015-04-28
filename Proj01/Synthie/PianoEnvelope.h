#pragma once
#include "audionode.h"

class CPianoEnvelope :
	public CAudioNode
{
public:
	CPianoEnvelope(void);
	~CPianoEnvelope(void);

public:
    virtual void Start();
    virtual bool Generate();

    void SetAttack(double a) {m_attack = a;}
    void SetRelease(double r) {m_release = r;}
    void SetDuration(double d) {m_duration = d;}
    void SetSource(CAudioNode *node) {m_source = node;}
	void Sustain(bool s){m_sustain = s;}

private:
    // The attack time in seconds
    double  m_attack;

    // The release time in seconds
    double  m_release;

    // Duration of the note in seconds
    double  m_duration;

    // Where we get the samples we will change
    CAudioNode *m_source;

    // Current time when generating
    double  m_time;

	//Both varaibles together will indicates if the pedal 
	//is pressed or released after being pressed
	bool m_sustain, m_pedalPressed;
	double tmpgain;
};
