//
//  myinclude.h
//  proj1
//
//  Created by GavinBan on 2/24/17.
//  Copyright © 2017 GavinBan. All rights reserved.
//

#ifndef myinclude_h
#define myinclude_h

#define MAXCOMMAND 500
#define MAXPPATH   100
#define SPINLOCK   1

#define AUTHORS "GuangWei Ban, Lun Li, Yating Zhou"

#define keyPressMode 1
extern char cmd[MAXCOMMAND];
extern char *arg[10];
extern int cn;
extern char addr[40];
extern pid_t PID;
#endif /* myinclude_h */
