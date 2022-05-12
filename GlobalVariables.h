#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>


extern struct Team * teams;
extern struct Meeting * meetings;
extern struct Employee * employees;

extern const int INPUT_LENGTH;
extern const int TEAM_NAME_LENGTH;
extern const int PROJECT_NAME_LENGTH;
extern const int NAME_LENGTH;
extern const int FILE_NAME_LENGTH;
extern const int ALGO_NAME_LENGTH;
extern const int DATE_LENGTH;
extern const int TIME_LENGTH;
extern const int FILE_WIDTH; // 76
extern const int EMPLOYEES_PER_TEAM;
extern const int OUTPUT_SIZE;
extern const int TM_YEAR_OFFSET;
extern const int TM_MONTH_OFFSET;
extern const int FIRST_WORKING_HOUR;
extern const int LAST_WORKING_HOUR;

extern int meetingIndex;
extern int scheduleIndex;

//pointers to head of the existing items list
static struct Team **team_head;
static struct Meeting **meet_head;
static struct Employee **empl_head;

//pointers to head of the free items list
static struct Team **free_team;
static struct Meeting **free_meet;
static struct Employee **free_empl;

//pointer to mutex in each list;
static pthread_mutex_t *mutex_team;
static pthread_mutex_t *mutex_meet;
static pthread_mutex_t *mutex_empl;

#endif