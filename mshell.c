#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "errno.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "signal.h"
#include "mstack.h"
#include "malias.h"
#include "mshell.h"
#include "mprofile.h"
#include "errdef.h"

#include "syscl_lib.h"

/*
 * precompile cmd.
 */
void precompile(char *cmd){
    //puts(cmd);
    //return;
    if(cmd[strlen(cmd)-1] == '\n'){       // = '\n'
        cmd[strlen(cmd)-1] = '\0';
    }

    if(precedence_check(cmd) != -1){
        precedence_parser(cmd);
    } else {
        print_error(PARENTHESIS_UNMATCH);
    }
}
/*
 * handler for Keyboard.
 */
void sigint_handler(int signal)
{
    char rv[20];
//    sigset_t mask, oldmask;

//    sigfillset(&mask);
//    sigprocmask(SIG_SETMASK, &mask, &oldmask);
//    sigprocmask(SIG_BLOCK ,&mask, NULL);

	/*
	TODO: KeyBoard Listener
	*/

    if (fgets(rv, 20, stdin) == NULL){
        printf("NULL!\n");
    }
//    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    printf("%s\n", rv);
    if (rv[0]=='Y' || rv[0] == 'y'){
        exit(0);
    }
    fflush(stdin);
}

/*
 * check command precedence, eg, parenthesized cmd have higer precedence.
 */
int precedence_check(char *cmd){
    int i,tier,max;
    for(i=0, tier=0, max=0; i<strlen(cmd); i++){
        if(cmd[i] == '('){
            tier++;
            if(max<tier){
                max = tier;
            }
        } else if(cmd[i] == ')'){
            tier--;
        }
    }
    return tier==0 ? max:-1;
}

/*
 * Build command into argv list.
 */
int build_argv(char *cmd, int argc){
    char *argv[argc+10];
    int i, j, k;

    // Check empty command, if command is empty, then return 1.
    while(*cmd == ' ' || *cmd == ','){
        cmd++;
    }
    if(strlen(cmd)== 0){
        set_error(EMPTY_COMMAND);
        return EMPTY_COMMAND;
    }

    // Check alias
    //getaliascmd(&cmd);

    // Seperate command into argv array.
    //argv[0] = (char*) malloc(strlen(cmd)*sizeof(char));
    argv[0] = (char*) calloc(strlen(cmd), sizeof(char));
    for(i=0, j=0, k=0; i<=strlen(cmd); i++, j++){
        if(cmd[i] == ' '){
            while(cmd[i+1] == ' '){
                i++;
            }
            argv[k][j] = '\0';
            j=-1;
            if(strlen(argv[k]) > 0){
                k++;
                //argv[k] = (char*) malloc(strlen(cmd)*sizeof(char));
                argv[k] = (char*) calloc(strlen(cmd), sizeof(char));
            }
        } else {
            argv[k][j] = cmd[i];
        }
    }
    if(strlen(argv[k]) == 0){
        argv[k] = NULL;
    } else {
        argv[k+1] = NULL;
    }
    //printf("Passing %s to execmd...\n", cmd);

    // Execute.
    //puts(cmd);
    //return 0;
    execcmd(cmd, argv);

    // Free resources.
    for(i=0; argv[i] != NULL; i++){
        free(argv[i]);
    }
    return 0;
}

/*
 * Execute a command touched by Yating Zhou(aka syscl)
 */
