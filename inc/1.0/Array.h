//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Inofor Array Template ---------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Copyright Inofor Hoek Aut BV Mar 2010, June 2024 ------------------
//---------------------------------------------------------------------------
//------- C. Wolters --------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//Note:
//  Type T MUST be one of: bool, short, int, long,
//                         float, double, wchar_t*, const wchar_t*
//  (Unsigned basic types are not acceptable)
//
//  Or else T or *T must be derived from ArrayElem
//  An exception will be thrown if T is of some other type!

// Type Overview
//   (AppClass is some class derived from ArrayElem

// Type T           ArgType          ReturnType       ConstReturnType
// --------------------------------------------------------------------------
// bool             bool             bool&            bool
//  ..              ..           ..                   ..(idem short etc)
//
// wchar_t*         const wchar_t*   const wchar_t*   const wchar_t*
// const wchar_t*   const wchar_t*   const wchar_t*   const wchar_t*
//
// AppClass         const AppClass&  AppClass&        const AppClass&
// const AppClass   const AppClass&  const AppClass&  const AppClass&
//
// AppClass*        AppClass*        AppClass*&       AppClass*
// const AppClass*  const AppClass*  const AppClass*  const AppClass*

// Note 1
//  If T is wchar_t* or const wchar_t*, methods add/insert will
//  insert a COPY of the string into the array and that copy is then
//  owned by the array.

// Note 2
// If T is AppClass or const AppClass, methods add/insert will
//  insert a COPY of the class into the array and that copy is then
//  owned by the array. This means an implicit or explicit copy constructor
//  and a destructor must exist.
//  The class does not need to have an assignment operator=();

// Note 3
//   If T is AppClass* or const AppClass* the type is treated just as a
//   bool, int etc. No copy constructor or destructor will ever be called.

//---------------------------------------------------------------------------

#ifndef INOARRAY_INC
#define INOARRAY_INC

#include "Exceptions.h"

namespace Ino
{
  class ArrayElem
  {
  public:
    ArrayElem() {}
    ArrayElem(const ArrayElem& cp) {}
    virtual ~ArrayElem() {}
  };
}
#include "ArrayTraits.h"

namespace Ino
{

//---------------------------------------------------------------------------

template <class T> class Array : public ArrayElem
{
  static const int TypeVal = ArrayTraits::BaseType<T>::TypeVal;
  typedef typename ArrayTraits::Type<T,TypeVal>::ElemType ElemType;

  bool objOwner;
  int capIncPercent;

  ElemType *lst;
  int lstSz, lstCap;

  void incCapacity();

public:
  typedef typename ArrayTraits::Type<T,TypeVal>::ArgType ArgType;
  typedef typename ArrayTraits::Type<T,TypeVal>::ReturnType ReturnType;
  typedef typename ArrayTraits::Type<T,TypeVal>::ConstReturnType ConstReturnType;

  Array(int initCap = 8, int capIncrPercent = 50);
  Array(bool owner, int initCap = 8, int capIncrPercent = 50);
  Array(const Array& cp);
  virtual ~Array();

  Array& operator=(const Array& src);

  bool isObjectOwner() const;
  void setObjectOwner(bool owner);

  int size() const { return lstSz; }
  int cap() const { return lstCap; }

  void ensureCapacity(int minCap);
  void shrinkCapacity(int reserveCap = 0);

  virtual void clear();

  virtual int add(ArgType item);
  virtual void set(int idx, ArgType item);
  virtual void insert(int idx, ArgType item);
  virtual void remove(int idx);

  virtual void swap(int idx1, int idx2);

  virtual ConstReturnType get(int idx) const;
  virtual ReturnType get(int idx);

  virtual ConstReturnType operator[](int idx) const;
  virtual ReturnType operator[](int idx);

#ifdef _WIN32
  template <class LessThan> void sort(LessThan& lt);
  template <class LessThan> void stableSort(LessThan *lt);
#endif

};

} // namespace Ino
#include "ArrayImp.h"

//---------------------------------------------------------------------------
#endif
