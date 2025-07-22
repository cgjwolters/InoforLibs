// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// ------------ Vector and matrix manipulations 2D and 3D ------------------
// -------------------------------------------------------------------------
// ---------------- Inofor Hoek Automatisering BV---------------------------
// -------------------------------------------------------------------------
// ------------------- Copyright C. Wolters Aug 1996 -----------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#include "Trf.h"

#include "Exceptions.h"

#include <cmath>

using namespace std;

namespace Ino
{

// -------------------------------------------------------------------------
/** \addtogroup geometry 2D/3D Geometry
  @{
*/

// -------------------------------------------------------------------------
/** \file Trf.h
  Defines basic \ref Ino::Vec2 "2D" and \ref Ino::Vec3 "3D" vector transformations.

  The transformations are based on the use of <em>homogeneous
  coordinates </em>.\n

  \see \ref Ino::Vec2 "Vec2"

  \author C. Wolters
  \date Oct 1996
*/


// -------------------------------------------------------------------------
// ------- 2D Transformations ----------------------------------------------
// -------------------------------------------------------------------------
/** \class Trf2
  Defines a 2D transformation matrix.

  The transformation is based on the use of <em>homogeneous
  coordinates</em>, so the transformation is 2x3 matrix.\n
  \n

  A transform can be in one of two modes:
  \li \ref isDerivative is \c false, the default.\n
  In this case the virtual matrix element [2,2] = 1.0.
  \li \ref isDerivative is \c true.\n
  In this case the virtual matrix element [2,2] = 0.0.

  \see \ref Vec2::transform2(const Trf2&) "Vec2::transform2".
  \author C. Wolters
  \date Aug 1996
*/

// -------------------------------------------------------------------------
/**
  @}
*/

// -------------------------------------------------------------------------
/** \var bool Trf2::isDerivative
  Defines the mode of the transform.
  If this public field is \c true, then when a \ref Vec2 "2D vector"
  is \ref Vec2::transform2(const Trf2&) "transformed", is it scaled,
  rotated <b>and translated</b>.\n
  If this field is \c false, then when a 2D vector
  is transformed, is it scaled, rotated <b>but not translated</b>.
*/

// -------------------------------------------------------------------------
/** \fn Trf2::Trf2()
  Default constructor.

  Initializes this transform to a unity transform, with zero translation.\n
  Field \ref isDerivative is set to \c false.
*/

// -------------------------------------------------------------------------
// ------- Initialise transform to unit matrix -----------------------------
// -------------------------------------------------------------------------
/** Initializes this transform to a unity matrix with zero translation.

   Additionally field \ref isDerivative is set to \c false.
*/

void Trf2::init()
{
   m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0;
   m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0;

   isDerivative = false;
}

// -------------------------------------------------------------------------
// ------- All elements to zero --------------------------------------------
// -------------------------------------------------------------------------
/** Initializes this transform to a matrix with all zeroes

   Field \ref isDerivative is not modified.
*/

void Trf2::zero()
{
   m[0][0] = 0.0; m[0][1] = 0.0; m[0][2] = 0.0;
   m[1][0] = 0.0; m[1][1] = 0.0; m[1][2] = 0.0;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
/** Constructor, defines a transform by individual parameters.
  \param m00 The x-axis scale.
  \param m01 The xy-shear.
  \param m02 The translation in x direction.
  \param m10 The yx shear.
  \param m11 The y-axis scale.
  \param m12 The translation in y direction.

  Field \ref isDerivative is set to \c false.
*/

Trf2::Trf2(double m00, double m01, double m02,
           double m10, double m11, double m12)
{
   m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
   m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;

   isDerivative = false;
}

// -------------------------------------------------------------------------
// ---- Initialise by origin and dir of x-axis -----------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
/** Constructor, defines a transform by origin and x-axis direction.
  \param org The origin of the new transform.
  \param dir The direction of the x-axis of the new transform.

  The new transform transforms from the \e old to the \e new coordinate
  system.\n
  The length of \c dir determines the scale of the transformation:\n
  the length of a transformed vector is the length of \c dir
  times the length of the original vector.\n
  \n
  Field \ref isDerivative is set to \c false.

  \note If the length of parameter \c dir is zero or very small the
  result is unpredictable.
*/

Trf2::Trf2(const Vec2& org, const Vec2& dir)
{
  m[0][0] =  dir.x; m[0][1] = dir.y; m[0][2] = -dir.x*org.x - dir.y*org.y;
  m[1][0] = -dir.y; m[1][1] = dir.x; m[1][2] =  dir.y*org.x - dir.x*org.y;

  isDerivative = false;
}

// -------------------------------------------------------------------------
/** Calculates the determinant of this transform.
  \return The determinant of this transform.
*/

double Trf2::determinant() const
{
  return m[0][0]*m[1][1]-m[0][1]*m[1][0];
}

// -------------------------------------------------------------------------
/** Calculates the scale of this transform in the X-direction.
    The scale is calculated as:\n
    <tt>sqrt(sqr(m(0,0)) + sqr(m(0,1))</tt>
    \return The scaling in the X-direction.
*/

double Trf2::scaleX() const
{
  return sqrt(sqr(m[0][0]) + sqr(m[0][1]));
}

// -------------------------------------------------------------------------
/** Calculates the scale of this transform in the Y-direction.
    The scale is calculated as:\n
    <tt>sqrt(sqr(m(1,0)) + sqr(m(1,1))</tt>
    \return The scaling in the Y-direction.
*/

double Trf2::scaleY() const
{
  return sqrt(sqr(m[1][0]) + sqr(m[1][1]));
}

// -------------------------------------------------------------------------
/** Determines if this transform is (also) a mirroring operation.
   \return \c true if \ref determinant() < 0.0, \c false otherwise.
*/

bool Trf2::mirrors() const
{
  return m[0][0]*m[1][1]-m[0][1]*m[1][0] < 0.0;
}

// -------------------------------------------------------------------------
/** Constructs a mirroring transformation.
   \param org The base point of the mirroring line.
   \param mirrorAxis The direction of the mirroring line.

   A transformation is constructed that mirrors across a line through
   \c org and perpendicular to \c mirroraxis.

   \par Method
   \li a. transform to origin \c org and x-axis \c mirroraxis,
   \li b. mirror horizontally (negate all x-coordinates)
   \li c. transform back
*/

void Trf2::mirror(const Vec2& org, const Vec2& mirrorAxis)
{
  Vec2 dir = mirrorAxis; dir.unitLen2();

  Trf2 trf(org,dir);

  Trf2 mirr; mirr.m[0][0] = -1.0;

  trf.invertInto(*this);

  operator*=(mirr);
  operator*=(trf);

  isDerivative = false;
}

// -------------------------------------------------------------------------
// ------- Swap row 1 and 2 ------------------------------------------------
// -------------------------------------------------------------------------

static void swap_2(double m[2][3])
{
  for (int i=0; i<3; i++) {
    double h = m[0][i]; m[0][i] = m[1][i]; m[1][i] = h;
  }
}

// -------------------------------------------------------------------------
/** \fn bool Trf2::invert()
  Inverts this transformation.
  \return \c true if the operation succeeded, \c false if this transform
  has no inverse (i.e. if \ref determinant() is zero).
  \throws OperationNotSupportedException If field \ref isDerivative is
  \c true.
*/

// -------------------------------------------------------------------------
/** Calculates the inverse of this transformation.
  \param invMat The transform that receives the result.
  \return \c true if the operation succeeded, \c false if this transform
  has no inverse (i.e. if \ref determinant() is zero).
  \throws OperationNotSupportedException If field \ref isDerivative is
  \c true.
*/

bool Trf2::invertInto(Trf2& invMat) const
{
  if (isDerivative) throw OperationNotSupportedException("");

  int i,j;
  double lmt[2][3], rmt[2][3];

  for (i=0; i<2; i++) {
    for (j=0; j<3; j++) {
      lmt[i][j] = m[i][j];
      if (i==j) rmt[i][j] = 1.0;
      else      rmt[i][j] = 0.0;
    }
  }

  if (fabs(lmt[0][0]) < fabs(lmt[1][0])) {
    swap_2(lmt); swap_2(rmt);
  }

  if (fabs(lmt[0][0]) <= NumAccuracy*fabs(lmt[1][0])) return false;

  double pivot = lmt[1][0]/lmt[0][0];

  lmt[1][1] -= (lmt[0][1]*pivot); lmt[1][2] -= (lmt[0][2]*pivot);

  rmt[1][0] -= (rmt[0][0]*pivot);
  rmt[1][1] -= (rmt[0][1]*pivot); rmt[1][2] -= (rmt[0][2]*pivot);

  if (fabs(lmt[1][1]) <= NumAccuracy*fabs(lmt[0][1])) return false;

  pivot = lmt[0][1]/lmt[1][1];

  lmt[0][2] -= (lmt[1][2]*pivot);

  rmt[0][0] -= (rmt[1][0]*pivot);
  rmt[0][1] -= (rmt[1][1]*pivot); rmt[0][2] -= (rmt[1][2]*pivot);

  rmt[0][2] -= lmt[0][2]; rmt[1][2] -= lmt[1][2];

  rmt[0][0] /= lmt[0][0]; rmt[0][1] /= lmt[0][0]; rmt[0][2] /= lmt[0][0];
  rmt[1][0] /= lmt[1][1]; rmt[1][1] /= lmt[1][1]; rmt[1][2] /= lmt[1][1];

  for (i=0; i<2; i++) {
    for (j=0; j<3; j++) invMat.m[i][j] = rmt[i][j];
  }

  return true;
}

// -----------------------------------------------------------------------------
/** Calculates the rotation angle of this transformation.

    A counterclockwise rotation is positive.

    \return The rotation angle in radians <tt>-Pi < angle <= Pi</tt>.
*/

double Trf2::angle() const
{
  return atan2(m[1][0],m[0][0]);
}

// -------------------------------------------------------------------------
/** Assignment operator.
  \param mt The transform to assign from.
  \return A reference to this transform.
*/

Trf2& Trf2::operator = (const Trf2& mt)
{
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) m[i][j] = mt.m[i][j];
  }

