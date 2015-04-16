#pragma once

// ImageProcessDoc.h : interface of the CImageProcessDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEPROCESSDOC_H__9DC9E63D_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)
#define AFX_IMAGEPROCESSDOC_H__9DC9E63D_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_

#include "graphics/GrImage.h"
#include "HPoint.h"

class CImageProcessDoc : public CDocument
{
protected: 
    CImageProcessDoc();
    DECLARE_DYNCREATE(CImageProcessDoc)

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CImageProcessDoc)
public:
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    //}}AFX_VIRTUAL

    // Implementation
public:
    void MousePress(int x, int y);
    void MouseMove(int x, int y);

    virtual ~CImageProcessDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    CGrImage *GetImage1() {return &m_image1;}
    CGrImage *GetImage2() {return &m_image2;}

protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(CImageProcessDoc)
    afx_msg void OnFilterNegative();
    afx_msg void OnFilterCopy();
    afx_msg void OnMouseDraw();
    afx_msg void OnUpdateMouseDraw(CCmdUI* pCmdUI);
    afx_msg void OnMouseThreshold();
    afx_msg void OnUpdateMouseThreshold(CCmdUI* pCmdUI);
    afx_msg void OnMouseCartman();
    afx_msg void OnUpdateMouseCartman(CCmdUI* pCmdUI);
    afx_msg void OnMouseCartmanparameters();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void FilterThreshold(int x, int y);
    int m_mousemode;
    CGrImage m_image1;
    CGrImage m_image2;
    int      m_clickcnt;

    // Cartman filter stuff
    void Cartman(int x, int y);
    CGrImage   m_cartman;
    CHPoint  m_delta;
    CHPoint  m_gamma;

    BOOL     m_nearest;
    BOOL     m_transparent;
public:
	afx_msg void OnFilterDim();
	afx_msg void OnFilterTint();
	afx_msg void OnFilterLowpassfilter();
	afx_msg void OnFilterMonochrome();
	afx_msg void OnFilterMedianfilter();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEPROCESSDOC_H__9DC9E63D_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)
