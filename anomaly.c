#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "doubleBuffering.h"
#include "visionCircle.h"
#include "structs.h"
#include "anomaly.h"
#include "wave.h"

// ---------------------------------
// 이상체 관련 항목
// ---------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이상체 생성 관련 로직 //


int fieldValue;					// 맵 내 이상체들의 총 life 수
double fieldCoin;				// 맵 내 이상체들이 줄 수 있는 총 코인량
double desiredValue = 36.0;		// 이상체 생성 기준 1, 현재 맵 내 이상체들의 총 life 수와 관련됨
int anchorCount;				// 이상체 생성 기준 2, 현재 존재하는 anchor의 수. anchor = 이상체 스폰의 기준점. 새 이상체는 anchor 근처의 적절한 위치에 생성됨.

// 이상체 생성 기준 3
int valueProductionBuckets[60] = { 0, };	// 60초 동안 벌은 코인을 저장함
int currentProductionBucket = 0;			// 기준 시간에서 지난 시간
clock_t lastProductionBucketTime = 0;		// 기준 시간

clock_t nextAnomalySpawnTime = 0;		// 이상체가 스폰되는 주기

// anchor들의 중심점 좌표
double spawnCenterX;
double spawnCenterY;

// 60초 동안 스캔한 횟수를 구하기
void updateValueProductionBucket() {
	clock_t now = clock() * 1000 / CLOCKS_PER_SEC;

	if (lastProductionBucketTime == 0) {	// 기준 시간이 없을 경우 현재 시간 넣고 반환
		lastProductionBucketTime = now;
		return;
	}

	while (now - lastProductionBucketTime >= 1000) {	// 1초 지났을 경우
		currentProductionBucket = (currentProductionBucket + 1) % 60;
		valueProductionBuckets[currentProductionBucket] = 0;
		lastProductionBucketTime += 1000;
	}
}


// 60초 동안 스캔한 횟수의 평균을 구하기
double getAverageCoinProductionPerSecond() {
	int i;
	int total = 0;

	for (i = 0; i < 60; i++) {
		total += valueProductionBuckets[i];
	}

	return total / 60.0;
}


// anomalyValue를 구한다.
int getAnomalyValue() {
	if (stats.anomalyValue < stats.coinsPerScan * 2) {		// anomaly value < coinsPerScan일 경우 anomaly value = coinsPerScan * 5로 보정
		return stats.coinsPerScan * 5;
	}
	return stats.anomalyValue;
}