  isDerivative = mt.isDerivative;

  return *this;
}

// -------------------------------------------------------------------------
/** Matrix \b post multiplication.
  \param mt The transform to multiply with.
  \return A reference to this transform.

  \note This transform is \b post-multiplied with transform \c mt.\n
  I.e. <tt>this = this * mt</tt>.
*/

Trf2& Trf2::operator *= (const Trf2& mt)
{
 int i,j,k;
 double cpm[2][3];

 // Copy this.m to cpm;

 for (i=0; i<2; i++) {
  for (j=0; j<3; j++) cpm[i][j] = m[i][j];
 }

 for (i=0; i<2; i++) {
   for (j=0; j<3; j++) {
     double sum = 0.0;
     for (k=0; k<2; k++) sum += cpm[i][k] * mt.m[k][j];

     m[i][j] = sum;
   }

   if (!mt.isDerivative) m[i][2] += cpm[i][2];
 }

 if (!isDerivative) isDerivative = mt.isDerivative;

 return *this;
}

// -------------------------------------------------------------------------
/** Matrix \b premultiplication.
  \param mt The transform to multiply with.
  \return A reference to this transform.

  \note This transform is \b pre-multiplied with transform \c mt.\n
  I.e. <tt>this = mt * this</tt>.
*/

Trf2& Trf2::preMultWith(const Trf2& mt)
{
  double cpm[2][3];

  // Copy this.m to cpm;

  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) cpm[i][j] = m[i][j];
  }

  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      double sum = 0.0;
      for (int k=0; k<2; k++) sum += mt.m[i][k] * cpm[k][j];

      m[i][j] = sum;
    }

    if (!isDerivative) m[i][2] += mt.m[i][2];
  }

  if (!isDerivative) isDerivative = mt.isDerivative;

  return *this;
}

