/*
  SPDX-License-Identifier: GPL-3.0-or-later
  MNT Pocket Reform Keyboard/Trackball Controller Firmware for RP2040
  Copyright 2021-2024 MNT Research GmbH (mntre.com)
*/

#include "usb_hid_keys.h"

// Top row, left to right
#define MATRIX_DEFAULT_ROW_1 \
  KEY_ESC,\
  KEY_1,\
  KEY_2,\
  KEY_3,\
  KEY_4,\
  KEY_5,\
  KEY_6,\
  KEY_7,\
  KEY_8,\
  KEY_9,\
  KEY_0,\
  KEY_BACKSPACE

// Third row
#define MATRIX_DEFAULT_ROW_2 \
  KEY_TAB,\
  KEY_Q,\
  KEY_W,\
  KEY_E,\
  KEY_R,\
  KEY_T,\
  KEY_Y,\
  KEY_U,\
  KEY_I,\
  KEY_O,\
  KEY_P,\
  KEY_SEMICOLON

// Fourth row
#define MATRIX_DEFAULT_ROW_3 \
  KEY_LEFTCTRL,\
  KEY_A,\
  KEY_S,\
  KEY_D,\
  KEY_F,\
  KEY_G,\
  KEY_H,\
  KEY_J,\
  KEY_K,\
  KEY_L,\
  KEY_APOSTROPHE,\
  KEY_ENTER

// Fifth row
#define MATRIX_DEFAULT_ROW_4 \
  KEY_LEFTSHIFT,\
  KEY_Z,\
  KEY_X,\
  KEY_C,\
  KEY_V,\
  KEY_B,\
  KEY_N,\
  KEY_M,\
  KEY_COMMA,\
  KEY_DOT,\
  KEY_SLASH,\
  KEY_RIGHTALT

// Sixth row
#define MATRIX_DEFAULT_ROW_5 \
  KEY_COMPOSE,\
  KEY_LEFTMETA,\
  KEY_LEFTALT,\
  KEY_BACKSLASH,\
  KEY_EQUAL,\
  KEY_SPACE,\
  KEY_SPACE,\
  KEY_MINUS,\
  KEY_LEFT,\
  KEY_DOWN,\
  KEY_UP,\
  KEY_RIGHT

// Every line of `matrix` is a row of the keyboard, starting from the top.
// Check keyboard.h for the definitions of the default rows.
uint8_t matrix[KBD_MATRIX_SZ] = {
  MATRIX_DEFAULT_ROW_1,
  MATRIX_DEFAULT_ROW_2,
  MATRIX_DEFAULT_ROW_3,
  MATRIX_DEFAULT_ROW_4,
  MATRIX_DEFAULT_ROW_5
};

// When holding down HYPER
uint8_t matrix_fn[KBD_MATRIX_SZ] = {
  // Media keys on Hyper + F7-F12
  KEY_GRAVE,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_DELETE,

  KEY_TAB,
  KEY_F11,
  KEY_F12,
  KEY_F13,
  KEY_R,
  KEY_T,
  KEY_Y,
  KEY_U,
  KEY_I,
  KEY_LEFTBRACE,
  KEY_RIGHTBRACE,
  KEY_SEMICOLON,

  KEY_LEFTCTRL,
  KEY_A,
  KEY_S,
  KEY_D,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_APOSTROPHE,
  KEY_ENTER,

  KEY_LEFTSHIFT,
  KEY_Z,
  KEY_X,
  KEY_C,
  KEY_V,
  KEY_B,
  KEY_N,
  KEY_M,
  KEY_COMMA,
  KEY_DOT,
  KEY_PAGEUP,
  KEY_RIGHTSHIFT,

  KEY_COMPOSE,
  KEY_RIGHTMETA,
  KEY_RIGHTALT,
  KEY_BACKSLASH,
  KEY_EQUAL,
  KEY_SPACE,
  KEY_SPACE,
  KEY_MINUS,
  KEY_SLASH,
  KEY_HOME,
  KEY_PAGEDOWN,
  KEY_END
};
