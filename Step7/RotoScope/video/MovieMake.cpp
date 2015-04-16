/*! \file MovieMake.cpp
 * Movie maker file based on Window Media Software Development Kit.
 * Writes mideo and stereo audio movie files based on
 * supply of audio and video data frame by frame.
 * \version 1.01  4-35-03 No longer references or requires CImage
 * \version 1.02 10-15-06 Requires CImage again Read profiles. Works for various sample rates and frame rates and sizes.
 * \version 2.00 11-06-10 Revised to use CPix instead of CImage. Local version of qedit.h
 * \version 2.10 10-28-12 Revised to use CGrImage instead of CPix. Documentation updates
 */

#include "StdAfx.h"
#include <vector>
#include <cassert>

#include "moviemake.h"
#include <wmsysprf.h>
#include <mmreg.h>

using namespace std;

/*! \brief Constructor */
CMovieMake::CMovieMake(void)
{
    CoInitialize(NULL);
    m_isopen = false;
    m_profileName = L"profile720x480.prx";
}

/*! \brief Destructor */
CMovieMake::~CMovieMake(void)
{
    Close();
    CoUninitialize();
}

//! Open a video file for output.
/*! \param p_filename File to open 
 * \returns true if successful */
bool CMovieMake::Open(const TCHAR * p_filename)
{
    Close();

    m_audiostream = -1;
    m_videostream = -1;

    // 
    // First create the writer object
    //

    HRESULT r = WMCreateWriter(NULL, &m_writer);
    if(r != S_OK)
    {
        AfxMessageBox(TEXT("Failed to create file writer"));
        return false;
        
    }

    if(!ObtainProfile())
    {
        m_writer.Release();
        return false;
    }

    //
    // Set the profile for the writer
    //

    r = m_writer->SetProfile(m_profile);
    if(r != S_OK)
    {
        m_writer.Release();
        m_profile.Release();
        AfxMessageBox(TEXT("Failed to set the profile for the writer"));
        return false;
    }

    //
    // Set the file name
    //

#if UNICODE
    r = m_writer->SetOutputFilename(p_filename);
#else
    std::vector<WCHAR>  wfilename;
    wfilename.resize(strlen(p_filename)+1);
    MultiByteToWideChar(CP_ACP, 0, p_filename, int(strlen(p_filename))+1, &wfilename[0], int(strlen(p_filename))+1);
    r = m_writer->SetOutputFilename(&wfilename[0]);
#endif
    if(r != S_OK)
    {
         m_writer.Release();
       AfxMessageBox(TEXT("Failed to create file writer"));
        return false;   
    }

    DWORD inputcnt;
    m_writer->GetInputCount(&inputcnt);
    assert(inputcnt == 2);

    r = m_writer->BeginWriting();
    if(r != S_OK)
    {
        AfxMessageBox(TEXT("Failed to begin writing"));
        m_writer.Release();
        return false;   
    }

    m_audioTime = m_videoTime = 0;
    m_isopen = true;
    m_filename = p_filename;

    return true;
}

//
// Name :         CMovieMake::ObtainProfile()
// Description :  Obtain a profile from a file.  The file name is specified in m_profileName 
//                and defaults to profile.pcx
//

