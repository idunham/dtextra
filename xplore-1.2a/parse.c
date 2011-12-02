
/*  A Bison parser, made from parse.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	COMMAND	257
#define	COMMENT	258
#define	DEFAULT	259
#define	DEVICE	260
#define	DROP	261
#define	ICON	262
#define	ITEM	263
#define	LARGEICON	264
#define	MAGIC	265
#define	MOUNT	266
#define	OPTIONS	267
#define	PUSH	268
#define	SEPARATOR	269
#define	SHELF	270
#define	SMALLICON	271
#define	TYPE	272
#define	UMOUNT	273
#define	RESCAN	274
#define	SORTBYNAME	275
#define	SORTBYSIZE	276
#define	SORTBYDATE	277
#define	REV	278
#define	DIRSFIRST	279
#define	UPDIR	280
#define	HIDDEN	281
#define	FILES	282
#define	DIRS	283
#define	ACTIONS	284
#define	ILLEGAL	285
#define	STRING	286

#line 6 "parse.y"


#include "xplore.h"
#include "parse.h"

#include "dirs.h"
#include "ftype.h"
#include "menus.h"
#include "devmount.h"
#include "util.h"

#ifdef __OpenBSD__
extern int yylineno;
#endif

int errline = 0;
char errstr[1000];
Boolean tmpflag = 0;

static String mount_act, umount_act, icon_nm, large_icon_nm, small_icon_nm,
    comment_str, push_act, push_dirnm, drop_act, drop_dirnm;
static Boolean magic_fl;
static int options, setoptions, clroptions, type, base;


static Boolean source_mode = True;

static String *strtable = NULL;
static int n_str = 0;

static void inittable(void);
static String mkstr(String s);
static void popstr(int count);

static void check_icon(String name, IconPtr icon);
static void yyerror(char *s);


#line 45 "parse.y"
typedef union {
    String s;
    int i;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		129
#define	YYFLAG		-32768
#define	YYNTBASE	39

#define YYTRANSLATE(x) ((unsigned)(x) <= 286 ? yytranslate[x] : 66)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    37,    36,    38,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    35,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    33,     2,    34,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     5,     8,    15,    16,    21,    22,    27,
    30,    31,    35,    36,    40,    41,    47,    52,    57,    60,
    61,    64,    67,    70,    73,    76,    78,    82,    84,    87,
    90,    92,    94,    96,    98,   100,   102,   104,   106,   108,
   110,   112,   117,   119,   123,   125,   126,   129,   132,   135,
   138,   142,   146,   152,   154,   155,   158,   164,   166,   167,
   170,   174,   178,   180,   183,   184,   187,   190,   191,   194,
   198,   202,   208
};

static const short yyrhs[] = {    40,
     0,    63,     0,     0,    40,    41,     0,     6,    61,    62,
    33,    47,    34,     0,     0,    35,    18,    42,    53,     0,
     0,    35,    11,    43,    53,     0,    35,     5,     0,     0,
    18,    44,    52,     0,     0,    11,    45,    52,     0,     0,
     5,    46,    33,    55,    34,     0,     3,    33,    57,    34,
     0,    16,    33,    59,    34,     0,     1,    34,     0,     0,
    47,    48,     0,    12,    61,     0,    19,    61,     0,    13,
    49,     0,     8,    61,     0,    50,     0,    49,    36,    50,
     0,    51,     0,    37,    51,     0,    38,    51,     0,    20,
     0,    21,     0,    22,     0,    23,     0,    24,     0,    25,
     0,    26,     0,    27,     0,    28,     0,    29,     0,    11,
     0,    53,    33,    55,    34,     0,    54,     0,    53,    36,
    54,     0,    61,     0,     0,    55,    56,     0,    10,    61,
     0,    17,    61,     0,     4,    61,     0,    14,    61,    62,
     0,     7,    61,    62,     0,     9,    61,    36,    61,    62,
     0,    15,     0,     0,    57,    58,     0,     9,    61,    36,
    61,    62,     0,    15,     0,     0,    59,    60,     0,     9,
    61,    62,     0,     5,    61,    62,     0,    32,     0,    61,
    32,     0,     0,    36,    61,     0,    30,    64,     0,     0,
    64,    65,     0,    14,    61,    62,     0,     7,    61,    62,
     0,     9,    61,    36,    61,    62,     0,    15,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    61,    63,    67,    69,    73,    93,    99,   101,   107,   109,
   113,   126,   139,   145,   155,   159,   172,   174,   176,   182,
   189,   193,   197,   201,   203,   209,   211,   215,   219,   223,
   229,   233,   237,   241,   245,   249,   253,   257,   261,   265,
   269,   275,   290,   292,   296,   307,   312,   317,   321,   325,
   329,   333,   337,   341,   347,   351,   355,   359,   365,   369,
   373,   380,   386,   388,   405,   409,   418,   427,   432,   436,
   440,   444,   448
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","COMMAND",
"COMMENT","DEFAULT","DEVICE","DROP","ICON","ITEM","LARGEICON","MAGIC","MOUNT",
"OPTIONS","PUSH","SEPARATOR","SHELF","SMALLICON","TYPE","UMOUNT","RESCAN","SORTBYNAME",
"SORTBYSIZE","SORTBYDATE","REV","DIRSFIRST","UPDIR","HIDDEN","FILES","DIRS",
"ACTIONS","ILLEGAL","STRING","'{'","'}'","'@'","','","'+'","'-'","source","config",
"item","@1","@2","@3","@4","@5","device_fields","device_field","option_list",
"option","option_name","type","pattern_list","pattern","type_fields","type_field",
"cmd_fields","cmd_field","shelf_fields","shelf_field","string","opt_string",
"action_list","action_fields","action_field", NULL
};
#endif

static const short yyr1[] = {     0,
    39,    39,    40,    40,    41,    42,    41,    43,    41,    41,
    44,    41,    45,    41,    46,    41,    41,    41,    41,    47,
    47,    48,    48,    48,    48,    49,    49,    50,    50,    50,
    51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
    51,    52,    53,    53,    54,    55,    55,    56,    56,    56,
    56,    56,    56,    56,    57,    57,    58,    58,    59,    59,
    60,    60,    61,    61,    62,    62,    63,    64,    64,    65,
    65,    65,    65
};

static const short yyr2[] = {     0,
     1,     1,     0,     2,     6,     0,     4,     0,     4,     2,
     0,     3,     0,     3,     0,     5,     4,     4,     2,     0,
     2,     2,     2,     2,     2,     1,     3,     1,     2,     2,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     4,     1,     3,     1,     0,     2,     2,     2,     2,
     3,     3,     5,     1,     0,     2,     5,     1,     0,     2,
     3,     3,     1,     2,     0,     2,     2,     0,     2,     3,
     3,     5,     1
};

static const short yydefact[] = {     3,
    68,     0,     2,    67,     0,     0,    15,     0,    13,     0,
    11,     0,     4,     0,     0,     0,    73,    69,    19,    55,
     0,    63,    65,     0,    59,     0,    10,     8,     6,    65,
     0,    65,     0,    46,    64,     0,     0,    14,     0,    43,
    45,     0,    12,     0,     0,    71,     0,    70,     0,    58,
    17,    56,     0,    66,    20,    46,     0,     0,     0,    18,
    60,     9,     7,    65,     0,     0,     0,     0,     0,     0,
    54,     0,    16,    47,     0,     0,    44,    65,    65,    72,
     0,    50,    65,     0,    48,    65,    49,     0,     0,     0,
     0,     5,    21,    42,    62,    61,    65,    52,     0,    51,
    25,    22,    41,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,     0,     0,    24,    26,    28,    23,    57,
    65,    29,    30,     0,    53,    27,     0,     0,     0
};

static const short yydefgoto[] = {   127,
     2,    13,    45,    44,    26,    24,    21,    75,    93,   116,
   117,   118,    38,    39,    40,    53,    74,    33,    52,    42,
    61,    41,    37,     3,     4,    18
};

static const short yypact[] = {   -26,
-32768,    11,-32768,    61,   -10,     2,-32768,     1,-32768,    12,
-32768,    26,-32768,     1,     1,     1,-32768,-32768,-32768,-32768,
    15,-32768,   -13,     1,-32768,     1,-32768,-32768,-32768,   -13,
     4,   -13,     0,-32768,-32768,     1,    23,-32768,   -23,-32768,
    25,    -4,-32768,     1,     1,-32768,     1,-32768,     1,-32768,
-32768,-32768,    80,    25,-32768,-32768,     1,     1,     1,-32768,
-32768,     7,     7,   -13,     6,     1,     1,     1,     1,     1,
-32768,     1,-32768,-32768,    13,    89,-32768,   -13,   -13,-32768,
     1,    25,   -13,    17,    25,   -13,    25,     1,     1,   104,
     1,-32768,-32768,-32768,-32768,-32768,   -13,-32768,     1,-32768,
    25,    25,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   123,   123,    27,-32768,-32768,    25,-32768,
   -13,-32768,-32768,   104,-32768,-32768,    65,    69,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   -52,  -112,    51,    10,    21,    30,-32768,-32768,-32768,-32768,
-32768,    -8,   -12,-32768,-32768,-32768
};


#define	YYLAST		152


static const short yytable[] = {    23,
    58,   122,   123,     1,    59,    30,    31,    32,    49,    56,
    -1,     5,    57,     6,    50,     7,     8,    46,    35,    48,
    88,     9,    36,    19,    89,    90,    10,    54,    11,    60,
    27,    91,    22,    51,    20,    35,    28,    35,    64,    47,
    65,    81,    57,    29,    25,    12,    92,    34,    35,    78,
    79,    80,    99,    62,    63,    55,    35,    82,    83,    84,
    85,    86,   124,    87,   128,    95,    96,    14,   129,    15,
    98,   126,    97,   100,    16,    17,    43,    77,     0,   101,
   102,     0,   119,    66,   120,    76,    67,     0,    68,    69,
   121,     0,    66,    70,    71,    67,    72,    68,    69,     0,
     0,     0,    70,    71,     0,    72,     0,     0,   125,     0,
     0,     0,     0,    73,   103,     0,     0,     0,     0,     0,
     0,     0,    94,   104,   105,   106,   107,   108,   109,   110,
   111,   112,   113,   103,     0,     0,     0,     0,     0,     0,
   114,   115,   104,   105,   106,   107,   108,   109,   110,   111,
   112,   113
};

static const short yycheck[] = {     8,
     5,   114,   115,    30,     9,    14,    15,    16,     9,    33,
     0,     1,    36,     3,    15,     5,     6,    30,    32,    32,
     8,    11,    36,    34,    12,    13,    16,    36,    18,    34,
     5,    19,    32,    34,    33,    32,    11,    32,    47,    36,
    49,    36,    36,    18,    33,    35,    34,    33,    32,    58,
    59,    64,    36,    44,    45,    33,    32,    66,    67,    68,
    69,    70,    36,    72,     0,    78,    79,     7,     0,     9,
    83,   124,    81,    86,    14,    15,    26,    57,    -1,    88,
    89,    -1,    91,     4,    97,    56,     7,    -1,     9,    10,
    99,    -1,     4,    14,    15,     7,    17,     9,    10,    -1,
    -1,    -1,    14,    15,    -1,    17,    -1,    -1,   121,    -1,
    -1,    -1,    -1,    34,    11,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    34,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    11,    -1,    -1,    -1,    -1,    -1,    -1,
    37,    38,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYPARSE_RETURN_TYPE
#define YYPARSE_RETURN_TYPE int
#endif


#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 222 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
YYPARSE_RETURN_TYPE
yyparse (void *);
#else
YYPARSE_RETURN_TYPE
yyparse (void);
#endif
#endif

YYPARSE_RETURN_TYPE
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
#ifndef YYSTACK_USE_ALLOCA
  int yyfree_stacks = 0;
#endif

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
#ifndef YYSTACK_USE_ALLOCA
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
#endif	    
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 5:
#line 75 "parse.y"
{
  int d;
  if (exists(yyvsp[-4].s)) {
    if (options)
      options = (options & ~NOMAGIC) | (~options & NOMAGIC);
    else
      options = NONE;
    setoptions = (setoptions & ~NOMAGIC) | (~setoptions & NOMAGIC);
    clroptions = (clroptions & ~NOMAGIC) | (~clroptions & NOMAGIC);
    d = newdev(yyvsp[-4].s, yyvsp[-3].s, icon_nm, mount_act, umount_act, options, setoptions,
	       clroptions);
    check_icon(icon_nm, devicon(d));
    inittable();
  }
;
    break;}
case 6:
#line 95 "parse.y"
{ magic_fl = False; type = add_file_type(tmpflag); ;
    break;}
case 7:
#line 99 "parse.y"
{ base = search_file_type(); pop_file_type(); ;
    break;}
case 8:
#line 103 "parse.y"
{ magic_fl = True; type = add_file_type(tmpflag); ;
    break;}
case 9:
#line 107 "parse.y"
{ base = search_file_type(); pop_file_type(); ;
    break;}
case 10:
#line 111 "parse.y"
{ base = n_types(); ;
    break;}
case 11:
#line 115 "parse.y"
{ magic_fl = False; type = add_file_type(tmpflag); ;
    break;}
case 12:
#line 126 "parse.y"
{
  if (tmpflag)
    if (base == ANY)
      /* "legacy" entry from pre-1.0d; takes highest priority */
      move_file_type(1);
    else if (base == NONE)
      /* orphans are silently removed */
      pop_file_type();
    else
      /* other entries are inserted before their respective base type */
      move_file_type(base);
