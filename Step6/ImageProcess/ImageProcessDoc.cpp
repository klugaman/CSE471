//
// Name :         ImageProcessDoc.cpp
// Description :  Implementation of the document class for the 
//                ImageProcess application.
//

// Notice:  Do not add any headers before stdafx.h
#include "stdafx.h"

#include "ImageProcess.h"
#include "graphics/GrImage.h"
#include "CartmanDlg.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>
#include "ImageProcessDoc.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

vector<int> median(8);

/////////////////////////////////////////////////////////////////////////////
// CImageProcessDoc

IMPLEMENT_DYNCREATE(CImageProcessDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessDoc, CDocument)
    //{{AFX_MSG_MAP(CImageProcessDoc)
    ON_COMMAND(ID_FILTER_NEGATIVE, OnFilterNegative)
    ON_COMMAND(ID_FILTER_COPY, OnFilterCopy)
    ON_COMMAND(ID_MOUSE_DRAW, OnMouseDraw)
    ON_UPDATE_COMMAND_UI(ID_MOUSE_DRAW, OnUpdateMouseDraw)
    ON_COMMAND(ID_MOUSE_THRESHOLD, OnMouseThreshold)
    ON_UPDATE_COMMAND_UI(ID_MOUSE_THRESHOLD, OnUpdateMouseThreshold)
    ON_COMMAND(ID_MOUSE_CARTMAN, OnMouseCartman)
    ON_UPDATE_COMMAND_UI(ID_MOUSE_CARTMAN, OnUpdateMouseCartman)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_MOUSE_CARTMANPARAMETERS, OnMouseCartmanparameters)
	ON_COMMAND(ID_FILTER_FILTER, &CImageProcessDoc::OnFilterDim)
	ON_COMMAND(ID_FILTER_FILTER32803, &CImageProcessDoc::OnFilterTint)
	ON_COMMAND(ID_FILTER_PROCESS, &CImageProcessDoc::OnFilterLowpassfilter)
	ON_COMMAND(ID_FILTER_MONOCHROME, &CImageProcessDoc::OnFilterMonochrome)
	ON_COMMAND(ID_FILTER_MEDIANFILTER, &CImageProcessDoc::OnFilterMedianfilter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageProcessDoc construction/destruction

CImageProcessDoc::CImageProcessDoc()
{
    m_mousemode = 0;     // No mouse mode
    m_clickcnt = 0;
    m_nearest = FALSE;
    m_transparent = TRUE;
}

CImageProcessDoc::~CImageProcessDoc()
{
}

BOOL CImageProcessDoc::OnNewDocument()
{
    m_image1.Clear();
    m_image2.SetSize(640, 480);
    m_image2.Fill(0, 0, 0);
    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CImageProcessDoc diagnostics

#ifdef _DEBUG
void CImageProcessDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CImageProcessDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageProcessDoc commands

BOOL CImageProcessDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
    BeginWaitCursor();
    DeleteContents();

    bool r = m_image1.LoadFile(lpszPathName);
    m_image2 = m_image1;
    EndWaitCursor();

    if(!r)
    {
        // Error message on return
        AfxMessageBox(m_image1.GetError(), NULL, MB_ICONINFORMATION | MB_OK);
        return FALSE;
    }

    SetPathName(lpszPathName);
    SetModifiedFlag(FALSE);     // start off with unmodified

    return TRUE;
}

//
//  Name :         CImageProcessDoc::OnSaveDocument()
//  Description :  Write a device independent bitmap file
//
BOOL CImageProcessDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
    BeginWaitCursor();
    DeleteContents();

    bool r = m_image2.SaveFile(lpszPathName, CGrImage::BMP);
    EndWaitCursor();

    if(!r)
    {
        // Error message on return
        AfxMessageBox(m_image2.GetError(), NULL, MB_ICONINFORMATION | MB_OK);
        return FALSE;
    }

    SetModifiedFlag(FALSE);     // start off with unmodified
    return TRUE;
}




//
// Menu option handlers
//


//
// Name :         CImageProcessDoc::OnFilterCopy()
// Description :  Example filter that just copies the image.
//

void CImageProcessDoc::OnFilterCopy() 
{
    m_image2 = m_image1;
    UpdateAllViews(NULL);
}



//
// Name :         CImageProcessDoc::OnFilterNegative() 
// Description :  Example filter that computes a negative image.
//

