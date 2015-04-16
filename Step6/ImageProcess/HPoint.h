// HPoint.h: interface for the CHPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HPOINT_H__59B42C41_7C34_11D3_931D_00104BE272D1__INCLUDED_)
#define AFX_HPOINT_H__59B42C41_7C34_11D3_931D_00104BE272D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHPoint  
{
public:
	double Length();
	CHPoint operator-(const CHPoint &p2) const;
	CHPoint & Normalize();
	CHPoint(double x, double y, double z=1);
	CHPoint & Set(double x, double y, double z=1.);
	CHPoint();
	virtual ~CHPoint();


	operator double *() {return p;}
	operator const double *() const {return p;}
	CHPoint operator-();

private:
	double p[3];
};


class CHMatrix  
{
public:
	CHPoint operator *(const CHPoint &p);
	CHMatrix operator *(const CHMatrix &n);
	CHMatrix & SetScaleUniform(double s);
	CHMatrix & SetScale(double x, double y);
	CHMatrix Transpose();
	CHMatrix & SetRotate(const CHPoint &p1, const CHPoint &p2);
	CHMatrix & SetRotate(double angle);
	CHMatrix & SetIdentity();
	CHMatrix & SetTranslate(const CHPoint &p);
    CHMatrix & SetTranslate(double x, double y);
    CHMatrix Inverse();
	CHMatrix();
	virtual ~CHMatrix();

	double *operator[](int i) {return m[i];}
	const double *operator[](int i) const {return m[i];}

    double &M(int r, int c) {return m[r][c];}

private:
	double m[3][3];
};


#endif // !defined(AFX_HPOINT_H__59B42C41_7C34_11D3_931D_00104BE272D1__INCLUDED_)
