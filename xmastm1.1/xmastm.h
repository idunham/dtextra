/*
 *	xmastm.h
 *
 *	RCS:
 *		$Revision$
 *		$Date$
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		text
 *
 *	Dependencies:
 *		file1.h
 *
 *	Comments:
 *		text
 *
 *	Development History:
 *		when	who					why
 *	10/29/94	muquit@semcor.com	first cut
 */

#ifndef XMASTM_H
#define XMASTM_H


#ifndef MAIN
#define EXTERN extern
#else
#define EXTERN
#endif	/* MAIN */

#if __STDC__ || defined(sgi) || defined(_AIX)
#define _D(formal_parameters) formal_parameters
#else
#define const
#define _D(forma_parameters) ()
#endif	/* __STDC__ */

#define Rows           10
#define Cols           4
#define MaxColors      6


/*
** globals
*/

    EXTERN Pixel
        Gpbg[6];

    EXTERN int
        CodePeg[Cols],
        GuessPeg[Cols];

	EXTERN unsigned long
		black_pixel,
		white_pixel;

	EXTERN
		String scb_bgcolor;

    EXTERN int
        G_rc_rip,
        G_rc_wrp;

/*
** no more global widget array
*/
	EXTERN Widget
        GtoplevelW,
        GRed1,GBlue1,
		GerrorDialogW,
		rolomessageTextW,
		messageTextW,
        GelecLogoW,
        GaskdW,
        Gaskd2W,
        GelecRespW,
        GelecResp2W;

	EXTERN unsigned int
        Gcol1,
        Gcol2,
        Gcol3,
        Gcol4,
        GcurrentRow,
        GcurrentCol;

	EXTERN int
		Gthiscard;

	EXTERN Widget
        w_special[(Rows*Cols)+1],
        GscoreLabW[(Rows*2)+1],
        GhiddenCodeW[Cols],
        GlabW,
		GSformdW,
		GSnameTbW,
        GScompanyTbW,
		GSaddressTbW,
		GSph1TbW,
		GSph2TbW,
		GSfaxTbW,
		GSemailTbW,
		GSremarksTbW,

		GsaverdW,
		GsaverTextW,

        GprintRemarksTbW;

EXTERN Boolean
    GprintDisplayedCard,
	GnoPixmaps,
	GsaveAsing,
	GrolodexModified,
	GsaveAndExit,
	GwroteToTempfile,
	GuseFolderPixmaps;

EXTERN char
	GversionString[20];

EXTERN int
	GnumberOfCards;

EXTERN Cursor
    GcautionCursor;

/*
** global handle to pixmaps in order to free them
*/

EXTERN Pixmap
	GArmPixmap[26];

EXTERN Pixmap
	GBPixmap[26];

EXTERN char
	GtempFilename[200];

/*
** Function Prototypes
*/

void
	CreateDialogs _D ((Widget));

Boolean
	CreateFileOpenDialog _D ((Widget));

Widget
	CreateMainWindow _D ((Widget));
	
void
	FileMenuCB _D ((Widget, XtPointer, XtPointer));

void 
	UnmanageCB _D ((Widget, XtPointer, XtPointer));

void
	Error _D ((char *, char *));

void
	Warning  _D ((char *, char *));

long int
	FileSize _D ((char *));

int
	ReadList _D ((char *));

GC
	MakeGC _D ((Display *, Drawable, unsigned long,
		unsigned long));

XFontStruct
	*LoadFont _D ((Display *, char *));

unsigned int
	AllocateNamedColor _D ((Display *, char *));

void
	FillPixmap _D ((void));

void	
	FileSelectionCB _D((Widget, XtPointer, XtPointer));

void
	CreateFolderPbW _D ((Widget));

void
	SetScrollbarColors _D ((Widget, Widget, Pixel));

int
	XError _D ((Display*, XErrorEvent *));

void
	FolderPbCB _D ((Widget, XtPointer, XtPointer));

void
	MakeNewRolo _D ((char *, char *,char *, char *, char *, 
		char *, char *, char *, char *, int));

void
	SelectedCB _D ((Widget, XtPointer, XtPointer));

void
	SearchRoloCB _D ((Widget, XtPointer, XtPointer));

int
	AllSpace _D ((char *));

int
	mystrcasecmp _D ((char *, char *));

int 
	mystrncasecmp _D ((char *, char *, int));

char
	*mystrcpy _D ((char *, char *));

char
	*mystrstrcase _D ((char *, char *));

void
	ClearSearchCB _D ((Widget, XtPointer, XtPointer));

Boolean
	CreateEditDialog _D ((Widget));

void
	ReturnHit _D ((Widget, XtPointer, XtPointer));