void CImageProcessDoc::OnFilterNegative() 
{
    BeginWaitCursor();

    // Make the output image the same size as the input image
    m_image2.SetSameSize(m_image1);

    for(int r=0;  r<m_image2.GetHeight();  r++)
    {
        for(int c=0;  c<m_image2.GetWidth() * 3;  c++)
        {
            m_image2[r][c] = 255 - m_image1[r][c];
        }
    }

    UpdateAllViews(NULL);
    EndWaitCursor();
}


//
// Name :         CImageProcessDoc::OnMouseDraw() 
// Description :  Set the mouse draw mode.
//

void CImageProcessDoc::OnMouseDraw() 
{
    m_mousemode = ID_MOUSE_DRAW;
}


//
// Name :         CImageProcessDoc::OnUpdateMouseDraw() 
// Description :  Set the mouse draw mode menu status.
//

void CImageProcessDoc::OnUpdateMouseDraw(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_mousemode == ID_MOUSE_DRAW);
}


void CImageProcessDoc::OnMouseThreshold() 
{
    m_mousemode = ID_MOUSE_THRESHOLD;
}

void CImageProcessDoc::OnUpdateMouseThreshold(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_mousemode == ID_MOUSE_THRESHOLD);
}


/////////////////////////////////////////////////////////////////////
//
// Mouse Handlers
//
/////////////////////////////////////////////////////////////////////


//
// Name :         CImageProcessDoc::MousePress()
// Description :  This function is called when the mouse is pressed over
//                m_image2.  The x,y coordinate is in the image.
//

void CImageProcessDoc::MousePress(int x, int y)
{
    // We count the mouse clicks
    m_clickcnt++;

    BeginWaitCursor();

    switch(m_mousemode)
    {
    case ID_MOUSE_DRAW:
        m_image2[y][x * 3] = 0;
        m_image2[y][x * 3 + 1] = 255;
        m_image2[y][x * 3 + 2] = 0;
        break;

    case ID_MOUSE_THRESHOLD:
        FilterThreshold(x, y);
        break;

    case ID_MOUSE_CARTMAN:
        Cartman(x, y);
        break;
    }

    UpdateAllViews(NULL);
    EndWaitCursor();
}


//
// Name :         CImageProcessDoc::MouseMove()
// Description :  This function is called when the mouse is moved over
//                m_image2.  The x,y coordinate is in the image.
//

void CImageProcessDoc::MouseMove(int x, int y)
{
    BeginWaitCursor();

    switch(m_mousemode)
    {
    case ID_MOUSE_DRAW:
        m_image2[y][x * 3] = 0;          // Blue
        m_image2[y][x * 3 + 1] = 255;    // Green
        m_image2[y][x * 3 + 2] = 255;    // Red
        break;
    }   

    UpdateAllViews(NULL);
    EndWaitCursor();
}



//
// Name :         CImageProcessDoc::FilterThreshold(int x, int y)
// Description :  This is an example filter that looks at a specific location
//                and determines a gray scale value we can use as a threshold.
//                We'll then use that threshold to convert the image into a 
//                bi-level image (black and white only).
//

void CImageProcessDoc::FilterThreshold(int x, int y)
{
    // What is the monochrome value at location x,y in the
    // input image, not the filtered image!
    int threshgray = int(m_image1[y][x * 3]) + int(m_image1[y][x * 3 + 1]) + int(m_image1[y][x * 3 + 2]) / 3;

    // Now do the image threshold
    // Make the output image the same size as the input image
    m_image2.SetSameSize(m_image1);

    for(int r=0;  r<m_image2.GetHeight();  r++)
    {
        for(int c=0;  c<m_image2.GetWidth();  c++)
        {
            // What is the gray value at this location?
            int pixelgray = int(m_image1[r][c * 3]) + int(m_image1[r][c * 3 + 1]) + int(m_image1[r][c * 3 + 2]) / 3;
            if(pixelgray >= threshgray)
            {
                // Set the pixel to white
                m_image2[r][c * 3] = 255;
                m_image2[r][c * 3 + 1] = 255;
                m_image2[r][c * 3 + 2] = 255;
            }
            else
            {
                // Set the pixel to black
                m_image2[r][c * 3] = 0;
                m_image2[r][c * 3 + 1] = 0;
                m_image2[r][c * 3 + 2] = 0;
            }

        }

    }

}




void CImageProcessDoc::OnMouseCartman() 
{
    // Reset the mouse click count whenever we select cartman mode.
    m_clickcnt = 0;
    m_image2 = m_image1;
    m_mousemode = ID_MOUSE_CARTMAN;
}

