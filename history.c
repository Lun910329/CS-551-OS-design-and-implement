//
//  history.c
//  proj1
//
//  Created by GavinBan on 2/24/17.
//  Copyright © 2017 GavinBan. All rights reserved.
//

#include "history.h"

//char save[10][20];
char save[10][40];
int savej[10];
int turn_num;

static struct termios stored_settings;




//==========================================================================
// format：	void history(void)
// function：	search history command
// TODO : length BUG
//==========================================================================
void history(int *length)
{
    int key=0;

    //while(1)
    {
        getchar();
        key=getchar();
        if(key == 65)
        {
            printf("\r                                                              ");
            printf("\rroot:%s$ %s", path , save[turn_num]);

            /*	set_keypress();
             if((key = getchar()) == 127)
             {
             save[--turn_num][20] = '\0';
             printf("\r                                                                 ");
             printf("\r[root@localhost %s] ysh>%s",get_current_dir_name(),save[turn_num]);
             }
             reset_keypress();
             */
            strcpy(cmd,save[turn_num]);

            if(turn_num > 0)
            {
                turn_num--;
            }
            //break;
            *length = savej[turn_num];
        }
        else if(key == 66)
        {
            printf("\r                                                              ");
            printf("\rroot:%s$ %s", path , save[turn_num]);
            strcpy(cmd,save[turn_num]);
            if(turn_num < 9)
            {
                turn_num++;
            }
            //break;
            *length = savej[turn_num];
        }

    }
}

//==========================================================================
// format：	void set_keypress(void)
// function：	close note function
//==========================================================================

void set_keypress(void)
{
    struct termios new_settings;

    tcgetattr(0,&stored_settings);

    new_settings = stored_settings;

    /* Disable canonical mode, and set buffer size to 1 byte */
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(0,TCSANOW,&new_settings);
    return;
}

//==========================================================================
// format：	void reset_keypress(void)
// function：	recover note function
//==========================================================================

void reset_keypress(void)
{
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}


//==========================================================================
// format：	int save_cmd(void)
// function：	after enter enter, save the cmd
//==========================================================================
int save_cmd(int length)
{

    int j;
    static int max_num;

    if(cmd[0] != '\0')
    {
        if(max_num == 9)
        {
            j=0;
            while(j<9)
            {
                savej[j] = savej[j+1];
                strcpy(save[j],save[j+1]);
                j++;
            }
        }
        strcpy(save[max_num],cmd);
        savej[max_num] = length;
        max_num++;

        if(max_num == 10)
        {
            max_num=9;
        }
    }
    turn_num = max_num;
    return 0;
}