// 현재 anchor 목록을 최신 상태로 만든다.
void refreshAnchors() {
	int i;
	double centerX = 0;							// anchor들의 중심점
	double centerY = 0;							// anchor들의 중심점
	double dx;
	double dy;
	double distance;
	double rad;
	int waveAnchorCount = 0;										// 안테나 파동 anchor 전용 카운터
	int waveAnchorLimit = fieldValue / getAnomalyValue();			// 안테나 파동 anchor 수 제한
	waveAnchorLimit = CLAMP(waveAnchorLimit / 3, 1, 5);
	anchorCount = 0;			// 초기화

	// 1. 기본 원점 기준 anchor (메인 안테나)
	anchor[anchorCount].x = 0;
	anchor[anchorCount].y = 0;
	anchor[anchorCount].type = ANCHOR_ORIGIN;
	anchor[anchorCount].weight = 8.0;
	anchorCount++;

	// 2. 설치된 안테나 기준 anchor
	i = atn_q.front;

	while (i != atn_q.rear && anchorCount < MAX_ANCHORS) {
		i = (i + 1) % MAX_ANTENNAS;

		anchor[anchorCount].x = atn_q.antenna[i].x;
		anchor[anchorCount].y = atn_q.antenna[i].y;
		anchor[anchorCount].type = ANCHOR_ANTENNA;
		anchor[anchorCount].weight = 2.0;
		anchorCount++;
	}

	// 3. 안테나 파동 기준 anchor
	i = atn_w_q.front;

	while (i != atn_w_q.rear && anchorCount < MAX_ANCHORS && waveAnchorCount < waveAnchorLimit) {
		i = (i + 1) % MAX_ANTENNA_WAVES;

		if (!atn_w_q.wave[i].active) continue;

		rad = atn_w_q.wave[i].angle * PI / 180.0;

		anchor[anchorCount].x = atn_w_q.wave[i].x + (int)round(cos(rad) * atn_w_q.wave[i].r);
		anchor[anchorCount].y = atn_w_q.wave[i].y - (int)round(sin(rad) * atn_w_q.wave[i].r / 2.0);
		anchor[anchorCount].type = ANCHOR_ANTENNA_WAVE;
		anchor[anchorCount].weight = 1.2;
		anchorCount++;
		waveAnchorCount++;
	}

	// 4. 중심점 계산
	for (i = 0; i < anchorCount; i++) {
		centerX += anchor[i].x;
		centerY += anchor[i].y;
	}
	centerX /= anchorCount;
	centerY /= anchorCount;

	spawnCenterX = centerX;
	spawnCenterY = centerY;

	// 5. 타입별 가중치에 대한 거리 보정 살짝 곱함
	for (i = 0; i < anchorCount; i++) {
		dx = anchor[i].x - centerX;					// 각 anchor가 중심점으로부터 떨어진 위치
		dy = (anchor[i].y - centerY) * 2.0;
		distance = sqrt(dx * dx + dy * dy);

		if (anchor[i].type == ANCHOR_ORIGIN)
			anchor[i].weight = 8.0;
		else if (anchor[i].type == ANCHOR_ANTENNA)
			anchor[i].weight = 2.0 + sqrt(distance) * 0.05;
		else if (anchor[i].type == ANCHOR_ANTENNA_WAVE)
			anchor[i].weight = 1.2 + sqrt(distance) * 0.01;
	}
}


// anchorCount를 반환한다.
int getAnchorCount() {
	refreshAnchors();
	return anchorCount;
}


double getDesiredValue() {
	int anchorCount = getAnchorCount();																						// 이상체 스폰 기준점을 센다.
	double multiplier = CLAMP(2.0 + anchorCount * 0.35, 3.0, 8.0);															// anchorCount에 따라 기본 목표 배율을 조정한다.
	double anchorDesiredValue = getAnomalyValue() * multiplier;																// anomalyValue * 배율로 anchor 기준 목표치를 만듦
	double productionDesiredValue = getAverageCoinProductionPerSecond() * 23.5 * (stats.coinsPerScan * 2.0);
	if (productionDesiredValue > anchorDesiredValue) {																		// 둘 중에 큰 거 리턴
		return productionDesiredValue;
	}
	return anchorDesiredValue;
}

// 해당 위치에 이상체가 이미 존재하는지 확인 (x, y = 월드 좌표 기준)
int isAlreadyExists(ANOMALY* anom, int x, int y) {
	int i;
	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anom[i].active) continue;

		if (anom[i].x == x && anom[i].y == y)
			return 1;
	}
	return 0;
}


// 이상체가 메인 안테나의 최대 파동 범위 내에 있는지 확인
int isInsideMainReach(int dx, int dy) {
	double distance;
	int nearR;
	nearR = (int)(BASE_WAVE_REACH * stats.waveReach);
	nearR = nearR - (nearR % 2);	// 짝수 보정

	distance = (double)(dx * dx) + (dy * 2.0) * (dy * 2.0);

	// 이상체와 메인 안테나 파동의 원점(0, 0) 거리가 반지름 nearR보다 작으면 cnt++ (즉, 이상체가 nearR인 원 안에 있음)
	if (distance <= (nearR * nearR)) {
		return 1;
	}

	return 0;
}


// 메인 파동 최대 범위 내에 있는 이상체 수 세기
int countMainReachAnomalies(ANOMALY* anom) {
	int i;
	int cnt = 0;

	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anom[i].active) continue;

		if (isInsideMainReach(anom[i].x, anom[i].y)) {
			cnt++;
		}
	}

	return cnt;
}


