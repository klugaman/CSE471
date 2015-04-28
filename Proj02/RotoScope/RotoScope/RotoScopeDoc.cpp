// RotoScopeDoc.cpp : implementation of the CRotoScopeDoc class
//

#include "stdafx.h"
#include "RotoScope.h"
#include <fstream>
#include "xmlhelp.h"
#include "RotoScopeDoc.h"
#include "graphics/GrImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// Simple inline function to range bound a double and cast to a short.
inline short ShortRange(double a) {return a > 32767 ? 32767 : (a < -32768. ? -32768 : short(a));}


// CRotoScopeDoc

IMPLEMENT_DYNCREATE(CRotoScopeDoc, CDocument)

BEGIN_MESSAGE_MAP(CRotoScopeDoc, CDocument)
    ON_COMMAND(ID_MOVIES_OPENSOURCEMOVIE, &CRotoScopeDoc::OnMoviesOpensourcemovie)
    ON_COMMAND(ID_MOVIES_OPENOUTPUTMOVIE, &CRotoScopeDoc::OnMoviesOpenoutputmovie)
    ON_COMMAND(ID_FRAMES_CREATEONEFRAME, &CRotoScopeDoc::OnFramesCreateoneframe)
    ON_COMMAND(ID_FRAMES_WRITEONEFRAME, &CRotoScopeDoc::OnFramesWriteoneframe)
    ON_UPDATE_COMMAND_UI(ID_FRAMES_WRITEONEFRAME, &CRotoScopeDoc::OnUpdateFramesWriteoneframe)
    ON_COMMAND(ID_MOVIES_CLOSESOURCEMOVIE, &CRotoScopeDoc::OnMoviesClosesourcemovie)
    ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSESOURCEMOVIE, &CRotoScopeDoc::OnUpdateMoviesClosesourcemovie)
    ON_COMMAND(ID_MOVIES_CLOSEOUTPUTMOVIE, &CRotoScopeDoc::OnMoviesCloseoutputmovie)
    ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSEOUTPUTMOVIE, &CRotoScopeDoc::OnUpdateMoviesCloseoutputmovie)
    ON_COMMAND(ID_FRAMES_WRITETHENCREATEONEFRAME, &CRotoScopeDoc::OnFramesWritethencreateoneframe)
    ON_UPDATE_COMMAND_UI(ID_FRAMES_WRITETHENCREATEONEFRAME, &CRotoScopeDoc::OnUpdateFramesWritethencreateoneframe)
    ON_COMMAND(ID_FRAMES_WRITETHENCREATEONESECOND, &CRotoScopeDoc::OnFramesWritethencreateonesecond)
    ON_COMMAND(ID_FRAMES_WRITETHENCREATEREMAINING, &CRotoScopeDoc::OnFramesWritethencreateremaining)
    ON_COMMAND(ID_MOVIES_OPENBACKGROUNDAUDIO, &CRotoScopeDoc::OnMoviesOpenbackgroundaudio)
    ON_COMMAND(ID_MOVIES_CLOSEBACKGROUNDAUDIO, &CRotoScopeDoc::OnMoviesClosebackgroundaudio)
    ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSEBACKGROUNDAUDIO, &CRotoScopeDoc::OnUpdateMoviesClosebackgroundaudio)
	ON_COMMAND(ID_EDIT_CLEARFRAME, &CRotoScopeDoc::OnEditClearframe)
	ON_COMMAND(ID_LIGHTSABER_RED, &CRotoScopeDoc::OnLightsaberRed)
	ON_COMMAND(ID_TASKS_ROTATION, &CRotoScopeDoc::OnTasksRotation)
	ON_COMMAND(ID_TASKS_BIRD, &CRotoScopeDoc::OnTasksBird)
	ON_COMMAND(ID_EDIT_ROTATE, &CRotoScopeDoc::OnEditRotate)

	ON_COMMAND(ID_ROTOSCOPING_REPLACEHEAD, &CRotoScopeDoc::OnRotoscopingReplacehead)
	ON_COMMAND(ID_ROTOSCOPING_SPLASH, &CRotoScopeDoc::OnRotoscopingSplash)
	ON_COMMAND(ID_ROTOSCOPING_REPLACEBACKGROUND, &CRotoScopeDoc::OnRotoscopingReplacebackground)
END_MESSAGE_MAP()