;
    break;}
case 13:
#line 141 "parse.y"
{ magic_fl = True; type = add_file_type(tmpflag); ;
    break;}
case 14:
#line 145 "parse.y"
{
  if (tmpflag)
    if (base == ANY)
      move_file_type(1);
    else if (base == NONE)
      pop_file_type();
    else
      move_file_type(base);
;
    break;}
case 15:
#line 157 "parse.y"
{ magic_fl = False; type = init_file_type(DFLT); ;
    break;}
case 16:
#line 161 "parse.y"
{
    set_comment(type, comment_str);
    set_icons(type, large_icon_nm, small_icon_nm);
    check_icon(large_icon_nm, large_icon(type));
    check_icon(small_icon_nm, small_icon(type));
    set_actions(type,
		push_act, push_dirnm,
		drop_act, drop_dirnm);
    inittable();
;
    break;}
case 19:
#line 178 "parse.y"
{ yyerrok; inittable(); ;
    break;}
case 20:
#line 184 "parse.y"
{
  mount_act = umount_act = NULL; options = setoptions = clroptions = 0;
  icon_nm = NULL;
;
    break;}
case 22:
#line 195 "parse.y"
{ mount_act = yyvsp[0].s; ;
    break;}
case 23:
#line 199 "parse.y"
{ umount_act = yyvsp[0].s; ;
    break;}