void CImageProcessDoc::OnUpdateMouseCartman(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_mousemode == ID_MOUSE_CARTMAN);
}


// These functions are needed to simplify mins and maxes...
inline double min4(double a, double b, double c, double d)
{
    double r = a;
    if(b < r)
        r = b;
    if(c < r)
        r = c;
    if(d < r)
        r = d;
    return r;
}

inline double max4(double a, double b, double c, double d)
{
    double r = a;
    if(b > r)
        r = b;
    if(c > r)
        r = c;
    if(d > r)
        r = d;
    return r;
}



//
// Name :         CImageProcessDoc::Cartman()
// Description :  This function implements an image warping example that moves
//                an image of Cartman onto two specified points on the image.
//

void CImageProcessDoc::Cartman(int x, int y)
{  
    // 
    // Step -1:  Ensure we have an image to warp onto our output.
    //

    if(m_cartman.IsEmpty())
    {
        BeginWaitCursor();

        bool err = m_cartman.LoadFile("cartman.bmp");
        EndWaitCursor();

        if(!err)
        {
            // Error message on return
            AfxMessageBox("Unable to load file cartman.bmp", NULL, MB_ICONINFORMATION | MB_OK);
            return;
        }

    }

    // If the template image is empty, do nothing
    if(m_cartman.GetHeight() <= 0 || m_cartman.GetWidth() <= 0)
        return;

    //
    // Step 0:  Handle the alternate mouse click feature.
    //

    // This handles the alternate mouse click feature
    // The first mouse click is just indicated on the
    // image, the second initiates the warping.
    if((m_clickcnt & 1) == 1)
    {
        m_image2 = m_image1;

        m_delta.Set(x, y);

        // Draw a dot on the image
        for(int i=-1;  i<=1;  i++)
            for(int j=-1;  j<=1;  j++)
                m_image2.Set(int(m_delta[0])+i, int(m_delta[1])+j, 255, 0, 0);
        return;
    }

    m_gamma.Set(x, y);
    // Draw a dot on the image
    for(int i=-1;  i<=1;  i++)
        for(int j=-1;  j<=1;  j++)
            m_image2.Set(int(m_gamma[0])+i, int(m_gamma[1])+j, 0, 255, 255);

    int w = m_cartman.GetWidth();
    int h = m_cartman.GetHeight();

    // 
    // Now we are actually going to do the warping work.
    //

    // These are the alignment points in the warp image
    // The destination points are m_delta and m_gamma
    CHPoint alpha(w / 2, 0);
    CHPoint beta(w / 2, h);

    // These are the corners of the warp image
    CHPoint tl(0, h);
    CHPoint tr(w, h);
    CHPoint bl(0, 0);
    CHPoint br(w, 0);

    //
    // Step 1:  Build a matrix that translates points in the source 
    // image to points in the destination image.  
    //

    // Translate the point we are going to rotate around to the origin
    CHMatrix m1;
    m1.SetTranslate(-alpha);

    // Rotate -90 degrees
    CHMatrix r1;
    r1.SetIdentity();
    r1[0][0] = 0;
    r1[0][1] = 1;
    r1[1][0] = -1;
    r1[1][1] = 0;

    // Rotate onto the m_delta to m_gamma vector.
    CHPoint v = m_gamma - m_delta;
    v.Normalize();

    CHMatrix r2;
    r2[0][0] = v[0];
    r2[0][1] = -v[1];
    r2[1][0] = v[1];
    r2[1][1] = v[0];

    // Scale image to the size of the output location.
    CHMatrix s;
    s.SetScaleUniform((m_gamma - m_delta).Length() / (beta - alpha).Length());

    // Translate to the destination
    CHMatrix m2;
    m2.SetTranslate(m_delta);

    // Multiply all of these matrices together.
    CHMatrix src_to_dest = m2 * s * r2 * r1 * m1;

    //
    // Step 2:  Create an inverse version of that matrix.
    //

    CHMatrix dest_to_src = src_to_dest.Inverse();

    // 
    // Step 3:  Determine the bounding box in the destination image for
    // the Cartman image.  
    //

    CHPoint itl = src_to_dest * tl;
    CHPoint itr = src_to_dest * tr;
    CHPoint ibl = src_to_dest * bl;
    CHPoint ibr = src_to_dest * br;
    double minx = min4(itl[0], itr[0], ibl[0], ibr[0]);
    double maxx = max4(itl[0], itr[0], ibl[0], ibr[0]);
    double miny = min4(itl[1], itr[1], ibl[1], ibr[1]);
    double maxy = max4(itl[1], itr[1], ibl[1], ibr[1]);

    // Make sure the bounding box is in the image
    if(minx < 0)
        minx = 0;

    if(maxx >= m_image2.GetWidth())
        maxx = m_image2.GetWidth() - 1;

    if(miny < 0)
        miny = 0;

    if(maxy >= m_image2.GetHeight())
        maxy = m_image2.GetHeight() - 1;

    // 
    // Step 4:  Loop over the pixels in the bounding box, determining 
    // their color in the source image.  We use op to indicate pixel 
    // locations in the output image and ip to indicate locations in the
    // input image.
    //

    for(int y2=int(miny+.5);  y2<=int(maxy+.5);  y2++)
    {
        for(int x2=int(minx+.5);  x2<=int(maxx+.5);  x2++)
        {
            // This is the equivalent point in the source image (cartman)
            // This is the equivalent point in the source image 
            double x1 = dest_to_src[0][0] * x2 + dest_to_src[0][1] * y2 + dest_to_src[0][2];
            double y1 = dest_to_src[1][0] * x2 + dest_to_src[1][1] * y2 + dest_to_src[1][2]; 

            int ix = int(x1);
            double ixf = x1 - ix;      // Fractional part
            int iy = int(y1);
            double iyf = y1 - iy;

            // Notice that I only use pixels that are going to be 
            // within the cartman image and have pixels all of the
            // way around.  This is so our bilinear interpolation
            // will have neighbors on all sides!  That's why we
            // have w-1 and h-1 here:
            if(ix >= 0 && ix < w-1 && 
                iy >= 0 && iy < h-1)
            {
                // This is a test for the color white, which is what I'll 
                // consider to be transparent.  We only use colors that are
                // not the pure white color.
                int c = ix * 3;
                if(!m_transparent || m_cartman[iy][c] != 255 ||
                    m_cartman[iy][c+1] != 255 ||
                    m_cartman[iy][c+1] != 255)
                {
                    if(m_nearest)
                    {
                        // Loop over the colors
                        for(int c1=0;  c1<3;  c1++)
                        {
                            // Nearest neighbor version
                            m_image2[y2][x2*3+c1] = m_cartman[iy][c+c1];
                        }
                    }
                    else
                    {
                        // Loop over the colors
                        for(int c1=0;  c1<3;  c1++)
                        {
                            // Bilinear interpolation version
                            m_image2[y2][x2*3+c1] = BYTE((1. - ixf) * (1. - iyf) * m_cartman[iy][c+c1] +
                                (1. - ixf) * iyf * m_cartman[iy + 1][c+c1] +
                                ixf * (1. - iyf) * m_cartman[iy][c+c1+3] +
                                ixf * iyf * m_cartman[iy + 1][c+c1+3]);

                        }
                    }


                }
            }
        }
    }

}