//! Constructor for the document class.  
CRotoScopeDoc::CRotoScopeDoc()
{
    ::CoInitialize(NULL);

    // Set the image size to an initial default value and black.
	m_image.SetSize(640, 480);
    m_image.Fill(0, 0, 0);
	m_movieframe = 0; 

	// Default color and width of the light saber
	m_rintensity = 255;
	m_gintensity = 255;
	m_bintensity = 255;
	m_linewidth = 1;

	// Load the bird image
	m_bird.LoadFile(L"bird.png");
	m_head.LoadFile(L"head.png");
	m_background.LoadFile(L"background.jpg");
	m_placebird = false;

	// codes added after this line were added for the project 2
	//m_moviemake.SetProfileName(L"profile720p.prx");
}

//! Destructor
CRotoScopeDoc::~CRotoScopeDoc()
{
    ::CoUninitialize();
}


//! Function that is called when a new document is created
//! \returns true if successful
BOOL CRotoScopeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



// CRotoScopeDoc diagnostics

#ifdef _DEBUG
void CRotoScopeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRotoScopeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Source movie management
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :         CRotoScopeDoc::OnMoviesOpensourcemovie()
// Description :  Open a video input source.
//

void CRotoScopeDoc::OnMoviesOpensourcemovie()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Video Files (*.avi;*.wmv;*.asf)|*.avi; *.wmv; *.asf|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".avi"), NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
        return;

    if(!m_moviesource.Open(dlg.GetPathName()))
        return;
}

void CRotoScopeDoc::OnMoviesClosesourcemovie() { m_moviesource.Close(); }
void CRotoScopeDoc::OnUpdateMoviesClosesourcemovie(CCmdUI *pCmdUI) { pCmdUI->Enable(m_moviesource.IsOpen()); }


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Background music management
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Name :        CRotoScopeDoc::OnMoviesOpenbackgroundaudio()
// Description : Opens an audio file we can use as a background music source.
//

void CRotoScopeDoc::OnMoviesOpenbackgroundaudio()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Audio Files (*.wav;*.mp3)|*.wav; *.mp3|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".wav"), NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
        return;

    if(!m_backaudio.Open(dlg.GetPathName()))
        return;
}

void CRotoScopeDoc::OnMoviesClosebackgroundaudio() { m_backaudio.Close(); }
void CRotoScopeDoc::OnUpdateMoviesClosebackgroundaudio(CCmdUI *pCmdUI) { pCmdUI->Enable(m_backaudio.IsOpen()); }


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Output movie management
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :        CRotoScopeDoc::OnMoviesOpenoutputmovie()
// Description : Open an output movie.
//

