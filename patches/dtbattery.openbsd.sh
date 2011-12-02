#!/bin/sh
# Script to get battery status for OpenBSD
# NOT TESTED!
# This should serve as a basis for getting the battery indicator
# working under OpenBSD.

if [ `apm -a` -eq 0 ]; then
        echo -n '-1 '
elif [ `apm -b` -eq 3 ]; then
        echo -n '1 '
elif [ `apm -a` -eq 1 ]; then
        echo -n '0 '
else
        exit 1
fi

echo -n "`apm -l` "

secs=$(apm -m)

if [ "$secs" = "unknown" ]; then
        echo -n '0'
else
        echo -n "$secs"
fi
