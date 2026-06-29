#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "structs.h"
#include "doubleBuffering.h"
#include "log.h"

// ---------------------------------
// 로그 관련 항목
// ---------------------------------

// 문자열 삽입
void enqueueLog(const char* str, int type) {

	// 포화 상태일 경우
	if ((gameLog.rear + 1) % LOG_LINES == gameLog.front) {
		strcpy(gameLog.data[gameLog.front].str, "");		// 문자열 초기화
		gameLog.data[gameLog.front].type = 0;
		gameLog.front = (gameLog.front + 1) % LOG_LINES;
	}

	gameLog.rear = (gameLog.rear + 1) % LOG_LINES;
	strcpy(gameLog.data[gameLog.rear].str, str);
	gameLog.data[gameLog.rear].type = type;
}


// 로그 표시
void printLog() {
	int i = gameLog.front;
	int line = 0;

	if (gameLog.front != gameLog.rear) {		// 큐가 비어있지 않은 경우

		while (i != gameLog.rear) {
			i = (i + 1) % (LOG_LINES);

			if (gameLog.data[i].type == SCANNED_LOG)
				textcolor(COLOR_YELLOW, COLOR_BLACK);
			else if (gameLog.data[i].type == SPECIAL_LOG)
				textcolor(COLOR_MAGENTA, COLOR_BLACK);
			else if (gameLog.data[i].type == FULLY_SCANNED_LOG)
				textcolor(COLOR_GRAY, COLOR_BLACK);

			printxy(4, 4 + line, gameLog.data[i].str);
			line++;
			textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
		}
	}
}