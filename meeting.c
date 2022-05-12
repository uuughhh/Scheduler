#ifndef MEETING_H
#define MEETING_H

#include "GlobalVariables.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

// Structure for holding meeting
struct Meeting {
    struct Meeting* next;
    struct Meeting* prev;

    struct Team* team;
    struct tm time;
    int duration;
    int status;
    int index;
};
 
// Converts meeting to a string (formatted to be put in an scheduling file)
static void meetingToString (struct Meeting* mtng, char outputString[128])
{
    char * date = (char *)malloc(sizeof(char) * DATE_LENGTH);
    char * startTime = (char *)malloc(sizeof(char) * TIME_LENGTH);
    char * endTime = (char *)malloc(sizeof(char) * TIME_LENGTH);

    struct tm end = mtng->time;

    end.tm_hour += mtng->duration;

    strftime(date, DATE_LENGTH, "%Y-%m-%d", &mtng->time);
    strftime(startTime, TIME_LENGTH, "%H:%M", &mtng->time);
    strftime(endTime, TIME_LENGTH, "%H:%M", &end);

    snprintf(outputString, FILE_WIDTH, "%*s%*s%*s%*s%*s\n", -DATE_LENGTH, date, -TIME_LENGTH, startTime, -TIME_LENGTH, endTime, -TEAM_NAME_LENGTH, mtng->team->teamName, -PROJECT_NAME_LENGTH, mtng->team->projectName);
    
    free(date);
    free(startTime);
    free(endTime);
    return;
}

// Schedules a new meeting and stores it at the beguinning of the doubly linked list head
struct Meeting * scheduleMeeting(struct Meeting** head, char * teamName, struct tm newTime, int newDuration, int creationIndex) // Schedules a new meeting and inserts it into the doubly linked list in chronological order of schedule
{
    // Create the new meeting
    struct Meeting* newMeeting = (struct Meeting*)malloc(sizeof(struct Meeting));
 
    // Add the information to the new meeting
    newMeeting->team = findTeam(teams, teamName);
    newMeeting->time = newTime;
    newMeeting->duration = newDuration;
    newMeeting->status = 0;
    newMeeting->index = creationIndex;

    newMeeting->next = NULL;
    newMeeting->prev = NULL;

    if(newMeeting->time.tm_hour < FIRST_WORKING_HOUR)
    {
        return NULL;
    }
    if(newMeeting->time.tm_hour + newMeeting->duration > LAST_WORKING_HOUR)
    {
        return NULL;
    }

    // If this is the first meeting, make the head point to it
    if ((*head) == NULL)
    {
        *head = newMeeting;
        return newMeeting;
    }

    // If this meeting is earlier than the earliest meeting, make it the head of the list
    if(difftime(mktime(&newMeeting->time), mktime(&((*head)->time))) < 0)
    {
        newMeeting->next = *head;
        (*head)->prev = newMeeting;
        *head = newMeeting;
        return newMeeting;
    }

    // Look for where the meeting should fit in the list
    struct Meeting* lastMeeting = *head;
    while (lastMeeting->next != NULL)
    {
        // If you encounter a meeting whose scheduled time is later than the new meeting, then insert the new meeting before that one
        if(difftime(mktime(&newMeeting->time), mktime(&lastMeeting->time)) < 0)
        {
            newMeeting->next = lastMeeting;
            lastMeeting = lastMeeting->prev;
            newMeeting->prev = lastMeeting;
            lastMeeting->next = newMeeting;
            newMeeting->next->prev = newMeeting;
            return newMeeting;
        }
        lastMeeting = lastMeeting->next;
    }        
    
    // If the new meeting is the latest in the list, append it to the end
    lastMeeting->next = newMeeting;
    newMeeting->prev = lastMeeting;

    return newMeeting;
}

bool CheckEventConflict(struct Meeting* MeetingOne, struct Meeting* MeetingTwo){
    struct tm temp;
    time_t time1 = mktime(&MeetingOne->time);
    time_t time2 = mktime(&MeetingTwo->time);
    //flag is for second time
    int * flag=0; 
    //chack is for identifying whether teams have common memberss
    bool check = false;

        //check whether the teams have the same members
    for(int i=0; i<EMPLOYEES_PER_TEAM; i++)
    {
        for(int j=0; j<EMPLOYEES_PER_TEAM;j++){
            if(!strcmp((MeetingOne->team->members)[i]->employeeName,(MeetingTwo->team->members)[j]->employeeName))
                check=true;
        }
    }   

        //check time overlap
    if(difftime(time1, time2)==0 && check){
        return true;
    }
    //there is conflict 
//considering that meeting two is always later than meeting one (&MeetingOne->time < &MeetingTwo->time)
    else if(check){
        temp = MeetingOne->time;
        temp.tm_hour += MeetingOne->duration;
        flag = &MeetingTwo->time.tm_hour;
        if(temp.tm_hour > *flag)
            return true;
    }
    return false;
}

#endif