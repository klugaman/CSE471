#pragma once
#include "AudioNode.h"
#include "Note.h"

class CSynthesizer;

class CInstrument :
	public CAudioNode
{
public:
	CInstrument();
	virtual ~CInstrument();
	virtual void SetNote(CNote *note) = 0;
	void SetSynth(CSynthesizer* s){ m_synth = s; }

private:
	CSynthesizer* m_synth=nullptr;
};

