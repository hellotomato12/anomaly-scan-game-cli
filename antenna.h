#pragma once
#include "structs.h"

// ---------------------------------
// 안테나 관련 항목
// ---------------------------------

extern ANTENNA* selectedAntenna;
extern int selectedAntennaType;
extern int antennaEditMode;

int findAntennaType(unsigned char key);
int getAntennaPrice(int type);
void createAntenna(unsigned char key);
void deleteAntenna();
void drawAntennaBuffer();
int isThereAnAntenna(int x, int y, ANTENNA* itself);
void moveSelectedAntenna();
void changeSelectedAntenna();
int isCircleWaveTouching(WAVE* w, ANTENNA* antenna);
int isAntennaWaveTouching(ANTENNA_WAVE* w, ANTENNA* antenna);
void rotateSelectedAntenna(int key);
int normalizeAngle(int angle);
int angleDiff(int a, int b);