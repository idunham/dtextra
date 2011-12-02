#!/usr/bin/perl -w
#$Id: postconf.pl,v 1.6 2003/04/07 16:57:25 rockyb Exp $
# Copyright (C) 1999-2002 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# Use perl 4 conventions: chop vs chomp, system(ln) vs symlink.
# Todo: Should use config.guess? 
print "Checking operating system and version...\n";
#$TARGET=`./config.guess`;
$os=`uname -s`; chop $os;
if ($os !~ /^AIX|^Linux|^SunOS|^NetBSD|^OpenBSD|^FreeBSD|^BSD/) {
    die "Sorry -- we only do AIX, Linux, Solaris, NetBSD, OpenBSD, FreeBSD or BSDI right now.";
} 
$release=`uname -r`; chop $release;
if ($os =~ /SunOS/) {
  if ($release !~ /^5.[56]/) {
    if ($release !~ /^5.6/) {
      print "SPARC releases 2.5.1 and 2.6, 2.8 have been tested.\n";
      print "SPARC 2.7 and 2.6 x68 have been alleged to work\n";
      print "Good luck!\n";
    }
  }
} elsif ($os =~ /Linux/) {
  if ($release !~ /^2.[0,2,4]/) {
    print "Only releases 2.0, 2.2 & 2.4 have been tested. Trying anyway.\n";
  }
} elsif ($os =~ /OpenBSD/) {
  $os = 'NetBSD';
} elsif ($os =~ /^BSD/) {
  $os="BSDI";	
  if ($release !~ /^2.1|^3.0|^4.3/) {
    print "Only releases 2.1 and 3.0, 4.3 have been tested. Trying anyway.\n";
  }
} 

print "Configuring for $os...\n";
# Okay, now just shut up and set it up.
system("chmod u+w .");
foreach $file ("os.h", "os.c", "sigmap.h", 
	       "loadavg.c", "loadavg.h")  {
    unlink "src/$file" if -f  "src/$file";
    print "Linking $file...\n";
    #symlink "@ARGV[0]/$os/$file", "src/$file";      # If Perl 5 
    system("ln -fs $ARGV[0]/$os/$file src/$file");   # IW
}

# Remind users to look at platform-specific information; some folks
# will, no doubt, not pick up this information reading README or
# INSTALL.xps, if they in fact they even look at these. 

$install_hints = "$os/install-hints.txt";
if (-f $install_hints) {
    print "***** Please also read $install_hints... *****\n";
}
