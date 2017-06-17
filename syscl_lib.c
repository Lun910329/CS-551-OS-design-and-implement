//
// Yating Zhou(aka syscl)'s library
//
#include "syscl_lib.h"

//==============================================================================
// print out personal information
//==============================================================================

void printHeader(void)
{
    puts("\n-------------------------------");
    puts("Welcome to custom shell");
    printf("Author: %s\n", AUTHORS);
    puts("HOME  : /usr/src");
    puts("Change profile path by: `loadprofile [profile]`");
    puts("-------------------------------");
}

//==============================================================================
// DebugLog log out the debug information
//==============================================================================

void DebugLog(char *log)
{
    if (kDEBUG == 1)
    {
        printf("Debug: ===> %s", log);
    }
    return;
}

//==============================================================================
// init and reset alarm()
// alarm is cancelled by a call to alarm() before anything else.
// A new one won't be created if the argument to alarm() is zero.
//==============================================================================

int kSetAlarm = kFALSE;

void setAlarmByUsr(int flag)
{
    if (flag == kTRUE)
    {
        // valiate flag bit, disable
        puts("Disable alarm");
        kSetAlarm = kFALSE;
    }
    else if (flag == kFALSE)
    {
        // validate flag bit, enable
        puts("Enable alarm");
        kSetAlarm = kTRUE;
    }
    else
    {
        // invalidate flag bit, ask user
        char ch;
        printf("Do you want to turn off alarm for process running longer than 5s? [y/n]: ");
        ch = getchar();
        printf("%c\n", ch);
        if (ch == 'y' || ch == 'Y')
        {
            setAlarmByUsr(kTRUE);
            //kSetAlarm = kTRUE;
        }
        else
        {
            setAlarmByUsr(kFALSE);
            //kSetAlarm = kFALSE;
        }
    }
    return;
}

void init_alarm(void)
{
    DebugLog("init_alarm()\n");
    alarm(kRESETALARM);
    DebugLog("init_alarm()::return\n");
}

//==============================================================================
// set_alarm() set alarm after exectuing a command
//==============================================================================

void set_alarm(void)
{
    signal(SIGALRM, sig_alarm);
}

//==============================================================================
// sig_alarm() notified by alarm()
//==============================================================================

void sig_alarm(int sig)
{
    char c;
    signal(sig, sig_alarm);
    printf("Do you want to terminal? [y/n]: ");
    DebugLog("getchar()::init()\n");
    c = getchar();
    DebugLog("getchar()::return()\n");
    if (c == 'y' || c == 'Y')
    {
        DebugLog("exit()\n");
        //exit(0);
    }
    return;
}

//==============================================================================
// gArgc(char **) get argument elements number
// argv[0] = "prog";
// argv[1] = "argv";
// ...
// argv[n] = NULL;
// gArgc(argv) return n
//==============================================================================

int gArgc(char **args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        i++;
    }
    return i;
}

int gAmphIndex(char **str)
{
    int i = 0;
    while (str[i] != NULL)
    {
        if (strcmp(str[i], "&") == 0)
        {
            //printf("gAmphIndex = %d\n", i);
            return i;
        }
        i++;
    }
    return -1;
}

void printArgs(int num, char **args)
{
    int i = 0;
    for (; i < num && args[i] != NULL; i++)
    {
        printf("args[%d] is %s\n", i, args[i]);
    }
    printf("args[%d] is %s\n\n", i, args[i]);
}

//==============================================================================

void tst_alarm(int timeout)
{
    for (int i = 1; i < timeout; i++)
    {
        printf("Sleep %ds...\n", i);
        sleep(1);
    }
}

//==============================================================================
// This routine trap the keystroke (input listener)
//==============================================================================

void trapKeys(void)
{
    signal(SIGINT, ctrl_cHandler);
}

//==============================================================================
// This routine trap the ctrl+c
//==============================================================================

void ctrl_cHandler(int sig)
{
    char c;
    signal(sig, ctrl_cHandler);
    printf("\nAre you sure to exit msh? [y/n]: ");
    DebugLog("getchar()::init()\n");
    c = getchar();
    printf("%c\n", c);
    DebugLog("getchar()::return()\n");
    if (c == 'y' || c == 'Y')
    {
        DebugLog("exit()\n");
        exit(0);
    }
    return;
}

//==============================================================================
// mute alarm when type in line that is not a command
//==============================================================================

const char gBIN[CMDSIZE][STRBUF] = {                                                            \
    "echo",            "loadkeys",        "read",            "sync",            "cat",          \
    "ed",              "ls",              "readclock",       "sysenv",          "cd",           \
    "expr",            "mkdir",           "reboot",          "tar",             "chmod",        \
    "mount",           "rm",              "test",            "clone",           "getopts",      \
    "mv",              "rmdir",           "command",         "getty",           "netconf",      \
    "sed",             "umask",           "cp",              "halt",            "pax",          \
    "service",         "umount",          "cpdir",           "intr",            "printconfig",  \
    "setup",           "update_bootcfg",  "cpio",            "kill",            "printroot",    \
    "sh",              "updateboot",      "date",            "ln",              "pwd",          \
    "shutdown",        "wait",            "sleep",           "git",             "syscl"         \
};

int isValidateCMD(char **str)
{
    //extern char *gBin[];
    int i = 0;
    while (strcmp(gBIN[i], "syscl") != 0)
    {
        if (strcmp(gBIN[i], str[0]) == 0)
        {
            //printf("%s is validate command\n", str[0]);
            return kTRUE;
        }
        i++;
    }
    //printf("%s is not a validate command\n", str[0]);
    return kFALSE;
}

int isMatch(char **str)
{
    int i = 0;
    while (strcmp(gBIN[i], "syscl") != 0)
    {
        if (strcmp(gBIN[i], str[0]) == 0)
        {
            printf("%s", gBIN[i]);
            return i;
        }
        i++;
    }
    return -1;
}

//==============================================================================
// syscl: return how many command string that are matching in the command list
//==============================================================================

int countMatch(char *cmdstr)
{
    int found = 0;
    for (int n = 0; n < CMDSIZE && strcmp(gBIN[n], "syscl") != 0; n++)
    {
        //
        // syscl: need to match from the begining
        //
        if (strstr(gBIN[n], cmdstr) == gBIN[n])
        {
            found++;
        }
    }
    //printf("there are %d matched\n", found);
    return found;
}

//==============================================================================
// End of line. Add function here
//==============================================================================
