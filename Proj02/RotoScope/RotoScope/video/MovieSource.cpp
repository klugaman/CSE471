/*! \file MovieSource.cpp
 * 
 * This module implements input from DXSHOW movie files.
 * \author Charles B. Owen
 */

#include "StdAfx.h"

#include <cassert>
#include <vector>
#include <string>

#include "MovieSource.h"

#pragma include_alias( "dxtrans.h", "qedit.h" )

#define __IDxtCompositor_INTERFACE_DEFINED__

#define __IDxtAlphaSetter_INTERFACE_DEFINED__

#define __IDxtJpeg_INTERFACE_DEFINED__

#define __IDxtKey_INTERFACE_DEFINED__
#include <audio/qedit.h>

//! \cond
// The video callback class
class CMovieSourceCallback : public ISampleGrabberCB
{
public:
    CMovieSourceCallback(CMovieSource *p_user, bool iv) : m_user(p_user), m_isVideo(iv) {}

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppv); 
    virtual HRESULT STDMETHODCALLTYPE SampleCB( double SampleTime, IMediaSample * pSample );
    virtual HRESULT STDMETHODCALLTYPE BufferCB( double dblSampleTime, BYTE * pBuffer, long iBufSize );

private:
    CMovieSource    *m_user;
    bool             m_isVideo;
};
//! \endcond


using namespace std;



//! Constructor for movie file input module.
CMovieSource::CMovieSource()
{
    CoInitialize(NULL);

    m_hasaudio = false;
    m_hasvideo = false;
    m_isopen = false;
    m_errorMsgBox = true;
    m_ignoreAudio = false;

    m_videoCallback = new CMovieSourceCallback(this, true);
    m_audioCallback = new CMovieSourceCallback(this, false);
}


/*! Destructor for module.  Note that modules are not
 * closed, but rather destroyed.  This takes care of
 * closing any open file.
 */
CMovieSource::~CMovieSource(void)
{
    Close();
    delete m_videoCallback;
    delete m_audioCallback;

    CoUninitialize();
}



//! Open the media file.
/*! \param p_filename File to open
 * \return true if successful
 */
bool CMovieSource::Open(const wchar_t *p_filename)
{
    m_filename = p_filename;

    string filename;

    size_t len = wcslen(p_filename);
    vector<char> vfilename;
    vfilename.resize(len + 1);
    int cnt = WideCharToMultiByte(CP_UTF8, 0, p_filename, -1, &vfilename[0], int(len) + 1, NULL, NULL);
    filename = &vfilename[0];

    return Open(filename.c_str());
}

//! Open the media file.
/*! \param p_filename File to open
 * \return true if successful
 */
bool CMovieSource::Open(const char *p_filename)
{
    Close();

    if(!TryOpen(p_filename))
    {
        Close();        // Open failed, so return to a closed state
        if(m_errorMsgBox)
        {
            AfxMessageBox(m_errorMsg.c_str());
        }
        return false;
    }

    return true;
}

//
// Name :         CMovieSource::TryOpen()
// Description :  Attempt to open the stream. This is the function
//                that actually does all of the work.
//

