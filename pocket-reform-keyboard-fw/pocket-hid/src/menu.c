/*
  SPDX-License-Identifier: GPL-3.0-or-later
  MNT Pocket Reform Keyboard/Trackball Controller Firmware for RP2040
  Copyright 2021-2024 MNT Research GmbH (mntre.com)
*/

#include "menu.h"
#include "oled.h"
#include "remote.h"
#include "usb_hid_keys.h"
#include "keyboard.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/watchdog.h"

int current_menu_y = 0;
int current_scroll_y = 0;
int current_menu_page = 0;
int8_t logo_timeout_ticks = 0;

#ifdef KBD_MODE_STANDALONE
// TODO
#else
#define MENU_NUM_ITEMS 7
const MenuItem menu_items[] = {
  { "Exit Menu         ESC", KEY_ESC },
  { "Power On            1", KEY_1 },
  { "Power Off           0", KEY_0 },
  { "Battery Status      b", KEY_B },
  { "Wake              SPC", KEY_SPACE },
  { "System Status       s", KEY_S },
  { "Reset Keyboard      r", KEY_R },
};
#endif

void rp2040_reset() {
  watchdog_enable(1, 1);
  while(1);
}

void rp2040_reset_to_bootloader() {
  reset_usb_boot(0, 0);
  while(1);
}

void reset_menu() {
  current_scroll_y = 0;
  current_menu_y = 0;
  current_menu_page = MENU_PAGE_NONE;
  gfx_clear();
  gfx_flush();
}

void reset_and_render_menu() {
  reset_menu();
  render_menu(current_scroll_y);
}

void render_menu(int y) {
  gfx_clear();
  gfx_invert_row((uint8_t)(current_menu_y-y));
  for (int i=0; i<MENU_NUM_ITEMS; i++) {
    gfx_poke_cstr(0,(uint8_t)(i-y),menu_items[i].title);
  }
  gfx_on();
  gfx_contrast(0xff);
  gfx_flush();
}

// automatically refresh the current menu page if needed
void refresh_menu_page() {
  if (current_menu_page == MENU_PAGE_BATTERY_STATUS) {
    remote_get_voltages();
  } else if (current_menu_page == MENU_PAGE_MNT_LOGO && --logo_timeout_ticks <= 0) {
    reset_menu();
  }
}

int execute_menu_row_function(int y) {
  current_menu_page = MENU_PAGE_NONE;

  if (y>=0 && y<MENU_NUM_ITEMS) {
    current_menu_page = MENU_PAGE_OTHER;
    return execute_menu_function(menu_items[y].keycode);
  }
  return execute_menu_function(KEY_ESC);
}

// returns 1 for navigation function (stay in menu mode), 0 for terminal function
int execute_menu_function(int keycode) {
  if (keycode == KEY_0) {
    // TODO: are you sure?
    led_set_brightness(0);
    anim_goodbye();
    remote_turn_off_som();
    reset_keyboard_state();
    return 0;
  }
  else if (keycode == KEY_1) {
    if (remote_turn_on_som()) {
      // initial backlight color
      led_set(0x004040);
      anim_hello();
    }
    return 0;
  }
  else if (keycode == KEY_R) {
    // reset the MCU
    rp2040_reset();
  }
  else if (keycode == KEY_T) {
    render_tina();
    logo_timeout_ticks = 10;
    current_menu_page = MENU_PAGE_MNT_LOGO;
    return 0;
  }
  else if (keycode == KEY_SPACE) {
    remote_wake_som();
  }
  else if (keycode == KEY_H) {
    // turn-on hint page
    gfx_clear();
    gfx_poke_cstr(0,1,"Press \x8a + \x8b for menu.");
    gfx_poke_cstr(0,2,"Hold to power up.");
    gfx_flush();
    logo_timeout_ticks = 5;
    current_menu_page = MENU_PAGE_MNT_LOGO;
    return 0;
  }
  else if (keycode == KEY_B) {
    current_menu_page = MENU_PAGE_BATTERY_STATUS;
    remote_get_voltages();
    return 0;
  }
  else if (keycode == KEY_S) {
    remote_get_status();
    return 0;
  }
  else if (keycode == KEY_F1) {
    //kbd_brightness_dec();
    return 1;
  }
  else if (keycode == KEY_F2) {
    //kbd_brightness_inc();
    return 1;
  }
  else if (keycode == KEY_UP) {
    current_menu_y--;
    if (current_menu_y<0) current_menu_y = 0;
    if (current_menu_y<=current_scroll_y) current_scroll_y--;
    if (current_scroll_y<0) current_scroll_y = 0;
    render_menu(current_scroll_y);
    return 1;
  }
  else if (keycode == KEY_DOWN) {
    current_menu_y++;
    if (current_menu_y>=MENU_NUM_ITEMS) current_menu_y = MENU_NUM_ITEMS-1;
    if (current_menu_y>=current_scroll_y+3) current_scroll_y++;
    render_menu(current_scroll_y);
    return 1;
  }
  else if (keycode == KEY_ENTER) {
    return execute_menu_row_function(current_menu_y);
  }
  else if (keycode == KEY_ESC) {
    gfx_clear();
    gfx_flush();
  }
  else if (keycode == KEY_X) {
    gfx_clear();
    gfx_poke_cstr(1, 1, "Entered firmware");
    gfx_poke_cstr(1, 2, "update mode.");
    gfx_on();
    gfx_flush();
    rp2040_reset_to_bootloader();
  }
  else if (keycode == KEY_L) {
    anim_hello();
    return 0;
  }

  gfx_clear();
  gfx_flush();

  return 0;
}

void render_tina(void) {
  gfx_clear();
  gfx_on();
  for (uint8_t y=0; y<4; y++) {
    gfx_invert_row(y);
  }
  for (int f=13; f>=0; f--) {
    for (uint8_t y=0; y<4; y++) {
      for (uint8_t x=0; x<6; x++) {
        if (x==6) {
          if (x+8+f<21) {
            gfx_poke((uint8_t)(x+8+f),y,' ');
          }
        } else {
          if (x+8+f<21) {
            gfx_poke((uint8_t)(x+8+f),y,(uint8_t)((4+y)*32+x+12));
          }
        }
      }
    }
    gfx_flush();
  }
}

void anim_hello(void) {
  current_menu_page = MENU_PAGE_MNT_LOGO;
  logo_timeout_ticks = 10;
  gfx_clear();
  gfx_on();
  for (uint8_t y=0; y<3; y++) {
    for (uint8_t x=0; x<12; x++) {
      gfx_poke(x+4,y+1,(uint8_t)((5+y)*32+x));
    }
    gfx_flush();
  }
  for (uint8_t y=0; y<0xff; y++) {
    gfx_contrast(y);
    sleep_ms(0);
  }
  for (uint8_t y=0; y<0xff; y++) {
    gfx_contrast(0xff-y);
    sleep_ms(0);
  }
}

void anim_goodbye(void) {
  gfx_clear();
  gfx_on();
  for (uint8_t y=0; y<3; y++) {
    for (uint8_t x=0; x<12; x++) {
      gfx_poke(x+4,y+1,(uint8_t)((5+y)*32+x));
    }
  }
  for (uint8_t y=0; y<3; y++) {
    for (uint8_t x=0; x<12; x++) {
      gfx_poke(x+4,y+1,' ');
    }
    gfx_flush();
  }
  gfx_off();
}
