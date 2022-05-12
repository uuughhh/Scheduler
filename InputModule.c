#ifndef INPUT_MODULE
#define INPUT_MODULE

#include "GlobalVariables.h"
#include "team.c"
#include "meeting.c"
#include "Employee.c"
#include "OutputModule.c"
#include "SchedulingModule.c"

#include <sys/types.h>
#include <string.h>
#include <time.h>

int mainMenuInput(FILE *);

// Get single line from the stream and copies it into the buffer
int getStrInput(char * buffer, int size, FILE * stream)
{
    char * result = fgets(buffer, size, stream);
    if(result == NULL)
    {
        return -1;
    }
    buffer[strcspn(buffer, "\r\n")] = 0; // Only \n on windows
    return 0;
}

// Input processing to add a single team
int processTeamCreation(char * input)
{
    // Allocate memory for all the strings and pointer that will be passed into the struct Employee
    char * team = (char*)malloc(TEAM_NAME_LENGTH * sizeof(char));
    char * project = (char*)malloc(PROJECT_NAME_LENGTH * sizeof(char));
    char * manager = (char*)malloc(NAME_LENGTH * sizeof(char));
    struct Employee ** members = (struct Employee **)malloc(EMPLOYEES_PER_TEAM * sizeof(struct Employee*));

    // Start reading the input word by word
    char * curInstruction = strtok(input, " ");

    // Go to menu if input is 0
    if(strcmp(curInstruction, "0") == 0)
    {
        return 0;
    }

    // Store the input into the corresponding string/var
    strcpy(team, curInstruction);
    curInstruction = strtok(NULL, " ");

    strcpy(project, curInstruction);
    curInstruction = strtok(NULL, " ");

    for(int i = 0; i < EMPLOYEES_PER_TEAM; i++)
    {
        char * empName = (char*)malloc(NAME_LENGTH * sizeof(char));;
        
        strcpy(empName, curInstruction);
        members[i] = recordEmployee(&employees, empName);
        
        curInstruction = strtok(NULL, " ");
    }

    // TODO add check for success

    // Create the team
    createTeam(&teams, team, project, members);

    // Print feedback
    displayTeamCrationConfirmation(team);

    return 1;
}

// Input processing to add a single meeting
int processMeetingRecording(char * input)
{
    // Allocate memory for string and struct
    char * teamName = (char*)malloc(TEAM_NAME_LENGTH * sizeof(char));
    struct tm * time = (struct tm*)malloc(sizeof(struct tm));
    char * log = (char *)malloc(OUTPUT_SIZE * sizeof(char));

    int duration;
    struct Meeting * newMeeting;

    // Start reading the input word by word
    char * curInstruction = strtok(input, " ");

    // Initialise the struct tm variable so that they have a valid value and do not interfere with calculations after
    time->tm_sec = 0;
    time->tm_wday = 0;
    time->tm_yday = 0;
    time->tm_isdst = -1;

    // Go to menu if input is 0
    if(strcmp(curInstruction, "0") == 0)
    {
        return 0;
    }

    // Store the input into the corresponding string/var
    strcpy(teamName, curInstruction);
    curInstruction = strtok(NULL, " ");

    sscanf(curInstruction, "%d-%d-%d", &(time->tm_year), &(time->tm_mon), &(time->tm_mday));
    curInstruction = strtok(NULL, " ");

    sscanf(curInstruction, "%d:%d", &(time->tm_hour), &(time->tm_min));
    curInstruction = strtok(NULL, " ");

    sscanf(curInstruction, "%d", &duration);

    // To accomodate for the offsets in the struct tm
    time->tm_year -= TM_YEAR_OFFSET;
    time->tm_mon  -= TM_MONTH_OFFSET;

    // Create meeting
    newMeeting = scheduleMeeting(&meetings, teamName, *time, duration, meetingIndex++);

    if(newMeeting != NULL)
    {
        // Log meeting creation
        meetingToString(newMeeting, log);
        appendLog(log);

        // Print feedback
        displayMeetingRecordingConfirmation();
    }

    // Free allocated memory
    free(log);
    return 1;
}

