#include "StdAfx.h"
#include "PianoInstrumentFactory.h"
#include "audio/DirSoundSource.h"

CPianoInstrumentFactory::CPianoInstrumentFactory(void)
{
	m_sustain = false;   //Initally pedal is set to not pressed
	m_dynamics = 0.34;   //Default value for piano dynamics
}

CPianoInstrumentFactory::~CPianoInstrumentFactory(void)
{
}

CPianoInstrument *CPianoInstrumentFactory::CreateInstrument()
{
	m_instrument = new CPianoInstrument();
    m_instrument->GetPlayer()->SetSamples(&m_wave, (int)m_wave.size());
	m_instrument->Sustain(m_sustain);

    return m_instrument;
}

bool CPianoInstrumentFactory::LoadPianoSampleFile(const char *filename)
{
    m_wave_piano.clear();

    CDirSoundSource m_file;
    if(!m_file.Open(filename))
    {
        CString msg = L"Unable to open audio file: ";
        msg += filename;
        AfxMessageBox(msg);
        return false;
    }

    for(int i=0;  i<m_file.NumSampleFrames();  i++)
    {
        short frame[2];
        m_file.ReadFrame(frame);
        m_wave_piano.push_back(frame[0]);
    }

    m_file.Close();
    return true;
}
bool CPianoInstrumentFactory::LoadForteSampleFile(const char *filename)
{
    m_wave_forte.clear();

    CDirSoundSource m_file;
    if(!m_file.Open(filename))
    {
        CString msg = L"Unable to open audio file: ";
        msg += filename;
        AfxMessageBox(msg);
        return false;
    }

    for(int i=0;  i<m_file.NumSampleFrames();  i++)
    {
        short frame[2];
        m_file.ReadFrame(frame);
        m_wave_forte.push_back(frame[0]);
    }

    m_file.Close();
    return true;
}
bool CPianoInstrumentFactory::LoadPedalPressed(const char *filename)
{
    m_pedalPress.clear();

    CDirSoundSource m_file;
    if(!m_file.Open(filename))
    {
        CString msg = L"Unable to open audio file: ";
        msg += filename;
        AfxMessageBox(msg);
        return false;
    }

    for(int i=0;  i<m_file.NumSampleFrames();  i++)
    {
        short frame[2];
        m_file.ReadFrame(frame);
        m_pedalPress.push_back(frame[0]/6);
    }

    m_file.Close();
    return true;
}
bool CPianoInstrumentFactory::LoadPedalReleased(const char *filename)
{
    m_pedalRelease.clear();

    CDirSoundSource m_file;
    if(!m_file.Open(filename))
    {
        CString msg = L"Unable to open audio file: ";
        msg += filename;
        AfxMessageBox(msg);
        return false;
    }

    for(int i=0;  i<m_file.NumSampleFrames();  i++)
    {
        short frame[2];
        m_file.ReadFrame(frame);
        m_pedalRelease.push_back(frame[0]*58);
    }

    m_file.Close();
    return true;
}
//Interpolates a value in m_wave between soft and lout piano samples
//range is values from 0 - 1, 1 - really loud, 0 - really soft
void CPianoInstrumentFactory::Interpolate(double range)
{
	m_wave.clear();
	for(unsigned int x=0; x< m_wave_forte.size();x++)
	{
		m_wave.push_back(short((range*m_wave_forte.at(x)) + ((1 - range)*m_wave_piano.at(x))));
	}
}

void CPianoInstrumentFactory::SetNote(CNote *note)
{
    // Get a list of all attribute nodes and the
    // length of that list
    CComPtr<IXMLDOMNamedNodeMap> attributes;
    note->Node()->get_attributes(&attributes);
    long len;
    attributes->get_length(&len);

    // Loop over the list of attributes
    for(int i=0;  i<len;  i++)
    {
        // Get attribute i
        CComPtr<IXMLDOMNode> attrib;
        attributes->get_item(i, &attrib);

        // Get the name of the attribute
        CComBSTR name;
        attrib->get_nodeName(&name);

        CComVariant value;
        attrib->get_nodeValue(&value);

        if(name == "pedal")
        {
			CComBSTR val;
			val = value.bstrVal;
			if(val == "press")
			{
				m_sustain = true;
				m_instrument->GetPlayer()->PlayPedal(&m_pedalPress);
			}
			else if(val == "release")
			{
				m_sustain = false;
				m_instrument->GetPlayer()->PlayPedal(&m_pedalRelease);
			}
		}
		else if(name == "dynamics")
        {
			value.ChangeType(VT_R8);
			m_dynamics = value.dblVal;
			if(m_dynamics > 1)
				m_dynamics = 1;
			else if(m_dynamics < 0)
				m_dynamics = 0;
			Interpolate(m_dynamics);
		}
    }
}