// -------------------------------------------------------------------------
/** \ref Ino::Vec2 "2D Vector" transformation.
  \param p the vector to be transformed.
  \return The transformed vector.

  If \ref isDerivative is \c true or if field \ref Ino::Vec2::isDerivative
  "isDerivative" of vector \c p is \c true, then this transformation
  will scale and rotate, but \b not translate.\n
  \n
  If \ref isDerivative is \c true then field \ref Vec2::isDerivative
  "isDerivative" of the returned vector will be set to \c true.

  \note Returning an object is an inefficient operation.\n
  Use method \ref Ino::Vec2::transform2(const Trf2&) "transform2" of class
  \ref Ino::Vec2 Vec2 instead of this method whenever possible.
*/

Vec2 Trf2::operator* (const Vec2& p) const      // Transform vector
{
  Vec2 v;

  v.x = m[0][0] * p.x + m[0][1] * p.y;
  v.y = m[1][0] * p.x + m[1][1] * p.y;

  if (!p.isDerivative) {
    v.x += m[0][2];
    v.y += m[1][2];
  }

  v.isDerivative = isDerivative || p.isDerivative;

  return v;
}

// -------------------------------------------------------------------------
/** Matrix addition.
  \param mt The transform to add to this one.
  \return A reference to this transform.
*/

