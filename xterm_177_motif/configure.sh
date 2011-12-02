#!/bin/sh

# muquit@muquit.com Apr-22-2003 

rm -f config.cache
c="./configure --enable-256-color --with-motif --enable-tcap-query"
#c="./configure --enable-256-color --with-motif"
echo $c
$c