// Input processing for batch input
int processBatchInput(char * fileName) 
{   
    // Go to menu if input is 0
    if(strcmp(fileName, "0") == 0)
    {
        displayFileNotFound(fileName);
        return 0;
    }
    
    // Declare variables
    FILE * fp;
    size_t len = 0;
    char * curInstruction;
    int result=0;

    // Open file
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        return 1;
    }
    
    // TODO handle wrong inputs by checking result
    // Run the file as the input stream through the normal parsing function
    
    
    char input[INPUT_LENGTH];

    while (result!=-1){
        result = getStrInput(input, INPUT_LENGTH, fp);
        if (result!= -1){
            processMeetingRecording(input);
        }
    }
    
    // Close file
    fclose(fp);
    
    return 1;
}

int processOption2c()
{
    return 1;
}

// Proccess input for scheduling
int processScheduling(char * input) 
{
    // Allocate memory for strings/structs
    char * algo = (char*)malloc(ALGO_NAME_LENGTH * sizeof(char));
    struct tm * startDate = (struct tm*)malloc(sizeof(struct tm));
    struct tm * endDate = (struct tm*)malloc(sizeof(struct tm));

    // Start reading the input word by word
    char * curInstruction = strtok(input, " ");

    // Initialise the struct tm variables so that they have a valid value and do not interfere with calculations after
    startDate->tm_sec = 0;
    startDate->tm_min = 0;
    startDate->tm_hour = 0;
    startDate->tm_wday = 0;
    startDate->tm_yday = 0;
    startDate->tm_isdst = -1;

    endDate->tm_sec = 59;
    endDate->tm_min = 59;
    endDate->tm_hour = 23;
    endDate->tm_wday = 0;
    endDate->tm_yday = 0;
    endDate->tm_isdst = -1;

    // Go to menu if input is 0
    if(strcmp(curInstruction, "0") == 0)
    {
        return 0;
    }

    // Store the input into the corresponding string/var
    strcpy(algo, curInstruction);
    curInstruction = strtok(NULL, " ");

    sscanf(curInstruction, "%d-%d-%d", &(startDate->tm_year), &(startDate->tm_mon), &(startDate->tm_mday));
    curInstruction = strtok(NULL, " ");
    
    sscanf(curInstruction, "%d-%d-%d", &(endDate->tm_year), &(endDate->tm_mon), &(endDate->tm_mday));

    // To accomodate for the offsets in the struct tm
    startDate->tm_year -= TM_YEAR_OFFSET;
    startDate->tm_mon -= TM_MONTH_OFFSET;
    endDate->tm_year -= TM_YEAR_OFFSET;
    endDate->tm_mon -= TM_MONTH_OFFSET;

    // Create schedule
    generateSchedule(algo, *startDate, *endDate);

    // Free the temporary variables
    free(startDate);
    free(endDate);
    
    // Print feedback
    displayScheduleGenerationConfirmation();

    return 1;
}

int processOption3c()
{
    printf(">>>>>> YYYY attending algorithm not implemented.\n");
    return 1;
}

int mainMenuInput(FILE * inputStream)
{
    // Create input buffer
    char * input = (char *)malloc(sizeof(char) * INPUT_LENGTH);
    
    getStrInput(input, INPUT_LENGTH, inputStream);
        
    // Quit if input is 4
    if(strcmp(input, "4") == 0)
    {
        return 8;
    }
    
    // Display input prompt if the input is the standard input
    if(inputStream == stdin)
    {
        displayPompt();
    }

    // Parse command
    if(strcmp(input, "1") == 0)
    {
        getStrInput(input, INPUT_LENGTH, inputStream);
        processTeamCreation(input);
        return 1;
    }
    else if(strcmp(input, "2a") == 0)
    {

        getStrInput(input, INPUT_LENGTH, inputStream);
        processMeetingRecording(input);
        return 2;
    }
    else if(strcmp(input, "2b") == 0)
    {
        getStrInput(input, INPUT_LENGTH, inputStream);
        processBatchInput(input);
        return 3;
    }
    else if(strcmp(input, "2c") == 0)
    {
        getStrInput(input, INPUT_LENGTH, inputStream);
        processOption2c();
        return 4;
    }
    else if(strcmp(input, "3a") == 0)
    {
        getStrInput(input, INPUT_LENGTH, inputStream);
        processScheduling(input);
        return 5;
    }
    else if(strcmp(input, "3b") == 0)
    {
        getStrInput(input, INPUT_LENGTH, inputStream);
        processScheduling(input);
        return 6;
    }
    else if(strcmp(input, "3c") == 0)
    {
        getStrInput(input, INPUT_LENGTH, inputStream);
        processOption3c();
        return 7;
    }
    else
    {
        printf("Error: command not recognized\n");
        return -1;
    }

    // Free input buffer
    free(input);
}

#endif