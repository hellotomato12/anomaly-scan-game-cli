#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include "constants.h"
#include "visionCircle.h"
#include "structs.h"
#include "anomaly.h"
#include "log.h"
#include "wave.h"
#include "antenna.h"
#include <mmsystem.h>
#include <process.h>
#pragma comment(lib, "winmm.lib")

// ---------------------------------
// 파동 관련 항목
// ---------------------------------

int waveCount = 0;		// 파동 카운트
int nextWaveCode = 1;	// 파동 고유번호


// 파동 고유 넘버 부여
int getNextWaveCode() {
	return nextWaveCode++;
}


// 시야 내 원형 파동을 숫자 형식으로 visionBuf에 저장 (drawVisionCircleBuffer와 비슷함)
// (반지름 짝수만 가능)
void drawWaveBuffer(WAVE* w) {
	int x, y;
	double distance;
	double f = 0.23 * log(w->r - 0.2) + 0.1;		// 보정값 (원의 크기가 달라도 항상 예쁘게 나오게)
	int screenX, screenY;							// 카메라 좌표에 따라 최종적으로 저장될 좌표

	for (x = -(w->r); x <= w->r; x++) {
		for (y = -(w->r / 2); y <= w->r / 2; y++) {
			distance = (double)(x * x) + (y * 2.0) * (y * 2.0);		// 원의 방정식: x^2 + y^2 = r^2

			// 원점과 반지름을 지나는 점 사이의 거리가 r^2와 비슷할 시 출력 (오차 보정값 = f)
			if (distance >= (w->r - f) * (w->r - f) &&
				distance <= (w->r + f) * (w->r + f)) {

				// (원점에서 떨어진 점) - (카메라 좌표) + (시야 원 원점)
				// (w->x + x) = 3		cameraX = -10	visionCircleCenterX = 36일 때
				// (w->x + x) + visionCircleCenterX = 3 + 36 = 39									(즉, 원점에서 3칸 오른쪽에 *이 찍힘)
				// (w->x + x) - cameraX + visionCircleCenterX = 3 - (-10) + 36 = 13 + 36 = 49		(시야 원에서 10칸 오른쪽으로 멀어짐)
				screenX = (w->x + x) - cameraX + visionCircleCenterX;
				screenY = (w->y + y) - cameraY + visionCircleCenterY;

				putGameBuffer(screenX, screenY, OBJ_WAVE);
			}
		}
	}
}

// 메인 파동 보내기
void sendMainWave(WAVE* w, int x, int y) {
	w->x = x;
	w->y = y;
	w->r = 0;		// 초기 파동(원) 크기
	w->prev_r = 0;	// 이전 프레임의 반지름 초기화
	w->max_r = (int)(BASE_WAVE_REACH * stats.waveReach) - (int)(BASE_WAVE_REACH * stats.waveReach) % 2;		// waveReach 값에 따라 최대 범위 증가
	w->active = 1;
	w->start_t = clock() * 1000 / CLOCKS_PER_SEC;
	w->code = getNextWaveCode();	// 파동 고유 넘버 부여

	// 로그 전송
	enqueueLog("신호를 보냈습니다... (gen 0)", COMMON_LOG);
}

// 파동 보내진 시간에 따라 업데이트
void updateWave(WAVE* w) {
	clock_t now = clock() * 1000 / CLOCKS_PER_SEC;
	int realWaveTime = (int)(BASE_WAVE_SPEED / stats.waveSpeed);	// max_r에 도달하는 데 걸리는 시간 , waveSpeed = 1.0일 때 10000, 2.0일 때 5000

	if (!w->active) return;

	w->prev_r = w->r;
	w->max_r = (int)(BASE_WAVE_REACH * stats.waveReach) - (int)(BASE_WAVE_REACH * stats.waveReach) % 2;		// waveReach 값에 따라 최대 범위 증가.

	if (now - w->start_t >= realWaveTime) {	// 처음 보내고 지난 시간이 max_r에 도달하는 데 걸리는 시간보다 크면
		w->r = w->max_r;
		//w->active = 0;
		return;
	}
	w->r = (int)((double)w->max_r * (now - w->start_t) / realWaveTime);			// max_r * (현재 지난 시간 / 총 걸리는 시간)
	w->r = w->r - (w->r % 2);		// 반지름 값 짝수만 나오게
}


