# $Id: xps.spec.in,v 1.4 2003/04/09 22:54:47 rockyb Exp $
%define	name	xps
%define	version	4.2
%define	release	1
%define	prefix	/usr

Summary:	GNU dynamic process tree display in via Motif
Name:		%{name}
Version:	%{version}
Release:	%{release}
Prefix:		%{prefix}
License:	GPL 
Group:		Applications/System
URL:		http://motif-pstree.sourceforge.net
Vendor:		R. Bernstein <rocky@panix.com>
Source:		http://prdownloads.sourceforge.net/motif-pstree/%{name}-%{version}.tar.gz
BuildRoot:	/var/tmp/%{name}-%{version}-root
Prereq:		/usr/X11R6/lib/libXm.so
Packager:	R. Bernstein <rocky@panix.com>
Requires: 	XFree86
BuildPrereq:	perl
#BuildPrereq:	LessTif-devel

%description
%{name} dynamically displays in an X Window the Unix processes as a tree
or forest, the roots on the left and the leaf processes (with no
children) on the right.  The status of each process running, sleeping,
stopped, etc.) can be indicated by a color.  Different users can
appear as different colors too.

Process selection can be made per user, all users or through a
regular expression pattern. 

In contrast to pstree and many tree-widget based programs, the tree
display uses diagonal lines; some effort is made to effectively use
the full 2-dimensional area of the screen by balancing levels and
centering the children of a node between their parent.  A goal of the
program is to visually give a picture of what is going on. So when
possible processes are kept close to their parents so one need not
scroll around too much and so that there is not a lot of redrawing as
processes are created or destroyed.

One can select viewing the processes by a single user, a 
regular expression for users, by all users and perhaps show
kernel processes. One can click on a process to get more information
(via ps) about that process, send a signal, or set the process
priority, assuming you have the permission to do so.

Since programs of this ilk can consume a bit of CPU on their own, some
effort has been made to turn off the update process when the program
is iconified or or not visible. Some attention has been paid to make
layout algorithm fairly fast display in most situations, although it
has to be admitted that this comes sometimes at the expense of
a nicer layout.
%prep
%setup -q

%build
%configure
make

%install
if [ -d ${RPM_BUILD_ROOT} ]; then rm -rf ${RPM_BUILD_ROOT}; fi
make DESTDIR="$RPM_BUILD_ROOT" install

%clean
if [ -d ${RPM_BUILD_ROOT} ]; then rm -rf ${RPM_BUILD_ROOT}; fi

%pre
# Nothing

%post
# Nothing

%preun 

%files
%defattr(-,root,root)
%doc COPYING NEWS README
%{_bindir}/*
%{_mandir}/man*/*
%config %attr(644,root,root) /usr/lib/X11/app-defaults/*

%changelog
* Wed Apr  9 2003 Oron Peled <oron@actcom.co.il>
  - Use DESTDIR, now that the XAPPLOADDIR hack isn't needed.

* Fri Mar 21 2003 Oron Peled <oron@actcom.co.il>
  - Remove redundant Copyright: tag
  - Fix the Source: tag -- should include the filename
  - Define the XAPPLOADDIR in %makeinstall. The rule in
    the makefile don't use DESTDIR.
  - Add the app-default file to the %file section
