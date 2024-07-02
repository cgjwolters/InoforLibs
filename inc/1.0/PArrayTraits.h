//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Inofor Persistable Array Template Type Traits ---------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Copyright Inofor Hoek Aut BV Mar 2010 -----------------------------
//---------------------------------------------------------------------------
//------- C. Wolters --------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef INOPARRAY_INC
#error Do not include directly, include PArray.h instead
#endif

namespace Ino
{

  class Persistable;

namespace PArrayTraits
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
  static Yes m(const Persistable *p);

  static typename Deref<T>::Type *t;
  static const bool IsPersistRef = __is_class(T) &&
                                         sizeof(m(t)) == sizeof(Yes);
  static const bool IsPersistPtr = !IsPersistRef &&
                                   (__is_class(typename Deref<T>::Type) &&
                                                sizeof(m(t)) == sizeof(Yes));
  static const bool IsPersist = IsPersistRef || IsPersistPtr;

  static const bool IsString = StringT<typename Deref<T>::Type>::value;

  static const int TypeVal =
                     IsPersistPtr ? 1 : IsPersistRef ? 2 : IsString ? 3 : 0;
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
// Pointer to class derived from Persistable

template <class T> struct Type<T,1> {
  typedef T ArgType;
  typedef T& ReturnType;
  typedef T ConstReturnType;
  typedef T ElemType;
};

//---------------------------------------------------------------------------
// Class derived from Persistable

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

} // namespace PArrayTraits
} // namespace Ino

//---------------------------------------------------------------------------
