
%{

/**************************************************************************/
/* lex.l: lexical analyzer for xplore config file                         */
/**************************************************************************/

static String str;
static int strleng;

#ifdef __OpenBSD__
int yylineno;
#endif

int yywrap(void)
{
    fclose(yyin);
    yyin = NULL;
    return 1;
}

%}

SPACE			[ \t\n]+

%s SRC STR CPP1 CPP2

%%

<SRC>Actions		if (source_mode) return ILLEGAL; else return ACTIONS;
<SRC>Command		return COMMAND;
<SRC>Comment		return COMMENT;
<SRC>Default		return DEFAULT;
<SRC>Description	return COMMENT;
<SRC>Device		return DEVICE;
<SRC>Drop		return DROP;
<SRC>Icon		return ICON;
<SRC>Item		return ITEM;
<SRC>LargeIcon		return LARGEICON;
<SRC>Magic		return MAGIC;
<SRC>Mount		return MOUNT;
<SRC>Options		return OPTIONS;
<SRC>Push		return PUSH;
<SRC>Separator		return SEPARATOR;
<SRC>Shelf		return SHELF;
<SRC>SmallIcon		return SMALLICON;
<SRC>Type		return TYPE;
<SRC>Umount		return UMOUNT;

<SRC>Rescan		return RESCAN;
<SRC>SortByName		return SORTBYNAME;
<SRC>SortBySize		return SORTBYSIZE;
<SRC>SortByDate		return SORTBYDATE;
<SRC>Reverse		return REV;
<SRC>DirsFirst		return DIRSFIRST;
<SRC>Updir		return UPDIR;
<SRC>Hidden		return HIDDEN;
<SRC>Files		return FILES;
<SRC>Dirs		return DIRS;

<SRC>[A-Za-z]+		return ILLEGAL;

<SRC>{SPACE}		;

<SRC>^#{SPACE}		BEGIN(CPP1);
<CPP1>[0-9]+		{
			register int c;
			yylineno = atoi(yytext)-1;
			while ((c = input()) != '"' && c != '\n' && c != EOF)
			    ;
			BEGIN(CPP2);
			}
<CPP2>([^"\n]|\\\")*	{
			register int c;
			unquote(fname, yytext);
			while ((c = input()) != '\n' && c != EOF)
			    ;
			BEGIN(SRC);
			}


<SRC>\"			BEGIN(STR);
<STR>\\\n		;
<STR>["\n]		{
			BEGIN(SRC);
			if (str) {
			    String tok = alloca(strleng+1);
			    unquote(tok, str);
			    yylval.s = mkstr(tok);
			    FREE(str); str = NULL; strleng = 0;
			} else
			    yylval.s = NULL;
			if (*yytext == '\n')
			    yyerror("unterminated string constant");
			return STRING;
			}
<STR>([^"\n]|\\\")+	{
			str = REALLOC(str, strleng+yyleng+1);
			strcpy(str+strleng, yytext);
			strleng += yyleng;
			}

<SRC>@			if (!tmpflag) return ILLEGAL; else return '@';
<SRC>.			return *yytext;
