/*
  kbdgfx.c -- Demo for drawing realtime graphics to the MNT Reform Keyboard
  Copyright 2022 MNT Research GmbH (https://mntre.com)
  License: MIT
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>

#define ROWS 6
#define COLS 12
#define BUFSZ (6+COLS*3)
#define FBUFSZ COLS*ROWS*3

// our unpacked, wasteful framebuffer (3 bytes per pixel)
uint8_t fb[FBUFSZ];
// the buffer we send over HID
uint8_t buf[BUFSZ];

void draw_sine(float t, uint8_t* dst, uint8_t r, uint8_t g, uint8_t b, float brite) {
  for (int x=0; x<COLS; x++) {
    float fy = 2.5 + sin(t + ((float)x/12.0 * 3.141))*2;
    for (int y=0; y<ROWS; y++) {
      float d = 1.0 / (10.0 * (abs((float)y - fy)));
      if (d > 1) d = 1;

      dst[3*(y*COLS + x)]   |= (uint8_t)(r * d * brite);
      dst[3*(y*COLS + x)+1] |= (uint8_t)(g * d * brite);
      dst[3*(y*COLS + x)+2] |= (uint8_t)(b * d * brite);
    }
  }
}

void draw_box(uint8_t* dst, uint8_t r, uint8_t g, uint8_t b, float brite) {
  for (int x=0; x<COLS; x++) {
    for (int y=0; y<ROWS; y++) {
      dst[3*(y*COLS + x)]   |= (uint8_t)(r * brite);
      dst[3*(y*COLS + x)+1] |= (uint8_t)(g * brite);
      dst[3*(y*COLS + x)+2] |= (uint8_t)(b * brite);
    }
  }
}

int main(int argc, char** argv) {
  // just a counter
  uint32_t t = 0;

  if (argc < 2) {
    printf("Usage: sudo kbdgfx /dev/hidraw0 [bitmap.raw]\n");
    exit(1);
  }

  // loop forever
  while (1) {

    // start with the command
    buf[0] = 'x';
    buf[1] = 'X';
    buf[2] = 'R';
    buf[3] = 'G';
    buf[4] = 'B';

    // clear
    memset(fb, 0, FBUFSZ);

    if (argc == 3) {
      // read bitmap from file
      FILE* bmf = fopen(argv[2],"r");
      if (!bmf) {
        printf("Couldn't open bitmap %s!\n", argv[2]);
        exit(2);
      }

      int res = fread(fb, FBUFSZ, 1, bmf);
      fclose(bmf);

      if (res<1) {
        printf("Couldn't read bitmap or wrong size.\n", argv[2]);
        exit(3);
      }
    } else {
      // graphics demo

      // paint
      //draw_sine((float)t*0.03, fb);
      if (t>500) {
        draw_sine((float)t*0.025+1.0, fb, 0xff, 0x00, 0x00, fmaxf(0.0, fminf(1.0, (700-((float)t/1.6))/200.0)));
        draw_sine((float)t*0.025, fb, 0x00, 0x00, 0xff, fmaxf(0.0, fminf(1.0, (700-((float)t/1.5))/200.0)));
        draw_sine((float)t*0.025-1.0, fb, 0x00, 0xff, 0x00, fmaxf(0.0, fminf(1.0, (700-((float)t/1.2))/200.0)));

        if (t>700) {
          draw_box(fb, 0xff, 0xff, 0xff, fmaxf(0.0, fminf(1.0, (((float)t/1.5)-500.0)/200.0)));
        }
      } else {
        draw_sine((float)t*0.025+1.0, fb, 0xff, 0x00, 0x00, fmaxf(0.0, fminf(1.0, (((float)t)-100)/200.0)));
        draw_sine((float)t*0.025, fb, 0x00, 0x00, 0xff, fmaxf(0.0, fminf(1.0, (((float)t/1.2)-200)/200.0)));
        draw_sine((float)t*0.025-1.0, fb, 0x00, 0xff, 0x00, fmaxf(0.0, fminf(1.0, (((float)t/1.1)-300)/200.0)));
      }
    }

    // send our buffer to the keyboard, one line at a time

    for (int row = 0; row < 6; row++) {
      FILE* f = fopen(argv[1],"w");
      if (!f) {
        printf("Couldn't open %s. Try sudo.\n", argv[1]);
        exit(1);
      }
      buf[5] = row;
      memcpy(buf+6, fb+row*(COLS*3), COLS*3);
      fwrite(buf, BUFSZ, 1, f);
      fclose(f);
    }

    // if we're in bitmap file mode, exit now
    if (argc == 3) exit(0);

    // ~50 FPS
    usleep(100*20);
    // ~2 FPS
    //usleep(1000*500);
    t++;
  }
}
