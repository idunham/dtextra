/*
 *  SetCb - sets the ascii values in decimal,hex or octal
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Description:
 *      text
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   19-Jun-96  first cut
 */


#include "xhead.h"
#include "mxascii.h"


static char *ascii_values[132] =
{
"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
"BS ", "HT ", "LF ", "VT ", "FF ", "CR ", "SO ", "SI ",
"DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYC", "ETB",
"CAN", "EM ", "SUB", "ESC", "FS ", "GS ", "RS ", "US ",
"SPA", "!  ", "\"  ", "#  ", "$  ", "%  ", "&  ", "'  ",
"(  ", ")  ", "*  ", "+  ", ",  ", "-  ", ".  ", "/  ",
"0  ", "1  ", "2  ", "3  ", "4  ", "5  ", "6  ", "7  ",
"8  ", "9  ", ":  ", ";  ", "<  ", "=  ", ">  ", "?  ",
"@  ", "A  ", "B  ", "C  ", "D  ", "E  ", "F  ", "G  ",
"H  ", "I  ", "J  ", "K  ", "L  ", "M  ", "N  ", "O  ",
"P  ", "Q  ", "R  ", "S  ", "T  ", "U  ", "V  ", "W  ",
"X  ", "Y  ", "Z  ", "[  ", "\\  ", "]  ", "^  ", "_  ",
"`  ", "a  ", "b  ", "c  ", "d  ", "e  ", "f  ", "g  ",
"h  ", "i  ", "j  ", "k  ", "l  ", "m  ", "n  ", "o  ",
"p  ", "q  ", "r  ", "s  ", "t  ", "u  ", "v  ", "w  ",
"x  ", "y  ", "z  ", "{  ", "|  ", "}  ", "~  ", "DEL",
"   ", "   ", "   ", "   "
};

static char *decimal_values[132] =
{
"   0","   1","   2","   3","   4","   5","   6","   7",
"   8","   9","  10","  11","  12","  13","  14","  15",
"  16","  17","  18","  19","  20","  21","  22","  23",
"  24","  25","  26","  27","  28","  29","  30","  31",
"  32","  33","  34","  35","  36","  37","  38","  39",
"  40","  41","  42","  43","  44","  45","  46","  47",
"  48","  49","  50","  51","  52","  53","  54","  55",
"  56","  57","  58","  59","  60","  61","  62","  63",
"  64","  65","  66","  67","  68","  69","  70","  71",
"  72","  73","  74","  75","  76","  77","  78","  79",
"  80","  81","  82","  83","  84","  85","  86","  87",
"  88","  89","  90","  91","  92","  93","  94","  95",
"  96","  97","  98","  99"," 100"," 101"," 102"," 103",
" 104"," 105"," 106"," 107"," 108"," 109"," 110"," 111",
" 112"," 113"," 114"," 115"," 116"," 117"," 118"," 119",
" 120"," 121"," 122"," 123"," 124"," 125"," 126"," 127",
"    ","    ","    ","    "
};

static char *hex_values [132] =
{
"0x00","0x01","0x02","0x03","0x04","0x05","0x06","0x07",
"0x08","0x09","0x0A","0x0B","0x0C","0x0D","0x0E","0x0F",
"0x10","0x11","0x12","0x13","0x14","0x15","0x16","0x17",
"0x18","0x19","0x1A","0x1B","0x1C","0x1D","0x1E","0x1F",
"0x20","0x21","0x22","0x23","0x24","0x25","0x26","0x27",
"0x28","0x29","0x2A","0x2B","0x2C","0x2D","0x2E","0x2F",
"0x30","0x31","0x32","0x33","0x34","0x35","0x36","0x37",
"0x38","0x39","0x3A","0x3B","0x3C","0x3D","0x3E","0x3F", 
"0x40","0x41","0x42","0x43","0x44","0x45","0x46","0x47",
"0x48","0x49","0x4A","0x4B","0x4C","0x4D","0x4E","0x4F",
"0x50","0x51","0x52","0x53","0x54","0x55","0x56","0x57",
"0x58","0x59","0x5A","0x5B","0x5C","0x5D","0x5E","0x5F",
"0x60","0x61","0x62","0x63","0x64","0x65","0x66","0x67",
"0x68","0x69","0x6A","0x6B","0x6C","0x6D","0x6E","0x6F", 
"0x70","0x71","0x72","0x73","0x74","0x75","0x76","0x77",
"0x78","0x79","0x7A","0x7B","0x7C","0x7D","0x7E","0x7F",
"    ","    ","    ","    "
};

