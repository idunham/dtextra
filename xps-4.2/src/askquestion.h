/****************************************************************
 * AskQuestion: A function that posts a question dialog and
 *               does not return until the user has responded.
 ****************************************************************/
Boolean AskQuestion ( Widget parent, char *str );
void ShowDialog ( Widget parent, const int dialog_type, char *str );

/* 
   Same as ShowDialog but uses allows for scroll bars and allows for 
   copying text. 
 */
void ShowScrolledDialog ( Widget parent, const int dialog_type, char *str );