case 25:
#line 205 "parse.y"
{ icon_nm = yyvsp[0].s; ;
    break;}
case 28:
#line 217 "parse.y"
{ options |= yyvsp[0].i; ;
    break;}
case 29:
#line 221 "parse.y"
{ setoptions |= yyvsp[0].i; clroptions &= ~yyvsp[0].i; ;
    break;}
case 30:
#line 225 "parse.y"
{ clroptions |= yyvsp[0].i; setoptions &= ~yyvsp[0].i; ;
    break;}
case 31:
#line 231 "parse.y"
{ yyval.i = CHECK_FILES; ;
    break;}
case 32:
#line 235 "parse.y"
{ yyval.i = SORT_BY_NAME; ;
    break;}
case 33:
#line 239 "parse.y"
{ yyval.i = SORT_BY_SIZE; ;
    break;}
case 34:
#line 243 "parse.y"
{ yyval.i = SORT_BY_DATE; ;
    break;}
case 35:
#line 247 "parse.y"
{ yyval.i = REVERSE; ;
    break;}
case 36:
#line 251 "parse.y"
{ yyval.i = DIRS_FIRST; ;
    break;}
case 37:
#line 255 "parse.y"
{ yyval.i = INCLUDE_UPDIR; ;
    break;}
case 38:
#line 259 "parse.y"
{ yyval.i = INCLUDE_HIDDEN; ;
    break;}
