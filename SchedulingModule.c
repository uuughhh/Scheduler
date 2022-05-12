#ifndef SCHEDULING_MODULE_H
#define SCHEDULING_MODULE_H

#include "GlobalVariables.h"
#include "meeting.c"
#include <stdio.h>

// Generates a schedule based on algoName
int generateSchedule(char * algoName, struct tm start, struct tm end)
{
    // Allocate memory
    char * fileName = (char *)malloc(sizeof(char) * FILE_NAME_LENGTH);
    char * outputString = (char *)malloc(sizeof(char) * OUTPUT_SIZE);
    char * headerString = (char *)malloc(sizeof(char) * OUTPUT_SIZE);
    char * startDate = (char *)malloc(sizeof(char) * DATE_LENGTH);
    char * endDate = (char *)malloc(sizeof(char) * DATE_LENGTH);

    // Create iterators
    struct Meeting * curMeeting = meetings;
    struct Meeting * lastValidMeeting;
    struct Employee * curEmployee = employees;
    
    // Initialise counters
    int rejectedCounter = 0;

    // Create variables
    FILE * outputFile;
    char * dividerString = "===========================================================================\n";
    time_t startTime = mktime(&start);
    time_t endTime = mktime(&end);
    struct Meeting * firstMeeting;
    struct Meeting * lastMeeting;
    
    // Generate schedule file dinamicaly
    sprintf(fileName, "G20_%s_Sched_%d.txt", algoName, scheduleIndex++);

    // Open schedule file
    outputFile = fopen (fileName, "w");

    // Record starting and ending date in a string
    strftime(startDate, DATE_LENGTH, "%Y-%m-%d", &start);
    strftime(endDate, DATE_LENGTH, "%Y-%m-%d", &end);

    // Create the header string for the tables
    sprintf(headerString, 
        "\n"
        "%*s%*s%*s%*s%*s\n"
        , -DATE_LENGTH, "Date", -TIME_LENGTH, "Start", -TIME_LENGTH, "End", -TEAM_NAME_LENGTH, "Team", -PROJECT_NAME_LENGTH, "Project");
    
    // Create the header
    sprintf(outputString, 
        "*** Project Meeting ***\n"
        "\n"
        "Algorithm used: %s\n"
        "Period: %s to %s\n"
        , algoName, startDate, endDate);

    // Put all the headers and divider in the schedule file
    fputs(outputString, outputFile);
    printf("%s",outputString);
    fputs(headerString, outputFile);
    printf("%s",headerString);
    fputs(dividerString, outputFile);
    printf("%s",dividerString);

    // Find the first meeting that is after the start dates
    while (curMeeting != NULL && (difftime(mktime(&curMeeting->time), startTime) < 0)) {
        curMeeting = curMeeting->next;
    }

    firstMeeting = curMeeting;
    lastValidMeeting = firstMeeting;

    // Iterate though all the meetings in the time span specified and update their status based on the algorithm chosen
    while (curMeeting != NULL && (difftime(mktime(&curMeeting->time), endTime) <= 0)) {
        if(curMeeting == firstMeeting || !CheckEventConflict(lastValidMeeting, curMeeting))
        {
            // Add valid meetings to the schedule
            meetingToString(curMeeting, outputString);
            fputs(outputString, outputFile);
            printf("%s",outputString);
            lastValidMeeting = curMeeting;
            // Accept meeting
            curMeeting->status = 1; 
        } 
        else
        {
            if(strcmp(algoName, "FCFS") == 0)
            {
                if(lastValidMeeting->index < curMeeting->status)
                {
                    lastValidMeeting->status = 2;
                    curMeeting->status = 1;
                    lastValidMeeting = curMeeting;
                }
                else
                {
                    curMeeting->status = 2;
                    rejectedCounter++;
                }
            }
            else if(strcmp(algoName, "ALPH"))
            {
                if(strcmp(lastValidMeeting->team->teamName, curMeeting->team->teamName) > 0)
                {
                    lastValidMeeting->status = 2;
                    curMeeting->status = 1;
                    lastValidMeeting = curMeeting;
                }
                else
                {
                    curMeeting->status = 2;
                    rejectedCounter++;
                }
            }
        }
        curMeeting = curMeeting->next;
    }
    // End the section
    fputs(dividerString, outputFile);
    printf("%s",dividerString);

    lastMeeting = curMeeting;

    curMeeting = firstMeeting;

    // Record meetings per employee
    while (curEmployee != NULL){
        sprintf(outputString, "Staff: %s\n", curEmployee->employeeName);
        fputs(outputString, outputFile);
        printf("%s",outputString);
        fputs(headerString, outputFile);
        printf("%s",headerString);
        fputs(dividerString, outputFile);
        printf("%s",dividerString);
        curMeeting = firstMeeting;
        while (curMeeting != lastMeeting)
        {
            for(int i = 0; i < EMPLOYEES_PER_TEAM; i++)
            {
                if(curMeeting->team->members[i] == curEmployee)
                {
                    meetingToString(curMeeting, outputString);
                    fputs(outputString, outputFile);
                    printf("%s",outputString);
                    break;
                }
            }
            curMeeting = curMeeting->next;
        }
        curEmployee = curEmployee->next;
        fputs(dividerString, outputFile);
        printf("%s",dividerString);
    }

    curMeeting = firstMeeting;
    
    // Create header for rejected meetings    
    sprintf(outputString, 
        "*** Meeting Request – REJECTED ***\n"
        "\n"
        "There are %d requests rejected for the required period.\n"
        , rejectedCounter);

    // Put headers in schedule file
    fputs(outputString, outputFile);
    printf("%s",outputString);
    fputs(headerString, outputFile);
    printf("%s",headerString);
    fputs(dividerString, outputFile);
    printf("%s",dividerString);
    
    // Record rejected meetings
    while (curMeeting != lastMeeting)
    {
        if(curMeeting->status == 2)
        {
            meetingToString(curMeeting, outputString);
            fputs(outputString, outputFile);
            printf("%s",outputString);
        }
        curMeeting = curMeeting->next;
    }

    // Close schedule file
    fclose(outputFile);

    // Free memory
    free(fileName);
    free(outputString);
    free(headerString);
    free(startDate);
    free(endDate);
    return 0;
}

#endif