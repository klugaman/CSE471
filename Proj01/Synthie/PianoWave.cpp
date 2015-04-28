#include "StdAfx.h"
#include "PianoWave.h"
#include <cmath>
#include "Note.h"

CPianoWave::CPianoWave(void)
{
	m_sample = 0;
	m_pedalCount = -1;
	//m_freq = 440;
}

CPianoWave::~CPianoWave(void)
{
//	delete[] m_samplewave;
}
void CPianoWave::Start()
{
    m_sample = 1820;
	tmp = 0;
	m_rate = m_freq / 261.626;
}

bool CPianoWave::Generate()
{
	//Linear interpolation between the sample values to get a weighted average of sample wave values 
	//when changing frequency of note
    double inbetween = fmod(m_sample, 1);
	double lrpsample = ((1. - inbetween) * m_samplewave->at(int(m_sample))) + (inbetween * m_samplewave->at(int(m_sample) + 1));
	m_frame[0] = lrpsample /  32768.0;
	if(m_pedalCount > -1)
	{
		m_frame[0] = m_frame[0] + (double(m_pedalwave->at(m_pedalCount))/32768.0);
		m_pedalCount++;
		if(m_pedalCount == m_pedalwave->size())
			m_pedalCount = -1;
	}
	m_frame[1] = m_frame[0];
	//Determine the rate at which the next sample is played, changing rates changes the frequency of the note
	//Initial note thats recorded is C4(middle C), thats 261.624Hz, to determine the rate of change for any other
	//note, divide the notes frequency by the frequency of C4 = 261.626
	m_sample += m_rate;
	//Make sure sample stays within the bounds of the maximum samples of the wave
	if(m_sample > 54002 )
		m_sample = 53834 + (m_sample - 54002); //This makes sure the different notes loop over the sample
    return true;
}

void CPianoWave::PlayPedal(std::vector<short> *pWave)
{
	m_pedalwave = pWave;
	m_pedalCount = 0;
}