#pragma once
#include "audionode.h"
#include "Note.h"

class CInstrument :
	public CAudioNode
{
public:
	CInstrument(void);
	virtual ~CInstrument(void);
	virtual void SetNote(CNote *note) = 0;

	bool hasEffect(int effect) {return m_effects[effect];}
	void setEffect(int effect) {m_effects[effect] = true;}
	int getNumEffects()
	{
		int count = 0;
		for(int i=0; i<4; i++)
		{
			if(m_effects[i])
				count++;
		}
		return count;
	}

private:
	bool m_effects[4];
};

