//
//  history.h
//  proj1
//
//  Created by GavinBan on 2/24/17.
//  Copyright © 2017 GavinBan. All rights reserved.
//

#ifndef history_h
#define history_h

#include <termios.h>
// syscl patch
#include <stdio.h>
#include <string.h>

#define MAXCOMMAND 500
#define MAXPPATH 100

char cmd[MAXCOMMAND];
char path[MAXPPATH];
//char save[10][20];
char save[10][40];
int turn_num;

int save_cmd(int length);
void set_keypress(void);
void reset_keypress(void);
void history(int *length);

#endif /* history_h */
