/*
** from mrolo
*/


#include "xhead.h"
#include <ctype.h>
#include "xmastm.h"

/* strcmp, case insensitive */
int mystrcasecmp(a,b)
     char *a, *b;
{
  int rval;
  for ( rval = 0; *a != '\0' && *b != '\0' && rval == 0; a++, b++ ) {
    rval = *a - *b;
    if ( rval != 0 )
      if ( isupper(*a) && islower(*b) )
        rval = *a - toupper(*b);
      else if ( islower(*a) && isupper(*b) )
        rval = toupper(*a) - *b;
  }
  return rval;
}

int mystrncasecmp(a,b,n)
     char *a, *b;
     int n;
{
  int rval, i;

  for ( i = 0, rval = 0; i < n && *a != '\0' && *b != '\0' && rval == 0; a++, b++, i++ ) {
    rval = *a - *b;
    if ( rval != 0 )
      if ( isupper(*a) && islower(*b) )
        rval = *a - toupper(*b);
      else if ( islower(*a) && isupper(*b) )
        rval = toupper(*a) - *b;
  }
  if ( i < n && *a == '\0' ) rval = -1;
  return rval;
}

char *mystrcpy(t,s)
	char *t, *s;
{
  char *a;

  a = t;
  for ( ; *s != '\0'; s++, t++ ) *t = *s;
  *t = '\0';
  return a;
}

char *mystrdup(s)
  char *s;
{
  char *t;

  if ( s == (char*)0 ) return (char*)0;
  t = (char*)malloc(strlen(s)+1);
  if ( t == (char*)0 ) return (char*)0;
  mystrcpy(t,s);
  return t;
}

char *mystrstrcase(t,s)
     char *t, *s;
{
  int n;
  char s1, s2;

  n = strlen(s);
  s1 = s[0];
  if ( isupper(s1) ) s2 = tolower(s1);
  else if ( islower(s1) ) s2 = toupper(s1);
  else s2 = s1;
  for (; *t != '\0'; t++ ) {
    if ( *t == s1 || *t == s2 )
      if ( mystrncasecmp(t,s,n) == 0 ) {
        return t;
      }
  }
  return (char*)0;
}

