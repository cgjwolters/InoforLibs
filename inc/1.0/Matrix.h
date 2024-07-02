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

  bool solveLs(int rows, int cols, double **vt,
               double **rhs, int rhs_cols, double **sol,
                                double relTol, int& rank, int& svd_iter);


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

  void upperTriang(int rows, int cols, Matrix& rhs, int rhsCols,
                                             Ino::ProgressReporter *rep=NULL);

  bool solveSvd(int rows, int cols, Matrix& vt, int& rank, int& svd_iter);

  bool solveSvd(int rows, int cols, Matrix& vt, Matrix& u,
                                      bool fullU, int& rank, int& svd_iter);

  bool solveLs(int rows, int cols, Matrix& vt, Vector& rhs, Vector& sol,
                                   double relTol, int& rank, int& svd_iter);

  bool solveLs(int rows, int cols, Matrix& vt, Matrix& rhs, Matrix& sol,
                                   double relTol, int& rank, int& svd_iter);

  bool invertGauss(Matrix& invMat);
  bool invertMoorePenrose(Matrix& invMat);

  void solveLDLT(Vector& b);
  void solveLDLT(Matrix& rhs);

  static bool gSvd2(Matrix& matA, Matrix& matB,
                    int rowsA, int rowsB, int cols,
                    Matrix& rhsA, Matrix& rhsB, int rhsCols, double tol,
                    Matrix& q, Vector& alpha, Vector& beta,
                    Ino::ProgressReporter *rep=NULL);

  friend class PMatrix;
};

} // namespace Ino

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif

