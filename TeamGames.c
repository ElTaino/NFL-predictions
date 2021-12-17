#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//PRELUDE
/*
I wanted to get closer to the machine, so I decided to learn C. This is the very first project, so if anyone comes across this
please feel free to make any changes. I'm always open to learning. I'm also learning x86 assembly if anyone has any advice.
*/

//QUICK SUMMARY
/*
    Organizes the weekly NFL schedules into team specific schedules that contains the ID for the game on ESPN.COM. New files will be created,
    so they can utilized later to gather stats quickly when we want to analyze a team and it's schedule.
*/

//LONG SUMMARY
/*
Last Change: 12/16/2021

There are files saved in "WebScraping/WeeklySchedule/" as Week1.txt, Week2.txt, etc. These are the NFL schedules for each week with
the ESPN ID for the game. For example, one line in this file could be:
     NYJ,NE,401326354

That means the New England Patriots played at the New York Jets. The number 401326354 is the key that links to the game on ESPN.com
https://www.espn.com/nfl/game/_/gameId/401326354

The code here will open the 18 files, set a stack for each of the teams in the line, then set the ESPN ID as the top element in the stack.
This means that at the end of the 18 files each of the 32 stacks will have 17 integers (The ESPN IDs). Then we pop each stack until it's
empty and then create the new file with the team abreviation containing all of the IDs.

Example:
NYJ.txt
401326354
401111111
401222222
401333333
etc.
*/

struct Stack {
    int top;
    int array[17];
};

struct Stack* createStack()
{
    struct Stack* stack = malloc(sizeof(struct Stack));
    stack->top = -1;
    return stack;
}

// Full when the top is the end of allocated memory
int isFull(struct Stack* check)
{
    return check->top == 17 - 1;
}

// Empty when the top is set to -1
int isEmpty(struct Stack* check)
{
    return check->top == -1;
}

// Adds an item to the stack
void push(struct Stack* stack, int item)
{
    if (isFull(stack)) return;
    stack->array[++stack->top] = item;
}

// Pops da stackk
// Question: Is it better to allocate this memory in heap since it will be increasing to a size of 17 and then decreasing to a size of 0?
int pop(struct Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top--];
}

// Looks at the top of the stack
int peek(struct Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top];
}

void OpenFiles(int week);
char* getTeam(const int teamKey);
struct Stack* list[32];

int main(int argc, char* argv[])
{
    for (int week = 1; week <= 18; week++) {
        OpenFiles(week);
    }

    for (int teamKey = 0; teamKey <= 31; teamKey++) {
        char* team = getTeam(teamKey);
        int pathLength = sizeof("TeamSchedule/.txt");
        pathLength += sizeof(sizeof(char) * strlen(team));

        char* path = malloc(sizeof(char) * (pathLength + 1));
        strcpy_s(path, sizeof(char) * (pathLength + 1), "TeamSchedule/");
        strcat_s(path, sizeof(char) * (pathLength + 1), team);
        strcat_s(path, sizeof(char) * (pathLength + 1), ".txt");

        FILE* ptrFile;
        fopen_s(&ptrFile, path, "w");
        free(path);

        while (list[teamKey]->top != -1) {
            fprintf(ptrFile, "%d\n", list[teamKey]->array[list[teamKey]->top]);
            pop(list[teamKey]);
        }

        if (ptrFile != NULL) {
            fclose(ptrFile);
        }
        printf("Team - %s \n", team);
    }
    //fwrite()
    return 0;
}



void OpenFiles(int week) {

    int pathLength = sizeof("WebScraping/WeeklySchedule/Week.txt");

    if (week >= 10) {
        pathLength += 1;
    }

    //Create the file path
    char* path = malloc(sizeof(char) * (pathLength + 1));
    char* strWeek = malloc(sizeof(char) * (3));
    sprintf_s(strWeek, sizeof(char) * (3), "%d", week);
    strcpy_s(path, sizeof(char) * (pathLength + 1), "WebScraping/WeeklySchedule/Week");
    strcat_s(path, sizeof(char) * (pathLength + 1), strWeek);
    strcat_s(path, sizeof(char) * (pathLength + 1), ".txt");

    FILE* ptrFile;
    fopen_s(&ptrFile, path, "r");
    free(path);
    free(strWeek);

    char strLine[20];
    char* context = NULL;
    while (ptrFile != NULL && fgets(strLine, sizeof(strLine), ptrFile)) {

        char* chunk;
        chunk = strtok_s(strLine, ",", &context);

        int homeCode = -1;
        int awayCode = -1;
        int gameId;
        struct Stack* home;
        while (chunk != NULL) {

            //Line for the file is Ex. NYJ,PAT,49182392 NYJ = 0, PAT = 1, 49182392 = 3
            //First option in the comma separated list is the home team.
            if (homeCode == -1) {
                homeCode = hash(chunk);
                if (list[homeCode] == NULL) {
                    list[homeCode] = createStack();
                }
            }
            //Second option is the away team
            else if (awayCode == -1) {
                awayCode = hash(chunk);
                if (list[awayCode] == NULL) {
                    list[awayCode] = createStack();
                }
            }
            else
            {
                gameId = strtol(chunk, NULL, 10);
            }
            chunk = strtok_s(NULL, ",", &context);
        }

        push(list[homeCode], gameId);
        push(list[awayCode], gameId);
        //printf("%d pushed to stack for team #'s %d, %d \n", gameId, homeCode, awayCode);
    }

    if (ptrFile != NULL) {
        fclose(ptrFile);
    }
}

char* getTeam(const int teamKey)
{
    if (teamKey == 0)
    {
        char* team = malloc(sizeof(char) * (strlen("NE\0")));
        team = "NE\0";
        return team;
    }
    else if (teamKey == 1)
    {
        char* team = malloc(sizeof(char) * (strlen("BUF\0")));
        team = "BUF\0";
        return team;
    }
    else if (teamKey == 2)
    {
        char* team = malloc(sizeof(char) * (strlen("NYJ\0")));
        team = "NYJ\0";
        return team;
    }
    else if (teamKey == 3)
    {
        char* team = malloc(sizeof(char) * (strlen("MIA\0")));
        team = "MIA\0";
        return team;
    }
    else if (teamKey == 4)
    {
        char* team = malloc(sizeof(char) * (strlen("KC\0")));
        team = "KC\0";
        return team;
    }
    else if (teamKey == 5)
    {
        char* team = malloc(sizeof(char) * (strlen("LAC\0")));
        team = "LAC\0";
        return team;
    }
    else if (teamKey == 6)
    {
        char* team = malloc(sizeof(char) * (strlen("DEN\0")));
        team = "DEN\0";
        return team;
    }
    else if (teamKey == 7)
    {
        char* team = malloc(sizeof(char) * (strlen("LV\0")));
        team = "LV\0";
        return team;
    }
    else if (teamKey == 8)
    {
        char* team = malloc(sizeof(char) * (strlen("BAL\0")));
        team = "BAL\0";
        return team;
    }
    else if (teamKey == 9)
    {
        char* team = malloc(sizeof(char) * (strlen("CLE\0")));
        team = "CLE\0";
        return team;
    }
    else if (teamKey == 10)
    {
        char* team = malloc(sizeof(char) * (strlen("CIN\0")));
        team = "CIN\0";
        return team;
    }
    else if (teamKey == 11)
    {
        char* team = malloc(sizeof(char) * (strlen("PIT\0")));
        team = "PIT\0";
        return team;
    }
    else if (teamKey == 12)
    {
        char* team = malloc(sizeof(char) * (strlen("TEN\0")));
        team = "TEN\0";
        return team;
    }
    else if (teamKey == 13)
    {
        char* team = malloc(sizeof(char) * (strlen("IND\0")));
        team = "IND\0";
        return team;
    }
    else if (teamKey == 14)
    {
        char* team = malloc(sizeof(char) * (strlen("HOU\0")));
        team = "HOU\0";
        return team;
    }
    else if (teamKey == 15)
    {
        char* team = malloc(sizeof(char) * (strlen("JAX\0")));
        team = "JAX\0";
        return team;
    }
    else if (teamKey == 16)
    {
        char* team = malloc(sizeof(char) * (strlen("DAL\0")));
        team = "DAL\0";
        return team;
    }
    else if (teamKey == 17)
    {
        char* team = malloc(sizeof(char) * (strlen("WSH\0")));
        team = "WSH\0";
        return team;
    }
    else if (teamKey == 18)
    {
        char* team = malloc(sizeof(char) * (strlen("PHI\0")));
        team = "PHI\0";
        return team;
    }
    else if (teamKey == 19)
    {
        char* team = malloc(sizeof(char) * (strlen("NYG\0")));
        team = "NYG\0";
        return team;
    }
    else if (teamKey == 20)
    {
        char* team = malloc(sizeof(char) * (strlen("ARI\0")));
        team = "ARI\0";
        return team;
    }
    else if (teamKey == 21)
    {
        char* team = malloc(sizeof(char) * (strlen("LAR\0")));
        team = "LAR\0";
        return team;
    }
    else if (teamKey == 22)
    {
        char* team = malloc(sizeof(char) * (strlen("SF\0")));
        team = "SF\0";
        return team;
    }
    else if (teamKey == 23)
    {
        char* team = malloc(sizeof(char) * (strlen("SEA\0")));
        team = "SEA\0";
        return team;
    }
    else if (teamKey == 24)
    {
        char* team = malloc(sizeof(char) * (strlen("GB\0")));
        team = "GB\0";
        return team;
    }
    else if (teamKey == 25)
    {
        char* team = malloc(sizeof(char) * (strlen("MIN\0")));
        team = "MIN\0";
        return team;
    }
    else if (teamKey == 26)
    {
        char* team = malloc(sizeof(char) * (strlen("CHI\0")));
        team = "CHI\0";
        return team;
    }
    else if (teamKey == 27)
    {
        char* team = malloc(sizeof(char) * (strlen("DET\0")));
        team = "DET\0";
        return team;
    }
    else if (teamKey == 28)
    {
        char* team = malloc(sizeof(char) * (strlen("TB\0")));
        team = "TB\0";
        return team;
    }
    else if (teamKey == 29)
    {
        char* team = malloc(sizeof(char) * (strlen("ATL\0")));
        team = "ATL\0";
        return team;
    }
    else if (teamKey == 30)
    {
        char* team = malloc(sizeof(char) * (strlen("NO\0")));
        team = "NO\0";
        return team;
    }
    else if (teamKey == 31)
    {
        char* team = malloc(sizeof(char) * (strlen("CAR\0")));
        team = "CAR\0";
        return team;
    }

    // assign a number to the first char of buffer from 0-25
    return -1;
}

int hash(const char* buffer)
{
    if (strcmp(buffer, "NE") == 0)
    {
        return 0;
    }
    else if (strcmp(buffer, "BUF") == 0)
    {
        return 1;
    }
    else if (strcmp(buffer, "NYJ") == 0)
    {
        return 2;
    }
    else if (strcmp(buffer, "MIA") == 0)
    {
        return 3;
    }
    else if (strcmp(buffer, "KC") == 0)
    {
        return 4;
    }
    else if (strcmp(buffer, "LAC") == 0)
    {
        return 5;
    }
    else if (strcmp(buffer, "DEN") == 0)
    {
        return 6;
    }
    else if (strcmp(buffer, "LV") == 0)
    {
        return 7;
    }
    else if (strcmp(buffer, "BAL") == 0)
    {
        return 8;
    }
    else if (strcmp(buffer, "CLE") == 0)
    {
        return 9;
    }
    else if (strcmp(buffer, "CIN") == 0)
    {
        return 10;
    }
    else if (strcmp(buffer, "PIT") == 0)
    {
        return 11;
    }
    else if (strcmp(buffer, "TEN") == 0)
    {
        return 12;
    }
    else if (strcmp(buffer, "IND") == 0)
    {
        return 13;
    }
    else if (strcmp(buffer, "HOU") == 0)
    {
        return 14;
    }
    else if (strcmp(buffer, "JAX") == 0)
    {
        return 15;
    }
    else if (strcmp(buffer, "DAL") == 0)
    {
        return 16;
    }
    else if (strcmp(buffer, "WSH") == 0)
    {
        return 17;
    }
    else if (strcmp(buffer, "PHI") == 0)
    {
        return 18;
    }
    else if (strcmp(buffer, "NYG") == 0)
    {
        return 19;
    }
    else if (strcmp(buffer, "ARI") == 0)
    {
        return 20;
    }
    else if (strcmp(buffer, "LAR") == 0)
    {
        return 21;
    }
    else if (strcmp(buffer, "SF") == 0)
    {
        return 22;
    }
    else if (strcmp(buffer, "SEA") == 0)
    {
        return 23;
    }
    else if (strcmp(buffer, "GB") == 0)
    {
        return 24;
    }
    else if (strcmp(buffer, "MIN") == 0)
    {
        return 25;
    }
    else if (strcmp(buffer, "CHI") == 0)
    {
        return 26;
    }
    else if (strcmp(buffer, "DET") == 0)
    {
        return 27;
    }
    else if (strcmp(buffer, "TB") == 0)
    {
        return 28;
    }
    else if (strcmp(buffer, "ATL") == 0)
    {
        return 29;
    }
    else if (strcmp(buffer, "NO") == 0)
    {
        return 30;
    }
    else if (strcmp(buffer, "CAR") == 0)
    {
        return 31;
    }

    // assign a number to the first char of buffer from 0-25
    return -1;
}