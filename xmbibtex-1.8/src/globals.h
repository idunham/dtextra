/* Reference Manager      */
/* global variables       */
/* Last change 05.05.1996 */

#include <Xm/Xm.h>
#include "mybiblio.h"

#define N_INPUT_FILTER 5   /* number of input filters */

/* Structure for menu system */
typedef struct {
  Widget f_menu;
  Widget f_new;
  Widget f_load;
  Widget f_save;
  Widget f_print;
  Widget f_quit;
} f_menu;

typedef struct {
  Widget e_menu;
  Widget e_add;
  Widget e_edit;
  Widget e_delete;
  Widget e_reduce;
} e_menu;

typedef struct {
  Widget s_menu;
  Widget s_searchall;
  Widget s_and;
  Widget s_or;
  Widget s_not;
  Widget s_showall;
  Widget s_case;
} s_menu;

typedef struct {
  f_menu file;
  e_menu edit;
  s_menu search;
} main_menu;


extern XtAppContext  app;
extern MYBIBLIO      b;
extern unsigned long *key;
extern char          *database;
extern Widget        toplevel;
extern Widget        liste;
extern Widget        message;
extern main_menu     mymenu;
