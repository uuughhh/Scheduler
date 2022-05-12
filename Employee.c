#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "GlobalVariables.h"
#include <stdio.h>
#include <string.h>

struct Employee {
    struct Employee* next;
    struct Employee* prev;

    char * employeeName;
};

struct Employee * recordEmployee(struct Employee** head, char * newName) 
{
    struct Employee* newEmployee = (struct Employee*)malloc(sizeof(struct Employee));
    
    newEmployee->employeeName = newName;

    newEmployee->next = NULL;
    newEmployee->prev = NULL;
 
    if ((*head) == NULL)
    {
        *head = newEmployee;
        return newEmployee;
    }

    struct Employee* curEmployee = *head;
    while (curEmployee->next != NULL)
    {
        if(strcmp(curEmployee->employeeName, newName) == 0)
        {
            return curEmployee;
        }
        curEmployee = curEmployee->next;
    }        
    
    if(strcmp(curEmployee->employeeName, newName) == 0)
    {
        return curEmployee;
    }
    
    curEmployee->next = newEmployee;
    newEmployee->prev = curEmployee;
    return newEmployee;
}
#endif