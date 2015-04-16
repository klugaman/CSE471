/*! \file MovieSource.h
 *  
 * Header file for implementation of DXSHOW-based media file input.
 * \version 2.00 10-27-07 Complete rewrite to remove all DirectDraw references.
 * \version 2.10 10-28-12 Revisions for consistency and documentation
 * \author Charles B. Owen
 */

#ifndef _MOVIESOURCE_H
#define _MOVIESOURCE_H

#include <vector>
#include <list>
#include <string>
#include <Afxmt.h>

#include "graphics/GrImage.h"

// The DirectX and Windows headers
#include <dshow.h>

//! Input class for reading movie files frame by frame
class CMovieSource
{
public:
    CMovieSource();
    ~CMovieSource();
    friend class CMovieSourceCallback;

    bool Open(const wchar_t *p_filename);
    bool Close();

    /*! \brief Indicate if the movie file is open
     * \returns true if currently open */
    bool IsOpen() const {return m_isopen;}

    //! Does this file have audio?
    /*! This will return false whenever there is no audio available, 
     * either because the file has no audio or if the end of the audio
     * in the file has been reached.
     * \returns true if file has audio */
    bool HasAudio() const {return m_hasaudio && !m_audioeof;}

    //! Does this file have video?
    /*! This will return false whenever there is no video available, 
     * either because the file has no video or if the end of the video
     * in the file has been reached.
     * \returns true if file has video */
    bool HasVideo() const {return m_hasvideo && !m_videoeof;}

    bool ReadImage(CGrImage &p_image);
    bool ReadAudio(std::vector<short> &p_audio);

    /*! \brief Get the sample rate
     * \returns Sample rate in samples per second */
    double GetSampleRate() const {return m_audiosamplerate;}

    /*! \brief Get the number of audio channels
     * \returns Number of audio channels */
    int GetNumChannels() const {return m_audiochannels;}

    /*! \brief Get the number of frames per second for the video
     * \returns Frames per second */
    double GetFPS() const {return m_videoframerate;}

    /*! \brief Get the file duration
     * \returns File duration in seconds */
    double GetDuration() const {return m_duration;}

    /*! Get the frame count
     * \returns Frame count */
    int GetFrameCount() const {return m_videoframescnt;}

    bool SeekTime(double p_time);

    //! Indicate if error message boxes are to be used.
    /*! If set true, an error will generate a message box using 
     * AfxMessageBox. The default is true. 
     * \param s true if error message boxes are to be used. */
    void SetErrorMsgBox(bool s) {m_errorMsgBox = s;}

    /*! \brief Obtain the error message
     * \returns Error message if an error occurs */
    const std::wstring &GetErrorMsg() const {return m_errorMsg;}

    //! Indicate if audio is to be ignored
    /*! The default is that both audio and video are loaded from the 
     * file. However, if the audio is not needed, this flag can be 
     * set prior to opening the file and the audio will be ignored.
     * If the audio is not ignored, it must be read by the program.
     * \param s true if audio is to be ignored */
    void SetIgnoreAudio(bool s) {m_ignoreAudio = s;}

    //! Get the name of the last loaded file
    /*! This will return the name of the last file successfully opened.
     * The value is whatever was passed to the Open function.
     * \returns Filename of last opened file */
    const wchar_t *GetFilename() const {return m_filename.c_str();}

private:
    bool TryOpen(const char *p_filename);
    void NewVideoSample(IMediaSample *s);
    void NewAudioSample(IMediaSample *s);
    bool Error(const TCHAR *msg);
    bool Open(const char *p_filename);

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
    std::wstring                    m_errorMsg;
    bool                            m_errorMsgBox;  // Message box on error

    std::wstring                    m_filename;

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
    // Video objects
    //

    CComPtr<IBaseFilter>            m_videoGrabberBF;

    bool                            m_hasvideo;             // Video data available

    double                          m_videoposition;
    int                             m_videowidth;           // Image width
    int	                            m_videoheight;          // Image height
    double                          m_videoframerate;       // Number of frames per second
    double                          m_videoframeduration;   // Frame duration as a float
    int                             m_videoframescnt;       // Number of frames
    int                             m_videodepth;           // Number of bits per pixel
    bool                            m_videoStreamEof;       // End of the video stream has been reached
    bool                            m_videoeof;             // At end of file for video

    CMovieSourceCallback                 *m_videoCallback;
    CEvent                          m_audioRead;

    std::list<Sample>               m_videoSamples;
    CCriticalSection                m_videoSamplesLock;

    //
    // Audio objects
    //

    CComPtr<IBaseFilter>            m_audioGrabberBF;

    bool                            m_hasaudio;             // Audio data available
    bool                            m_ignoreAudio;          // Indicate we don't want the audio
    CMovieSourceCallback                 *m_audioCallback;
    CEvent                          m_videoRead;

    std::list<Sample>               m_audioSamples;
    CCriticalSection                m_audioSamplesLock;

    double                          m_audioposition;        // Current audio position in time
    double                          m_audiosampleduration;  // Inverse of sample rate
    int                             m_audiochannels;        // Number of channels of audio in file
    double                          m_audiosamplerate;      // Sample rate of any audio stream
    int                             m_audiobitsize;         // Audio sample size in bits
    bool                            m_audioStreamEof;       // End of the audio stream has been reached
    bool                            m_audioeof;             // At end of file for audio
    int                             m_audioconsumed;        // Audio consumed from first block in queue
    int                             m_audiobytesperframe;
    double                          m_audioSecPerByte;      // Seconds per byte of audio
};


#endif

