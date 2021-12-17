#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
//Predicting the scores of NFL games
//1. Save information in text file for averages per team (offense, defense)
//2. Info on each team, the schedule they played.
//3. Analyze the weight to give to the team based on the strength of the teams.
//...Ex. Jets average 4.2 per carry, but played defenses giving up on average 3.5 opposed to a team that
//...... average 4.6 per carry, but played defenses giving up on average 5.5.
//4. Use the weight to adjust the info from file(s) from #1 in this list.
//5. Put the 2 teams up to each other (or all games that week), then compare and adjust based on their strengths.
//.... Ex. on average the jets only allowed 80% of teams normal yard per rush or w/e.
//6. Ask user if there is a specific portion that should determine output (passing yards more heavily favored for a matchup).
//7. Ask user about different weights for each section (rush, pass, rush d, pass d, etc.) to do weekly weights
//... ex. Kamara out for the saints severely reduces the saints output.
//Extra extra: Maybe split between home and away to analyze that also.

/*
ENUM of strength
*/
struct Team {
    //PREGUNTA - Uso el heap por el tamano del abrev del equipo? Puede ser 2 or 3 caracteres. NYJ TB por ejemplo.
    //PREGUNTA - Debo darle uno mas? Para el NULL pointer?
    char name[4];
    struct TeamStats* offense;
    struct TeamStats* defense;
    int dataSet;
};

struct TeamStats {
    char name[4];
    double yardsPerGame, ptsPerGame;

    //Pass - Lo uso para ambos offense y defense.
    double yardsPerPass, passYpg, passerRating;
    int tdPasses, interceptions, sacks;

    //Rush - Lo uso para ambos offense y defense.
    double yardsPerCarry, rushYpg;
    int tdRushes, fumbles;

    //PREGUNTA - Tal vez yo ponga esto para heridas y algunas cosas. COVID, etc.
    //char strength;
};

//Weights que uso para cambiar los stats del equipo. 
//Pq si dos equipos tengo 5.5 yards per carry, pero uno tiene un horario loco... Me entiende, uno es mejor de los dos.
struct Weights {
    double score, yardsPerCarry, yardsPerPass, passerRating;
};

struct Game {
    int week;
    struct Team* homeTeam;
    struct Team* awayTeam;
    struct GameStats* homeStats;
    struct GameStats* awayStats;
};

struct GameStats {
    char name[4];
    int week, score;
    double yardsPerCarry, yardsPerPass; 
};

//Verifique que el equipo sale del archivo para la semana.
void VerifyEntry(struct Game* match, struct Team* mainTeam, struct Team* oppTeam);
void GetYearlyStats(struct Game* match, char* filePath, int rushOrPass, int offOrDef);
void AddTeamToMatch(struct Game* match, struct Team* homeTeam, struct Team* awayTeam);
void SetStats(struct TeamStats* team, char* lineCopy, int rushOrPass);

