/*
  SPDX-License-Identifier: GPL-3.0-or-later
  MNT Pocket Reform Keyboard/Trackball Controller Firmware for RP2040
  Copyright 2021-2024 MNT Research GmbH (mntre.com)
*/

#include <stdlib.h>
#include "remote.h"
#include "oled.h"
#include "keyboard.h"

#include "bsp/board_api.h"
#include "pico/stdlib.h"

#define UART_ID uart1

// received by uart
#define RESPONSE_MAX 128
static char response[RESPONSE_MAX];
static int uart_rx_i = 0;
static int uart_print_response = 0;
static int uart_response_complete = 0;

uint8_t term_x = 0;
uint8_t term_y = 0;
double voltages[8];
int alert_low_battery = 0;
int alert_blink = 0;
uint8_t remote_som_power_expected_state = 0;

int command_sent = 0;
int soc_power_on = 0; // fixme

void insert_bat_icon(char* str, int x, double v) {
  char icon = 0;
  if (v>=3.3) {
    icon = 8;
  } else if (v>=3.1) {
    icon = 6;
  } else if (v>=3.0) {
    icon = 4;
  } else if (v>=2.9) {
    icon = 2;
  } else {
    icon = 0;
  }
  str[x]   = 4*32+icon;
  str[x+1] = 4*32+icon+1;
}

int remote_get_power_state() {
  return soc_power_on;
}

void remote_on_uart_rx() {
  while (uart_is_readable(UART_ID)) {
    char c = uart_getc(UART_ID);
    if (uart_rx_i < RESPONSE_MAX-1) {
      response[uart_rx_i++] = c;
      response[uart_rx_i] = 0;
    } else {
      uart_rx_i = 0;
    }

    uint8_t poke_chr = c;

    if (c == '\n') {
      uart_rx_i = 0;
      poke_chr=' ';
    }

    if (c!='\r') {
      if (uart_print_response) {
        gfx_poke(term_x,term_y,poke_chr);
        gfx_poke(term_x+1,term_y,' ');
        term_x++;
        if (term_x>=20) {
          term_x=0;
          term_y++;
          if (term_y>=3) {
            term_y=0;
          }
        }
      }
    } else {
      uart_response_complete = 1;
    }
  }
}

int remote_try_command(const char* cmd, int print_response) {
  int ok = 1;

  memset(response, 0, RESPONSE_MAX);
  uart_rx_i = 0;
  uart_response_complete = 0;
  uart_print_response = print_response;
  if (print_response) {
    term_x = 0;
    term_y = 0;
  }
  uart_puts(UART_ID, cmd);

  int timeout = 0;
  while (!uart_response_complete) {
    sleep_ms(10);
    timeout++;
    if (timeout > 10) {
      gfx_poke(0,0,'T');
      gfx_flush();
      ok = 0;
      break;
    }
  }

  if (print_response) {
    gfx_flush();
  };

  uart_print_response = 0;

  return ok;
}

int remote_get_status(void) {
  gfx_clear();

  gfx_poke_cstr(0, 2, "MNT Pocket Reform HID");
  gfx_poke_cstr(0, 3, PREF_HID_FW_REV);
  gfx_on();
  gfx_flush();

  int ok = remote_try_command("s\r", 1);
  return ok;
}

int remote_get_voltages(int quiet) {
  term_x = 0;
  term_y = 0;

  double bat_volts = 0;
  double bat_amps = 0;
  char bat_gauge[5] = {0,0,0,0,0};

  int ok = remote_try_command("c\r", 0);
  if (!ok) return ok;

  // lpc format: 32 32 32 32 32 32 32 32 mA 0256mV26143 ???% P1
  //             |  |  |  |  |  |  |  |  | |      |     |    |
  //             0  3  6  9  12 15 18 21 24|      |     |    |
  //                                       26     33    39   44
  //                                       |
  //                                       `- can be a minus
  double sum_volts = 0;

  for (int i=0; i<8; i++) {
    voltages[i] = ((double)((response[i*3]-'0')*10 + (response[i*3+1]-'0')))/10.0;
    if (voltages[i]<0) voltages[i]=0;
    if (voltages[i]>=10) voltages[i]=9.9;
    sum_volts += voltages[i];
  }

  int amps_offset = 3*8+2;
  // cut off string
  response[amps_offset+5]=0;
  bat_amps = ((double)atoi(&response[amps_offset]))/1000.0;
  int volts_offset = amps_offset+5+2;
  response[volts_offset+5]=0;
  bat_volts = ((double)atoi(&response[volts_offset]))/1000.0;
  int gauge_offset = volts_offset+5+1;
  strncpy(bat_gauge, &response[gauge_offset], 4);

  const char* power_str = "   ";
  int syspower_offset = gauge_offset+5;
  char power_digit = response[syspower_offset+1];
  if (power_digit == '1') {
    power_str = " On";
    soc_power_on = 1;
  } else if (power_digit == '0') {
    power_str = "Off";
    soc_power_on = 0;
  }

  if (quiet) return ok;

  // plot
  gfx_clear();
  char str[32];

  sprintf(str,"[] %.1f           %s",voltages[0],bat_gauge);
  insert_bat_icon(str,0,voltages[0]);
  gfx_poke_str(0,0,str);

  sprintf(str,"[] %.1f            %s",voltages[1],power_str);
  insert_bat_icon(str,0,voltages[1]);
  gfx_poke_str(0,1,str);

  if (bat_amps>=0) {
    sprintf(str,"               %2.3fA",bat_amps);
  } else {
    sprintf(str,"               %2.2fA",bat_amps);
  }
  gfx_poke_str(0,2,str);

  sprintf(str,"               %2.2fV",bat_volts);
  gfx_poke_str(0,3,str);
  gfx_flush();

  return ok;
}

int remote_turn_on_som() {
  remote_try_command("1p\r", 0);
  soc_power_on = 1;
  return 1;
}

int remote_turn_off_som() {
  remote_try_command("0p\r", 0);
  soc_power_on = 0;
  return 1;
}

int remote_wake_som() {
  remote_try_command("1w\r", 0);
  return 1;
}
