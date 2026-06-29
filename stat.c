#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include "constants.h"
#include "structs.h"
#include "doubleBuffering.h"
#include "stat.h"

// ---------------------------------
// 게임 시스템 관련 항목
// ---------------------------------

// 스탯 ------------------------------------------------------------------------------------------------------------------------------------------
//
// double autoPingDelay;	// 메인 안테나(플레이어)가 자동으로 핑을 보낸다. 수동 핑도 여전히 사용할 수 있다.					(OFF => 5.0s => 3.33s => 2.22s => 1.48s => 0.99s => 0.66s => 0.44s => 0.29s)
// int anomalyValue;		// 각 이상체를 더 여러 번 스캔할 수 있게 되어, 하나의 이상체에서 얻는 총 보상이 증가한다.			(12 => 20 => 33 => 54 => 89 => 147 => 242 => 400 => 659)
// int chainDepth;			// 각 파동이 사라지기 전에 더 여러 번 다른 대상으로 이어질 수 있다.								(3 => 4 => 5 => 7 => 8 => 10 => 13 => 16 => 19)
// double waveSpeed;		// 파동이 더 빠르게 이동한다.																	(1.0 => 1.35 => 1.82 => 2.46 => 3.32 => 4.48 => 6.05 => 8.17 => 11.03)
// int coinsPerScan;		// 이상체를 한 번 스캔할 때마다 얻는 보상이 증가한다. 또한 각 이상체를 더 빠르게 처리할 수 있다.	(1 => 2 => 3 => 5 => 8 => 14 => 24 => 41 => 70) (anomaly value < coinsPerScan일 경우 anomaly value = coinsPerScan * 5로 보정)
// int anomalyEcho;			// 이상체가 완전히 스캔되었을 때, 이상체가 스캔 파동을 방출한다.									(0 => 1)
// double anomalyGlow;		// 이상체가 스캔된 뒤 더 오랫동안 빛난다.														(1.25 => 1.63 => 2.11 => 2.75 => 3.57 => 4.64 => 6.03 => 7.84 => 10.2)
// double waveReach;		// 파동이 더 멀리 이동한다.																	(1.0 => 1.22 => 1.49 => 1.82 => 2.22 => 2.7 => 3.3 => 4.02 => 4.91)
// int antennaLifetime;		// 안테나의 수명이 증가한다.																	(60s => 90s => 135s => 202s => 304s => 456s)
// 
// -----------------------------------------------------------------------------------------------------------------------------------------------


int statMode = 0;		// 스탯 모드 활성화 여부
int selectedStat = 0;	// 선택된 스탯
int firstStatBox = 0;	// 처음으로 출력될 스탯 박스
int lastStatBox;		// 마지막으로 출력된 스탯 박스

STAT_INFO statInfo[STATS_COUNT] = {
	{ "Auto Ping", "메인 안테나가 자동으로 핑을 보낸다. 수동 핑도 여전히 사용할 수 있다.", 0, {-2.0, 5.0, 3.33, 2.22, 1.48, 0.99, 0.66, 0.44, 0.29, -1.0}, {20, 56, 157, 439, 1229, 3442, 9638, 26986} },
	{ "Anomaly Value", "각 이상체를 더 여러 번 스캔할 수 있게 되어, 하나의 이상체에서 얻는 총 보상이 증가한다.", 0, {12.0, 20.0, 33.0, 54.0, 89.0, 147.0, 242.0, 400.0, 659.0, -1.0 }, {12, 23, 46, 89, 174, 338, 660, 1287} },
	{ "Chain Depth", "각 파동이 사라지기 전에 더 여러 번 다른 대상으로 이어질 수 있다.", 0, {3.0, 4.0, 5.0, 7.0, 8.0, 10.0, 13.0, 16.0, 19.0, -1.0 }, {38, 89, 210, 493, 1159, 2723, 6400, 15040} },
	{ "Wave Speed", "파동이 더 빠르게 이동한다.", 0, {1.0, 1.35, 1.82, 2.46, 3.32, 4.48, 6.05, 8.17, 11.03, -1.0 }, {100, 245, 600, 1471, 3603, 8827, 21627, 52986} },
	{ "Coins per Scan", "이상체를 한 번 스캔할 때마다 얻는 보상이 증가한다. 또한 각 이상체를 더 빠르게 처리할 수 있다.", 0, {1.0, 2.0, 3.0, 5.0, 8.0, 14.0, 24.0, 41.0, 70.0, -1.0 }, {26, 51, 99, 193, 376, 733, 1429, 2788} },
	{ "Anomaly Echo", "이상체가 완전히 스캔되었을 때, 이상체가 스캔 파동을 방출한다.", 0, {0.0, 1.0, -1.0 }, {575} },
	{ "Anomaly Glow", "이상체가 스캔된 뒤 더 오랫동안 빛난다.", 0, {1.25, 1.63, 2.11, 2.75, 3.57, 4.64, 6.03, 7.84, 10.2, -1.0 }, {18, 35, 68, 133, 260, 508, 990, 1930} },
	{ "Wave Reach", "파동이 더 멀리 이동한다.", 0, {1.0, 1.22, 1.49, 1.82, 2.22, 2.7, 3.3, 4.02, 4.91, -1.0}, {48, 103, 222, 477, 1026, 2205, 4741, 10193} },
	{ "Antenna Life Time", "안테나의 수명이 증가한다.", 0, {60.0, 90.0, 135.0, 202.0, 304.0, 456.0, -1.0 }, {26, 57, 126, 277, 609} }
};


