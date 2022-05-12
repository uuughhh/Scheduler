#ifndef OUTPUT_MODULE_H
#define OUTPUT_MODULE_H

#include "GlobalVariables.h"
#include "team.c"
#include "meeting.c"

#include <stdio.h>
#include <stdlib.h>

// Displays the main menu
void displayMainMenu () 
{
    printf("\n~~ WELCOME TO PolyStar ~~\n"
            "\n"
            "1. Create Project Team\n"
            "\n"
            "2. Project Meeting Request\n"
            "\t2a. Single input\n"
            "\t2b. Batch input\n"
            "\n"
            "3. Print Meeting Schedule\n"
            "\t3a. FCFS (First Come First Serverd)\n"
            "\t3b. ALPH (Alphabetical)\n"
            "\n"
            "4. Exit\n"
            "\n"
            "Enter an option: \n");
}

// Displays prompt for input
void displayPompt() 
{
    printf("Enter> ");
}

// Displays a divider
void displayDivider()
{
    printf("===========================================================================\n");
}

// Displays feedback for sucessfully adding a team
void displayTeamCrationConfirmation(char * team)
{
    printf(">>>>>> Project %s is created.\n", team);
}

// Displays feedback for sucessfully recording a meeting
void displayMeetingRecordingConfirmation()
{
    printf(">>>>>> Meeting is recorded.\n");
}

// Displays feedback for sucessfully generating the schedule meeting
void displayScheduleGenerationConfirmation()
{
    printf(">>>>>> Scheduling file generated.\n");
}

// Displays feedback for unsucessful file opening
void displayFileNotFound(char * fileName)
{
    printf("File %s not found\n", fileName);
}


// Prints all the meetings stored in the list
void printMeetings(struct Meeting* head) 
{
    printf("\n%*s%*s%*s%*s%*s\n", -DATE_LENGTH, "Date", -TIME_LENGTH, "Start", -TIME_LENGTH, "End", -TEAM_NAME_LENGTH, "Team", -PROJECT_NAME_LENGTH, "Project");
    displayDivider();
    
    char output[128];
    while (head != NULL) 
    {
        meetingToString(head, output);
        printf( "%s", output);
        head = head->next;
    }
}

// Prints all the teams stored in the list
static void printTeams(struct Team* head)
{
    printf("\n%*s%*s%*s%*s%*s%*s\n", -PROJECT_NAME_LENGTH, "Project Name", -TEAM_NAME_LENGTH, "Team Name", -NAME_LENGTH, "Manager Name", -NAME_LENGTH, "Member 1", -NAME_LENGTH, "Member 2", -NAME_LENGTH, "Member 3");
    displayDivider();
    
    char output[128];
    while (head != NULL) 
    {
        teamToString(head, output);
        printf("%s",output);
        head = head->next;
    }
}

// Append new line to the log
int appendLog(char * string)
{
    FILE * outputFile;
    
    outputFile = fopen ("log.bat", "a");

    fputs(string, outputFile);

    fclose(outputFile);

    return 0;
}

static 
#endif