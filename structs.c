#include "constants.h"
#include "structs.h"

WAVE wave[MAX_CIRCLE_WAVES] = { 0, };			// 원형 파동 구조체
ANOMALY anomaly[MAX_ANOMALIES] = { 0, };		// 일반 이상체 구조체 배열
ANCHOR anchor[MAX_ANCHORS];
ANTENNA_QUEUE atn_q = { 0, };				// 안테나 원형 큐 구조체
ANTENNA_WAVE_QUEUE atn_w_q = { 0, };		// 안테나 파동 큐 구조체
GAME_STATS stats = { -2.0, 12, 3, 1.0, 1, 0, 1.25, 1.0, 60 };
LOG gameLog = { 0, };