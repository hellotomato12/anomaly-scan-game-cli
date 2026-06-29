#pragma once
#include "constants.h"

// ---------------------------------
// 게임 내 시야 원 관련 항목
// ---------------------------------

extern int visionBuf[VISION_SQR_HEIGHT][VISION_SQR_WIDTH];
extern int spaceBuf[VISION_SQR_HEIGHT][VISION_SQR_WIDTH];
extern int visionCircleCenterX;
extern int visionCircleCenterY;

extern int cameraX;
extern int cameraY;

void clearGameBuffer();
int isInsideVision(int x, int y);
void putGameBuffer(int x, int y, int obj);
void drawVisionCircleBuffer();
void drawPlayerBuffer();
void drawBackgroundBuffer();
void printVisionBuffer();
void moveCamera();