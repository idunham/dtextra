/*
 * compressed.c
 *	Utility functions to deal with compressed/gzipped files.
 *
 * Copyright (C) 1996  Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/unmangle.c 1.30 1998/09/01 20:49:39 $")

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <mine/mgv.h>
#include <mine/unmangle.h>
#include <mine/strings.h>
#include <mine/app.h>

/*
 * the mode bits that we want on the temp directories (just a little
 * paranoia)
 */
#define	WANTMODE	(S_IRUSR | S_IWUSR | S_IXUSR)

/*
 * Where the temp files go (allocated with XtMalloc).
 */
static char *tempdir = NULL;

/*
 * atexit() function for cleaning up the temp directory
 */
static void
cleanup(void)
{
	DIR		*dir;
	struct dirent	*d;
	char		pwd[1024];

	if(tempdir == NULL
	|| getcwd(&pwd[0], sizeof(pwd) - 1) == NULL
	|| chdir(tempdir) != 0
	|| (dir = opendir(tempdir)) == NULL)
		return;

	/*
	 * I would prefer to use pointer arithmetic instead of
	 * strcmp() to check for "." and ".." but some systems
	 * (MP-RAS so far) define d_name to be an array of one char
	 * (so that struct dirent has a variable size, i.e. only one
	 * malloc is needed (false laziness!)) and that
	 * causes many compiler warnings about array bounds that
	 * I'd rather not see.
	 */
	while((d = readdir(dir)) != NULL) {
		if(strcmp(d->d_name, ".")  == 0
		|| strcmp(d->d_name, "..") == 0)
			continue;
		remove(d->d_name);
	}

	closedir(dir);
	chdir(pwd);
	rmdir(tempdir);
	XtFree(tempdir);
	tempdir = NULL;
}

/*
 * create the temp directory and ensure some sanity
 */
static int
mk_tmp(char *dir)
{
	struct stat	st;

	assert(dir     != NULL);
	assert(tempdir == NULL);

	atexit(cleanup);

	tempdir = XtMalloc(strlen(dir) + sizeof("/mgv.") + 64);
	sprintf(tempdir, "%s/mgv.%ld", dir, (long)getpid());

	if(mkdir(tempdir, 0) != 0) {
		XtFree(tempdir);
		tempdir = NULL;
		return FALSE;
	}
	memset((void *)&st, '\0', sizeof(st));
	if(stat(tempdir, &st) != 0) {
		remove(dir);
		XtFree(tempdir);
		tempdir = NULL;
		return FALSE;
	}
	if((st.st_mode & WANTMODE) != WANTMODE)
		chmod(tempdir, st.st_mode | WANTMODE);

	return TRUE;
}

/*
 * Magic numbers for various compressed files (from gzip.h in gzip 1.2.4
 * and bzip2.c from bzip2-0.1pl2).
 */
#define	FT_NORM	0
#define	FT_GZIP	1
#define	FT_BZIP	2
static struct {
	char	*ext;
	char	magic[2];
	int	type;
} magic[] = {
	{ ".z",   {'\037', '\036'}, FT_GZIP },	/* pack		*/
	{ ".gz",  {'\037', '\213'}, FT_GZIP },	/* gzip		*/
	{ ".z",   {'\037', '\236'}, FT_GZIP },	/* old gzip	*/
	{ ".Z",   {'\037', '\235'}, FT_GZIP },	/* compress	*/
	{ ".bz2", {'B',    'Z'},    FT_BZIP },	/* bzip2	*/
};

/*
 * we can deal with PDF files here or in dsc.c, since some people might
 * try and compress PDF files (not much point since PDF files are already
 * compressed) dsc.c is where it will end up
 */

static int
filetype(char *file)
{
	int	fd, i, n;
	char	buf[2];
	int	len = strlen(file);

	if((fd = open(file, O_RDONLY)) == -1)
		return FT_NORM;
	n = read(fd, (void *)&buf[0], sizeof(buf));
	close(fd);
	if(n != sizeof(buf))
		return FT_NORM;

	/*
	 * gzip.h says one byte order but the files sometimes say
	 * another so we'll just check both and hope for the best.
	 */
	for(i = 0; i < (int)(sizeof(magic)/sizeof(magic[0])); ++i) {
		int	mlen = strlen(magic[i].ext);
		if(len <= mlen
		|| strcmp(magic[i].ext, file + len - mlen) != 0)
			continue;
		if((buf[0] == magic[i].magic[0] && buf[1] == magic[i].magic[1])
		|| (buf[0] == magic[i].magic[1] && buf[1] == magic[i].magic[0]))
			return magic[i].type;
	}
	return FT_NORM;
}

