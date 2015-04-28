/*! \file DirSoundSource.h
 * 
 * Header file for implementation of DXSHOW-based media file input.
 * \version 3.00 10-11-09 Completely new version
 * \version 3.10 10-28-12 Revisions for consistency and documentation
 * \author Charles B. Owen
 */

#pragma once

#ifndef _DIRSOUNDSOURCE_H
#define _DIRSOUNDSOURCE_H

#include <vector>
#include <list>
#include <string>
#include <Afxmt.h>

// The DirectX and Windows headers
#include <dshow.h>

//! \cond
namespace std
{
#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif
}
//! \endcond

//! Input module for audio as read by the DirectShow system.
class CDirSoundSource
{
public:
    CDirSoundSource();
    ~CDirSoundSource();
    friend class CMovieCallback;

    bool Open(const char *p_filename);
    bool Open(const wchar_t *p_filename);
    bool Close();

    /*! \brief Indicate if the audio file is open
     * \returns true if currently open */
    bool IsOpen() const {return m_isopen;}

    //! Does this file have audio?
    /*! This will return false whenever there is no audio available, 
     * either because the file has no audio or if the end of the audio
     * in the file has been reached.
     * \returns true if file has audio */
    bool HasAudio() const {return m_hasaudio && !m_audioeof;}

    bool ReadFrame(short *frame);

    /*! \brief Read a frame as stereo.
     * The ReadFrame function will read however many channels are
     * available in the source file, including one if the audio is 
     * mono. This function will cause a mono file to create a stereo
     * frame by copying the sample read to the right channel. If the
     * audio file is stereo, this function works just like ReadFrame. 
     * \param p_frame Pointer to a location to store two audio samples
     * \returns true if a frame is read */
    bool ReadStereoFrame(short *p_frame)
    {
        if(!ReadFrame(p_frame)) return false;
        if(m_audiochannels == 1)
            p_frame[1] = p_frame[0];
        return true;
    }

    void Rewind();

    /*! \brief Get the sample rate
     * \returns Sample rate in samples per second */
    double GetSampleRate() const {return m_audiosamplerate;}

     /*! \brief Get the number of audio channels
     * \returns Number of audio channels */
    int GetNumChannels() const {return m_audiochannels;}

    /*! \brief Get the file duration
     * \returns File duration in seconds */
    double GetDuration() const {return m_duration;}

    /*! \brief Get the total number of sample frames
     * \returns Frame count */
    int GetNumSampleFrames() const {return int(m_duration * m_audiosamplerate);}


    /*! \brief Indicate if error message boxes are to be used.
     * If set true, an error will generate a message box using 
     * AfxMessageBox. The default is true. 
     * \param s true if error message boxes are to be used. */
    void SetErrorMsgBox(bool s) {m_errorMsgBox = s;}

    /*! \brief Obtain the error message
     * \returns Error message if an error occurs */
    const std::tstring &GetErrorMsg() const {return m_errorMsg;}

private:
    bool TryOpen(const char *p_filename);
    void NewAudioSample(IMediaSample *s);
    bool Error(const TCHAR *msg);

    HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, CComPtr<IPin> &pin);
    bool NullRendererDownstream(IBaseFilter *filter);
    bool DeleteDownstream(IBaseFilter *filter);
    void CheckEvents();
    bool InputConnected(IBaseFilter *filter);

    struct Sample
    {
        std::vector<BYTE>   m_data;
        double              m_cur;
        double              m_end;
    };

    // Error management
    std::tstring                    m_errorMsg;
    bool                            m_errorMsgBox;  // Messasge box on error

    // General graph objects
    CComPtr<IGraphBuilder>          m_graphBuilder;
    CComPtr<IFilterGraph>           m_filterGraph;
    CComPtr<IMediaControl>          m_mediaControl;
    CComPtr<IMediaPosition>         m_mediaPosition;
    CComPtr<IMediaEvent>            m_mediaEvent;
    CComPtr<IMediaSeeking>          m_mediaSeeking;
    HANDLE                          m_eventHandle;

    bool                            m_isopen;
    double                          m_duration;    // Stream duration
    double                          m_seekTime;     // Where we seeked to

    //
    // Audio objects
    //

    CComPtr<IBaseFilter>            m_audioGrabberBF;

    bool                            m_hasaudio;             // Audio data available
    CMovieCallback                 *m_audioCallback;
    CEvent                          m_videoRead;

    std::list<Sample>               m_audioSamples;
    CCriticalSection                m_audioSamplesLock;

    Sample                         *m_audioCurrentSample;
    int                             m_audioCurrentSamplePos;

    double                          m_audiosampleduration;  // Inverse of sample rate
    int                             m_audiochannels;        // Number of channels of audio in file
    double                          m_audiosamplerate;      // Sample rate of any audio stream
    int                             m_audiobitsize;         // Audio sample size in bits
    bool                            m_audioStreamEof;       // End of the audio stream has been reached
    bool                            m_audioeof;             // At end of file for audio
    int                             m_audiobytesperframe;
    double                          m_audioSecPerByte;      // Seconds per byte of audio
    CEvent                          m_audioRead;

};


#endif

