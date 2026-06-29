#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "constants.h"
#include "doubleBuffering.h"
#include "structs.h"
#include "visionCircle.h"
#include "anomaly.h"
#include "antenna.h"
#include "stat.h"
#include "ui.h"

// ---------------------------------
// 기타 UI 관련 항목
// ---------------------------------

// 상단 바 텍스트 출력
void printTopBarText() {
	printxy(6, 2, "signal.exe");			// 상단에 signal.exe 출력
	printxy(173, 2, "X");					// 상단에 가짜 종료 버튼 출력
}


// 현재 좌표 + reset position 버튼 출력
void printCoordAndRP() {
	char buf[20];

	textcolor(COLOR_GRAY, COLOR_BLACK);		// 회색 글자, 검은 배경

	// 현재 좌표 출력 (1/(3.93)해서 보기 좋게 만듦)
	sprintf(buf, "x: %.2lf", (cameraX / 2.0) * (1.0 / 3.93));
	printxy(55, 40, buf);
	sprintf(buf, "y: %.2lf", -cameraY * (1.0 / 3.93));
	printxy(55, 41, buf);

	printxy(55, 43, "R [reset position]");			// [reset position] 출력
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);		// 끝나기 전 다시 원래 색으로
}



// 안테나 구매 버튼 출력
void printAntennaOptions() {
	if (!openAntenna) return;

	if (antennaEditMode && selectedAntennaType == NARROW_ANTENNA)
		textcolor(COLOR_GREEN, COLOR_BLACK);
	printxy(55, 31, "Q [narrow antenna]");
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);

	if (antennaEditMode && selectedAntennaType == MEDIUM_ANTENNA)
		textcolor(COLOR_BLUE, COLOR_BLACK);
	printxy(57, 33, "W [medium antenna]");
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);

	if (antennaEditMode && selectedAntennaType == WIDE_ANTENNA)
		textcolor(COLOR_GRAY, COLOR_BLACK);
	printxy(62, 35, "E [wide antenna]");
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
}



// 현재 시야 내 이상체 수 출력
void printAnomaliesCount() {
	int x, y;
	char buf[30];
	int anomInVision = 0;
	int i;

	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anomaly[i].active) continue;

		x = anomaly[i].x - cameraX + visionCircleCenterX;
		y = anomaly[i].y - cameraY + visionCircleCenterY;

		if (isInsideVision(x, y)) {
			anomInVision++;
		}
	}

	if (anomInVision == 0)
		textcolor(COLOR_GRAY, COLOR_BLACK);			// 시야 내 이상체 없을 시 회색 글자
	else
		textcolor(COLOR_YELLOW, COLOR_BLACK);		// 노란 글자, 검은 배경

	sprintf(buf, "[%d anomalies in view]", anomInVision);
	printxy(98, 43, buf);

	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);		// 끝나기 전 다시 원래 색으로
}


// 설명 출력
void printDescription() {
	char buf[100];

	// antennaEditMode인 경우
	if (antennaEditMode && selectedAntenna != NULL) {
		sprintf(buf, "angle: %d / 방향키: 안테나 조작 / ENTER: 설치 / A D: 방향 회전", selectedAntenna->angle);
		printxy(55, 47, buf);
	}
	// statMode인 경우
	else if (statMode) {
		printxy(55, 47, statInfo[selectedStat].description);
	}
	// 기본
	else {
		if (scannedSpecialAnomalies == SPECIAL_ANOMALY_COUNT)
			printxy(55, 47, "클리어!");
		else
			printxy(55, 47, "방향키: 카메라 조작 / SPACE: 파동 보내기 / TAB: 안테나 선택");
	}
}


// 스탯 창 출력
void printStats() {
	char buf[50];

	// 현재 소지한 코인 출력
	printxy(152, 4, " / Available Funds");
	textcolor(COLOR_YELLOW, COLOR_BLACK);
	sprintf(buf, "   $%d", currentCoin);
	printxy(152, 5, buf);
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
	printxy(173, 5, "/");

	printxy(152, 7, "-----------------------");
	printxy(152, 8, " Upgrades [P]");
	printxy(152, 9, "-----------------------");
}


int clearMode = 0;
clock_t clear_start_t = 0;

