
#include "xplore.h"
#include "util.h"

int mysetenv(String name, String val, int overw /* ignored */)
{
  char *envstr = malloc(strlen(name)+strlen(val)+2);
  if (envstr) {
    sprintf(envstr, "%s=%s", name, val);
    return putenv(envstr);
  } else
    return -1;
}

String split(String s, char c)
{
    static String t;
    if (!s)
	s = t;
    t = s;
    if (t && (t = strchr(t, c)))
	*t++ = '\0';
    return s;
}

String split2(String s, char c)
{
    static String t;
    if (!s)
	s = t;
    t = s;
    if (t)
	while ((t = strchr(t, c)) && t>s && t[-1]=='\\') t++;
    if (t)
	*t++ = '\0';
    return s;
}

String split_type(String *s)
{
    String t = *s;
    if (t)
	if (*t++ == '<' && split2(t, '>')) {
	    *s = t+strlen(t)+1;
	    if (!**s) *s = NULL;
	    return t;
	} else
	    return NULL;
    else
	return NULL;
}

String quote(String s, String t, String c)
{
    String s0 = s;
    for (; *t; t++) {
	if (strchr(c, *t))
	    *s++ = '\\';
	*s++ = *t;
    }
    *s = '\0';
    return s0;
}

String unquote(String s, String t)
{
    String s0 = s;
    for (; *t; t++)
	if (*t != '\\')
	    *s++ = *t;
	else if (*++t)
	    switch (*t) {
	    case 'n':
		*s++ = '\n';
		break;
	    case 't':
		*s++ = '\t';
		break;
	    case 'r':
		*s++ = '\r';
		break;
	    case 'b':
		*s++ = '\b';
		break;
	    case 'f':
		*s++ = '\f';
		break;
	    default:
		*s++ = *t;
	    }
	else
	    break;
    *s = '\0';
    return s0;
}
      
Boolean prefix(String s, String t)
{
    String p, q;
    for (p = s, q = t; *p && *q && *p == *q; p++, q++)
	;
    return !*p;
}

Boolean suffix(String s, String t)
{
    String p, q;
    for (p = s+strlen(s), q = t+strlen(t); p > s && q > t && *p == *q;
					   p--, q--)
	;
    return p == s && *p == *q;
}

String fnexpand(String result, String name)
{
    static String home = NULL;

    if (!home && !(home = getenv("HOME")))
	return strcpy(result, name);
    else if (!name[0] || name[0] != '~' ||
	     name[1] != '\0' && name[1] != '/')
	return strcpy(result, name);
    else
	return strcat(strcpy(result, home), name+1);
}

String pathname(String result, String dirname, String filename)
{
    int l = strlen(dirname);
    strcpy(result, dirname);
    if (l && result[l-1] != '/')
	result[l++] = '/';
    strcpy(result+l, filename);
    return result;
}

String abspath(String result, String basename, String filename)
{
    char s[MAXPATHLEN+1], t[MAXPATHLEN+1];

    fnexpand(s, filename);
    if (*s != '/')
	return shortestpath(result, pathname(t, basename, s));
    else
	return shortestpath(result, s);
}

static String *splitpath(String pathname, int *n_parts)
{
    String *parts=NULL, part, path = alloca((strlen(pathname)+1)*sizeof(char));
    strcpy(path, pathname);
    if (*path == '/') path++;
    *n_parts = 0;
    if (*path && (part = split(path, '/')))
	do {
	    parts = (String*) realloc(parts, (*n_parts+1)*sizeof(String));
	    parts[(*n_parts)++] = part?strdup(part):NULL;
        } while ((part = split(NULL, '/')));
    return parts;
}

String relpath(String result, String basename, String pathname)
{
    if (*basename != '/' || *pathname != '/')
	return strcpy(result, pathname);
    else {
	int i, n, n_baseparts, n_pathparts;
	String *baseparts, *pathparts;
	char base[MAXPATHLEN+1], path[MAXPATHLEN+1];
	shortestpath(base, basename);
	shortestpath(path, pathname);
	baseparts = splitpath(base, &n_baseparts);
	pathparts = splitpath(path, &n_pathparts);
	for (i = 0; i < n_baseparts && i < n_pathparts; i++)
	    if (strcmp(baseparts[i], pathparts[i]))
		break;
	n = i; /* n = length of maximum common prefix */
	*result = '\0';
	for (i = 0; i < n_baseparts-n; i++)
	    strcat(result, "../");
	for (i = n; i < n_pathparts; i++)
	    strcat(strcat(result, pathparts[i]), "/");
	if (*result)
	    result[strlen(result)-1] = '\0';
	for (i = 0; i < n_baseparts; i++)
	    free(baseparts[i]);
	for (i = 0; i < n_pathparts; i++)
	    free(pathparts[i]);
	free(baseparts); free(pathparts);
	if (!*result)
	    /* basename and pathname are identical */
	    strcpy(result, ".");
	return result;
    }
}

