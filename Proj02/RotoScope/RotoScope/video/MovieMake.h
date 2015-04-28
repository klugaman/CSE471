#pragma once

#include <string>
#include <wmsdk.h>
#include "graphics/GrImage.h"

//! Class that is used to create a movie file 
/*! Class that can be used to create a movie file. 
 *
 * The output format is determined by a profile created by the
 * program WMProEdt, the Windows profile editor. Some example profiles 
 * have been provided. The default profile is profile720x480.prx, which 
 * creates 720 by 480 progressive scan output at 29.97 frames per second.
 */
class CMovieMake
{
public:
    CMovieMake(void);
    ~CMovieMake(void);

    bool Open(const wchar_t * p_filename);
    bool Close(void);

    //! \brief Returns true if movie file is open
    //! \returns true if open
    bool IsOpen() const {return m_isopen;}

    bool WriteImage(const CGrImage &p_image);
    bool WriteAudio(const std::vector<short> &p_audio);
    bool WriteAudio(const short * p_audio, int p_framecnt);

    //! Get the current image time.
    /*! This is the time in the stream for the next image to write.
     * \returns Image time */
    double GetImageTime() const {return m_videoTime;}

    //! Get the current audio time.
    /*! This is the time in the stream for the next audio frame block to write.
     * \returns Audio time */
    double GetAudioTime() const {return m_audioTime;}

    /*! \brief Get the number of audio channels
     * \returns Number of audio channels */
    int GetNumChannels() const {return m_numChannels;}

    /*! \brief Get the sample rate
     * \returns Sample rate in samples per second */
    double GetSampleRate() const {return m_sampleRate;}

    /*! \brief Get the average number of bits per audio sample 
     * \returns Bits per audio sample */
    int GetBitPerSample() const {return m_bitsPerSample;}

    /*! \brief Get the width of the output file image 
     * \returns Image width */
    int GetWidth() const {return m_width;}

    /*! \brief Get the height of the output file image
     * \returns Image height */
    int GetHeight() const {return m_height;}

    /*! \brief Get the number of frames per second for the output file
     * \return Frames per second */
    double GetFPS() const {return m_fps;}

    /*! \brief Get the profile name
     * \returns The current profile name. The default profile is profile720x480.prx
     */
    const wchar_t *GetProfileName() const {return m_profileName.c_str();}

    //! Set the profile name
    /*! A new profile name must be set prior to opening the file for writing.
     * \param name New profile name
     */
    void SetProfileName(const wchar_t *name) {m_profileName = name;}

    //! Get the name of the last loaded file
    /*! This will return the name of the last file successfully opened.
     * The value is whatever was passed to the Open function.
     * \returns Filename of last opened file */
    const wchar_t *GetFilename() const {return m_filename.c_str();}

private:
    bool WriteImage(const BYTE *p_image);
    bool ObtainProfile();

    CComPtr<IWMWriter>  m_writer;
    CComPtr<IWMProfile> m_profile;

    std::wstring        m_filename;

    std::wstring        m_profileName;
    double              m_videoTime;
    double              m_audioTime;
    bool                m_isopen;

    int                 m_videostream;
    int                 m_audiostream;

    // Audio stream information
    int                 m_numChannels;
    int                 m_sampleRate;
    double              m_samplePeriod;
    int                 m_bitsPerSample;

    // Video stream information
    int                 m_width;
    int                 m_height;
    double              m_fps;
    double              m_spf;          // Seconds per frame...
};

