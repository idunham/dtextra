
/***************************************************************************/
/* parse.y: parse xplorerc file                                            */
/***************************************************************************/

#ifndef PARSE_H
#define PARSE_H

#include "xplore.h"

void parse_config_file(String name, String cpp_command, String cpp_options);
void parse_config_string(String source);
Boolean parse_actions(int type, String actions);

/* report errors when parsing interactively: */
extern int errline;
extern char errstr[];

#endif /* PARSE_H */
