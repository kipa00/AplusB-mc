# 진짜 최종 구데기컵 2 - A+B (MC)

## 컴파일러 및 인터프리터 설치

이 프로그램을 `make`하면 `target` 풀더에 실행 파일 두 개가 생깁니다. `make`할 때 zlib 라이브러리가 필요할 수 있습니다.

만일 이외의 이유로 컴파일 오류가 나는 경우 즉시 연락 주세요.

각 서브태스크의 컴파일과 실행은 다음 명령으로 합니다:

* 컴파일: `./mc-compile (file name)`
* 실행:
  * 서브태스크 1: `./mc-run a.out`
  * 서브태스크 2: `./mc-run a.out --ban-block comparator`
  * 서브태스크 3: `./mc-run a.out --ban-block torch`
  * 서브태스크 4: `./mc-run a.out --max-tick 15`
