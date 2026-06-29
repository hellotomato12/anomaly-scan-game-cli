#pragma once
#include "constants.h"

// ---------------------------------
// 레이아웃 생성 및 출력 항목
// ---------------------------------

extern int boxLayoutBuf[SCREEN_HEIGHT + 1][SCREEN_WIDTH + 1];

void drawBoxToBuffer(int x1, int y1, int x2, int y2);
void printLayouts();
void drawBox();