static char *oct_values[132] =
{
" 000", " 001", " 002", " 003", " 004", " 005", " 006", " 007",
" 010", " 011", " 012", " 013", " 014", " 015", " 016", " 017",
" 020", " 021", " 022", " 023", " 024", " 025", " 026", " 027",
" 030", " 031", " 032", " 033", " 034", " 035", " 036", " 037",
" 040", " 041", " 042", " 043", " 044", " 045", " 046", " 047",
" 050", " 051", " 052", " 053", " 054", " 055", " 056", " 057",
" 060", " 061", " 062", " 063", " 064", " 065", " 066", " 067",
" 070", " 071", " 072", " 073", " 074", " 075", " 076", " 077",
"0100", "0101", "0102", "0103", "0104", "0105", "0106", "0107",
"0110", "0111", "0112", "0113", "0114", "0115", "0116", "0117",
"0120", "0121", "0122", "0123", "0124", "0125", "0126", "0127",
"0130", "0131", "0132", "0133", "0134", "0135", "0136", "0137",
"0140", "0141", "0142", "0143", "0144", "0145", "0146", "0147",
"0150", "0151", "0152", "0153", "0154", "0155", "0156", "0157",
"0160", "0161", "0162", "0163", "0164", "0165", "0166", "0167",
"0170", "0171", "0172", "0173", "0174", "0175", "0176", "0177",
"    ", "    ", "    ", "    "  
};



static char
    **Sdecimal_columns,
    **Sascii_columns,
    **Shex_columns,
    **Soct_columns;

static XmString
    Sxmstr_dval[6],
    Sxmstr_hval[6],
    Sxmstr_oval[6],
    Sxmstr_ascii[6];


void MakeTable (void)
{
    register int
        i,
        j;

    Sdecimal_columns = (char **)XtCalloc (6, sizeof (char *));
    Sascii_columns = (char **)XtCalloc (6, sizeof (char *));
    Shex_columns = (char **)XtCalloc (6, sizeof (char *));
    Soct_columns = (char **)XtCalloc (6, sizeof (char *));
    for (i=0; i < 6; i++)
    {

        Sdecimal_columns[i] = (char *)  XtCalloc (112, sizeof (char));
        Sascii_columns[i] = (char *)  XtCalloc (112, sizeof (char));
        Shex_columns[i] = (char *)  XtCalloc (112, sizeof (char));
        Soct_columns[i] = (char *)  XtCalloc (112, sizeof (char));

        for (j= (i*22); j < ((i+1) * 22); j++)
        {
            strcat (Sascii_columns[i], ascii_values[j]);
            strcat (Sascii_columns[i], "\n");

            strcat (Sdecimal_columns[i], decimal_values[j]);
            strcat (Sdecimal_columns[i], "\n");

            strcat (Shex_columns[i], hex_values[j]);
            strcat (Shex_columns[i], "\n");

            strcat (Soct_columns[i], oct_values[j]);
            strcat (Soct_columns[i], "\n");

        }
    }

    
    for (i=0; i < 6; i++)
    {
        Sxmstr_dval[i] = XmStringCreateLtoR(Sdecimal_columns[i],
            XmSTRING_DEFAULT_CHARSET);
        Sxmstr_hval[i] = XmStringCreateLtoR(Shex_columns[i],
            XmSTRING_DEFAULT_CHARSET);
        Sxmstr_oval[i] = XmStringCreateLtoR(Soct_columns[i],
            XmSTRING_DEFAULT_CHARSET);

        Sxmstr_ascii[i] = XmStringCreateLtoR(Sascii_columns[i],
            XmSTRING_DEFAULT_CHARSET);
    }
}

void SetCb (Widget widget,XtPointer client_data,XtPointer call_data)
{
    int
        i;

    int
        type;

    static int
        first_time=0;

    type=(int) client_data;

    if (first_time == 0)
    {
        for (i=0; i < 6; i++)
        XtVaSetValues (Gascii_labelW[i],
            XmNlabelString,Sxmstr_ascii[i],
            NULL);
        first_time=1;
    }

    for (i=0; i < 3; i++)
    {
        XtVaSetValues(Gtoggle_menuW[i],
            XmNset, False,
            NULL);
    }
    XtVaSetValues (Gtoggle_menuW[type],
        XmNset, True,
        NULL);

    switch (type)
    {

        case 0: /* ASCII */
        {
            for (i=0; i < 6; i++)
            {
                XtVaSetValues (Gvalue_labelW[i],
                    XmNlabelString, Sxmstr_dval[i],
                    NULL);
                if (!first_time)
                XtVaSetValues (Gascii_labelW[i],
                    XmNlabelString,Sxmstr_ascii[i],
                    NULL);
            }
            break;
        }

        case 1: /* HEX */
        {
            for (i=0; i < 6; i++)
            {
                XtVaSetValues (Gvalue_labelW[i],
                    XmNlabelString, Sxmstr_hval[i],
                    NULL);
                if (!first_time)
                XtVaSetValues (Gascii_labelW[i],
                    XmNlabelString,Sxmstr_ascii[i],
                    NULL);
            }
            break;
        }

        case 2: /* OCT */
        {
            for (i=0; i < 6; i++)
            {
                XtVaSetValues (Gvalue_labelW[i],
                    XmNlabelString, Sxmstr_oval[i],
                    NULL);
                if (!first_time)
                XtVaSetValues (Gascii_labelW[i],
                    XmNlabelString,Sxmstr_ascii[i],
                    NULL);
            }
            break;
        }
    }

}