void CRotoScopeDoc::OnMoviesOpenoutputmovie()
{
	m_movieframe = 0;

	static TCHAR BASED_CODE szFilter[] = TEXT("ASF Files (*.asf)|*.asf|All Files (*.*)|*.*||");

	CFileDialog dlg(FALSE, TEXT(".asf"), NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
        return;

    if(!m_moviemake.Open(dlg.GetPathName()))
        return;

    // Make CImage match the size of the output movie.
    m_image.SetSize(m_moviemake.GetWidth(), m_moviemake.GetHeight());

    // Make the audio buffer match the storage requirement for one video frame
    m_audio.clear();
    m_audio.resize( size_t(m_moviemake.GetSampleRate() / m_moviemake.GetFPS()) * m_moviemake.GetNumChannels() );

    UpdateAllViews(NULL);
}


void CRotoScopeDoc::OnMoviesCloseoutputmovie() { m_moviemake.Close(); }
void CRotoScopeDoc::OnUpdateMoviesCloseoutputmovie(CCmdUI *pCmdUI) { pCmdUI->Enable(m_moviemake.IsOpen()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Creating and writing video frames.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :         CRotoScopeDoc::OnFramesCreateoneframe()
// Description :  Menu handler for Frame/Create One Frame menu option.
//                This will call the function that creates one frame.
//

void CRotoScopeDoc::OnFramesCreateoneframe()
{
    CreateOneFrame();
    UpdateAllViews(NULL);
}


//
// Name :        CRotoScopeDoc::CreateOneFrame()
// Description : This function creates a frame for display.
//               This is a demonstration of how we can read frames and audio 
//               and create an output frame.
//

void CRotoScopeDoc::CreateOneFrame()
{
    //
    // Clear our frame first
    //

    m_image.Fill(0, 0, 0);

    //
    // Read any image from source video?
    //

    if(m_moviesource.HasVideo())
    {
        // Important:  Don't read directly into m_image.  Our source may be a 
        // different size!  I'm reading into a temporary image, then copying
        // the data over.

        /*CGrImage image;*/
        if(m_moviesource.ReadImage(m_initial))
        {
			DrawImage();
            // Write this into m_image
            /*for(int r=0;  r<m_image.GetHeight() && r<image.GetHeight();  r++)
            {
                for(int c=0;  c<m_image.GetWidth() && c<image.GetWidth();  c++)
                {
                    m_image[r][c*3] = image[r][c*3];
                    m_image[r][c*3+1] = image[r][c*3+1];
                    m_image[r][c*3+2] = image[r][c*3+2];
                }
            }*/

			// Save a copy of the original, unmodified image
			/*m_initial = m_image;*/

			// Write any saved drawings into the frame
			/*if (m_movieframe < (int)m_draw.size())
			{
				for (list<CPoint>::iterator i = m_draw[m_movieframe].begin();
					i != m_draw[m_movieframe].end();  i++)
				{
					m_image.Set(i->x, i->y, 255, 0, 0);
				}
			}*/

        }

    }

    //
    // Read any audio from the source video?  Note that we read and write the 
    // audio associated with one frame of video.
    //

    std::vector<short> audio;
    if(m_moviesource.HasAudio() && m_moviesource.ReadAudio(audio))
    {
        // The problem is that the input audio may not be in the same format
        // as the output audio.  For example, we may have a different number of 
        // audio frames for a given video frame.  Also, the channels may be
        // different.  I'll assume my output is stereo here, since I created a
        // stereo profile, but the input may be mono.

        if(m_moviesource.GetNumChannels() == 2)
        {
            // Easiest, both are the same.
            // What's the ratio of playback?
            double playrate = double(audio.size()) / double(m_audio.size());
            for(unsigned f=0;  f<m_audio.size() / 2;  f++)
            {
                int srcframe = int(playrate * f);
                m_audio[f*2] = audio[srcframe*2];
                m_audio[f*2+1] = audio[srcframe*2+1];
            }
        }
        else
        {
            // Mono into stereo
            double playrate = double(2. * audio.size()) / double(m_audio.size());
            for(unsigned f=0;  f<m_audio.size() / 2;  f++)
            {
                int srcframe = int(playrate * f);
                m_audio[f*2] = audio[srcframe];
                m_audio[f*2+1] = audio[srcframe];
            }
        }

    }
    else
    {
        // If there is no audio to read, set to silence.
        for(unsigned int i=0;  i<m_audio.size();  i++)
            m_audio[i] = 0;
    }

    //
    // Is there any background audio to mix in?
    //

    if(m_backaudio.IsOpen())
    {
        for(std::vector<short>::iterator i=m_audio.begin();  i!=m_audio.end();  )
        {
            short audio[2];
            m_backaudio.ReadStereoFrame(audio);

            *i = ShortRange(*i + audio[0] * 0.3);
            i++;
            *i = ShortRange(*i + audio[1] * 0.3);
            i++;
        }
    }
}

//
// Most of the following are various menu options for video processing designed to make
// the user interface easier.
//

void CRotoScopeDoc::OnFramesWriteoneframe()
{
    m_moviemake.WriteImage(m_image);
    m_moviemake.WriteAudio(m_audio);
	m_movieframe++;
}

void CRotoScopeDoc::OnUpdateFramesWriteoneframe(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_moviemake.IsOpen());
}


void CRotoScopeDoc::OnFramesWritethencreateoneframe()
{
    OnFramesWriteoneframe();
    OnFramesCreateoneframe();
}

void CRotoScopeDoc::OnUpdateFramesWritethencreateoneframe(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_moviemake.IsOpen());
}

void CRotoScopeDoc::OnFramesWritethencreateonesecond()
{
    if(m_moviesource.IsOpen() && !m_moviesource.HasAudio())
        return;

    // Do the creation operation for one entire second
    for(int i=0;  i<int(m_moviemake.GetFPS() + 0.5);  i++)
    {
        OnFramesWriteoneframe();
        OnFramesCreateoneframe();
        if(m_moviesource.IsOpen() && !m_moviesource.HasVideo())
            break;

        MessagePump();
    }
}

//
// Name :        CRotoScopeDoc::OnFramesWritethencreateremaining()
// Description : This is an example of a loop that will consume all remaining 
//               source video.
//

void CRotoScopeDoc::OnFramesWritethencreateremaining()
{
    // This only makes sense if there's an input video
    if(!m_moviesource.IsOpen() || !m_moviesource.HasAudio())
        return;

    // Do the creation operation for one entire second
    for(int i=0;  ;  i++)
    {
        OnFramesWriteoneframe();
        OnFramesCreateoneframe();
        if(!m_moviesource.HasVideo())
            break;

        MessagePump();
    }

    AfxMessageBox(TEXT("All Done"));
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// The mouse-based user interface.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//! This function is called by the view class when the mouse is
//! pressed or moved on the screen.
/*! It sets the associated pixel in the image to red right now. You'll 
 * likely make it do something different
 * \param p_x X location in image
 * \param p_y Y location in image */
void CRotoScopeDoc::Mouse(int p_x, int p_y)
{
    // We need to convert screen locations to image locations
    /*int x = p_x;                            // No problem there.
    int y = m_image.GetHeight() - p_y - 1;     // Just invert it.

    m_image.Set(x, y, 255, 0, 0);       // Note:  Set does error checking on x,y for us.
    m_image.Set(x+1, y, 255, 0, 0);       // Note:  Set does error checking on x,y for us.
    m_image.Set(x+1, y+1, 255, 0, 0);       // Note:  Set does error checking on x,y for us.
    m_image.Set(x, y+1, 255, 0, 0);       // Note:  Set does error checking on x,y for us.

	// Ensure there is an entry for every frame up till this one...
	std::list<CPoint> empty;
	while ((int)m_draw.size() < m_movieframe + 1)
		m_draw.push_back(empty);

	// Add the mouse point to the list for the frame
	m_draw[m_movieframe].push_back(CPoint(x, y));

    UpdateAllViews(NULL);*/
	// We need to convert screen locations to image locations
	int x = p_x;                            // No problem there.
	int y = m_image.GetHeight() - p_y - 1;     // Just invert it.

	// Ensure there is an entry for every frame up till this one...
	std::list<CPoint> empty;
	while ((int)m_draw.size() < m_movieframe + 1)
		m_draw.push_back(empty);

	if (!m_placebird)
	{
		// Add the mouse point to the list for the frame
		m_draw[m_movieframe].push_back(CPoint(x, y));
	}

	DrawImage();
	//if (m_placebird)
	//{
	//	PlaceBird(y, x);
	//}
}

void CRotoScopeDoc::DrawLine(CGrImage &image, int x1, int y1, int x2, int y2)
{
	if (abs(x2 - x1) > abs(y2 - y1))
	{
		// Step in the X direction...
		if (x1 > x2)
		{
			swap(x1, x2);
			swap(y1, y2);
		}
		if (x1 == x2)
			image.Set(x1, y1, m_rintensity, m_gintensity, m_bintensity);
		else
		{
			for (int x = x1; x <= x2; x++)
			{
				image.Set(x, y1 + (x - x1) * (y2 - y1) / (x2 - x1), m_rintensity, m_gintensity, m_bintensity);
			}
		}
	}
	else
	{
		// Step in the Y direction...
		if (y1 > y2)
		{
			swap(x1, x2);
			swap(y1, y2);
		}
		if (y1 == y2)
			image.Set(x1, y1, m_rintensity, m_gintensity, m_bintensity);
		else
		{
			for (int y = y1; y <= y2; y++)
			{
				image.Set(x1 + (y - y1) * (x2 - x1) / (y2 - y1), y, m_rintensity, m_gintensity, m_bintensity);
			}
		}
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utility functions...
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Name :        CRotoScopeDoc::MessagePump(void)
// Description : This function is a Windows message pump. It allows message processing to 
//               proceed while your program is in some loop.  This is so the menus and such
//               will continue to work.  Be very careful.  Usually you pop up a modal 
//               dialog box while this is going on to prevent accessing anything other than
//               a cancel button.  I'm not doing that, so be careful about what you hit.
//

void CRotoScopeDoc::MessagePump(void)
{
    // Allow any messages to be processed
    MSG msg;
    while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) 
    {
            AfxGetThread()->PumpMessage();
    }
}



BOOL CRotoScopeDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	//
	// Create an XML document
	//

	// CComPtr<> is a "smart" pointer. It takes care of reference counting on COM objects for
	// us so they know when to delete themselves.
	CComPtr<IXMLDOMDocument>  xmlDoc;
	HRESULT hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&xmlDoc);
	if (hr != S_OK)
	{
		AfxMessageBox(L"Unable to create XML document");
		return false;
	}

	// This is a pointer we'll use for nodes in our XML
	CComPtr<IXMLDOMNode> node;

	// Create a processing instruction targeted for xml.
	// This creates the line:  <?xml version='1.0'>
	CComPtr<IXMLDOMProcessingInstruction> pi;
	xmlDoc->createProcessingInstruction(L"xml", L"version='1.0'", &pi);
	xmlDoc->appendChild(pi, &node);
	pi.Release();
	node.Release();

	// Create the root element
	CComPtr<IXMLDOMElement> pe;
	xmlDoc->createElement(L"movie", &pe);
	xmlDoc->appendChild(pe, &node);
	pe.Release();

	SaveMovieData(xmlDoc, node);

	node.Release();

	hr = xmlDoc->save(CComVariant(lpszPathName));
	xmlDoc.Release();


	return TRUE;
}


void CRotoScopeDoc::SaveMovieData(IXMLDOMDocument *xmlDoc, IXMLDOMNode *inNode)
{
	for (int frame = 0; frame < (int)m_draw.size(); frame++)
	{
		// Create an XML node for the frame
		CComPtr<IXMLDOMNode> node;
		CComPtr<IXMLDOMElement> pe;
		xmlDoc->createElement(L"frame", &pe);

		// Add an attribute for the frame number
		pe->setAttribute(L"num", CComVariant(frame));

		// Append the node to the node we are nested inside.
		inNode->appendChild(pe, &node);
		pe.Release();

		//
		// Now save the point data for the frame
		//

		for (std::list<CPoint>::iterator p = m_draw[frame].begin(); p != m_draw[frame].end(); p++)
		{
			// Create an XML node for the point
			xmlDoc->createElement(L"point", &pe);

			// Add attributes for the point
			pe->setAttribute(L"x", CComVariant(p->x));
			pe->setAttribute(L"y", CComVariant(p->y));

			// Append the node to the node we are nested inside.
			CComPtr<IXMLDOMNode> nodePoint;
			node->appendChild(pe, &nodePoint);
			pe.Release();
			nodePoint.Release();
		}

		// When done, release <frame> the node
		node.Release();
	}
}



BOOL CRotoScopeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// Clear any previous frame information
	m_draw.clear();

	//
	// Create an XML document
	//

	CComPtr<IXMLDOMDocument>  pXMLDoc;
	bool succeeded = SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
		IID_IXMLDOMDocument, (void**)&pXMLDoc));
	if (!succeeded)
	{
		AfxMessageBox(L"Failed to create an XML document to use");
		return FALSE;
	}

	// Open the XML document
	VARIANT_BOOL ok;
	succeeded = SUCCEEDED(pXMLDoc->load(CComVariant(lpszPathName), &ok));
	if (!succeeded || ok == VARIANT_FALSE)
	{
		AfxMessageBox(L"Failed to open XML score file");
		return FALSE;
	}

	//
	// Traverse the XML document in memory!!!!
	// Top level tag is <score>
	//

	CComPtr<IXMLDOMNode> node;
	pXMLDoc->get_firstChild(&node);
	for (; node != NULL; NextNode(node))
	{
		// Get the name of the node
		CComBSTR nodeName;
		node->get_nodeName(&nodeName);

		if (nodeName == L"movie")
		{
			XmlLoadMovie(node);
		}
	}


	return TRUE;
}
void CRotoScopeDoc::XmlLoadMovie(IXMLDOMNode *xml)
{
	// Handle the children of a <movie> tag
	CComPtr<IXMLDOMNode> node;
	xml->get_firstChild(&node);
	for (; node != NULL; NextNode(node))
	{
		// Get the name of the node
		CComBSTR nodeName;
		node->get_nodeName(&nodeName);

		if (nodeName == L"frame")
		{
			XmlLoadFrame(node);
		}
	}
}

