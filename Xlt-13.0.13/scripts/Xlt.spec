Summary:   Xlt : LessTif/Motif extension library
Name:      Xlt
Version:   13.0.13
Release:   1
Copyright: Free to use but copyrighted
Packager:  dannybackx@users.sourceforge.net
Group:     System Environment/Libraries
Prefixes:  /usr/X11R6 /usr/doc
Source:    ftp://ftp.hungry.com/pub/hungry/lesstif/Xlt-13.0.13.tar.gz
BuildRoot: /tmp/Xlt_13.0.13-root

%description
Xlt : LessTif/Motif extension library

%prep
%setup -n Xlt-13.0.13

LESSTIFTOP=$PWD

CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=/usr/X11R6 --enable-shared --disable-static --disable-debug

%build
make -j2

%install
make install DESTDIR=$RPM_BUILD_ROOT

# cleanup in a preparation for an installation - unify layout

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%clean
#do this manually; it's a better idea
#rm -rf $RPM_BUILD_ROOT

%files
# %attr(644, root, root) /usr/X11R6/LessTif/AUTHORS
# %attr(644, root, root) /usr/X11R6/LessTif/BUG-REPORTING
# %attr(644, root, root) /usr/X11R6/LessTif/COPYING
%attr(755, root, root) /usr/X11R6/lib/libXlt.la
%attr(755, root, root) /usr/X11R6/lib/libXlt.so*
#%attr(755, root, root) %dir /usr/X11R6/LessTif/doc
#%attr(-, root, root) /usr/X11R6/LessTif/doc/*
#%attr(644, root, root) /usr/X11R6/man/man1/lesstif.1*
%attr(644, root, root) /usr/X11R6/include/Xlt/*

%changelog
* Fri Jul 20 2001 Danny Backx <danny.backx@skynet.be>
- Initial spec file.