Trf2& Trf2::operator += (const Trf2& mt)
{
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) m[i][j] += mt.m[i][j];
  }

  return *this;
}

// -------------------------------------------------------------------------
/** Matrix subtraction.
  \param mt The transform to subtract from this one.
  \return A reference to this transform.
*/

Trf2& Trf2::operator -= (const Trf2& mt)
{
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) m[i][j] -= mt.m[i][j];
  }

  return *this;
}

// -------------------------------------------------------------------------
/** Scale transformation.
  \param fact The factor to multiply every element with.
  \return A reference to this transform.
*/

Trf2& Trf2::operator *=(double fact)
{
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) m[i][j] *= fact;
  }

  return *this;
}

// -------------------------------------------------------------------------
/** Matrix element extraction, returns an individual matrix element.
  \param ix The row index, either 0 or 1.
  \param iy The column index, either 0,1 or 2.
  \return The specified matrix element.

  \note There is no array bound protection, make sure \c ix and \c iy
  are within the specified bounds.
*/

double Trf2::operator()(int ix, int iy) const
{
  if (ix >= 0 && ix < 2 && iy >= 0 && iy < 3) return m[ix][iy];

  return 0;
}

// -------------------------------------------------------------------------
/** Matrix element access, returns a \b reference to an individual matrix
   element.
  \param ix The row index, either 0 or 1.
  \param iy The column index, either 0,1 or 2.
  \return A reference to the specified matrix element.

  \note There is no array bound protection, make sure \c ix and \c iy
  are within the specified bounds.
*/

double& Trf2::operator()(int ix, int iy)
{
  if (ix >= 0 && ix < 2 && iy >= 0 && iy < 3) return m[ix][iy];

  return *((double *)0);
}

// -------------------------------------------------------------------------
// ------- 3D Transformations ----------------------------------------------
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
/** \addtogroup geometry 2D/3D Geometry
  @{
*/

// -------------------------------------------------------------------------
/** \class Trf3
  Defines a 3D transformation matrix.

  The transformation is based on the use of <em>homogeneous
  coordinates</em>, so the transformation is 3x4 matrix.\n
  \n

  A transform can be in one of two modes:
  \li \ref isDerivative is \c false, the default.\n
  In this case the virtual matrix element [3,3] = 1.0.
  \li \ref isDerivative is \c true.\n
  In this case the virtual matrix element [3,3] = 0.0.

  \see \ref Vec3::transform3(const Trf3&) "Vec3::transform3".
  \author C. Wolters
  \date Aug 1996
*/