int main1(int argc, char* argv[])
{
    struct Game match;
    struct Team mainTeam, oppTeam;
    mainTeam.dataSet = 0, oppTeam.dataSet = 0;

    strcpy_s(oppTeam.name, sizeof(char[4]), "def");

    //Get a team from the matchup the person wants to fuggin see.
    printf("What is the team abreviation?\n");
    scanf_s("%s", mainTeam.name, (unsigned)sizeof(mainTeam.name));
    printf("\n\nWhat week? Ex: \'15\' for week 15\n");
    scanf_s("%d", &match.week);

    VerifyEntry(&match, &mainTeam, &oppTeam);

    if (strcmp(oppTeam.name, "def") == 0) {
        printf("Team not found: %s\n", mainTeam.name);
        printf("HOME: %s\nAWAY: %s\n\n", match.homeTeam->name, match.awayTeam->name);
        return 1;
    }
    printf("HOME: %s\nAWAY: %s\n\n", match.homeTeam->name, match.awayTeam->name);

    //Get stats from a location. Using 2 "boolean" parameters. Pass/Rush and Offense/Defense
    GetYearlyStats(&match, "WebScraping/YearlyStats/RushOffense.txt", 0, 1);
    GetYearlyStats(&match, "WebScraping/YearlyStats/PassOffense.txt", 1, 1);
    GetYearlyStats(&match, "WebScraping/YearlyStats/RushDefense.txt", 0, 0);
    GetYearlyStats(&match, "WebScraping/YearlyStats/PassDefense.txt", 1, 0);

    /*
    printf("HOME RUSH OFF: .....%s || %lf || %lf || %d || %d\n",
        match.homeTeam->offense->name, match.homeTeam->offense->yardsPerCarry, match.homeTeam->offense->rushYpg, match.homeTeam->offense->tdRushes, match.homeTeam->offense->fumbles);
    printf("AWAY RUSH OFF: .....%s || %lf || %lf || %d || %d\n",
        match.awayTeam->offense->name, match.awayTeam->offense->yardsPerCarry, match.awayTeam->offense->rushYpg, match.awayTeam->offense->tdRushes, match.awayTeam->offense->fumbles);
    printf("\nHOME PASS OFF: .....%s || %lf || %lf || %d || %d || %d || %lf\n",
        match.homeTeam->offense->name, match.homeTeam->offense->yardsPerPass, match.homeTeam->offense->passYpg, match.homeTeam->offense->tdPasses, match.homeTeam->offense->interceptions, match.homeTeam->offense->sacks, match.homeTeam->offense->passerRating);
    printf("AWAY PASS OFF: .....%s || %lf || %lf || %d || %d || %d || %lf\n",
        match.awayTeam->offense->name, match.awayTeam->offense->yardsPerPass, match.awayTeam->offense->passYpg, match.awayTeam->offense->tdPasses, match.awayTeam->offense->interceptions, match.awayTeam->offense->sacks, match.awayTeam->offense->passerRating);
    printf("\nHOME PASS DEF: .....%s || %lf || %lf || %d || %d || %d || %lf\n",
        match.homeTeam->name, match.homeTeam->defense->yardsPerPass, match.homeTeam->defense->passYpg, match.homeTeam->defense->tdPasses, match.homeTeam->defense->interceptions, match.homeTeam->defense->sacks, match.homeTeam->defense->passerRating);
    printf("AWAY PASS DEF: .....%s || %lf || %lf || %d || %d || %d || %lf\n",
        match.awayTeam->name, match.awayTeam->defense->yardsPerPass, match.awayTeam->defense->passYpg, match.awayTeam->defense->tdPasses, match.awayTeam->defense->interceptions, match.awayTeam->defense->sacks, match.awayTeam->defense->passerRating);
    printf("\nHOME RUSH DEF: .....%s || %lf || %lf || %d || %d\n",
        match.homeTeam->defense->name, match.homeTeam->defense->yardsPerCarry, match.homeTeam->defense->rushYpg, match.homeTeam->defense->tdRushes, match.homeTeam->defense->fumbles);
    printf("AWAY RUSH DEF: .....%s || %lf || %lf || %d || %d\n",
        match.awayTeam->defense->name, match.awayTeam->defense->yardsPerCarry, match.awayTeam->defense->rushYpg, match.awayTeam->defense->tdRushes, match.awayTeam->defense->fumbles);
    */
    return 0;
}

static inline void* MallocMe(size_t sizeOfMem)
{
    void* pointer = malloc(sizeOfMem);
    if (!pointer && sizeOfMem)
    {
        printf("Could not allocate. HIDE YO KIDS HIDE YO WIFE");
        exit(-1);
    }
    return pointer;
}