bool CMovieSource::TryOpen(const char *p_filename)
{
    HRESULT hr;
    string filename;

    filename = p_filename;

    // Until we know
    m_hasvideo = false;
    m_hasaudio = false;
    m_audiosamplerate = 0.;
    m_audioStreamEof = false;
    m_videoStreamEof = false;
    m_videoeof = false;
    m_audioeof = false;
    m_audioposition = 0;
    m_videoposition = 0;
    m_audioconsumed = 0;
    m_seekTime = 0;

    m_videoSamples.clear();
    m_audioSamples.clear();
   
    // Create the filter graph
    if (FAILED(m_graphBuilder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
        return Error(TEXT("Unable to create a DirectShow filter graph"));

    // Obtain a bunch of interfaces we will neet
    m_graphBuilder.QueryInterface(&m_mediaControl);
    m_graphBuilder.QueryInterface(&m_mediaPosition);
    m_graphBuilder.QueryInterface(&m_filterGraph);
    m_graphBuilder.QueryInterface(&m_mediaSeeking);

    m_graphBuilder.QueryInterface(&m_mediaEvent);
    m_mediaEvent->GetEventHandle( (OAEVENT*)&m_eventHandle);

    //
    // Create the video sample grabber
    //

    if(FAILED(m_videoGrabberBF.CoCreateInstance(CLSID_SampleGrabber)))
        return Error(TEXT("Unable to create a DirectShow sample grabber"));

    CComPtr<ISampleGrabber>         videoGrabber;
    m_videoGrabberBF.QueryInterface(&videoGrabber);

    // Set the sample grabber media type
    AM_MEDIA_TYPE sg;

    ZeroMemory(&sg, sizeof(AM_MEDIA_TYPE));
    sg.majortype = MEDIATYPE_Video;
    sg.subtype = MEDIASUBTYPE_RGB24;

    videoGrabber->SetMediaType(&sg);

    if(FAILED(m_graphBuilder->AddFilter(m_videoGrabberBF, L"VideoGrabber")))
        return Error(TEXT("Unable to add the video sample grabber to the filter graph"));

    //
    // Create the audio sample grabber
    //

    if(FAILED(m_audioGrabberBF.CoCreateInstance(CLSID_SampleGrabber)))
        return Error(TEXT("Unable to create a DirectShow sample grabber"));

    CComPtr<ISampleGrabber>         audioGrabber;
    m_audioGrabberBF.QueryInterface(&audioGrabber);

    // Set the sample grabber media type
    AM_MEDIA_TYPE asg;

    ZeroMemory(&asg, sizeof(AM_MEDIA_TYPE));
    asg.majortype = MEDIATYPE_Audio;
    asg.subtype = MEDIASUBTYPE_PCM;

    audioGrabber->SetMediaType(&asg);

    if(FAILED(m_graphBuilder->AddFilter(m_audioGrabberBF, L"AudioGrabber")))
        return Error(TEXT("Unable to add the video sample grabber to the filter graph"));

    //
    // Now we render the file.  The video will connect to the sample grabber.
    // A video renderer will be created as well, which we'll have to remove.
    //

    int flen = int(filename.length());
    std::vector<WCHAR>   wPath(flen+1);
    MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, &wPath[0], flen+1);

    hr = m_graphBuilder->RenderFile(&wPath[0], NULL);
    if(hr != S_OK)
        return Error(TEXT("Unable to open video file"));

    // Determine if anything connected to our two sample grabbers.
    if(InputConnected(m_videoGrabberBF))
        m_hasvideo = true;
    else
        m_videoGrabberBF.Release();     // Don't need it...

    if(InputConnected(m_audioGrabberBF))
        m_hasaudio = true;
    else
        m_audioGrabberBF.Release();

    if(m_hasvideo)
    {
        // Replace downstream of video and audio sample grabbers with a null renderer
        if(!NullRendererDownstream(m_videoGrabberBF))
            return false;

        videoGrabber->SetBufferSamples(false);
        videoGrabber->SetCallback(m_videoCallback, 0);
        
        //
        // Determine information about the video.  Some videos just won't give
        // you much in DirectShow other than total duration.  For those we 
        // just have to live without knowing.
        //

        videoGrabber->GetConnectedMediaType(&sg);
        VIDEOINFOHEADER *vi = (VIDEOINFOHEADER *)(sg.pbFormat);

        REFTIME length;
        m_mediaPosition->get_Duration(&length);
        m_duration = length;

        m_videoframeduration = vi->AvgTimePerFrame * 0.0000001;
        if(m_videoframeduration > 0)
        {
            m_videoframerate = 1. / m_videoframeduration;
            m_videoframescnt = int(m_videoframerate * m_duration);
        }
        else
        {
            m_videoframerate = 0;
            m_videoframescnt = 0;
        }
        
        m_videowidth = vi->bmiHeader.biWidth;
        m_videoheight = vi->bmiHeader.biHeight;
        m_videoRead.SetEvent();
    }

    if(m_hasaudio)
    {
        if(!NullRendererDownstream(m_audioGrabberBF))
            return false;

        audioGrabber->SetBufferSamples(false);

        if(!m_ignoreAudio)
            audioGrabber->SetCallback(m_audioCallback, 0);
        else
            m_hasaudio = false;
        
        //
        // Determine information about the audio.
        //

        audioGrabber->GetConnectedMediaType(&asg);
        WAVEFORMATEX *wf = (WAVEFORMATEX *)(asg.pbFormat);

        m_audiosamplerate = wf->nSamplesPerSec;
        m_audiosampleduration = 1./m_audiosamplerate;
        m_audiobitsize = wf->wBitsPerSample;
        m_audiochannels = wf->nChannels;
        m_audiobytesperframe = m_audiobitsize / 8 * m_audiochannels;
        m_audioSecPerByte = m_audiosampleduration / m_audiobytesperframe;
        m_audioRead.SetEvent();
    }

    //
    // We set the graph synchronization source to NULL so the
    // graph has no clock and will run as fast as possible.
    //

    CComPtr<IMediaFilter> mediaFilter;
    m_graphBuilder.QueryInterface(&mediaFilter);

    mediaFilter->SetSyncSource(NULL);


    m_isopen = true;
    m_mediaControl->Run();

    return true;
}

//
// Name :         CMovieSource::InputConnected(IBaseFilter *filter)
// Description :  Returns true if the input of a filter is connected to something.
//

bool CMovieSource::InputConnected(IBaseFilter *filter)
{
    // Locate the filters input pin
    CComPtr<IPin> inputPin;
    GetPin(filter, PINDIR_INPUT, 0, inputPin);
    if(inputPin == NULL)
        return false;

    CComPtr<IPin> fromPin;
    inputPin->ConnectedTo(&fromPin);
    if(fromPin == NULL)
        return false;

    return true;
}


//
// Name :         CMovieSource::NullRendererDownstream()
// Description :  Given a pointer to a filter in the filter graph, this function
//                removes all filters that are downstream of that filter and replaces
//                them with a Null Renderer.  This is used to
//                remove the filters automatically created by the 
//                RenderFile function.
//

bool CMovieSource::NullRendererDownstream(IBaseFilter *filter)
{
    HRESULT hr;

    // Delete everything downstream from this filter
    if(!DeleteDownstream(filter))
        return false;

    //
    // Locate the filters output pin
    //

    CComPtr< IPin > pGrabOut;
    hr = GetPin(filter, PINDIR_OUTPUT, 0, pGrabOut);

    //
    // Create a null renderer to follow the filter
    //

    CComPtr<IBaseFilter> nullRenderer;

    if(FAILED(nullRenderer.CoCreateInstance(CLSID_NullRenderer)))
        return Error(TEXT("Unable to create null renderer filter"));

    if(FAILED(m_graphBuilder->AddFilter(nullRenderer, L"NullRenderer")))
        return Error(TEXT("Unable to add null renderer to the filter graph"));

    //
    // Get the pins for the sample grabber and the null renderer
    //


    CComPtr< IPin > pNullIn;
    if(FAILED(GetPin(nullRenderer, PINDIR_INPUT, 0, pNullIn)))
        return Error(TEXT("Unable find the null renderer input pin."));

    if(FAILED(m_graphBuilder->Connect( pGrabOut, pNullIn )))
        return Error(TEXT("Unable find the sample grabber output pin."));

    return true;
}


//
// Name :       CMovieSource::DeleteDownstream()
// Description: Delete all nodes downstream of a selected node.
// Note :       Recursive.
//

bool CMovieSource::DeleteDownstream(IBaseFilter *filter)
{
    HRESULT hr;

    //
    // Locate the filters output pin
    //

    CComPtr< IPin > pGrabOut;
    if(GetPin(filter, PINDIR_OUTPUT, 0, pGrabOut) != S_OK || pGrabOut == NULL)
        return true;            // If no output pin

    // What is it connected to?
    CComPtr<IPin> pGrabToPin;
    hr = pGrabOut->ConnectedTo(&pGrabToPin);
    if(pGrabToPin == NULL)
        return true;            // If output pin not connected to anything

    // Determine the connected filter
    PIN_INFO pinfo;
    pGrabToPin->QueryPinInfo(&pinfo);

    IBaseFilter *filterDownstream = pinfo.pFilter;

    // Disconnect them.
    hr = m_filterGraph->Disconnect(pGrabOut);
    hr = m_filterGraph->Disconnect(pGrabToPin);

    DeleteDownstream(filterDownstream);
    hr = m_filterGraph->RemoveFilter(filterDownstream);
    return true;
}


/*
 * Name:        CMovieSource::GetPin()
 * Description: A helper func which get input/output pins of a filter
 */

HRESULT CMovieSource::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, CComPtr<IPin> &pin)
{
    CComPtr< IEnumPins > pEnum;
    pin = NULL;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;
        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                pin = pPin;
                // Found requested pin, so clear error
                hr = S_OK;
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}



