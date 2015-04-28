
// AudioGenerateDoc.h : interface of the CAudioGenerateDoc class
//


#pragma once

#include "Progress.h"
#include "audio/wave.h"
#include "audio/DirSoundStream.h"	// Added by ClassView
#include "audio/WaveformBuffer.h"

class CAudioGenerateDoc : public CDocument, private CProgress
{
protected: // create from serialization only
	CAudioGenerateDoc();
	DECLARE_DYNCREATE(CAudioGenerateDoc)

// Attributes
public:
    double Duration() const {return m_duration;}
    double SampleRate() const {return m_sampleRate;}
    int NumChannels() const {return m_numChannels;}
    double Amplitude() const {return m_amplitude;}

    CWaveformBuffer *GetWaveformBuffer() {return &m_waveformBuffer;}


// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CAudioGenerateDoc();
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
	bool m_fileoutput;
	bool m_audiooutput;
	void GenerateWriteFrame(short *p_frame);
	double m_vibratoamp;
	bool OpenGenerateFile(CWaveOut &p_wave);
	void GenerateEnd();
	bool GenerateBegin();
	bool GenerateProgress(double p) {return Progress(p);}


    // Audio destinations..
    CWaveOut        m_wave;
    CDirSoundStream m_soundstream;
    CWaveformBuffer m_waveformBuffer;

	double   m_freq2;
	double   m_freq1;
	double   m_nyquist2;
	double   m_nyquist1;
	double   m_amplitude;
	double   m_duration;
	double   m_sampleRate;
	int      m_numChannels;
public:
	afx_msg void OnGenerateFileoutput();
	afx_msg void OnGenerateAudiooutput();
	afx_msg void OnGenerateSinewave();
	afx_msg void OnGenParameters();
	afx_msg void OnUpdateGenerateFileoutput(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGenerateAudiooutput(CCmdUI *pCmdUI);
	afx_msg void OnGenerateSinewaves();
	afx_msg void OnGenerate234();
	afx_msg void OnGenerate357();
	afx_msg void OnGenerateAllharmonics();
	afx_msg void OnGenerateOddharmonics();
};