void CRotoScopeDoc::XmlLoadFrame(IXMLDOMNode *xml)
{
	// When this function is called we have a new <frame> tag.
	// We assume we don't skip any tag numbers.
	// Push on an empty frame
	list<CPoint> empty;
	m_draw.push_back(empty);
	m_drawlines.push_back(empty);

	// Traverse the children of the <frame> tag
	CComPtr<IXMLDOMNode> node;
	xml->get_firstChild(&node);
	for (; node != NULL; NextNode(node))
	{
		// Get the name of the node
		CComBSTR nodeName;
		node->get_nodeName(&nodeName);

		// Handle finding a nested <point> tag
		if (nodeName == L"point")
		{
			CPoint point;

			// Get a list of all attribute nodes and the
			// length of that list
			CComPtr<IXMLDOMNamedNodeMap> attributes;
			node->get_attributes(&attributes);
			long len;
			attributes->get_length(&len);

			// Loop over the list of attributes
			for (int i = 0; i<len; i++)
			{
				// Get attribute i
				CComPtr<IXMLDOMNode> attrib;
				attributes->get_item(i, &attrib);

				// Get the name of the attribute
				CComBSTR name;
				attrib->get_nodeName(&name);

				// Get the value of the attribute.  A CComVariant is a variable
				// that can have any type. It loads the attribute value as a
				// string (UNICODE), but we can then change it to an integer 
				// (VT_I4) or double (VT_R8) using the ChangeType function 
				// and then read its integer or double value from a member variable.
				CComVariant value;
				attrib->get_nodeValue(&value);

				if (name == "x")
				{
					value.ChangeType(VT_I4);
					point.x = value.intVal;
				}
				else if (name == "y")
				{
					value.ChangeType(VT_I4);
					point.y = value.intVal;
				}
			}

			// When we've pulled the x,y values from the
			// tag, push it onto the end of our list of 
			// points.
			m_draw.back().push_back(point);
		}
	}
}


