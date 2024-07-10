//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//-------------- General Matrix and Vector Definitions ----------------------
//---------------------------------------------------------------------------
//------------------------ Copyright Inofor Hoek Aut BV 1991..1999 ----------
//------------------------------------------------------ C.Wolters ----------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef MATRIX_INC
#define MATRIX_INC

#include "Basics.h"

namespace Ino
{

//---------------------------------------------------------------------------

 class Matrix;

class Vector
{
  double *va;
  int sz;
  int cap;

  Vector(double *arr, int len); // For PVector

public:
  Vector(int size, bool zeroInit=true);
  Vector(const Vector& cp);
  ~Vector();

  Vector& operator=(const Vector& src);

  void setSize(int newSz, bool preserve = false, bool zeroInit = true);
  int size() const { return sz; }

  void clear();

  double  operator[](int idx) const;
  double& operator[](int idx);

  Vector  operator+(const Vector& v) const;
  Vector& operator+=(const Vector& v);

  Vector  operator-(const Vector& v) const;
  Vector& operator-=(const Vector& v);

  Vector  operator*(double fact) const;
  Vector& operator*=(double fact);

  double operator*(const Vector& v) const;

  double len() const;
  double len(int dims) const;

  friend class Matrix;
  friend class PVector;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class Matrix
{
  double **mat;
  double  *matRows;
  int    rws, cls;

  void alloc(int rows, int cols, bool setZero = true);

  Matrix(int rows, int columns, double *elems); // For PMatrix

public:
  Matrix(int rows, int cols, bool zeroInit=true);
  Matrix(const Matrix& cp);
  ~Matrix();

  void setRows(int rows, bool zeroInit=true);
  void setColumns(int cols, bool zeroInit=true);

  void resize(int rows, int cols, bool zeroInit=true);

  Matrix& operator=(const Matrix& src);

  int getRows() const { return rws; }
  int getColumns() const { return cls; }

  void clear();

  double& operator()(int r, int c) { return mat[r][c]; }
  double  operator()(int r, int c) const { return mat[r][c]; }

  const double *row(int r) const { return mat[r]; } // Optimiser
  double *row(int r)  { return mat[r]; }            // Optimiser

  void transpose(Matrix& transposedMat) const;
  void multiply(const Matrix& b, Matrix& result) const;

  void solveLDLT(Vector& b);
  void solveLDLT(Matrix& rhs);
};

} // namespace Ino

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif

