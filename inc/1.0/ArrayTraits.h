//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Inofor Array Template Type Traits ---------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Copyright Inofor Hoek Aut BV Mar 2010, June 2024 ------------------
//---------------------------------------------------------------------------
//------- C. Wolters --------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef INOARRAY_INC
#error Do not include directly, include Array.h instead
#endif

namespace Ino
{

namespace ArrayTraits
{

//---------------------------------------------------------------------------

template <class T> struct Deref { typedef T Type; };
template <class T> struct Deref<T*> { typedef T Type; };
template <class T> struct Deref<const T*> { typedef T Type; };

template <class T> struct StringT { static const bool value = false; };
template <> struct StringT<wchar_t> { static const bool value = true; };

//---------------------------------------------------------------------------

template <class T> struct BaseType
{
  struct Yes { char c; };
  struct No  { char c[8]; };

  static No  m(...);
  static Yes m(const ArrayElem *);

  static typename Deref<T>::Type *t;
  static const bool IsArrayElemRef = __is_class(T) &&
                                         sizeof(m(t)) == sizeof(Yes);
  static const bool IsArrayElemPtr = !IsArrayElemRef &&
                                   (__is_class(typename Deref<T>::Type) &&
                                                sizeof(m(t)) == sizeof(Yes));
  static const bool IsArrayElem = IsArrayElemRef || IsArrayElemPtr;

  static const bool IsString = StringT<typename Deref<T>::Type>::value;

  static const int TypeVal =
                     IsArrayElemPtr ? 1 : IsArrayElemRef ? 2 : IsString ? 3 : 0;
};

//---------------------------------------------------------------------------
// Basic type

template <class T, int typeVal> struct Type {
  typedef T ArgType;
  typedef T& ReturnType;
  typedef T ConstReturnType;
  typedef T ElemType;
};

//---------------------------------------------------------------------------
// Pointer to class derived from ArrayElem

template <class T> struct Type<T,1> {
  typedef T ArgType;
  typedef T& ReturnType;
  typedef T ConstReturnType;
  typedef T ElemType;
};

//---------------------------------------------------------------------------
// Class derived from ArrayElem

template <class T> struct Type<T,2>
{
  typedef const T& ArgType;
  typedef T& ReturnType;
  typedef const T& ConstReturnType;
  typedef T *ElemType;
};

//---------------------------------------------------------------------------
// Type wchar_t*

template <class T> struct Type<T,3>
{
  typedef const wchar_t *ArgType;
  typedef const wchar_t *ReturnType;
  typedef const wchar_t *ConstReturnType;
  typedef wchar_t *ElemType;
};

} // namespace ArrayTraits
} // namespace Ino

//---------------------------------------------------------------------------