void CRotoScopeDoc::OnEditClearframe()
{
	if (m_movieframe >= 0 &&
		m_movieframe < int(m_draw.size()))
		m_draw[m_movieframe].clear();
	m_splashVideo = false;

	DrawImage();
	UpdateAllViews(NULL);
}
void CRotoScopeDoc::DrawImage()
{
	// Write image from m_initial into the current image
	for (int r = 0; r<m_image.GetHeight() && r<m_initial.GetHeight(); r++)
	{
		for (int c = 0; c<m_image.GetWidth() && c<m_initial.GetWidth(); c++)
		{
			m_image[r][c * 3] = m_initial[r][c * 3];
			m_image[r][c * 3 + 1] = m_initial[r][c * 3 + 1];
			m_image[r][c * 3 + 2] = m_initial[r][c * 3 + 2];
		}
	}

	if (m_movieframe < (int)m_draw.size())
	{
		for (list<CPoint>::iterator i = m_draw[m_movieframe].begin();
			i != m_draw[m_movieframe].end();  i++)
		{
			m_image.Set(i->x, i->y, 255, 0, 0);
		}
	}

	if (m_movieframe < (int)m_drawlines.size())
	{
		CPoint p1 = m_drawlines[m_movieframe].front();
		CPoint p2 = m_drawlines[m_movieframe].back();
		m_image.DrawLine(p1.x, p1.y, p2.x, p2.y, 255, 0, 0);
		m_image.DrawLine(p1.x + 1, p1.y + 1, p2.x + 1, p2.y + 1, 255, 0, 0);
	}

	CGrImage temp = m_image;

	if (m_changeBackground == true)
	{
		for (int r = 0; r < m_image.GetHeight() && r < m_initial.GetHeight(); r++)
		{
			for (int c = 0; c < m_image.GetWidth() && c < m_initial.GetWidth(); c++)
			{
				if (r > 460 || r < 25 || (c > m_image.GetWidth() - 65) || c < 80)
				{
					m_image[r][c * 3] = 0;
					m_image[r][c * 3 + 1] = 255;
					m_image[r][c * 3 + 2] = 0;
				}
			}
		}
		ChangeBackground();
	}
	

	if (m_switchHead == true){

		CPoint head_pos = TrackHair(temp);

		for (int r = 0; r < m_head.GetHeight(); r++)
		{
			for (int c = 0; c < m_head.GetWidth(); c++)
			{
				auto adjust = m_head.GetWidth() / 2;

				auto row = r + head_pos.y - adjust + 5;
				auto col = c + head_pos.x - adjust - 10;

				if (m_head[r][c * 4 + 3] >= 192)
				{
					if (row >= 0 && col >= 0 && row < m_image.GetHeight() && col < m_image.GetWidth())
					{
						m_image[row][col * 3 + 0] = m_head[r][c * 4 + 0];
						m_image[row][col * 3 + 1] = m_head[r][c * 4 + 1];
						m_image[row][col * 3 + 2] = m_head[r][c * 4 + 2];
					}
				}
			}
		}
	}

	if (m_splashVideo == true)
	{
		SplashEffect(m_image);
	}
	

	UpdateAllViews(NULL);
}


