#include <stdio.h>
#include "regexp.h"

void
regerror(s)
char *s;
{
	fprintf(stderr, "regexp(3): %s", s);
}
