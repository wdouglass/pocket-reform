/*
  MNT Reform 2.0 Keyboard Firmware
  See keyboard.c for Copyright
  SPDX-License-Identifier: MIT
*/

//#include "backlight.h"
//#include "keyboard.h"
#include "menu.h"
#include "oled.h"
#include "remote.h"
#include "usb_hid_keys.h"
#include "keyboard.h"
#include "pico/stdlib.h"
//#include "scancodes.h"

int current_menu_y = 0;
int current_scroll_y = 0;
int current_menu_page = 0;
int8_t logo_timeout_ticks = 0;

#ifdef KBD_MODE_STANDALONE
#define MENU_NUM_ITEMS 5
const MenuItem menu_items[] = {
  { "Exit Menu         ESC", KEY_ESC },
  { "Key Backlight-     F1", KEY_F1 },
  { "Key Backlight+     F2", KEY_F2 },
  { "System Status       s", KEY_S },
  { "USB Flashing Mode   x", KEY_X },
};
#else
#define MENU_NUM_ITEMS 7
const MenuItem menu_items[] = {
  { "Exit Menu         ESC", KEY_ESC },
  { "Power On            1", KEY_1 },
  { "Power Off           0", KEY_0 },
  { "Reset               r", KEY_R },
  { "Battery Status      b", KEY_B },
  { "Wake              SPC", KEY_SPACE },
  { "System Status       s", KEY_S },

  // Only needed for debugging.
  // The keyboard will go to sleep when turning off
  // main system power.
  { "KBD Power-Off       p", KEY_P },
};
#endif

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

int execute_menu_function(int y) {
  current_menu_page = MENU_PAGE_NONE;

  if (y>=0 && y<MENU_NUM_ITEMS) {
    current_menu_page = MENU_PAGE_OTHER;
    return execute_meta_function(menu_items[y].keycode);
  }
  return execute_meta_function(KEY_ESC);
}

// returns 1 for navigation function (stay in meta mode), 0 for terminal function
int execute_meta_function(int keycode) {
  if (keycode == KEY_0) {
    // TODO: are you sure?
    anim_goodbye();
    remote_turn_off_som();
    //keyboard_power_off();
    reset_keyboard_state();
    // Directly enter menu again
    return 2;
  }
  else if (keycode == KEY_1) {
    //kbd_brightness_init();
    if (remote_turn_on_som()) {
      anim_hello();
    }
    return 0;
  }
  else if (keycode == KEY_R) {
    // TODO: are you sure?
    //remote_reset_som();
  }
  else if (keycode == KEY_SPACE) {
    remote_wake_som();
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
    return execute_menu_function(current_menu_y);
  }
  else if (keycode == KEY_ESC) {
    gfx_clear();
    gfx_flush();
  }
  /*else if (keycode == KEY_X) {
    gfx_clear();
    gfx_poke_str(1, 1, "Entered firmware");
    gfx_poke_str(1, 2, "update mode.");
    gfx_on();
    gfx_flush();
    jump_to_bootloader();
  }*/
  else if (keycode == KEY_L) {
    anim_hello();
    return 0;
  }

  gfx_clear();
  gfx_flush();

  return 0;
}

void anim_hello(void) {
  current_menu_page = MENU_PAGE_MNT_LOGO;
  logo_timeout_ticks = 10; // ~30sec
  gfx_clear();
  //kbd_brightness_set(0);
  gfx_on();
  for (uint8_t y=0; y<3; y++) {
    for (uint8_t x=0; x<12; x++) {
      gfx_poke(x+4,y+1,(uint8_t)((5+y)*32+x));
    }
    gfx_flush();
  }
  for (uint8_t y=0; y<0xff; y++) {
    /*if ((y % 32) == 0) {
      kbd_brightness_inc();
      }*/
    gfx_contrast(y);
    sleep_ms(0);
  }
  for (uint8_t y=0; y<0xff; y++) {
    /*if ((y % 64) == 0) {
      kbd_brightness_dec();
      }*/
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
  //int16_t brt = kbd_brightness_get();
  /*while (brt--) {
    kbd_brightness_dec();
    Delay_MS(64);
  }*/
  gfx_off();
}
