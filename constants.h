#pragma once

// ---------------------------------

// 화면 크기
#define SCREEN_WIDTH 176		// = screen max-width
#define SCREEN_HEIGHT 50		// = screen max-height

// 색상 정의
#define COLOR_BLACK		0
#define COLOR_GREEN		2
#define COLOR_CYAN		3
#define COLOR_LIGHTGRAY	7
#define COLOR_GRAY		8
#define COLOR_BLUE		9
#define COLOR_ANOM_GREEN 10
#define COLOR_RED		12
#define COLOR_MAGENTA	13
#define COLOR_YELLOW	14

// ---------------------------------

// tools
#define CLAMP(val, MIN, MAX) (((val) > (MAX)) ? (MAX) : (((val) < (MIN)) ? (MIN) : (val)))		// 특정 값이 최소값과 최대값을 못 벗어남. val > MAX인 경우 MAX를, val < MIN인 경우 MIN을, 그 사이일 경우 val을 반환
#define PI 3.1415926535





// ---------------------------------
// 시야 원(Vision Circle) 관련 정보
// ---------------------------------

// 시야 원의 원점 좌표
#define VISION_CIRCLE_SCR_CENTER_X 108
#define VISION_CIRCLE_SCR_CENTER_Y 23

// 시야 원의 반지름
#define VISION_CIRCLE_RADIUS 36

// 시야 원을 감싸는 사각형에 대한 버퍼 인덱스 크기
#define VISION_SQR_WIDTH 73						// VISION_CIRCLE_RADIUS * 2 + 1
#define VISION_SQR_HEIGHT 37					// VISION_CIRCLE_RADIUS + 1

// 오브젝트 관련 번호 매크로
#define OBJ_EMPTY 0				// 빈 공간
#define OBJ_WAVE 1				// 파동
#define OBJ_VISION_OUTLINE 2	// 원 테두리 (시야 경계선)
#define OBJ_PLAYER 3			// 플레이어
#define OBJ_ANOMALY 4			// 이상체
#define OBJ_SPECIAL_ANOMALY 5	// 특별 이상체
#define OBJ_NARROW_ANTENNA 6	// narrow antenna
#define OBJ_MEDIUM_ANTENNA 7	// medium antenna
#define OBJ_WIDE_ANTENNA 8		// wide antenna
#define OBJ_IGNORE 99			// 2칸 차지하는 문자의 다음 칸을 메꿔주는용

// ---------------------------------





// ---------------------------------
// 이상체 관련 정보
// ---------------------------------

// 특별 이상체 개수
#define SPECIAL_ANOMALY_COUNT 10	// 총 10개

// 이상체 총 개수
#define MAX_ANOMALIES 100 // 총 100개

// ANCHOR 타입
#define ANCHOR_ORIGIN 0			// 메인 파동 원점 기준 anchor
#define ANCHOR_ANTENNA 1		// 안테나 기준 anchor
#define ANCHOR_ANTENNA_WAVE 2	// 안테나 파동 기준 anchor

// ANCHOR 총 개수
#define MAX_ANCHORS 128

// ANCHOR 주변에 이상체가 생성될 때 최소 최대 거리
#define MIN_SPAWN_DISTANCE 6			// ANCHOR 주변에 이상체가 생성될 때 최소 거리
#define MAX_SPAWN_DISTANCE 28			// ANCHOR 주변에 이상체가 생성될 때 최대 거리

// 클러스터 생성
#define CLUSTER_CHANCE 30				// 30%
#define CLUSTER_SEARCH_RADIUS 30		// anchor 주변 기존 이상체 탐색 범위
#define CLUSTER_MIN_DISTANCE 4			// 클러스터 최소 거리
#define CLUSTER_MAX_DISTANCE 10			// 클러스터 최대 거리

// 이상체끼리 안 겹치게 간격 조정
#define SPAWN_SEPERATION 2

// 이상체가 파동 테두리에 생성되지 않도록 간격 조정
#define WAVE_SEPERATION 2

// 이상체와 원점 사이의 최소, 최대 반지름 길이
#define MIN_ORIGIN_ANOMALY_R 6
#define MAX_ORIGIN_ANOMALY_R 14

// ---------------------------------





// ---------------------------------
// 안테나 관련 정보
// ---------------------------------

// 안테나 총 개수
#define MAX_ANTENNAS 100

// 안테나 타입
#define NARROW_ANTENNA 0
#define MEDIUM_ANTENNA 1
#define WIDE_ANTENNA 2

// 안테나 회전 다이얼 반지름
#define ROTATION_DIAL_R 12

// ---------------------------------





// ---------------------------------
// 기타 인게임 관련 정보
// ---------------------------------

// 파동 최대 횟수
#define MAX_CIRCLE_WAVES 200		// 원형 파동 (메인 파동 혹은 이상체에서 나온 파동)
#define MAX_ANTENNA_WAVES 1000		// 안테나의 파동

// 각 스탯별 베이스 값
#define BASE_WAVE_SPEED 10000	// 10초
#define BASE_WAVE_REACH 26

// ---------------------------------

// 조작키
#define SPECIAL1 0xe0	// 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00	// keypad 경우 0x00 + key 로 구성된다.
#define UP 0x48			// 위 방향키
#define DOWN 0x50		// 아래 방향키
#define LEFT 0x4b		// 왼쪽 방향키
#define RIGHT 0x4d		// 오른쪽 방향키
#define SPACE 0x20		// 메인 안테나 파동 보내기
#define TAB 0x09		// 탭, 설치된 이상체를 선택함. 누를 때마다 다음 이상체로 넘어감
#define ENTER 0x0d		// 엔터

#define KEY_R 0x72		// reset position

#define KEY_Q 0x71		// narrow antenna 선택
#define KEY_W 0x77		// medium antenna 선택
#define KEY_E 0x65		// wide antenna 선택

#define KEY_D 0x64		// 안테나 시계방향 회전
#define KEY_A 0x61		// 안테나 반시계방향 회전

#define KEY_P 0x70		// 스탯 모드 on
#define KEY_ESC 0x1b	// 스탯 모드 off

// ---------------------------------

// 스탯 개수
#define STATS_COUNT 9	// 스탯 총 9개

// 스탯의 최대 레벨
#define MAX_LEVEL 9		// 최대 레벨 9

// 스탯 인덱스
#define AUTO_PING				0
#define ANOMALY_VALUE			1
#define CHAIN_DEPTH				2
#define WAVE_SPEED				3
#define COINS_PER_SCAN			4
#define ANOMALY_ECHO			5
#define ANOMALY_GLOW			6
#define WAVE_REACH				7
#define ANTENNA_LIFE_TIME		8

// 스탯 창
#define STAT_BOX_MIN_X			152
#define STAT_BOX_MIN_Y			10

// ---------------------------------

// 로그 라인 수
#define LOG_LINES 21

#define COMMON_LOG 0		// 평범한 로그
#define SCANNED_LOG 1		// 스캔 로그
#define SPECIAL_LOG 2		// 특별 이상체 스캔 로그
#define FULLY_SCANNED_LOG 3	// 완전 스캔 로그

// ---------------------------------

extern int currentCoin;
extern int openAntenna;
extern int openStat;