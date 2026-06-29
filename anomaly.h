#pragma once
#include <time.h>
#include "structs.h"

// ---------------------------------
// 이상체 관련 항목
// ---------------------------------

extern int fieldValue;
extern double fieldCoin;
extern double desiredValue;
extern int anchorCount;
extern int valueProductionBuckets[60];
extern int currentProductionBucket;
extern clock_t lastProductionBucketTime;
extern clock_t nextAnomalySpawnTime;
extern double spawnCenterX;
extern double spawnCenterY;

extern int scannedSpecialAnomalies;

void updateValueProductionBucket();
double getAverageCoinProductionPerSecond();
int getAnomalyValue();
void refreshAnchors();
int getAnchorCount();
double getDesiredValue();
int isAlreadyExists(ANOMALY* anom, int x, int y);
int isInsideMainReach(int dx, int dy);
int countMainReachAnomalies(ANOMALY* anom);
int isSpawnPositionSafe(int x, int y);
ANCHOR* chooseRandomAnchor();
int getClusterOrigin(ANCHOR* anchor, int* outX, int* outY);
int isTooCloseToOrigin(int x, int y);
void createAnomaly(ANOMALY* anomaly);
void createAnomalyNearOrigin(ANOMALY* anomaly);
void updateAnomalySpawner();
void placeSpecialAnomalies();
void spawnAnomaliesTrio();
void printAnomalies(ANOMALY* anom);
void deleteAnomaly(ANOMALY* anom);