void CRotoScopeDoc::OnLightsaberRed()
{
	m_rintensity = 255;
	m_gintensity = 0;
	m_bintensity = 0;
	m_linewidth = 2;
	// Do the creation operation for one entire second
	for (int i = 0;; i++)
	{
		OnFramesCreateoneframe();
		OnFramesWriteoneframe();
		if (!m_moviesource.HasVideo())
			break;

		MessagePump();
	}

	AfxMessageBox(TEXT("All Done"));
}

void CRotoScopeDoc::RotateFrame(double angle) 
{
	CGrImage temp;
	temp.SetSameSize(m_image);
	int height = temp.GetHeight();
	int width = temp.GetWidth();
	angle = (2 * 3.1415 * angle) / 360;
	for (int r = 0; r < temp.GetHeight(); r++)
	{
		for (int c = 0; c < temp.GetWidth(); c++)
		{
			// Translation
			int source_c = (c - width / 2);
			int source_r = (r - height / 2);
			// Rotation
			int t_c = source_c * cos(angle) + source_r * sin(angle);
			int t_r = - source_c * sin(angle) + source_r * cos(angle);
			// Translation 
			source_c = (t_c + width / 2);
			source_r = (t_r + height / 2);
			
			if (source_r > 0 && source_c > 0 && source_c < temp.GetWidth() && source_r < temp.GetHeight())
			{
				temp[r][c * 3] = m_image[source_r][source_c * 3];
				temp[r][c * 3 + 1] = m_image[source_r][source_c * 3 + 1];
				temp[r][c * 3 + 2] = m_image[source_r][source_c * 3 + 2];
			}
		}
	}
	m_image = temp;
}
void CRotoScopeDoc::OnTasksRotation()
{
	m_rintensity = 0;
	m_gintensity = 255;
	m_bintensity = 0;
	m_linewidth = 3;
	double rotation_angle = 360 / 30;
	// Do the creation operation for one entire second
	OnFramesCreateoneframe();
	for (int i = 0;; i++)
	{
		RotateFrame(rotation_angle *(i+1));
		OnFramesWriteoneframe();
		OnFramesCreateoneframe();
		if (!m_moviesource.HasVideo())
			break;

		MessagePump();
	}

	AfxMessageBox(TEXT("All Done"));
}

