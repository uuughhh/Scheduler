#ifndef TEAM_H
#define TEAM_H

#include "GlobalVariables.h"
#include "Employee.c"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct Team {
    struct Team* next;
    struct Team* prev;

    char * teamName;
    char * projectName;
    struct Employee ** members;
};

// Converts meeting to a string (formatted to be put in an scheduling file)
static void teamToString(struct Team* tm, char * outputString) 
{
    snprintf(outputString, FILE_WIDTH, "%*s%*s%*s%*s%*s%*s\n", -PROJECT_NAME_LENGTH, tm->projectName, -TEAM_NAME_LENGTH, tm->teamName, -NAME_LENGTH, tm->members[0]->employeeName, -NAME_LENGTH, tm->members[1]->employeeName, -NAME_LENGTH, tm->members[2]->employeeName, tm->members[3]->employeeName);
    return;
}

// Checks if a team is legal
// bool isLegalTeam(newTeam)
// {
//     struct Team * curTeam = teams;

//     int teamParticipationCounter[EMPLOYEES_PER_TEAM + 1];

//     while(curTeam != NULL)
//     {
//         if (strcmp(curTeam->managerName, newTeam->managerName))
//         {
//             return false;
//         }
        
//         for(int i = 0; i < EMPLOYEES_PER_TEAM)
//         {
//             for(int j = 0; i < EMPLOYEES_PER_TEAM)
//             {
            
//             }
//         }
//     }
// }

// Create new team and adds it at the start of the team list
struct Team * createTeam(struct Team** head, char * newTeamName, char * newProjectName, struct Employee ** newMembers)
{
    // Allocate memory for new team
    struct Team* newTeam = (struct Team*)malloc(sizeof(struct Team));
  
    // Store the new data in the new team
    newTeam->teamName = newTeamName;
    newTeam->projectName = newProjectName;
    newTeam->members = newMembers;
    
    // if(!isLegalTeam(newTeam))
    // {
    //     return NULL;
    // }

    newTeam->next = (*head);
    newTeam->prev = NULL;
    
    // If the head is not null link the head after the new meeting
    if ((*head) != NULL)
        (*head)->prev = newTeam;
    

    // Make the new team the head of the list
    (*head) = newTeam;
    return newTeam;
}

// Returns pointer to the the team with name string
static struct Team* findTeam(struct Team* head, char * tmName)
{
    struct Team* prev;
    while (head != NULL) {
        if(strcmp(head->teamName, tmName) == 0)
        {
            return head;
        }
        prev = head;
        head = head->next;
    }
    return NULL;
}

#endif