// 스탯 갱신
void updateStat(int statType) {
	int* level = &(statInfo[statType].level);

	if (statType < 0 || statType >= STATS_COUNT) return;					// 0~9 사이 아닐 시 리턴

	if (statInfo[statType].statList[*level + 1] < 0)							// 만렙일 시 리턴
		return;

	if (currentCoin < statInfo[statType].price[*level])						// 돈 부족하면 리턴
		return;

	currentCoin -= statInfo[statType].price[*level];						// 돈 차감
	(*level)++;		// 레벨업

	// 스탯 업데이트
	switch (statType) {
	case AUTO_PING:
		stats.autoPingDelay = statInfo[AUTO_PING].statList[*level];
		if (*level == 1) openAntenna = 1;
		break;
	case ANOMALY_VALUE:
		stats.anomalyValue = (int)(statInfo[ANOMALY_VALUE].statList[*level]);
		if (*level == 1) openStat = ANTENNA_LIFE_TIME;
		break;
	case CHAIN_DEPTH:
		stats.chainDepth = (int)(statInfo[CHAIN_DEPTH].statList[*level]);
		break;
	case WAVE_SPEED:
		stats.waveSpeed = statInfo[WAVE_SPEED].statList[*level];
		break;
	case COINS_PER_SCAN:
		stats.coinsPerScan = (int)(statInfo[COINS_PER_SCAN].statList[*level]);
		break;
	case ANOMALY_ECHO:
		stats.anomalyEcho = (int)(statInfo[ANOMALY_ECHO].statList[*level]);
		break;
	case ANOMALY_GLOW:
		stats.anomalyGlow = statInfo[ANOMALY_GLOW].statList[*level];
		break;
	case WAVE_REACH:
		stats.waveReach = statInfo[WAVE_REACH].statList[*level];
		break;
	case ANTENNA_LIFE_TIME:
		stats.antennaLifetime = (int)(statInfo[ANTENNA_LIFE_TIME].statList[*level]);
		break;
	}
}


// 요소가 스탯 창 안에 있는지 확인
int isInsideStatWindow(int y) {
	if (y > 48) return 0;
	return 1;
}


// 시간 단위 스탯 출력
void printTimeStat(STAT_INFO* s, int level, int y) {
	char buf[30] = "";
	int i = 0;
	int len = 0;

	while (i < 2) {
		if ((int)(s->statList[level] * 100.0) % 10 == 0) {
			len += sprintf(buf + len, "%.1lfs", s->statList[level]);
		}
		else {
			len += sprintf(buf + len, "%.2lfs", s->statList[level]);
		}

		if ((i == 1) || (s->statList[level + 1] < 0)) break;
		else {
			len += sprintf(buf + len, " => ");
			level++;
			i++;
		};
	}
	printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 4 + y, buf);
}


// 실수 단위 스탯 출력
void printDoubleStat(STAT_INFO* s, int level, int y) {
	char buf[30] = "";
	int i = 0;
	int len = 0;

	while (i < 2) {
		if ((int)(s->statList[level] * 100.0) % 10 == 0) {
			len += sprintf(buf + len, "%.1lf", s->statList[level]);
		}
		else {
			len += sprintf(buf + len, "%.2lf", s->statList[level]);
		}

		if ((i == 1) || (s->statList[level + 1] < 0)) break;
		else {
			len += sprintf(buf + len, " => ");
			level++;
			i++;
		}
	}
	printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 4 + y, buf);
}


// 정수 단위 스탯 출력
void printIntStat(STAT_INFO* s, int level, int y) {
	char buf[30] = "";
	int i = 0;
	int len = 0;

	while (i < 2) {
		len += sprintf(buf + len, "%d", (int)(s->statList[level]));

		if ((i == 1) || (s->statList[level + 1] < 0)) break;
		else {
			len += sprintf(buf + len, " => ");
			level++;
			i++;
		}
	}
	printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 4 + y, buf);
}