//! Close the open file.
//! \returns true if successful
bool CMovieSource::Close()
{
    if(!m_isopen)
        return false;

    m_isopen = false;

    if(m_mediaControl != NULL)
    {
        m_audioRead.SetEvent();   // Release any threads that may be waiting
        m_videoRead.SetEvent();   // Release any threads that may be waiting
        m_mediaControl->Stop();

        // Second time just in case
        m_audioRead.SetEvent();   // Release any threads that may be waiting
        m_videoRead.SetEvent();   // Release any threads that may be waiting
    }

    m_graphBuilder.Release();
    m_filterGraph.Release();
    m_mediaControl.Release();
    m_mediaPosition.Release();
    m_mediaEvent.Release();
    m_mediaSeeking.Release();
    m_videoGrabberBF.Release();
    m_audioGrabberBF.Release();

    m_videoSamples.clear();
    m_audioSamples.clear();

    m_hasaudio = false;
    m_hasvideo = false;

    return true;
}
 

//! Read one image from the video file.
/*! \param p_image Destination for read image
 * \returns true if successful */
bool CMovieSource::ReadImage(CGrImage &p_image)
{
    CheckEvents();

    if(!m_hasvideo)
        return false;

    if(m_videoeof)
        return false;

    m_videoSamplesLock.Lock();
    int tries = 0;
    while(!m_videoStreamEof && m_videoSamples.empty())
    {
        // Handle running out of video before we run out of audio...
        tries++;
        if(tries >= 50)
        {
            m_videoStreamEof = true;
            break;
        }

        m_videoSamplesLock.Unlock();
        Sleep(10);
        m_videoSamplesLock.Lock();
    }

    // Handle case of end of file
    if(m_videoStreamEof && m_videoSamples.empty())
    {
        m_videoeof = true;
        m_videoSamplesLock.Unlock();
        return false;
    }

    Sample &s = m_videoSamples.front();
    m_videoposition = s.m_cur;

    if(s.m_end >= m_duration)
        m_videoeof = true;

    // Allocate the frame to correct size.
    p_image.SetSize(m_videowidth, m_videoheight);

    long bufferSize = m_videowidth * m_videoheight * 3;
    memcpy(p_image[0], &s.m_data[0], bufferSize);

    m_videoSamples.pop_front();
    m_videoSamplesLock.Unlock();
    m_videoRead.SetEvent();
    return true;
}