void CRotoScopeDoc::PlaceBird(int position_r, int position_c)
{
	for (int r = 0; r<m_bird.GetHeight(); r++)
	{
		for (int c = 0; c<m_bird.GetWidth(); c++)
		{
			if (m_bird[r][c * 4 + 3] >= 192)
			{
				if (r + position_r < m_image.GetHeight() && c + position_c - m_bird.GetWidth() / 2 < m_image.GetWidth())
				{
					m_image[r + position_r][(c + position_c - m_bird.GetWidth() / 2) * 3] = m_bird[r][c * 4];
					m_image[r + position_r][(c + position_c - m_bird.GetWidth() / 2) * 3 + 1] = m_bird[r][c * 4 + 1];
					m_image[r + position_r][(c + position_c - m_bird.GetWidth() / 2) * 3 + 2] = m_bird[r][c * 4 + 2];
				}
			}
		}
	}
	UpdateAllViews(NULL);
}
void CRotoScopeDoc::OnTasksBird()
{
	m_placebird = true;
}


void CRotoScopeDoc::OnEditRotate()
{
	// TODO: Add your command handler code here
	m_rintensity = 0;
	m_gintensity = 255;
	m_bintensity = 0;
	m_linewidth = 3;
	double rotation_angle = 360 / 30;
	// Do the creation operation for one entire second
	OnFramesCreateoneframe();
	for (int i = 0;; i++)
	{
		RotateFrame(rotation_angle *(i + 1));
		OnFramesWriteoneframe();
		OnFramesCreateoneframe();
		if (!m_moviesource.HasVideo())
			break;

		MessagePump();
	}

	AfxMessageBox(TEXT("All Done"));
}

