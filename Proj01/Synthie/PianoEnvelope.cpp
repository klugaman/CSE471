#include "StdAfx.h"
#include "PianoEnvelope.h"

CPianoEnvelope::CPianoEnvelope(void)
{
	m_attack = 0.01;
    m_release = 0.001;
    m_duration = 0.1;
    m_source = NULL;
	m_sustain = false;
	m_pedalPressed = false;
	tmpgain = 0;
}

CPianoEnvelope::~CPianoEnvelope(void)
{
}

void CPianoEnvelope::Start()
{
    m_time = 0;
}

bool CPianoEnvelope::Generate()
{
	double decay = GetSamplePeriod()/4;
	if(m_sustain)
	{
		m_duration = 30;
		decay = GetSamplePeriod()/50;
		m_pedalPressed = true;
	}
	else if(!m_sustain && m_pedalPressed == true)
	{
		m_time = m_duration - 0.3;
		m_pedalPressed = false;
	} 
    //
    // Part 1: Determine the current gain.  This depends
    // on what time period we are in.
    //


	double gain = 1;
	if(m_duration > 0.00)
	{
    if(m_time < m_attack)
    {
        // m_time/m_attack goes from 0 to 1 in the period
        gain = m_time / m_attack;
    }
    else if(m_time > (m_duration - m_release) && m_time < m_duration)
    {
        double releaseStart = m_duration - m_release;

        // (m_time - releaseStart) / (m_duration - releaseStart) goes
        // from 0 to 1 in the release period.  Subtract from 1 to make
        // it go from 1 to 0
        gain = 1 - (m_time - releaseStart) / (m_duration - releaseStart);
    }
	else if(m_time > 1.0 && m_time < m_duration)
	{
		if(tmpgain >= 0)
			gain = tmpgain-decay;
		else
			gain = tmpgain+decay;
	}
	}
    if(m_time >= m_duration)
    {
        // If we are done, set the audio to silence and
        // return false.
        m_frame[0] = m_frame[1] = 0;
        return false;
    }
	tmpgain = gain;
    //
    // Part 2: Generate the output from the input multiplied
    // by the gain.
    //
	
    if(m_source != NULL)
    {
        m_frame[0] = m_source->Frame(0) * gain;
        m_frame[1] = m_source->Frame(1) * gain;
    }

    //
    // Part 3: Update the time.
    //

    m_time += GetSamplePeriod();
    return true;
}
