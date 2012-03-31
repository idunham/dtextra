/*
 * process.c
 *	Tools and such for dealing with child processes.
 *
 * Copyright (C) 1998 Eric A. Howe
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/process.c 1.4 1998/09/01 20:49:39 $")

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <Xm/XmStrDefs.h>

#include <wlib/wlib.h>
#include <wlib/stdlib.h>
#include <mine/process.h>

int
mgv_child_msg(Widget msgbox, int fd)
{
	char  buf[1024];
	int   size;
	char *msg      = NULL;
	int   msg_size = 0;

	while((size = read(fd, buf, sizeof(buf))) > 0) {
		msg = XtRealloc(msg, size + msg_size + 1);
		memcpy(&msg[msg_size], buf, size);
		msg[msg_size + size] = '\0';
		msg_size += size;
	}

	if(msg == NULL)
		return FALSE;

	XtVaSetValues(msgbox,
		XtVaTypedArg,	XmNmessageString,
				XtRString, msg,
				strlen(msg) + 1,
		NULL);
	wl_dlg_popup(msgbox);
	XtFree(msg);

	return TRUE;
}

int
mgv_popen(PIPEKID *pk, char *cmd, char **argv)
{
#	define copy_or_close(cp, p, fd) (void)((cp) ? (p) = (fd) : close(fd))
	int   std_in[2], std_out[2], std_err[2];
	pid_t kid;

	if(pipe(std_in) < 0)
		return errno;
	if(pipe(std_out) < 0) {
		int error = errno;
		close(std_in[0]);
		close(std_in[1]);
		return error;
	}
	if(pipe(std_err) < 0) {
		int error = errno;
		close(std_in[0]);
		close(std_in[1]);
		close(std_out[0]);
		close(std_out[1]);
		return error;
	}

	kid = fork();
	switch(kid) {
	case -1:
		return errno;
		break;

	case 0:
		close(std_out[0]);
		dup2(std_out[1], fileno(stdout));
		close(std_out[1]);

		close(std_err[0]);
		dup2(std_err[1], fileno(stderr));
		close(std_err[1]);

		close(std_in[1]);
		dup2(std_in[0], fileno(stdin));
		close(std_in[0]);

		execvp(cmd, argv);
		fprintf(stderr, "Cannot execute %s:\n%s", cmd, strerror(errno));
		exit(EXIT_FAILURE);
		break;

	default:
		close(std_out[1]);
		close(std_err[1]);
		close(std_in[0]);

		copy_or_close(pk->flags & PK_IN,  pk->in,  std_in[1]);
		copy_or_close(pk->flags & PK_OUT, pk->out, std_out[0]);
		copy_or_close(pk->flags & PK_ERR, pk->err, std_err[0]);
		pk->pid = kid;

		break;
	}

	return 0;
#	undef copy_or_close
}
