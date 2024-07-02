//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Inofor Persistable Array Template Implementation ------------------
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

#include "Basics.h"

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <algorithm>
#endif

namespace Ino
{

/** \addtogroup templates Templates
 @{
*/

//---------------------------------------------------------------------------
/** \file PArray.h
  This file contains the definition of the \ref Ino::PArray template.

  A \ref Ino::PArray is a generalized Persistable array,
  but suitable only for a limited set of types:

  \li <b>Signed basic types.</b>\n
  The array element can be of type:
  <tt>bool, short, int, long, wchar_t, float</tt> or \c double.\n\n
  \c Unsigned types are \em not supported due to the restrictions
  imposed by the Persistable base class.

  \li <b>Unicode string (i.e. type <tt>wchar_t *)</tt>.</b>\n
  In this case the array will manage and \b own a \b copy
  of any string added.

  \li <b>A class derived from Persistable.</b>\n
  The array will manage and \b own a \b copy of any object added.\n\n
  For this to work the class must have an (implicit or explicit)
  copy constructor or,\n
  in the case of an abstract class, it must have a <tt>clone()</tt> method.\n\n
  The assigment operator of the class will never be called.\n
  \n
  Note that is is \b not possible (yet) to create an array that contains\n
  elements of different types all derived from a base element type \c T,\n
  unless type \c T \b itself is \b abstract.\n
  \n
  In that case consider a \c PArray of \b pointer to your element type.\n
  You may then set the array to be
  \link Ino::PArray::setObjectOwner(bool) owner\endlink of the objects pointed to.\n
  Thus you \b can have a polymorphic array.
  
  \li <b>A pointer to a class derived from Persistable.</b>\n
  Such a pointer is handled the same as a basic type, i.e only\n
  the pointer value itself is handled by the array, unless this array
  \b owns the object: see below.\n\n
  The copy constructor or assigment operator will never be called.\n
  \n
  The PArray can optionally be made to \b own the objects pointed to,
  see this \link Ino::PArray::PArray(bool,int,int) constructor\endlink and
  method \link Ino::PArray::setObjectOwner(bool) setObjectOwner()\endlink.\n
  In that case operator delete \b will be used to the destroy the objects
  pointed to.

  For any other type the
  \link Ino::PArray::PArray(int, int) PArray<T> constructor\endlink
  will throw an Ino::IllegalArgumentException.

  \par <b>Warning: Persistence Restore</b>

  If a \ref Ino::PArray "PArray" is restored from a stream
  (using \ref Ino::PersistentReader "PersistentReader") and
  the following applies:\n\n
  \li The element type is a class derived from
  \ref Ino::Persistable "Persistable"
  \li The stream contains one or more elements of a type
  (derived from \ref Ino::Persistable "Persistable") not
  (yet) known\n
  to the \ref Ino::PersistentReader "PersistentReader"
  (see \ref Ino::PersistentTypeDef "PersistentTypeDef")\n\n

  Then such elements are silently \b removed from the array and the
  \ref Ino::PArray::size() "size()" of the array\n
  may therefore be less than would otherwise have been expected.

  \author C. Wolters
  \date Mar 2010
*/

namespace PArrayFuncs
{

//---------------------------------------------------------------------------

template <class T, bool typeVal> struct New
{
  static inline T *newItem(const T& it) { return new T(it); }
};

//---------------------------------------------------------------------------

template <class T> struct New<T,true>
{
  static inline T *newItem(const T& it) { return it.clone(); }
};

//---------------------------------------------------------------------------
// Basic type

template <class T, int typeVal> struct Func
{
  static T *dupLst(const T *lst, int sz) {
    T *newLst = (T *)malloc(sz * sizeof(T));

    memcpy(newLst,lst,sz*sizeof(T));

    return newLst;
  }

  static inline void clearItem(T,bool) {}
  static inline void clearAll(T *, int, bool) {}
  static inline T newItem(T it) { return it; }
  static inline T& getItem(T& it) { return it; }
};

//---------------------------------------------------------------------------
// T is pointer to Persistable

template <class T> struct Func<T,1>
{

  static T *dupLst(const T *lst, size_t sz) {
    size_t byteSz = sz * sizeof(T);

    T *newLst = (T *)malloc(byteSz);

#pragma warning(push)
#pragma warning(disable : 6387)
    memcpy_s(newLst,byteSz,lst,byteSz);

    return newLst;
#pragma warning(pop)
  }

  static inline void clearItem(T it, bool owner) {
    if (owner) delete it;
  }

  static inline void clearAll(T *lst, int lstSz, bool owner) {
    if (!owner) return;

    for (int i=0; i<lstSz; ++i) delete lst[i];
  }

  static inline T newItem(T it) { return it; }
  static inline T& getItem(T& it) { return it; }
};

//---------------------------------------------------------------------------
// T is type derived from Persistable

template <class T> struct Func<T,2>
{
  static T **dupLst(T **const lst, int sz) {
    T **newLst = (T **)malloc(sz * sizeof(T*));

    for (int i=0; i<sz; ++i) newLst[i] = newItem(*lst[i]);

    return newLst;
  }

  static inline void clearItem(T *it,bool) { delete it; }

  static void clearAll(T **lst, int sz, bool) {
    for (int i=0; i<sz; ++i) delete lst[i];
  }

  static inline T *newItem(const T& it)
  {
    return New<T,__is_abstract(T)>::newItem(it);
  }

  static inline T& getItem(T *it) { return *it; }
};

//---------------------------------------------------------------------------
// T is wchar_t* or const wchar_t*

template <class T> struct Func<T,3>
{
  static wchar_t **dupLst(wchar_t *const *const lst, int sz) {
    wchar_t **newLst = (wchar_t **)malloc(sz * sizeof(wchar_t *));

    for (int i=0; i<sz; ++i) newLst[i] = dupStr(lst[i]);

    return newLst;
  }

  static inline void clearItem(wchar_t *it,bool) { delete[] it; }

  static void clearAll(wchar_t **lst, int sz, bool) {
    for (int i=0; i<sz; ++i) delete[] lst[i];
  }

  static inline wchar_t *newItem(const wchar_t *it) { return dupStr(it); }
  static inline wchar_t *getItem(wchar_t *it) { return it; }
};

} // namespace PArrayFuncs

//---------------------------------------------------------------------------

template <class T> void PArray<T>::incCapacity()
{
  int newCap = lstCap + (int)(((long long)lstCap) * capIncPercent / 100);

  if (newCap < 8) newCap = 8;

  lstCap = newCap;

  ElemType *mem = (ElemType *)realloc(lst,lstCap*sizeof(ElemType));
  if (mem) lst = mem;
}

//---------------------------------------------------------------------------
/** \typedef typename PArrayTraits::Type<T,TypeVal>::ArgType PArray::ArgType
  Defines the type of argument \c item for methods add(), set() and insert().

  See \ref TypeResolution for details.
*/

//---------------------------------------------------------------------------
/** \typedef PArrayTraits::Type<T,TypeVal>::ReturnType PArray::ReturnType
  Defines the return type for methods get(int) and operator[](int).

  See \ref TypeResolution for details.
*/

//---------------------------------------------------------------------------
/** \typedef PArrayTraits::Type<T,TypeVal>::ConstReturnType PArray::ConstReturnType
  Defines the return type for methods get(int) const and operator[](int) const

  See \ref TypeResolution for details.
*/

//---------------------------------------------------------------------------
/** \fn int PArray::size() const
    Returns the size of this array.

    \return The number of elements in this array.
*/

//---------------------------------------------------------------------------
/** \fn int PArray::cap() const
    Returns the capacity of this array.

    \return The capacity, that is the allocated size of this array.\n
    The value is in number of elements (\b not bytes).
*/

//---------------------------------------------------------------------------
/** Returns whether this array owns the <tt>Persistable *</tt> 
    objects in this list.

  This method must only be called if type \c T is
  <tt>Persistable *</tt>&nbsp;!\n
  For other types an OperationNotSupportedException will thrown.

  \return if \c true, then the objects in this array will be deleted when
  required.\n
  This happens when a non NULL element is \link remove() removed\endlink or
  \link set() replaced\endlink,\n
  or else when method clear() or the
  \link ~PArray() destructor\endlink is called.

  \throw OperationNotSupportedException if the element type \c T of
  this array is not a <tt>Persistable *</tt>.
*/

template <class T> bool PArray<T>::isObjectOwner() const
{
  if (!PArrayTraits::BaseType<T>::IsPersistPtr)
    throw OperationNotSupportedException("PArray<T>::isObjectOwner()\n"
                                   "Can only be owner of Persistable *");

  return objOwner;
}

//---------------------------------------------------------------------------
/** Set whether this array owns the <tt>Persistable *</tt> 
    objects in this list.

  This method must only be called if type \c T
  is <tt>Persistable *</tt>&nbsp;!\n
  For other types an OperationNotSupportedException will be thrown.

  \param owner if \c true then the objects in this array will be deleted when
  required.\n
  This happens when a non NULL element is \link remove() removed\endlink or
  \link set() replaced\endlink,\n
  or else when method clear() or the
  \link ~PArray() destructor\endlink is called.

  \throw OperationNotSupportedException if the element type \c T of
  this array is not a <tt>Persistable *</tt>.
*/

template <class T> void PArray<T>::setObjectOwner(bool owner)
{
  if (!PArrayTraits::BaseType<T>::IsPersistPtr)
    throw OperationNotSupportedException("PArray<T>::setObjectOwner()\n"
                                   "Can only be owner of Persistable *");

  objOwner = owner;
}

//---------------------------------------------------------------------------
/** Ensures the allocated size of this array (the capacity)
  is at least \p minCap.

  \param minCap The minimum required allocated size, as a number of
  elements (\b not bytes).

  This is a no-op if the \link cap() allocated size\endlink is
  already >= \p minCap.

  Call this method if you intend to add many elements to avoid multiple
  increases of the capacity.

  \note The capacity is never shrunk by this method.
*/

template <class T> void PArray<T>::ensureCapacity(int minCap)
{
  if (lstCap >= minCap) return;

  lstCap = minCap;

  ElemType *mem = (ElemType *)realloc(lst,lstCap*sizeof(ElemType));
  if (mem) lst = mem;
}

//---------------------------------------------------------------------------
/** Sets the allocated size to the current size() plus a \p reserveCap.

  \param reserveCap The extra required capacity beyond the current size() in
  number of elements (\b not bytes).

  This method is a no-op if <tt>size() + reserveCap > cap()</tt>, i.e.
  this method cannot be used to increase the capacity.\n
  Use method ensureCapacity() for that.
*/

template <class T> void PArray<T>::shrinkCapacity(int reserveCap)
{
  if (reserveCap < 0) reserveCap = 0;

  int newCap = lstSz + reserveCap;

  if (newCap < 1) {
    if (lst) free(lst);
    lst = NULL;
    lstSz = lstCap = 0;
    return;
  }
  
  if (newCap == lstSz || newCap >= lstCap) return;

  lstCap = newCap;
  lst = (ElemType *)realloc(lst,lstCap*sizeof(ElemType));
}

//---------------------------------------------------------------------------
/** \class PArray
  A PArray is a generalized Persistable array,
  but suitable only for a limited set of types:

  \li <b>Signed basic types.</b>\n
  The array element can be of type:
  <tt>bool, short, int, long, wchar_t, float</tt> or \c double.\n\n
  \c Unsigned types are \em not supported due to the restrictions
  imposed by the Persistable base class.

  \li <b>Unicode string (i.e. type <tt>wchar_t *)</tt>.</b>\n
  In this case the array will manage and \b own a \b copy
  of any string added.

  \li <b>A class derived from Persistable.</b>\n
  The array will manage and \b own a \b copy of any object added.\n\n
  For this to work the class must have an (implicit or explicit)
  copy constructor or,\n
  in the case of an abstract class, it must have a <tt>clone()</tt> method.\n\n
  The assigment operator of the class will never be called.\n
  \n
  Note that is is \b not possible (yet) to create an array that contains\n
  elements of different types all derived from a base element type \c T,\n
  unless type \c T \b itself is \b abstract.\n
  \n
  In that case consider a \c PArray of \b pointer to your element type.\n
  You may then set the array to be
  \link Ino::PArray::setObjectOwner(bool) owner\endlink of the objects pointed to.\n
  Thus you \b can have a polymorphic array.

  \li <b>A pointer to a class derived from Persistable.</b>\n
  Such a pointer is handled the same as a basic type, i.e only\n
  the pointer value itself is handled by the array, unless the array
  \b owns the objects, see below.\n
  \n
  The copy constructor or assigment operator will never be called.\n
  \n
  The PArray can optionally be made to \b own the objects pointed to,
  see PArray::PArray(bool,int,int) and setObjectOwner(bool).\n
  In that case operator delete \b will be used to the destroy the objects
  pointed to.

  For any other type the
  \link PArray(int, int) PArray<T> constructor\endlink
  will throw an IllegalArgumentException.

  \anchor TypeResolution

  \par <b>Type Resolution</b></a>

  Below the various argument and return types are shown in relation to the
  template type T.\n\n

  (\c AppClass is some class derived from \c Persistable (or \c MainPersistable))\n\n

  <table border="1" cellpadding="4" cellspacing="4">
  <tr><th align="left">Type T</th>          <th align="left">ArgType</th>
      <th align="left">ReturnType</th>      <th align="left">ConstReturnType</th>
  <tr><td><code>bool</code></td>            <td><code>bool</code></td>
      <td><code>bool&</code></td>           <td><code>bool</code></td>
  <tr><td>..</td>                           <td>..</td>
      <td>..</td>                           <td>..(idem \c short etc)</td>
  <tr><td>&nbsp;</td>                       <td>&nbsp;</td>
      <td>&nbsp;</td>                       <td>&nbsp;</td>
  <tr><td><code>wchar_t*</code></td>        <td><code>const wchar_t*</code></td>
      <td><code>const wchar_t*</code></td>  <td><code>const wchar_t*</code></td>
  <tr><td><code>const wchar_t*</code></td>  <td><code>const wchar_t*</code></td>
      <td><code>const wchar_t*</code></td>  <td><code>const wchar_t*</code></td>
  <tr><td>&nbsp;</td>                       <td>&nbsp;</td>
      <td>&nbsp;</td>                       <td>&nbsp;</td>
  <tr><td><code>AppClass</code></td>        <td><code>const AppClass&</code></td>
      <td><code>AppClass&amp;</code></td>   <td><code>const AppClass&amp;</code></td>
  <tr><td><code>const AppClass</code></td>  <td><code>const AppClass&amp;</code></td>
      <td><code>const AppClass&amp;</code></td> <td><code>const AppClass&amp;</code></td>
  <tr><td>&nbsp;</td>                       <td>&nbsp;</td>
      <td>&nbsp;</td>                       <td>&nbsp;</td>
  <tr><td><code>AppClass*</code></td>       <td><code>AppClass*</code></td>
      <td><code>AppClass*&</code></td>      <td><code>AppClass*</code></td>
  <tr><td><code>const AppClass*</code></td> <td><code>const AppClass*</code></td>
      <td><code>const AppClass*</code></td> <td><code>const AppClass*</code></td>
  </table>

 \par Note 1
  If T is <tt>wchar_t*</tt> or <tt>const wchar_t*</tt>, methods add/insert/set will
  insert a \b copy of\n
  the string into the array and that copy is then owned by the array.

 \par Note 2
  If T is \c AppClass or \c const \c AppClass, methods add/insert/set will
  insert a \b copy of\n
  the class into the array and that copy is then owned by the array.\n
  This means an implicit or explicit copy constructor and a destructor must exist.\n
  The class does not need to have an assignment <tt>%operator=()</tt>;

 \par Note 3
  If T is \c AppClass* or \c const \c AppClass* the type is treated just as a
  \c bool, \c int etc.\n
  No copy constructor will ever be called.\n
  Depending on whether this array is \link isObjectOwner() owner\endlink
  of the elements the destructor \b may be called.

 \par <b>Warning: Persistence Restore</b>

  If a PArray is restored from a stream (using PersistentReader) and
  the following applies:\n\n
  \li The element type is a class derived from Persistable
  \li The stream contains one or more elements of a type
  (derived from Persistable) not (yet) known\n
  to the PersistentReader
  (see \ref Ino::PersistentTypeDef "PersistentTypeDef")\n\n

  Then such elements are silently \b removed from the array and the size() of
  the array\n
  may therefore be less than would otherwise have been expected.

  \author C. Wolters
  \date Mar 2010
*/

//---------------------------------------------------------------------------
/** Constructor.
  Creates a new empty PArray.

  \param initCap The initial capacity of the array, may be zero.
  \param capIncrPercent Grow percentage.\n

  \throw IllegalArgumentException
  If the element type is not a basic type, a string <tt>wchar_t *</tt> or\n
  a (pointer to) a class derived from Persistable.

  \p capIncrPercent is the \em percentage by which the capacity will
  grow if more space is required.\n
  If the new capacity required is less than 8, 8 is used instead.\n\n
  If \c capIncrPercent is less than 10 it is silently set to 10.\n
  If \c capIncrPercent is more than 200 it is silently set to 200.

  The size() of the new array is zero.

  \note The array will \b not be \link isObjectOwner() owner\endlink.
*/

template <class T> PArray<T>::PArray(int initCap, int capIncrPercent)
: MainPersistable(), objOwner(false),
  capIncPercent(capIncrPercent),
  lst(NULL), lstSz(0), lstCap(0)
{
  const type_info& tp = typeid(T);

  if (tp != typeid(bool) && tp != typeid(short) && tp != typeid(int) &&
      tp != typeid(long) && tp != typeid(float) && tp != typeid(double) &&
      tp != typeid(wchar_t *) && tp != typeid(const wchar_t *) &&
      !PArrayTraits::BaseType<T>::IsPersist)
        throw IllegalArgumentException("PArray<T>: Unsupported Type");

  if (capIncrPercent < 10)  capIncPercent = 10;
  if (capIncrPercent > 200) capIncPercent = 200;

  if (initCap > 0) ensureCapacity(initCap);
}

//---------------------------------------------------------------------------
/** Constructor.
  Creates a new empty PArray.

  \param owner if \c true:
  \li if element type \c T is a pointer to a class derived from Persistable
  (<tt>Persistable *</tt>) then this array owns the elements pointed
  at (and will delete them when required).<br>
  \li otherwise throws a WrongTypeException.<br>

  \param initCap The initial capacity of the array, may be zero.
  \param capIncrPercent Grow percentage.\n

  \throw WrongTypeException if \a owner is \c true and element
  type \c T is not a pointer to a class derived from Persistable.

  \throw IllegalArgumentException
  If the element type is not a basic type, a string <tt>wchar_t *</tt> or\n
  a (pointer to) a class derived from Persistable.

  \p capIncrPercent is the \em percentage by which the capacity will
  grow if more space is required.\n
  If the new capacity required is less than 8, 8 is used instead.\n\n
  If \c capIncrPercent is less than 10 it is silently set to 10.\n
  If \c capIncrPercent is more than 200 it is silently set to 200.

  The size() of the new array is zero.
*/

template <class T> PArray<T>::PArray(bool owner,
                                     int initCap, int capIncrPercent)
: MainPersistable(), objOwner(owner),
  capIncPercent(capIncrPercent),
  lst(NULL), lstSz(0), lstCap(0)
{
  if (objOwner && !PArrayTraits::BaseType<T>::IsPersistPtr)
    throw WrongTypeException("PArray<T>: Can only be owner of Persistable *");

  const type_info& tp = typeid(T);

  if (tp != typeid(bool) && tp != typeid(short) && tp != typeid(int) &&
      tp != typeid(long) && tp != typeid(float) && tp != typeid(double) &&
      tp != typeid(wchar_t *) && tp != typeid(const wchar_t *) &&
      !PArrayTraits::BaseType<T>::IsPersist)
        throw IllegalArgumentException("PArray<T>: Unsupported Type");

  if (capIncrPercent < 10)  capIncPercent = 10;
  if (capIncrPercent > 200) capIncPercent = 200;

  if (initCap > 0) ensureCapacity(initCap);
}

//---------------------------------------------------------------------------
/** Copy constructor.
  Creates a copy this array.

  \param cp The array to be copied.

  If the element type is:

  \li <b>A class derived from Persistable.</b>\n
  To copy the elements the copy constructor of each element is called\n
  or, in case the element type is abstract, method <tt>clone()</tt>
  is called for each element.

  \li <b>A unicode string <tt>wchar_t *</tt></b>.\n
  To copy the elements each string is copied (if not \c NULL).

  \li <b>A pointer to a class derived from Persistable.</b>\n
  In this case only the pointers are copied.\n
  The copy constructor of the element type is \b not called.

  \note In all cases method isObjectOwner() of this new array will
  return \c false!
*/

template <class T> PArray<T>::PArray(const PArray& cp)
: MainPersistable(cp), objOwner(false),
  capIncPercent(cp.capIncPercent),
  lst(PArrayFuncs::Func<T,TypeVal>::dupLst(cp.lst,cp.lstSz)),
  lstSz(cp.lstSz), lstCap(cp.lstSz)
{
}

//---------------------------------------------------------------------------
/** Destructor.

  First method clear() is called.\n
  Then the array is discarded.
*/

template <class T> PArray<T>::~PArray()
{
  clear();
  if (lst) free(lst);
}

//---------------------------------------------------------------------------
/** Assigment operator.
  Assign another PArray to this array.

  \param src The array to assign to this array.
  \return A reference to this array.

  If the element type is:

  \li <b>A class derived from Persistable.</b>\n
  First the destructor of every element in this array is called.\n\n
  Then the copy constructor is called on each element in \p src or,\n
  if the element type is abstract, method <tt>clone()</tt> is used instead
  to create a copy.

  \li <b>A unicode string <tt>wchar_t *</tt></b>.\n
  First for every element in this array that is not \c NULL,
  operator <tt>delete[]</tt> is called.\n
  \n
  Then for every element in \p src that is not \c NULL, a copy of
  the string in \p src is assigned to this array.

  \li <b>A pointer to a class derived from Persistable.</b>\n
  If method isObjectOwner() would return \c true operator \c delete
  is called on every element.\n
  Then all element pointers are simply copied from the \a src array.

  \note In all cases method isObjectOwner() of this array will
  return \c false after a call to this method,
  regardless of the previous setting!
*/

template <class T> PArray<T>& PArray<T>::operator=(const PArray& src)
{
  clear();
  free(lst);

  objOwner = false;

  lst    = PArrayFuncs::Func<T,TypeVal>::dupLst(src.lst,src.lstSz);
  lstSz  = src.lstSz;
  lstCap = lstSz;

  return *this;
}

//---------------------------------------------------------------------------
/** Clears the array, that is, sets the size() to zero.

  If the element type is:

  \li <b>A class derived from Persistable.</b>\n
  The destructor of every element is called.

  \li <b>A unicode string <tt>wchar_t *</tt></b>.\n
  For every element that is not \c NULL, operator <tt>delete[]</tt>
  is called.

  \li <b>A pointer to a class derived from Persistable.</b>\n
  If method isObjectOwner() would return \c true operator \c delete
  is called on every element.\n
  Otherwise all element pointers are simply discarded.\n
*/

template <class T> void PArray<T>::clear()
{
  if (lst) PArrayFuncs::Func<T,TypeVal>::clearAll(lst,lstSz,objOwner);

  lstSz = 0;
}

//---------------------------------------------------------------------------
/** Adds (appends) an element to this array.

  The size() will increase by one.

  \param item The element to be added.

  If the element type is:

  \li <b>A class derived from Persistable.</b>\n
  A \b copy of \p item is added either by calling copy constructor of
  \p item or,\n
  in case the element type is abstract, method
  <tt>clone()</tt> of \p item  is called.

  \li <b>A unicode string <tt>wchar_t *</tt></b>.\n
  A copy of \p item is added to this array (if \p item is not \c NULL).

  \li <b>A pointer to a class derived from Persistable.</b>\n
  In this case only the pointer \p item is copied.\n
  The copy constructor of the element type is \b not called.
*/

template <class T> int PArray<T>::add(ArgType item)
{
  insert(lstSz,item);

  return lstSz-1;
}

//---------------------------------------------------------------------------
/** Set an element in this array to a new value.

  You will need this method in particular if the template type T is
  (<tt>wchar_t *</tt>),\n
  since method get() and operator[]() do not return an lvalue in this case.

  For all other types direct assigment to an element is possible,\n
  since in that case get() and operator[]() return a \em reference to the
  array element.

  \param idx The index of the element to be overwritten.
  \param item The item to be assigned to
  the element at index \p idx.

  \throw IndexOutOfBoundsException If \p idx < 0 || \p idx >= size()

  If the element type is:

  \li <b>A class derived from Persistable.</b>\n
  First the destructor of the element at index \p idx is called.\n\n
  Then a \b copy of \p item is assigned to the element at index \p idx,
  either by calling the copy constructor of \p item or,
  in case the element type is abstract, by calling
  method <tt>clone()</tt>.

  \li <b>A unicode string <tt>wchar_t *</tt></b>.\n
  First operator <tt>delete[]</tt> is called on the element at
  index \p idx (if not \c NULL).\n
  Then a copy of \p item is assigned to the element at index \p idx.

  \li <b>A pointer to a class derived from Persistable.</b>\n
  If method isObjectOwner() would return \c true, operator \c delete is
  called on the element at index \a idx.\n
  Then the pointer \a item is assigned to the element at index \a idx.\n
  The copy constructor of the element type is \b not called.
*/

template <class T> void PArray<T>::set(int idx, ArgType item)
{
  if (idx < 0 || idx > lstSz)
    throw IndexOutOfBoundsException("PArray<T>::set");

  PArrayFuncs::Func<T,TypeVal>::clearItem(lst[idx],objOwner);

  lst[idx] = PArrayFuncs::Func<T,TypeVal>::newItem(item);
}

//---------------------------------------------------------------------------
/** Inserts a new element into this array.

  The size() is increased by one.

  \param idx The index position \em before which the element is inserted.\n
  If \p idx == size() the new element is \em appended.

  \param item The element to be inserted.

  \throw IndexOutOfBoundsException If \p idx < 0 or \p idx > size().

  If the element type is:

  \li <b>A class derived from Persistable.</b>\n
  A \b copy if \p item is added either by calling the copy constructor of
  \p item or,\n
  in case the element type is abstract, by calling
  method <tt>clone()</tt> of \p item.

  \li <b>A unicode string <tt>wchar_t *</tt></b>.\n
  A copy of \p item is added to this array (if \p item is not \c NULL).

  \li <b>A pointer to a class derived from Persistable.</b>\n
  In this case only the pointer \p item is copied.\n
  The copy constructor of the element type is \b not called.
*/

template <class T> void PArray<T>::insert(int idx, ArgType item)
{
  if (idx < 0 || idx > lstSz)
    throw IndexOutOfBoundsException("PArray<T>::insert");

  if (lstSz >= lstCap) incCapacity();

  memmove(lst+idx+1,lst+idx,(lstSz-idx)*sizeof(ElemType));
  lstSz++;

  lst[idx] = PArrayFuncs::Func<T,TypeVal>::newItem(item);
}

//---------------------------------------------------------------------------
/** Removes an element from this array.

  The size() is decreased by one.

  \param idx The index the element to remove.

  \throw IndexOutOfBoundsException If \p idx < 0 || \p idx >= size()

  If the element type is:

  \li <b>A class derived from Persistable.</b>\n
  The destructor of the element at index \p idx is called.\n\n

  \li <b>A unicode string <tt>wchar_t *</tt></b>.\n
  operator <tt>delete[]</tt> is called on the element at
  index \p idx (if not \c NULL).

  \li <b>A pointer to a class derived from Persistable.</b>\n
  If method isObjectOwner() would return \c true operator \c delete
  is called on the element at index \a idx.\n
  otherwise the element is simply discarded.
*/

template <class T> void PArray<T>::remove(int idx)
{
  if (idx < 0 || idx >= lstSz)
    throw IndexOutOfBoundsException("PArray<T>::remove");

  PArrayFuncs::Func<T,TypeVal>::clearItem(lst[idx],objOwner);

  lstSz--;
  memmove(lst+idx,lst+idx+1,(lstSz-idx)*sizeof(ElemType));
}

//---------------------------------------------------------------------------
/** Swaps two elements in this array.

  \param idx1 The index of the first element to swap.
  \param idx2 The index of the second element to swap.

  \throw IndexOutOfBoundsException<br>
  if <tt>idx1 < 0 || idx1 >= size()</tt> or\n
  &nbsp;&nbsp;&nbsp;<tt>idx2 < 0 || idx2 >= size()</tt>

  Even if the element type is a class derived from Persistable
  the swap merely has to exchange two pointers.\n
  So no copy constructor or destructor will ever be called.
*/

template <class T> void PArray<T>::swap(int idx1, int idx2)
{
  if (idx1 < 0 || idx1 >= lstSz)
    throw IndexOutOfBoundsException("PArray<T>::swap 1");

  if (idx2 < 0 || idx2 >= lstSz)
    throw IndexOutOfBoundsException("PArray<T>::swap 2");

  ElemType el = lst[idx1];
  lst[idx1] = lst[idx2];
  lst[idx2] = el;
}

//---------------------------------------------------------------------------
/** Returns the element at the specified index position.

  \param idx The index of the element to return.
  \return A \em reference to the element at index \p idx or,\n
  if the element type is <tt>wchar_t *</tt> or
  <tt>const wchar_t *</tt> the pointer value itself.

  \throw IndexOutOfBoundsException If \p idx < 0 || \p idx >= size().

  \see \ref TypeResolution

  \note If the element type \c T is a pointer to Persistable and
  isObjectOwner() returns \c true, beware of memory management issues.\n
  It is possible to directy assign another object to an element in the list.\n
  The original object must then eventually be deleted by yourself.
*/

template <class T> typename PArray<T>::ReturnType PArray<T>::get(int idx)
{
  if (idx < 0 || idx >= lstSz)
    throw IndexOutOfBoundsException("PArray<T>::get");

  return PArrayFuncs::Func<T,TypeVal>::getItem(lst[idx]);
}

//---------------------------------------------------------------------------
/** Returns the element at the specified index position.

  \param idx The index of the element to return.
  \return The element at index \p idx or,\n
  if the element type is a class
  derived from Persistable a \c const \em reference to the element.

  \throw IndexOutOfBoundsException If \p idx < 0 || \p idx >= size().

  \see \ref TypeResolution
*/

template <class T> typename PArray<T>::ConstReturnType
                                         PArray<T>::get(int idx) const
{
  if (idx < 0 || idx >= lstSz)
    throw IndexOutOfBoundsException("PArray<T>::get const");

  ConstReturnType v = PArrayFuncs::Func<T,TypeVal>::getItem(lst[idx]);

  return v;
}

//---------------------------------------------------------------------------
/** Returns the element at the specified index position.

  \param idx The index of the element to return.
  \return The element at index \p idx or,\n
  if the element type is a class
  derived from Persistable a \c const \em reference to the element.

  \throw IndexOutOfBoundsException If \p idx < 0 || \p idx >= size().

  \see \ref TypeResolution
*/

template <class T> typename PArray<T>::ConstReturnType
                                    PArray<T>::operator[](int idx) const
{
  if (idx < 0 || idx >= lstSz)
    throw IndexOutOfBoundsException("PArray<T>::operator[] const");

  ConstReturnType v = PArrayFuncs::Func<T,TypeVal>::getItem(lst[idx]);

  return v;
}

//---------------------------------------------------------------------------
/** Returns the element at the specified index position.

  \param idx The index of the element to return.
  \return A \em reference to the element at index \p idx or,\n
  if the element type is <tt>wchar_t *</tt> or
  <tt>const wchar_t *</tt> the pointer value itself.

  \throw IndexOutOfBoundsException If \p idx < 0 || \p idx >= size().

  \see \ref TypeResolution

  \note If the element type \c T is a pointer to Persistable and
  isObjectOwner() returns \c true, beware of memory management issues.\n
  It is possible to directy assign another object to an element in the list.\n
  The original object must then eventually be deleted by yourself.
*/

template <class T> typename PArray<T>::ReturnType
                                    PArray<T>::operator[](int idx)
{
  if (idx < 0 || idx >= lstSz)
    throw IndexOutOfBoundsException("PArray<T>::operator[]");

  return PArrayFuncs::Func<T,TypeVal>::getItem(lst[idx]);
}

//---------------------------------------------------------------------------
#ifdef _WIN32
/** Sort operator for this array.

  \param lt A comparison functor type.\n
  It must be either:
  \li A function: <tt>bool (*lt)(const T& t1, const T& t2);</tt>\n
      The function must must return \c true if and only
      if \a t1 is <em>less than \a t2</em>.\n
  \li A class with <tt>operator()</tt> defined:\n
  <tt>class Comparator
  {
  public:
    bool operator()(const T& t1, const T& t2);
  };</tt>\n
  with similar functionality.

  \note Not (yet) available ion Linux.
*/

template <class T> template <class LessThan>
                                         void PArray<T>::sort(LessThan& lt)
{
  if (lstSz < 2) return;

  std::sort(lst,lst+lstSz,lt);
}

//---------------------------------------------------------------------------
/** Stable sort operator for this array.

  See the STL documentation for a definition of stable sort.

  \param lt A comparison functor type.\n
  It must be either:
  \li A function: <tt>bool (*lt)(const T& t1, const T& t2);</tt>\n
      The function must must return \c true if and only
      if \a t1 is <em>less than \a t2</em>.\n
  \li A class with <tt>operator()</tt> defined:\n
  <tt>class Comparator
  {
  public:
    bool operator()(const T& t1, const T& t2);
  };</tt>\n
  with similar functionality.

  \note Not (yet) available ion Linux.
*/

template <class T> template <class LessThan> 
                                    void PArray<T>::stableSort(LessThan *lt)
{
  if (lstSz < 2) return;

  std::stable_sort(lst,lst+lstSz-1,lt);
}
#endif

//---------------------------------------------------------------------------
// Persistence Section

//---------------------------------------------------------------------------

namespace PArrayFuncs
{

/**
 @}
*/

} // namespace Ino

//---------------------------------------------------------------------------
