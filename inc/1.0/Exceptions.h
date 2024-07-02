//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Standard Exceptions -----------------------------------------------
//---------------------------------------------------------------------------
//------- Copyright Inofor Hoek Aut BV Jul 2005 -----------------------------
//---------------------------------------------------------------------------
//------- C. Wolters --------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef INOEXCEPTIONS_INC
#define INOEXCEPTIONS_INC

#include <stdexcept>

namespace Ino
{

//using namespace std;

//---------------------------------------------------------------------------
// All exceptions below are derived from logic_error
// (directly or indirectly), except OutOfmemoryException.
//---------------------------------------------------------------------------

class OutOfMemoryException : public std::runtime_error
{
  public:
    OutOfMemoryException(const std::string& s) : runtime_error(s) {}
};

//---------------------------------------------------------------------------

// Exceptions that are programming errors

class NullPointerException : public std::invalid_argument
{
  public:
    NullPointerException(const std::string& s) : std::invalid_argument(s) {}
};

//---------------------------------------------------------------------------

class IndexOutOfBoundsException : public std::out_of_range
{
  public:
    explicit IndexOutOfBoundsException(const std::string& s) : std::out_of_range(s) {}
};

//---------------------------------------------------------------------------

class IllegalArgumentException : public std::invalid_argument
{
  public:
    explicit IllegalArgumentException(const std::string& s) : std::invalid_argument(s) {}
};

//---------------------------------------------------------------------------

class IllegalStateException : public std::domain_error
{
  public:
    explicit IllegalStateException(const std::string& s) : std::domain_error(s) {}
};

//---------------------------------------------------------------------------

class OperationNotSupportedException : public std::logic_error
{
  public:
    explicit OperationNotSupportedException(const std::string& s) : std::logic_error(s) {}
};

//---------------------------------------------------------------------------

class WrongTypeException : public std::logic_error
{
  public:
    explicit WrongTypeException(const std::string& s) : std::logic_error(s) {}
};

//---------------------------------------------------------------------------

// Application specific exceptions
// Mostly user induced
// Are normally caught and handled'

class NoSuchElementException : public std::logic_error
{
  public:
    explicit NoSuchElementException(const std::string& s) : std::logic_error(s) {}
};
 
//---------------------------------------------------------------------------

class DuplicateNameException : public std::logic_error
{
  public:
    explicit DuplicateNameException(const std::string& s) : std::logic_error(s) {}
};
 
//---------------------------------------------------------------------------

class InterruptedException : public std::logic_error
{
  public:
    explicit InterruptedException(const std::string& s) : std::logic_error(s) {}
}; 

//---------------------------------------------------------------------------

class IOException : public std::logic_error
{
  public:
    explicit IOException(const std::string& s) : std::logic_error(s) {}
}; 

//---------------------------------------------------------------------------

class IllegalFormatException : public IOException
{
  public:
    explicit IllegalFormatException(const std::string& s) : IOException(s) {}
};

//---------------------------------------------------------------------------

class NumberFormatException : public IOException
{
  public:
    explicit NumberFormatException(const std::string& s) : IOException(s) {}
};

//---------------------------------------------------------------------------

class FileFormatException : public IOException
{
  public:
    explicit FileFormatException(const std::string& s) : IOException(s) {}
};
 
//---------------------------------------------------------------------------

class StreamCorruptedException : public IOException
{
  public:
    explicit StreamCorruptedException(const std::string& s) : IOException(s) {}
};
 
//---------------------------------------------------------------------------

class StreamClosedException : public IOException
{
  public:
    explicit StreamClosedException(const std::string& s) : IOException(s) {}
};
 
//---------------------------------------------------------------------------

class StreamAbortedException : public IOException
{
  public:
    explicit StreamAbortedException(const std::string& s) : IOException(s) {}
};

//---------------------------------------------------------------------------

class FileNotFoundException : public IOException
{
  public:
    explicit FileNotFoundException(const std::string& s) : IOException(s) {}
};

//---------------------------------------------------------------------------

class AccessDeniedException : public IOException
{
  public:
    explicit AccessDeniedException(const std::string& s) : IOException(s) {}
};

} // namespace Ino

//---------------------------------------------------------------------------
#endif