//! Read an audio frame
/*! This has become more complicated, since we don't always know
 * any frame duration information.  So, we may return a variable 
 * number of audio frames per video frame.
 * \param p_audio Destination vector for read audio frame
 * \returns true if audio available, false if no audio or end of file.
 * Will return true and a p_audio size of zero if we are not
 * at the end of the file, but we have not read any audio in, yet. */
bool CMovieSource::ReadAudio(std::vector<short> &p_audio)
{
    CheckEvents();

    if(!m_hasaudio || m_audioeof)
    {
        p_audio.clear();
        return false;
    }

    int bytesavailable;
    int framesavailable;

    //
    // We have a loop here that will wait for sufficient audio data.  We 
    // may not have enough data for a complete frame yet, so this will try
    // to see what we have and loop back if necessary.
    //

    m_audioSamplesLock.Lock();
    int tries = 0;
    while(true)
    {
        // Determine how much audio data is currently avaialble.
        // We're going to return audio up to 33 milliseconds past the current
        // frame if available.  First we have to determine how many bytes
        // of audio are available right now that we can consume from.
        bytesavailable = -m_audioconsumed;      // How many bytes we can use
                                                // m_audioconsumed of first block is not available
        double desiredendtime;
        if(m_videoframeduration > 0)
            desiredendtime = m_videoposition + m_videoframeduration + 0.001;  // Will accept till this time
        else
            desiredendtime = m_videoposition + 0.035;  // Will accept till this time

        bool doneforsure = false;
        for(list<Sample>::iterator i=m_audioSamples.begin();  i!=m_audioSamples.end();  i++)
        {
            // Is the block starts after our desired end time, we are done.
            if(i->m_cur >= desiredendtime)
            {
                // If we have a block start time greater than or equal
                // to the desired end time, we have all we need and are done for sure.
                doneforsure = true;
                break;
            }

            // Count the data from the block
            bytesavailable += (int)i->m_data.size();
        }

        // doneforsure will not be set if we reached the end of the audio without reaching
        // a block that is at or beyond our desired end time.  In that case we may
        // want to wait, though we might have enough if the block had enough in it.

        if(bytesavailable > 0)
        {
            // This may be too much if we have data past our target end time.
            // If so, trim it a bit
            double haveendtime = m_audioSamples.front().m_cur + (m_audioconsumed + bytesavailable) * m_audioSecPerByte;
            if(haveendtime > desiredendtime)
            {
                // Trim the amount too much and we are done because we have
                // enough available to reach our desired end time.
                bytesavailable -= int((haveendtime - desiredendtime) * m_audiosamplerate) * m_audiobytesperframe;
                break;      // We're there...
            }
        }
        
        if(doneforsure)
        {
            // In this case we are done because the next block is beyond our desired end time.
            break;
        }

        // If stream end of file, we accept whatever we have.
        if(m_audioStreamEof)
            break;

        //
        // Some files appear to be short on audio.  For those files, we expect audio
        // up to the end of the video and it's not there.  So, we keep track of
        // how many times this loops and consider the audio done if we loop for
        // 500ms for example.
        //

        tries++;
        if(tries >= 50)
        {
            m_audioStreamEof = true;
            break;
        }

        m_audioSamplesLock.Unlock();
        Sleep(10);          // Wait for some decompression to occur
        m_audioSamplesLock.Lock();
    }

    // It must be an even number of audio frames
    framesavailable = bytesavailable / m_audiobytesperframe;    // Number of frames

    // Handle the case of no bytes available at all.
    // This will only occur if there stream has reached end of file.
    if(bytesavailable <= 0)
    {
        m_audioposition = m_duration;
        m_audioeof = true;
        m_audioSamplesLock.Unlock();
        p_audio.clear();
        return false;
    }

    // Current audio position is the start time of the first available block
    // of audio plus the amount of time we've consumed from it.
    m_audioposition = m_audioSamples.front().m_cur + m_audioconsumed * m_audioSecPerByte;

    if(m_videoframerate > 0)
    {
        // If we are close to a perfect frame, make that be 
        // the frame size we use.
        int samplesperframe = int(m_audiosamplerate * m_videoframeduration) - 1;

        if(framesavailable > samplesperframe)
            framesavailable = samplesperframe + 1;
    }

    bytesavailable = framesavailable * m_audiobytesperframe;        // Even number of bytes

    // Allocate it in p_audio
    p_audio.resize(framesavailable * m_audiochannels);

    // Now consume it
    vector<short>::iterator dest = p_audio.begin(); // For writing it out
    while(bytesavailable > 0)
    {
        Sample &samp = m_audioSamples.front();      // Reference to first block

        // Amount of available data in the block
        int blocksize = (int)samp.m_data.size();            // Total size of the block
        int availinblock = blocksize - m_audioconsumed;     // Available unconsumed.

        // We copy the lesser of what's avialable or what we need
        int tocopy = bytesavailable < availinblock ? bytesavailable : availinblock;
        memcpy(&(*dest), &(samp.m_data[m_audioconsumed]), tocopy);
        dest += tocopy / sizeof(short);             // Advance

        bytesavailable -= tocopy;       // Consume these bytes from available
        m_audioconsumed += tocopy;      // And indicate as consumed in the block

        if(m_audioconsumed >= blocksize)
        {
            // We have consumed the entire block
            m_audioconsumed = 0;            // Nothing consumed from the next one
            m_audioSamples.pop_front();     // Consume the block
        }
    }

    if(m_audioposition >= m_duration)
        m_audioeof = true;

    m_audioSamplesLock.Unlock();
    m_audioRead.SetEvent();
    return true;
}