// 새 이상체의 위치가 안전한지 검사
int isSpawnPositionSafe(int x, int y) {				// x, y = 새로 생성하려는 이상체 좌표
	int i;
	int dx, dy;
	double distance;

	// 1. 기존 이상체랑 가까우면 생성 X
	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anomaly[i].active) continue;

		dx = x - anomaly[i].x;
		dy = y - anomaly[i].y;

		distance = (dx * dx) + (dy * 2.0) * (dy * 2.0);

		// 새 이상체 스폰 위치와 기존 이상체의 거리가 SPAWN_SEPERATION보다 적으면(가까우면) 0
		if (distance < (SPAWN_SEPERATION * SPAWN_SEPERATION)) {
			return 0;
		}
	}

	// 2. 기존 안테나랑 가까우면 생성 X
	i = atn_q.front;
	while (i != atn_q.rear) {
		i = (i + 1) % MAX_ANTENNAS;

		dx = x - atn_q.antenna[i].x;
		dy = y - atn_q.antenna[i].y;

		distance = (dx * dx) + (dy * 2.0) * (dy * 2.0);

		if (distance < (SPAWN_SEPERATION * SPAWN_SEPERATION)) {
			return 0;
		}
	}

	// 3. 원형 파동 테두리랑 가까우면 생성 X
	for (i = 0; i < MAX_CIRCLE_WAVES; i++) {
		if (!wave[i].active) continue;

		dx = x - wave[i].x;
		dy = y - wave[i].y;

		distance = sqrt((dx * dx) + (dy * 2.0) * (dy * 2.0));

		if (distance >= wave[i].r && distance - wave[i].r <= WAVE_SEPERATION) {
			return 0;
		}
	}

	// 4. 안테나 파동 테두리랑 가까우면 생성 X
	i = atn_w_q.front;

	while (i != atn_w_q.rear) {
		i = (i + 1) % MAX_ANTENNA_WAVES;

		if (!atn_w_q.wave[i].active) continue;

		dx = x - atn_w_q.wave[i].x;
		dy = y - atn_w_q.wave[i].y;

		// 안테나 파동의 부채꼴 각도 안에 있는지 확인
		if (!isInsideAntennaWaveAngle(&atn_w_q.wave[i], dx, dy))
			continue;

		distance = sqrt((dx * dx) + (dy * 2.0) * (dy * 2.0));

		if (distance >= atn_w_q.wave[i].r && distance - atn_w_q.wave[i].r <= WAVE_SEPERATION) {
			return 0;
		}
	}

	return 1;
}


// 랜덤한 anchor를 반환한다.
ANCHOR* chooseRandomAnchor() {
	double totalWeight = 0.0;			// 이상체들의 모든 가중치의 총합
	double pick;						// 그 중에서 뽑은 랜덤 수
	double acc = 0.0;					// 이상체들의 모든 가중치의 누적합
	int i;

	if (anchorCount <= 0)				// anchor 없으면 NULL 반환
		return NULL;

	// 모든 anchor 가중치 더함
	for (i = 0; i < anchorCount; i++) {
		totalWeight += anchor[i].weight;
	}

	if (totalWeight <= 0.0) {
		return &anchor[rand() % anchorCount];
	}

	// 가중치 중에서 랜덤 숫자를 하나 뽑음
	pick = (rand() / (double)RAND_MAX) * totalWeight;

	for (i = 0; i < anchorCount; i++) {
		acc += anchor[i].weight;

		// 누적합 acc가 랜덤으로 뽑은 pick 이상일 시 해당 anchor 반환
		if (pick <= acc) {
			return &anchor[i];
		}
	}

	return &anchor[rand() % anchorCount];
}


