#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#ifndef XPLORELIBDIR
#define XPLORELIBDIR "/usr/lib/X11/xplore"
#endif

#ifndef XPLORE_MAGIC
#define XPLORE_MAGIC "magic"
#endif

extern void magic_parse_file();
extern int magic_get_type();

char basedir[MAXPATHLEN+1];
char libdir[MAXPATHLEN+1];
char libmagic[MAXPATHLEN+1];

char buf[1024];

void usage(prog)
char *prog;
{
    fprintf(stderr, "Usage: %s [-m magic_file ] [-f] file ...\n", prog);
}

int main(argc, argv)
int argc;
char **argv;
{
    char *s;
    int i;

    getcwd(basedir, MAXPATHLEN);
    if ((s = getenv("XPLORELIBDIR")))
      abspath(libdir, basedir, s);
    else
      strcpy(libdir, XPLORELIBDIR);
    sprintf(libmagic, "%s/%s", libdir, XPLORE_MAGIC);

    magic_parse_file(libmagic);

    for(i = 1; i < argc; i++)
    {
	if(argv[i][0] == '-')
	{
	    switch (argv[i][1])
	    {
	    case 'm':
		i++;
		if(i < argc)
		    magic_parse_file(argv[i]);
		else
		{
		    fprintf(stderr, "Missing argument: magic file\n");
		    usage(argv[0]);
		    exit(1);
		}
		break;
	    case 'f':
		i++;
		if(i < argc)
		{
		    magic_get_type(argv[i], buf);
		    printf("%s: %s\n", argv[i], buf);
		}
		else
		{
		    fprintf(stderr, "Missing argument: file\n");
		    usage(argv[0]);
		    exit(1);
		}
		break;
	    default:
		fprintf(stderr, "Bad option %s\n", argv[i]);
		usage(argv[0]);
		exit(1);
	    }
	    continue;
	}
	magic_get_type(argv[i], buf);
	printf("%s: %s\n", argv[i], buf);
    }
}