void VerifyEntry(struct Game* match, struct Team* mainTeam, struct Team* oppTeam) {

    int pathLength = 36;
    int weekLength = 1;
    if (match->week >= 10) {
        pathLength += 1;
        weekLength += 1;
    }

    //Create the file path
    char* path = MallocMe(sizeof(char) * (pathLength + 1));
    char* strWeek = MallocMe(sizeof(char) * (weekLength + 1));
    sprintf_s(strWeek, sizeof(char) * (weekLength + 1), "%d", match->week);
    strcpy_s(path, sizeof(char) * (pathLength + 1), "WebScraping/WeeklySchedule/Week");
    strcat_s(path, sizeof(char) * (pathLength + 1), strWeek);
    strcat_s(path, sizeof(char) * (pathLength + 1), ".txt");

    //Get file and deallocate memory
    FILE* ptrFile;
    fopen_s(&ptrFile, path, "r");
    free(path);
    free(strWeek);

    char strLine[256];
    char* context = NULL;
    while (ptrFile != NULL && fgets(strLine, sizeof(strLine), ptrFile)) {
        if (strcmp(strtok_s(strLine, ",", &context), mainTeam->name) == 0) {
            char* chunk;
            chunk = strtok_s(NULL, ",", &context);

            strcpy_s(oppTeam->name, sizeof(char) * (strlen(oppTeam->name) + 1), chunk);
            oppTeam->name[strlen(chunk)] = 0;

            AddTeamToMatch(match, mainTeam, oppTeam);
            break;
        }
        else {
            char* chunk;
            chunk = strtok_s(NULL, ",", &context);

            //chunk[strlen(chunk)] = 0;
            if (strcmp(chunk, mainTeam->name) == 0) {
                strcpy_s(oppTeam->name, sizeof(char) * (strlen(oppTeam->name) + 1), strLine);
                oppTeam->name[strlen(strLine)] = 0;

                AddTeamToMatch(match, oppTeam, mainTeam);
                break;
            }
        }
    }
    if (ptrFile != NULL) {
        fclose(ptrFile);
    }
}

void AddTeamToMatch(struct Game* match, struct Team* homeTeam, struct Team* awayTeam) {
    match->homeTeam = homeTeam;
    match->awayTeam = awayTeam;

    //Allocate memory for the offense and defense structs
    if (match->homeTeam->dataSet == 0) {
        //Home team offense
        match->homeTeam->offense = MallocMe(sizeof(struct TeamStats));
        //Home team defnse
        match->homeTeam->defense = MallocMe(sizeof(struct TeamStats));
        match->homeTeam->dataSet = 1;
    }

    if (match->awayTeam->dataSet == 0) {
        //Away team offense
        match->awayTeam->offense = MallocMe(sizeof(struct TeamStats));
        //Away team defense
        match->awayTeam->defense = MallocMe(sizeof(struct TeamStats));
        match->awayTeam->dataSet = 1;
    }
}

void SetStats(struct TeamStats* team, char* lineCopy, int passOrRush) {

    if (passOrRush == 1) {
        sscanf_s(lineCopy, "%[^,],%lf,%lf,%d,%d,%d,%lf",
            team->name, (unsigned)sizeof(team->name),
            &team->yardsPerPass,
            &team->passYpg,
            &team->tdPasses,
            &team->interceptions,
            &team->sacks,
            &team->passerRating);

    }
    else {
        sscanf_s(lineCopy, "%[^,],%lf,%lf,%d,%d",
            team->name, (unsigned)sizeof(team->name),
            &team->yardsPerCarry,
            &team->rushYpg,
            &team->tdRushes,
            &team->fumbles);
    }
}

void GetPassOffense(struct Game* match) {
    FILE* ptrFile;
    char strLine[256];
    char* chunk;
    char* context = NULL;
    char lineCopy[256];
    fopen_s(&ptrFile, "NFLPass.txt", "r");

    while (ptrFile != NULL && fgets(strLine, sizeof(strLine), ptrFile)) {
        //strcpy(lineCopy, strLine);
        strcpy_s(lineCopy, 256, strLine);
        chunk = strtok_s(strLine, ",", &context);
        if (strcmp(chunk, match->homeTeam->name) == 0) {
            SetStats(match->homeTeam->offense, lineCopy, 1);
        }
        else if (strcmp(chunk, match->awayTeam->name) == 0) {
            SetStats(match->awayTeam->offense, lineCopy, 1);
        }
    }
    if (ptrFile != NULL) {
        fclose(ptrFile);
    }
}

