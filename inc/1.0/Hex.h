//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------- Hex encoding/decoding ---------------------------------------------
//---------------------------------------------------------------------------
//------- Copyright Inofor Hoek Aut BV NOV 2006 -----------------------------
//---------------------------------------------------------------------------
//------- A.N. Kloosterhuis -------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef INOHEX_INC
#define INOHEX_INC

namespace Ino 
{

//---------------------------------------------------------------------------

class Hex
{
  char *buf;
  int cap;

  void ensureCap(int minCap);

  Hex(const Hex& cp);
  Hex& operator=(const Hex& src);

public:
  Hex();
  ~Hex();

  const char *encode(const char *msg, int len, int& codeLen);
  const char *decode(const char *msg, int len, int& decodeLen);
};

} // namespace

//---------------------------------------------------------------------------
#endif // HEX_INC
