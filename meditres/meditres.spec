#
# $Header: /cvsroot-fuse/meditres/meditres/meditres.spec.in,v 1.1.1.1 2001/08/15 13:00:14 dannybackx Exp $
#
Summary         : A free tool for data entry and reporting
Name            : meditres
Version         : 0.1
Release         : 2
Group           : Office/Spreadsheet
Copyright       : GPL
Packager        : danny.backx@skynet.be
# Icon            : src/meditres.xpm
BuildRoot       : /tmp/meditres-0.1
Source          : ftp://alpha.gnu.org/gnu/meditres/meditres-0.1.tar.gz

%description
...

%prep
%setup

%build
./configure
make

%install
DESTDIR=$RPM_BUILD_ROOT; export DESTDIR
[ -n "`echo $DESTDIR | sed -n 's:^/tmp/[^.].*$:OK:p'`" ] && rm -rf $DESTDIR ||
(echo "Invalid BuildRoot: '$DESTDIR'! Check this .spec ..."; exit 1) || exit 1

make install DESTDIR="$DESTDIR" datadir="$DESTDIR/usr/local/share" gnulocaledir="$DESTDIR/usr/local/share/locale" xdefaultsdir="$DESTDIR/usr/local/share/meditres"

# gzip $DESTDIR/usr/local/info/*

%clean
DESTDIR=$RPM_BUILD_ROOT;export DESTDIR;[ -n "$UID" ]&&[ "$UID" -gt 0 ]&&exit 0
[ -n "`echo $DESTDIR | sed -n 's:^/tmp/[^.].*$:OK:p'`" ] && rm -rf $DESTDIR ||
(echo "Invalid BuildRoot: '$DESTDIR'! Check this .spec ..."; exit 1) || exit 1


%files
%doc AUTHORS COPYING ChangeLog FAQ NEWS README
/usr/local/bin/meditres