// -------------------------------------------------------------------------
/**
 @}
*/

// -------------------------------------------------------------------------
/** \var bool Trf3::isDerivative
  Defines the mode of the transform.
  If this public field is \c true, then when a \ref Vec3 "3D vector"
  is \ref Vec3::transform3(const Trf3&) "transformed", is it scaled,
  rotated <b>and translated</b>.\n
  If this field is \c false, then when a 3D vector
  is transformed, is it scaled, rotated <b>but not translated</b>.
*/

// -------------------------------------------------------------------------
/** Initializes this transform to a unity matrix with zero translation.

   Additionally field \ref isDerivative is set to \c false.
*/

void Trf3::init()
{
  m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
  m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
  m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;

  isDerivative = false;
}

// -------------------------------------------------------------------------
// ------- All Elements to zero --------------------------------------------
// -------------------------------------------------------------------------
/** Initializes this transform to a matrix with all zeroes

   Field \ref isDerivative is not modified.
*/

void Trf3::zero()
{
  m[0][0] = 0.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
  m[1][0] = 0.0; m[1][1] = 0.0; m[1][2] = 0.0; m[1][3] = 0.0;
  m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 0.0; m[2][3] = 0.0;
}

// -------------------------------------------------------------------------
/** \fn Trf3::Trf3()
  Default constructor.

  Initializes this transform to a unity transform, with zero translation.\n
  Field \ref isDerivative is set to \c false.
*/

// -------------------------------------------------------------------------
/** Constructor, defines a transform by individual parameters.
  \param m00 The x-axis scale.
  \param m01 The xy-shear.
  \param m02 The xz-shear.
  \param m03 The translation in x direction.
  \param m10 The yx shear.
  \param m11 The y-axis scale.
  \param m12 The yz shear.
  \param m13 The translation in y direction.
  \param m20 The zx shear.
  \param m21 The zy shear.
  \param m22 The z-axis scale.
  \param m23 The translation in z direction.

  Field \ref isDerivative is set to \c false.
*/

Trf3::Trf3(double m00, double m01, double m02, double m03,
           double m10, double m11, double m12, double m13,
           double m20, double m21, double m22, double m23)
{
  m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
  m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
  m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;

  isDerivative = false;
}

// -------------------------------------------------------------------------
// ---- Initialise by origin and dir of z- and x-axis ----------------------
// -------------------------------------------------------------------------
/** Constructor, defines a transform by origin and axis directions.
  \param org The origin of the new transform.
  \param zDir The direction of the z-axis of the new transform.
  \param xDir The direction of the x-axis of the new transform.

  The new transform transforms from the \e old to the \e new coordinate
  system.\n
  The scale of the transform is set to one (1).
  \n
  The y-axis is calculated from the outer product of parameters
  \c zDir and \c xDir.\n
  Then the x-axis is calculated as the outer
  product of \c zDir and the y-axis.\n
  Field \ref isDerivative is set to \c false.
  
  \note If the length of parameter \c zDir or \c xDir is zero or very
  small the result is unpredictable.\n
  Also if \c zDir and \c xDir have (nearly) the same direction the
  result is undefined.
*/
Trf3::Trf3(const Vec3& org, const Vec3& zDir, const Vec3& xDir)
{
//  double len = xDir.len3();

  Vec3 lz(zDir);
  lz.unitLen3();

  Vec3 ly(lz.outer(xDir));
  ly.unitLen3();
  
  Vec3 lx(ly.outer(lz));

  m[0][0] = lx.x; m[0][1] = lx.y; m[0][2] = lx.z;
  m[1][0] = ly.x; m[1][1] = ly.y; m[1][2] = ly.z;
  m[2][0] = lz.x; m[2][1] = lz.y; m[2][2] = lz.z;

  m[0][3] = -lx.x*org.x - lx.y*org.y - lx.z * org.z;
  m[1][3] = -ly.x*org.x - ly.y*org.y - ly.z * org.z;
  m[2][3] = -lz.x*org.x - lz.y*org.y - lz.z * org.z;

  isDerivative = false;
}