int execcmd(char *cmd, char** argv)
{
    //puts(cmd);
    //return 0;

    int status;
    int cpid;

    if (strcmp(argv[0], "exit")==0)
    {
        exit(0);
    }
    else if (strcmp(argv[0],"loadprofile") == 0 && argv[1] != NULL)
    {
        load_profile(argv[1]);
    }
    else if (strcmp(argv[0], "init_alarm") == 0)
    {
        if (argv[1] != NULL)
        {
            if (strcmp(argv[1], "1") == 0)
            {
                setAlarmByUsr(1);
            }
            else if (strcmp(argv[1], "0") == 0)
            {
                setAlarmByUsr(0);
            }
        }
        else
        {
            setAlarmByUsr(3);
        }
    }
    else if(strcmp(argv[0],"alias") == 0)
    {
        setmalias(cmd);
    }
    else if(strcmp(argv[0],"unalias") == 0)
    {
        if(argv[1]!=NULL)
        {
            unmalias(argv[1]);
        }
        else
        {
            print_error(COMMAND_INCORRECT, cmd);
            return COMMAND_INCORRECT;
        }
    }
    else if (strcmp(argv[0],"cd") == 0)
    {
        if (argv[1] == NULL || strcmp(argv[1],"~")==0 || strcmp(argv[1], "") == 0)
        {
            chdir("/root");
        }
        else
        {
            if ((status = chdir(argv[1])) != 0)
            {
                print_error(COMMAND_EXECUTE_FAIL, cmd, status);
                return COMMAND_EXECUTE_FAIL;
            }
        }
    }
    else
    {
        //
        // Yating Zhou(aka syscl) for final execute step
        //

        //
        // Yating(aka syscl): the last operator '&' is processed here
        //
        int argvlen = gArgc(argv);
        int rfork = gAmphIndex(argv); // get '&' index e.g. echo 1 & echo 2 => rfork == 2
        if (rfork == 0)
        {
            //
            // '&' is at position zero, left shift argv
            //puts("Need a shift operation");
            //puts("Shifting...");
            //printArgs(argvlen, argv);
            //
            int s = 0;
            while (argv[s] != NULL)
            {
                argv[s] = argv[s + 1];
                s++;
            }
            argv[s - 1] = NULL;
            //
            // update argvlen due to char '&' has been eliminated
            //
            argvlen--;
            rfork--;
        }
        else if (rfork > 0)
        {
            int k = rfork + 1;
            if (argv[k] == NULL)
            {
                //
                // cmd2 is totally empty, which means only
                // one cmd(cmd1) left to be executed
                //
                argv[rfork] = NULL;
                argvlen--;
                //
                // no need to pass both cmd1 and cmd2 in this case
                // set rfork to -1 to disable cmdline2
                //
                rfork = -1;
            }
        }

        char *cmdline1[argvlen];
        char *cmdline2[argvlen];
        //printArgs(argvlen, argv);

        if (rfork >0)
        {
            int v;
            for (v = 0; v < rfork; v++)
            {
                cmdline1[v] = argv[v];
            }
            cmdline1[v] = 0;

            int w;
            for (w = 0; w < v && v < argvlen; w++)
            {
                v++;
                cmdline2[w] = argv[v];
            }
            w++;
            cmdline2[w] = 0;
        }
        /*
        // this loop should not be entered if there's no '&'
        for (int u = 0; u < argvlen && rfork > 0; u++)
        {
            if (strcmp(argv[u], "&") == 0)
            {
                //
                // yes, we find '&', build/separate two commands
                //
                int v;
                for (v = 0; v < u; v++)
                {
                    //printf("argv is %s\n", argv[v]);
                    cmdline1[v] = argv[v];
                    //printf("cmdline1 is %s\n", cmdline1[v]);
                }
                //v++;
                //printf("cmdline1 v++ is %s\n", cmdline1[0]);
                cmdline1[v] = 0;

                //printf("cmdline1 built %s\n", cmdline1[0]);
                //
                // now build command2
                //
                int w;
                for (w = 0; w < v && v < argvlen; w++)
                {
                    v++;
                    cmdline2[w] = argv[v];
                }
                w++;
                cmdline2[w] = 0;
                //printArgs(u, cmdline1);
                //printArgs(argvlen - u, cmdline2);
                break;
            }
            printf("cmdline1 for %s, cmdline2 for %s\n", cmdline1[0], cmdline2[0]);
        }*/
        //printf("cmdline1 is %s\n", cmdline1[0]);

        if (rfork != -1)
        {
            //
            // & is present, now we call fork()
            //
            //printf("cmdline1 is %s\n", cmdline1[0]);
            cpid = fork();
            if (cpid == 0)
            {
                //
                // child process
                //
                //puts("enter fork");
                if (rfork == 1)
                {
                    int v;
                    for (v = 0; v < rfork; v++)
                    {
                        cmdline1[v] = argv[v];
                    }
                    cmdline1[v] = 0;
                }
                execvp(cmdline1[0], cmdline1);
            }
            else if (cpid > 0)
            {
                //
                // parent process
                //
                execcmd(cmdline2[0], cmdline2);
            }
        }
        else
        {
            int isValCMD = isValidateCMD(argv);
            if (isValCMD)
            {
                cpid = fork();
                if (cpid == 0)
                {
                    //puts("enter second fork()");
                    //printf("%s\n", argv[0]);
                    //printArgs(argv);
                    execvp(argv[0], argv);
                }
                else if (cpid > 0)
                {
                    int isValidateCommand = isValidateCMD(argv);

                    if (isValidateCommand)
                    {
                        gProcIsDead = kFALSE;

                        for (int loop = 0; !gProcIsDead && loop < kLOOPCYCLE && kSetAlarm; ++loop)
                        {
                            if (waitpid(cpid, &status, WNOHANG) == cpid)
                            {
                                //
                                // child process finished
                                //
                                gProcIsDead = kTRUE;
                            }
                            usleep(kMICROSEC);
                        }

                        if (!gProcIsDead && kSetAlarm)
                        {
                            char c;
                            printf("Do you want to terminal? [y/n]: ");
                            c = getchar();
                            if (c == 'y' || c == 'Y')
                            {
                                DebugLog("kill child process\n");
                                kill(cpid, SIGKILL);
                            }
                            else
                            {
                                waitpid(cpid, &status, 0);
                            }
                        }
                        else
                        {
                            waitpid(cpid, &status, 0);
                        }
                    }
                }
            }
        }
    }
    return kRETSUCCESS;
}

