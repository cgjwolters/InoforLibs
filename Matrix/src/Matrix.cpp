//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//-------------- General Matrix and Vector Definitions ----------------------
//---------------------------------------------------------------------------
//------------------------ Copyright Inofor Hoek Aut BV 1991..2004 ----------
//---------------------------------------------------------------------------
//------------------------------------------------------ C.Wolters ----------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "Matrix.h"
#include "Exceptions.h"

#include <stddef.h>
#include <math.h>

#include <string.h>
#include <algorithm>

#ifdef _WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

namespace Ino
{

//---------------------------------------------------------------------------

void Vector::setSize(int newSz, bool preserve, bool zeroInit)
{
  if (newSz < 0) throw IllegalArgumentException("Vector::setSize");

  if (newSz >= cap/2 && newSz <= cap) {
    if (zeroInit && newSz > sz) memset(va+sz,0,(newSz-sz)*sizeof(double));
    sz = newSz;
    return;
  }

  if (preserve && va) {
    double* newVa = new double[newSz];

    if (newSz <= sz) memmove(newVa,va,newSz*sizeof(double));
    else {
      memmove(newVa,va,sz*sizeof(double));
      if (zeroInit) memset(newVa+sz,0,(newSz-sz)*sizeof(double));
    }

    delete[] va;

    va = newVa;
  }
  else {
    delete[] va;
    va = new double[newSz];

    if (zeroInit) memset(va,0,newSz * sizeof(double));
  }
  
  cap = newSz;
  sz  = newSz;
}

//---------------------------------------------------------------------------
// For PVector

Vector::Vector(double *arr, int len)
: va(arr), sz(arr ? len : 0), cap(len)
{
}

//---------------------------------------------------------------------------

Vector::Vector(int vSize, bool zeroInit)
: va(NULL), sz(vSize), cap(vSize)
{
  if (vSize < 0) throw IllegalArgumentException("Vector::Vector");

  va = new double[cap];

  if (zeroInit) memset(va,0,sz*sizeof(double));
}

//---------------------------------------------------------------------------

Vector::Vector(const Vector& cp)
: va(NULL), sz(cp.sz), cap(cp.sz)
{
  va = new double[cap];

  memmove(va,cp.va,sz*sizeof(double));
}

//---------------------------------------------------------------------------

Vector::~Vector()
{
  delete[] va;
}

//---------------------------------------------------------------------------

Vector& Vector::operator=(const Vector& src)
{
  setSize(src.sz,false,false);

  memmove(va,src.va,sz*sizeof(double));

  return *this;
}

//---------------------------------------------------------------------------

void Vector::clear()
{
  memset(va,0,sz*sizeof(double));
}

//---------------------------------------------------------------------------

double Vector::operator[](int idx) const
{
  if (idx < 0 || idx >= sz) throw IndexOutOfBoundsException("Vector::operator[]");

  return va[idx];
}

//---------------------------------------------------------------------------

double& Vector::operator[](int idx)
{
  if (idx < 0 || idx >= sz)
                 throw IndexOutOfBoundsException("Vector::operator[]");

  return va[idx];
}

//---------------------------------------------------------------------------

Vector Vector::operator+(const Vector& v) const
{
  if (sz != v.sz) throw IllegalArgumentException("Vector::operator+");

  Vector result(sz,false);

  for (int i=0; i<sz; i++) result.va[i] = va[i] + v.va[i];

  return result;
}

//---------------------------------------------------------------------------

Vector& Vector::operator+=(const Vector& v)
{
  if (sz != v.sz) throw IllegalArgumentException("Vector::operator+=");

  for (int i=0; i<sz; i++) va[i] += v.va[i];

  return *this;
}

//---------------------------------------------------------------------------

Vector Vector::operator-(const Vector& v) const
{
  if (sz != v.sz) throw IllegalArgumentException("Vector::operator-");

  Vector result(sz,false);

  for (int i=0; i<sz; i++) result.va[i] = va[i] - v.va[i];

  return result;
}

//---------------------------------------------------------------------------

Vector& Vector::operator-=(const Vector& v)
{
  if (sz != v.sz) throw IllegalArgumentException("Vector::operator-=");

  for (int i=0; i<sz; i++) va[i] -= v.va[i];

  return *this;
}

//---------------------------------------------------------------------------

double Vector::operator*(const Vector& v) const
{
  if (sz != v.sz) throw IllegalArgumentException("Vector::operator*");

  double prod = 0.0;

  for (int i=0; i<sz; i++) prod += (va[i] * v.va[i]);

  return prod;
}

//---------------------------------------------------------------------------

Vector Vector::operator*(double fact) const
{
  Vector result(sz,false);

  for (int i=0; i<sz; i++) result.va[i] = va[i] * fact;

  return result;
}

//---------------------------------------------------------------------------

Vector& Vector::operator*=(double fact)
{
  for (int i=0; i<sz; i++) va[i] *= fact;

  return *this;
}

//---------------------------------------------------------------------------

double Vector::len() const
{
  double len = 0.0;

  for (int i=0; i<sz; i++) {
    double val = va[i];
    len += val*val;
  }

  return sqrt(len);
}

//---------------------------------------------------------------------------

double Vector::len(int dims) const
{
  if (dims > sz) throw IllegalArgumentException("Vector::length");

  double len = 0.0;

  for (int i=0; i<dims; i++) {
    double val = va[i];
    len += val*val;
  }

  return sqrt(len);
}

//---------------------------------------------------------------------------

void Matrix::alloc(int rows, int cols, bool setZero)
{
  rws = rows;
  cls = cols;

  delete[] mat;
  delete[] matRows;

  mat     = new double*[rws];
  matRows = new double[rws*cls];

  double *p = matRows;

  for (int i=0; i<rws; i++) {
    mat[i] = p;
    p += cls;
  }

  if (setZero) memset(matRows,0,rws*cls*sizeof(double));
}

//---------------------------------------------------------------------------

Matrix::Matrix(int rows, int columns, double *elems) // For PMatrix
: mat(new double*[rows]), matRows(elems),
  rws(rows), cls(columns)
{
  double *p = matRows;

  for (int i=0; i<rws; i++) {
    mat[i] = p;
    p += cls;
  }
}

//---------------------------------------------------------------------------

Matrix::Matrix(int rows, int cols, bool zeroInit)
: mat(NULL), matRows(NULL), rws(rows), cls(cols)
{
  alloc(rows,cols,zeroInit);
}

//---------------------------------------------------------------------------

Matrix::Matrix(const Matrix& cp)
: mat(new double*[cp.rws]), matRows(new double[cp.rws*cp.cls]),
  rws(cp.rws), cls(cp.cls)
{
  double *p = matRows;

  for (int i=0; i<rws; i++) {
    mat[i] = p;
    p += cls;
  }

  memmove(matRows,cp.matRows,rws*cls*sizeof(double));
}

//---------------------------------------------------------------------------

Matrix::~Matrix()
{
  delete[] matRows;
  delete[] mat;
}

//---------------------------------------------------------------------------

void Matrix::setRows(int rows, bool zeroInit)
{
  if (rows < 1) throw IllegalArgumentException("Matrix::setRows");

  alloc(rows,cls,zeroInit);
}

//---------------------------------------------------------------------------

void Matrix::setColumns(int cols, bool zeroInit)
{
  if (cols < 1) throw IllegalArgumentException("Matrix::setColumns");

  alloc(rws,cols,zeroInit);
}

//---------------------------------------------------------------------------

void Matrix::resize(int rows, int cols, bool zeroInit)
{
  if (rows < 1 || cols < 1) throw IllegalArgumentException("Matrix::resize");

  alloc(rows,cols,zeroInit);
}

//---------------------------------------------------------------------------

Matrix& Matrix::operator=(const Matrix& src)
{
  if (rws != src.rws || cls != src.cls) alloc(src.rws,src.cls,false);

  memmove(matRows,src.matRows,rws*cls*sizeof(double));

  return *this;
}

//---------------------------------------------------------------------------

void Matrix::clear()
{
  memset(matRows,0,rws*cls*sizeof(double));
}

//---------------------------------------------------------------------------

void Matrix::transpose(Matrix& transposedMat) const
{
  if (transposedMat.rws != cls || transposedMat.cls != rws)
    transposedMat.alloc(cls,rws,false);

  for (int i=0; i<rws; ++i) {
    double *row = mat[i];

    for (int j=0; j<cls; ++j) transposedMat.mat[j][i] = row[j];
  }
}

//---------------------------------------------------------------------------

void Matrix::multiply(const Matrix& b, Matrix& result) const
{
  if (b.rws != cls) throw IllegalArgumentException("Matrix::multiply");

  if (result.rws != rws || result.cls != b.cls) result.alloc(rws,b.cls,false);

  for (int i=0; i<rws; ++i) {
    double *row = mat[i];

    for (int j=0; j<b.cls; ++j) {
      double s = 0.0;

      for (int k=0; k<cls; ++k) s += row[k] * b.mat[k][j];

      result.mat[i][j] = s;
    }
  }
}

//---------------------------------------------------------------------------
// Solve the symmetric positve definite banded system mat * x = rhs
// Storage is optimized: only diagonal and upper are stored, set
// the number of colums to the bandwidth.
// Element [i,j] is stored at [i,j-i], where j >= i, i.e. the diagonal is in
// column zero.
//
// Vector rhs contains the solution vector upon return;
//
// Speed is proportional to the number of rows (for small bandwidth)
//
// This method uses LDLT decomposition. On return the diagonal contains
// the elements of D and the uppertriangle contains the off-diagonal elements
// of LT.
//
// As an optimization this routine tries to avoid multiplying zeroes (see lwbIdx)
// This may greatly speed up things if only a few equations actually need the
// matrix bandwidth.

void Matrix::solveLDLT(Vector& rhs)
{
  if (rws < cls)
    throw IllegalArgumentException("Matrix::solveLDLT (rows < columns");

  if (rhs.size() != rws)
    throw IllegalArgumentException("Matrix::solveLDLT (unmatching row counts");

  enum { MaxAllocSz = 4000 };

  int *lwbIdx;
  double *r;

  if (rws > MaxAllocSz) {
    lwbIdx = new int[rws];
    r =      new double[rws];
  }
  else {
    lwbIdx = (int *)_malloca(rws * sizeof(int));
    r      = (double *)_malloca(rws * sizeof(double));
  }

  for (int i=0; i<cls; ++i) lwbIdx[i] = 0;
  for (int i=cls; i<rws; ++i) lwbIdx[i] = i-cls+1;

//  int ops = 0;

  for (int i=0; i<rws; ++i) {
    double dd = mat[i][0];

    for (int j=lwbIdx[i]; j<i; ++j) {
      double m = mat[j][i-j];

      r[j] = mat[j][0] * m;
      dd -= r[j] * m;
    }

    mat[i][0] = dd;

    int upb = std::min(i + cls,rws);

    for (int j=i+1; j<upb; ++j) {
      double& m = mat[i][j-i];

      int st = std::max(lwbIdx[i],lwbIdx[j]);

      for (int k=st;k<i; ++k) {
        m -= mat[k][j-k] * r[k];
//        ops++;
      }

      m /= dd;

      if (fabs(m) < 1e-12) {
        if (lwbIdx[j] == i) lwbIdx[j]++;
      }
    }
  }

  for (int i=0; i<rws; ++i) {
    for (int j=lwbIdx[i]; j<i; ++j) rhs[i] -= mat[j][i-j] * rhs[j];
  }

  for (int i=0; i<rws; ++i) rhs[i] /= mat[i][0];

  for (int i=rws-1; i>=0; --i) {
    int upb = std::min(i + cls,rws);

    for (int j=i+1; j<upb; ++j) rhs[i] -= mat[i][j-i] * rhs[j];
  }

  if (rws > MaxAllocSz) {
    delete[] lwbIdx;
    delete[] r;
  }

//  ops += 0; // Put breakpoint here
}

//---------------------------------------------------------------------------
// Solve the symmetric positve definite banded system mat * x = rhs
// Storage is optimized: only diagonal and upper are stored, set
// the number of colums to the bandwidth.
// Element [i,j] is stored at [i,j-i], where j >= i, i.e. the diagonal is in
// column zero.
//
// Matrix rhs contains the solution vector(s) upon return;
//
// Speed is proportional to the number of rows (for small bandwidth)
//
// This method uses LDLT decomposition. On return the diagonal contains
// the elements of D and the uppertriangle contains the off-diagonal elements
// of LT.
//
// As an optimization this routine tries to avoid multiplying zeroes (see lwbIdx)
// This may greatly speed up things if only a few equations actually need the
// matrix bandwidth.

void Matrix::solveLDLT(Matrix& rhs)
{
  if (rws < cls)
    throw IllegalArgumentException("Matrix::solveLDLT (less rows than columns");

  if (rhs.rws != rws)
    throw IllegalArgumentException("Matrix::solveLDLT (unmatching row counts");

  enum { MaxAllocSz = 4000 };

  int *lwbIdx;
  double *r;

  if (rws > MaxAllocSz) {
    lwbIdx = new int[rws];
    r =      new double[rws];
  }
  else {
    lwbIdx = (int *)_malloca(rws * sizeof(int));
    r      = (double *)_malloca(rws * sizeof(double));
  }

  for (int i=0; i<cls; ++i) lwbIdx[i] = 0;
  for (int i=cls; i<rws; ++i) lwbIdx[i] = i-cls+1;

  for (int i=0; i<rws; ++i) {
    double dd = mat[i][0];

    for (int j=lwbIdx[i]; j<i; ++j) {
      double m = mat[j][i-j];

      r[j] = mat[j][0] * m;
      dd -= r[j] * m;
    }

    mat[i][0] = dd;

    int upb = std::min(i + cls,rws);

    for (int j=i+1; j<upb; ++j) {
      double& m = mat[i][j-i];

      int st = std::max(lwbIdx[i],lwbIdx[j]);

      for (int k=st;k<i; ++k) m -= mat[k][j-k] * r[k];

      m /= dd;

      if (fabs(m) < 1e-12) {
        if (lwbIdx[j] == i) lwbIdx[j]++;
      }
    }
  }

  int rhsColSz = rhs.getColumns();

  for (int i=0; i<rws; ++i) {
    for (int j=lwbIdx[i]; j<i; ++j) {
      double m = mat[j][i-j];

      for (int k=0; k<rhsColSz; ++k) rhs(i,k) -= m * rhs(j,k);
    }
  }

  for (int i=0; i<rws; ++i) {
    double d = mat[i][0];
    for (int j=0; j<rhsColSz; ++j) rhs(i,j) /= d;
  }

  for (int i=rws-1; i>=0; --i) {
    int upb = std::min(i + cls,rws);

    for (int j=i+1; j<upb; ++j) {
      double m = mat[i][j-i];

      for (int k=0; k<rhsColSz; ++k) rhs(i,k) -= m * rhs(j,k);
    }
  }

  if (rws > MaxAllocSz) {
    delete[] lwbIdx;
    delete[] r;
  }
}

} // namespace Ino

//---------------------------------------------------------------------------