// -------------------------------------------------------------------------

static void swap_3(double mt[3][4], int i1, int i2)
{
  if (i1 == i2) return;

  for (int j=0; j<4; j++) {
    double h = mt[i1][j]; mt[i1][j] = mt[i2][j]; mt[i2][j] = h;
  }
}

// -------------------------------------------------------------------------
/** Calculates the determinant of this transform.
  \return The determinant of this transform.
*/

double Trf3::determinant() const
{
  return m[0][0] * (m[1][1]*m[2][2] - m[1][2]*m[2][1]) -
         m[0][1] * (m[1][0]*m[2][2] - m[1][2]*m[2][0]) +
         m[0][2] * (m[1][0]*m[2][1] - m[1][1]*m[2][0]);
}

// -------------------------------------------------------------------------
/** Calculates the scale of this transform in the X-direction.
    The scale is calculated as:\n
    <tt>sqrt(sqr(m(0,0)) + sqr(m(0,1) + sqr(m(0,2))</tt>
    \return The scaling in the X-direction.
*/

double Trf3::scaleX() const
{
  return sqrt(sqr(m[0][0]) + sqr(m[0][1]) + sqr(m[0][2]));
}

// -------------------------------------------------------------------------
/** Calculates the scale of this transform in the Y-direction.
    The scale is calculated as:\n
    <tt>sqrt(sqr(m(1,0)) + sqr(m(1,1) + sqr(m(1,2))</tt>
    \return The scaling in the Y-direction.
*/

double Trf3::scaleY() const
{
  return sqrt(sqr(m[1][0]) + sqr(m[1][1]) + sqr(m[1][2]));
}

// -------------------------------------------------------------------------
/** Calculates the scale of this transform in the Z-direction.
    The scale is calculated as:\n
    <tt>sqrt(sqr(m(2,0)) + sqr(m(2,1) + sqr(m(2,2))</tt>
    \return The scaling in the Z-direction.
*/

double Trf3::scaleZ() const
{
  return sqrt(sqr(m[2][0]) + sqr(m[2][1]) + sqr(m[2][2]));
}

// -------------------------------------------------------------------------
/** \fn Trf3::mirrors() const
   Determines if this transform is (also) a mirroring operation.
   \return \c true if \ref determinant() < 0.0, \c false otherwise.
*/

// -------------------------------------------------------------------------
/** Constructs a mirroring transformation.
  \param org The base point of the mirroring line.
  \param mirrorAxis The direction of the mirroring line.

  A transformation is constructed that mirrors across a plane through
  \c org and perpendicular to \c mirroraxis.
*/

void Trf3::mirror(const Vec3& org, const Vec3& mirrorAxis)
{
  Vec3 xDir(1,0,0), yDir(0,1,0);

  if (fabs(mirrorAxis * yDir) < fabs(mirrorAxis * xDir)) xDir = yDir;

  Trf3 trf(org,mirrorAxis,xDir);

  Trf3 mirr; mirr.m[2][2] = -1.0;

  trf.invertInto(*this);

  operator*=(mirr);
  operator*=(trf);

  isDerivative = false;
}

// -------------------------------------------------------------------------
/** \fn bool Trf3::invert()
  Inverts this transformation.
  \return \c true if the operation succeeded, \c false if this transform
  has no inverse (i.e. if \ref determinant() is zero).
  \throws OperationNotSupportedException If field \ref isDerivative is
  \c true.
*/

// -------------------------------------------------------------------------
/** Calculates the inverse of this transformation.
  \param invMat The transform that receives the result.
  \return \c true if the operation succeeded, \c false if this transform
  has no inverse (i.e. if \ref determinant() is zero).
  \throws OperationNotSupportedException If field \ref isDerivative is
  \c true.
*/