void
	InfoButtonCB _D ((Widget, XtPointer, XtPointer));

void
	AddCardCB _D ((Widget, XtPointer, XtPointer));

void
	WriteList _D ((char *));

char
	*mystrdup _D((char *));

int
	AskUser _D((Widget, char *, int));

void
	Response _D((Widget, XtPointer, XmAnyCallbackStruct *));

void
	DeleteCard _D((int));

int
	MyCopy _D((char *, char *));

void
	TemporaryFilename _D((char *));

void 
	SetResources _D((Widget));

Boolean
	CreateMailDialog _D ((Widget));

void
	MailCB  _D ((Widget, XtPointer, XtPointer));

void
	WriteStatusMessage _D ((int, Widget, char *));

void
	SendMail _D ((void));

void 
	AddSignature _D ((void));

void
	WriteStatus _D ((int, Widget, char *));

void
	CreateFields _D ((Widget));

void
	EditCardCB _D ((Widget, XtPointer, XtPointer));
void
	ClearFields _D ((void));

void
	UpdateView _D ((int));

int 
	PrepareList _D ((void));

void
	SetList _D ((char *));

void
	PutCard _D ((void));

void
	NullCallback _D ((Widget, XtPointer, XtPointer));

Boolean
	CreateSearchOptionD _D ((Widget));

void
	SearchOptiondCB _D ((Widget, XtPointer, XtPointer));

Boolean
	CreateSaveRoloD _D ((Widget));

void
	RoloSaveCB _D ((Widget, XtPointer, XtPointer));

void
	RoloSaveOkCB _D ((Widget, XtPointer, XtPointer));

void
	ModifyVerifyCB _D ((Widget, XtPointer, XtPointer));

int
	CheckFile _D ((char *, int));

int
	OpenAndSaveFile _D ((char *));

int
	IsFileReadable _D ((char *));

Widget
	CreateMessageDialog _D ((Widget, char *, int,
		int, int, int));

void
	ShowErrorDialog _D ((int, char *));


void
	PopSaveDialog _D ((Widget, XtPointer, XtPointer));

Boolean
	CreateAskSaveDialogW _D ((Widget));

Boolean
	CreateHelpDialog _D ((Widget));

Boolean
	CreateAboutDialog _D ((Widget));

Boolean
	CreateRolodexInfoD _D  ((Widget));   

Boolean
	CreateIncludeFileOpenD _D ((Widget));

Boolean
    CreateSearchOutputDiaog _D ((Widget));

Boolean
    CreatePrintDialog _D ((Widget));

void
	HelpCB _D ((Widget, XtPointer, XtPointer));


void
	AboutCB _D ((Widget, XtPointer, XtPointer));

void
	ContextualHelpCB _D ((Widget, XtPointer, XtPointer));

void
	HelpContextCB _D ((Widget, XtPointer, XtPointer));


void
	RoloInfoCB _D ((Widget, XtPointer, XtPointer));

void
	UpdateRoloInfo _D ((void));

void
	Usage _D ((char *));

unsigned int
	ExpandTilde _D ((char *));

void
	SetBusyCursorState _D ((Widget, unsigned int));


void
	IncludeFileCB _D ((Widget, XtPointer, XtPointer));

void
	IncludeFileToWidget _D ((Widget, char *, unsigned int));

char
	*mybasename _D ((char *));

int
	IsBinary _D ((char *));

void
	Uuencode _D ((char *, char *));

void
    GetSearchItems _D ((char *));


void
    SearchSelectCB _D ((Widget, XtPointer, XtPointer));

void
    PrintCB _D ((Widget, XtPointer, XtPointer));

void ManageWidget _D ((Widget));

void
    BestPixel _D ((Display *, Colormap, XColor *, unsigned int, XColor *));


void
    PrintToggleCB _D ((Widget, XtPointer, XtPointer));

void
    PrintFileCB _D ((Widget, XtPointer, XtPointer));

FILE
    *OpenFile _D ((char *, char *));

int
    PrintCards _D ((char *));

Cursor
    MakeCautionCursor _D ((Display *, unsigned long, unsigned long));

void
    SetCautionCursorState _D ((Widget, unsigned int));

void
    SetColor _D ((Widget, XtPointer, XtPointer));

void
    CommitCB _D ((Widget, XtPointer, XtPointer));

void
    MakeCode _D ((void));

void
    SetScore _D ((int));

void
    Restart _D ((void));

void
    SetSymbolPixmap _D ((Widget, int));

void
    ScoreMove _D ((void));

void
    CreateAskDialog _D ((Widget));
void
    CreateAskDialog2 _D ((Widget));

int
    AskCB _D ((Widget, XtPointer, XtPointer));

#endif	/*XCGM_H*/
