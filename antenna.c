#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "constants.h"
#include "structs.h"
#include "wave.h"
#include "visionCircle.h"

// ---------------------------------
// 안테나 관련 항목
// ---------------------------------

ANTENNA* selectedAntenna;					// 선택된 안테나
int selectedAntennaType = -1;				// 선택된 안테나 타입
int antennaEditMode = 0;					// 안테나 위치/방향 편집 모드


// 안테나 타입 확인
int findAntennaType(unsigned char key) {
	if (key == KEY_Q) return NARROW_ANTENNA;
	if (key == KEY_W) return MEDIUM_ANTENNA;
	if (key == KEY_E) return WIDE_ANTENNA;

	return -1;
}


// 안테나 가격
int getAntennaPrice(int type) {
	if (type == NARROW_ANTENNA) return 10;	// 가격: 10원
	if (type == MEDIUM_ANTENNA) return 38;	// 가격: 38원
	if (type == WIDE_ANTENNA) return 72;	// 가격: 72원

	return 0;
}


// 안테나 생성
void createAntenna(unsigned char key) {
	int type = findAntennaType(key);					// 안테나 타입
	selectedAntennaType = type;
	ANTENNA* antenna;

	// 현재 소지한 코인이 부족하면 구매 X
	if (currentCoin < getAntennaPrice(type))
		return;

	if (((atn_q.rear + 1) % MAX_ANTENNAS) == atn_q.front) {		// 큐가 포화 상태일 경우
		return;
	}

	atn_q.rear = (atn_q.rear + 1) % MAX_ANTENNAS;				// 마지막 요소 인덱스를 다음으로 넘김
	antenna = &atn_q.antenna[atn_q.rear];

	// 안테나 생성
	antenna->angle = 0;					// 안테나 방향 (기본: 0도)
	antenna->type = type;				// 안테나 타입
	antenna->x = cameraX;				// 안테나 좌표 (기본: 화면 중앙)
	antenna->y = cameraY;
	antenna->start_t = clock() * 1000 / CLOCKS_PER_SEC;		// 안테나가 생성된 시간
	antenna->cnt = 0;					// 파동과 닿은 횟수

	// 돈 차감
	currentCoin -= getAntennaPrice(type);

	// 바로 해당 안테나에 대해 편집 모드 진입
	selectedAntenna = antenna;
	antennaEditMode = 1;

	// 처음 안테나 설치 시 openStat = ANOMALY_VALUE까지
	if (atn_q.rear == 1) openStat = ANOMALY_VALUE;
}


// 안테나 삭제
void deleteAntenna() {
	ANTENNA* antenna;
	clock_t now = clock() * 1000 / CLOCKS_PER_SEC;
	int next;

	while (atn_q.front != atn_q.rear) {	// 큐가 비어있지 않을 경우
		next = (atn_q.front + 1) % MAX_ANTENNAS;	// front + 1부터 시작
		antenna = &atn_q.antenna[next];

		// 안테나가 아직 살아있으면 삭제 X
		if (now - antenna->start_t <= (stats.antennaLifetime * 1000)) {
			break;
		}

		// 선택된 안테나가 삭제될 안테나일 시
		if (selectedAntenna == antenna) {
			selectedAntenna = NULL;
			selectedAntennaType = -1;
			antennaEditMode = 0;
		}

		// 큐에서 제거
		atn_q.front = next;
	}
}


// 현재 시야에 존재하는 안테나를 visionBuf에 저장
void drawAntennaBuffer() {
	int x, y;
	int obj;
	ANTENNA* antenna;

	if (atn_q.front != atn_q.rear) {	// 큐가 비어있지 않을 경우
		int i = atn_q.front;

		while (i != atn_q.rear) {
			i = (i + 1) % MAX_ANTENNAS;		// front + 1부터 시작
			antenna = &atn_q.antenna[i];

			// 안테나의 타입 확인
			switch (antenna->type) {
			case NARROW_ANTENNA:
				obj = OBJ_NARROW_ANTENNA;
				break;
			case MEDIUM_ANTENNA:
				obj = OBJ_MEDIUM_ANTENNA;
				break;
			case WIDE_ANTENNA:
				obj = OBJ_WIDE_ANTENNA;
				break;
			default:
				obj = OBJ_IGNORE;
				break;
			}

			x = antenna->x - cameraX + visionCircleCenterX;
			y = antenna->y - cameraY + visionCircleCenterY;

			// visionBuf에 오브젝트 저장
			if (isInsideVision(x, y) && isInsideVision(x + 1, y)) {
				visionBuf[y][x] = obj;
				visionBuf[y][x + 1] = OBJ_IGNORE;	// 2칸 차지하는 거 보정
			}
		}
	}
}


// 해당 위치에 안테나가 이미 있는지 검사
int isThereAnAntenna(int x, int y, ANTENNA* itself) {
	ANTENNA* antenna;
	if (atn_q.front != atn_q.rear) {	// 큐가 비어있지 않을 경우
		int i = atn_q.front;

		while (i != atn_q.rear) {
			i = (i + 1) % MAX_ANTENNAS;		// front + 1부터 시작
			antenna = &atn_q.antenna[i];

			if (antenna == itself) {		// 자기 자신일 때
				if (x == 0 && y == 0)		// 플레이어랑 겹치는지만 확인
					return 1;
				else continue;				// 아니면 넘어감 
			}

			if ((antenna->x == x && antenna->y == y) || x == 0 && y == 0)	// x y 위치에 안테나가 있거나 원점(플레이어 위치)일 때
				return 1;
		}
	}
	return 0;
}


