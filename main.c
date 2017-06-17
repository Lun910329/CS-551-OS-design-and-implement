#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "signal.h"
#include "mshell.h"
#include "mprofile.h"
#include "errdef.h"
#include "syscl_lib.h"
#include "history.h"


#define MAXCOMMAND 500
#define MAXPPATH   100
#define SPINLOCK   1
#define LISTLENTH  2

#define keyPressMode 1
extern char cmd[MAXCOMMAND];
extern char path[MAXPPATH];
extern char *arg[10];
extern int cn;
extern char addr[40];
extern pid_t PID;
//extern char save[10][20];
extern char save[10][40];
extern int turn_num;

//==============================================================================

int my_strstr(char cmd[MAXCOMMAND], char *arg[10])
{
    char *token;
    int i=0;
    token=strtok(cmd," ");
    while(token != NULL)
    {
        arg[i++]=token;
        token=strtok(NULL," ");
    }
    arg[i]=NULL;
    return i;
}

//==============================================================================

int main(int argc, char **argv)
{
    printHeader();
    sigset_t mask;
    int key = 0;
    int j   = 0;
    //
    // Yating: set key stroke trap/listener
    //
    trapKeys();

    sigfillset(&mask);
//    sigdelset(&mask,SIGQUIT);
//    sigdelset(&mask,SIGKILL);
//    sigdelset(&mask,SIGTERM);
    sigdelset(&mask,SIGINT);
    //sigprocmask(SIG_BLOCK ,&mask, NULL);

    read_profile();                         // Read profile from default profile

    while (SPINLOCK)
    {

        j = 0;
        memset(cmd, 0, MAXCOMMAND/*sizeof(cmd)*/);
        getcwd(path, MAXPPATH);
        printf("\nroot:%s$ ", path);
        if(keyPressMode == 1){


            set_keypress();

            while(1) {


                key = getchar();

                //TODO: NOTED List

                if (key == 9)
                {
                    //
                    // tap key detected
                    //
                    int findIndex;
                    int findCount = countMatch(cmd);
                    for (int n=0; n < CMDSIZE && strcmp(gBIN[n], "syscl") != 0; n++)
                    {
                        if(strstr(gBIN[n], cmd) == gBIN[n])
                        {
                            findIndex = n;
                        }
                    }

                    if (findCount > 1)
                    {
                        //
                        // two or more command match, thus print it out to screen
                        //
                        putchar('\n');
                        for (int n=0; n < CMDSIZE && strcmp(gBIN[n], "syscl") != 0; n++)
                        {
                            int k = 0;
                            if (strstr(gBIN[n], cmd) == gBIN[n])
                            {
                                // find k, plus one
                                k++;
                                printf("%s\t",gBIN[n]);
                            }
                            if ((k + 1) % 4 == 0)
                            {
                                putchar('\n');
                            }
                        }
                        printf("\nroot:%s$ %s", path, cmd);
                    }
                    else if (findCount == 1)
                    {
                        //
                        // syscl: match command found, auto fill here
                        //
                        memset(cmd, 0, sizeof(cmd));
                        strcpy(cmd, gBIN[findIndex]);
                        printf("\r                                                              ");
                        printf("\rroot:%s$ %s", path , cmd);
                        //set j depend on new command lenth
                        int indexi = 0;
                        for (indexi = 0; gBIN[findIndex][indexi] != '\0'; indexi++)
                            ;
                            j = indexi;
                        }
                }
                else if(key == 27)		//detect special key
                /**{

                    getchar();
                    key = getchar();
                    //show history
                    if(key == 65)           //up key
                    {
                        printf("\r                                                              ");
                        printf("\rroot:%s$ %s", path , save[turn_num]);

                        strcpy(cmd,save[turn_num]);

                        if(turn_num > 0)
                        {
                            turn_num--;
                        }
                        //break;
                    }
                    else if(key == 66)          //down key
                    {
                        printf("\r                                                              ");
                        printf("\rroot:%s$ %s", path , save[turn_num]);
                        strcpy(cmd,save[turn_num]);
                        if(turn_num < 9)
                        {
                            turn_num++;
                        }
                        //break;
                    }

                }**/
                {
                    history(&j);
                }
                else if(key == 127)		//输入的是Backspace,删除命令字符
                {
                    if(j > 0){
                        cmd[--j] = '\0';

                        printf("\r                                                              ");
                        printf("\rroot:%s$ %s", path , cmd);
                        //printf("%d\n", j);
                    }
                }
                else if(key == 10)	//键入Enter时，保存命令
                {

                    //cmd[j] = (char)key;
                    save_cmd(j);
                    printf("\n");
                    //printf("cmd is %s\n",cmd);
                    //getchar();
                    break;
                }
                else			//保存命令到数组并显示在屏幕
                {

                    //printf("%c",key);
                    putchar(key);
                    cmd[j]=(char)key;
                    j++;
                }
            }

        }

        reset_keypress();  //close this mode
        precompile(cmd);
        fflush(stdin);
    }
}
