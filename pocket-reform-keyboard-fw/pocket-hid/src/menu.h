/*
  SPDX-License-Identifier: GPL-3.0-or-later
  MNT Pocket Reform Keyboard/Trackball Controller Firmware for RP2040
  Copyright 2021-2024 MNT Research GmbH (mntre.com)
*/

#ifndef _MENU_H_
#define _MENU_H_

typedef struct MenuItem {
  const char* title;
  int keycode;
} MenuItem;

#define MENU_PAGE_NONE 0
#define MENU_PAGE_OTHER 1
#define MENU_PAGE_BATTERY_STATUS 2
#define MENU_PAGE_MNT_LOGO 3

void reset_and_render_menu(void);
void reset_menu(void);
void render_menu(int y);
void refresh_menu_page(void);
int execute_menu_row_function(int y);
int execute_menu_function(int keycode);
void render_tina(void);
void anim_hello(void);
void anim_goodbye(void);

#endif