// 안테나 시간 스탯 출력
void printAntennaStat(STAT_INFO* s, int level, int y) {
	char buf[30] = "";
	int i = 0;
	int len = 0;

	while (i < 2) {
		len += sprintf(buf + len, "%dm", (int)(s->statList[level]) / 60);
		if (((int)(s->statList[level]) % 60) > 0)
			len += sprintf(buf + len, " %ds", (int)(s->statList[level]) % 60);

		if ((i == 1) || (s->statList[level + 1] < 0)) break;
		else {
			len += sprintf(buf + len, " => ");
			level++;
			i++;
		}
	}
	printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 4 + y, buf);
}


// 스탯 박스 생성
void printStatBox(STAT_INFO* s, int statType, int y) {
	int i;
	y *= 6;
	char buf[50];
	int level = s->level;

	// 해당 스탯 박스가 선택된 경우
	if (statMode && (statType == selectedStat)) {
		textcolor(COLOR_YELLOW, COLOR_GRAY);
	}

	// 첫 번째 행 출력
	if (isInsideStatWindow(STAT_BOX_MIN_Y + y)) {
		printxy(STAT_BOX_MIN_X, STAT_BOX_MIN_Y + y, "┌────────────────────┐ ");
	}
	else {
		lastStatBox = statType;
		textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
		return;
	}

	// 두 번째 행 출력
	if (isInsideStatWindow(STAT_BOX_MIN_Y + 1 + y)) {
		printxy(STAT_BOX_MIN_X, STAT_BOX_MIN_Y + 1 + y, "│                    │ ");

		// 현재 레벨 출력
		gotoxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 1 + y);
		for (i = 0; i <= s->level; i++) {
			printscr("●");
		}
		while (s->statList[i] > 0) {
			printscr("○");
			i++;
		}
	}
	else {
		lastStatBox = statType;
		textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
		return;
	}

	// 세 번째 행 출력
	if (isInsideStatWindow(STAT_BOX_MIN_Y + 2 + y)) {
		printxy(STAT_BOX_MIN_X, STAT_BOX_MIN_Y + 2 + y, "│                    │ ");

		// 가격 출력
		if (s->statList[level + 1] < 0) {					// MAX_LEVEL 일 시
			printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 2 + y, "MAX");
		}
		else {
			sprintf(buf, "$%d", s->price[level]);
			printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 2 + y, buf);
		}
	}
	else {
		lastStatBox = statType;
		textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
		return;
	}

	// 네 번째 행 출력
	if (isInsideStatWindow(STAT_BOX_MIN_Y + 3 + y)) {
		printxy(STAT_BOX_MIN_X, STAT_BOX_MIN_Y + 3 + y, "│                    │ ");

		// 스탯 이름 출력
		printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 3 + y, s->name);
	}
	else {
		lastStatBox = statType;
		textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
		return;
	}

	// 다섯 번째 행 출력
	if (isInsideStatWindow(STAT_BOX_MIN_Y + 4 + y)) {
		printxy(STAT_BOX_MIN_X, STAT_BOX_MIN_Y + 4 + y, "│                    │ ");

		i = 0;

		switch (statType) {
		case AUTO_PING:
			if (level == 0) {
				printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 4 + y, "OFF => 5.0s");
			}
			else {
				printTimeStat(s, level, y);
			}
			break;
		case ANOMALY_GLOW:
			printTimeStat(s, level, y);
			break;
		case WAVE_SPEED: case WAVE_REACH:
			printDoubleStat(s, level, y);
			break;
		case ANOMALY_VALUE: case CHAIN_DEPTH: case COINS_PER_SCAN:
			printIntStat(s, level, y);
			break;
		case ANOMALY_ECHO:
			if (level == 0)
				printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 4 + y, "OFF => ON");
			else if (level == 1)
				printxy(STAT_BOX_MIN_X + 2, STAT_BOX_MIN_Y + 4 + y, "ON");
			break;
		case ANTENNA_LIFE_TIME:
			printAntennaStat(s, level, y);
			break;
		}
	}
	else {
		lastStatBox = statType;
		textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
		return;
	}

	// 여섯 번째 행 출력
	if (isInsideStatWindow(STAT_BOX_MIN_Y + 5 + y)) {
		printxy(STAT_BOX_MIN_X, STAT_BOX_MIN_Y + 5 + y, "└────────────────────┘ ");
	}
	else {
		lastStatBox = statType;
		textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
		return;
	}

	y += 6;
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
}


int statY = 0;
// 다른 스탯 선택하기
void moveSelectedStat() {
	unsigned char ch = getch();		// key 값을 읽는다

	switch (ch) {
	case UP:
		if ((selectedStat == firstStatBox) && (openStat == ANTENNA_LIFE_TIME)) {
			firstStatBox = 0;
			statY = 0;
		}
		if (selectedStat != 0)
			selectedStat -= 1;
		break;
	case DOWN:
		if ((selectedStat == lastStatBox) && (openStat == ANTENNA_LIFE_TIME)) {
			firstStatBox = 3;
			statY = -3;
			break;
		}
		if (selectedStat < openStat)
			selectedStat += 1;
		break;
	}
}