String shortestpath(String result, String pathname)
{
    String cur, last, part, path = alloca((strlen(pathname)+1)*sizeof(char));
    strcpy(path, pathname);
    cur = result;
    *cur = '\0';
    if (*path == '/')
	strcpy(cur++, "/"), path++;
    if ((part = split(path, '/')))
	do {
	    if (!strcmp(part, "."))
		;
	    else if (!strcmp(part, "..") && (last = strrchr(result, '/'))) {
		if (last > result)
		    cur = last;
		else
		    cur = last+1;
		*cur = '\0';
	    } else {
		if (cur > result && cur[-1] != '/')
		    *cur++ = '/';
		strcpy(cur, part);
		cur += strlen(part);
	    }
	} while ((part = split(NULL, '/')));
    return result;
}

String resolve(String result, String pathname)
{
  char buf[MAXPATHLEN+1], path[MAXPATHLEN+1], dirname[MAXPATHLEN+1];
  int len = readlink(pathname, buf, MAXPATHLEN);

  if (len >= 0) {
    buf[len] = '\0';
    abspath(path, dirpart(dirname, pathname), buf);
  } else
    strcpy(path, pathname);
  return shortestpath(result, path);
}

String dirpart(String result, String pathname)
{
    String p;
    strcpy(result, pathname);
    if ((p = strrchr(result, '/'))) {
	if (p == result) p++;
	*p = '\0';
    } else
	*result = '\0';
    return result;
}

String filepart(String result, String pathname)
{
    String p;
    strcpy(result, pathname);
    if ((p = strrchr(pathname, '/')))
	return strcpy(result, p+1);
    else
	return strcpy(result, pathname);
}

String bakname(String result, String pathname)
{
  int n = 0;
  String p = result+strlen(pathname);

  strcpy(result, pathname);
  do {
    sprintf(p, ".~%d~", ++n);
  } while (exists(result));
  return result;
}

Boolean exists(char *name)
{
  struct stat stats;

  return (lstat(name, &stats) == 0);
}

Boolean identical(char *name1, char *name2)
{
  struct stat stats1, stats2;

  return !lstat(name1, &stats1) && !lstat(name2, &stats2) &&
    stats1.st_ino == stats2.st_ino;
}

String searchpath(String result, String path, String name)
{
    char fullname[MAXPATHLEN+1];
    String s, t;

    if (*name == '/')
	return strcpy(result, name);
    for (s = path; *s; s = t) {
	int l;
	if (!(t = strchr(s, ':')))
	    t = strchr(s, 0);
	if (s == t) goto next;
	if (s[0] == '.')
	    if ((t = s+1))
		s = t;
	    else if (s[1] == '/')
		s += 2;
	l = t-s;
	strncpy(fullname, s, l);
	if (l > 0 && fullname[l - 1] != '/')
	    fullname[l] = '/', l++;
	strcpy(fullname+l, name);
	abspath(result, basedir, fullname);
	if (exists(result))
	    return result;
      next:
	if (*t) t++;
    }
    return NULL;
}

/* fnmatch(): file matching routine for shell-like patterns.

   Currently only *, ? and [...] (character classes) are recognized, no curly
   braces. As a replacement, more than one pattern may be specified,
   with different patterns being separated with whitespace.

   This could be implemented more efficiently, but the present simple
   backtracking routine does reasonably well for the usual kinds of
   patterns. */

Boolean fnmatch(String pattern, String fn)
{
    String p, pat = alloca((strlen(pattern)+1)*sizeof(char));
    strcpy(pat, pattern);
    if ((p = split2(pat, ' ')))
	do
	    if (fnmatch1(p, fn))
		return True;
	while ((p = split2(NULL, ' ')));
    return False;
}

Boolean fnmatch1(String pattern, String fn)
{
    char *start, *fn0 = fn;
  
    for (;; fn++, pattern++) {
    
	switch (*pattern) {
      
	case '?':
	    if (!*fn)
		return False;
	    break;
      
	case '*':
	    pattern++;
	    do
		if (fnmatch1(pattern, fn))
		    return True;
	    while (*fn++);
	    return False;
      
	case '[':
	    start = pattern+1;
	    do {
	      next:
		pattern++;
		if (*pattern == ']')
		    return False;
		else if (pattern[0] == '-' && pattern > start &&
			 pattern[1] != ']')
		    if (pattern[-1] <= *fn && *fn <= pattern[1])
			break;
		    else {
			start = (++pattern)+1;
			goto next;
		    }
	    } while (*fn != *pattern);
	    while (*pattern != ']')
		if (!*pattern++)
		    return False;
	    break;
   
	case '\\':
	    if (*(++pattern) != *fn)
		return False;
	    break;

	default:
	    if (*fn != *pattern)
		return False;
	}
    
	if (!*fn)
	    return True;
    }
}

Boolean fnxmatch(String pattern, String fn)
{
    String p, pat = alloca((strlen(pattern)+1)*sizeof(char));
    strcpy(pat, pattern);
    if ((p = split2(pat, ' ')))
	do
	    if (!strcmp(p, fn))
		return True;
	while ((p = split2(NULL, ' ')));
    return False;
}

Boolean fnmatchnodot(String pattern, String fn)
{
    if (strcmp(fn, ".") && strcmp(fn, ".."))
	return fnmatch(pattern, fn);
    else
	return fnxmatch(pattern, fn);
}

String resname(String result, String label)
{
  String s, t;

  for (s = label, t = result; *s; s++)
    if (*s == ' ')
      *t++ = '_';
    else if (isalnum(*s) || *s == '_' || *s == '-')
      *t++ = *s;
  *t++ = '\0';
  return result;
}