/*
 * split semicolon of a command.
 */
int split_semicolon(char *cmd){
    char *str;
    int i, j, argc;

    //str = (char*) malloc(strlen(cmd)*sizeof(char) + 10);
    str = (char*) calloc(strlen(cmd) + 10, sizeof(char));
    for(i=0, j=0, argc=0; i<=strlen(cmd); i++, j++){
        if(cmd[i] == ';' || cmd[i] == ','){
            str[j] = '\0';
            build_argv(str, argc);
            argc = 0;
            j = -1;
        } else {
            if(cmd[i] == ' '){
                argc++;
            }
            str[j] = cmd[i];
        }
    }
    build_argv(str, argc);

    free(str);
    return 0;
}

/*
 * parse command into command list based on parenthesis.
 */
int precedence_parser(char *cmd){
    char *str;
    void *value;
    int i, j;
    mstack *stk;

    initstack(&stk);

    //str = (char*) malloc(strlen(cmd)*sizeof(char) + 10);
    str = (char*) calloc(strlen(cmd) + 10, sizeof(char));
    for(i=0, j=0; i<=strlen(cmd); i++, j++){
        if(cmd[i] == '('){
            str[j] = ';';
            str[j+1] = '\0';
            push(str, stk);     //Push current str into stacks
            //str = (char*) malloc(strlen(cmd)*sizeof(char) + 10);       //str point into new string
            str = (char*) calloc(strlen(cmd) + 10, sizeof(char));
            j= -1;
        } else if(cmd[i] == ')'){
            str[j] = '\0';
            pop(&value, stk);   //Pop item from stack
            split_semicolon(str);
            free(str);
            str = (char *)value;
            j = strlen(str)-1;
        } else {
            str[j] = cmd[i];
        }
    }
    split_semicolon(str);

    free(str);
    freestack(stk);
    return 0;
}
