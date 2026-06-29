# anomaly-scan-game-cli (Taos Research Project)

Yogurt The Horse와 kiberptah가 제작한 인디 게임인 'Taos Research Project'를 C언어 콘솔 환경으로 재구현한 프로젝트입니다.

---

## 개요
- **개발 기간:** 2026년 5월 12일 - 6월 8일 (대학 텀 프로젝트)
- **개발 언어:** C
- **개발 환경:** Visual Studio
- **원작 출처:** [Taos Research Project](https://yogurtthehorse.itch.io/signal)

---

## 실행 화면
<img width="600" alt="image" src="https://github.com/user-attachments/assets/d95d8373-d74c-438e-896d-c178d153bf99" />

---

## 구현
- **콘솔 최적화:** 콘솔 특유의 화면 깜빡임을 줄이기 위해 더블 버퍼링 사용
- **원 및 호 구현:** 원의 방정식과 삼각 함수를 이용하여 원점으로부터 반지름 거리만큼 떨어진 위치에 텍스트를 출력하는 방식으로 원과 호 모양을 구현함
- **데이터 구조:** 로그나 안테나, 파동 등 계속 저장되고 사라져야 하는 요소를 큐로 구현함

---

## 실행 방법
1. 저장소 클론: `git clone [https://github.com/hellotomato12/anomaly-scan-game-cli.git](https://github.com/hellotomato12/anomaly-scan-game-cli.git)`
2. `main.c` 컴파일 후 실행

---

## 이슈
- 안테나가 시간이 지나 사라져도 TAB 키로 안테나 선택 시 이미 없어진 안테나의 위치로 이동하는 현상 발생
- 안테나 선택 시 존재하는 안테나일 경우 선택하고, 없을 시 다음 안테나를 확인하는 방식으로 수정할 예정
