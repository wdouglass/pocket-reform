/*
  MNT Reform 2.0 Keyboard Firmware
  See keyboard.c for Copyright
  SPDX-License-Identifier: MIT
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define PREF_HID_FW_REV "PREFHID20240412"

void reset_keyboard_state(void);

void led_set(uint32_t rgb);
void led_task(uint32_t rgb);
void led_mod_hue(int d);
void led_mod_brightness(int d);
void led_set_brightness(int b);
void led_mod_saturation(int d);
void led_cycle_hue();

#endif
