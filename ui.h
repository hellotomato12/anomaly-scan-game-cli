#pragma once
#include <time.h>

// ---------------------------------
// 기타 UI 관련 항목
// ---------------------------------

extern int clearMode;
extern clock_t clear_start_t;

void printTopBarText();
void printCoordAndRP();
void printAntennaOptions();
void printAnomaliesCount();
void printDescription();
void printStats();
void clearAnimation();
void showScannedSpecialAnomalies();