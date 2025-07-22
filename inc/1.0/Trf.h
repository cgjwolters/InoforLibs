// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// ------------ Vector and matrix manipulations 2D/3D ----------------------
// -------------------------------------------------------------------------
// ---------------- Inofor Hoek Automatisering BV---------------------------
// -------------------------------------------------------------------------
// ------------------- Copyright C. Wolters aug 1996 -----------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifndef INOTRF_INC
#define INOTRF_INC

#include "Basics.h"

#include "Vec.h"

namespace Ino
{

// -------------------------------------------------------------------------
// ------- 2D Transformation -----------------------------------------------
// -------------------------------------------------------------------------

class Trf2
{
    double m[2][3];

  public:
    bool isDerivative;

    void init();                            // Initialize to unit mat
    void zero();                            // All elements to zero

    Trf2() { init(); }                      // Initialise to unitmat

    Trf2(double m00, double m01, double m02,
         double m10, double m11, double m12);

    Trf2(const Vec2& org, const Vec2& dir); // Initialise to org + dir

    double determinant() const;

    bool mirrors() const;                   // Does it mirror?

    void mirror(const Vec2& org, const Vec2& mirrorAxis); // Mirror along
                                                          // axis
    bool invert() { return invertInto(*this); }
    bool invertInto(Trf2& invMat) const;   // Invert transform

    double angle() const;

    double scaleX() const;
    double scaleY() const;

    Trf2& operator = (const Trf2& mt);       // Assignment

    Trf2& operator += (const Trf2& mt);      // Add mt
    Trf2& operator -= (const Trf2& mt);      // Subtract mt
    Trf2& operator *= (double fact);         // Scaling

    Trf2& operator *= (const Trf2& mt);      // Post multiply with mt
    Trf2& preMultWith(const Trf2& mt);       // Pre multiply with mt

    Vec2 operator* (const Vec2& p) const;    // Transform Vec2

    double operator()(int ix, int iy) const; // Return matrix element
    double& operator()(int ix, int iy);      // Return matrix el. ref.

    friend class Vec2;
};

// -------------------------------------------------------------------------
// ------- 3D Transformation -----------------------------------------------
// -------------------------------------------------------------------------

class Trf3
{
    double m[3][4];

  public:
    bool isDerivative;

    void init();
    void zero();                            // All elements to zero

    Trf3() { init(); }                      // Initialize to unitmat

    Trf3(double m00, double m01, double m02, double m03,
         double m10, double m11, double m12, double m13,
         double m20, double m21, double m22, double m23);

    Trf3(const Vec3& org, const Vec3& zDir, const Vec3&xDir);

    double determinant() const;

    double scaleX() const;
    double scaleY() const;
    double scaleZ() const;

    bool mirrors() const { return determinant() < 0.0; } // Does it mirror?

    void mirror(const Vec3& org, const Vec3& mirrorAxis); // Mirror along
                                                          // axis

    bool invert() { return invertInto(*this); } // Invert transform
    bool invertInto(Trf3& invMat) const;        // Invert transform

    Trf3& operator = (const Trf3& mt);          // Assignment

    Trf3& operator += (const Trf3& mt);         // Add mt
    Trf3& operator -= (const Trf3& mt);         // Subtract mt
    Trf3& operator *= (double fact);            // Scaling

    Trf3& operator *= (const Trf3& mt);         // Post multiply with mt
    Trf3& preMultWith(const Trf3& mt);          // Pre multiply with mt

    Vec3 operator* (const Vec3& p) const;       // Transform Vec3

    double  operator()(int ix, int iy) const;   // Return matrix element
    double& operator()(int ix, int iy);         // Return matrix el. ref.

    friend class Vec3;
};
} // namespace Ino

// Interface Section

extern "C" __declspec(dllexport) void* Trf3New();
extern "C" __declspec(dllexport) void* Trf3Copy(void *cppTrf);

extern "C" __declspec(dllexport) double GetElementTrf3(void *cppTrf, int row, int col);
extern "C" __declspec(dllexport) void SetElementTrf3(void *cppTrf, int row, int col, double value);

extern "C" __declspec(dllexport) bool GetDerivativeTrf3(void* cppTrf);
extern "C" __declspec(dllexport) void SetDerivativeTrf3(void* cppTrf, bool isDer);

extern "C" __declspec(dllexport) void InitTrf3(void* cppTrf);
extern "C" __declspec(dllexport) void ZeroTrf3(void* cppTrf); // All elements to zero

extern "C" __declspec(dllexport) double DeterminantTf3(void *cppTrf);

extern "C" __declspec(dllexport) void MirrorTrf3(void *cppTrf, const Ino::Vec3& org,
                                                 const Ino::Vec3& mirrorAxis); // Mirror along

extern "C" __declspec(dllexport) bool Invert(void* cppTrf);
extern "C" __declspec(dllexport) bool InvertInto(void* cppTrf, void* cppInvTrf);

// -------------------------------------------------------------------------

#endif
