
/**************************************************************************/
/* devmount.h: routines for automatically mounting/unmounting devices     */
/**************************************************************************/

#ifndef DEVMOUNT_H
#define DEVMOUNT_H

#include "xplore.h"
#include "icons.h"

/* determine whether mount/umount errors should be handled */

extern Boolean check;

/* operations to build the device table */

void initdevs(void);
/* (re)initialize the device table */

int newdev(String name, String label, String icon_name,
	   String mount_action, String umount_action,
	   int options, int setoptions, int clroptions);
/* enter a new device into the table:
   - name: absolute pathname of mountpoint
   - label: descriptive label used to show the device in the indicator panel
   - icon_name: name of icon shown in the indicator panel
   - mount_action, umount_action: commands to mount and unmount the device
     (may be NULL to denote a nop)
   - options: default options to use for the device (may be NONE to specify
     that no special option values should be used)
   - setoptions: options to add to the option set
   - clroptions: options to remove from the option set
   - return value: number of new device */

/* mount/unmount operations */

int devmount(String name);
/* mount device identified by directory name if necessary; return values:
   ANY (device does not need to be mounted), NONE (error mounting device);
   any other value is number of mounted device which can be passed to
   devumount (see below) */

int devmount_ignore(String name);
/* ignore failed mount action (pretend that the device has been mounted) */

Boolean devumount(int d);
/* unmount device given by handle d, return True if successful */

void devumount_ignore(int d);
/* ignore failed umount action (pretend that the device has been unmounted) */

/* value operations */

int ndevs(void);
/* return number of devices in table, valid range of devices is
   0..ndevs()-1 */

int finddev(String name);
/* return the number of the device matching name, ANY if not in the table;
   links are resolved */

String devnam(int d);
String devlabel(int d);
String devmount_action(int d);
String devumount_action(int d);
int devoptions(int d);
int devsetoptions(int d);
int devclroptions(int d);
/* return information associated with the device */

IconPtr devicon(int d);
/* icon for the device */

int devcount(int d);
/* current mount count of device */

Boolean devcheck(int d);
/* check whether the device is still mounted, i.e. whether it has a nonzero
   mount count; this always returns True if d is ANY or NONE; other devices
   are checked against the current mount table if necessary */

Boolean premounted(int d);
/* check whether a device is premounted, i.e. has been mounted by some other
   program and will hence not be unmounted automatically */

#endif /* DEVMOUNT_H */
