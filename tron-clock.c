// gcc -Wall -o tron-clock tron-clock.c -lncursesw
// `-lncurses` for simple characters; `-lncursesw` for unicode characters

#include <unistd.h> // for sleep and usleep
#include <string.h>
#include <locale.h> // for setLocale
#include <ncurses.h>
#include <time.h> // for time, localtime, strftime, clock_gettime
#include <stdlib.h> // for srand and rand
#include <wchar.h> // for wchar_t

#define UNIX_TIME_SIZE_BINARY 31 // change to 32 after 03:14:07 UTC on 19 January 2038!
#define UNIX_TIME_SIZE_DECIMAL 10 // change to 11 after 17:46:39 UTC on 20 November 2286!
#define USE_OFFSETS false
#define MILLISECONDS false
#define PAD 32
#define ONE 0x25cf
#define ZERO 0x25cb

void to7bit(int value, char *result) {
	for (int i = 0; i < 7; i++) {
		result[i] = (value >> i) & 1;
	}
}

void unixTimeToBinary(time_t value, char *result) {
	for (int i = 0; i < UNIX_TIME_SIZE_BINARY; i++) {
		result[i] = (value >> i) & 1;
	}
}

int main(int argc, char *argv[]) {
	usleep(500000); // time for the terminal's window to be maximised

	int display = 0;

	if (argc >= 2) {
		display = atoi(argv[1]) % 100;
	}

	setlocale(LC_ALL, ""); // for displaying unicode characters

	initscr();
	curs_set(0);

	int row, col;
	getmaxyx(stdscr, row, col);

	int offsetX = 0;
	int offsetY = 0;

	#if USE_OFFSETS
		srand(time(NULL));
		offsetX = rand() % 11 - 5;
		offsetY = rand() % 11 - 5;
	#endif

	time_t timer;
	struct tm localTime;
	char formatedTime[20];

	#if MILLISECONDS
		struct timespec clockTime;
		char milliseconds[10];
	#endif

	char timeMatrix[6][7];
	bool pad[6];

	char unixTimeStr[UNIX_TIME_SIZE_DECIMAL];

	char unixTime[UNIX_TIME_SIZE_BINARY];

	while (1) {
		timer = time(NULL);

		localTime = *localtime(&timer);

		if (display >= 35) {
			strftime(formatedTime, 20, "%y:%m:%d:%H:%M:%S", &localTime);

			#if MILLISECONDS
				clock_gettime(CLOCK_MONOTONIC_RAW, &clockTime);
				sprintf(milliseconds, "%.9ld", clockTime.tv_nsec);
				move(row / 2 + offsetY, (col - 20) / 2 + offsetX);
				printw("%s:%c%c", formatedTime, milliseconds[0], milliseconds[1]);
			#else
				move(row / 2 + offsetY, (col - 17) / 2 + offsetX);
				printw("%s", formatedTime);
			#endif
		} else if (display >= 15) {
			to7bit(localTime.tm_year - 100, timeMatrix[0]);
			to7bit(localTime.tm_mon + 1, timeMatrix[1]);
			to7bit(localTime.tm_mday, timeMatrix[2]);
			to7bit(localTime.tm_hour, timeMatrix[3]);
			to7bit(localTime.tm_min, timeMatrix[4]);
			to7bit(localTime.tm_sec, timeMatrix[5]);

			pad[0] = true;
			pad[1] = true;
			pad[2] = true;
			pad[3] = true;
			pad[4] = true;
			pad[5] = true;

			for (int i = 6; i >= 0; i--) {
				move(row / 2 - 3 + i + offsetY, (col - 18) / 2 + offsetX);

				for (int j = 0; j < 6; j++) {
					if (timeMatrix[j][i] == 0) {
						if (pad[j] && i > 0) {
							printw(" %lc ", PAD);
						} else {
							printw(" %lc ", ZERO);
						}
					} else if (timeMatrix[j][i] == 1) {
						pad[j] = false;
						printw(" %lc ", ONE);
					}
				}
			}
		} else if (display >= 5) {
			move(row / 2 + offsetY, (col - UNIX_TIME_SIZE_DECIMAL * 2 - 1) / 2 + offsetX);

			sprintf(unixTimeStr, "%ld", timer);

			for (int i = 0; i < sizeof(unixTimeStr); i++) {
				printw("%c ", unixTimeStr[i]);
			}
		} else {
			unixTimeToBinary(timer, unixTime);

			move(row / 2 + offsetY, (col - UNIX_TIME_SIZE_BINARY * 2 - 1) / 2 + offsetX);

			for (int i = sizeof(unixTime) - 1; i >= 0; i--) {
				if (unixTime[i] == 0) {
					printw("%lc ", ZERO);
				} else {
					printw("%lc ", ONE);
				}
			}
		}

		refresh();

		#if MILLISECONDS
			usleep(10000);
		#else
			sleep(1);
		#endif
	}

	endwin();

	return 0;
}
