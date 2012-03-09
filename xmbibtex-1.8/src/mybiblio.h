#include "biblio.h"

class MYBIBLIO: public BIBLIO {
public:
  CITATION *d;
  MYBIBLIO();
  ~MYBIBLIO();
  void ClearDummy();
  void ToDummy(unsigned long);
  void FromDummy(unsigned long);
  short DocTypeN(unsigned long index);
};



