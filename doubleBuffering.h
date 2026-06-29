#pragma once
#include <Windows.h>

// ---------------------------------
// 더블 버퍼링 관련 항목
// ---------------------------------

extern int hidden_index;
extern HANDLE scr_handle[2];

void scr_init();
void scr_switch();
void scr_clear();
void scr_release();
void gotoxy(int x, int y);
void printscr(char* str);
void printxy(int x, int y, char* str);
void textcolor(int fg_color, int bg_color);