bool CMovieMake::ObtainProfile()
{
    
    //
    // Obtain a profile manager.
    //

    CComPtr<IWMProfileManager> profilemanager;
    HRESULT r = WMCreateProfileManager(&profilemanager);
    if(r != S_OK)
    {
        AfxMessageBox(TEXT("Failed to create a profile manager for video output"));
        return false;
    }

    //
    // Open the profile file
    //
    HANDLE              hFile = INVALID_HANDLE_VALUE;
    hFile = CreateFileW( m_profileName.c_str(), 
                         GENERIC_READ, 
                         FILE_SHARE_READ, 
                         NULL, 
                         OPEN_EXISTING, 
                         FILE_ATTRIBUTE_NORMAL, 
                         NULL );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        AfxMessageBox(TEXT("Unable to open profile file."));
        return false;
    }

    DWORD dwLength = GetFileSize( hFile, NULL );
    if( -1 == dwLength )
    {
        AfxMessageBox(TEXT("Unable to read profile file."));
        return false;
    }

    //
    // Allocate memory for profile buffer
    //
    WCHAR *pProfile = (WCHAR *) new BYTE[ dwLength + sizeof(WCHAR) ];

    // The buffer must be NULL terminated.
    memset( pProfile, 0, dwLength + sizeof(WCHAR) );

    //
    // Read the profile to a buffer
    //
    DWORD dwBytesRead;
    if( !ReadFile( hFile, pProfile, dwLength, &dwBytesRead, NULL ) )
    {
        delete pProfile;
        CloseHandle(hFile);
        AfxMessageBox(TEXT("Unable to read profile file."));
        return false;
    }

    CloseHandle(hFile);

    //
    // Load the profile from the buffer
    //
    HRESULT hr = profilemanager->LoadProfileByData( pProfile, &m_profile );
    if(hr != S_OK)
    {
        delete pProfile;
        AfxMessageBox(TEXT("Unable to read profile data."));
    }

    // Release resources
    delete pProfile;

    //
    // Now determine some information about the streams in this profile.
    //

    DWORD streamcnt;
    m_profile->GetStreamCount(&streamcnt);
    for(int s=0; s<(int)streamcnt;  s++)
    {
        CComPtr<IWMStreamConfig> sconfig;
        m_profile->GetStream(s, &sconfig);

        // Obtain the media property data for the stream.
        CComPtr<IWMMediaProps> mediaprops;
        hr = sconfig->QueryInterface( IID_IWMMediaProps, (void **)&mediaprops );
        if(hr != S_OK)
            continue;

        std::vector<BYTE> mediatypeB;
        DWORD smediatype = 0;
        mediaprops->GetMediaType(NULL, &smediatype);    // Obtain space required

        mediatypeB.resize(smediatype);
        mediaprops->GetMediaType((WM_MEDIA_TYPE *)&mediatypeB[0], &smediatype);  // Get data

        WM_MEDIA_TYPE *pMediatype = (WM_MEDIA_TYPE *)&mediatypeB[0];

        // What type of stream is this?
        if(pMediatype->majortype == WMMEDIATYPE_Video)
        {
            if(m_videostream >= 0)
                continue;               // Only use first video stream
            m_videostream = s;

            WMVIDEOINFOHEADER *info = (WMVIDEOINFOHEADER *)pMediatype->pbFormat;
            m_width = info->bmiHeader.biWidth;
            m_height = info->bmiHeader.biHeight;
            m_fps = 10000000. / info->AvgTimePerFrame;
            m_spf = 1. / m_fps;
        }
        else if(pMediatype->majortype == WMMEDIATYPE_Audio)
        {
            if(m_audiostream >= 0)
                continue;               // Only use first audio stream
            m_audiostream = s;

            WAVEFORMATEX *info = (WAVEFORMATEX *)pMediatype->pbFormat;
            m_numChannels = info->nChannels;
            m_sampleRate = info->nSamplesPerSec;
            m_samplePeriod = 1. / double(m_sampleRate);
            m_bitsPerSample = info->wBitsPerSample;
        }

    }


    return true;
}


//! Close the video file when we are done writing to it.
bool CMovieMake::Close(void)
{
    if(!m_isopen)
        return true;

    HRESULT r = m_writer->Flush();
    r = m_writer->EndWriting();

    m_writer.Release();
    m_profile.Release();
    m_isopen = false;

    return true;
}

//! \brief Add an image to the video file.
//! \param p_image Image source to write
//! \returns true if successful
bool CMovieMake::WriteImage(const CGrImage &p_image)
{
    if(!m_isopen)
        return false;

    assert(p_image.GetWidth() == m_width && p_image.GetHeight() == m_height);
    return WriteImage(p_image.GetRow(0));
}

//! \brief Add an image to the video file.
//! \param p_image Image source to write as byte array
//! \returns true if successful
bool CMovieMake::WriteImage(const BYTE *p_image)
{
    if(!m_isopen)
        return false;

    if(m_videostream < 0)
        return false;

    BYTE *buffer;
    CComPtr<INSSBuffer> videoSample;

    int samplesize = m_width * m_height * 3;
    m_writer->AllocateSample(samplesize, &videoSample);
    videoSample->GetBuffer(&buffer);

    memcpy(buffer, p_image, samplesize);

    HRESULT r = m_writer->WriteSample(m_videostream, QWORD(m_videoTime * 10000000.), 0, videoSample);
    assert(r == S_OK);

    m_videoTime += m_spf;

    videoSample.Release();

    return true;
}



//!\brief   Write audio frames to the output video file.
//! \param p_audio Audio sample frames to write
//! \returns true if successful
bool CMovieMake::WriteAudio(const std::vector<short> &p_audio)
{
    return WriteAudio(&p_audio[0], int(p_audio.size()) / m_numChannels);
}

//! \brief  Write audio frames to the output video file.
//! \param p_audio Audio sample frames to write
//! \param p_framecnt Number of audio frames to write
//! \returns true if successful
bool CMovieMake::WriteAudio(const short * p_audio, int p_framecnt)
{
    if(!m_isopen)
        return false;

    if(m_audiostream < 0)
        return false;

    CComPtr<INSSBuffer> audioSample;
    BYTE *buffer;

    m_writer->AllocateSample(p_framecnt * 2 * m_numChannels, &audioSample);
    audioSample->GetBuffer(&buffer);

    memcpy(buffer, p_audio, p_framecnt * 2 * m_numChannels);

    HRESULT r = m_writer->WriteSample(m_audiostream, QWORD(m_audioTime * 10000000.), 0, audioSample);
    assert(r == S_OK);

    m_audioTime += p_framecnt * m_samplePeriod;

    audioSample.Release();

    return true;
}