case 39:
#line 263 "parse.y"
{ yyval.i = INCLUDE_FILES; ;
    break;}
case 40:
#line 267 "parse.y"
{ yyval.i = INCLUDE_DIRS; ;
    break;}
case 41:
#line 271 "parse.y"
{ yyval.i = NOMAGIC; ;
    break;}
case 42:
#line 277 "parse.y"
{
    set_comment(type, comment_str);
    set_icons(type, large_icon_nm, small_icon_nm);
    check_icon(large_icon_nm, large_icon(type));
    check_icon(small_icon_nm, small_icon(type));
    set_actions(type,
		push_act, push_dirnm,
		drop_act, drop_dirnm);
    inittable();
;
    break;}
case 45:
#line 298 "parse.y"
{
    String p = yyvsp[0].s;
    String m = split_type(&p);

    add_pattern(type, magic_fl, m, p);
;
    break;}
case 46:
#line 309 "parse.y"
{ large_icon_nm = small_icon_nm = comment_str =
      push_act = push_dirnm = drop_act = drop_dirnm = NULL; ;
    break;}
case 48:
#line 319 "parse.y"
{ large_icon_nm = yyvsp[0].s; ;
    break;}
case 49:
#line 323 "parse.y"
{ small_icon_nm = yyvsp[0].s; ;
    break;}
