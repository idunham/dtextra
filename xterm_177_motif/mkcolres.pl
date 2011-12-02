#!/usr/bin/perl
#
# make the resource file XTerm with color names from rgb.txt file
# if the xterm is compiled with 256 color support you can have lots of fun
# in vim
# muquit@muquit.com Apr-22-2003 
#
use strict;

my $argc=$#ARGV+1;
if ($argc != 1)
{
    print "usage: $0 <rgb.txt>\n";
    exit;
}
my $file=$ARGV[0];
local *FD;

open(FD,$file) or die "could not open file for reading $file ($!)";

# first 16 colors are pre-defined
print<<EOF;
*VT100*colorMode: on
*VT100*dynamicColors: on

! Uncomment this to use color for underline attribute
*VT100*colorULMode: on
*VT100*underLine: off

! Uncomment this to use color for the bold attribute
*VT100*colorBDMode: on

! Uncomment this to use the bold/underline colors in preference to other colors
!*VT100*colorAttrMode: on

*VT100*color0:                   black
*VT100*color1:                   red
*VT100*color2:                   #008000
*VT100*color3:                   #ff8247
*VT100*color4:                   LightSkyBlue
*VT100*color5:                   #8a2be2
*VT100*color6:                   sienna1
*VT100*color7:                   LightSkyBlue
*VT100*color8:                   yellow
*VT100*color9:                   sienna1
*VT100*color10:                  PaleVioletRed1
*VT100*color11:                  LightSkyBlue
*VT100*color12:                  black
*VT100*color13:                  white
*VT100*color14:                  white
*VT100*color15:                  white
EOF
;
my $i=15;

while (<FD>)
{
    my $line=$_;
    $line =~ /[0-9]+\s+[0-9]+\s+[0-9]+\s+(.*)$/;
    my $color="$1";
    next if (! $color);
    next if ($color =~ /\s+/);
    $i++;
    print "*VT100*color${i}:       $color\n";
    chomp($line);
}
