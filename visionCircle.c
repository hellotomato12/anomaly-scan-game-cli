#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "constants.h"
#include "doubleBuffering.h"
#include "visionCircle.h"

// ---------------------------------
// 게임 내 시야 원 관련 항목
// ---------------------------------

// 시야 원을 감싸는 정사각형 배열을 만들어 저장

int visionBuf[VISION_SQR_HEIGHT][VISION_SQR_WIDTH];			// 시야 원 안의 오브젝트를 관리하고, 화면에 표시해줌.
int spaceBuf[VISION_SQR_HEIGHT][VISION_SQR_WIDTH];			// 시야 원 안의 오브젝트가 원 밖으로 이탈하는 것을 방지하기 위함

int visionCircleCenterX = VISION_CIRCLE_RADIUS;				// 시야 원의 원점 x, 36 (반지름과 같음)
int visionCircleCenterY = VISION_CIRCLE_RADIUS / 2;			// 시야 원의 원점 y, 18 (반지름과 같음. 세로 길이 보정하려고 2로 나눔)


// 카메라 좌표
int cameraX = 0;
int cameraY = 0;


// 인게임 화면 지우기
void clearGameBuffer() {
	int x, y;

	for (y = 0; y < VISION_SQR_HEIGHT; y++) {
		for (x = 0; x < VISION_SQR_WIDTH; x++) {
			visionBuf[y][x] = OBJ_EMPTY;
		}
	}
}


// 오브젝트 인덱스가 vision(시야 안)에 있는지 검사
int isInsideVision(int x, int y) {
	if (x < 0 || x >= VISION_SQR_WIDTH ||			// 오브젝트 인덱스가 vision(시야 내)에 없을 시 return
		y < 0 || y >= VISION_SQR_HEIGHT)
		return 0;
	return !spaceBuf[y][x];
}


// 오브젝트를 숫자 형식으로 visionBuf에 저장
void putGameBuffer(int x, int y, int obj) {
	if (!isInsideVision(x, y)) return;

	visionBuf[y][x] = obj;							// 해당 좌표에 오브젝트 저장
}


// 시야 원 경계선을 숫자 형식으로 visionBuf에 저장
void drawVisionCircleBuffer() {
	int x, y;			// 원의 원점에서 떨어진 점 (= 원을 그리는 점)
	double distance;	// 원의 원점과 x, y 사이의 거리
	double f = 0.9;		// 보정값
	static int isMadeSpaceBuf;	// spaceBuf 채워졌는지 확인

	for (x = -visionCircleCenterX; x <= visionCircleCenterX; x++) {			// x = -36 ~ 36
		for (y = -visionCircleCenterY; y <= visionCircleCenterY; y++) {		// y = -18 ~ 18
			distance = (double)(x * x) + (y * 2.0) * (y * 2.0);				// 원의 방정식: x^2 + y^2 = r^2

			// 원점과 반지름을 지나는 점 사이의 거리가 r^2와 비슷할 시 출력 (오차 보정값 = f)
			if (distance >= (VISION_CIRCLE_RADIUS - f) * (VISION_CIRCLE_RADIUS - f) &&
				distance <= (VISION_CIRCLE_RADIUS + f) * (VISION_CIRCLE_RADIUS + f)) {
				visionBuf[visionCircleCenterY + y][visionCircleCenterX + x] = OBJ_VISION_OUTLINE;
			}

			// 원점과 반지름을 지나는 점 사이의 거리가 r^2와 "크거나 같을 시" 1 (오차 = f)
			// 원 안의 오브젝트가 원 밖으로 이탈하는 것을 방지하기 위함
			if (!isMadeSpaceBuf) {
				spaceBuf[visionCircleCenterY + y][visionCircleCenterX + x] = (distance >= (VISION_CIRCLE_RADIUS - f) * (VISION_CIRCLE_RADIUS - f)) ? 1 : 0;
			}
		}
	}
	isMadeSpaceBuf = 1;
}