case 50:
#line 327 "parse.y"
{ comment_str = yyvsp[0].s; ;
    break;}
case 51:
#line 331 "parse.y"
{ push_act = yyvsp[-1].s; push_dirnm = yyvsp[0].s; ;
    break;}
case 52:
#line 335 "parse.y"
{ drop_act = yyvsp[-1].s; drop_dirnm = yyvsp[0].s; ;
    break;}
case 53:
#line 339 "parse.y"
{ add_menu_action(type, yyvsp[-3].s, yyvsp[-1].s, yyvsp[0].s); ;
    break;}
case 54:
#line 343 "parse.y"
{ add_menu_action(type, NULL, NULL, NULL); ;
    break;}
case 55:
#line 349 "parse.y"
{ init_cmd_menu(); ;
    break;}
case 57:
#line 357 "parse.y"
{ add_cmd_action(yyvsp[-3].s, yyvsp[-1].s, yyvsp[0].s); ;
    break;}
case 58:
#line 361 "parse.y"
{ add_cmd_action(NULL, NULL, NULL); ;
    break;}
case 59:
#line 367 "parse.y"
{ init_shelf_menu(); ;
    break;}
case 61:
#line 375 "parse.y"
{ if (yyvsp[-1].s && default_shelf == NONE)
    default_shelf = add_shelf(yyvsp[-1].s, yyvsp[0].s);
  else
    add_shelf(yyvsp[-1].s, yyvsp[0].s); ;
    break;}
case 62:
#line 382 "parse.y"
{ default_shelf = add_shelf(yyvsp[-1].s, yyvsp[0].s); ;
    break;}
case 64:
#line 390 "parse.y"
{
  if (!yyvsp[-1].s)
    yyval.s = yyvsp[0].s;
  else if (!yyvsp[0].s)
    yyval.s = yyvsp[-1].s;
  else {
    String s = alloca(strlen(yyvsp[-1].s)+strlen(yyvsp[0].s)+1);
    strcat(strcpy(s, yyvsp[-1].s), yyvsp[0].s);
    popstr(2);
    yyval.s = mkstr(s);
  }
;
    break;}
case 65:
#line 407 "parse.y"
{ yyval.s = NULL; ;
    break;}
case 66:
#line 411 "parse.y"
{ yyval.s = yyvsp[0].s; ;
    break;}
case 67:
#line 420 "parse.y"
{
    set_actions(type,
		push_act, push_dirnm,
		drop_act, drop_dirnm);
    inittable();
;
    break;}
case 68:
#line 429 "parse.y"
{ large_icon_nm = small_icon_nm = comment_str =
      push_act = push_dirnm = drop_act = drop_dirnm = NULL; ;
    break;}
case 70:
#line 438 "parse.y"
{ push_act = yyvsp[-1].s; push_dirnm = yyvsp[0].s; ;
    break;}
case 71:
#line 442 "parse.y"
{ drop_act = yyvsp[-1].s; drop_dirnm = yyvsp[0].s; ;
    break;}
case 72:
#line 446 "parse.y"
{ add_menu_action(type, yyvsp[-3].s, yyvsp[-1].s, yyvsp[0].s); ;
    break;}
case 73:
#line 450 "parse.y"
{ add_menu_action(type, NULL, NULL, NULL); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 554 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
#ifndef YYSTACK_USE_ALLOCA
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
#endif
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
#ifndef YYSTACK_USE_ALLOCA
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
#endif    
  return 1;
}
#line 454 "parse.y"