// 원형 파동으로 이상체 스캔되었는지 확인
void isScanned(ANOMALY* anom, WAVE* w) {
	int dx, dy;
	double distance;
	int scanDamage = stats.coinsPerScan * 2;			// 이상체를 스캔한 수
	char buf[50];

	// 이미 해당 파동에 대해 스캔된 경우
	for (int i = 0; i < anom->cnt; i++) {
		if (w->code == anom->detectedBy[i]) {
			return;
		}
	}

	dx = anom->x - w->x;		// 파동의 원점은 항상 (0,0)이 아니므로 (x-a)^2 + (y-b)^2 = r^2을 이용
	dy = anom->y - w->y;
	distance = (double)(dx * dx) + (dy * 2.0) * (dy * 2.0);

	// 해당 파동의 새로운 스캔일 경우
	// 이상체와 파동의 원점의 거리가 파동의 이전 반지름보단 크고, 현재 반지름보단 작거나 같을 경우.
	// 즉, 이전 반지름의 바깥쪽, 현재 반지름의 안쪽, prev_r과 r 사이에 걸쳐있을 때
	if (distance > w->prev_r * w->prev_r &&
		distance <= w->r * w->r) {
		anom->detectedBy[anom->cnt++] = w->code;
		anom->detected = 1;
		anom->start_t = clock() * 1000 / CLOCKS_PER_SEC;		// 이상체 감지된 시간
		if (scanDamage > anom->life) scanDamage = anom->life;	// 남은 스캔 수보다 한번에 스캔하는 수가 클 경우 남은 스캔 수만큼만 차감
		anom->life -= scanDamage;
		if (anom->type == OBJ_ANOMALY) {						// 일반 이상체만 fieldValue에 반영됨
			fieldValue -= scanDamage;
			sprintf(buf, "이상체가 스캔됨, $%d 획득", stats.coinsPerScan);
			enqueueLog(buf, SCANNED_LOG);
			if (anom->life > 0) {
				sprintf(buf, "해당 이상체는 %d번 더 스캔 가능합니다.", (anom->life + scanDamage - 1) / scanDamage);
				enqueueLog(buf, COMMON_LOG);
			}
		}
		else {
			enqueueLog("--------------------------", SPECIAL_LOG);
			enqueueLog("* 특별 알림 *", SPECIAL_LOG);
			sprintf(buf, "특별 이상체가 스캔됨, $%d 획득", stats.coinsPerScan);
			enqueueLog(buf, SPECIAL_LOG);
			sprintf(buf, "해당 이상체는 %d번 더 스캔 가능합니다.", (anom->life + scanDamage - 1) / scanDamage);
			enqueueLog(buf, SPECIAL_LOG);
			enqueueLog("--------------------------", SPECIAL_LOG);
		}

		currentCoin += stats.coinsPerScan;
		valueProductionBuckets[currentProductionBucket] += stats.coinsPerScan;	// 초당 벌은 돈을 저장함

		// 효과음 출력
		PlaySound(TEXT("scanned.wav"), 0, SND_FILENAME | SND_ASYNC);
	}

	if (anom->life <= 0) {		// 완전 스캔될 시 해당 이상체 제거
		if (anom->type == OBJ_ANOMALY) {
			enqueueLog("이상체가 완전히 스캔됨.", FULLY_SCANNED_LOG);
		}
		// special Anomaly일 시 이벤트 발생
		else if (anom->type == OBJ_SPECIAL_ANOMALY) {
			scannedSpecialAnomalies++;
		}
		// anomalyEcho 활성화 되었을 시 그 자리에 파동 생성
		if (stats.anomalyEcho == 1) {
			sendMainWave(&wave[waveCount++], anom->x, anom->y);
			if (waveCount == MAX_CIRCLE_WAVES)
				waveCount = 0;
		}
		deleteAnomaly(anom);
	}
}


