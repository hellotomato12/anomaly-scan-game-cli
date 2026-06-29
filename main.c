#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <mmsystem.h>
#include <process.h>
#pragma comment(lib, "winmm.lib")
#include "constants.h"
#include "doubleBuffering.h"
#include "log.h"
#include "layout.h"
#include "visionCircle.h"
#include "structs.h"
#include "anomaly.h"
#include "antenna.h"
#include "wave.h"
#include "stat.h"
#include "ui.h"

// ---------------------------------
// 메인 함수
// ---------------------------------

int main(int argc, char* argv[]) {

	// 배경 음악 재생
	if (argc == 1) {
		// bgm 재생
		PlaySound(TEXT("bgm.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP);
		spawnl(P_WAIT, argv[0], argv[0], "gamerun", NULL);
		PlaySound(NULL, NULL, 0);
		exit(0);
	}

	system("COLOR 07");						// 화면 배경=검정, 텍스트=연한회색
	system("mode con: cols=198 lines=54");	// 화면 크기 설정 (약 16:9 비율)
	scr_init();								// 스크린 버퍼 초기화
	//printLayouts();

	// 기타
	int i, j;
	unsigned char ch = 0;					// 키보드 입력값 받는 변수
	int gameStart = 0;						// 게임 시작했는지 확인
	srand((unsigned)time(NULL));			// 난수 초기화

	// 플레이어 관련
	PLAYER p = { 0, };

	// 파동 관련
	clock_t autoPing_new_t = 0, autoPing_old_t = 0;

	// 이상체 관련
	// 특별 이상체 배치
	placeSpecialAnomalies();

	// 처음 생성되는 이상체 수: 3
	spawnAnomaliesTrio();

	// 처음 로그 출력
	enqueueLog("/BYPASSING FIREWALL... STATUS: 64", COMMON_LOG);
	enqueueLog("/DECRYPTING RSA-4096 KEY: ATTEMPT 14,902...", COMMON_LOG);
	enqueueLog("/SECURITY PROTOCOL BREACH DETECTED -", COMMON_LOG);
	enqueueLog("RE-ROUTING...", COMMON_LOG);
	enqueueLog("/ACCESS DENIED. INITIATING OVERRIDE SEQUENCE...", COMMON_LOG);
	enqueueLog("/REMOTE ROOT ACCESS GRANTED. WELCOME, ADMIN.", COMMON_LOG);
	enqueueLog(".", COMMON_LOG);
	enqueueLog(".", COMMON_LOG);
	enqueueLog(".", COMMON_LOG);
	enqueueLog("LOADING KERNEL MODULES... [OK]", COMMON_LOG);
	enqueueLog("MOUNTING /DEV/SDA1 ON /MNT/HIDDEN_STORAGE...", COMMON_LOG);
	enqueueLog("INITIALIZING NEURAL NETWORK LAYER 7...", COMMON_LOG);
	enqueueLog("TRACEBACK (MOST RECENT CALL LAST):", COMMON_LOG);
	enqueueLog("MEMORY_LEAK_ERROR", COMMON_LOG);
	enqueueLog("PACKET SNIFFING ACTIVE: 192.168.1.1 ->", COMMON_LOG);
	enqueueLog("[ENCRYPTED]", COMMON_LOG);
	enqueueLog("", COMMON_LOG);


	// 초기 화면
	clock_t loading_old_t = 0;
	clock_t loading_new_t = 0;
	int isFullLoaded = 0;

	while (!isFullLoaded) {

		if (kbhit()) {
			ch = getch();
			if (ch == SPACE) {
				gameStart = 1;
				loading_old_t = clock() * 1000 / CLOCKS_PER_SEC;
			}
		}

		scr_clear();

		if (gameStart) {
			loading_new_t = clock() * 1000 / CLOCKS_PER_SEC;

			if (loading_new_t - loading_old_t >= 500)
				drawBoxToBuffer(52, 3, 150, 45);								// 인게임 구역
			if (loading_new_t - loading_old_t >= 550)
				drawBoxToBuffer(1, 1, 40, 3);									// signal.exe 있는 구역
			if (loading_new_t - loading_old_t >= 600)
				drawBoxToBuffer(1, 3, 52, 25);									// 로그 구역
			if (loading_new_t - loading_old_t >= 650)
				drawBoxToBuffer(170, 1, SCREEN_WIDTH - 1, 3);					// 닫기 버튼 구역
			if (loading_new_t - loading_old_t >= 700)
				drawBoxToBuffer(150, 3, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);	// 스탯 구역
			if (loading_new_t - loading_old_t >= 750)
				drawBoxToBuffer(1, 25, 52, SCREEN_HEIGHT - 1);					// 수집 내용 표시 구역
			if (loading_new_t - loading_old_t >= 800)
				drawBoxToBuffer(40, 1, 170, 3);									// 그 오른쪽 빈 공간 구역
			if (loading_new_t - loading_old_t >= 850) {
				drawBoxToBuffer(52, 45, 150, SCREEN_HEIGHT - 1);				// 설명 구역
				isFullLoaded = 1;
			}
			drawBox();							// 레이아웃 출력
		}

		clearGameBuffer();					// 시야 원 초기화 (visionBuf 초기화)
		drawVisionCircleBuffer();			// 시야 원 그리기 및 최초 실행 시 spaceBuf 채움
		drawPlayerBuffer();					// 플레이어 그리기
		printVisionBuffer();				// 최종 출력
		drawBackgroundBuffer();				// 배경 출력
		printxy(3, 2, "[ PRESS SPACEBAR... ]");

		if (gameStart) {
			printxy(3, 3, "KEY INPUT DETECTED - BEGIN LOADING...");

			if (loading_new_t - loading_old_t >= 500)
				printxy(3, 4, "\"CENTER_BOX\" HAS BEEN LOADED SUCCESSFULLY.");
			if (loading_new_t - loading_old_t >= 550) {
				printxy(3, 5, "\"TOP_WINDOW\" HAS BEEN LOADED SUCCESSFULLY.");
			}
			if (loading_new_t - loading_old_t >= 600) {
				printxy(3, 6, "\"LOG\" HAS BEEN LOADED SUCCESSFULLY.");
			}
			if (loading_new_t - loading_old_t >= 650) {
				printxy(3, 7, "LOADING FAILED...");
			}
			if (loading_new_t - loading_old_t >= 700) {
				printStats();						// 스탯 출력
				printCoordAndRP();					// 현재 좌표 + reset position 출력
				printxy(3, 8, "\"???\" HAS BEEN LOADED SUCCESSFULLY.");
			}
			if (loading_new_t - loading_old_t >= 750)
				printxy(3, 9, "\"SPECIAL_ANOMALY_WINDOW\" HAS BEEN LOADED SUCCESSFULLY.");
			if (loading_new_t - loading_old_t >= 800) {
				printDescription();
				printxy(3, 10, "\"DESCRIPTION\" HAS BEEN LOADED SUCCESSFULLY.");
			}
			if (loading_new_t - loading_old_t >= 850) {
				printAnomaliesCount();				// 현재 시야 내 이상체 수 출력
				printxy(3, 11, "\"VIEW\" HAS BEEN LOADED SUCCESSFULLY.");
			}
		}

		scr_switch();
		Sleep(10);
	}


	// 루프 시작
	while (1) 
	{
		// 입력 처리
		if (kbhit()) {
			ch = getch();
			switch (ch) {
			case SPECIAL1:
			case SPECIAL2:
				if (antennaEditMode) {									// 안테나 편집 모드일 시 선택된 안테나를 움직임
					p.new_move_t = clock() * 1000 / CLOCKS_PER_SEC;
					if (p.new_move_t - p.old_move_t >= 30) {
						moveSelectedAntenna();
						p.old_move_t = p.new_move_t;
					}
				}
				else if (statMode) {									// 스탯 모드일 시 스탯을 선택할 수 있게 함
					moveSelectedStat();
				}
				else {													// 안테나 편집 모드가 아닐 시 카메라를 움직임
					p.new_move_t = clock() * 1000 / CLOCKS_PER_SEC;
					if (p.new_move_t - p.old_move_t >= 30) {			// 30ms마다 카메라를 움직일 수 있다.
						moveCamera();
						p.old_move_t = p.new_move_t;
					}
				}
				break;
			case SPACE:
				p.new_wave_t = clock() * 1000 / CLOCKS_PER_SEC;
				if (p.new_wave_t - p.old_wave_t >= 2000) {			// 2초마다 플레이어가 파동을 보낸다.
					sendMainWave(&wave[waveCount++], 0, 0);
					if (waveCount == MAX_CIRCLE_WAVES)
						waveCount = 0;
					p.old_wave_t = p.new_wave_t;
				}
				break;
			case KEY_R:
				if (antennaEditMode) {
					selectedAntenna->x = selectedAntenna->y = 0;
				}
				cameraX = cameraY = 0;								// reset position
				break;
			case KEY_Q:												// narrow antenna 설치
			case KEY_W:												// medium antenna 설치
			case KEY_E:												// wide antenna 설치
				if (!antennaEditMode && !statMode && openAntenna) {
					createAntenna(ch);
				}
				break;
			case KEY_D:
			case KEY_A:
				if (antennaEditMode) {
					rotateSelectedAntenna(ch);
				}
				break;
			case KEY_P:
				if (!antennaEditMode) {
					statMode = 1;
				}
				break;
			case KEY_ESC:
				statMode = 0;
				break;
			case TAB:
				changeSelectedAntenna();
				break;
			case ENTER:
				if (antennaEditMode) {
					selectedAntennaType = -1;
					antennaEditMode = 0;
					selectedAntenna = NULL;
				}
				else if (statMode) {
					updateStat(selectedStat);
				}
				break;
			}
		}

		scr_clear();

		drawBox();							// 레이아웃 출력

		clearGameBuffer();					// 시야 원 초기화 (visionBuf 초기화)
		drawVisionCircleBuffer();			// 시야 원 그리기 및 최초 실행 시 spaceBuf 채움


		// auto ping 활성화 시 일정 시간마다 파동 발사
		if (stats.autoPingDelay > 0) {
			autoPing_new_t = clock() * 1000 / CLOCKS_PER_SEC;
			if ((autoPing_new_t - autoPing_old_t) >= (int)(stats.autoPingDelay * 1000)) {			// autoPingDelay마다 플레이어가 파동을 보낸다.
				sendMainWave(&wave[waveCount++], 0, 0);
				if (waveCount == MAX_CIRCLE_WAVES)
					waveCount = 0;
				autoPing_old_t = autoPing_new_t;
			}
		}


		for (i = 0; i < MAX_CIRCLE_WAVES; i++) {
			updateWave(&wave[i]);			// 파동의 상태를 확인 후 업데이트

			if (wave[i].active) {
				drawWaveBuffer(&wave[i]);
			}
		}

		// 안테나 파동 업데이트 및 그리기
		i = atn_w_q.front;

		while(i != atn_w_q.rear) {
			i = (i + 1) % MAX_ANTENNA_WAVES;

			updateAntennaWave(&(atn_w_q.wave[i]));		// 파동의 상태를 확인 후 업데이트

			if (atn_w_q.wave[i].active) {
				drawAntennaWaveBuffer(&(atn_w_q.wave[i]));
			}
		}

		drawPlayerBuffer();					// 플레이어 그리기
		drawAntennaBuffer();				// 안테나 그리기
		printVisionBuffer();				// 최종 출력
		drawBackgroundBuffer();				// 배경 출력
		printAnomaliesCount();				// 현재 시야 내 이상체 수 출력
		printDescription();					// 설명 출력
		updateValueProductionBucket();		// 초당 스캔한 수를 저장함
		
		for (i = 0; i < MAX_CIRCLE_WAVES; i++) {
			if (!wave[i].active) continue;

			// 원형 파동에 닿은 안테나 검사 후 chain함
			j = atn_q.front;
			while (j != atn_q.rear) {
				j = (j + 1) % MAX_ANTENNAS;
				
				tryAmplifyCircleWave(&wave[i], &(atn_q.antenna[j]), j);
			}

			// 원형 파동으로 이상체 스캔
			for (j = 0; j < MAX_ANOMALIES; j++) {
				if (!anomaly[j].active) continue;

				isScanned(&anomaly[j], &wave[i]);
			}

			// max_r 도달한 파동 제거
			if (wave[i].r >= wave[i].max_r) {
				wave[i].active = 0;
			}
		}

		// 안테나 파동 처리
		i = atn_w_q.front;

		while (i != atn_w_q.rear) {
			i = (i + 1) % MAX_ANTENNA_WAVES;

			if (!atn_w_q.wave[i].active) continue;

			// 안테나 파동에 닿은 안테나 검사 후 chain함
			j = atn_q.front;

			while (j != atn_q.rear) {
				j = (j + 1) % MAX_ANTENNAS;

				tryAmplifyAntennaWave(&(atn_w_q.wave[i]), &(atn_q.antenna[j]), j);
			}

			// 안테나 파동으로 이상체 스캔
			for (j = 0; j < MAX_ANOMALIES; j++) {
				if (!anomaly[j].active) continue;

				isScannedByAntenna(&anomaly[j], &(atn_w_q.wave[i]));
			}
		}

		deleteAntenna();						// 시간 다 된 안테나 제거

		printAnomalies(anomaly);				// 이상체 출력

		updateAnomalySpawner();					// 이상체 생성기

		printTopBarText();					// 상단 바 텍스트 출력 (signal.exe, X)
		printCoordAndRP();					// 현재 좌표 + reset position 출력
		printAntennaOptions();				// 안테나 구매 버튼 출력

		printStats();						// 스탯 출력

		// 스탯 박스 출력
		i = firstStatBox;
		if (openStat == 0) lastStatBox = AUTO_PING;
		else if (openStat == 1) lastStatBox = ANOMALY_VALUE;
		else lastStatBox = STATS_COUNT;

		while ((lastStatBox != i - 1) && i < STATS_COUNT) {
			printStatBox(&(statInfo[i]), i, statY + i);
			i++;
		}

		printLog();							// 로그 출력
		showScannedSpecialAnomalies();		// 스캔 완료된 특별 이상체 개수 출력

		scr_switch();
		Sleep(10);
	}

	return 0;
}