
// AudioProcessDoc.h : interface of the CAudioProcessDoc class
//


#pragma once

#include "Progress.h"
#include "audio/wave.h"
#include "audio/DirSoundStream.h"
#include "audio/DirSoundSource.h"
#include "audio/WaveformBuffer.h"


class CAudioProcessDoc : public CDocument, private CProgress
{
protected: // create from serialization only
	CAudioProcessDoc();
	DECLARE_DYNCREATE(CAudioProcessDoc)

// Attributes
public:
    int SampleFrames() const {return m_numSampleFrames;}
    double SampleRate() const {return m_sampleRate;}
    int NumChannels() const {return m_numChannels;}
    double Amplitude() const {return m_amplitude;}

    CWaveformBuffer *GetWaveformBuffer() {return &m_waveformBuffer;}

	// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CAudioProcessDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

private:
	void ProcessReadFrame(short *p_frame);
	bool m_fileoutput;
	bool m_audiooutput;
	void ProcessWriteFrame(short *p_frame);
	bool OpenProcessFile(CWaveOut &p_wave);
	void ProcessEnd();
	bool ProcessBegin();
    bool ProcessProgress(double p) {return Progress(p);}

    CDirSoundSource   m_wavein;
    CWaveOut          m_waveout;
    CWaveformBuffer m_waveformBuffer;

    double   m_amplitude;
    double   m_sampleRate;
    int      m_numChannels;
    int      m_numSampleFrames;
	struct FTerm {
		int m_delay;
		double m_weight;
	};

	inline	short	RangeBound(double	d)
	{
		if (d	>	32767)
			return	32767;
		if (d	<	-32768)
			return	-32768;
		return	short(d);
	};

	int sLinex = 0;
	int sLiney = 0;
	std::list<FTerm> m_xterms;
	std::list<FTerm> m_yterms;


    CDirSoundStream  m_soundstream;
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnProcessFileoutput();
	afx_msg void OnProcessAudiooutput();
	afx_msg void OnUpdateProcessFileoutput(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProcessAudiooutput(CCmdUI *pCmdUI);
	afx_msg void OnProcessCopy();
	afx_msg void OnProcessParameters();
	afx_msg void OnProcessRamp();
	afx_msg void OnProcessRampin();
	afx_msg void OnProcessTremolo();
	afx_msg void OnProcessHalfspeed();
	afx_msg void OnProcessDoublespeed();
	afx_msg void OnProcessBackwards();


	afx_msg void OnFilterResonfilter();
	afx_msg void OnFilterLoadandapply();
	afx_msg void OnProcessDelay();
};
