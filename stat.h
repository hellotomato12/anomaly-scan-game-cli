#pragma once
#include "constants.h"
#include "structs.h"

// ---------------------------------
// 게임 시스템 관련 항목
// ---------------------------------

extern int statMode;
extern int selectedStat;
extern int firstStatBox;
extern int lastStatBox;

extern STAT_INFO statInfo[STATS_COUNT];

extern int statY;

void updateStat(int statType);
int isInsideStatWindow(int y);
void printTimeStat(STAT_INFO* s, int level, int y);
void printDoubleStat(STAT_INFO* s, int level, int y);
void printIntStat(STAT_INFO* s, int level, int y);
void printAntennaStat(STAT_INFO* s, int level, int y);
void printStatBox(STAT_INFO* s, int statType, int y);
void moveSelectedStat();