void GetRushOffense(struct Game* match) {
    FILE* ptrFile;
    char strLine[256];
    char* chunk;
    char* context = NULL;
    char lineCopy[sizeof(strLine)];
    fopen_s(&ptrFile, "NFLRush.txt", "r");

    while (ptrFile != NULL && fgets(strLine, sizeof(strLine), ptrFile)) {
        //strcpy(lineCopy, strLine);
        strcpy_s(lineCopy, 256, strLine);

        chunk = strtok_s(strLine, ",", &context);
        if (strcmp(chunk, match->homeTeam->name) == 0) {
            SetStats(match->homeTeam->offense, lineCopy, 0);
        }
        else if (strcmp(chunk, match->awayTeam->name) == 0) {
            SetStats(match->awayTeam->offense, lineCopy, 0);
        }
    }
    if (ptrFile != NULL) {
        fclose(ptrFile);
    }
}

void GetPassDefense(struct Game* match) {
    FILE* ptrFile;
    char strLine[256];
    char* chunk;
    char* context = NULL;
    char lineCopy[sizeof(strLine)];
    fopen_s(&ptrFile, "NFLPassDef.txt", "r");
    while (ptrFile != NULL && fgets(strLine, 256, ptrFile)) {
        //strcpy(lineCopy, strLine);
        strcpy_s(lineCopy, sizeof(strLine), strLine);

        //if (match.homeTeam->yppOff != 0 && match.awayTeam->yppOff != 0) {
        chunk = strtok_s(strLine, ",", &context);
        if (strcmp(chunk, match->homeTeam->name) == 0) {
            SetStats(match->homeTeam->defense, lineCopy, 1);
        }
        else if (strcmp(chunk, match->awayTeam->name) == 0) {
            SetStats(match->awayTeam->defense, lineCopy, 1);
        }
    }
    if (ptrFile != NULL) {
        fclose(ptrFile);
    }
}

void GetRushDefense(struct Game* match) {
    FILE* ptrFile;
    char strLine[256];
    char* chunk;
    char* context = NULL;
    char lineCopy[sizeof(strLine)];

    fopen_s(&ptrFile, "NFLRushDef.txt", "r");
    while (ptrFile != NULL && fgets(strLine, 256, ptrFile)) {
        //strcpy(lineCopy, strLine);
        strcpy_s(lineCopy, sizeof(strLine), strLine);

        //if (match.homeTeam->yppOff != 0 && match.awayTeam->yppOff != 0) {
        chunk = strtok_s(strLine, ",", &context);
        if (strcmp(chunk, match->homeTeam->name) == 0) {
            SetStats(match->homeTeam->defense, lineCopy, 0);
        }
        else if (strcmp(chunk, match->awayTeam->name) == 0) {
            SetStats(match->awayTeam->defense, lineCopy, 0);
        }
    }
    if (ptrFile != NULL) {
        fclose(ptrFile);
    }
}

void GetYearlyStats(struct Game* match, char* filePath, int passOrRush, int offOrDef) {
    FILE* ptrFile;
    char strLine[256];
    char* chunk;
    char* context = NULL;
    char lineCopy[sizeof(strLine)];
    fopen_s(&ptrFile, filePath, "r");

    int homeSet = 0;
    int awaySet = 0;
    while (ptrFile != NULL && fgets(strLine, sizeof(strLine), ptrFile)) {
        strcpy_s(lineCopy, sizeof(strLine), strLine);

        //if (match.homeTeam->yppOff != 0 && match.awayTeam->yppOff != 0) {
        chunk = strtok_s(strLine, ",", &context);
        if (strcmp(chunk, match->homeTeam->name) == 0) {
            if (offOrDef == 1) {
                SetStats(match->homeTeam->offense, lineCopy, passOrRush);
            }
            else {
                SetStats(match->homeTeam->defense, lineCopy, passOrRush);
            }

            if (awaySet == 1) {
                break;
            }
            else {
                homeSet = 1;
            }
        }
        else if (strcmp(chunk, match->awayTeam->name) == 0) {
            if (offOrDef == 1) {
                SetStats(match->awayTeam->offense, lineCopy, passOrRush);
            }
            else {
                SetStats(match->awayTeam->defense, lineCopy, passOrRush);
            }

            if (homeSet == 1) {
                break;
            }
            else {
                awaySet = 1;
            }
        }
    }
    if (ptrFile != NULL) {
        fclose(ptrFile);
    }
}