bool Trf3::invertInto(Trf3& invMat) const
{
  if (isDerivative) throw OperationNotSupportedException("");

  int ir,i,j;
  double lmt[3][4], rmt[3][4];

  for (i=0; i<3; i++) {
    for (j=0; j<4; j++) {
      lmt[i][j] = m[i][j];
      if (i==j) rmt[i][j] = 1.0;
      else      rmt[i][j] = 0.0;
    }
  }

  for (ir=0; ir<3; ir++) {
    // Find max value
    int maxi = ir; double maxval = fabs(lmt[ir][ir]);
    for (i=ir+1; i<3; i++) {
      if (fabs(lmt[i][ir]) > maxval) {
        maxi = i; maxval = fabs(lmt[i][ir]);
      }
    }

    // Swap rows
    swap_3(lmt,ir,maxi); swap_3(rmt,ir,maxi);

    // Clean column ir
    for (i=0; i<3; i++) {
      if (i==ir) continue;
      if (fabs(lmt[ir][ir]) <= NumAccuracy*fabs(lmt[i][ir])) return false;

      double pivot = lmt[i][ir]/lmt[ir][ir];

      for (j=ir+1; j<4; j++) lmt[i][j] -= (lmt[ir][j]*pivot);
      for (j=0;    j<4; j++) rmt[i][j] -= (rmt[ir][j]*pivot);
    }
  }

  // Clean last column, multiply and copy to result.
  for (i=0; i<3; i++) {
    rmt[i][3] -= lmt[i][3];
    for (j=0; j<4; j++) {
      invMat.m[i][j] = rmt[i][j]/lmt[i][i];
    }
  }

  return true;
}

// -------------------------------------------------------------------------
/** Assignment operator.
*/

Trf3& Trf3::operator = (const Trf3& mt)
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) m[i][j] = mt.m[i][j];
  }

  isDerivative = mt.isDerivative;

  return *this;
}

// -------------------------------------------------------------------------
/** Matrix \b post multiplication.
  \param mt The transform to multiply with.
  \return A reference to this transform.

  \note This transform is \b post-multiplied with transform \c mt.\n
  I.e. <tt>this = this * mt</tt>.
*/

Trf3& Trf3::operator *= (const Trf3& mt)
{
  int i,j,k;
  double cpm[3][4];

  // Copy this.m to cpm;

  for (i=0; i<3; i++) {
    for (j=0; j<4; j++) cpm[i][j] = m[i][j];
  }

  for (i=0; i<3; i++) {
    for (j=0; j<4; j++) {
      double sum = 0.0;
      for (k=0; k<3; k++) sum += cpm[i][k] * mt.m[k][j];

      m[i][j] = sum;
    }

    if (!mt.isDerivative) m[i][3] += cpm[i][3];
  }

  if (!isDerivative) isDerivative = mt.isDerivative;

  return *this;
}

// -------------------------------------------------------------------------
/** Matrix \b pre multiplication.
  \param mt The transform to multiply with.
  \return A reference to this transform.

  \note This transform is \b pre-multiplied with transform \c mt.\n
  I.e. <tt>this = mt * this</tt>.
*/

Trf3& Trf3::preMultWith(const Trf3& mt)
{
  double cpm[3][4];

  // Copy this.m to cpm;

  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) cpm[i][j] = m[i][j];
  }

  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) {
      double sum = 0.0;
      for (int k=0; k<3; k++) sum += mt.m[i][k] * cpm[k][j];

      m[i][j] = sum;
    }

    if (!isDerivative) m[i][3] += mt.m[i][3];
  }

  if (!isDerivative) isDerivative = mt.isDerivative;

  return *this;
}

// -------------------------------------------------------------------------
/** \ref Ino::Vec3 "3D Vector" transformation.
  \param p the vector to be transformed.
  \return The transformed vector.

  If \ref isDerivative is \c true or if field \ref Ino::Vec3::isDerivative
  "isDerivative" of vector \c p is \c true, then this transformation
  will scale and rotate, but \b not translate.\n
  \n
  If \ref isDerivative is \c true then field \ref Ino::Vec3::isDerivative
  "isDerivative" of the returned vector will be set to \c true.

  \note Returning an object is an inefficient operation.\n
  Use method \ref Ino::Vec3::transform3(const Trf3&) "transform3" of class
  \ref Ino::Vec3 Vec3 instead of this method whenever possible.
*/