void CImageProcessDoc::OnMouseCartmanparameters()
{
    CCartmanDlg dlg;

    dlg.m_nearest = m_nearest;
    dlg.m_transparent = m_transparent;

    if(dlg.DoModal() == IDOK)
    {
        m_nearest = dlg.m_nearest;
        m_transparent = dlg.m_transparent;
        UpdateAllViews(NULL);
    }
}



void CImageProcessDoc::OnFilterDim()
{
	// TODO: Add your command handler code here
	BeginWaitCursor();

    // Make the output image the same size as the input image
    m_image2.SetSameSize(m_image1);

    for(int r=0;  r<m_image2.GetHeight();  r++)
    {
        for(int c=0;  c<m_image2.GetWidth() * 3;  c++)
        {
            m_image2[r][c] = BYTE(m_image1[r][c] * 0.33);
        }
    }

    UpdateAllViews(NULL);
    EndWaitCursor();
}


void CImageProcessDoc::OnFilterTint()
{
	// TODO: Add your command handler code here
	BeginWaitCursor();

    // Make the output image the same size as the input image
    m_image2.SetSameSize(m_image1);

    for(int r=0;  r<m_image2.GetHeight();  r++)
    {
        for(int c=0;  c<m_image2.GetWidth();  c++)
        {
            m_image2[r][c*3] = BYTE(m_image1[r][c*3] * 0.66);       // Blue
            m_image2[r][c*3+1] = BYTE(m_image1[r][c*3+1] * 1);   // Green
            m_image2[r][c*3+2] = BYTE(m_image1[r][c*3+2] * 0.33);   // Red
        }
    }

    UpdateAllViews(NULL);
    EndWaitCursor();
}


