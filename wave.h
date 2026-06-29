#pragma once
#include "structs.h"

// ---------------------------------
// 파동 관련 항목
// ---------------------------------

extern int waveCount;
extern int nextWaveCode;

int getNextWaveCode();
void drawWaveBuffer(WAVE* w);
void sendMainWave(WAVE* w, int x, int y);
void updateWave(WAVE* w);
void isScanned(ANOMALY* anom, WAVE* w);
void isScannedByAntenna(ANOMALY* anom, ANTENNA_WAVE* w);

int getAntennaWaveRange(int type);
int getAntennaWaveSpreadAngle(int type);
void sendAntennaWave(ANTENNA* antenna, int antennaIndex, int chain);
void tryAmplifyCircleWave(WAVE* w, ANTENNA* antenna, int antennaIndex);
void tryAmplifyAntennaWave(ANTENNA_WAVE* w, ANTENNA* antenna, int antennaIndex);
int isInsideAntennaWaveAngle(ANTENNA_WAVE* w, int dx, int dy);
void drawAntennaWaveBuffer(ANTENNA_WAVE* w);
void updateAntennaWave(ANTENNA_WAVE* w);