static void
showmsg(MGV *m, char *file, char *tmp)
{
	FILE       *fp;
	char       *buf;
	struct stat st;

	if(tmp == NULL || *tmp == '\0')
		return;

	memset((void *)&st, '\0', sizeof(st));
	if(stat(tmp, &st) < 0)
		return;

	fp = fopen(tmp, "r");
	remove(tmp);
	if(fp == NULL
	|| (buf = XtCalloc(st.st_size + 1, 1)) == NULL)
		return;

	fread((void *)buf, 1, st.st_size, fp);
	fclose(fp);

	if(*buf != '\0')
		wl_warn(m->main, "%s %s:\n%s",
				mgv_s(m->main, MgvScantDecompress), file, buf);
	XtFree(buf);
}

/*
 * Uncompress a file and return the new file name, if the file doesn't
 * appear to be compressed or the uncompression fails, the original
 * file name will be returned.  In all cases, the return value is
 * allocated with XtNewString().
 *
 * We use "system()" in here to avoid the hassle of converting the
 * decompression commands from strings to argv's but this means that
 * some kludging is needed to grab the error output.  This would be
 * "cleaner" (in some sense) if pipe/dup2/fork/exec/XtAppAddInput
 * were used instead but sometimes laziness is good.
 */
char *
mgv_unmangle(MGV *m, char *file)
{
#	define  CMD_FMT "%s <%s >%s 2>%s"
	char    *cmd, *out, *tmp, *s;
	int     t, cmd_size;
	MGV_APP *r = mgv_appres(m->main);

	if((t = filetype(file)) == FT_NORM
	|| (tempdir == NULL && !mk_tmp(r->temp_dir)))
		return XtNewString(file);

	if((s = strrchr(file, '/')) == NULL)
		s = file;
	else
		++s;

	out = XtCalloc(strlen(tempdir) + 1 + strlen(s) + 1 + 64 + 1, 1);
	tmp = XtCalloc(strlen(tempdir) + 1 + 64 + 1 + 1, 1);

	sprintf(out, "%s/%s.%ld.%d", tempdir, s,
				(long)XtWindow(m->main), m->inst_num);
	sprintf(tmp, "%s/%ld.%d", tempdir, (long)getpid(), m->inst_num);

	cmd_size = strlen(file) + strlen(out) + strlen(tmp) + sizeof(CMD_FMT);

	switch(t) {
	case FT_GZIP:
		cmd = XtCalloc(cmd_size + strlen(r->gunzip), 1);
		sprintf(cmd, "%s <%s >%s 2>%s", r->gunzip, file, out, tmp);
		break;
	case FT_BZIP:
		cmd = XtCalloc(cmd_size + strlen(r->bunzip), 1);
		sprintf(cmd, "%s <%s >%s 2>%s", r->bunzip, file, out, tmp);
		break;
	default:
		assert("mgv_unmangle(): confused by file type!" != NULL);
		XtFree(out);
		XtFree(tmp);
		return XtNewString(file);
		break;
	}

	errno = 0;
	if(system(cmd) != 0 && errno != 0) {
		XtFree(out);
		out = XtNewString(file);
	}
	showmsg(m, file, &tmp[0]);

	XtFree(tmp);
	XtFree(cmd);

	return out;
#	undef CMD_FMT
}

/*
 * Create a new tempfile and copy src to it.  The new temp file will
 * be openned (for reading) and returned.  This implementation is pretty
 * brain-dead but it should be okay for our purposes.
 */
FILE *
mgv_tcopy(MGV *m, char **name, FILE *src)
{
#	define STDIN_FMT "%s/stdin.%ld.%d"
	FILE  *fp;
	char  *out;
	char   buf[5120];
	size_t sz;

	if(tempdir == NULL && !mk_tmp(mgv_appres(m->main)->temp_dir))
		return NULL;

	out = XtCalloc(strlen(tempdir) + sizeof(STDIN_FMT) + 64, 1);
	sprintf(out, "%s/stdin.%ld.%d", tempdir, (long)XtWindow(m->main),
								m->inst_num);

	if((fp = fopen(out, "w")) == NULL) {
		XtFree(out);
		return NULL;
	}
	while((sz = fread((void *)&buf[0], 1, sizeof(buf), src)) > 0)
		fwrite((void *)&buf[0], 1, sz, fp);
	fclose(fp);

	*name = out;
	return fopen(*name, "r");
#	undef STDIN_FMT
}
