#include "StdAfx.h"
#include "Instrument.h"


CInstrument::CInstrument(void)
{
	for(int i=0; i<4; i++)
	{
		m_effects[i] = false;
	}
}


CInstrument::~CInstrument(void)
{
}