// 기존 이상체 중에 anchor 근처에 있는 후보를 고름
// 30% 확률로 선택된 anchor 근처에 있는 이상체 하나를 고른다.
int getClusterOrigin(ANCHOR* anchor, int* outX, int* outY) {
	int arr[MAX_ANOMALIES];		// 클러스터 후보 이상체의 인덱스 저장
	int cnt = 0;				// 후보 수
	int i;
	int dx, dy;					// anchor와 이상체 사이의 거리 좌표
	double distance;
	int r = CLUSTER_SEARCH_RADIUS;

	// 30% 확률을 뚫지 못하면 넘어감
	if ((rand() % 100) >= CLUSTER_CHANCE)
		return 0;

	// 후보 탐색
	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anomaly[i].active) continue;

		dx = anomaly[i].x - anchor->x;
		dy = anomaly[i].y - anchor->y;
		distance = (double)(dx * dx) + (dy * 2.0) * (dy * 2.0);

		// anchor와 이상체 사이의 거리가 CLUSTER_SEARCH_RADIUS 이하일 때, 해당 이상체에 대한 인덱스 저장
		if (distance <= (r * r)) {
			arr[cnt++] = i;
		}
	}

	if (!cnt) return 0;			// 후보 없으면 넘어감

	// 후보 중에 랜덤한 이상체 하나를 골라서 x, y 좌표를 반환한다.
	i = arr[rand() % cnt];
	*outX = anomaly[i].x;
	*outY = anomaly[i].y;

	return 1;
}


int isTooCloseToOrigin(int x, int y) {
	double distance;

	distance = (double)(x * x) + (y * 2.0) * (y * 2.0);

	if (distance < MIN_ORIGIN_ANOMALY_R * MIN_ORIGIN_ANOMALY_R) {
		return 1;
	}

	return 0;
}


// 이상체 생성
void createAnomaly(ANOMALY* anomaly) {
	int i, j;
	int x, y;
	double angle;
	double spread = PI / 3.0;					// (60도, 라디안) 
	int distance;
	ANCHOR* anchor;
	int originX, originY;						// anchor의 원점 좌표
	int minDist, maxDist;						// 이상체 생성 최소 범위와 최대 범위
	double dx, dy;								// anchor와 anchor 중심점의 거리
	double baseAngle;
	int clusterSuccess;

	// inactive된 배열의 인덱스를 찾는다.
	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anomaly[i].active)
			break;
	}

	if (i == MAX_ANOMALIES) return;

	anchor = chooseRandomAnchor();		// 랜덤한 anchor를 선택
	if (anchor == NULL) return;

	// anchor 근처에 있는 이상체를 찾아본다. 성공 시 해당 이상체 주변 생성, 실패 시 anchor 주변 생성
	clusterSuccess = getClusterOrigin(anchor, &originX, &originY);

	if (clusterSuccess) {
		minDist = CLUSTER_MIN_DISTANCE;
		maxDist = CLUSTER_MAX_DISTANCE;
	}
	else {
		originX = anchor->x;
		originY = anchor->y;
		minDist = MIN_SPAWN_DISTANCE;
		maxDist = MAX_SPAWN_DISTANCE;
	}

	// anchor와 anchor 중심점의 거리
	dx = anchor->x - spawnCenterX;
	dy = (anchor->y - spawnCenterY) * 2.0;
	baseAngle = atan2(dy, dx);					// 중심점을 기준으로 anchor의 각도

	// 새 이상체 위치가 기존 이상체보다 가까우면 다시 생성 (30회 시도)
	for (j = 0; j < 30; j++) {
		// 좌표 생성
		if (clusterSuccess || (anchor->x == 0 && anchor->y == 0)) {
			angle = (rand() / (double)RAND_MAX) * 2.0 * PI;										// = rand() % 360.0 (라디안으로 변환)
		}
		else {
			angle = baseAngle + (((rand() / (double)RAND_MAX) * 2.0 - 1.0) * spread);			// baseAngle 방향을 기준으로 -spread ~ spread 사이 랜덤한 각도 생성
		}
		distance = minDist + rand() % (maxDist - minDist + 1);								// anchor 주변 이상체 또는 anchor 원점에서 떨어진 랜덤한 위치 생성
		x = originX + (int)round(cos(angle) * distance);
		x = x - (x % 2);																	// 짝수 보정
		y = originY + (int)round(sin(angle) * (distance / 2.0));

		if (isTooCloseToOrigin(x, y))
			continue;

		if (!isSpawnPositionSafe(x, y))
			continue;

		// 이상체 찐 생성
		anomaly[i].x = x;													// 이상체의 x 좌표 설정
		anomaly[i].y = y;													// 이상체의 y 좌표 설정
		anomaly[i].active = 1;												// 이상체 활성화
		anomaly[i].detected = 0;											// 이상체 감지 0(OFF)으로 초기화
		anomaly[i].cnt = 0;													// 이상체를 감지한 파동 수 0으로 초기화
		anomaly[i].life = getAnomalyValue();								// 완전 스캔되는 데 필요한 스캔 수 = anomalyValue
		anomaly[i].maxLife = getAnomalyValue();								// 최대 스캔 수 저장
		anomaly[i].type = OBJ_ANOMALY;										// 타입 저장 (특별 이상체와 구분용)
		fieldValue += getAnomalyValue();									// 맵 내 이상체 총 life 수 증가

		break;
	}
}


