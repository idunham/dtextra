
#include "xplore.h"
#include "devmount.h"
#include "mnttable.h"

#include "util.h"

Boolean check = True;

typedef struct _DevRec {
    String name, label;
    String mount_action, umount_action;
    IconPtr icon;
    int options, setoptions, clroptions;
    int count;
    Boolean res, chk;
} DevRec, *DevList;

static int numdevs = 0;
static DevList devs = NULL;

void initdevs(void)
{
    int d;

    for (d = 0; d < numdevs; d++) {
	FREE(devs[d].name);
	FREE(devs[d].mount_action);
	FREE(devs[d].umount_action);
    }
    FREE(devs);
    devs = NULL;
    numdevs = 0;
}

int newdev(String name, String label, String icon_name,
	   String mount_action, String umount_action,
	   int options, int setoptions, int clroptions)
{
  char path[MAXPATHLEN+1];

  if (name)
    name = abspath(path, basedir, name);
  devs = (DevList) REALLOC(devs, (numdevs+1)*sizeof(DevRec));
  devs[numdevs].name = NEWSTRING(name);
  devs[numdevs].label = NEWSTRING(label);
  if (icon_name && *icon_name)
    devs[numdevs].icon = read_icon(icon_name);
  else
    devs[numdevs].icon = NULL;
  devs[numdevs].mount_action = NEWSTRING(mount_action);
  devs[numdevs].umount_action = NEWSTRING(umount_action);
  devs[numdevs].options = options;
  devs[numdevs].setoptions = setoptions;
  devs[numdevs].clroptions = clroptions;
  devs[numdevs].count = 0;
  devs[numdevs].res = False;
  devs[numdevs].chk = False;
  return numdevs++;
}

int devmount(String name)
{
  int d = finddev(name);

  if (d != ANY) {
    if (check_mnt_table())
      get_mnt_table();
    if (devs[d].count)
      devs[d].count++;
    else if (search_mnt_table(devs[d].name) != -1) {
      devs[d].res = False;
      devs[d].chk = True;
      devs[d].count++;
    } else {
      if (!devs[d].mount_action) {
	devs[d].res = True;
	devs[d].chk = search_mnt_table(devs[d].name) != -1;
      } else {
	fileExec(devs[d].mount_action, NULL, NULL);
	get_mnt_table();
	if (search_mnt_table(devs[d].name) != -1)
	  devs[d].res = devs[d].chk = True;
	else if (!check)
	  devs[d].res = devs[d].chk = False;
	else
	  return NONE;
      }
      devs[d].count++;
    }
  }
  return d;
}

int devmount_ignore(String name)
{
  int d = finddev(name);
  if (d != ANY) {
    if (!devs[d].count)
      devs[d].res = devs[d].chk = False;
    devs[d].count++;
  }
  return d;
}

Boolean devumount(int d)
{
  if (d == ANY || d == NONE)
    return True;
  else if (d < 0 || d >= numdevs)
    return False;
  else if (!devs[d].count)
    return True;
  else {
    if (devs[d].count == 1 &&
	devs[d].res &&
	devs[d].umount_action) {
      fileExec(devs[d].umount_action, NULL, NULL);
      get_mnt_table();
      if (check && search_mnt_table(devs[d].name) != -1)
	return False;
    }
    devs[d].count--;
    return True;
  }
}

void devumount_ignore(int d)
{
    if (0 <= d && d < numdevs)
	devs[d].count = 0;
}

int ndevs(void)
{
    return numdevs;
}

int finddev(String name)
{
    int d;
    char path[MAXPATHLEN+1];

    resolve(path, name);
    for (d = 0; d < numdevs; d++)
	if (devs[d].name &&
	    (!strcmp(devs[d].name, path) ||
	     prefix(devs[d].name, path) &&
	     name[strlen(devs[d].name)] == '/'))
	    return d;
    return ANY;
}

String devnam(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].name;
    else
	return NULL;
}

String devlabel(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].label;
    else
	return NULL;
}

String devmount_action(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].mount_action;
    else
	return NULL;
}

String devumount_action(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].umount_action;
    else
	return NULL;
}

int devoptions(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].options;
    else
	return NONE;
}

int devsetoptions(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].setoptions;
    else
	return NONE;
}

int devclroptions(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].clroptions;
    else
	return NONE;
}

IconPtr devicon(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].icon;
    else
	return NULL;
}

int devcount(int d)
{
    if (0 <= d && d < numdevs)
	return devs[d].count;
    else
	return 0;
}

Boolean devcheck(int d)
{
    if (d == ANY || d == NONE)
	return True;
    else if (d < 0 || d >= numdevs || !devs[d].count)
	return False;
    else
	return !devs[d].chk || search_mnt_table(devs[d].name) != -1;
}

Boolean premounted(int d)
{
    if (d == ANY || d == NONE)
	return False;
    else if (d < 0 || d >= numdevs)
	return False;
    else if (devs[d].count)
        return !devs[d].res && devs[d].chk;
    else {
        if (check_mnt_table())
            get_mnt_table();
	return search_mnt_table(devs[d].name) != -1;
    }
}
