#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "doubleBuffering.h"
#include "layout.h"

// ---------------------------------
// 레이아웃 생성 및 출력 항목
// ---------------------------------

// 박스 레이아웃 버퍼
int boxLayoutBuf[SCREEN_HEIGHT + 1][SCREEN_WIDTH + 1] = { 0 };

// 0: 공백
// 1: ┌	 2: ┐	 3: └	 4: ┘
// 5: ─	 6: │	 7: ├	 8: ┤

// 박스 형태를 배열에 숫자 형식으로 저장만 함
void drawBoxToBuffer(int x1, int y1, int x2, int y2) {
	int x, y;

	boxLayoutBuf[y1][x1] = (!boxLayoutBuf[y1 - 1][x1]) ? 1 : 7;		// ┌ or ├
	boxLayoutBuf[y2][x1] = (!boxLayoutBuf[y2 + 1][x1]) ? 3 : 7;		// └ or ├
	boxLayoutBuf[y1][x2] = (!boxLayoutBuf[y1 - 1][x2]) ? 2 : 8;		// ┐ or ┤
	boxLayoutBuf[y2][x2] = (!boxLayoutBuf[y2 + 1][x2]) ? 4 : 8;		// ┘ or ┤

	for (y = y1 + 1; y < y2; y++) boxLayoutBuf[y][x2] = 6;
	for (y = y1 + 1; y < y2; y++) boxLayoutBuf[y][x1] = 6;
	for (x = x1 + 1; x < x2; x++) if (!boxLayoutBuf[y2][x]) boxLayoutBuf[y2][x] = 5;
	for (x = x1 + 1; x < x2; x++) if (!boxLayoutBuf[y1][x]) boxLayoutBuf[y1][x] = 5;
}


// 각 구역별로 박스 생성
void printLayouts() {
	drawBoxToBuffer(1, 1, 40, 3);									// signal.exe 있는 구역
	drawBoxToBuffer(40, 1, 170, 3);									// 그 오른쪽 빈 공간 구역
	drawBoxToBuffer(170, 1, SCREEN_WIDTH - 1, 3);					// 닫기 버튼 구역
	drawBoxToBuffer(1, 3, 52, 25);									// 로그 구역
	drawBoxToBuffer(1, 25, 52, SCREEN_HEIGHT - 1);					// 수집 내용 표시 구역
	drawBoxToBuffer(52, 3, 150, 45);								// 인게임 구역
	drawBoxToBuffer(52, 45, 150, SCREEN_HEIGHT - 1);				// 설명 구역
	drawBoxToBuffer(150, 3, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);	// 스탯 구역
}


// boxLayoutBuf에 따른 박스 출력
void drawBox() {
	int x, y;
	char buf[SCREEN_WIDTH * 2 + 1];	// 화면 가로 크기 * 특수문자 2글자씩 차지하는 거 보정 + '\0'

	// 한 행 단위로 buf에 저장한 뒤 한번에 printscr 한다.
	for (y = 0; y <= SCREEN_HEIGHT; y++) {
		gotoxy(0, y);
		strcpy(buf, "");	// 버퍼 초기화

		for (x = 0; x <= SCREEN_WIDTH; x++) {
			switch (boxLayoutBuf[y][x]) {
				// 임시 버퍼 buf에 한 줄로 이어 붙인 뒤
			case 1: strcat(buf, "┌"); break;
			case 2: strcat(buf, "┐"); break;
			case 3: strcat(buf, "└"); break;
			case 4: strcat(buf, "┘"); break;
			case 5: strcat(buf, "─"); break;
			case 6: strcat(buf, "│"); break;
			case 7: strcat(buf, "├"); break;
			case 8: strcat(buf, "┤"); break;
			case 0:
			default:
				strcat(buf, " "); break;
			}
		}
		printscr(buf);	// 마지막에 hidden screen에 출력
	}
}