// 원점 근처 이상체를 생성
void createAnomalyNearOrigin(ANOMALY* anomaly) {
	int i, j;
	int x, y;
	int angle;
	int distance;
	double rad;

	// inactive된 배열의 인덱스를 찾는다.
	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anomaly[i].active)
			break;
	}

	// 꽉찬 경우 메인 파동 범위에 없는 이상체 하나 강제 교체
	if (i == MAX_ANOMALIES) {
		i = 0;
		// 메인 파동에 없는 이상체의 인덱스를 찾음
		while (i < MAX_ANOMALIES && isInsideMainReach(anomaly[i].x, anomaly[i].y)) {
			i++;
		}
		if (i == MAX_ANOMALIES) return;

		fieldValue -= anomaly[i].life;									// 해당 이상체 남은 life만큼 뺌
		deleteAnomaly(&anomaly[i]);										// 해당 이상체 삭제 후 재생성
	}

	// 새 이상체 위치가 기존 이상체 위치에 있으면 재생성
	for (j = 0; j < 30; j++) {
		// 좌표 생성
		angle = rand() % 360;																					// 랜덤한 각도 생성
		rad = angle * PI / 180.0;
		distance = MIN_ORIGIN_ANOMALY_R + rand() % (MAX_ORIGIN_ANOMALY_R - MIN_ORIGIN_ANOMALY_R + 1);			// MIN_ORIGIN_ANOMALY_R ~ MAX_ORIGIN_ANOMALY_R
		x = (int)round(cos(rad) * distance);
		x = x - (x % 2);																	// 짝수 보정
		y = (int)round(sin(rad) * (distance / 2.0));

		if (isAlreadyExists(anomaly, x, y))
			continue;

		// 이상체 찐 생성
		anomaly[i].x = x;													// 이상체의 x 좌표 설정
		anomaly[i].y = y;													// 이상체의 y 좌표 설정
		anomaly[i].active = 1;												// 이상체 활성화
		anomaly[i].detected = 0;											// 이상체 감지 0(OFF)으로 초기화
		anomaly[i].cnt = 0;													// 이상체를 감지한 파동 수 0으로 초기화
		anomaly[i].life = getAnomalyValue();								// 완전 스캔되는 데 필요한 스캔 수 = anomalyValue
		anomaly[i].maxLife = getAnomalyValue();								// 최대 스캔 수 저장
		anomaly[i].type = OBJ_ANOMALY;										// 타입 저장 (특별 이상체와 구분용)
		fieldValue += getAnomalyValue();									// 맵 내 이상체 총 life 수 증가

		break;
	}
}


