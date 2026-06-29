#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "constants.h"
#include "doubleBuffering.h"

// ---------------------------------
// 더블 버퍼링 및 텍스트 색상 관련 항목
// ---------------------------------

int hidden_index;	// Hidden 화면 번호 0 or 1
HANDLE scr_handle[2];	// 화면 버퍼 변수

void scr_init() {
	CONSOLE_CURSOR_INFO cci;

	// 화면 버퍼 2개를 만든다.
	scr_handle[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	scr_handle[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	// 커서를 숨긴다.
	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(scr_handle[0], &cci);
	SetConsoleCursorInfo(scr_handle[1], &cci);

	// 0번 화면이 default
	SetConsoleActiveScreenBuffer(scr_handle[0]);
	hidden_index = 1;	// 1번 화면이 Hidden Screen
}

void scr_switch() {
	// hidden 을 active 로 변경한다.
	SetConsoleActiveScreenBuffer(scr_handle[hidden_index]);
	// active 를 hidden 으로 변경한다.
	hidden_index = !hidden_index;	// 0 <-> 1 toggle
}

void scr_clear() {
	COORD Coor = { 0, 0 };
	DWORD dw;
	// hidden screen를 clear한다.
	// WIDTH*2 * HEIGHT 값은 [속성]에서 설정한 값과 정확히 같아야 한다.
	// 즉, 화면 속성에서 너비(W)=80, 높이(H)=25라면 특수 문자는 2칸씩 이므로 WIDTH=40, HEIGHT=25이다.
	FillConsoleOutputCharacter(scr_handle[hidden_index], ' ', SCREEN_WIDTH * 2 * SCREEN_HEIGHT, Coor, &dw);
}

void scr_release() {
	CloseHandle(scr_handle[0]);
	CloseHandle(scr_handle[1]);
}

// 내가 원하는 커서로 위치 이동
void gotoxy(int x, int y) {
	//DWORD dw;
	COORD CursorPosition = { x, y };
	// hidden screen에 gotoxy
	SetConsoleCursorPosition(scr_handle[hidden_index], CursorPosition);
}

void printscr(char* str) {
	DWORD dw;
	// hidden screen에 gotoxy 되었다고 가정하고 print
	WriteFile(scr_handle[hidden_index], str, strlen(str), &dw, NULL);
}

void printxy(int x, int y, char* str) {
	DWORD dw;
	COORD CursorPosition = { x, y };
	// hidden screen에 gotoxy + print
	SetConsoleCursorPosition(scr_handle[hidden_index], CursorPosition);
	WriteFile(scr_handle[hidden_index], str, strlen(str), &dw, NULL);
}

// 개별 텍스트 색상 지정
void textcolor(int fg_color, int bg_color) {
	SetConsoleTextAttribute(scr_handle[hidden_index], fg_color | bg_color << 4);
}