void CMovieSource::CheckEvents()
{
    if(WaitForSingleObject(m_eventHandle, 0) == WAIT_OBJECT_0)
    {
        long eventCode, param1, param2;
        m_mediaEvent->GetEvent(&eventCode, &param1, &param2, 0);
        if(eventCode == EC_COMPLETE)
        {
            m_videoStreamEof = true;
            m_audioStreamEof = true;
        }

        m_mediaEvent->FreeEventParams(eventCode, param1, param2);
    }
}

//! Seek the source file to a specified time point
/*! \param p_time The time to seek to
 * \returns true if successful */
bool CMovieSource::SeekTime(double p_time)
{
    if(!m_isopen)
        return false;

    // First we stop everything.  This is like a partial close here
    m_isopen = false;

    if(m_mediaControl != NULL)
    {
        m_videoRead.SetEvent();
        m_audioRead.SetEvent();
        m_mediaControl->Stop();
        m_videoRead.SetEvent();
        m_audioRead.SetEvent();
    }

    Sleep(1000);

    // Clear all of the queues
    m_videoSamples.clear();
    m_audioSamples.clear();
    m_audioconsumed = 0;

    // Get a seeking interface
    CComPtr<IMediaSeeking> mediaSeeking;
    m_filterGraph.QueryInterface(&mediaSeeking);

    // What are our capabilities?
    DWORD cap;
    mediaSeeking->GetCapabilities(&cap);
    bool canseek = false;

    if(m_videoposition < p_time)
    {
        // Can do this if we can seek forward or absolute
        if(cap & (AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanSeekForwards))
        {
            // Can do it
            canseek = true;
        }
    }
    else
    {
        if(cap & AM_SEEKING_CanSeekAbsolute)
            canseek = true;
    }

    if(canseek)
    {
        REFERENCE_TIME reftime = (REFERENCE_TIME)(p_time * 10000000);
        HRESULT hr = mediaSeeking->SetPositions(&reftime, AM_SEEKING_AbsolutePositioning,
                                   NULL, AM_SEEKING_NoPositioning);

        m_videoposition = p_time;
        m_audioposition = p_time;
        m_seekTime = p_time;

        m_videoStreamEof = false;
        m_audioStreamEof = false;
        m_audioeof = false;
        m_videoeof = false;
    }
    else
    {
        m_seekTime = m_videoposition;       // We start where we left off
    }

    // Back active again
    m_mediaControl->Run();
    m_isopen = true;
    return canseek;
}