static char fname[MAXPATHLEN+1];

#include "lex.c"

static void check_icon(String name, IconPtr icon)
{
    if (name && *name && !icon) {
	char msg[MAXPATHLEN+20];

	sprintf(msg, "error reading icon %s", name);
	error(msg);
    }
}

static void yyerror(String s)
{
  if (source_mode)
    fprintf(stderr, "%s: %d: %s at or near '%s'\n", fname, yylineno, s,
	    yytext);
  else {
    errline = yylineno;
    sprintf(errstr, "%d: %s at or near '%s'", yylineno, s,
	    yytext);
  }
}

static void inittable(void)
{
    int i;

    for (i = 0; i < n_str; i++)
	FREE(strtable[i]);
    FREE(strtable); strtable = NULL; n_str = 0;
}

static String mkstr(String s)
{
    strtable = (String*) REALLOC(strtable, (n_str+1)*sizeof(String));
    strtable[n_str] = NEWSTRING(s);
    return strtable[n_str++];
}

static void popstr(int count)
{
    for (; n_str > 0 && count > 0; count--)
	FREE(strtable[--n_str]);
}

void parse_config_file(String name, String cpp_command, String cpp_options)
{
  source_mode = True;
  strcpy(fname, name);
  if (exists(name)) {
    /* we use a named pipe here; hopefully this fixes some probs with popen()
       and flex-generated scanners */
    String pipe, cpp_line;
    
    pipe = tmpnam(NULL);
    mkfifo(pipe, 0600);
    cpp_line = alloca(strlen(cpp_command) +
		      strlen(cpp_options) +
		      strlen(name) +
		      strlen(pipe) + 10);
    sprintf(cpp_line, "%s %s %s > %s &", cpp_command, cpp_options, name, pipe);
    system(cpp_line);
    if ((yyin = fopen(pipe, "r"))) {
#ifdef FLEX_SCANNER
      yyrestart(yyin);
#endif
      yylineno = 1;
      BEGIN(SRC);
      tmpflag = False; base = NONE;
      yyparse();
      inittable();
      FREE(str);
      str = NULL;
      remove(pipe);
    } else
      error("error parsing config file: cannot exec cpp");
  }
}

void parse_config_string(String source)
{
  FILE *tmpfile;
  if (!source) return;
  source_mode = True;
  tmpnam(fname);
  if (!(tmpfile = fopen(fname, "w"))) {
    error("error writing temp file");
    return;
  }
  fputs(source, tmpfile);
  if (fclose(tmpfile)) {
    error("error writing temp file");
    unlink(fname);
    return;
  }
  if ((yyin = fopen(fname, "r"))) {
#ifdef FLEX_SCANNER
    yyrestart(yyin);
#endif
    yylineno = 1;
    BEGIN(SRC);
    tmpflag = True; base = ANY;
    yyparse();
    inittable();
    FREE(str);
    str = NULL;
    unlink(fname);
    return;
  } else {
    error("error reading temp file");
    unlink(fname);
    return;
  }
}

Boolean parse_actions(int t, String actions)
{
  FILE *tmpfile;
  if (!actions) return;
  source_mode = False;
  errline = 0;
  *errstr = '\0';
  tmpnam(fname);
  if (!(tmpfile = fopen(fname, "w"))) {
    errline = 0;
    strcpy(errstr, "error writing temp file");
    return False;
  }
  fputs("Actions\t", tmpfile);
  fputs(actions, tmpfile);
  if (fclose(tmpfile)) {
    errline = 0;
    strcpy(errstr, "error writing temp file");
    unlink(fname);
    return False;
  }
  if ((yyin = fopen(fname, "r"))) {
    type = t;
#ifdef FLEX_SCANNER
    yyrestart(yyin);
#endif
    yylineno = 1;
    BEGIN(SRC);
    tmpflag = True; base = NONE;
    yyparse();
    inittable();
    FREE(str);
    str = NULL;
    unlink(fname);
    return (*errstr) == '\0';
  } else {
    errline = 0;
    strcpy(errstr, "error reading temp file");
    unlink(fname);
    return False;
  }
}