// 플레이어를 숫자 형식으로 visionBuf에 저장
void drawPlayerBuffer() {
	int x = visionCircleCenterX - cameraX;
	int y = visionCircleCenterY - cameraY;

	if (isInsideVision(x, y) && isInsideVision(x + 1, y)) {
		visionBuf[y][x] = OBJ_PLAYER;
		visionBuf[y][x + 1] = OBJ_IGNORE;	// 2칸 차지하는 거 보정
	}
}


// 패턴 배경을 깔아서 카메라가 움직이는 듯한 효과 추가 (5칸 단위로 '+' 회색으로 넣으면 좋을듯)
void drawBackgroundBuffer() {
	int x, y;
	int tempCameraX = cameraX % 18;
	int tempCameraY = cameraY % 9;

	textcolor(COLOR_GRAY, COLOR_BLACK);
	for (x = 0; x < VISION_SQR_WIDTH; x += 18) {
		for (y = 0; y < VISION_SQR_HEIGHT; y += 9) {
			if (!visionBuf[y - tempCameraY][x - tempCameraX] && isInsideVision(x - tempCameraX, y - tempCameraY)) {	// 오브젝트가 없고, 시야 내에 있을 경우 출력
				printxy(x - tempCameraX + VISION_CIRCLE_SCR_CENTER_X - visionCircleCenterX, y - tempCameraY + VISION_CIRCLE_SCR_CENTER_Y - visionCircleCenterY, "+");
			}
		}
	}
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);		// 끝나기 전 다시 원래 색으로
}


// 저장된 visionBuf에 따라 문자로 바꿔서 최종 출력 (drawBox와 비슷함)
void printVisionBuffer() {
	int x, y;
	char buf[VISION_SQR_WIDTH + 1];		// 화면 가로 크기 + '\0'

	// 한 행 단위로 buf에 저장한 뒤 한번에 printscr 한다.
	for (y = 0; y < VISION_SQR_HEIGHT; y++) {
		strcpy(buf, "");	// 버퍼 초기화

		for (x = 0; x < VISION_SQR_WIDTH; x++) {
			switch (visionBuf[y][x]) {
				// 임시 버퍼 buf에 한 줄로 이어 붙인 뒤
			case OBJ_EMPTY:
				strcat(buf, " "); break;
			case OBJ_WAVE:
			case OBJ_VISION_OUTLINE:
				strcat(buf, "*"); break;
			case OBJ_NARROW_ANTENNA:
				strcat(buf, "▲"); break;
			case OBJ_PLAYER:
			case OBJ_MEDIUM_ANTENNA:
				strcat(buf, "●"); break;
			case OBJ_WIDE_ANTENNA:
				strcat(buf, "■"); break;
			case OBJ_IGNORE:
				break;
			default:
				strcat(buf, "?"); break;
			}
		}

		// 마지막에 hidden screen에 출력
		// (VISION_CIRCLE_SCR_CENTER_X - VISION_CIRCLE_RADIUS)			= 원점에서 반지름만큼 x방향으로 이동			 = 맨 왼쪽부터 시작
		// (VISION_CIRCLE_SCR_CENTER_Y - (VISION_CIRCLE_RADIUS/2)) + y  = 원점에서 반지름만큼 y방향으로 이동 + y		 = 맨 위쪽부터 시작해서 y값에 따라 아래로 내려옴
		printxy(VISION_CIRCLE_SCR_CENTER_X - VISION_CIRCLE_RADIUS, VISION_CIRCLE_SCR_CENTER_Y - (VISION_CIRCLE_RADIUS / 2) + y, buf);
	}
}


// 카메라 기능
void moveCamera() {
	unsigned char ch = getch();		// key 값을 읽는다

	switch (ch) {
	case UP:
		cameraY--;
		break;
	case DOWN:
		cameraY++;
		break;
	case LEFT:
		cameraX -= 2;
		break;
	case RIGHT:
		cameraX += 2;
		break;
	}
}