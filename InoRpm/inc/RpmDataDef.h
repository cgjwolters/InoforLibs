//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- InoRpm Persistence Data Definitions  ------------------------------
//---------------------------------------------------------------------------
//------- Copyright Inofor Hek Aut BV Oct 2009 ------------------------------
//---------------------------------------------------------------------------
//------- C. Wolters --------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef INORPM_DATADEF_INC
#define INORPM_DATADEF_INC

#include "PersistentIO.h"

namespace InoRpm
{

using namespace Ino;

//---------------------------------------------------------------------------

class RpmDataDef : public PersistentTypeDef
{
  RpmDataDef(RpmDataDef& cp);             // No Copying
  RpmDataDef& operator=(RpmDataDef& src); // No Assignment

public:
  RpmDataDef();
};

} // namespace

//---------------------------------------------------------------------------
#endif