CPoint CRotoScopeDoc::Mapping(int x, int y)
{
	int imageHeight = m_image.GetHeight();
	int imageWidth = m_image.GetWidth();
	CPoint sourcePoint;

	double dc = sqrt(double(x - imageWidth / 2) * (x - imageWidth / 2) +
		(y - imageHeight / 2) * (y - imageHeight / 2));

	double offset = sin(dc / 50. * 6.2830) * 20;

	double dc1 = dc + offset;

	sourcePoint.x = imageWidth / 2 + dc1 / dc * (x - imageWidth / 2);
	sourcePoint.y = imageHeight / 2 + dc1 / dc * (y - imageHeight / 2);

	return sourcePoint;
}

void CRotoScopeDoc::SplashEffect(CGrImage& image)
{
	CGrImage imageApplied;
	imageApplied.SetSameSize(image);

	int imageHeight = imageApplied.GetHeight();
	int imageWidth = imageApplied.GetWidth();

	// Write image from m_initial into the current image
	for (int r = 0; r < imageHeight; r++)
	{
		for (int c = 0; c < imageWidth; c++)
		{
			CPoint mappingPoint = Mapping(c, r);

			if (mappingPoint.y > 0 && mappingPoint.x > 0 && mappingPoint.x < imageWidth && mappingPoint.y < imageHeight)
			{
				imageApplied[r][c * 3] = image[mappingPoint.y][mappingPoint.x * 3];
				imageApplied[r][c * 3 + 1] = image[mappingPoint.y][mappingPoint.x * 3 + 1];
				imageApplied[r][c * 3 + 2] = image[mappingPoint.y][mappingPoint.x * 3 + 2];
			}
		}
	}

	m_image = imageApplied;
}


void CRotoScopeDoc::ChangeBackground()
{
	for (int r = 0; r<m_image.GetHeight(); r++)
	{
		for (int c = 0; c<m_image.GetWidth(); c++)
		{
			double red = m_image[r][c * 3 + 2] / 255.;
			double green = m_image[r][c * 3 + 1] / 255.;
			double blue = m_image[r][c * 3] / 255.;
			double a = (1. - 7.5 * (green - 1 * red));

			if (a < 0.)
				a = 0.;
			else if (a > 1.)
				a = 1.;

			if (green > 1.3 * red)
				green = 1.3 * red;

			m_image[r][c * 3] = BYTE(a * blue * 255. +
				(1. - a) * m_background[r][c * 3]);
			m_image[r][c * 3 + 1] = BYTE(a * green * 255. +
				(1. - a) * m_background[r][c * 3 + 1]);
			m_image[r][c * 3 + 2] = BYTE(a * red * 255. +
				(1. - a) * m_background[r][c * 3 + 2]);
		}
	}

	UpdateAllViews(NULL);
}


CPoint CRotoScopeDoc::TrackHair(CGrImage &image)
{
	for (int r = 400; r != 100; r--)
	{
		for (int c = 300; c <= 500; c++)
		{			
			auto red = image[r][c * 3 + 2];
			auto green = image[r][c * 3 + 1];
			auto blue = image[r][c * 3 + 0];
			auto p = max(blue, max(green, red));
			if (p <= 25)
				return CPoint(c, r);
		}
	}
}

void CRotoScopeDoc::OnRotoscopingReplacehead()
{
	// TODO: Add your command handler code here
	if (m_switchHead == true) {
		m_switchHead = false;
	}
	else {
		m_switchHead = true;
	}
}


void CRotoScopeDoc::OnRotoscopingSplash()
{
	// TODO: Add your command handler code here
	if (m_splashVideo == true) {
		m_splashVideo = false;
	}
	else {
		m_splashVideo = true;
	}
}


void CRotoScopeDoc::OnRotoscopingReplacebackground()
{
	// TODO: Add your command handler code here
	if (m_changeBackground == true) {
		m_changeBackground = false;
	}
	else {
		m_changeBackground = true;
	}
}
