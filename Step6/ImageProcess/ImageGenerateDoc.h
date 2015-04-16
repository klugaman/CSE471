#pragma once

// ImageGenerateDoc.h : interface of the CImageGenerateDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ImageGenerateDoc_H__9DC9E63D_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)
#define AFX_ImageGenerateDoc_H__9DC9E63D_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_

#include "graphics/GrImage.h"
#include "HPoint.h"

class CImageGenerateDoc : public CDocument
{
protected: 
    CImageGenerateDoc();
    DECLARE_DYNCREATE(CImageGenerateDoc)

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CImageGenerateDoc)
public:
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    //}}AFX_VIRTUAL

    // Implementation
public:
    void MousePress(int x, int y);
    void MouseMove(int x, int y);

    virtual ~CImageGenerateDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    CGrImage *GetImage() {return &m_image;}

protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(CImageGenerateDoc)
    afx_msg void OnMouseDraw();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void FilterThreshold(int x, int y);
    int m_mousemode;
    CGrImage m_image;
    int      m_clickcnt;
public:
    afx_msg void OnGenerateFillwhite();
	afx_msg void OnGenerateFillgreen();
	afx_msg void OnGenerateFilllightskyblue();
	afx_msg void OnGenerateGeneratehorizontalgradient();
	afx_msg void OnGenerateGenerateverticalgradient();
	afx_msg void OnGenerateGenerate();
	afx_msg void OnGenerateHorizontalline();
	afx_msg void OnGenerateVerticalline();
	afx_msg void OnGenerateDiagonalline();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ImageGenerateDoc_H__9DC9E63D_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)
