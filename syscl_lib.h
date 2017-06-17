//
// Yating Zhou(aka syscl)'s header
//

#ifndef __SYSCL_LIB_HEADER__
#define __SYSCL_LIB_HEADER__
//
// kDEBUG == 0 - turn off debug log
// kDEBUG == 1 - turn on debug log
//
#define kDEBUG       0

#define kTRUE        1
#define kFALSE       0

#define kRETSUCCESS  0
#define kRETFAILURE  1

#define kMAXBUF      255

#define kTIMEOUT     5        /* 5 seconds */
#define kMICROSEC    10000    /* 10000 microseconds = 0.01s */
#define kMICSEC_SEC  0.01     /* microseconds to sec */
#define kLOOPCYCLE   (kTIMEOUT / kMICSEC_SEC)

#define kRESETALARM  0     /* extra bonus: a long period of time to disable alarm */
//==============================================================================
// some debug and test cases (c) syscl/Yating Zhou
//==============================================================================

void DebugLog(char*);

//==============================================================================
// signal for alarm and trap (c) syscl/Yating Zhou
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
// for int usleep(useconds_t microseconds)
#include <unistd.h>
#include <time.h>

int kSetAlarm;
//void setkSetAlarm(char*);
void setAlarmByUsr(int);

int gTerminatePromt;
int gProcIsDead;

void init_alarm(void);
void set_alarm(void);
void sig_alarm(int);
void tst_alarm(int);

void trapKeys(void);
void ctrl_cHandler(int);

//==============================================================================
// parse operators (c) syscl/Yating Zhou
//==============================================================================

int gArgc(char **);
int gAmphIndex(char**);
void printArgs(int, char**);
int isValidateCMD(char **);
int isMatch(char **);
int countMatch(char *);
//
// auto completion
//
//#define CONFIG_CMDLINE_EDITING
//#define CONFIG_AUTO_COMPLETE
//==============================================================================
#define AUTHORS "GuangWei Ban, Lun Li, Yating Zhou"
void printHeader(void);

#define CMDSIZE 255
#define STRBUF  63
const char gBIN[CMDSIZE][STRBUF];

#endif
