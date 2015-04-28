/*! \file RotoScopeDoc.h
 *
 * Header file for the RotoScope document class
 * \author Charles B. Owen
 */

#pragma once

#include <vector>

#include "video/MovieSource.h"
#include "video/MovieMake.h"
#include "audio/DirSoundSource.h"
#include "graphics/GrImage.h"


/*! \mainpage RotoScope Starter Application
 *
 * The Rotoscope application is a simple program that allows you to 
 * load frames of video, modify them in interesting ways, and save 
 * them to a new video file. Rotoscoping is an animation technique where 
 * frames of a movie are advanced one by one and edited by a user. 
 */

//! Rotoscope document class 
/*! RotoScope document class */
class CRotoScopeDoc : public CDocument
{
public:
	CRotoScopeDoc();
	virtual ~CRotoScopeDoc();
	DECLARE_DYNCREATE(CRotoScopeDoc)

	virtual BOOL OnNewDocument();
    void Mouse(int p_x, int p_y);

    /*! Get the image we are editing 
     * \returns Constant pointer to image we are editing */
    const CGrImage &GetImage() const {return m_image;}

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:
    afx_msg void OnMoviesOpensourcemovie();
    afx_msg void OnMoviesOpenoutputmovie();
    afx_msg void OnFramesCreateoneframe();

    void CreateOneFrame();

    CMovieSource        m_moviesource;      // Any source movie
    CMovieMake          m_moviemake;        // Any movie we are outputing to
    CDirSoundSource     m_backaudio;        // Any background audio?
	int m_movieframe;       // Movie frame we are writing
	int m_rintensity;
	int m_gintensity;
	int m_bintensity;
	int m_linewidth;

    CGrImage            m_image;            // Current image being displayed
	CGrImage m_initial;
	CGrImage    m_bird;
	CGrImage m_head;
	CGrImage m_background;
	bool m_placebird;

    std::vector<short>  m_audio;            // Current audio to be written to the output

	std::vector<std::list<CPoint>> m_draw;
	std::vector<std::list<CPoint>> m_drawlines;
    afx_msg void OnFramesWriteoneframe();
    afx_msg void OnUpdateFramesWriteoneframe(CCmdUI *pCmdUI);
    afx_msg void OnMoviesClosesourcemovie();
    afx_msg void OnUpdateMoviesClosesourcemovie(CCmdUI *pCmdUI);
    afx_msg void OnMoviesCloseoutputmovie();
    afx_msg void OnUpdateMoviesCloseoutputmovie(CCmdUI *pCmdUI);
    afx_msg void OnFramesWritethencreateoneframe();
    afx_msg void OnUpdateFramesWritethencreateoneframe(CCmdUI *pCmdUI);
    afx_msg void OnFramesWritethencreateonesecond();
    void MessagePump(void);
    afx_msg void OnFramesWritethencreateremaining();
    afx_msg void OnMoviesOpenbackgroundaudio();
    afx_msg void OnMoviesClosebackgroundaudio();
    afx_msg void OnUpdateMoviesClosebackgroundaudio(CCmdUI *pCmdUI);
	CPoint Mapping(int x, int y);
	void SplashEffect(CGrImage& image);
	CPoint TrackHair(CGrImage &image);
	void ChangeBackground();

	bool m_changeBackground = false;
	bool m_splashVideo = false;
	bool m_switchHead = false;


public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	void CRotoScopeDoc::SaveMovieData(IXMLDOMDocument *xmlDoc, IXMLDOMNode *inNode);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	void CRotoScopeDoc::XmlLoadMovie(IXMLDOMNode *xml);
	void CRotoScopeDoc::XmlLoadFrame(IXMLDOMNode *xml);
	afx_msg void OnEditClearframe();
	void DrawImage();
	void DrawLine(CGrImage &image, int x1, int y1, int x2, int y2);
	afx_msg void OnLightsaberRed();
	afx_msg void OnTasksRotation();
	afx_msg void OnTasksBird();
	void RotateFrame(double angle);
	void PlaceBird(int position_r, int position_c);
	afx_msg void OnEditRotate();
	afx_msg void OnEditWrap();
	afx_msg void OnEditChangeBackground();
	afx_msg void OnRotoscopingReplacehead();
	afx_msg void OnRotoscopingSplash();
	afx_msg void OnRotoscopingReplacebackground();
};


