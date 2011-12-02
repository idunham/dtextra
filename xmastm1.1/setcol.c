/*
 *  SetColor() -   sets the picked color
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Security:
 *      Unclassified
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
 *      who                 when        why
 *      muquit@semcor.com   16-Apr-95   first cut
 */

#include "xhead.h"
#include "xmastm.h"

void SetColor (widget, client_data, call_data)
Widget
    widget;
XtPointer
    client_data;
XtPointer
    call_data;
{

    int
        index;

    switch ((int) client_data)
    {
        case 0:
        case 4:
        case 8:
        case 12:
        case 16:
        case 20:
        {
            switch (GcurrentRow)
            {
                case 0:
                {
                    index = 0;
                    break;
                }

                case 1:
                {
                    index = 4;
                    break;
                }
                
                case 2:
                {
                    index = 8;
                    break;
                }

                case 3:
                {
                    index = 12;
                    break;
                }

                case 4:
                {
                    index = 16;
                    break;
                }

                case 5:
                {
                    index = 20;
                    break;
                }

                case 6:
                {
                    index = 24;
                    break;
                }

                case 7:
                {
                    index = 28;
                    break;
                }

                case 8:
                {
                    index = 32;
                    break;
                }

                case 9:
                {
                    index = 36;
                    break;
                }

            }
            GcurrentCol = 0;
            Gcol1=1;
            GuessPeg[GcurrentCol] = (int) client_data/4;
            XtVaSetValues (w_special[index],
                XmNbackground, Gpbg[(int) client_data/4],
                NULL);
            break;
        }

        case 1:
        case 5:
        case 9:
        case 13:
        case 17:
        case 21:
        {
            switch (GcurrentRow)
            {
                case 0:
                {
                    index = 1;
                    break;
                }

                case 1:
                {
                    index = 5;
                    break;
                }
                
                case 2:
                {
                    index = 9;
                    break;
                }

                case 3:
                {
                    index = 13;
                    break;
                }

                case 4:
                {
                    index = 17;
                    break;
                }               

                case 5:
                {
                    index = 21;
                    break;
                }

                case 6:
                {
                    index = 25;
                    break;
                }

                case 7:
                {
                    index = 29;
                    break;
                }

                case 8:
                {
                    index = 33;
                    break;
                }

                case 9:
                {
                    index = 37;
                    break;
                }

            }
            GcurrentCol = 1;
            Gcol2=1;
            GuessPeg[GcurrentCol] = (int) client_data/4;
            XtVaSetValues (w_special[index],
                XmNbackground, Gpbg[(int) client_data/4],
                NULL);
            break;
        }

        case 2:
        case 6:
        case 10:
        case 14:
        case 18:
        case 22:
        {
            switch (GcurrentRow)
            {
                case 0:
                {
                    index = 2;
                    break;
                }

                case 1:
                {
                    index = 6;
                    break;
                }
                
                case 2:
                {
                    index = 10;
                    break;
                }

                case 3:
                {
                    index = 14;
                    break;
                }

                case 4:
                {
                    index = 18;
                    break;
                }               

                case 5:
                {
                    index = 22;
                    break;
                }

                case 6:
                {
                    index = 26;
                    break;
                }

                case 7:
                {
                    index = 30;
                    break;
                }

                case 8:
                {
                    index = 34;
                    break;
                }

                case 9:
                {
                    index = 38;
                    break;
                }

            }          
            GcurrentCol = 2;
            Gcol3=1;
            GuessPeg[GcurrentCol] = (int) client_data/4;
            XtVaSetValues (w_special[index],
                XmNbackground, Gpbg[(int) client_data/4],
                NULL);
            break;
        }

        case 3:
        case 7:
        case 11:
        case 15:
        case 19:
        case 23:
        {
            switch (GcurrentRow)
            {
                case 0:
                {
                    index = 3;
                    break;
                }

                case 1:
                {
                    index = 7;
                    break;
                }
                
                case 2:
                {
                    index = 11;
                    break;
                }

                case 3:
                {
                    index = 15;
                    break;
                }

                case 4:
                {
                    index = 19;
                    break;
                }               

                case 5:
                {
                    index = 23;
                    break;
                }

                case 6:
                {
                    index = 27;
                    break;
                }

                case 7:
                {
                    index = 31;
                    break;
                }

                case 8:
                {
                    index = 35;
                    break;
                }

                case 9:
                {
                    index = 39;
                    break;
                }

            }                    
            GcurrentCol = 3;
            Gcol4=1;
            GuessPeg[GcurrentCol] = (int) client_data/4;
            XtVaSetValues (w_special[index],
                XmNbackground, Gpbg[(int) client_data/4],
                NULL);
            break;
        }
    }

}
