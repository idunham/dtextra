/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */

#ifndef _COMMON_REGEXPR_H
#define _COMMON_REGEXPR_H

#define NSUBEXP  10

typedef struct _regexpr {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
} regexpr;

extern regexpr *reg_compile(char *);
extern int reg_execute(regexpr *, char *);
extern void reg_substitute(regexpr *, char *, char *);
extern void reg_error(char *);

#endif