void CMovieSource::NewVideoSample(IMediaSample *s)
{ 
    m_videoSamplesLock.Lock();
    while(m_isopen && m_videoSamples.size() >= 5)
    {
        m_videoSamplesLock.Unlock();
        m_videoRead.Lock();
        m_videoSamplesLock.Lock();
    }

    if(!m_isopen)
    {
        // We have been closed
        m_videoSamplesLock.Unlock();
        return;
    }

    long size = s->GetActualDataLength();
    BYTE *ptr;
    s->GetPointer(&ptr);

    m_videoSamples.push_back(Sample());
    Sample &samp = m_videoSamples.back();

    REFERENCE_TIME cur, end;
    s->GetTime(&cur, &end);
    samp.m_cur = m_seekTime + cur * 0.0000001;
    samp.m_end = m_seekTime + end * 0.0000001;

    samp.m_data.resize(size);
    memcpy(&samp.m_data[0], ptr, size);

    m_videoSamplesLock.Unlock();
}




void CMovieSource::NewAudioSample(IMediaSample *s)
{ 
    m_audioSamplesLock.Lock();
    //while(m_isopen && m_audioSamples.size() >= 200)
    //{
    //    m_audioSamplesLock.Unlock();
    //    m_audioRead.Lock();
    //    m_audioSamplesLock.Lock();
    //}

    if(!m_isopen)
    {
        m_audioSamplesLock.Unlock();
        return;
    }

    long size = s->GetActualDataLength();
    BYTE *ptr;
    s->GetPointer(&ptr);

    m_audioSamples.push_back(Sample());
    Sample &samp = m_audioSamples.back();

    REFERENCE_TIME cur, end;
    s->GetTime(&cur, &end);
    samp.m_cur = m_seekTime + cur * 0.0000001;
    samp.m_end = m_seekTime + end * 0.0000001;

    samp.m_data.resize(size);
    memcpy(&samp.m_data[0], ptr, size);

    m_audioSamplesLock.Unlock();
}

//
// Name :         CMovieSource::Error(const TCHAR *msg)
// Description :  Sets the error string and returns false (error value)
//

bool CMovieSource::Error(const TCHAR *msg)
{
    m_errorMsg = msg;
    return false;
}

//! \cond
ULONG CMovieSourceCallback::AddRef() {return 2;}
ULONG CMovieSourceCallback::Release() {return 1;}

//
// Name :        CMovieSourceCallback::QueryInterface()
// Description : Return a pointer to a specific interface for this class.
//

HRESULT CMovieSourceCallback::QueryInterface(REFIID riid, void ** ppv) 
{
	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
	{
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}    
	return ResultFromScode(E_NOINTERFACE);
}

HRESULT CMovieSourceCallback::SampleCB( double SampleTime, IMediaSample * pSample ) 
{ 
    if(m_isVideo)
        m_user->NewVideoSample(pSample);
    else
        m_user->NewAudioSample(pSample);
    return 0; 
};

HRESULT CMovieSourceCallback::BufferCB( double dblSampleTime, BYTE * pBuffer, long iBufSize )  {return 0;}

//! \endcond