// 이상체 생성기
void updateAnomalySpawner() {
	clock_t now = clock() * 1000 / CLOCKS_PER_SEC;

	if (now < nextAnomalySpawnTime) return;

	desiredValue = getDesiredValue();

	if (countMainReachAnomalies(anomaly) < 2) {			// 원점 근처 이상체가 2개 미만일 시 무조건 이상체 생성
		createAnomalyNearOrigin(anomaly);

		if (countMainReachAnomalies(anomaly) < 2) {
			createAnomalyNearOrigin(anomaly);
		}
		return;
	}

	if (fieldValue < desiredValue) {			// 맵 내 이상체들의 anomalyValue가 desiredValue보다 작을시 
		createAnomaly(anomaly);		// 이상체 생성
		nextAnomalySpawnTime = now + 600;	// 0.6초 간격으로 업데이트하도록 함 
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int scannedSpecialAnomalies = 0;		// 스캔 완료된 특별 이상체 카운트

// 특별 이상체 배치
void placeSpecialAnomalies() {
	int i;
	int xy[SPECIAL_ANOMALY_COUNT][2] = {
		{   48,  -12 },
		{  -68,   16 },
		{   90,   22 },
		{ -112,  -26 },
		{  136,  -32 },
		{ -162,   38 },
		{  190,   44 },
		{ -220,  -52 },
		{  252,  -60 },
		{ -286,   68 }
	};

	for (i = 0; i < SPECIAL_ANOMALY_COUNT; i++) {
		anomaly[i].x = xy[i][0];
		anomaly[i].y = xy[i][1];
		anomaly[i].active = 1;
		anomaly[i].detected = 0;
		anomaly[i].cnt = 0;
		anomaly[i].life = 250;
		anomaly[i].maxLife = 250;
		anomaly[i].type = OBJ_SPECIAL_ANOMALY;
	}
}


// 초기 이상체 3개 생성
void spawnAnomaliesTrio() {
	int i;
	int xy[3][2] = {
		{ -4, -3 },	// 1 좌표
		{ 4, 4 },	// 2 좌표
		{-6, 6 }	// 3 좌표
	};

	for (i = 10; i < 13; i++) {
		anomaly[i].x = xy[i - 10][0];
		anomaly[i].y = xy[i - 10][1];
		anomaly[i].active = 1;
		anomaly[i].detected = 0;
		anomaly[i].cnt = 0;
		anomaly[i].life = stats.anomalyValue;
		anomaly[i].maxLife = stats.anomalyValue;
		anomaly[i].type = OBJ_ANOMALY;

		fieldValue += stats.anomalyValue;
	}
}


// 이상체 출력
void printAnomalies(ANOMALY* anom) {
	int x, y;
	int i;
	clock_t now = clock() * 1000 / CLOCKS_PER_SEC;
	int glowTime = stats.anomalyGlow * 1000;
	double ratio;

	for (i = 0; i < MAX_ANOMALIES; i++) {
		if (!anom[i].active) continue;
		if (!anom[i].detected) continue;		// 활성화 혹은 감지 안 되었을 시 출력 X

		if (now - anom[i].start_t > glowTime) {					// Anomaly glow 시간 종료되었을 시 감지 해제
			anom[i].detected = 0;
			continue;
		}

		ratio = (double)anom[i].life / (double)anom[i].maxLife;

		if (anom[i].type == OBJ_SPECIAL_ANOMALY) {	// 특별 이상체일 경우 마젠타색
			textcolor(COLOR_MAGENTA, COLOR_BLACK);
		}
		else if (ratio >= (3.0 / 4.0))			// (남은 스캔 수 / 최대 스캔 수) >= 3/4 일 시 초록색
			textcolor(COLOR_ANOM_GREEN, COLOR_BLACK);
		else if (ratio >= (1.0 / 4.0))			// (남은 스캔 수 / 최대 스캔 수) >= 1/4 일 시 노란색
			textcolor(COLOR_YELLOW, COLOR_BLACK);
		else if (ratio >= 0)					// (남은 스캔 수 / 최대 스캔 수) >= 0 일 시 빨간색
			textcolor(COLOR_RED, COLOR_BLACK);

		x = anom[i].x - cameraX + visionCircleCenterX;
		y = anom[i].y - cameraY + visionCircleCenterY;

		if (isInsideVision(x, y) && isInsideVision(x + 1, y)) {
			printxy(x + VISION_CIRCLE_SCR_CENTER_X - VISION_CIRCLE_RADIUS, y + VISION_CIRCLE_SCR_CENTER_Y - (VISION_CIRCLE_RADIUS / 2), "●");
		}
	}
	textcolor(COLOR_LIGHTGRAY, COLOR_BLACK);
}


// 이상체 제거
void deleteAnomaly(ANOMALY* anom) {
	anom->active = 0;
	anom->detected = 0;
	anom->cnt = 0;
	anom->life = 0;
	anom->maxLife = 0;

	for (int i = 0; i < MAX_CIRCLE_WAVES + MAX_ANTENNA_WAVES; i++) {
		anom->detectedBy[i] = 0;
	}
}