void CImageProcessDoc::OnFilterLowpassfilter()
{
	// TODO: Add your command handler code here
	 // Allocates an output image the same size as the input image
   m_image2.SetSameSize(m_image1);

   for(int r=0;  r<m_image2.GetHeight();  r++)
   {
      for(int c=0;  c<m_image2.GetWidth() * 3;  c++)
      {
         int pixel = 0;
         for(int i=-1; i<=1;  i++)
         {
            if((r+i) >= 0 && (r+i) < m_image2.GetHeight())
            {
               for(int j=-3;  j<=3;  j+=3)
               {
                  if((c+j) >= 0 && (c+j) < m_image2.GetWidth() * 3)
                     pixel += m_image1[r+i][c+j];
               }
            }
         }

         m_image2[r][c] = BYTE(pixel / 9);
      }
   }

   // Force a redraw
   UpdateAllViews(NULL);

}


void CImageProcessDoc::OnFilterMonochrome()
{
	// TODO: Add your command handler code here
	BeginWaitCursor();

    // Make the output image the same size as the input image
    m_image2.SetSameSize(m_image1);

    for(int r=0;  r<m_image2.GetHeight();  r++)
    {
        for(int c=0;  c<m_image2.GetWidth()*3;  c+=3)
        {
            m_image2[r][c] = m_image2[r][c + 1] = m_image2[r][c + 2] = BYTE((m_image2[r][c] + m_image2[r][c + 1] + m_image2[r][c + 2])/3);     

        }
    }

    UpdateAllViews(NULL);
    EndWaitCursor();
}


void CImageProcessDoc::OnFilterMedianfilter()
{
	// TODO: Add your command handler code here
	m_image2.SetSameSize(m_image1);

	for(int r=1; r < m_image2.GetHeight()-1; r++)
	{
		for(int c=1; c < m_image2.GetWidth()-1; c++)
		{
			median.push_back(m_image2[r - 1][(c - 1)*3 + 0]);
			median.push_back(m_image2[r - 1][c * 3 + 0]);
			median.push_back(m_image2[r - 1][(c + 1)*3 + 0]);
			median.push_back(m_image2[r][(c - 1) * 3 + 0]);
			median.push_back(m_image2[r][c * 3 + 0]);
			median.push_back(m_image2[r][(c + 1) * 3 + 0]);
			median.push_back(m_image2[r + 1][(c - 1) * 3 + 0]);
			median.push_back(m_image2[r + 1][c * 3 + 0]);
			median.push_back(m_image2[r + 1][(c + 1) * 3 + 0]);
			sort(median.begin(),median.end());
			m_image2[r][c * 3 + 0] = (median[3] + median[4])/2;
			median.clear();

			median.push_back(m_image2[r - 1][(c - 1) * 3 + 1]);
			median.push_back(m_image2[r - 1][c * 3 + 1]);
			median.push_back(m_image2[r - 1][(c + 1) * 3 + 1]);
			median.push_back(m_image2[r][(c - 1) * 3 + 1]);
			median.push_back(m_image2[r][c * 3 + 1]);
			median.push_back(m_image2[r][(c + 1) * 3 + 1]);
			median.push_back(m_image2[r + 1][(c - 1) * 3 + 1]);
			median.push_back(m_image2[r + 1][c * 3 + 1]);
			median.push_back(m_image2[r + 1][(c + 1) * 3 + 1]);
			sort(median.begin(),median.end());
			m_image2[r][c * 3 + 1] = (median[3] + median[4])/2;
			median.clear();

			median.push_back(m_image2[r - 1][(c - 1) * 3 + 2]);
			median.push_back(m_image2[r - 1][c * 3 + 2]);
			median.push_back(m_image2[r - 1][(c + 1) * 3 + 2]);
			median.push_back(m_image2[r][(c - 1) * 3 + 2]);
			median.push_back(m_image2[r][c * 3 + 2]);
			median.push_back(m_image2[r][(c + 1) * 3 + 2]);
			median.push_back(m_image2[r + 1][(c - 1) * 3 + 2]);
			median.push_back(m_image2[r + 1][c * 3 + 2]);
			median.push_back(m_image2[r + 1][(c + 1) * 3 + 2]);
			sort(median.begin(),median.end());
			m_image2[r][c*3 + 2] = (median[3] + median[4])/2;
			median.clear();
		}
	}

   // Force a redraw
   UpdateAllViews(NULL);
}
