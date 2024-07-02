//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//-------------- Singular Value Decomposition -------------------------------
//-------------- (Matrix Equation Solver) -----------------------------------
//---------------------------------------------------------------------------
//------------------------ Copyright Inofor Hoek Aut BV 1991..1999 ----------
//------------------------------------------------------ C.Wolters ----------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "Svd.h"

#include "Basics.h"
#include "Exceptions.h"
// #include "Matrix.h"

#include <stddef.h>
#include <math.h>

namespace Ino
{

const double Double_Precision = 4.440892098500626E-16;

//----------------------------------------------------------------------------

static void unitMat(DMat mat,int m, int n)
{
  for (int i=0; i<m; i++) {
    DVec row = mat[i];

    for (int j=0; j<n; j++) {
      if (i==j) row[j] = 1.0;
      else      row[j] = 0.0;
    }
  }
}

//----------------------------------------------------------------------------

static void houseHoldRow(DMat mat, int row, int k, int j,
                                                 double& vk, double& beta)
{
  DVec vec = mat[row];

  vk = vec[k];

  double vkMax = fabs(vk);

  for (int i=k+1; i<j; i++) {
    double h = fabs(vec[i]);
    if (h > vkMax) vkMax = h;
  }

  double alpha = 0;
  beta = 0;
  if (vkMax <= 0) return;

  vk /= vkMax;

  alpha = sqr(vk);

  for (int i=k+1; i<j; i++) {
    double& vi = vec[i];

    vi /= vkMax;
    alpha += sqr(vi);
  }

  alpha = sqrt(alpha);

  beta = alpha*(alpha+fabs(vk));

  if (vk > 0) vk += alpha;
  else        vk -= alpha;

  alpha *= vkMax;

  if (vk > 0) vec[k] = -alpha;
  else        vec[k] =  alpha;
}

//----------------------------------------------------------------------------

static void houseHoldCol(DMat mat, int col, int k, int j,
                                                  double& vk, double& beta)
{
  vk = mat[k][col];

  double vkMax = fabs(vk);

  for (int i=k+1; i<j; i++) {
    double h = fabs(mat[i][col]);
    if (h > vkMax) vkMax = h;
  }

  double alpha = 0;
  beta = 0;
  if (vkMax <= 0) return;

  vk /= vkMax;

  alpha = sqr(vk);

  for (int i=k+1; i<j; i++) {
    double& vi = mat[i][col];

    vi /= vkMax;
    alpha += sqr(vi);
  }

  alpha = sqrt(alpha);

  beta = alpha*(alpha+fabs(vk));

  if (vk > 0) vk += alpha;
  else        vk -= alpha;

  alpha *= vkMax;

  if (vk > 0) mat[k][col] = -alpha;
  else        mat[k][col] =  alpha;
}

//----------------------------------------------------------------------------

static void houseUpdCols(DMat mat, int m, int n, DMat cMat, int col,
                                      int k, int j, double vk, double b)
{
  if (fabs(b) <= 0) return;

  for (int i1=m; i1<n; i1++) {
    double s = mat[k][i1] * vk;

    for (int i2=k+1; i2<j; i2++) s += mat[i2][i1] * cMat[i2][col];

    s /= b;
    mat[k][i1] -= s * vk;

    for (int i2=k+1; i2<j; i2++) mat[i2][i1] -= s * cMat[i2][col];
  }
}

//----------------------------------------------------------------------------

static void houseUpdRows(DMat mat, int m, int n, DMat rMat, int row,
                                      int k, int j, double vk, double b)
{
  if (fabs(b) <= 0) return;

  DVec vec = rMat[row];

  for (int i1=m; i1<n; i1++) {
    DVec rowVec = mat[i1];

    double s = rowVec[k] * vk;

    for (int i2=k+1; i2<j; i2++) s += rowVec[i2] * vec[i2];

    s /= b;
    rowVec[k] -= s * vk;

    for (int i2=k+1; i2<j; i2++) rowVec[i2] -= s * vec[i2];
  }
}

//----------------------------------------------------------------------------

static void givensBlock(double y, double z, double& c, double& s)
{
  double ay = fabs(y), az = fabs(z);

  if (az <= 0.0) {
    c = 1.0; s = 0.0;
  }
  else {
    if (az >= ay) {
      double t = y/z; s = 1/sqrt(1+sqr(t)); c = s * t;
    }
    else {
      double t = z/y; c = 1.0/sqrt(1.0+sqr(t)); s = c * t;
    }
  }
}

//----------------------------------------------------------------------------

static void givensUpdCols(DMat mat, int m, int n, int j, int k,
                                                        double c, double s)
{
  for (int i=m; i<=n; i++) {
    double v = mat[j][i];
    double w = mat[k][i];

    mat[j][i] =  c*v+s*w;
    mat[k][i] = -s*v+c*w;
  }
}

//----------------------------------------------------------------------------

static void givensUpdRows(DMat mat, int m, int n, int j, int k,
                                                          double c, double s)
{
  for (int i=m; i<=n; i++) {
    DVec row = mat[i];

    double v = row[j];
    double w = row[k];

    row[j] =  c*v+s*w;
    row[k] = -s*v+c*w;
  }
}

//----------------------------------------------------------------------------

bool UpperTriang(DMat mat, int m, int n, DMat u, bool fullU,
                 DMat rhs, int rhsCols, ProgressReporter *rep)
{
  DVec vkvec = NULL, bvec = NULL;
  int *upbVec = NULL;

  if (u != NULL) {
    vkvec = new double[n];
    bvec  = new double[n];
    upbVec = new int[n];
  }

  for (int i=0; i<n; i++) {
    int upb = m-1;

    while (upb > i) {
      if (mat[upb][i] == 0.0) upb-- ;
      else break;
    }
    upb++;

    double b,vk;
    houseHoldCol(mat,i,i,upb,vk,b);
    houseUpdCols(mat,i+1,n,mat,i,i,upb,vk,b);

    if (rhs != NULL) houseUpdCols(rhs,0,rhsCols,mat,i,i,upb,vk,b);

    if (u != NULL) {
      vkvec[i] = vk;
      bvec[i] = b;
      upbVec[i] = upb;
    }

    if (rep && !rep->incProgress()) return false;
  }

  if (u != NULL) {
    int upbU = n;
    if (fullU) upbU = m;

    unitMat(u,upbU,n);

    for (int i=n-1; i>=0; i--) {
      if (fullU) upbU = m;
      else       upbU = upbVec[i];
      houseUpdCols(u,i,n,mat,i,i,upbU,vkvec[i],bvec[i]);
    }

    delete[] bvec;
    delete[] vkvec;
    delete[] upbVec;
  }

  return true;
}

//----------------------------------------------------------------------------

static void upperBiDiag(DMat mat, int m, int n, DMat u, bool fullU,
                                           DMat vt, DMat rhs, int rhscols)
{
  DVec lvkvec = NULL, lbvec = NULL;

  if (u != NULL) {
    lvkvec = new double[n];
    lbvec  = new double[n];
  }

  DVec rvkvec = NULL, rbvec = NULL;

  if (vt != NULL) {
    rvkvec = new double[n];
    rbvec  = new double[n];
  }
 
  for (int i=0; i<n; i++) {
    double b,vk;

    houseHoldCol(mat,i,i,m,vk,b);
    houseUpdCols(mat,i+1,n,mat,i,i,m,vk,b);

    if (rhs != NULL) houseUpdCols(rhs,0,rhscols,mat,i,i,m,vk,b);

    if (u != NULL) {
      lvkvec[i] = vk;
      lbvec[i]  = b;
    }
  
    if (i < n-2) {
      houseHoldRow(mat,i,i+1,n,vk,b);
      houseUpdRows(mat,i+1,m,mat,i,i+1,n,vk,b);

      if (vt != NULL) {
        rvkvec[i] = vk;
        rbvec[i]  = b;
      }
    }
  }  /* for */

  if (vt != NULL) {
      unitMat(vt,n,n);

      if (n >= 3) {
        for (int i=n-3; i>=0; i--) {
          houseUpdRows(vt,i,n,mat,i,i+1,n,rvkvec[i],rbvec[i]);
        }
      }

      delete[] rbvec;
      delete[] rvkvec;
  }

  if (u != NULL) {
      int upb = n;
      if (fullU) upb = m;

      unitMat(u,upb,n);

      for (int i=n-1; i>=0; i--)
             houseUpdCols(u,i,n,mat,i,i,upb,lvkvec[i],lbvec[i]);

      delete[] lbvec;
      delete[] lvkvec;
   }
}

//----------------------------------------------------------------------------

static void SVD_Step(DMat mat, int lwb, int upb, int m, int n,
                                     DMat u, DMat vt, DMat rhs, int rhsCols)
{
  double c = mat[upb-1][upb-1];
  double s = mat[upb][upb];
  double y = mat[upb-1][upb];

  double z;
  if (upb > 1) z = mat[upb-2][upb-1]; /* IF statement necessary but implication not tested */
  else         z = 0;

  z = sqr(z) + sqr(c);
  s = sqr(s) + sqr(y);

  double r = (z-s)/2;
  y = sqrt(sqr(r) + sqr(c*y));

  if (r >= 0) y = s + r - y;
  else        y = s + r + y;

  c = mat[lwb][lwb];

  y = sqr(c) - y;
  z = c * mat[lwb][lwb+1];

  for (int i=lwb; i<upb; i++) {
    givensBlock(y,z,c,s);

    if (i == lwb) givensUpdRows(mat,  i,i+1,i,i+1,c,s);
    else          givensUpdRows(mat,i-1,i+1,i,i+1,c,s);

    if (vt != NULL) givensUpdCols(vt,0,n-1,i,i+1,c,s);

    y = mat[i][i];
    z = mat[i+1][i];
    givensBlock(y,z,c,s);

    if (i < upb-1) givensUpdCols(mat,i,i+2,i,i+1,c,s);
    else           givensUpdCols(mat,i,i+1,i,i+1,c,s);

    if (u != NULL) givensUpdRows(u,0,m-1,i,i+1,c,s);

    if (rhs != NULL) givensUpdCols(rhs,0,rhsCols-1,i,i+1,c,s);

    if (i < upb-1) {
      y = mat[i][i+1];
      z = mat[i][i+2];
    }
  }
}

//----------------------------------------------------------------------------

static double biDiagMax(DMat mat, int lwb, int upb)
{
  double mx = mat[lwb][lwb];

  for (int i=lwb+1; i<upb; i++) {
    double a = fabs(mat[i-1][i]);
    if (a > mx) mx = a;

    a = fabs(mat[i][i]);
    if (a > mx) mx = a;
  }

  return mx;
}

//----------------------------------------------------------------------------

static double diagMax(DMat mat, int lwb, int upb)
{
  double mx = 0;

  for (int i=lwb; i<upb; i++) {
    double a = fabs(mat[i][i]);
    if (a > mx) mx = a;
  }

  return mx;
}

//----------------------------------------------------------------------------

static double locTol = 10 * Double_Precision;

bool Svd(DMat mat, int m, int n, DMat u, bool fullU, DMat vt,
                           DMat rhs, int rhsCols, int& iter)
{
  if (m < n || n < 1) throw IllegalArgumentException("Svd solver");

  if ((m > 1.67*n) && (u == NULL)) {
    UpperTriang(mat,m,n,NULL,false,rhs,rhsCols);

    for (int i=0; i<n; i++) {
      for (int j=i+1; j<m; j++) mat[j][i] = 0.0;
    }

    upperBiDiag(mat,n,n,u,fullU,vt,rhs,rhsCols);
  }
  else upperBiDiag(mat,m,n,u,fullU,vt,rhs,rhsCols);

  int lwb = 0;
  int upb = n-1;
  int loops = iter;

  bool checkloop = loops > 0;

  if (loops < 2*n) loops = 2*n;

  iter = 0;

  do {
    if (checkloop && (iter > loops)) return false;

    for (int i=lwb; i<upb; i++)   mat[i+1][i] = 0;
    for (int i=lwb; i<upb-1; i++) mat[i][i+2] = 0;

    double a = fabs(mat[0][0]);

    for (int i=lwb+1; i<=upb; i++) {
      double b = fabs(mat[i][i]);
      if (fabs(mat[i-1][i]) <= locTol*(a+b)) mat[i-1][i] = 0;
      a = b;
    }

    while (upb > 0) {
      if (mat[upb-1][upb] != 0) break;
      upb-- ;
    }

    if (upb < 1) break;

    lwb = upb-1;
    while (lwb > 0) {
      if (mat[lwb-1][lwb] == 0) break;
      lwb-- ;
    }

    double mx  = biDiagMax(mat,0,upb+1);
    double mx2 = diagMax(mat,upb+1,n);
    if (mx2 > mx) mx = mx2;

    double norm = locTol*mx;

    bool decoupled = false;
    for (int i=lwb; i<upb; i++) {
      if (fabs(mat[i][i]) < norm) {
        decoupled = true;
        mat[i][i+1] = 0;
      }
    }

    if (decoupled) continue;

    iter++ ;
    SVD_Step(mat,lwb,upb,m,n,u,vt,rhs,rhsCols);
  } while (upb > 0);

  return true;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

static inline double sgn(double x, double y)
{
  if (y >= 0.0) return fabs(x);
  else return -fabs(x);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

static void slartG(double f, double g, double& c, double& s)
{
  if (f == 0.0 && g == 0.0) {
    c = 1.0;
    s = 0.0;

    return;
  }

  if (fabs(f) > fabs(g)) {
    double t  = g/f;
    double tt = sqrt(1.0 + t*t);

    c = 1.0/tt;
    s = t * c;
  }
  else {
    double t  = f/g;
    double tt = sqrt(1.0 + t*t);

    s = 1.0/tt;
    c = t * s;
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

static void slashV2(double f, double g, double h, double& sMax, double& sMin,
                                 double& sl, double& cl, double& sr, double& cr)
{
  double ft = f;
  double fa = fabs(ft);

  double ht = h;
  double ha = fabs(h);

  int pMax = 1;

  bool swap = ha > fa;

  if (swap) {
    pMax = 3;

    double s = ft;
    ft = ht;
    ht = s;

    s = fa;
    fa = ha;
    ha = s;
  }

  double clt = 0.0, crt = 0.0, slt = 0.0, srt = 0.0;

  double gt = g;
  double ga = fabs(gt);

  if (ga == 0.0) {
    sMin = ha;
    sMax = fa;

    clt = 1.0;
    crt = 1.0;
    slt = 0.0;
    srt = 0.0;
  }
  else {
    bool gasMal = true;

    if (ga > fa) {
      pMax = 2;

      if (fabs(fa/ga) < 1e-17) {
        gasMal = false;
        sMax = ga;

        if (ha > 1.0) sMin = fa / (ga / ha);
        else sMin = (fa/ga) * ha;

        clt = 1.0;
        slt = ht/gt;
        srt = 1.0;
        crt = ft/gt;
      }
    }

    if (gasMal) {
      double d = fa - ha;
      
      double l,r;

      if (d == fa) l = 1.0;
      else l = d /fa;

      double m = gt/ft;
      double t = 2.0 - l;
      double mm = m*m;
      double tt = t*t;

      double s = sqrt(tt+mm);

      if (l == 0.0) r = fabs(m);
      else r = sqrt(l*l + mm);

      double a = 0.5 * (s+r);
      sMin = ha/a;
      sMax = fa*a;

      if (mm == 0.0) {
        if (l == 0.0) t = sgn(2.0,ft)*sgn(1.0,gt);
        else t = gt/sgn(d,ft) + m/t;
      }
      else t = (m/(s+t)+m/(r+l))*(1.0+a);

      l = sqrt(t*t+4.0);
      crt = 2.0/l;
      srt = t/l;
      clt = (crt + srt*m)/a;
      slt = (ht/ft)*srt/a;
    }
  }

  if (swap) {
    cl = srt;
    sl = crt;
    cr = slt;
    sr = clt;
  }
  else {
    cl = clt;
    sl = slt;
    cr = crt;
    sr = srt;
  }

  double tSgn = 0.0;

  if (pMax == 1) tSgn = sgn(1.0,cr)*sgn(1.0,cl)*sgn(1.0,f);
  if (pMax == 2) tSgn = sgn(1.0,sr)*sgn(1.0,cl)*sgn(1.0,g);
  if (pMax == 3) tSgn = sgn(1.0,sr)*sgn(1.0,sl)*sgn(1.0,h);

  sMax = sgn(sMax,tSgn);
  sMin = sgn(sMin,tSgn*sgn(1.0,f)*sgn(1.0,h));
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

static void gsvd22Upper(int i, int j, DMat matA, DMat matB,
                             double u[2][2], double v[2][2], double q[2][2])
{
  double c11 = matA[i][i] * matB[j][j];
  double c12 = matA[i][j] * matB[i][i] - matA[i][i] * matB[i][j];
  double c22 = matA[j][j] * matB[i][i];

  double sMax,sMin,sl,cl,sr,cr;
  slashV2(c11,c12,c22,sMax,sMin,sl,cl,sr,cr);

  u[0][0] =  cl; u[0][1] = sl;
  u[1][0] = -sl; u[1][1] = cl;

  v[0][0] =  cr; v[0][1] = sr;
  v[1][0] = -sr; v[1][1] = cr;

  double cq,sq;

  if (fabs(cl) >= fabs(sl) || fabs(cr) >= fabs(sr)) {
    double u00 =  cl * matA[i][i] + sl * matA[j][i];
    double u01 =  cl * matA[i][j] + sl * matA[j][j];
    double v00 =  cr * matB[i][i] + sr * matB[j][i];
    double v01 =  cr * matB[i][j] + sr * matB[j][j];
    double ua01 = fabs(cl*matA[i][j]) + fabs(sl*matA[j][j]);
    double va01 = fabs(cr*matB[i][j]) + fabs(sr*matB[j][j]);

    if (ua01*(fabs(v00)+fabs(v01)) <= va01*(fabs(u00)+fabs(u01)))
         slartG(-u00,u01,cq,sq);
    else slartG(-v00,v01,cq,sq);
  }
  else {
    double u10 = -sl * matA[i][i] + cl * matA[j][i];
    double u11 = -sl * matA[i][j] + cl * matA[j][j];
    double v10 = -sr * matB[i][i] + cr * matB[j][i];
    double v11 = -sr * matB[i][j] + cr * matB[j][j];
    double ua11 = fabs(sl*matA[i][j]) + fabs(cl*matA[j][j]);
    double va11 = fabs(sr*matB[i][j]) + fabs(cr*matB[j][j]);

    if (ua11*(fabs(v10)+fabs(v11)) <= va11*(fabs(u10)+fabs(u11)))
         slartG(-u10,u11,cq,sq);
    else slartG(-v10,v11,cq,sq);

    // Swap:
    double h;

    h = u[0][0]; u[0][0] = u[1][0]; u[1][0] = -h;
    h = u[0][1]; u[0][1] = u[1][1]; u[1][1] = -h;

    h = v[0][0]; v[0][0] = v[1][0]; v[1][0] = -h;
    h = v[0][1]; v[0][1] = v[1][1]; v[1][1] = -h;
  }

  q[0][0] =  cq; q[0][1] = sq;
  q[1][0] = -sq; q[1][1] = cq;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

static void gsvd22Lower(int i, int j, DMat matA, DMat matB,
                             double u[2][2], double v[2][2], double q[2][2])
{
  double c11 = matA[i][i] * matB[j][j];
  double c21 = matA[j][i] * matB[j][j] - matA[j][j] * matB[j][i];
  double c22 = matA[j][j] * matB[i][i];

  double sMax,sMin,sl,cl,sr,cr;
  slashV2(c11,c21,c22,sMax,sMin,sr,cr,sl,cl);

  u[0][0] =  cl; u[0][1] = sl;
  u[1][0] = -sl; u[1][1] = cl;

  v[0][0] =  cr; v[0][1] = sr;
  v[1][0] = -sr; v[1][1] = cr;

  double cq,sq;

  if (fabs(cl) >= fabs(sl) || fabs(cr) >= fabs(sr)) {
    double u10 = u[1][0]*matA[i][i] + u[1][1]*matA[j][i];
    double u11 = u[1][0]*matA[i][j] + u[1][1]*matA[j][j];
    double v10 = v[1][0]*matB[i][i] + v[1][1]*matB[j][i];
    double v11 = v[1][0]*matB[i][j] + v[1][1]*matB[j][j];
    double ua10 = fabs(u[1][0]*matA[i][i]) + fabs(u[1][1]*matA[j][i]);
    double va10 = fabs(v[1][0]*matB[i][i]) + fabs(v[1][1]*matB[j][i]);

    if (ua10*(fabs(v10)+fabs(v11)) <= va10*(fabs(u10)+fabs(u11)))
         slartG(u11,u10,cq,sq);
    else slartG(v11,v10,cq,sq);
  }
  else {
    double u00 =  u[0][0]*matA[i][i] + u[0][1]*matA[j][i];
    double u01 =  u[0][0]*matA[i][j] + u[0][1]*matA[j][j];
    double v00 =  v[0][0]*matB[i][i] + v[0][1]*matB[j][i];
    double v01 =  v[0][0]*matB[i][j] + v[0][1] *matB[j][j];
    double ua00 = fabs(u[0][0]*matA[i][i]) + fabs(u[0][1]*matA[j][i]);
    double va00 = fabs(v[0][0]*matB[i][i]) + fabs(v[0][1]*matB[j][i]);

    if (ua00*(fabs(v00)+fabs(v01)) <= va00*(fabs(u00)+fabs(u01)))
         slartG(u01,u00,cq,sq);
    else slartG(v01,v00,cq,sq);

    // Swap:
    double h;

    h = u[0][0]; u[0][0] = u[1][0]; u[1][0] = -h;
    h = u[0][1]; u[0][1] = u[1][1]; u[1][1] = -h;

    h = v[0][0]; v[0][0] = v[1][0]; v[1][0] = -h;
    h = v[0][1]; v[0][1] = v[1][1]; v[1][1] = -h;
  }

  q[0][0] =  cq; q[0][1] = sq;
  q[1][0] = -sq; q[1][1] = cq;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

void preMultBlk(double p[2][2], int colLwb, int colUpb, int i1, int i2, DMat mat)
{
  for (int i=colLwb; i<colUpb; ++i) {
    double& mat1 = mat[i1][i];
    double& mat2 = mat[i2][i];

    double m1 = p[0][0] * mat1 + p[0][1] * mat2;
    double m2 = p[1][0] * mat1 + p[1][1] * mat2;

    mat1 = m1;
    mat2 = m2;
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

void postMultBlk(int rowLwb, int rowUpb, int i1, int i2, DMat mat, double p[2][2])
{
  for (int i=rowLwb; i<rowUpb; ++i) {
    double& mat1 = mat[i][i1];
    double& mat2 = mat[i][i2];

    double m1 = mat1 * p[0][0] + mat2 * p[1][0];
    double m2 = mat1 * p[0][1] + mat2 * p[1][1];

    mat1 = m1;
    mat2 = m2;
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

static bool gsvdSweep(bool even, DMat matA, DMat matB, int cols,
                      DMat rhsA, DMat rhsB, int rhsCols, DMat Q,
                      ProgressReporter *rep)
{
  double u[2][2], v[2][2], q[2][2];

  for (int i=0; i<cols-1; ++i) {
    for (int j=i+1; j<cols; ++j) {
      if (even) {
        gsvd22Upper(i,j,matA,matB,u,v,q);

        preMultBlk(u,0,i+1,i,j,matA);
        preMultBlk(u,j,cols,i,j,matA);
        postMultBlk(i,j+1,i,j,matA,q);

        matA[i][j] = 0.0;

        preMultBlk(v,0,i+1,i,j,matB);
        preMultBlk(v,j,cols,i,j,matB);
        postMultBlk(i,j+1,i,j,matB,q);

        matB[i][j] = 0.0;
      }
      else {
        gsvd22Lower(i,j,matA,matB,u,v,q);

        preMultBlk(u,i,j+1,i,j,matA);
        postMultBlk(0,i+1,i,j,matA,q);
        postMultBlk(j,cols,i,j,matA,q);

        matA[j][i] = 0.0;

        preMultBlk(v,i,j+1,i,j,matB);
        postMultBlk(0,i+1,i,j,matB,q);
        postMultBlk(j,cols,i,j,matB,q);

        matB[j][i] = 0.0;
      }
      
      preMultBlk(u,0,rhsCols,i,j,rhsA);
      preMultBlk(v,0,rhsCols,i,j,rhsB);
      postMultBlk(0,cols,i,j,Q,q);
    }

    if (rep && !rep->incProgress()) return false;
  }

  return true;
}

//-------------------------------------------------------------------------------

static double checkDeviation(DMat matA, DMat matB, int cols, DMat hlpMat)
{
  double deviation = 0.0;

  for (int i=0; i<cols-1; ++i) {
    int sz = 0;

    double la = 0.0, lb = 0.0;

    for (int j=i; j<cols; ++j) {
      la += sqr(matA[i][j]);
      lb += sqr(matB[i][j]);
    }

    la = sqrt(la); lb = sqrt(lb);

    if (la == 0.0 || lb == 0.0) continue;

    for (int j=i; j<cols; ++j) {
      hlpMat[sz][0] = matA[i][j]/la;
      hlpMat[sz][1] = matB[i][j]/lb;
      ++sz;
    }

    double vk,beta;
    houseHoldCol(hlpMat,0,0,sz,vk,beta);
    houseUpdCols(hlpMat,1,2,hlpMat,0,0,sz,vk,beta);
    houseHoldCol(hlpMat,1,1,sz,vk,beta);

    double sMax,sMin,sl,cl,sr,cr;
    slashV2(hlpMat[0][0],hlpMat[0][1],hlpMat[1][1],sMax,sMin,sr,cr,sl,cl);

    deviation += fabs(sMin);
  }

  return deviation/cols;
}

//-------------------------------------------------------------------------------

static void calcGsvs(DMat matA, DMat matB, int cols, DVec alpha, DVec beta)
{
  for (int i=0; i<cols; ++i) {
    double la = 0.0, lb = 0.0, inprod = 0.0;

    for (int j=i; j<cols; ++j) {
      la += sqr(matA[i][j]);
      lb += sqr(matB[i][j]);
      inprod += matA[i][j] * matB[i][j];
    }

    la = sqrt(la); lb = sqrt(lb);

    if (la != 0.0) {
      for (int j=i; j<cols; ++j) matA[i][j] /= la;
    }

    if (lb != 0.0) {
      if (inprod < 0.0) lb = -lb;
      for (int j=i; j<cols; ++j) matB[i][j] /= lb;
    }

    if (la == 0.0 && lb != 0.0) {
      for (int j=i; j<cols; ++j) matA[i][j] = matB[i][j];
    }

    alpha[i] = la; beta[i] = lb;
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

bool GSvd2(DMat matA, DMat matB, int rowsA, int rowsB, int cols,
           DMat rhsA, DMat rhsB, int rhsCols, double tol,
           DMat q, DVec alpha, DVec beta, ProgressReporter *rep)
{
  tol = fabs(tol);

  if (rowsA < cols || rowsB < cols || cols < 1)
                          throw IllegalArgumentException("GSvd solver");

  if (!UpperTriang(matA,rowsA,cols,NULL,false,rhsA,rhsCols,rep)) return false;
  if (!UpperTriang(matB,rowsB,cols,NULL,false,rhsB,rhsCols,rep)) return false;

  for (int i=0; i<cols; ++i) {
    for (int j=i+1; j<rowsA; ++j) matA[j][i] = 0.0;
    for (int j=i+1; j<rowsB; ++j) matB[j][i] = 0.0;
  }

  DMat hlpMat = new double *[cols];
  double *hlpRows = new double[cols*2];

  for (int i=0; i<cols; ++i) hlpMat[i] = hlpRows+i*2;

  unitMat(q,cols,cols);

  bool ok = false;

  for (int i=0; i<40; ++i) {
    if (!gsvdSweep(true,matA,matB,cols,rhsA,rhsB,rhsCols,q,rep)) return false;
    if (!gsvdSweep(false,matA,matB,cols,rhsA,rhsB,rhsCols,q,rep)) return false;

    // Check convergence
    double dev = checkDeviation(matA,matB,cols,hlpMat);

    if (dev < tol) {
      calcGsvs(matA,matB,cols,alpha,beta);

      ok = true;
      break;
    }
  }

  delete[] hlpMat;
  delete[] hlpRows;

  return ok;
}

} // namespace Ino

//----------------------------------------------------------------------------
