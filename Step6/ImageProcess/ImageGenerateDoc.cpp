//
// Name :         ImageGenerateDoc.cpp
// Description :  Implementation of the document class for the 
//                ImageProcess application.
//

// Notice:  Do not add any headers before stdafx.h
#include "stdafx.h"

#include "ImageProcess.h"
#include "graphics/GrImage.h"
#include <cmath>

#include <fstream>

#include "ImageGenerateDoc.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CImageGenerateDoc

IMPLEMENT_DYNCREATE(CImageGenerateDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageGenerateDoc, CDocument)
    //{{AFX_MSG_MAP(CImageGenerateDoc)
    ON_COMMAND(ID_MOUSE_DRAW, OnMouseDraw)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_GENERATE_FILLWHITE, &CImageGenerateDoc::OnGenerateFillwhite)
	ON_COMMAND(ID_GENERATE_FILLGREEN, &CImageGenerateDoc::OnGenerateFillgreen)
	ON_COMMAND(ID_GENERATE_FILLLIGHTSKYBLUE, &CImageGenerateDoc::OnGenerateFilllightskyblue)
	ON_COMMAND(ID_GENERATE_GENERATEHORIZONTALGRADIENT, &CImageGenerateDoc::OnGenerateGeneratehorizontalgradient)
	ON_COMMAND(ID_GENERATE_GENERATEVERTICALGRADIENT, &CImageGenerateDoc::OnGenerateGenerateverticalgradient)
	ON_COMMAND(ID_GENERATE_GENERATE, &CImageGenerateDoc::OnGenerateGenerate)
	ON_COMMAND(ID_GENERATE_HORIZONTALLINE, &CImageGenerateDoc::OnGenerateHorizontalline)
	ON_COMMAND(ID_GENERATE_VERTICALLINE, &CImageGenerateDoc::OnGenerateVerticalline)
	ON_COMMAND(ID_GENERATE_DIAGONALLINE, &CImageGenerateDoc::OnGenerateDiagonalline)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageGenerateDoc construction/destruction

CImageGenerateDoc::CImageGenerateDoc()
{
    m_mousemode = 0;     // No mouse mode
    m_clickcnt = 0;
}

CImageGenerateDoc::~CImageGenerateDoc()
{
}

BOOL CImageGenerateDoc::OnNewDocument()
{
    m_image.SetSize(640, 480);
    m_image.Fill(0, 0, 0);
    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CImageGenerateDoc diagnostics

#ifdef _DEBUG
void CImageGenerateDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CImageGenerateDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageGenerateDoc commands

BOOL CImageGenerateDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
    return FALSE;
}

//
//  Name :         CImageGenerateDoc::OnSaveDocument()
//  Description :  Write the image to a BMP file.
//

BOOL CImageGenerateDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
    BeginWaitCursor();
    DeleteContents();

    bool r = m_image.SaveFile(lpszPathName, CGrImage::BMP);
    EndWaitCursor();

    if(!r)
    {
        // Error message on return
        AfxMessageBox(m_image.GetError(), NULL, MB_ICONINFORMATION | MB_OK);
        return FALSE;
    }

    SetModifiedFlag(FALSE);     // start off with unmodified
    return TRUE;
}




//
// Menu option handlers
//


//
// Name :         CImageGenerateDoc::OnGenerateFillwhite()
// Description :  Fill the image with white.
//
void CImageGenerateDoc::OnGenerateFillwhite()
{
    for(int r=0;  r<m_image.GetHeight();  r++)
    {
        // Looping over the columns of the array
        for(int c=0;  c<m_image.GetWidth() * 3;  c++)
        {
            m_image[r][c] = 255;
        }
    }

    UpdateAllViews(NULL);
}



//
// Name :         CImageGenerateDoc::OnMouseDraw() 
// Description :  Set the mouse draw mode.
//

void CImageGenerateDoc::OnMouseDraw() 
{
    m_mousemode = ID_MOUSE_DRAW;
}






/////////////////////////////////////////////////////////////////////
//
// Mouse Handlers
//
/////////////////////////////////////////////////////////////////////


//
// Name :         CImageGenerateDoc::MousePress()
// Description :  This function is called when the mouse is pressed over
//                m_image2.  The x,y coordinate is in the image.
//

void CImageGenerateDoc::MousePress(int x, int y)
{
    // We count the mouse clicks
    m_clickcnt++;

    BeginWaitCursor();

    switch(m_mousemode)
    {
    case ID_MOUSE_DRAW:
        m_image[y][x * 3] = 0;
        m_image[y][x * 3 + 1] = 255;
        m_image[y][x * 3 + 2] = 0;
        break;

    }

    UpdateAllViews(NULL);
    EndWaitCursor();
}


//
// Name :         CImageGenerateDoc::MouseMove()
// Description :  This function is called when the mouse is moved over
//                m_image.  The x,y coordinate is in the image.
//

