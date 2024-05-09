#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void cleanup() {
}

void handle_sigint() {
	exit(0);
}

typedef struct pref_key {
	char label[5];
	int code;
	int det;
} pref_key;

typedef struct pref_row {
	int num_keys;
	pref_key keys[12];
} pref_row;

#define NUM_R 5
pref_row ROWS[NUM_R] = {
	{
		12, {
			{ "ESC", 1 },
			{ "1", 2 },
			{ "2", 3 },
			{ "3", 4 },
			{ "4", 5 },
			{ "5", 6 },
			{ "6", 7 },
			{ "7", 8 },
			{ "8", 9 },
			{ "9", 10 },
			{ "0", 11 },
			{ "BSP", 14 },
		}
	},
	{
		12, {
			{ "TAB", 15 },
			{ "Q", 16 },
			{ "W", 17 },
			{ "E", 18 },
			{ "R", 19 },
			{ "T", 20 },
			{ "Y", 21 },
			{ "U", 22 },
			{ "I", 23 },
			{ "O", 24 },
			{ "P", 25 },
			{ ";", 39 },
		}
	},
	{
		12, {
			{ "CTL", 29 },
			{ "A", 30 },
			{ "S", 31 },
			{ "D", 32 },
			{ "F", 33 },
			{ "G", 34 },
			{ "H", 35 },
			{ "J", 36 },
			{ "K", 37 },
			{ "L", 38 },
			{ "'", 40 },
			{ "RET", 28 },
		}
	},
	{
		12, {
			{ "LSH", 42 },
			{ "Z", 44 },
			{ "X", 45 },
			{ "C", 46 },
			{ "V", 47 },
			{ "B", 48 },
			{ "N", 49 },
			{ "M", 50 },
			{ ",", 51 },
			{ ".", 52 },
			{ "UP",  103},
			{ "AGR", 100 },
		}
	},
	{
		12, {
			{ "", 0 },
			{ "MNT", 125 },
			{ "ALT", 56 },
			{ "\\", 43 },
			{ "=", 13 },
			{ "SPC", 57 },
			{ "SPC", 57 },
			{ "-", 12 },
			{ "/", 53 },
			{ "LFT", 105 },
			{ "DWN", 108 },
			{ "RGT", 106 },
		}
	},
};

int main(int argc, char** argv) {
	atexit(cleanup);
	signal(SIGINT, handle_sigint);

	if (argc < 2) exit(1);
	int dev = open(argv[1], O_RDONLY);
	if (!dev) exit(2);

	struct input_event e;

	while (1) {
		puts("\x1B[2J");
		int alldet = 1;
		for (int y=0; y<NUM_R; y++) {
			for (int x=0; x<ROWS[y].num_keys; x++) {
				pref_key* k = &ROWS[y].keys[x];
				if (k->det == 1) {
					printf("    ");
				} else {
					printf("%3s ", k->label);
					alldet = 0;
				}
			}
			printf("\n");
		}
		printf("\n");

		if (alldet) break;

		read(dev, &e, sizeof(e));
		if (e.type == 1) {
			//printf("code: %d value: %d\n", e.code, e.value);
			for (int y=0; y<NUM_R; y++) {
				for (int x=0; x<ROWS[y].num_keys; x++) {
					if (ROWS[y].keys[x].code == e.code) {
						ROWS[y].keys[x].det = 1;
					}
				}
			}
		}
	}

	return 0;
}
