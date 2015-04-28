//
//  Name :         GrTexture.cpp
//  Description :  Implementation of the CGrImage class.
//                 This class allows for the simple manipulation of BITMAP images.
//  Version :      See GrImage.h
//
// This work is Copyright (C) 2002-2012 Michigan State University
// This work is licensed under Microsoft Public License (Ms-PL)
//
// Please include author attribution when using this code.
//
// Author:  Charles B. Owen
//

#include <stdafx.h>
#include "GrImage.h"
#include <cassert>

using namespace std;

#include <GL/glu.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string
#endif

inline void _swap(int &a, int &b)
{
    int t = a;
    a = b;
    b = t;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGrImage::CGrImage()
{
    m_planes = 3;       // Default is RGB
    m_rowpitch = 0;
    m_height = 0;
    m_width = 0;
    m_error = NULL;
    m_image = NULL;
}

CGrImage::CGrImage(const CGrImage &p_img)
{
    m_planes = 3;       // Default is RGB
    m_rowpitch = 0;
    m_height = 0;
    m_width = 0;
    m_image = NULL;
    m_error = NULL;

    Copy(p_img);
}

CGrImage::~CGrImage()
{
    Clear();
}


int CGrImage::GetPlanes() const {return m_planes;}

int CGrImage::GetRowPitch() const {return m_rowpitch;}


void CGrImage::Clear()
{
    if(m_image != NULL)
    {
        delete [] m_image[0];
        delete [] m_image;
        m_image = NULL;
    }

    if(m_error != NULL)
    {
        delete m_error;
        m_error = NULL;
    }

    m_height = 0;
    m_width = 0;
}


bool CGrImage::IsEmpty() const {return m_width <= 0 || m_height <= 0;}

BYTE *CGrImage::operator[](int i) {return m_image[i];}
const BYTE *CGrImage::operator[](int i) const {return m_image[i];}
BYTE *CGrImage::GetRow(int i) {return m_image[i];}
const BYTE *CGrImage::GetRow(int i) const {return m_image[i];}

int CGrImage::GetWidth() const {return m_width;}
int CGrImage::GetHeight() const {return m_height;}
BYTE *CGrImage::GetImageBits() const {return m_image[0];}




//
// Name :         CGrImage::Draw()
// Description :  Draw the image at a specific location on the screen.
//

bool CGrImage::Draw(CDC *pDC, int p_x, int p_y) const
{
	if(IsEmpty())
		return true;

	BITMAPINFOHEADER bmi;

	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = m_width;
	bmi.biHeight = m_height;
	bmi.biPlanes = 1;
	bmi.biBitCount = m_planes * 8;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;
	bmi.biXPelsPerMeter = 1;
	bmi.biYPelsPerMeter = 1;
	bmi.biClrUsed = 0;
	bmi.biClrImportant = 0;

	SetDIBitsToDevice(pDC->m_hDC,             // hDC
							p_x,              // DestX
							p_y,              // DestY
							m_width,                // nDestWidth
							m_height,               // nDestHeight
							0,                      // SrcX
							0,                      // SrcY
							0,                      // nStartScan
							m_height,               // nNumScans
							m_image[0],             // lpBits
							(LPBITMAPINFO)&bmi,     // lpBitsInfo
							DIB_RGB_COLORS);        // wUsage

	return true;
}


void CGrImage::DrawLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{
    // Which dimension is greater?
    if(abs(x2-x1) > abs(y2-y1))
    {
        // Draw left to right
        if(x1 > x2)
        {
            _swap(x1, x2);
            _swap(y1, y2);
        }

        double y = y1;
        double dy = double(y2 - y1) / double(x2 - x1);
        for(int x=x1;  x<=x2;  x++)
        {
            Set(x, int(y + 0.5), r, g, b, a);
            y += dy;
        }
    }
    else
    {
        // Draw bottom to top
        if(y1 > y2)
        {
            _swap(x1, x2);
            _swap(y1, y2);
        }

        double x = x1;
        double dx = double(x2 - x1) / double(y2 - y1);
        for(int y=y1;  y<=y2;  y++)
        {
            Set(int(x + 0.5), y, r, g, b, a);
            x += dx;
        }
    }
}



//////////////////////////////////////////////////////////////////////
// Basic Manipulations
//////////////////////////////////////////////////////////////////////

//
// Name :         CGrImage::Copy()
// Description :  Copy another image into this one.
//

void CGrImage::Copy(const CGrImage &p_img)
{
    SetSameSize(p_img);

    for(int i=0;  i<m_height;  i++)
    {
        memcpy(m_image[i], p_img.m_image[i], m_width * m_planes);
    }
}


CGrImage & CGrImage::operator =(const CGrImage &p_img)
{
    Copy(p_img);
    return *this;
}



void CGrImage::SetSameSize(const CGrImage &p_img, int planes)
{
    if(planes <= 0)
        SetSize(p_img.m_width, p_img.m_height, p_img.m_planes);
    else
        SetSize(p_img.m_width, p_img.m_height, planes);
}

//
// Name :         CGrImage::SetSize()
// Description :  Sets the size of the image and allocates memory.
//                An image size of 0 is an empty empty.
//

void CGrImage::SetSize(int p_x, int p_y, int planes)
{
    // If already the right size, don't do anything, we are happy
    if(p_x == m_width && m_height == p_y && m_planes == planes)
        return;

    Clear();

    // Member variables
    m_height = p_y;
    m_width = p_x;
    m_planes = planes;

    if(p_x <= 0 || p_y <= 0)
        return;

    // Allocate memory for the image.  Note that storage for rows must
    // be on DWORD boundaries.  (32 bit boundaries)
    const int PADSIZE = 4;

    int usewidth = (m_width * m_planes + (PADSIZE - 1)) / PADSIZE;
    usewidth *= PADSIZE;
    m_rowpitch = usewidth;

    BYTE *image = new BYTE[usewidth * m_height];
    m_image = new BYTE *[m_height];
    for(int i=0;  i<m_height;  i++, image += usewidth)
    {
        m_image[i] = image;
    }
}

void CGrImage::Set(int x, int y, int r, int g, int b, int a)
{
    if(x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        BYTE *img = m_image[y] + x * m_planes;
        if(m_planes == 1)
        {
            *img = r;
        }
        else if(m_planes == 3)
        {
            *img++ = b;
            *img++ = g;
            *img = r;
        }
        else if(m_planes == 4)
        {
            *img++ = b;
            *img++ = g;
            *img++ = r;
            *img = a;
        }

    }
}


void CGrImage::Fill(int r, int g, int b, int a)
{
    switch(m_planes)
    {
    case 1:
        for(int i=0; i<m_height;  i++)
        {
            BYTE *img = m_image[i];
            for(int j=0;  j<m_width;  j++)
            {
                *img++ = b;
            }
        }
        break;

    case 3:
        for(int i=0; i<m_height;  i++)
        {
            BYTE *img = m_image[i];
            for(int j=0;  j<m_width * 3;  j+=3)
            {
                *img++ = b;
                *img++ = g;
                *img++ = r;
            }
        }
        break;

    case 4:
        for(int i=0; i<m_height;  i++)
        {
            BYTE *img = m_image[i];
            for(int j=0;  j<m_width * 4;  j+=4)
            {
                *img++ = b;
                *img++ = g;
                *img++ = r;
                *img++ = a;
            }
        }
        break;
    }

}

//////////////////////////////////////////////////////////////////////
// Generic file and memory reading operations
//////////////////////////////////////////////////////////////////////

// This function allows us to copy a string into an array allocated to
// the size of the string, releasing any memory that may have been 
// previously allocated.
static void SetTString(TCHAR **dest, const TCHAR *src, int size)
{
    if(*dest != NULL)
    {
        delete *dest;
        *dest = NULL;
    }

    // Leave space for null termination
    *dest = new TCHAR[size + 1];

    // And copy it over...
    ::_tcsnccpy_s(*dest, size+1, src, size);
}


//
//  Name :         CGrImage::LoadFile()
//  Description :  Load this image from a file of type BMP or PPM
//  Returns :      true if successful

bool CGrImage::LoadFile(LPCTSTR filename)
{
    Clear();

    CImage image;
    HRESULT hr = image.Load(filename);
    if(FAILED(hr))
    {
        LPTSTR errorText = NULL;

        FormatMessage(
            // use system message tables to retrieve error text
            FORMAT_MESSAGE_FROM_SYSTEM
            // allocate buffer on local heap for error text
            |FORMAT_MESSAGE_ALLOCATE_BUFFER
            // Important! will fail otherwise, since we're not 
            // (and CANNOT) pass insertion parameters
            |FORMAT_MESSAGE_IGNORE_INSERTS,  
            NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errorText,  // output 
            0, // minimum size for output buffer
            NULL);   // arguments - see note 

        tstring msg = TEXT("Unable to read image file: ");
        msg += filename;
        msg += TEXT(" - ");
        msg += errorText;
        LocalFree(errorText);
        
        SetTString(&m_error, msg.c_str(), msg.size());

        return false;
    }

    return LoadFrom(&image);
}


bool CGrImage::SaveFile(LPCTSTR filename, CGrImage::SaveTypes type)
{
    // Put into a CImage object
    CImage image;
    SaveTo(&image);

    // And write to a file
    HRESULT hr;
    switch(type)
    {
    case BMP:
        hr = image.Save(filename, Gdiplus::ImageFormatBMP);
        break;

    case JPEG:
        hr = image.Save(filename, Gdiplus::ImageFormatJPEG);
        break;

    case PNG:
        hr = image.Save(filename, Gdiplus::ImageFormatPNG);
        break;
    }

    // Test for any error...
    if(FAILED(hr))
    {
        LPTSTR errorText = NULL;

        FormatMessage(
            // use system message tables to retrieve error text
            FORMAT_MESSAGE_FROM_SYSTEM
            // allocate buffer on local heap for error text
            |FORMAT_MESSAGE_ALLOCATE_BUFFER
            // Important! will fail otherwise, since we're not 
            // (and CANNOT) pass insertion parameters
            |FORMAT_MESSAGE_IGNORE_INSERTS,  
            NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errorText,  // output 
            0, // minimum size for output buffer
            NULL);   // arguments - see note 

        tstring msg = TEXT("Unable to write image file: ");
        msg += filename;
        msg += TEXT(" - ");
        msg += errorText;
        LocalFree(errorText);
        
        SetTString(&m_error, msg.c_str(), msg.size());

        return false;
    }

    return true;
}


bool CGrImage::LoadFrom(const ATL::CImage *image)
{
    return LoadFrom(image, NULL);
}


bool CGrImage::SaveTo(ATL::CImage *image)
{
    image->Create(m_width, m_height, m_planes * 8, 
        m_planes == 4 ? CImage::createAlphaChannel : 0);

    int pitch = image->GetPitch();
    BYTE *bits = (BYTE *)image->GetBits();

    for(int r=0;  r<m_height;  r++)
    {
        // Bits in the destination image row
        BYTE *pixel = bits;
        BYTE *row = m_image[m_height - r - 1];

        // Copy the row
        for(int c=0;  c<m_width * m_planes;  c++)
        {
            *pixel++ = *row++;
        }

        bits += pitch;
    }

    return true;
}




bool CGrImage::LoadFrom(const ATL::CImage *image, LPCTSTR filename)
{
    Clear();

    int bpp = image->GetBPP();
    int pitch = image->GetPitch();
    BYTE *bits = (BYTE *)image->GetBits();
    bool idib = image->IsDIBSection();
    if(!image->IsDIBSection())
    {
        tstring msg = TEXT("Unable to read image");
        if(filename != NULL)
        {
            msg += TEXT(" file:");
            msg += filename;
        }
        else
        {
            msg += TEXT(": ");
        }

        msg += TEXT(" - File format could not be loaded");
        SetTString(&m_error, msg.c_str(), msg.size());
    }


    switch(bpp)
    {
    case 8:
        {
            SetSize(image->GetWidth(), image->GetHeight(), 1);
            for(int r=0;  r<m_height;  r++)
            {
                BYTE *pixel = bits;

                for(int c=0;  c<m_width;  c++)
                {
                    int g = pixel[0];

                    Set(c, m_height - r - 1, g, g, g);

                    pixel++;
                }

                bits += pitch;
            }
        }
        break;

    case 24:
        {
            SetSize(image->GetWidth(), image->GetHeight(), 3);
            for(int r=0;  r<m_height;  r++)
            {
                BYTE *pixel = bits;

                for(int c=0;  c<m_width;  c++)
                {
                    int blu = pixel[0];
                    int grn = pixel[1];
                    int red = pixel[2];

                    Set(c, m_height - r - 1, red, grn, blu);

                    pixel += 3;
                }

                bits += pitch;
            }
        }
        break;

    case 32:
        {
            SetSize(image->GetWidth(), image->GetHeight(), 4);
            for(int r=0;  r<m_height;  r++)
            {
                BYTE *pixel = bits;

                for(int c=0;  c<m_width;  c++)
                {
                    int blu = pixel[0];
                    int grn = pixel[1];
                    int red = pixel[2];
                    int a = pixel[3];

                    Set(c, m_height - r - 1, red, grn, blu, a);

                    pixel += 4;
                }

                bits += pitch;
            }
        }
        break;

    default:
        {
            tstring msg = TEXT("Unable to read image");
            if(filename != NULL)
            {
                msg += TEXT(" file:");
                msg += filename;
            }
            else
            {
                msg += TEXT(": ");
            }

            msg += TEXT(" - File format could not be loaded");

            SetTString(&m_error, msg.c_str(), msg.size());
        }
        break;
    }

    return true;
}


