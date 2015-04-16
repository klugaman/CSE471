// HPoint.cpp: implementation of the CHPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HPoint.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHPoint::CHPoint()
{
	p[2] = 1;

}

CHPoint::~CHPoint()
{

}

CHPoint & CHPoint::Set(double x, double y, double z)
{
	p[0] = x;
	p[1] = y;
	p[2] = z;
	return *this;
}

CHPoint CHPoint::operator -()
{
	return CHPoint(-p[0], -p[1]);
}


//////////////////////////////////////////////////////////////////////
// CHMatrix Class
//////////////////////////////////////////////////////////////////////

CHMatrix::CHMatrix()
{
	for(int i=0;  i<3;  i++)
		for(int j=0;  j<3;  j++)
			m[i][j] = 0;

	m[2][2] = 1.;
}

CHMatrix::~CHMatrix()
{

}


CHPoint::CHPoint(double x, double y, double z)
{
	p[0] = x;
	p[1] = y;
	p[2] = z;
}


CHMatrix & CHMatrix::SetTranslate(const CHPoint &p)
{
	SetIdentity();
	m[0][2] = p[0];
	m[1][2] = p[1];
	return *this;
}

CHMatrix & CHMatrix::SetTranslate(double x, double y)
{
	SetIdentity();
	m[0][2] = x;
	m[1][2] = y;
	return *this;
}



CHMatrix & CHMatrix::SetIdentity()
{
	for(int i=0;  i<3;  i++)
		for(int j=0;  j<3;  j++)
			m[i][j] = i == j ? 1 : 0;

	return *this;
}

CHMatrix & CHMatrix::SetRotate(const CHPoint &p1, const CHPoint &p2)
{
    SetIdentity();
	CHPoint d = p2 - p1;
	d.Normalize();

    m[0][0] = d[0];
    m[0][1] = d[1];
    m[1][0] = -d[1];
    m[1][1] = d[0];

	return *this;
}

CHMatrix & CHMatrix::SetRotate(double angle)
{
    double ca = cos(angle);
    double sa = sin(angle);

    SetIdentity();
	
    m[0][0] = ca;
    m[0][1] = -sa;
    m[1][0] = sa;
    m[1][1] = ca;

	return *this;
}


CHPoint & CHPoint::Normalize()
{
	double d = sqrt(p[0] * p[0] + p[1] * p[1]);
	p[0] /= d;
	p[1] /= d;
	return *this;
}

CHMatrix  CHMatrix::Transpose()
{
    CHMatrix T = *this;
    double t;
    for(int i=0;  i<2;  i++)
    {
        for(int j=i+1; j<3;  j++)
        {
            t = T.m[i][j];  T.m[i][j] = T.m[j][i];  T.m[j][i] = t;
        }

    }
    return T;
}

CHMatrix & CHMatrix::SetScaleUniform(double s)
{
	SetIdentity();
	m[0][0] = m[1][1] = s;
	return *this;
}

CHMatrix & CHMatrix::SetScale(double x, double y)
{
	SetIdentity();
	m[0][0] = x;
    m[1][1] = y;
	return *this;
}


CHPoint CHPoint::operator -(const CHPoint &p2) const
{
	return CHPoint(p[0] - p2[0], p[1] - p2[1]);
}

double CHPoint::Length()
{
	return sqrt(p[0] * p[0] + p[1] * p[1]);
}

CHMatrix CHMatrix::operator *(const CHMatrix &n)
{
	CHMatrix m3;

   for(int i=0;  i<3;  i++)
      for(int j=0;  j<3;  j++)
      {
         m3[i][j] = 0.;
         for(int k=0;  k<3;  k++)
            m3[i][j] += m[i][k] * n[k][j];
      }

	return m3;
}

CHPoint CHMatrix::operator *(const CHPoint &p)
{
	return CHPoint(m[0][0] * p[0] + m[0][1] * p[1] + m[0][2] * p[2],
		m[1][0] * p[0] + m[1][1] * p[1] + m[1][2] * p[2],
		m[2][0] * p[0] + m[2][1] * p[1] + m[2][2] * p[2]);
}

CHMatrix CHMatrix::Inverse()
{
    double adjoint[3][3];

    adjoint[0][0] =  (M(1, 1) * M(2, 2) - M(1, 2) * M(2, 1));
    adjoint[1][0] = -(M(1, 0) * M(2, 2) - M(1, 2) * M(2, 0));
    adjoint[2][0] =  (M(1, 0) * M(2, 1) - M(1, 1) * M(2, 0));
    adjoint[0][1] = -(M(0, 1) * M(2, 2) - M(0, 2) * M(2, 1));
    adjoint[1][1] =  (M(0, 0) * M(2, 2) - M(0, 2) * M(2, 0));
    adjoint[2][1] = -(M(0, 0) * M(2, 1) - M(0, 1) * M(2, 0));
    adjoint[0][2] =  (M(0, 1) * M(1, 2) - M(0, 2) * M(1, 1));
    adjoint[1][2] = -(M(0, 0) * M(1, 2) - M(0, 2) * M(1, 0));
    adjoint[2][2] =  (M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0));

    // Now compute the determinate
    // It is the sum of the products of the cofactors and the elements of one 
    // row of the matrix:

    double det = M(0, 0) * adjoint[0][0] + M(0, 1) * adjoint[1][0] + M(0, 2) * adjoint[2][0];
    if(det == 0)
        det = 0.000001;

    CHMatrix I;
    I.M(0, 0) = adjoint[0][0] / det;
    I.M(0, 1) = adjoint[0][1] / det;
    I.M(0, 2) = adjoint[0][2] / det;
    I.M(1, 0) = adjoint[1][0] / det;
    I.M(1, 1) = adjoint[1][1] / det;
    I.M(1, 2) = adjoint[1][2] / det;
    I.M(2, 0) = adjoint[2][0] / det;
    I.M(2, 1) = adjoint[2][1] / det;
    I.M(2, 2) = adjoint[2][2] / det;

    return I;
}