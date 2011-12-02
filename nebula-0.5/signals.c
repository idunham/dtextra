/* --------------------------------------------------------------------- *
 * signals.c                                                             *
 * --------------------------------------------------------------------- *
 * This file is part of the nebula irc client                            *
 * http://nebula-irc.sourceforge.net                                     *

 * Copyright (c) 2004 Riccardo Mottola. All rights reserved.             *
 * mail: rmottola@users.sourceforge.net                                  *

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
Copyright (c) 2004-2005, Riccardo Mottola

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer. 
Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution. 
Neither the name of "Riccardo Mottola" nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS;´OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "main.h"
#include "network.h"
#include "signals.h"

extern systemStatusRecord sysState;


RETSIGTYPE brokenPipe()
{
    signal(SIGPIPE, brokenPipe);
    printf("BrokenPipe!\n");
    if (sysState.isConnected)
    {
        printf ("Was connected...n");
        if (!close(sysState.socket))
            printf("closed ns\n");
    } else {
    	printf("This makes no sense!!! broken pipe on a closed socket!\n");
    	exit(0);
    }
    sysState.isConnected = NO;
    sysState.isAuthenticated = NO;
}

