/*
  SPDX-License-Identifier: GPL-3.0-or-later
  MNT Pocket Reform Keyboard/Trackball Controller Firmware for RP2040
  Copyright 2021-2024 MNT Research GmbH (mntre.com)
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define PREF_HID_FW_REV "PREFHID20240416"

void reset_keyboard_state(void);

void led_set(uint32_t rgb);
void led_task(uint32_t rgb);
void led_mod_hue(int d);
void led_mod_brightness(int d);
void led_set_brightness(int b);
void led_mod_saturation(int d);
void led_cycle_hue();

#endif