// 클리어 텍스트 애니메이션
void clearAnimation() {
	clock_t now = clock() * 1000 / CLOCKS_PER_SEC;
	int time = now - clear_start_t;

	if (!clearMode) return;

	textcolor(COLOR_YELLOW, 12);

	if (time < 100) {
		printxy(3, 26, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 27, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 28, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 29, "                                                 ");
	}
	else if (time < 200) {
		// + 4
		printxy(3, 27, "                                                 ");
		printxy(3, 28, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 29, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 30, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 31, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 32, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 33, "                                                 ");
	}
	else if (time < 300) {
		// + 3
		printxy(3, 30, "                                                 ");
		printxy(3, 31, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 32, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 33, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 34, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 35, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 36, "                                                 ");
	}
	else if (time < 400) {
		// + 2
		printxy(3, 32, "                                                 ");
		printxy(3, 33, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 34, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 35, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 36, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 37, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 38, "                                                 ");
	}
	else if (time < 500) {
		// + 1
		printxy(3, 33, "                                                 ");
		printxy(3, 34, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 35, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 36, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 37, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 38, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 39, "                                                 ");
	}
	else if (time < 600) {
		// + 1
		printxy(3, 34, "                                                 ");
		printxy(3, 35, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 36, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 37, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 38, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 39, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 40, "                                                 ");
	}
	else if (time < 700) {
		// + 1
		printxy(3, 35, "                                                 ");
		printxy(3, 36, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 37, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 38, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 39, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 40, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 41, "                                                 ");
	}
	else if (time < 800) {
		// + 2
		printxy(3, 37, "                                                 ");
		printxy(3, 38, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 39, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 40, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 41, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 42, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 43, "                                                 ");
	}
	else if (time < 900) {
		// + 3
		printxy(3, 40, "                                                 ");
		printxy(3, 41, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 42, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 43, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 44, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
		printxy(3, 45, "    VMMMP\" dMMMMMP dMMMMMP dMP dMP dMP dMP       ");
		printxy(3, 46, "                                                 ");
	}
	else if (time < 1000) {
		// + 4
		printxy(3, 44, "                                                 ");
		printxy(3, 45, "       .aMMMb  dMP     dMMMMMP .aMMMb  dMMMMb    ");
		printxy(3, 46, "      dMP\"VMP dMP     dMP     dMP\"dMP dMP.dMP    ");
		printxy(3, 47, "     dNP     dMP     dMMMP   dMMMMMP dMMMMK\"     ");
		printxy(3, 48, "    dMP.aMP dMP     dMP     dMP dMP dMP\"AMF      ");
	}
	else if (time > 1100) {
		clearMode = 0;
	}
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
}


// 스캔 완료된 특별 이상체 개수를 표시함
void showScannedSpecialAnomalies() {
	static int complete = 0;
	//sprintf(buf, "%d/10", scannedSpecialAnomalies);
	//printxy(30, 40, buf);

	if (scannedSpecialAnomalies >= 1) {
		printxy(3, 26, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#%%%%%%%%%%%%%%%%");
		printxy(3, 27, "%%%%%%%%%%%##%%%%%%%%%%%%%%%%%#***%%%%%%%%%%%%%%%");
	}
	if (scannedSpecialAnomalies >= 2) {
		printxy(3, 28, "%%%%%%%%%%%+=+++#%%%%%%%%%%%#*+***#%%%%%%%%%%%%%%");
		printxy(3, 29, "%%%%%%%%%%%+=++++=+**++*##%%%#**+**%%%%%%%%%%%%%%");
	}

	if (scannedSpecialAnomalies >= 3) {
		printxy(3, 30, "%%%%%%%%%%%*====++=++*++####*++*#***%%%%%%%%%%%%%");
		printxy(3, 31, "%%%%%%%%%%%%*+===+=+*###*=--:::-=+###%%%%%%%%%%%%");
	}

	if (scannedSpecialAnomalies >= 4) {
		printxy(3, 32, "%%%%%%%%%%%%%**++==+**+=::::::::::-=+*%%%%%%%%%%%");
		printxy(3, 33, "%%%%%%%%%%%%%%===-=+++-::::::::::::-=+%%%%%%%%%%%");
	}

	if (scannedSpecialAnomalies >= 5) {
		printxy(3, 34, "%%%%%%%%%%%%%%*-:-=+=-::::::::::::::=+%%%%%%%%%%%");
		printxy(3, 35, "%%%%%%%%%%%%%%%=:-=+=-:::::::::::::-=+%%%%%%%%%%%");
	}

	if (scannedSpecialAnomalies >= 6) {
		printxy(3, 36, "%%%%%%%%%%%%%%%%--++++=---::::::---==#%%%%%%%%%%%");
		printxy(3, 37, "%%%%%%%%%%%%%%%%+++++++==--------===#%%%%%%%%%%%%");
	}

	if (scannedSpecialAnomalies >= 7) {
		printxy(3, 38, "%%%%%%%%%%%%%%%*++++++++++=========#%%%%%%%%%%%%%");
		printxy(3, 39, "%%%%%%%%%%%%%%#*++++++++++========+%%%%%%%%%%%%%%");
		printxy(3, 40, "%%%%%%%%%%%%%%*++++++++++++=======+#%%%%%%%%%%%%%");
	}

	if (scannedSpecialAnomalies >= 8) {
		printxy(3, 41, "%%%%%%%%%%%%%**+++++++++++++++===++#%%%%%%%%%%%%%");
		printxy(3, 42, "%%%%%%%%%%%##*+++++++++++++++++++++#%%%%%%%%%%%%%");
		printxy(3, 43, "%%%%%%%%%%###*+++++++++++++++++++++%%%%%%%%%%%%%%");
	}
	if (scannedSpecialAnomalies >= 9) {
		printxy(3, 44, "%%%%%%%%%%##*+++++++++++++++++====+%%%%%%%%%%%%%%");
		printxy(3, 45, "%%%%%%%%%%#**++++++++++++++++++++=*%%%%%%%%%%%%%%");
	}
	if (scannedSpecialAnomalies >= 10) {
		printxy(3, 46, "%%%%%%%%%%***+++++++++++++===+++++*%%%%%%%%%%%%%%");
		printxy(3, 47, "%%%%%%%%%****+=*****+========+++++*%%%%%%%%%%%%%%");
		printxy(3, 48, "%%%%%%%%%%%%%%%**+++**#####***+++*#%%%%%%%%%%%%%%");

		if (!complete) {
			clearMode = 1;
			complete = 1;
			clear_start_t = clock() * 1000 / CLOCKS_PER_SEC;
		}
	}

	if (clearMode) {
		clearAnimation();
	}
}