Vec3 Trf3::operator* (const Vec3& p) const      // Transform vector
{
  Vec3 v;

  v.x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z;
  v.y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z;
  v.z = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z;

  if (!p.isDerivative) {
    v.x += m[0][3];
    v.y += m[1][3];
    v.z += m[2][3];
  }

  v.isDerivative = isDerivative || p.isDerivative;

  return v;
}

// -------------------------------------------------------------------------
/** Matrix addition.
  \param mt The transform to add to this one.
  \return A reference to this transform.
*/

Trf3& Trf3::operator += (const Trf3& mt)
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) m[i][j] += mt.m[i][j];
  }

  return *this;
}

// -------------------------------------------------------------------------
/** Matrix subtraction.
  \param mt The transform to subtract from this one.
  \return A reference to this transform.
*/

Trf3& Trf3::operator -= (const Trf3& mt)
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) m[i][j] -= mt.m[i][j];
  }

  return *this;
}

// -------------------------------------------------------------------------
/** Scale transformation.
  \param fact The factor to multiply every element with.
  \return A reference to this transform.
*/

Trf3& Trf3::operator *=(double fact)
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) m[i][j] *= fact;
  }

  return *this;
}

// -------------------------------------------------------------------------
/** Matrix element extraction, returns an individual matrix element.
  \param ix The row index, either 0,1 or 2.
  \param iy The column index, either 0,1,2 or 3.
  \return The specified matrix element.

  \note There is no array bound protection, make sure \c ix and \c iy
  are within the specified bounds.
*/

double Trf3::operator()(int ix, int iy) const
{
  if (ix >= 0 && ix < 3 && iy >= 0 && iy < 4) return m[ix][iy];

  return 0;
}

// -------------------------------------------------------------------------
/** Matrix element access, returns a \b reference to an individual matrix
   element.
  \param ix The row index, either 0,1 or 2.
  \param iy The column index, either 0,1,2 or 3.
  \return A reference to the specified matrix element.

  \note There is no array bound protection, make sure \c ix and \c iy
  are within the specified bounds.
*/

double& Trf3::operator()(int ix, int iy)
{
  if (ix >= 0 && ix < 3 && iy >= 0 && iy < 4) return m[ix][iy];

  return *((double *)0);
}

} // namespace Ino

//---------------------------------------------------------------------------
// Interface Section


void* Trf3New()
{
  return new Ino::Trf3();
}

void* Trf3Copy(void* cppTrf)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  return new Ino::Trf3(*trf);
}

double GetElementTrf3(void* cppTrf, int row, int col)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  return (*trf)(row, col);
}

void SetElementTrf3(void* cppTrf, int row, int col, double value)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  (*trf)(row,col) = value;
}

bool GetDerivativeTrf3(void* cppTrf)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  return trf->isDerivative;
}

void SetDerivativeTrf3(void* cppTrf, bool isDer)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  trf->isDerivative = isDer;
}

void InitTrf3(void* cppTrf)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  trf->init();
}

void ZeroTrf3(void* cppTrf)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  trf->zero();
}

double DeterminantTrf3(void* cppTrf)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  return trf->determinant();
}

void MirrorTrf3(void* cppTrf, const Ino::Vec3& org, const Ino::Vec3& mirrorAxis)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  trf->mirror(org, mirrorAxis);
}

bool Invert(void* cppTrf)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;

  return trf->invert();
}

bool InvertInto(void* cppTrf, void* cppInvTrf)
{
  Ino::Trf3* trf = (Ino::Trf3*)cppTrf;
  Ino::Trf3* invTrf = (Ino::Trf3*)cppInvTrf;

  return trf->invertInto(*invTrf);
}

// -------------------------------------------------------------------------
// ------------------- End of module ---------------------------------------
// -------------------------------------------------------------------------
