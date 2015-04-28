#pragma once
#include "PianoInstrument.h"
#include "Note.h"
#include <vector>

class CPianoInstrumentFactory
{
public:
	CPianoInstrumentFactory(void);
	~CPianoInstrumentFactory(void);

	CPianoInstrument *CreateInstrument();
	void SetNote(CNote *note);
	bool LoadPianoSampleFile(const char *);
	bool LoadForteSampleFile(const char *);
	bool LoadPedalPressed(const char *);
	bool LoadPedalReleased(const char *);
	void Interpolate(double range);
	bool GetSustain(){return m_sustain;}

private:
	std::vector<short> m_wave;
    std::vector<short> m_wave_piano;
	std::vector<short> m_wave_forte;
	std::vector<short> m_pedalPress;
	std::vector<short> m_pedalRelease;
	CPianoInstrument *m_instrument;
	double m_dynamics; //Variable used to determine the dynamics of the piano
	bool m_sustain;  //varaible used to indicate if pedal is pressed or released(true = pressed, falsue = not pressed)
};