// 안테나 파동으로 이상체 스캔되었는지 확인
void isScannedByAntenna(ANOMALY* anom, ANTENNA_WAVE* w) {
	int dx, dy;
	double distance;
	int scanDamage = stats.coinsPerScan * 2;			// 이상체를 스캔한 수
	char buf[50];

	// 이미 해당 파동에 대해 스캔된 경우
	for (int i = 0; i < anom->cnt; i++) {
		if (w->code == anom->detectedBy[i]) {
			return;
		}
	}

	dx = anom->x - w->x;		// 파동의 원점은 항상 (0,0)이 아니므로 (x-a)^2 + (y-b)^2 = r^2을 이용
	dy = anom->y - w->y;

	// 안테나 파동 부채꼴 범위 밖이면 스캔 X
	if (!isInsideAntennaWaveAngle(w, dx, dy)) {
		return;
	}

	distance = (double)(dx * dx) + (dy * 2.0) * (dy * 2.0);

	// 해당 파동의 새로운 스캔일 경우
	// 이상체와 파동의 원점의 거리가 파동의 이전 반지름보단 크고, 현재 반지름보단 작거나 같을 경우.
	// 즉, 이전 반지름의 바깥쪽, 현재 반지름의 안쪽, prev_r과 r 사이에 걸쳐있을 때
	if (distance > w->prev_r * w->prev_r &&
		distance <= w->r * w->r) {
		anom->detectedBy[anom->cnt++] = w->code;
		anom->detected = 1;
		anom->start_t = clock() * 1000 / CLOCKS_PER_SEC;		// 이상체 감지된 시간
		if (scanDamage > anom->life) scanDamage = anom->life;	// 남은 스캔 수보다 한번에 스캔하는 수가 클 경우 남은 스캔 수만큼만 차감
		anom->life -= scanDamage;
		if (anom->type == OBJ_ANOMALY) {						// 일반 이상체만 fieldValue에 반영됨
			fieldValue -= scanDamage;
			sprintf(buf, "이상체가 스캔됨, $%d 획득", stats.coinsPerScan);
			enqueueLog(buf, SCANNED_LOG);
			if (anom->life > 0) {
				sprintf(buf, "해당 이상체는 %d번 더 스캔 가능합니다.", (anom->life + scanDamage - 1) / scanDamage);
				enqueueLog(buf, COMMON_LOG);
			}
		}
		else {
			enqueueLog("--------------------------", SPECIAL_LOG);
			enqueueLog("* 특별 알림 *", SPECIAL_LOG);
			sprintf(buf, "특별 이상체가 스캔됨, $%d 획득", stats.coinsPerScan);
			enqueueLog(buf, SPECIAL_LOG);
			sprintf(buf, "해당 이상체는 %d번 더 스캔 가능합니다.", (anom->life + scanDamage - 1) / scanDamage);
			enqueueLog(buf, SPECIAL_LOG);
			enqueueLog("--------------------------", SPECIAL_LOG);
		}

		currentCoin += stats.coinsPerScan;
		valueProductionBuckets[currentProductionBucket] += stats.coinsPerScan;	// 초당 벌은 돈을 저장함

		// 효과음 출력
		PlaySound(TEXT("scanned.wav"), 0, SND_FILENAME | SND_ASYNC);
	}

	if (anom->life <= 0) {		// 완전 스캔될 시 해당 이상체 제거
		if (anom->type == OBJ_ANOMALY) {
			enqueueLog("이상체가 완전히 스캔됨.", FULLY_SCANNED_LOG);
		}
		// special Anomaly일 시 이벤트 발생
		else if (anom->type == OBJ_SPECIAL_ANOMALY) {
			scannedSpecialAnomalies++;
		}
		// anomalyEcho 활성화 되었을 시 그 자리에 파동 생성
		if (stats.anomalyEcho == 1) {
			sendMainWave(&wave[waveCount++], anom->x, anom->y);
			if (waveCount == MAX_CIRCLE_WAVES)
				waveCount = 0;
		}
		deleteAnomaly(anom);
	}
}


// 안테나 타입에 따른 거리 반환
int getAntennaWaveRange(int type) {
	switch (type) {
	case NARROW_ANTENNA: return (int)(104 * stats.waveReach);	// waveReach 값에 따라 최대 범위 증가
	case MEDIUM_ANTENNA: return (int)(58 * stats.waveReach);
	case WIDE_ANTENNA: return (int)(40 * stats.waveReach);
	}
	return 0;
}


// 안테나 타입에 따른 중심각 반환
int getAntennaWaveSpreadAngle(int type) {
	switch (type) {
	case NARROW_ANTENNA: return 6;
	case MEDIUM_ANTENNA: return 90;
	case WIDE_ANTENNA: return 160;
	}
	return 0;
}


// 안테나 파동 생성
void sendAntennaWave(ANTENNA* antenna, int antennaIndex, int chain) {
	ANTENNA_WAVE* w;
	char buf[30];

	if (((atn_w_q.rear + 1) % MAX_ANTENNA_WAVES) == atn_w_q.front) {		// 큐가 포화 상태일 경우
		// 여기에 경고 문구 화면에 출력하게 할 예정
		return;
	}

	atn_w_q.rear = (atn_w_q.rear + 1) % MAX_ANTENNA_WAVES;				// 마지막 요소 인덱스를 다음으로 넘김
	w = &atn_w_q.wave[atn_w_q.rear];

	w->x = antenna->x;													// 안테나 파동이 시작되는 위치
	w->y = antenna->y;
	w->r = 0;
	w->prev_r = 0;

	w->max_r = getAntennaWaveRange(antenna->type);
	w->max_r = w->max_r - (w->max_r % 2);

	w->angle = antenna->angle;
	w->spreadAngle = getAntennaWaveSpreadAngle(antenna->type);

	w->chain = chain;
	w->myAntennaIndex = antennaIndex;

	w->active = 1;
	w->start_t = clock() * 1000 / CLOCKS_PER_SEC;
	w->code = getNextWaveCode();

	// 로그 전송
	sprintf(buf, "신호를 보냈습니다... (gen %d)", w->chain);
	enqueueLog(buf, COMMON_LOG);
}


