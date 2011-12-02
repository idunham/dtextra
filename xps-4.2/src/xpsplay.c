/*
  $Id: xpsplay.c,v 1.1 2002/12/01 22:00:56 rockyb Exp $
  Play a tune. Minor revisions to put this into modern C.
*/
/* Not sure if the following is necessary, since the original also
 * appeared in play.c, which did not have the same notice.  Anyway, to
 * be on the safe side ...                (JMP)
 */

/*
 * $XConsortium: Mailbox.c,v 1.17 88/09/30 08:45:06 swick Exp $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */


#include <X11/Intrinsic.h>
#include "xpsplay.h"

/* Convert string to integer.  I would use strtol(), but that's not on BSD.  */
static int 
getnum(char *ptr, char *newptr[])
{
  int n = 0;

  /* Scan for a digit */
  while (*ptr && (*ptr < '0' || *ptr > '9')) ptr++;

  /* Convert numbers until a non-digit is reached */
  while (*ptr >= '0' && *ptr <= '9')
    {
      n = n * 10 + *ptr - '0';
      ptr++;
    }
  *newptr = ptr;
  return n;
}

/* 
   Play a tune.
   Format of tuneptr string is pitch duration [pitch duration]...
*/
void 
xpsplay(Display *dpy, char *tuneptr)
{
  char *ptr, *newptr;
  XKeyboardState cur_state;
  XKeyboardControl new_ctl;

  /* Get current keyboard settings */
  XGetKeyboardControl(dpy, &cur_state);

  /* Play the tune */
  for (ptr = tuneptr; strlen(ptr) > 0; )
    {
      new_ctl.bell_pitch = getnum(ptr, &newptr);
      if (ptr == newptr) break;
      ptr = newptr;
      new_ctl.bell_duration = getnum(ptr, &newptr);
      if (ptr == newptr) break;
      ptr = newptr;
      if (new_ctl.bell_duration == 0) new_ctl.bell_duration = 50;
      XChangeKeyboardControl(dpy, (unsigned long)
                             (KBBellPitch | KBBellDuration), &new_ctl);
      XBell(dpy, 0);
    }

  /* Restore original settings.  Even though the mask specifies that only
     pitch and duration should be changed, other settings are modified (at
     least on AIX that's what happens).  Therefore, restore click and bell
     percent also.  The LED mask that's returned by XGetKeyboardControl
     does not change if an LED is set from the keyboard, so it doesn't do
     much good to try to restore that.  */

  new_ctl.key_click_percent = cur_state.key_click_percent;
  new_ctl.bell_percent = cur_state.bell_percent;
  new_ctl.bell_pitch = cur_state.bell_pitch;
  new_ctl.bell_duration = cur_state.bell_duration;
  XChangeKeyboardControl(dpy, (unsigned long)
                         (KBKeyClickPercent | KBBellPercent | KBBellPitch |
                          KBBellDuration), &new_ctl);
}

