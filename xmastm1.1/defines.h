#ifndef DEFINES_H
#define DEFINES_H

/*
** some important client_data
*/

#define CHSET1      "chset1"
#define CHSET2      "chset2"
#define CHSET3      "chset3"
#define CHSET4      "chset4"
#define CHSET5      "chset5"

#define PrintCommand    "lpr"
#define Rc_Rp           1
#define Rc_Wp           2
#define Wc_Wp           0

#define Version	"1.1"
#define VersionString "Version1.1"

#define ALWAYS_SENSITIVE        1
#define PRINT_THIS_CARD         2
#define PRINT_DATABASE          3

#define RESTART 				1
#define FILE_SAVE				2
#define QUIT					3
#define SAVE_AS					4
#define INCLUDE_FILE			5



#define FILE_EXISTS			1
#define FILE_IS_A_DIRECTORY 2
#define FILE_IS_UNREADABLE  3
#define FILE_IS_NOT_REGULAR 4
#define FILE_IS_NOT_WRITABLE	5



/*
** help client_data
*/

#define ABOUT_HELP					1	
#define HINTS_HELP					2
#define FILE_MENU					3	
#define OPTIONS_MENU				4
#define SEARCH_OPTIONS				5
#define ADDRESS_LIST_HELP			6
#define	FOLDER_PIXMAPS_HELP			7
#define STATUS_HELP					8
#define SEARCH_HELP					9
#define ADD_CARD_HELP				10
#define EDIT_CARD_HELP				11
#define DELETE_CARD_HELP			12
#define SEND_MAIL_HELP				13
#define VIEW_SCREEN_NAME_HELP		14
#define VIEW_SCREEN_PH1_HELP		15
#define VIEW_SCREEN_PH2_HELP		16
#define VIEW_SCREEN_FAX_HELP		17
#define VIEW_SCREEN_EMAIL_HELP		18
#define VIEW_SCREEN_ADDRESS_HELP	19
#define VIEW_SCREEN_REMARKS_HELP	20
#define PRINT_CARDS                 21
#define FILE_OPEND_HELP             22
#define VIEW_COMPANY_HELP           23
#define VIEW_ADDRESS_HELP           24

#define YES	1
#define NO	2

#define TRUE    1
#define FALSE   0

#define HappyFace   1
#define SadFace     2

#define EDIT_OR_ADD_CARD	0
#define DELETE_CARD			1
#define DISMISS_WINDOW		2
#define SEND_MAIL			3
	

#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))

#ifndef True
#define True  1
#endif
#ifndef False
#define False  0
#endif

#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define SystemCommand(command)  system(command)
#define MaxTextLength		2048

#endif /*DEFINES_H*/