// 원형 파동에 닿았을 시 파동 증폭
void tryAmplifyCircleWave(WAVE* w, ANTENNA* antenna, int antennaIndex) {
	if (isCircleWaveTouching(w, antenna)) {
		sendAntennaWave(antenna, antennaIndex, 1);
		antenna->triggeredBy[antenna->cnt++] = w->code;	// 자신한테 닿은 파동을 저장함
	}
}


// 안테나 파동에 닿았을 시 파동 증폭
void tryAmplifyAntennaWave(ANTENNA_WAVE* w, ANTENNA* antenna, int antennaIndex) {

	if (w->chain >= stats.chainDepth) {		// chainDepth 이상 됐을 시 chain 안 함
		return;
	}

	if (antennaIndex == w->myAntennaIndex) {	// 자기 안테나 파동에 증폭 안 함
		return;
	}

	if (isAntennaWaveTouching(w, antenna)) {
		sendAntennaWave(antenna, antennaIndex, (w->chain + 1));
		antenna->triggeredBy[antenna->cnt++] = w->code;	// 자신한테 닿은 파동을 저장함
	}
}


// 개체가 안테나 파동의 방향 및 범위 안에 있는지 확인
int isInsideAntennaWaveAngle(ANTENNA_WAVE* w, int dx, int dy) {
	double angleToPoint;

	// dx, dy의 위치를 각도로 변환함
	angleToPoint = atan2(-(double)dy * 2.0, (double)dx) * 180.0 / PI;	// -dy = 콘솔 좌표랑 반대라서 보정, * 180.0 / PI = 라디안을 각도로 변환
	angleToPoint = normalizeAngle((int)angleToPoint);

	// angleToPoint가 angle 각도에서 spreadAngle 폭만큼 
	if (angleDiff((int)angleToPoint, w->angle) <= w->spreadAngle / 2.0)
		return 1;

	return 0;
}


// 시야 내 안테나 파동을 숫자 형식으로 visionBuf에 저장
// (반지름 짝수만 가능)
void drawAntennaWaveBuffer(ANTENNA_WAVE* w) {
	int x, y;
	double distance;
	double f = 0.15 * log(w->r - 0.2) + 0.2;		// 보정값 (원의 크기가 달라도 항상 예쁘게 나오게)
	int screenX, screenY;							// 카메라 좌표에 따라 최종적으로 저장될 좌표

	for (x = -(w->r); x <= w->r; x++) {
		for (y = -(w->r / 2); y <= w->r / 2; y++) {
			distance = (double)(x * x) + (y * 2.0) * (y * 2.0);		// 원의 방정식: x^2 + y^2 = r^2

			// 원점과 반지름을 지나는 점 사이의 거리가 r^2와 비슷할 시 출력 (오차 보정값 = f)
			if (distance >= (w->r - f) * (w->r - f) &&
				distance <= (w->r + f) * (w->r + f)) {

				if (!isInsideAntennaWaveAngle(w, x, y)) {		// 안테나 파동의 각도 폭에 없을 경우 출력 X (부채꼴 모양 만듦)
					continue;
				}

				screenX = (w->x + x) - cameraX + visionCircleCenterX;
				screenY = (w->y + y) - cameraY + visionCircleCenterY;

				putGameBuffer(screenX, screenY, OBJ_WAVE);
			}
		}
	}
}


// 안테나 파동 보내진 시간에 따라 업데이트
void updateAntennaWave(ANTENNA_WAVE* w) {
	clock_t now = clock() * 1000 / CLOCKS_PER_SEC;
	int realWaveTime = (int)(BASE_WAVE_SPEED / stats.waveSpeed);						// max_r에 도달하는 데 걸리는 시간 , waveSpeed = 1.0일 때 10000, 2.0일 때 5000

	w->prev_r = w->r;
	w->max_r = getAntennaWaveRange(atn_q.antenna[w->myAntennaIndex].type);		// waveReach 값에 따라 최대 범위 증가.
	w->max_r = w->max_r - (w->max_r % 2);

	if (now - w->start_t >= realWaveTime) {										// 처음 보내고 지난 시간이 max_r에 도달하는 데 걸리는 시간보다 크면
		w->r = w->max_r;
		w->active = 0;															// 해당 안테나 파동 inactive하고
		atn_w_q.front = (atn_w_q.front + 1) % MAX_ANTENNA_WAVES;				// front++
		return;
	}
	w->r = (int)((double)w->max_r * (now - w->start_t) / realWaveTime);			// max_r * (현재 지난 시간 / 총 걸리는 시간)
	w->r = w->r - (w->r % 2);													// 반지름 값 짝수만 나오게
}