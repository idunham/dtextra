#
# $Header: /cvsroot/xlt/Xlt/lib/Makefile.am,v 1.58 2005/04/18 15:34:52 dannybackx Exp $
#

MAINTAINERCLEANFILES=Makefile.in

INCLUDES = @MOTIF_CFLAGS@ @X_CFLAGS@ @XPM_CFLAGS@

xltdir = $(includedir)/Xlt

libXlt_la_LDFLAGS = -version-info @CURRENT@:@REVISION@:@AGE@

lib_LTLIBRARIES = libXlt.la

libXlt_la_LIBADD = $(MOTIF_LIBADD)
#libXlt_la_LIBADD =  $(X_LIBS) -lXt $(X_PRE_LIBS) @XPM_LIBS@ -lX11 $(X_EXTRA_LIBS) -lm
# alternatives above don't work due to libtool/auto* flaws:
# if you would have to link against a libXm not in path where the X libraries
# are, you would pick up that one in the X library path :-(
#libXlt_la_LIBADD =


noinst_HEADERS = calc.h

libXlt_la_SOURCES = \
	NumEntry.c calc.c BubbleButton.c AnimatedButton.c Host.c HostTranscript.c \
	EntrySB.c DisplayFallbackResources.c DisplayOptions.c RedirectStdErr.c \
	SelectDebugLevel.c SetClientIcon.c Sound.c ToolBar.c Working.c YesNo.c \
	Stroke.c NodeButton.c FilePicker.c SciPlot.c SciPlotUtil.c ListTree.c \
	AppShell.c FontChooser.c SelectionBox.c SlideC.c Rdb.c Timer.c \
	LiteClue.c


xlt_HEADERS = \
	Xlt.h NumEntry.h NumEntryP.h BubbleButton.h BubbleButtonP.h \
	AnimatedButton.h AnimatedButtonP.h Host.h HostP.h EntrySB.h EntrySBP.h \
	Sound.h Stroke.h NodeButton.h NodeButtonP.h FilePicker.h FilePickerP.h \
	SciPlot.h SciPlotP.h SciPlotUtil.h AppShell.h AppShellP.h \
	ListTree.h ListTreeP.h FontChooser.h FontChooserP.h SelectionBox.h SelectionBoxP.h \
	SlideC.h SlideCP.h Timer.h LiteClue.h LiteClueP.h
