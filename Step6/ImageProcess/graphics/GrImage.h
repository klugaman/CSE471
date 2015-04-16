//
// Name :         GrImage.h
// Description :  Header for CGrImage, a general purpose image class.
//
// This work is Copyright (C) 2002-2012 Michigan State University
// This work is licensed under Microsoft Public License (Ms-PL)
//
// Please include author attribution when using this code.
//
// Author:  Charles B. Owen
//
#pragma once

#if !defined(_GRIMAGE_H)
#define _GRIMAGE_H

#if !defined(LibGrafx)
#define LibGrafx
#endif

#include <atlimage.h>
#include <fstream>

class CDC; //Avoids unknown CDC type; not used here anyway

//! Class to load and utilize image files.

/*! The CGrImage class loads an image file into memory and allows it be
be easily manipulated. This class
will load any image that can be loaded using the MFC function: CImage::Load().

\version 1.00 01-01-2012 New version merging CPix and CGrTexture
*/

class LibGrafx CGrImage  
{
public:
    //! Default Constructor
    CGrImage();

    //! Copy Constructor. 
    /*! The copy constructor will copy another image. 
        \param img The other image */
    CGrImage(const CGrImage &img);

    //! Destructor
    virtual ~CGrImage();

    //! Draw the image at a specified location on the screen
    /*! \param pDC Pointer to an MFC Device Context
        \param x X location on the screen.
        \param y Y location on the screen. */
    bool Draw(CDC *pDC, int x, int y) const;

    //! Load an image from an MFC image
    /* \param image The image we are loading from */
    bool LoadFrom(const ATL::CImage *image);

    //! Load a image from a file
    /* \param filename The filename as a character string in the current 
       setting of Unicode or multibyte strings. 
       \return true if located successfully. If false, an error 
       message will be placed into the error string and can be
       accessed via the GetError() function. */
    bool LoadFile(LPCTSTR filename);

    //! Save an image into an MFC CImage object
    //! \param image The destination image object
    //! \return true if successful.
    bool SaveTo(ATL::CImage *image);

    //! The available save file types
    enum SaveTypes {BMP, PNG, JPEG};
    
    //! Save an image to a file
    /*! \param filename The filename as a character string in the current
        setting of Unicode or multibyte strings.
        \param type The type of file to save 
        \return true if successful. On any error, an error message is placed in 
        the error string and can be accessed via the GetError() function. */
    bool SaveFile(LPCTSTR filename, SaveTypes type);


    //! Clear the image 
    /*! Clears the image and releases any memory. */
    void Clear();

    //! Fill the image with a color
    /*! Fills the image with a single color. The 
        values for R, G, B, and A are in the range 0 to 255.
        \param r Red component (optional).
        \param g Green component (optional).
        \param b Blue component (optional). 
        \param a Alpha component (optional). */
    void Fill(int r=0, int g=0, int b=0, int a=0);

    //! Set the image size
    /*! Sets the image size. THis will clear the image if it
        has already been loaded. 
        \param wid New image width 
        \param hit New image height 
        \param planes Number of color planes. Default, if not supplied 3 is used. */
    void SetSize(int wid, int hit, int planes=3);

    //! Set a pixel value
    /*! Sets the value of a pixel. 
        \param x X location (column)
        \param y Y location (row)
        \param r Red component.
        \param g Green component.
        \param b Blue component.
        \param a Alpha component (optional) */
    void Set(int x, int y, int r, int g, int b, int a=0);

    //! Set this image to be the same size as another
    /*! Sets the image size identical to that of another
        image. If the optional number of planes is supplied,
        the new image is set to that number of color planes.
        It not, the image is set to the same number of color 
        planes as the other image.
        \param img The other image */
    void SetSameSize(const CGrImage &img, int planes=0);

    //! Copies an image into this one.
    void Copy(const CGrImage &p_img);

    //! Returns true if the texture image is empty.
    bool IsEmpty() const;

    //! Assignment operator that copies a texture image into this one.
    CGrImage &operator=(const CGrImage &p_img);

    //! Bracket operator gets access to a given row of the image
    /*! This operator can be used like this:  \code
    BYTE value = texture[row][byte];
\endcode
        Use with care. The return value is that same as that for
        ImageBits. It can be used to directly access the 
        data for the texture image. It consists of a sequence of 
        bytes in the order B, G, and R. Rows start on the texture
        word (32 bit) boundary. */
    BYTE *operator[](int i);

    //! Bracket operator gets access to a given row of the image
        /*! This operator can be used like this:  \code
    BYTE value = image[row][arraycolumn];
\endcode
        Use with care. The return value is that same as that for
        Row. It can be used to directly access the 
        data for the texture image. It consists of a sequence of 
        bytes in the order B, G, and R for the given row. */
    const BYTE *operator[](int i) const;

    //! Access to pixels in a given row of the image
        /*! This operator can be used like this:  \code
    BYTE *row = image.Row(row);
    BYTE red = row[column * 3 + 2];
\endcode
        Use with care. It can be used to directly access the 
        data for the image. It consists of a sequence of 
        bytes in the order B, G, and R for the given row. */
    BYTE *GetRow(int i);

    //! Access to pixels in a given row of the image
    /*! This operator can be used like this:  \code
    const BYTE *row = image.GetRow(row);
    BYTE red = row[column * 3 + 2];
\endcode
        Use with care. It can be used to directly access the 
        data for the image. It consists of a sequence of 
        bytes in the order B, G, R, and optionally A for the given row. */
    const BYTE *GetRow(int i) const;

    //! Gets the texture image width
    int GetWidth() const;

    //! Gets the texture image height
    int GetHeight() const;

    //! Direct access to the image bits. 
    /*! This function can be used to directly access the 
        data for the image data. It consists of a sequence of 
        bytes in the order B, G, and R. Rows start on a
        word (32 bit) boundary. */
    BYTE *GetImageBits() const;

    //! Get the number of color planes
    //! \return The number of color planes. Can be 1, 3, or 4
    int GetPlanes() const;

    //! Get the row pitch
    //! \return The row pitch. This is the number of bytes from
    //! the start of one row to the start of the next.
    int GetRowPitch() const;

    //! Access any error message generated when a file is
    //! loaded.
    //! \return The error string or NULL if no error
    const TCHAR *GetError() const {return m_error;}

    void DrawLine(int x1, int y1, int x2, int y2, int r, int g=0, int b=0, int a=0);

private:
    bool LoadFrom(const ATL::CImage *image, LPCTSTR filename);

    // Number of image planes (colors)
    int m_planes;

    // Number of bytes per row
    int m_rowpitch;

    // Image height
    int m_height;

    // Image width
    int m_width;

    // The actual image data
    BYTE ** m_image;

    // Any error message
    TCHAR *m_error;
};

#endif 
