//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- General basic types, constants and functions ----------------------
//---------------------------------------------------------------------------
//------- Copyright Inofor Hoek Aut BV NOV 2000 -----------------------------
//---------------------------------------------------------------------------
//------- C. Wolters --------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// 2011-03-04 Now in SVN

#include "Basics.h"

#include <cctype>
#include <cwctype>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cwchar>
#include <ctime>
#include <cstdio>

//---------------------------------------------------------------------------
/** \namespace Ino
  Namespace Ino is used for library functions that are developed by
  (and copyright of) Inofor Hoek Aut BV.

  Copyright &copy; 1993..2015 Inofor Hoek Aut BV.\n
  \n
  All classes, functions and other source code that is part of
  this namespace may not be used without written permission
  from:\n
  Inofor Hoek Aut BV\n
  Derde Broekdijk 18\n
  7132 EC Lichtenvoorde\n
  The Netherlands\n
  \n
  <a href="http://www.inofor.nl">www.inofor.nl</a>

  \author C. Wolters
  \date May 2014
*/

namespace Ino
{

//---------------------------------------------------------------------------
/** \addtogroup general_functions General Functions
 @{
*/

//---------------------------------------------------------------------------
/** \file Basics.h
  Defines a set of general purpose functions and classes.

  The string manipulation functions generally have both an ASCII and a
  Unicode variant.

  \author C. Wolters
  \date June 2005
*/

//---------------------------------------------------------------------------
/// General very small tolerance value.
const double NumAccuracy = 1.0e-16;

/// Conversion factor from inch to mm.
const double InchInMm = 25.4;

static char    convBuf[128]  = "";
static wchar_t convBufW[128] = L"";

static double radToDegs = 180.0/3.141592653589793238462643383279502884197169;

//---------------------------------------------------------------------------
//--- The version of this library -------------------------------------------
//---------------------------------------------------------------------------

void inoLibVersion(unsigned char& major, unsigned char& minor,
                                                  unsigned char& release)
{
  major   = 1;
  minor   = 1;
  release = 1;
}

//---------------------------------------------------------------------------
/** \fn inline T sqr(T v)
  Returns the square of a value.

  \param v The value to be squared, can be any numeric type.
  \return The square of parameter \c v.
*/

//---------------------------------------------------------------------------
/** \fn inline T abs(T v)
  Returns the absolute counterpart of a value.

  \param v The value to be returned absolute, can be any numeric type.
  \return The absolute value of parameter \c v.
*/

//---------------------------------------------------------------------------
/** Trims white space from both ends of an ASCII character string.
  \param line The string to trim, may be \c NULL, in which case this function
  is a no-op.
*/

void trim(char *line)
{
  if (!line) return;

  long sz = (long)strlen(line);

  for (long i=sz-1; i>=0; --i) {
    if (isspace(line[i])) line[i] = '\0';
    else break;
  }

  sz = (long)strlen(line);

  for (int i=0; i<sz; ++i) {
    if (!isspace(line[i])) {
      memmove(line,line+i,sz+1-i);
      return;
    }
  }
}

//---------------------------------------------------------------------------
/** Trims white space from both ends of a Unicode string.
  \param line The string to trim, may be \c NULL, in which case this function
  is a no-op.
*/

void trim(wchar_t *line)
{
  if (!line) return;

  long sz = (long)wcslen(line);

  for (long i=sz-1; i>=0; --i) {
    if (iswspace(line[i])) line[i] = '\0';
    else break;
  }

  sz = (long)wcslen(line);

  for (int i=0; i<sz; ++i) {
    if (!iswspace(line[i])) {
      memmove(line,line+i,(sz+1-i)*sizeof(wchar_t));
      return;
    }
  }
}

//---------------------------------------------------------------------------
/** Converts the characters in a ASCII string to uppercase.
  \param s The string to convert, if \c NULL this function is a no-op
*/

void toUpper(char *s)
{
  if (!s) return;

  while (*s) {
    *s = (char)toupper(*s);
    ++s;
  }
}

//---------------------------------------------------------------------------
/** Converts the characters in a Unicode string to uppercase.
  \param s The string to convert, if \c NULL this function is a no-op
*/

void toUpper(wchar_t *s)
{
  if (!s) return;

  while (*s) {
    *s = towupper(*s);
    ++s;
  }
}

//---------------------------------------------------------------------------
/** Converts the characters in an ASCII string to lowercase.
  \param s The string to convert, if \c NULL this function is a no-op
*/

void toLower(char *s)
{
  if (!s) return;

  while (*s) {
    *s = (char)tolower(*s);
    ++s;
  }
}

//---------------------------------------------------------------------------
/** Converts the characters in a Unicode string to lowercase.
  \param s The string to convert, if \c NULL this function is a no-op
*/

void toLower(wchar_t *s)
{
  if (!s) return;

  while (*s) {
    *s = towlower(*s);
    ++s;
  }
}

//---------------------------------------------------------------------------
/** Returns a duplicate of an ASCII string in a newly allocated buffer.
  \param s The string to duplicate, may be \c NULL.
  \return A newly allocated buffer that contains a copy of the supplied string
  or \c NULL if \c NULL was supplied in parameter \c s.
*/

char *dupStr(const char *s)
{
  if (!s) return NULL;

  char *buf = new char[strlen(s)+1];

  strcpy(buf,s);

  return buf;
}


//---------------------------------------------------------------------------
/** Returns a duplicate of a Unicode string in a newly allocated buffer.
  \param s The string to duplicate, may be \c NULL.
  \return A newly allocated buffer that contains a copy of the supplied string
  or \c NULL if \c NULL was supplied in parameter \c s.
*/

wchar_t *dupStr(const wchar_t *s)
{
  if (!s) return NULL;

  wchar_t *buf = new wchar_t[wcslen(s)+1];

  wcscpy(buf,s);

  return buf;
}

//---------------------------------------------------------------------------
/** Returns a duplicate of an ASCII string as a \b Unicode string in a
  newly allocated buffer.
  \param s The ASCII string to duplicate, may be \c NULL.
  \return A newly allocated buffer that contains a copy of the supplied string
  or \c NULL if \c NULL was supplied in parameter \c s.
*/

wchar_t *dupChar2WChar(const char *s)
{
  if (!s) return NULL;

  long len = (long)strlen(s);

  wchar_t *newStr = new wchar_t[len+1];

  for (long i=0; i<len; ++i) newStr[i] = s[i];

  newStr[len] = 0;

  return newStr;
}

//---------------------------------------------------------------------------
/** Converts an ASCII string to a Unicode string.
    \param s The ASCII string to convert.
    \param ws The string to receive the Unicode equivalent of the ASCII
    string.

    \note
    If parameter \c s or \c ws is \c NULL, then this function is a no-op.
    \note
    The capacity of the destination buffer \c ws must be sufficient, or else
    unexpected behavior may be the result.
*/

void char2WChar(const char *s, wchar_t *ws)
{
  if (!s || !ws) return;

  while ((*ws++ = *s++) != 0) {}
}

//---------------------------------------------------------------------------
/** Returns a duplicate of a Unicode string as an \b ASCII string in a
  newly allocated buffer.
  \param s The Unicode string to duplicate, may be \c NULL.
  \return A newly allocated buffer that contains a copy of the supplied string
  or \c NULL if \c NULL was supplied in parameter \c s.

  \note
  All Unicode characters in the supplied string with a numeric value greater
  than 127 will be represented by a hash mark '#' in the returned string.
*/

char *dupWChar2Char(const wchar_t *s)
{
  if (!s) return NULL;

  long len = (long)wcslen(s);

  char *newStr = new char[len+1];

  for (long i=0; i<len; ++i) {
    if (s[i] > 127) newStr[i] = '#';
    else newStr[i] = (char)s[i];
  }

  newStr[len] = 0;

  return newStr;
}

//---------------------------------------------------------------------------
/** Converts a Unicode string to an ASCII string.
    \param s The Unicode string to convert.
    \param ws The string to receive the ASCII equivalent of the Unicode
    string.

    \note
    If parameter \c ws or \c s is \c NULL, then this function is a no-op.
    \note
    The capacity of the destination buffer \c ws must be sufficient, or else
    unexpected behavior may be the result.
    \note
    All Unicode characters in the supplied string with a numeric value greater
    than 127 will be represented by a hash mark '#' in the resulting ASCII string.
*/

void WChar2Char(const wchar_t *ws, char *s)
{
  if (!ws || !s) return;

  while (*ws) {
    if (*ws > 127) *s = '#';
    else           *s = (char)*ws;

    ws++;
    s++;
  }

  *s = 0;
}

//---------------------------------------------------------------------------
/** Generates a hashcode for an ASCII string.
  \param s The string to generate the hashcode for, may be \c NULL.
  \return The hashcode, zero if returned if <tt>s == NULL</tt> or if the
  supplied string is empty.
*/

long hashCode(const char *s)
{
  if (!s) return 0;

  int hash = 0;

  while (*s) {
    hash ^= *s++;
    hash <<= 1;
  }

  return hash;
}

//---------------------------------------------------------------------------
/** Generates a hashcode for a Unicode string.
  \param s The string to generate the hashcode for, may be \c NULL.
  \return The hashcode, zero if returned if <tt>s == NULL</tt> or if the
  supplied string is empty.
*/

long hashCode(const wchar_t *s)
{
  if (!s) return 0;

  int hash = 0;

  while (*s) {
    hash ^= *s++;
    hash <<= 1;
  }

  return hash;
}

//---------------------------------------------------------------------------
/** Reads a field from a comma separated ASCII string.

  This function extracts the first field from a string that contains a comma
  separated list of values.

  \param s The string to read the field from, if \c NULL this function is
  a no-op.
  \param buf The buffer to receive the extracted field, must have sufficient
  capacity and must \b not be \c NULL.
  \param trimFld If \c true the resulting field will have beem \ref trim(char *)
  "trimmed" at both ends.
  \return A pointer to the first unread character in parameter \c s, or \c NULL
  if parameter <tt>s == NULL</tt>.\n
  Use this pointer to extract the next field from the string.

  \note
  Parameter \c buf \b cannot be \c NULL and must be of sufficient size or else
  unexpected behavior will occur.
  \note
  This function never returns \c NULL (unless parameter <tt>s == NULL</tt>).\n
  The returned pointer will however point to an empty string if no more fields
  are left in the string.
  \note Parameter \c buf may contain an empty string on return if the string
  pointed to by parameter \c s happens to contain only white space and parameter
  <tt>trimFld == true</tt>!
*/

const char *readField(const char *s, char *buf, bool trimFld)
{
  char *d = buf;

  while (*s && *s != ',') *d++ = *s++;
  *d = '\0';

  if (trimFld) trim(buf);

  if (*s) s++; // Skip comma

  return s;
}

//---------------------------------------------------------------------------
/** Reads a field from a comma separated Unicode string.

  This function extracts the first field from a string that contains a comma
  separated list of values.

  \param s The string to read the field from, if \c NULL this function is
  a no-op.
  \param buf The buffer to receive the extracted field, must have sufficient
  capacity and must \b not be \c NULL.
  \param trimFld If \c true the resulting field will have beem \ref trim(char *)
  "trimmed" at both ends.
  \return A pointer to the first unread character in parameter \c s, or \c NULL
  if parameter <tt>s == NULL</tt>.\n
  Use this pointer to extract the next field from the string.

  \note
  Parameter \c buf \b cannot be \c NULL and must be of sufficient size or else
  unexpected behavior will occur.
  \note
  This function never returns \c NULL (unless parameter <tt>s == NULL</tt>).\n
  The returned pointer will however point to an empty string if no more fields
  are left in the string.
  \note Parameter \c buf may contain an empty string on return if the string
  pointed to by parameter \c s happens to contain only white space and parameter
  <tt>trimFld == true</tt>!
*/


const wchar_t *readField(const wchar_t *s, wchar_t *buf, bool trimFld)
{
  wchar_t *d = buf;

  while (*s && *s != ',') *d++ = *s++;
  *d = '\0';

  if (trimFld) trim(buf);

  if (*s) s++; // Skip comma

  return s;
}

//---------------------------------------------------------------------------
/** Converts an ASCII string to an integer.
   It uses standard function \c strtol for the actual conversion.

   \param s The string to read the integer from, may be \c NULL or empty.
   A copy of this string is first \ref trim(char *) "trimmed" and then
   converted.
   \param intVal Receives the result.
   \return \c true if and only if the trimmed string contains a valid integer
   and no characters are left to be read after the conversion.

   Use function \ref readField(const char *s, char *buf, bool trimFld) "readField"
   together with this function to extract a series of numbers from a string.
*/

bool toInt(const char *s, long& intVal)
{
  if (!s || strlen(s) < 1) return false;

  char buf[256] = "";
  strcpy(buf,s);
  trim(buf);

  char *ep=NULL;

  intVal = strtol(buf,&ep,10);

  if (ep != buf+strlen(buf)) return false;

  return true;
}

//---------------------------------------------------------------------------
/** Converts a Unicode string to an integer.
   It uses standard function \c wcstol for the actual conversion.

   \param s The string to read the integer from, may be \c NULL or empty.
   A copy of this string is first \ref trim(wchar_t *) "trimmed" and then
   converted.
   \param intVal Receives the result.
   \return \c true if and only if the trimmed string contains a valid integer
   and no characters are left to be read after the conversion.\n
   
   Use function \ref readField(const wchar_t *s, wchar_t *buf, bool trimFld)
   "readField" together with this function to extract a series of numbers
   from a string.
*/

bool toInt(const wchar_t *s, long& intVal)
{
  if (!s || wcslen(s) < 1) return false;

  wchar_t buf[256] = L"";
  wcscpy(buf,s);
  trim(buf);

  wchar_t *ep=NULL;

  intVal = wcstol(buf,&ep,10);

  if (ep != buf+wcslen(buf)) return false;

  return true;
}

//---------------------------------------------------------------------------
/** Converts an ASCII string to a double value.
   It uses standard function \c strtod for the actual conversion.

   \param s The string to read the double value from, may be \c NULL or empty.
   A copy of this string is first \ref trim(char *) "trimmed" and then
   converted.
   \param dblVal Receives the result.
   \return \c true if and only if the trimmed string contains a valid double
   value and no characters are left to be read after the conversion.

   Use function \ref readField(const char *s, char *buf, bool trimFld) "readField"
   together with this function to extract a series of numbers from a string.
*/

bool toDouble(const char *s, double& dblVal)
{
  if (!s || strlen(s) < 1) return false;

  char buf[256] = "";
  strcpy(buf,s);
  trim(buf);

  char *ep=NULL;

  dblVal = strtod(buf,&ep);

  if (ep != buf+strlen(buf)) return false;

  return true;
}

//---------------------------------------------------------------------------
/** Converts a Unicode string to a double value.
   It uses standard function \c wcstod for the actual conversion.

   \param s The string to read the double value from, may be \c NULL or empty.
   A copy of this string is first \ref trim(wchar_t *) "trimmed" and then
   converted.
   \param dblVal Receives the result.
   \return \c true if and only if the trimmed string contains a valid double
   value and no characters are left to be read after the conversion.\n
   
   Use function \ref readField(const wchar_t *s, wchar_t *buf, bool trimFld)
   "readField" together with this function to extract a series of numbers
   from a string.
*/

bool toDouble(const wchar_t *s, double& dblVal)
{
  if (!s || wcslen(s) < 1) return false;

  wchar_t buf[256] = L"";
  wcscpy(buf,s);
  trim(buf);

  wchar_t *ep=NULL;

  dblVal = wcstod(buf,&ep);

  if (ep != buf+wcslen(buf)) return false;

  return true;
}

//---------------------------------------------------------------------------
/** Converts an integer to an ASCII string.
    This method converts an integer to a const char* string. It uses sprintf
    for the conversion.
    \param val the integer value to convert.
    \return A pointer to a static buffer that contains the converted string.

    \note
    The returned string will be overwritten once this function or a similar
    one is called.
*/

const char *fromInt(long val)
{
  sprintf(convBuf,"%ld",val);

  return convBuf;
}

//---------------------------------------------------------------------------
/** Converts an integer to a Unicode string.
    This method converts an integer to a const char* string. It uses swprintf
    for the conversion.
    \param val the integer value to convert.
    \return A pointer to a static buffer that contains the converted string.

    \note
    The returned string will be overwritten once this function or a similar
    one is called.
*/

const wchar_t *fromIntW(long val)
{
  swprintf(convBufW,128,L"%d",val);

  return convBufW;
}

//---------------------------------------------------------------------------
/** Converts a double to an ASCII string.
    This method converts a double to a const char* string. It uses sprintf
    for the conversion.
    \param val the double value to convert.
    \param digits the number of fractional digits required,
    <tt>0 <= digits <= 17</tt>.
    \return A pointer to a static buffer that contains the converted string.

    \note
    The returned string will be overwritten once this function or a similar
    one is called.
*/

const char *fromDouble(double val, int digits)
{
  if (digits < 0) sprintf(convBuf,"%G",val);
  else {
    if (digits > 17) digits = 17;
    char spec[32] = "";
    sprintf(spec,"%%.%df",digits);

    sprintf(convBuf,spec,val);
  }

  return convBuf;
}

//---------------------------------------------------------------------------
/** Converts a double to a Unicode string.
    This method converts a double to a const char* string. It uses sprintf
    for the conversion.
    \param val the double value to convert.
    \param digits the number of fractional digits that has to converted,
    <tt>0 <= digits <= 17)</tt>.
    \return A pointer to a static buffer that contains the converted string.

    \note
    The returned string will be overwritten once this function or a similar
    one is called.
*/

const wchar_t *fromDoubleW(double val, int digits)
{
  if (digits < 0) swprintf(convBufW,128,L"%G",val);
  else {
    if (digits > 17) digits = 17;
    wchar_t spec[32] = L"";

    swprintf(spec,32,L"%%.%df",digits);
    swprintf(convBufW,128,spec,val);
  }

  return convBufW;
}

//---------------------------------------------------------------------------
/** Rounds a floating point value.
  \param val The value to round.
  \param delta The relative round off value.
  \return The rounded value.

  Example:\n
  <tt> val = 101.1234;\n
       delta = 0.01;\n
  </tt>\n
  Then then result is: 101.12\n
  \n
  The actual algorithm:\n
  <tt>
     if (val < 0.0)\n
         return floor(val/delta + 0.5) * delta;\n
     else return ceil (val/delta - 0.5) * delta;
  </tt>
*/

double round(double val, double delta)
{
  delta = fabs(delta);

  if (delta < fabs(val) * 1e-16) return val;

  if (val < 0.0) 
       return floor(val/delta + 0.5) * delta;
  else return ceil (val/delta - 0.5) * delta;
}

//---------------------------------------------------------------------------
/** Compares two ASCII strings lexicographically.
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.
    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
    \return -1 If \c s1 is less than \c s2.\n
    0 If \c s1 equals \c s2.\n
    1 If \c s1 is greater than \c s2.
 */

int compareStr(const char *s1, const char *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  return strcmp(s1,s2);
}

//---------------------------------------------------------------------------
/** Compares two Unicode strings lexicographically.
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.
    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
    \return -1 If \c s1 is less than \c s2.\n
     0 If \c s1 equals \c s2.\n
     1 If \c s1 is greater than \c s2.
 */

int compareStr(const wchar_t *s1, const wchar_t *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  return wcscmp(s1,s2);
}

//---------------------------------------------------------------------------
/** Compares two ASCII strings lexicographically,  ignoring case.
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.
    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
    \return -1 If \c s1 is less than \c s2.\n
    0 If \c s1 equals \c s2.\n
    1 If \c s1 is greater than \c s2.
*/

int compareNcStr(const char *s1, const char *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  while (*s1 && *s2) {
    char c1 = (char)tolower(*s1++);
    char c2 = (char)tolower(*s2++);

    if (c1 < c2) return -1;
    else if (c1 > c2) return 1;
  }

  if (*s1) return  1;
  if (*s2) return -1;

  return 0;
}

//---------------------------------------------------------------------------
/** Compares two Unicode strings lexicographically,  ignoring case.
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.
    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
    \return -1 If \c s1 is less than \c s2.\n
    0 If \c s1 equals \c s2.\n
    1 If \c s1 is greater than \c s2.
*/

int compareNcStr(const wchar_t *s1, const wchar_t *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  while (*s1 && *s2) {
    wchar_t c1 = towlower(*s1++);
    wchar_t c2 = towlower(*s2++);

    if (c1 < c2) return -1;
    else if (c1 > c2) return 1;
  }

  if (*s1) return  1;
  if (*s2) return -1;

  return 0;
}

//---------------------------------------------------------------------------
/** Compares two ASCII strings <em>alpha numerically</em>, as opposed to
    lexicographically.
  
    Sequences of digits embedded in the string are treated as integer numbers
    and compared as such.\n
    \n
    For example: John1Doe is less than John10Doe.\n
    \n
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.

    \note Numerical substrings are accumulated in a \c long \c long,\n
          and thus may overflow if they are too long.

    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
    \return -1 If \c s1 is less than \c s2.\n
    0 If \c s1 equals \c s2.\n
    1 If \c s1 is greater than \c s2.
*/

int compareAlphaNum(const char *s1, const char *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  while (*s1 && *s2) {
    char c1 = 0, c2 = 0;
    unsigned __int64 v1 = 0, v2 = 0;
    int digs1 = 0, digs2 = 0;

    while (*s1 && isdigit(c1 = *s1++)) {
      digs1++;
      v1 *= 10; v1 += (c1 - '0');
    }

    while (*s2 && isdigit(c2 = *s2++)) {
      digs2++;
      v2 *= 10; v2 += (c2 - '0');
    }

    if (v1 < v2) return -1;
    if (v1 > v2) return  1;

    if (digs1 < digs2) return -1;
    if (digs1 > digs2) return  1;

    if (c1 < c2) return -1;
    if (c1 > c2) return  1;
  }

  if (*s1) return  1;
  if (*s2) return -1;

  return 0;
}

//---------------------------------------------------------------------------
/** Compares two ASCII strings <em>alpha numerically</em>, as opposed to
    lexicographically, <b>ignoring case</b>.
  
    Sequences of digits embedded in the string are treated as integer numbers
    and compared as such.\n
    \n
    For example: John1Doe is less than John10Doe.\n
    \n
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.

    \note Numerical substrings are accumulated in a \c long \c long,\n
          and thus may overflow if they are too long.

    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
    \return -1 If \c s1 is less than \c s2.\n
    0 If \c s1 equals \c s2.\n
    1 If \c s1 is greater than \c s2.
*/

int compareNcAlphaNum(const char *s1, const char *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  while (*s1 && *s2) {
    char c1 = 0, c2 = 0;
    unsigned __int64 v1 = 0, v2 = 0;
    int digs1 = 0, digs2 = 0;

    while (*s1 && isdigit(c1 = (char)tolower(*s1++))) {
      digs1++;
      v1 *= 10; v1 += (c1 - '0');
    }

    while (*s2 && isdigit(c2 = (char)tolower(*s2++))) {
      digs2++;
      v2 *= 10; v2 += (c2 - '0');
    }

    if (v1 < v2) return -1;
    if (v1 > v2) return  1;

    if (digs1 < digs2) return -1;
    if (digs1 > digs2) return  1;

    if (c1 < c2) return -1;
    if (c1 > c2) return  1;
  }

  if (*s1) return  1;
  if (*s2) return -1;

  return 0;
}

//---------------------------------------------------------------------------
/** Compares two Unicode strings <em>alpha numerically</em>, as opposed to
    lexicographically.
  
    Sequences of digits embedded in the string are treated as integer numbers
    and compared as such.\n
    \n
    For example: John1Doe is less than John10Doe.\n
    \n
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.

    \note Numerical substrings are accumulated in a \c long \c long,\n
          and thus may overflow if they are too long.

    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
   \return -1 If \c s1 is less than \c s2.\n
    0 If \c s1 equals \c s2.\n
    1 If \c s1 is greater than \c s2.
*/

int compareAlphaNum(const wchar_t *s1, const wchar_t *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  while (*s1 && *s2) {
    wchar_t c1 = 0, c2 = 0;
    unsigned __int64 v1 = 0, v2 = 0;
    int digs1 = 0, digs2 = 0;

    while (*s1 && iswdigit(c1 = *s1++)) {
      digs1++;
      v1 *= 10; v1 += (c1 - '0');
    }

    while (*s2 && iswdigit(c2 = *s2++)) {
      digs2++;
      v2 *= 10; v2 += (c2 - '0');
    }

    if (v1 < v2) return -1;
    if (v1 > v2) return  1;

    if (digs1 < digs2) return -1;
    if (digs1 > digs2) return  1;

    if (c1 < c2) return -1;
    if (c1 > c2) return  1;
  }

  if (*s1) return  1;
  if (*s2) return -1;

  return 0;
}

//---------------------------------------------------------------------------
/** Compares two Unicode strings <em>alpha numerically</em>, as opposed to
    lexicographically, <b>ignoring case</b>.
  
    Sequences of digits embedded in the string are treated as integer numbers
    and compared as such.\n
    \n
    For example: John1Doe is less than John10Doe.\n
    \n
    This function also allows the one or both parameters to be \c NULL.\n
    A \c NULL pointer is defined to be less than any non \c NULL string.

    \note Numerical substrings are accumulated in a \c long \c long,\n
          and thus may overflow if they are too long.

    \param s1 The string to compare, may be \c NULL.
    \param s2 The string to compare with, may be \c NULL.
    \return -1 If \c s1 is less than \c s2.\n
    0 If \c s1 equals \c s2.\n
    1 If \c s1 is greater than \c s2.
*/

int compareNcAlphaNum(const wchar_t *s1, const wchar_t *s2)
{
  if (!s1) {
    if (s2) return -1;
    else return 0;
  }
  else if (!s2) return 1;

  while (*s1 && *s2) {
    wchar_t c1 = 0, c2 = 0;
    unsigned __int64 v1 = 0, v2 = 0;
    int digs1 = 0, digs2 = 0;

    while (*s1 && iswdigit(c1 = towlower(*s1++))) {
      digs1++;
      v1 *= 10; v1 += (c1 - '0');
    }

    while (*s2 && iswdigit(c2 = towlower(*s2++))) {
      digs2++;
      v2 *= 10; v2 += (c2 - '0');
    }

    if (v1 < v2) return -1;
    if (v1 > v2) return  1;

    if (digs1 < digs2) return -1;
    if (digs1 > digs2) return  1;

    if (c1 < c2) return -1;
    if (c1 > c2) return  1;
  }

  if (*s1) return  1;
  if (*s2) return -1;

  return 0;
}

//---------------------------------------------------------------------------
/** Converts radians to degrees.
    \param val the value to convert from radians.
    \return The value converted to degrees.
*/

double toDegrees(double val)
{
  return radToDegs * val;
}

//---------------------------------------------------------------------------
/** Converts degrees to radians.
    \param val the value to convert from degrees.
    \return The value converted to radians.
*/

double toRadians(double val)
{
  return val / radToDegs;
}

//---------------------------------------------------------------------------
/** Returns the current time in seconds since January 1 1970.
   \return The current time.
*/
long currentTime()
{
#ifdef _WIN32
  return _time32(NULL);
#else
  return time(NULL);
#endif
}

//---------------------------------------------------------------------------
/** Generates a readable ASCII date-time string that is valid as (part of) a
    filename.

    The format is: <tt>-%Y-%m-%d %H.%M.%S</tt>.
    \param s The string to receive the result, must have a capacity of at
    least 19 bytes.
    \param cap The capacity of the string pointed to by parameter \c s.
    \return \c true If the operation was successful,\n
    \c false if <tt>s == NULL || cap < 19</tt>.
*/

bool fileDateTime(char *s, size_t cap)
{
  if (!s || cap < 19) return false;

  time_t now = time(NULL);

  strftime(s,19,"-%Y-%m-%d %H.%M.%S",localtime(&now));

  return true;
}

//---------------------------------------------------------------------------
/** Generates a readable Unicode date-time string that is valid as (part of)
    a filename.
    The format is: <tt>-%Y-%m-%d %H.%M.%S</tt>.
    \param s The string to receive the result, must have a capacity of at
    least 19 bytes.
    \param cap The capacity of the string pointed to by parameter \c s.
    \return \c true If the operation was successful,\n
    \c false if <tt>s == NULL || cap < 19</tt>.
*/

bool fileDateTime(wchar_t *s, size_t cap)
{
  if (!s || cap < 19) return false;

  time_t now = time(NULL);

  wcsftime(s,19,L"-%Y-%m-%d %H.%M.%S",localtime(&now));

  return true;
}

//---------------------------------------------------------------------------
/**
 @}
*/

//---------------------------------------------------------------------------
//----  Exception documentation
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/** \addtogroup exceptions Exception Classes
 @{
*/

/** \file Exceptions.h
  In this header a set of <em>standard</em> exception classes are defined.

  An exception class has been defined for the most common cases.\n
  More classes may be added as required.\n
  All constructor accept a string argument, that may be used to
  document the reason of the exception.
  \n
  All classes (except OutOfMemoryException) are derived directly or
  indirectly derived from \c logic_error (A C++ standard exception class).\n
  \n
  The exceptions are divided in two classes:
  \par Programming Error Exceptions
  These are essentially throw due to programming errors and should \b not
  be caught:
  \li Ino::NullPointerException
  \li Ino::IndexOutOfBoundsException
  \li Ino::IllegalArgumentException
  \li Ino::IllegalStateException
  \li Ino::OperationNotSupportedException

  \par Application Exceptions
  These are mostly user induced and arenormally caught and handled.\n
  The classes derived from IOException relate to file I/O exceptions.
  \li Ino::NoSuchElementException
  \li Ino::DuplicateNameException
  \li Ino::InterruptedException
  \li Ino::IOException
  \li Ino::IllegalFormatException
  \li Ino::NumberFormatException
  \li Ino::FileFormatException
  \li Ino::StreamCorruptedException
  \li Ino::StreamClosedException
  \li Ino::StreamAbortedException
  \li Ino::FileNotFoundException
  \li Ino::AccessDeniedException

  \author C. Wolters
  \date June 2005
*/

//---------------------------------------------------------------------------
/** \class OutOfMemoryException
  To be thrown in case no memory could be allocated by the application.
*/

//---------------------------------------------------------------------------
/** \fn OutOfMemoryException::OutOfMemoryException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class NullPointerException
  Thrown if a pointer argument of a method or function is \c NULL,
  where it should not be.
*/

//---------------------------------------------------------------------------
/** \fn NullPointerException::NullPointerException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class IndexOutOfBoundsException
  Thrown if an array index is out of its legal bounds.
*/

//---------------------------------------------------------------------------
/** \fn IndexOutOfBoundsException::IndexOutOfBoundsException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class IllegalArgumentException
  Thrown if a parameter to a method or function has an unexpected or
  unacceptable value.
*/

//---------------------------------------------------------------------------
/** \fn IllegalArgumentException::IllegalArgumentException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class IllegalStateException
  Thrown if a class or datastructure would enter (or has entered) an
  illegal or inconsistent state while a method or function is being
  executed.
*/

//---------------------------------------------------------------------------
/** \fn IllegalStateException::IllegalStateException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class WrongTypeException
  Thrown if a class (or template) or one of its methods is being passed
  an argument of a type that is not supported.
*/

//---------------------------------------------------------------------------
/** \fn WrongTypeException::WrongTypeException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class OperationNotSupportedException
  Thrown if a method or function is required to execute an operation
  that is not (currently) supported. 
*/

//---------------------------------------------------------------------------
/** \fn OperationNotSupportedException::OperationNotSupportedException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class NoSuchElementException
  Thrown if a find or search operation failed. 
*/

//---------------------------------------------------------------------------
/** \fn NoSuchElementException::NoSuchElementException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class DuplicateNameException
  Thrown if for instance an insert, append or add operation fails because
  an element with the same name or id already exists.
*/

//---------------------------------------------------------------------------
/** \fn DuplicateNameException::DuplicateNameException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class InterruptedException
  Thrown if an (asynchronous) operation was unexpectedly interrupted or
  terminated.
*/

//---------------------------------------------------------------------------
/** \fn InterruptedException::InterruptedException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class IOException
  General I/O related exception class.
  
  More specific derived classes are available.
*/

//---------------------------------------------------------------------------
/** \fn IOException::IOException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class IllegalFormatException
  Thrown if a file, datastream (or other data) does not have the expected
  format.
*/

//---------------------------------------------------------------------------
/** \fn IllegalFormatException::IllegalFormatException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class NumberFormatException
  Thrown if supplied data cannot be converted to a number.
*/

//---------------------------------------------------------------------------
/** \fn NumberFormatException::NumberFormatException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class FileFormatException
  Thrown if data read from a file is not in the expected format.
*/

//---------------------------------------------------------------------------
/** \fn FileFormatException::FileFormatException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class StreamCorruptedException
  Thrown if data read from stream is not in the expected format.
*/

//---------------------------------------------------------------------------
/** \fn StreamCorruptedException::StreamCorruptedException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class StreamClosedException
  Thrown if an attempt to read data from a stream has failed because the
  stream has been closed.
*/

//---------------------------------------------------------------------------
/** \fn StreamClosedException::StreamClosedException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class StreamAbortedException
  Thrown if an attempt to read data from a stream has failed because the
  read operation was aborted (perhaps due to an external cause).
*/

//---------------------------------------------------------------------------
/** \fn StreamAbortedException::StreamAbortedException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class FileNotFoundException
  Thrown if a specific file could not be found.
*/

//---------------------------------------------------------------------------
/** \fn FileNotFoundException::FileNotFoundException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/** \class AccessDeniedException
  Thrown if access to some resource was denied.
*/

//---------------------------------------------------------------------------
/** \fn AccessDeniedException::AccessDeniedException(const string& s)
  Constructor.
  \param s A reason or comment.
*/

//---------------------------------------------------------------------------
/**
 @}
*/

} // namespace Ino

//---------------------------------------------------------------------------
