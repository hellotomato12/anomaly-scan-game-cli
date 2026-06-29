#pragma once
#include <time.h>
#include "constants.h"

// ---------------------------------
// 구조체
// ---------------------------------

typedef struct player {
	clock_t old_wave_t;	// 이전 파동을 보낸 시간
	clock_t new_wave_t;	// 새 파동을 보낸 시간
	clock_t old_move_t;	// 이전 카메라를 움직인 시간
	clock_t new_move_t;	// 새 카메라를 움직인 시간
} PLAYER;

typedef struct wave {	// 원형 파동 전용
	int x;				// 원의 원점 좌표 x
	int y;				// 원의 원점 좌표 y
	int r;				// 원의 반지름 r
	int prev_r;			// 파동의 이전 프레임의 반지름
	int max_r;			// 파동의 최대 반지름 (파동의 최대 크기를 구함)
	int active;			// 파동 활성화 여부 (0 or 1)
	clock_t start_t;	// 파동이 시작된 시간
	int code;			// 파동 고유 넘버
} WAVE;

typedef struct antenna_wave {
	int x;
	int y;
	int r;
	int prev_r;
	int max_r;
	int active;
	int angle;			// 파동이 퍼지는 방향
	int spreadAngle;	// 파동의 각도 폭
	clock_t start_t;
	int code;
	int chain;			// chain된 횟수
	int myAntennaIndex;	// 자기 자신 재트리거 방지용
} ANTENNA_WAVE;

typedef struct anomaly {
	int x;				// 이상체의 좌표 x
	int y;				// 이상체의 좌표 y
	int active;			// 이상체 활성화 여부
	int detected;		// 이상체 감지 여부
	clock_t start_t;	// 이상체가 감지된 시간
	int detectedBy[MAX_CIRCLE_WAVES + MAX_ANTENNA_WAVES];	// 자신을 감지한 파동을 저장
	int cnt;			// 자신을 감지한 파동의 수
	int life;			// 남은 스캔 수
	int maxLife;		// 최대 스캔 수
	int type;			// OBJ_ANOMALY, OBJ_SPECIAL_ANOMALY
} ANOMALY;

typedef struct antenna {
	int x;				// 안테나의 위치 x
	int y;				// 안테나의 위치 y
	int type;			// 안테나 종류 (0 = narrow, 1 = medium, 2 = wide)
	int angle;			// 안테나가 바라보는 방향
	clock_t start_t;	// 안테나가 생성된 시간
	int triggeredBy[MAX_CIRCLE_WAVES + MAX_ANTENNA_WAVES];	// 자신과 닿은 파동을 저장
	int cnt;			// 자신과 닿은 파동의 수
} ANTENNA;

typedef struct anchor {
	int x;				// anchor의 좌표 x
	int y;				// anchor의 좌표 y
	int type;			// 0 = 메인 파동, 1 = 안테나, 2 = 안테나 파동
	double weight;		// anchor가 선택될 확률 가중치
} ANCHOR;

typedef struct {							// 원형 큐를 구현하여 안테나를 생성
	ANTENNA antenna[MAX_ANTENNAS];
	int front;
	int rear;
} ANTENNA_QUEUE;

typedef struct {							// 원형 큐를 구현하여 안테나 파동을 생성
	ANTENNA_WAVE wave[MAX_ANTENNA_WAVES];
	int front;
	int rear;
} ANTENNA_WAVE_QUEUE;

typedef struct game_stats {
	double autoPingDelay;	// default: OFF
	int anomalyValue;		// default: 12
	int chainDepth;			// default: 3
	double waveSpeed;		// default: 1.00
	int coinsPerScan;		// default: 1
	int anomalyEcho;		// default: 0
	double anomalyGlow;		// default: 1.25
	double waveReach;		// default: 1.00
	int antennaLifetime;	// default: 60s (1분)
} GAME_STATS;

typedef struct stat_info {
	char name[20];				// 스탯 이름
	char description[150];		// 스탯 설명
	int level;					// 현재 레벨
	double statList[MAX_LEVEL + 1];	// 레벨별 수치
	int price[MAX_LEVEL];		// 레벨별 가격
} STAT_INFO;

// 로그 구조체
typedef struct logData {
	char str[100];				// 문자열
	int type;					// 로그 타입
} LOGDATA;

// 로그 큐
typedef struct log_queue {
	LOGDATA data[LOG_LINES];
	int front;
	int rear;
} LOG;

extern WAVE wave[MAX_CIRCLE_WAVES];
extern ANOMALY anomaly[MAX_ANOMALIES];
extern ANCHOR anchor[MAX_ANCHORS];
extern ANTENNA_QUEUE atn_q;
extern ANTENNA_WAVE_QUEUE atn_w_q;
extern GAME_STATS stats;
extern LOG gameLog;