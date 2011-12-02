#!/bin/perl
$vcid='$Id: makesig.pl,v 1.1 2000/04/26 12:26:44 rocky Exp $';
# Create a C array structure from a list of signals. 
#
#    Copyright (C) 1998,1999 Rocky Bernstein
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
&init(); 
$output = `$kill_cmd`;
# BSDI kill gives signals in lower case.
$output =~ tr/a-z/A-Z/;
$rc=$?>>8;
die "Can't run $kill_cmd: $!" if $rc;
$no_action="NO ACTION";
$max_sig=length($no_action);

@sigs = split(/\s+/, $output);

# All of this just so we get our favorite signals listed first in 
# the order we specified. 
foreach $sig (@fav_sigs) { $fav_sigs{$sig}++; }
foreach $sig (@sigs) {
  next if $sig =~ /[-,+]|RTMIN|RTMAX/;  # RTMIN+1, RTMAX-1, etc. doesn't work. 

  $max_sig = length($sig) if length($sig) > $max_sig;
  if ($fav_sigs{$sig}) {
    $fav_sigs{$sig}++;
  } else {
    push(@new_sigs, $sig);
  }
}

# We've now marked signals common to @fav_sig and @sig. @new_sig
# has those that are not in @fav_sig. %fav_sigs is 2 where the signal
# is in both lists, (which most of the time is probably all of fav_sigs).

@sigs = sort @new_sigs;
while(@fav_sigs) {
  $sig = pop(@fav_sigs);
  unshift(@sigs, $sig) if $fav_sigs{$sig}==2;
}

print 
"/* Created by $program
   for $sysinfo 
   on $now using:
   $kill_cmd 
*/

";

# Add include if we can find it.
print "#include <sys/signal.h>\n"
    if -f "/usr/include/sys/signal.h";

print "

#define MAX_SIGNAL_NAME $max_sig
typedef
  struct {
    char     signal_name[MAX_SIGNAL_NAME];
    signal_t sig;
  } sigmap_el;

static sigmap_el signal_map[] =
{
";
  printf "  {  \"%-15s\t0},\n", "$no_action\",";

foreach $sig (@sigs) {
  $signame="SIG$sig";
  printf "#ifdef $signame\n";
  printf "  {  \"%-15s\t$signame},\n", "$sig\",";
  printf "#endif\n";
}
print "};\n";
	
sub init {
  ($program = $0) =~ s,.*/,,;
  $ENV{'PATH'} = "$ENV{'PATH'}:/bin:/sbin:/usr/bin:/usr/sbin";
  $sysinfo=`uname -a`; chop $sysinfo;
  $now=localtime;
  $kill_cmd = "kill -l";
  # These signals will appear first.
  @fav_sigs = ('HUP', 'TERM', 'KILL', 'STOP', 'CONT', 'QUIT', 'USR1');
}

sub show_version {
  print "$vcid
by R. Bernstein Copyright (C) 1998,99 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.
There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.
";
  exit 10;
}
