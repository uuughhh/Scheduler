#ifndef PMS_H
#define PMS_H

#include "InputModule.c"
#include "OutputModule.c"
#include "GlobalVariables.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/wait.h>

#define MAX_LIST_SZ 1024



// Defining globale variables
struct Team * teams = NULL;
struct Meeting * meetings = NULL;
struct Employee * employees = NULL;

// TODO Optimise length so that it uses less memory
const int INPUT_LENGTH = 258;
const int TEAM_NAME_LENGTH = 14;
const int PROJECT_NAME_LENGTH = 28;
const int NAME_LENGTH = 16;
const int FILE_NAME_LENGTH = 64;
const int ALGO_NAME_LENGTH = 5;
const int DATE_LENGTH = 15;
const int TIME_LENGTH = 8;
const int FILE_WIDTH = 128; // 76
const int EMPLOYEES_PER_TEAM = 4;
const int OUTPUT_SIZE = 512;
const int TM_YEAR_OFFSET = 1900;
const int TM_MONTH_OFFSET = 1;
const int FIRST_WORKING_HOUR = 9;
const int LAST_WORKING_HOUR = 18;

int meetingIndex = 0;
int scheduleIndex = 0;

int main(int argc, char const *argv[])
{   
    //pointer to memory mapping
    void *ptr_team;
    void *ptr_meet;
    //@void *ptr_employee;

    //size of the mapping memory
    size_t team_sz=0;
    size_t meet_sz=0;
    //@size_t empl_sz=0;
    
    //Space for the teams nodes
    team_sz += sizeof(**team_head)*MAX_LIST_SZ;
    //Space for house-keeping pointers
    team_sz += sizeof(team_head)+sizeof(free_team);
    //Space for the mutex
    team_sz += sizeof(*mutex_team);
    ptr_team = mmap(NULL, team_sz, PROT_READ | PROT_WRITE , MAP_SHARED | MAP_ANONYMOUS , -1, 0);
    if (ptr_team == MAP_FAILED) {
        perror("mmap(2) failed");
        exit(EXIT_FAILURE);
    }

    //initialize head pointers
    mutex_team = ptr_team;
    free_team = (struct Team **) (((char *) ptr_team)+sizeof(*mutex_team));
    team_head = free_team+1;

    *free_team = (struct Team *) (team_head+1);
    *team_head = NULL;

    /* Initialize free list */
    int i;
    struct Team *curr;
    for (i=0, curr = *free_team; i < MAX_LIST_SZ-1; i++, curr++) {
        if (i==0){ curr->prev = NULL;}
        curr->next = curr+1;
        curr->next->prev = curr;
    }
    curr->next = NULL;

    //initialize mutex
    pthread_mutexattr_t mutex_attr_team;
    if (pthread_mutexattr_init(&mutex_attr_team) < 0) {
        perror("Failed to initialize mutex attributes for teams");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutexattr_setpshared(&mutex_attr_team, PTHREAD_PROCESS_SHARED) < 0) {
        perror("Failed to change mutex attributes for teams");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(mutex_team, &mutex_attr_team) < 0) {
        perror("Failed to initialize mutex of team");
        exit(EXIT_FAILURE);
    }

    int childNum = 3;
    int isRunning = 0;
    //fd[0]-input write pipes fd[1]-output write pipes fd[2]-scheduler fd[3]-pms
    int pipeFD[4][3][2];
    char buf[20];
    int readLen = 0;

    //create pipes
    for (int x=0; x<4; x++){
        for (int y=0; y<3; y++){
            if (pipe(pipeFD[x][y])<0){
                printf ("pipe creation error at [%d][%d]\n",x,y);
                exit(1);
            }
        }
    }

    for (int curChild=0; curChild<childNum; curChild++){
        int childID = fork();
        if (childID < 0){
            printf ("Fork failed at %d\n",curChild);
        } else if (childID==0){                       //enter child process
            for (int a=0; a<4; a++){
                for (int b=0; b<3; b++){
                    if (a==curChild){
                        close(pipeFD[a][b][0]);           //close read end for curChild write pipe
                    } else {
                        close(pipeFD[a][b][1]);           //close write end of other pipes
                        if (b==2 && curChild!=2) 
                        {close(pipeFD[a][b][0]);}         //close read ends of pms reading pipes
                    }
                }
            }
            switch (curChild)
            {

            case 0:                                 //child for input module
                //close other unused read end
                for (int c=1; c<4; c++){
                    close(pipeFD[c][1][0]);
                }

                //start loop
                while (isRunning!=8){
                    readLen = read(pipeFD[3][0][0],buf,20);
                    if (readLen>0 && (buf[0]=='s')){        //get start signal from parent
                        buf[readLen] = 0;
                        isRunning = mainMenuInput(stdin);        //start input module
                        sprintf(buf,"%d",isRunning);
                        write(pipeFD[curChild][2][1],buf,20);   //send the running signal to parent
                    }
                }

                //close all pipes and exit
                close(pipeFD[curChild][0][1]);                  //close write ends
                close(pipeFD[curChild][1][1]);
                close(pipeFD[curChild][2][1]);
                for (int d=1; d<4; d++){            //close read ends
                    close(pipeFD[d][0][0]);
                }
                exit(0);
                break;
            
            case 1:                                 //child for output module
                //close other unused read end
                for (int c=0; c>4; c++){
                    if (c!=curChild){
                        if (c!=0){ close(pipeFD[c][0][0]); } 
                        else { close (pipeFD[c][1][0]);}
                    }
                }
                
                //start loop
                while (isRunning!=8){
                    readLen = read(pipeFD[3][1][0],buf,20);
                    if (readLen>0 ){
                        buf[readLen]=0;
                        if (buf[0]=='s'){            //get start signal from parent
                            buf[readLen] = 0;
                            displayMainMenu();
                        } else if (buf[0]=='8'){
                            isRunning = 8;       //end message received if buf = '8'
                        }
                    }
                    /*
                    readLen = read(pipeFD[3][1][0],buf,20);
                    if (readLen>0 ){
                        buf[readLen]=0;
                        if (buf[0]=='8'){
                            isRunning = 8;       //end message received if buf = '8'
                        }
                    }*/
                    
                }

                //close all pipes and exit
                close(pipeFD[curChild][0][1]);          //close write ends
                close(pipeFD[curChild][1][1]);
                close(pipeFD[curChild][2][1]);
                for (int d=0; d>4; d++){            //close read ends
                    if (d!=1){
                        if (d!=0){ close(pipeFD[d][1][0]); } 
                        else { close (pipeFD[d][0][0]);}
                    }
                }
                exit(0);
                break;

           
            case 2:                                 //child for scheduling module
                //close other unused read end
                for (int c=0; c>4; c++){
                    if (c!=curChild){
                        if (c!=3){ 
                            close(pipeFD[c][0][0]);
                            close(pipeFD[c][2][0]);
                        } else { 
                            close (pipeFD[c][0][0]);
                            close (pipeFD[c][1][0]);
                        }
                    }
                }

                //start loop
                while (isRunning!=8){
                    readLen = read(pipeFD[3][2][0],buf,20);
                    if (readLen>0 ){
                        buf[readLen]=0;
                        isRunning = atoi(buf);//end message received if buf = '8'
                    }
                }

                //close all pipes and exit
                close(pipeFD[curChild][0][1]);          //close write ends
                close(pipeFD[curChild][1][1]);
                close(pipeFD[curChild][2][1]);
                close(pipeFD[0][1][0]);                 //close read ends
                close(pipeFD[1][1][0]);
                close(pipeFD[3][2][0]);
                exit(0);
                break;
            }

        }
    }


    //close unused pipes for parent process
    for (int a=0; a<4; a++){
        for (int b=0; b<3; b++){
            if (a!=3){
                close(pipeFD[a][b][1]);                    //close unused write ends
                if (b!=2){close(pipeFD[a][b][0]);}         //close unused read ends
            } else {
                close(pipeFD[a][b][0]);                    //close pms writing pipe's read end
            }
        }
    }
    
    while (isRunning != 8)
    {
        buf[0]='s';
        //send start signal to output, input to print menu and receive input
        write(pipeFD[3][1][1],buf,20);
        write(pipeFD[3][0][1],buf,20);
        readLen = read(pipeFD[0][2][0],buf,20);
        if (readLen>0){
            buf[readLen]=0;
            isRunning = atoi(buf);          //read signal from input, see if end loop
        }
        switch (isRunning)
        {
        case 1:
            //buf[0] = 't'; //signal for output module to print teams
            //write(pipeFD[3][1][1],buf,20);
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            //attendance?
            break;
        case 5:
            //tell schedule module to schedule FCFS
            //buf[0]='5';
            //write(pipeFD[3][2][1],buf,20);
            break;
        case 6:
            break;
        case 8:
            //tell all child to end
            buf[0]='8';
            write(pipeFD[3][2][1],buf,20);
            write(pipeFD[3][1][1],buf,20);
            break;
        default:
            break;
        }
    }

    //close all pipes -> wait all children -> exit
    for (int c=0; c<4; c++){
        if (c==3){
            close(pipeFD[c][0][1]);
            close(pipeFD[c][1][1]);
            close(pipeFD[c][2][1]);
        } else {
            close (pipeFD[c][2][0]);
        }
    }
    for (int d=0; d<3; d++){
        wait(NULL);
    }
    exit(0);

    return 0;

}

#endif