void CImageGenerateDoc::MouseMove(int x, int y)
{
    BeginWaitCursor();

    switch(m_mousemode)
    {
    case ID_MOUSE_DRAW:
        m_image[y][x * 3] = 0;          // Blue
        m_image[y][x * 3 + 1] = 255;    // Green
        m_image[y][x * 3 + 2] = 255;    // Red
        break;
    }   

    UpdateAllViews(NULL);
    EndWaitCursor();
}


void CImageGenerateDoc::OnGenerateFillgreen()
{
	// TODO: Add your command handler code here
    for(int r=0;  r<m_image.GetHeight();  r++)
    {
        // Looping over the columns of the image
        for(int c=0;  c<m_image.GetWidth();  c++)
        {
            m_image[r][c*3 + 0] = 0;
            m_image[r][c*3 + 1] = 255;
            m_image[r][c*3 + 2] = 0;
        }
    }

    UpdateAllViews(NULL);
}


void CImageGenerateDoc::OnGenerateFilllightskyblue()
{
	// TODO: Add your command handler code here
    for(int r=0;  r<m_image.GetHeight();  r++)
    {
        // Looping over the columns of the image
        for(int c=0;  c<m_image.GetWidth();  c++)
        {
            m_image[r][c*3 + 0] = 250;
            m_image[r][c*3 + 1] = 206;
            m_image[r][c*3 + 2] = 135;
        }
    }

    UpdateAllViews(NULL);
}


void CImageGenerateDoc::OnGenerateGeneratehorizontalgradient()
{
	// TODO: Add your command handler code here
		for(int r=0;  r<m_image.GetHeight();  r++)
    {
        // Looping over the columns of the image
        for(int c=0;  c<m_image.GetWidth();  c++)
        {
            m_image[r][c*3 + 0] = BYTE(float(c) / float(m_image.GetWidth()-1) * 250);
            m_image[r][c*3 + 1] = BYTE(float(c) / float(m_image.GetWidth()-1) * 206);
            m_image[r][c*3 + 2] = BYTE(float(c) / float(m_image.GetWidth()-1) * 135);
        }
    }

    UpdateAllViews(NULL);
}


void CImageGenerateDoc::OnGenerateGenerateverticalgradient()
{
	// TODO: Add your command handler code here
	for(int c=0;  c<m_image.GetWidth();  c++)
    {
        // Looping over the columns of the image
        for(int r=0;  r<m_image.GetHeight();  r++)
        {
            m_image[r][c*3 + 0] = BYTE(float(r) / float(m_image.GetHeight()-1) * 255);
            m_image[r][c*3 + 1] = BYTE(float(r) / float(m_image.GetHeight()-1) * 0);
            m_image[r][c*3 + 2] = BYTE(float(r) / float(m_image.GetHeight()-1) * 0);
        }
    }

    UpdateAllViews(NULL);
}


void CImageGenerateDoc::OnGenerateGenerate()
{
	// TODO: Add your command handler code here
	int r_1 = 255, g_1 = 0, b_1 = 0;
	int r_2 = 0, g_2 = 255, b_2 = 0;
	int c = 0;
	int r = 0;

	while (c < m_image.GetWidth() && r < m_image.GetHeight())
    {
        if(r < m_image.GetHeight()-1)
		{
		    r++;
		}
        else
		{
            c++;
			r=0;
		}
        int i = c+r;
        m_image[r][c*3 + 2] = r_1 + (i * (r_2-r_1) / (m_image.GetWidth()+m_image.GetHeight()));
        m_image[r][c*3 + 1] = g_1 + (i * (g_2-g_1) / (m_image.GetWidth()+m_image.GetHeight()));
        m_image[r][c*3 + 0] = b_1 + (i * (b_2-b_1) / (m_image.GetWidth()+m_image.GetHeight()));
    } 
    UpdateAllViews(NULL);
}


void CImageGenerateDoc::OnGenerateHorizontalline()
{
	// TODO: Add your command handler code here
	int r = 100;
	for ( int c = 0; c < m_image.GetWidth(); c++ )
	{
		m_image[r][c*3 + 0] = 225;
        m_image[r][c*3 + 1] = 228;
        m_image[r][c*3 + 2] = 255;
	}
	UpdateAllViews(NULL);
}


void CImageGenerateDoc::OnGenerateVerticalline()
{
	// TODO: Add your command handler code here
		for(int c=100; c<102; c++)
	{
		for(int r=0;  r<m_image.GetHeight();  r++)
		{
			m_image[r][c*3 + 0] = 225;
			m_image[r][c*3 + 1] = 228;
			m_image[r][c*3 + 2] = 255;
		}
	}
	UpdateAllViews(NULL);
}


void CImageGenerateDoc::OnGenerateDiagonalline()
{
	// TODO: Add your command handler code here
	for(int c=100;  c<=400;  c++)
    {
		 m_image[int(100.+0.33333*(c-100.))][c*3 + 0] = 225;
         m_image[int(100.+0.33333*(c-100.))][c*3 + 1] = 228;
         m_image[int(100.+0.33333*(c-100.))][c*3 + 2] = 255;
    }
    UpdateAllViews(NULL);
}
