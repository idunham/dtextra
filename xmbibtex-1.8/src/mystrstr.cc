/* mystrstr.cc                                                  */
/* returns a pointer to the first occurance of s1 in s2         */
/* returns NULL if s1 is not included in s2                     */
/* if the switch c is 0 then the comparison is case sensitive   */
/* if the switch c is 1 then the comparison is case insensitive */


#include <string.h>
#include <ctype.h>

char *mystrstr(const char *s1, const char *s2, short c) {

  char *hs1, *hs2;
  char *rc;
  int  i;

  hs1=new char[strlen(s1)+1];
  hs2=new char[strlen(s2)+1];

  strcpy(hs1,s1);
  strcpy(hs2,s2);

  if (c) {
    for (i=0; hs1[i]; i++) hs1[i]=toupper(hs1[i]);
    for (i=0; hs2[i]; i++) hs2[i]=toupper(hs2[i]);
  }

  rc=strstr(hs1,hs2);

  delete [] hs1; delete [] hs2;

  return rc;

}
