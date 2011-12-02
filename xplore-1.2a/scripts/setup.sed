#!/bin/sh
# xplore setup script

# Create the user's xplore setup. As distributed, this script copies the
# personal config files and the startup script, creates a shelf and puts some
# links in there.

userconf=.xplore
sysxplorerc=system.xplorerc
userxplorerc=$userconf/xplorerc
sysmagic=magic
usermagic=$userconf/magic
sysstartup=startup
userstartup=$userconf/startup
sysshelf=shelf
usershelf=shelf

if [ -d "$XPLORELIBDIR" ]; then
  cd "$XPLORELIBDIR"
else
  cd @XPLORELIBDIR@
fi

if [ ! -d "$sysshelf" -o ! -f "$sysxplorerc" -o ! -f "$sysmagic" ]; then
  echo $0: fatal error: invalid library directory >&2
  echo "(Please check the value of the XPLORELIBDIR environment variable.)" >&2
  exit 1
fi

echo Xplore user setup:

if [ ! -d "$HOME/$userconf" ]; then

  echo creating $HOME/$userconf
  mkdir "$HOME/$userconf"

fi

if [ ! -f "$HOME/$userxplorerc" ]; then
  echo copying $PWD/$sysxplorerc to $HOME/$userxplorerc
  cp $sysxplorerc $HOME/$userxplorerc
  chmod 644 $HOME/$userxplorerc
fi

if [ ! -f "$HOME/$usermagic" ]; then
  echo copying $PWD/$sysmagic to $HOME/$usermagic
  cp $sysmagic $HOME/$usermagic
  chmod 644 $HOME/$usermagic
fi

if [ ! -f "$HOME/$userstartup" ]; then
  echo copying $PWD/$sysstartup to $HOME/$userstartup
  cp $sysstartup $HOME/$userstartup
  chmod 755 $HOME/$userstartup
fi

if [ ! -d "$HOME/$usershelf" ]; then

  echo copying $PWD/$sysshelf to $HOME/$usershelf
  mkdir "$HOME/$usershelf"
  cp -R "$sysshelf/"* "$HOME/$usershelf"

# Make some links to devices and the user's home directory.
# Note: These are only examples. You probably have to adapt this (and the
# system.xplorerc file) to your local setup.

  echo setting up the shelf
  if [ -d /floppy ]; then
    ln -s /floppy "$HOME/$usershelf/Desk/A:"
  fi
  if [ -d /dos ]; then
    ln -s /dos "$HOME/$usershelf/Desk/C:"
  elif [ -d /win ]; then
    ln -s /win "$HOME/$usershelf/Desk/C:"
  fi
  if [ -d /cdrom ]; then
    ln -s /cdrom "$HOME/$usershelf/Desk/CDR"
  fi

  ln -s "$HOME/$usershelf/Clipboard" "$HOME/$usershelf/Desk/Clipboard"
  ln -s "$HOME/$usershelf/Trash" "$HOME/$usershelf/Desk/Trash"
  ln -s "$HOME" "$HOME/$usershelf/Desk/Home"

# Populate the Programs shelf with some common applications.
# Note: This requires the which(1) program.

  apps="ee emacs gimp mozilla netscape xemacs xfig xterm"
  if [ -d "$HOME/$usershelf/Programs" ]; then
    for app in $apps; do
      if [ -x "`which $app 2>/dev/null`" ]; then
        ln -s "`which $app 2>/dev/null`" "$HOME/$usershelf/Programs"
      fi
    done
  fi

fi
echo Xplore user setup completed successfully.