// 안테나 이동
void moveSelectedAntenna() {
	unsigned char ch = getch();		// key 값을 읽는다

	if (selectedAntenna == NULL) {
		return;
	}

	switch (ch) {
	case UP:
		if (!isInsideVision(selectedAntenna->x - cameraX + visionCircleCenterX, selectedAntenna->y - 1 - cameraY + visionCircleCenterY) ||
			!isInsideVision(selectedAntenna->x + 1 - cameraX + visionCircleCenterX, selectedAntenna->y - 1 - cameraY + visionCircleCenterY))	// 시야의 끝에 가면 카메라가 같이 움직임
			cameraY--;
		if (!isThereAnAntenna(selectedAntenna->x, selectedAntenna->y - 1, selectedAntenna))		// 해당 위치에 이미 안테나가 없을 시
			selectedAntenna->y--;
		break;
	case DOWN:
		if (!isInsideVision(selectedAntenna->x - cameraX + visionCircleCenterX, selectedAntenna->y + 1 - cameraY + visionCircleCenterY) ||
			!isInsideVision(selectedAntenna->x + 1 - cameraX + visionCircleCenterX, selectedAntenna->y + 1 - cameraY + visionCircleCenterY))
			cameraY++;
		if (!isThereAnAntenna(selectedAntenna->x, selectedAntenna->y + 1, selectedAntenna))
			selectedAntenna->y++;
		break;
	case LEFT:
		if (!isInsideVision(selectedAntenna->x - cameraX - 2 + visionCircleCenterX, selectedAntenna->y - cameraY + visionCircleCenterY))
			cameraX -= 2;
		if (!isThereAnAntenna(selectedAntenna->x - 2, selectedAntenna->y, selectedAntenna))
			selectedAntenna->x -= 2;
		break;
	case RIGHT:
		if (!isInsideVision(selectedAntenna->x + 2 - cameraX + visionCircleCenterX, selectedAntenna->y - cameraY + visionCircleCenterY) ||
			!isInsideVision(selectedAntenna->x + 3 - cameraX + visionCircleCenterX, selectedAntenna->y - cameraY + visionCircleCenterY))
			cameraX += 2;
		if (!isThereAnAntenna(selectedAntenna->x + 2, selectedAntenna->y, selectedAntenna))
			selectedAntenna->x += 2;
		break;
	}
}


// 다른 안테나 선택
void changeSelectedAntenna() {
	static int idx;

	if (selectedAntenna == NULL) idx = atn_q.front;

	if (atn_q.front != atn_q.rear) {
		if (idx != atn_q.rear) {
			idx++;
		}
		else {
			idx = atn_q.front + 1;
		}
		selectedAntenna = &atn_q.antenna[idx];
		cameraX = selectedAntenna->x;
		cameraY = selectedAntenna->y;
		antennaEditMode = 1;
	}
}


// 안테나가 원형 파동에 닿았는지 확인 (isScanned랑 비슷)
int isCircleWaveTouching(WAVE* w, ANTENNA* antenna) {
	// dx, dy = 원형 파동의 원점을 기준으로 이상체와의 거리
	int dx = antenna->x - w->x;
	int dy = antenna->y - w->y;
	double distance = (double)(dx * dx) + (dy * 2.0) * (dy * 2.0);

	// 이미 해당 파동에 대해 스캔된 경우 판정 X
	for (int i = 0; i < antenna->cnt; i++) {
		if (w->code == antenna->triggeredBy[i]) {
			return 0;
		}
	}

	if (distance > w->prev_r * w->prev_r &&
		distance <= w->r * w->r) {
		return 1;
	}

	return 0;
}


// 안테나가 안테나 파동에 닿았는지 확인
int isAntennaWaveTouching(ANTENNA_WAVE* w, ANTENNA* antenna) {
	// dx, dy = 안테나 파동의 시작점을 기준으로 이상체와의 거리
	int dx = antenna->x - w->x;
	int dy = antenna->y - w->y;
	double distance;

	// 이미 해당 파동에 대해 스캔된 경우 판정 X
	for (int i = 0; i < antenna->cnt; i++) {
		if (w->code == antenna->triggeredBy[i]) {
			return 0;
		}
	}

	if (!isInsideAntennaWaveAngle(w, dx, dy))
		return 0;

	distance = (double)(dx * dx) + (dy * 2.0) * (dy * 2.0);

	if (distance > w->prev_r * w->prev_r &&
		distance <= w->r * w->r) {
		return 1;
	}

	return 0;
}


// 안테나 회전 (5도씩 회전)
void rotateSelectedAntenna(int key) {
	if (key == KEY_D)	// D키 = 시계 방향으로 회전
		selectedAntenna->angle = (selectedAntenna->angle + 355) % 360;
	else if (key == KEY_A)	// A키 = 반시계 방향으로 회전
		selectedAntenna->angle = (selectedAntenna->angle + 5) % 360;
}


// 안테나 각도 보정
int normalizeAngle(int angle) {
	while (angle < 0) angle += 360;			// 각도가 0도보다 작으면 +360으로 보정
	while (angle >= 360) angle -= 360;		// 각도가 360보다 크면 -360으로 보정

	return angle;
}

// 두 방향이 얼마나 벌어져 있는지 계산
int angleDiff(int a, int b) {
	int diff;

	a = normalizeAngle(a);
	b = normalizeAngle(b);

	diff = (int)fabs(a - b);

	if (diff > 180)
		diff = 360 - diff;

	return diff;
}