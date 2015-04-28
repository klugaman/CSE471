#include "StdAfx.h"
#include "AdditiveSineWave.h"
#include <iostream>
#include <cmath>


short catchOverflow(double d)
{
    if(d < -32768)
        return -32768;
    else if(d > 32767)
        return 32767;

    return short(d);
}

CAdditiveSineWave::CAdditiveSineWave(void)
{
	m_vibratoFreq = 0.0;
	m_vibratoRate = 0.0 ;
	m_amplitude = 1000.0;
	m_amp[0] = 1.0;
	for(int i=1; i<=7; i++) m_amp[i] = 0.0;
	m_freq = 440.0;
	
}


CAdditiveSineWave::~CAdditiveSineWave(void)
{
	delete[] audio;
	audio = NULL;
}

void CAdditiveSineWave::Start()
{
	audio = new short[int(m_duration*GetSampleRate())];
    index = 0;
	CAdditiveSineWave::GenerateWaveTable();
}

bool CAdditiveSineWave::Generate()
{
    m_frame[1] = m_frame[0] = audio[index++];

    return true;
}

void CAdditiveSineWave::GenerateWaveTable()
{
	//double m_freq2 = 8.0 / (1./m_duration);
	//double m_freq1 = 6.0 / (1./m_duration);

	double radians1 = 0; // For the sine wave
	double radians2 = 0; // For the vibrato wave

	double time = 0; int i;
	for(i = 0; i < int(m_duration*GetSampleRate()); i++, time += 1./GetSampleRate())
	{   

		double sample = 0;
		for(float n = 1.0; n*m_freq <= 22050 && n<=8.0; n++)
			sample += (m_amplitude * m_amp[int(n-1)] * (sin(n * radians1)));		
				
		//In case we want to vary vibratoRate			
		/*if(time < m_duration/2.0){
			vibratoRate = time/(m_duration/2.0)*(m_freq2- m_freq1) + m_freq1;
		}
		else{
			vibratoRate = (time-m_duration/2.0)/(m_duration/2.0)*(m_freq1- m_freq2) + m_freq2;
		}*/
	
		radians2 += (2 * PI * m_vibratoRate) /  GetSampleRate();
		radians1 += (2 * PI * (m_freq + m_vibratoFreq * sin(radians2))) /  GetSampleRate();
		

		audio[i] = catchOverflow(sample);
	}
}