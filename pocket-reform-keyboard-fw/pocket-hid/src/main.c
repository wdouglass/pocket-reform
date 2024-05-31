/*
  SPDX-License-Identifier: GPL-3.0-or-later
  MNT Pocket Reform Keyboard/Trackball Controller Firmware for RP2040
  Copyright 2021-2024 MNT Research GmbH (mntre.com)

  TinyUSB callbacks/code based on code
  Copyright 2019 Ha Thach (tinyusb.org)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"

#include "usb_descriptors.h"
#include "oled.h"
#include "menu.h"
#include "remote.h"
#include "keyboard.h"

#include "ws2812.pio.h"

#define KBD_VARIANT_QWERTY_US
#define KBD_COLS 12
#define KBD_ROWS 6
#define KBD_MATRIX_SZ KBD_COLS * KBD_ROWS + 4

#include "matrix.h"

#define PIN_SDA 0
#define PIN_SCL 1

#define PIN_UART_TX 4
#define PIN_UART_RX 5

#define PIN_ROW1 19
#define PIN_ROW2 20
#define PIN_ROW3 23
#define PIN_ROW4 22
#define PIN_ROW5 21
#define PIN_ROW6 18

#define PIN_COL1 6
#define PIN_COL2 7
#define PIN_COL3 8
#define PIN_COL4 9
#define PIN_COL5 10
#define PIN_COL6 11
#define PIN_COL7 12
#define PIN_COL8 13
#define PIN_COL9 14
#define PIN_COL10 15
#define PIN_COL11 16
#define PIN_COL12 17

#define PIN_LEDS 24

#define ADDR_SENSOR (0x79)

#define MAX_SCANCODES 6
static uint8_t pressed_scancodes[MAX_SCANCODES] = {0,0,0,0,0,0};
static int pressed_keys = 0;
static volatile uint32_t led_value = 0;

void hid_task(void);
int process_keyboard(uint8_t* resulting_scancodes);

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// can be used as a global clock, incrementing around every ~10ms
static int hid_task_counter = 0;

int main(void)
{
  board_init();

  set_sys_clock_48mhz();

  tusb_init();

  uart_init(UART_ID, BAUD_RATE);
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
  uart_set_hw_flow(UART_ID, false, false);
  uart_set_fifo_enabled(UART_ID, true);
  gpio_set_function(PIN_UART_TX, GPIO_FUNC_UART);
  gpio_set_function(PIN_UART_RX, GPIO_FUNC_UART);
  unsigned int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  gpio_init(PIN_LEDS);
  gpio_set_dir(PIN_LEDS, true); // output

  gpio_init(PIN_COL1);
  gpio_set_dir(PIN_COL1, true);
  gpio_init(PIN_COL2);
  gpio_set_dir(PIN_COL2, true);
  gpio_init(PIN_COL3);
  gpio_set_dir(PIN_COL3, true);
  gpio_init(PIN_COL4);
  gpio_set_dir(PIN_COL4, true);
  gpio_init(PIN_COL5);
  gpio_set_dir(PIN_COL5, true);
  gpio_init(PIN_COL6);
  gpio_set_dir(PIN_COL6, true);
  gpio_init(PIN_COL7);
  gpio_set_dir(PIN_COL7, true);
  gpio_init(PIN_COL8);
  gpio_set_dir(PIN_COL8, true);
  gpio_init(PIN_COL9);
  gpio_set_dir(PIN_COL9, true);
  gpio_init(PIN_COL10);
  gpio_set_dir(PIN_COL10, true);
  gpio_init(PIN_COL11);
  gpio_set_dir(PIN_COL11, true);
  gpio_init(PIN_COL12);
  gpio_set_dir(PIN_COL12, true);

  gpio_init(PIN_ROW1);
  gpio_set_dir(PIN_ROW1, false);
  gpio_pull_down(PIN_ROW1);
  gpio_init(PIN_ROW2);
  gpio_set_dir(PIN_ROW2, false);
  gpio_pull_down(PIN_ROW2);
  gpio_init(PIN_ROW3);
  gpio_set_dir(PIN_ROW3, false);
  gpio_pull_down(PIN_ROW3);
  gpio_init(PIN_ROW4);
  gpio_set_dir(PIN_ROW4, false);
  gpio_pull_down(PIN_ROW4);
  gpio_init(PIN_ROW5);
  gpio_set_dir(PIN_ROW5, false);
  gpio_pull_down(PIN_ROW5);
  gpio_init(PIN_ROW6);
  gpio_set_dir(PIN_ROW6, false);
  gpio_pull_down(PIN_ROW6);

  i2c_init(i2c0, 100 * 1000);
  gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);

  bi_decl(bi_2pins_with_func(PIN_SDA, PIN_SCL, GPIO_FUNC_I2C));

  unsigned char buf[] = {0x7f, 0x00, 0x00, 0x00};
  i2c_write_blocking(i2c0, ADDR_SENSOR, buf, 2, false);

  buf[0] = 0x05;
  buf[1] = 0x01;
  i2c_write_blocking(i2c0, ADDR_SENSOR, buf, 2, false);

  PIO pio = pio0;
  uint sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);

  ws2812_program_init(pio, sm, offset, PIN_LEDS, 800000, false);

  led_set_brightness(0x0);

  gfx_init(false);
  //anim_hello();

  irq_set_exclusive_handler(UART_IRQ, remote_on_uart_rx);
  irq_set_enabled(UART_IRQ, true);
  // bool rx_has_data, bool tx_needs_data
  uart_set_irq_enables(UART_ID, true, false);

  while (1) {
    pressed_keys = process_keyboard(pressed_scancodes);
    tud_task(); // tinyusb device task
    hid_task();
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  // called
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  // never called
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  // never called
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  // never called
}

// RGB LEDS

int __attribute__((optimize("Os"))) delay300ns() {
  // ~300ns
  asm volatile(
               "mov  r0, #9\n"    		// 1 cycle (was 10)
               "loop1: sub  r0, r0, #1\n"	// 1 cycle
               "bne   loop1\n"          	// 2 cycles if loop taken, 1 if not
               );
  return 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

typedef struct TU_ATTR_PACKED
{
  uint8_t buttons; /**< buttons mask for currently pressed buttons in the mouse. */
  int8_t  x;       /**< Current delta x movement of the mouse. */
  int8_t  y;       /**< Current delta y movement on the mouse. */
  int8_t  wheel;   /**< Current delta wheel movement on the mouse. */
  int8_t  pan;     // using AC Pan
} hid_trackball_report_t;

bool tud_hid_trackball_report(uint8_t report_id,
                            uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal)
{
  hid_trackball_report_t report =
  {
    .buttons = buttons,
    .x       = x,
    .y       = y,
    .wheel   = vertical,
    .pan     = horizontal
  };

  return tud_hid_report(report_id, &report, sizeof(report));
}

uint8_t matrix_debounce[KBD_COLS*KBD_ROWS];
uint8_t matrix_state[KBD_COLS*KBD_ROWS];

int active_menu_mode = 0;
uint8_t last_menu_key = 0;
uint32_t hyper_enter_long_press_start_ms = 0;

uint8_t* active_matrix = matrix;
bool media_toggle = 0;
bool hyper_key = 0; // Am I holding HYPER?

// enter the menu
void enter_menu_mode(void) {
  active_menu_mode = 1;
  reset_and_render_menu();
}

void exit_menu_mode(void) {
  active_menu_mode = 0;
}

void reset_keyboard_state(void) {
  for (int i = 0; i < KBD_COLS*KBD_ROWS; i++) {
    matrix_debounce[i] = 0;
    matrix_state[i] = 0;
  }
  last_menu_key = 0;
  reset_menu();
}

int process_keyboard(uint8_t* resulting_scancodes) {
  // how many keys are pressed this round
  uint8_t total_pressed = 0;
  uint8_t used_key_codes = 0;

  for (int i=0; i<MAX_SCANCODES; i++) {
    pressed_scancodes[i] = 0;
  }

  for (int x = 0; x < KBD_COLS; x++) {
    gpio_put(PIN_COL1, 0);
    gpio_put(PIN_COL2, 0);
    gpio_put(PIN_COL3, 0);
    gpio_put(PIN_COL4, 0);
    gpio_put(PIN_COL5, 0);
    gpio_put(PIN_COL6, 0);
    gpio_put(PIN_COL7, 0);
    gpio_put(PIN_COL8, 0);
    gpio_put(PIN_COL9, 0);
    gpio_put(PIN_COL10, 0);
    gpio_put(PIN_COL11, 0);
    gpio_put(PIN_COL12, 0);

    switch (x) {
    case 0: gpio_put(PIN_COL1, 1); break;
    case 1: gpio_put(PIN_COL2, 1); break;
    case 2: gpio_put(PIN_COL3, 1); break;
    case 3: gpio_put(PIN_COL4, 1); break;
    case 4: gpio_put(PIN_COL5, 1); break;
    case 5: gpio_put(PIN_COL6, 1); break;
    case 6: gpio_put(PIN_COL7, 1); break;
    case 7: gpio_put(PIN_COL8, 1); break;
    case 8: gpio_put(PIN_COL9, 1); break;
    case 9: gpio_put(PIN_COL10, 1); break;
    case 10: gpio_put(PIN_COL11, 1); break;
    case 11: gpio_put(PIN_COL12, 1); break;
    }

    // wait for signal to stabilize
    //_delay_us(10);
    sleep_us(1);

    for (int y = 0; y < KBD_ROWS; y++) {
      uint8_t keycode;
      int loc = y*KBD_COLS+x;
      keycode = active_matrix[loc];
      uint8_t  pressed = 0;
      uint8_t  debounced_pressed = 0;

      switch (y) {
      case 0:  pressed = gpio_get(PIN_ROW1); break;
      case 1:  pressed = gpio_get(PIN_ROW2); break;
      case 2:  pressed = gpio_get(PIN_ROW3); break;
      case 3:  pressed = gpio_get(PIN_ROW4); break;
      case 4:  pressed = gpio_get(PIN_ROW5); break;
      case 5:  pressed = gpio_get(PIN_ROW6); break;
      }

      // shift new state as bit into debounce "register"
      matrix_debounce[loc] = (uint8_t)(matrix_debounce[loc]<<1)|pressed;

      // if unclear state, we need to keep the last state of the key
      if (matrix_debounce[loc] == 0x00) {
        matrix_state[loc] = 0;
      } else if (matrix_debounce[loc] == 0x01) {
        matrix_state[loc] = 1;
      }
      debounced_pressed = matrix_state[loc];

      if (debounced_pressed) {
        total_pressed++;

        // hyper + enter? open OLED menu
        if (keycode == KEY_ENTER && hyper_key) {
          if (!last_menu_key) {
            if (!active_menu_mode) {
              enter_menu_mode();
            }
            uint32_t now_ms = board_millis();
            if (!now_ms) now_ms++;

            if (!hyper_enter_long_press_start_ms) {
              hyper_enter_long_press_start_ms = now_ms;
              // edge case
            }
            if (now_ms - hyper_enter_long_press_start_ms > 1000) {
              // turn on computer after 2 seconds of holding hyper + enter
              execute_menu_function(KEY_1);
              exit_menu_mode();
              last_menu_key = KEY_1;
              hyper_enter_long_press_start_ms = 0;
            }
          }
        } else if (keycode == KEY_F12) {
          remote_wake_som();
        } else if (keycode == KEY_COMPOSE) {
          hyper_key = 1;
          active_matrix = matrix_fn;
        } else {
          if (active_menu_mode) {
            // not holding the same key?
            if (last_menu_key != keycode) {
              // hyper/menu functions
              int stay_menu = execute_menu_function(keycode);
              // don't repeat action while key is held down
              last_menu_key = keycode;

              // exit menu mode
              if (!stay_menu) {
                exit_menu_mode();
              }

              // after wake-up from sleep mode, skip further keymap processing
              if (stay_menu == 2) {
                reset_keyboard_state();
                enter_menu_mode();
                return 0;
              }
            }
          } else if (!last_menu_key) {
            // not menu mode, regular key: report keypress via USB
            // 6 keys is the limit in the HID descriptor
            if (used_key_codes < MAX_SCANCODES && resulting_scancodes && y < 5) {
              resulting_scancodes[used_key_codes++] = keycode;
            }
          }
        }
      } else {
        // key not pressed
        if (keycode == KEY_COMPOSE) {
          hyper_key = 0;
          active_matrix = matrix;
          hyper_enter_long_press_start_ms = 0;
        } else if (keycode == KEY_ENTER) {
          hyper_enter_long_press_start_ms = 0;
        }
      }
    }
  }

  // if no more keys are held down, allow a new menu command
  if (total_pressed<1) last_menu_key = 0;

  // if device is off and user is pressing random keys,
  // show a hint for turning on the device
  if (!remote_get_power_state()) {
    if (total_pressed>0 && !active_menu_mode && !hyper_key && !last_menu_key) {
      execute_menu_function(KEY_H);
    }
  }

  return used_key_codes;
}


int prev_buttons = 0;
int scroll_toggle = 0;
int prev_num_keys = 0;

int8_t tb_nx = 0;
int8_t tb_ny = 0;
int tb_btn1 = 0;
int tb_btn2 = 0;
int tb_btn3 = 0;
int tb_btn4 = 0;

// returns motion yes/no
static int poll_trackball()
{
  tb_btn1 = matrix_state[KBD_COLS*5+3]>0; // left
  tb_btn4 = matrix_state[KBD_COLS*5+4]>0; // middle
  tb_btn3 = matrix_state[KBD_COLS*5+7]>0; // scroll
  tb_btn2 = matrix_state[KBD_COLS*5+8]>0; // right

  uint8_t buf[] = {0x7f, 0x00, 0x00, 0x00};

  buf[0] = 0x02;

  i2c_write_blocking_until(i2c0, ADDR_SENSOR, buf, 1, true, make_timeout_time_ms(2));
  i2c_read_blocking_until(i2c0, ADDR_SENSOR, buf, 1, false, make_timeout_time_ms(2));

  if (buf[0] & 0xf0) {
    buf[0] = 0x03;
    i2c_write_blocking_until(i2c0, ADDR_SENSOR, buf, 1, true, make_timeout_time_ms(2));
    i2c_read_blocking_until(i2c0, ADDR_SENSOR, buf, 2, false, make_timeout_time_ms(2));

    tb_nx = (int8_t)buf[0];
    tb_ny = (int8_t)buf[1];

    // backlight hue/value wheel
    if (matrix_state[KBD_COLS*4+0]) {
      if (tb_ny) {
        // shift held? saturation
        if (matrix_state[KBD_COLS*3+0]) {
          led_mod_saturation(tb_ny);
        } else {
          led_mod_brightness(tb_ny);
        }
      }
      if (tb_nx) {
        led_mod_hue(tb_nx);
      }
    }

    return 1;
  }
  return 0;
}

static void send_hid_report(uint8_t report_id)
{
  // skip if hid is not ready yet
  if (!tud_hid_ready()) return;

  switch (report_id) {
    case REPORT_ID_KEYBOARD:
    {
      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, pressed_scancodes);
      prev_num_keys = pressed_keys;
    }
    break;

    case REPORT_ID_MOUSE:
    {
      int buttons = tb_btn1 | (tb_btn2<<1) | (tb_btn4<<2);
      int motion = poll_trackball();

      if (motion) {
        // no button, right + down, no scroll pan
        if (tb_btn3 || scroll_toggle) {
          tud_hid_mouse_report(REPORT_ID_MOUSE, (uint8_t)buttons, 0, 0, 2*tb_ny, -2*tb_nx);
        } else {
          tud_hid_mouse_report(REPORT_ID_MOUSE, (uint8_t)buttons, -2*tb_nx, -2*tb_ny, 0, 0);
        }
      } else {
        if (tb_btn2 && tb_btn3) {
          // enter sticky scroll mode
          scroll_toggle = 1;
        } else {
          if (tb_btn1 && scroll_toggle) {
            // exit sticky scroll mode
            scroll_toggle = 0;
          } else {
            // actually report the buttons
            tud_hid_mouse_report(REPORT_ID_MOUSE, (uint8_t)buttons, 0, 0, 0, 0);
          }
        }
      }

      prev_buttons = buttons;
    }
    break;

    case REPORT_ID_CONSUMER_CONTROL:
    {
      // use to avoid send multiple consecutive zero report
      /*static bool has_consumer_key = false;

      if ( btn )
      {
        // volume down
        uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
        has_consumer_key = true;
      }else
      {
        // send empty key report (release key) if previously has key pressed
        uint16_t empty_key = 0;
        if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
        has_consumer_key = false;
        }*/
    }
    break;

    case REPORT_ID_GAMEPAD:
    {
      // TODO: later
    }
    break;

    default: break;
  }
}

void led_task(uint32_t color) {
  uint32_t pixel_grb = color;

  for (int y=0; y<6; y++) {
    int w = 12;
    if (y==5) w = 4;
    for (int x=0; x<w; x++) {
      pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
    }
  }
}

uint8_t led_rgb_buf[12*3*6];

void led_bitmap(uint8_t row, const uint8_t* row_rgb) {
  // row = 5 -> commit
  if (row > 5) return;

  uint8_t* store = &led_rgb_buf[row*3*12];
  int offset = 0;
  for (int x=0; x<3*12; x++) {
    if (row == 5 && x == 0*3) offset = 3*3;
    if (row == 5 && x == 2*3) offset = 7*3;
    store[x] = row_rgb[x+offset];
    if (row == 5 && x == 4*3) break;
  }

  if (row == 5) {
    for (int y=0; y<6; y++) {
      int w = 12;
      if (y==5) w = 4;
      uint8_t* bitmap = &led_rgb_buf[y*3*12];
      for (int x=0; x<w; x++) {
        uint32_t pixel_grb = (bitmap[1]<<16u) | (bitmap[2]<<8u) | bitmap[0];
        pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
        bitmap+=3;
      }
    }
  }
}


// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms) {
    // not enough time passed
    return;
  }
  start_ms += interval_ms;

  // Remote wakeup
  if (tud_suspended() && pressed_keys > 0)
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  } else {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_KEYBOARD);
  }

  hid_task_counter++;
  if (hid_task_counter%1000 == 0) {
    // quietly get voltages to update power state
    remote_get_voltages(1);
  }
  if (hid_task_counter%100 == 0) {
    refresh_menu_page();

    // power state debugging
    /*if (remote_get_power_state()) {
      gfx_poke_cstr(0,0,"[pwr on]");
    } else {
      gfx_poke_cstr(0,0,"[pwr off]");
    }
    gfx_flush();*/
  }
}

int led_brightness = 0;
int led_saturation = 255;
int led_hue = 127;

typedef struct RgbColor
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RgbColor;

typedef struct HsvColor
{
  unsigned char h;
  unsigned char s;
  unsigned char v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv)
{
  RgbColor rgb;
  unsigned char region, remainder, p, q, t;

  if (hsv.s == 0)
    {
      rgb.r = hsv.v;
      rgb.g = hsv.v;
      rgb.b = hsv.v;
      return rgb;
    }

  region = hsv.h / 43;
  remainder = (unsigned char)((hsv.h - (region * 43)) * 6);

  p = (unsigned char)((hsv.v * (255 - hsv.s)) >> 8);
  q = (unsigned char)((hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8);
  t = (unsigned char)((hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8);

  switch (region)
    {
    case 0:
      rgb.r = hsv.v; rgb.g = t; rgb.b = p;
      break;
    case 1:
      rgb.r = q; rgb.g = hsv.v; rgb.b = p;
      break;
    case 2:
      rgb.r = p; rgb.g = hsv.v; rgb.b = t;
      break;
    case 3:
      rgb.r = p; rgb.g = q; rgb.b = hsv.v;
      break;
    case 4:
      rgb.r = t; rgb.g = p; rgb.b = hsv.v;
      break;
    default:
      rgb.r = hsv.v; rgb.g = p; rgb.b = q;
      break;
    }

  return rgb;
}

void led_set(uint32_t rgb) {
  led_task(rgb);
}

void led_set_hsv() {
  HsvColor hsv;
  RgbColor rgb;
  hsv.h = (unsigned char)led_hue;
  hsv.s = (unsigned char)led_saturation;
  hsv.v = (unsigned char)led_brightness;

  rgb = HsvToRgb(hsv);
  led_set((rgb.r<<16)|(rgb.g<<8)|(rgb.b));
}

void led_mod_brightness(int d) {
  led_brightness+=d;
  if (led_brightness>0xff) led_brightness = 0xff;
  if (led_brightness<0) led_brightness = 0;
  led_set_hsv();
}

void led_mod_hue(int d) {
  led_hue+=d;
  if (led_hue>0xff) led_hue = 0xff;
  if (led_hue<0) led_hue = 0;
  led_set_hsv();
}

void led_mod_saturation(int d) {
  led_saturation+=d;
  if (led_saturation>0xff) led_saturation = 0xff;
  if (led_saturation<0) led_saturation = 0;
  led_set_hsv();
}

void led_set_saturation(int s) {
  led_saturation = s;
}

void led_set_brightness(int b) {
  led_brightness = b;
  led_set_hsv();
}

void led_cycle_hue() {
  led_hue++;
  if (led_hue>0xff) led_hue = 0;
  led_set_hsv();
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id);
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

#define CMD_TEXT_FRAME      "OLED"     // fill the screen with a single wall of text
#define CMD_OLED_CLEAR      "WCLR"     // clear the oled display
#define CMD_OLED_BITMAP     "WBIT"     // (u16 offset, u8 bytes...) write raw bytes into the oled framebuffer
#define CMD_POWER_OFF       "PWR0"     // turn off power rails
#define CMD_BACKLIGHT       "LITE"     // keyboard backlight level
#define CMD_RGB_BACKLIGHT   "LRGB"     // keyboard backlight rgb
#define CMD_RGB_BITMAP      "XRGB"     // push rgb backlight bitmap
#define CMD_LOGO            "LOGO"     // play logo animation
#define CMD_OLED_BRITE      "OBRT"     // OLED brightness level
#define CMD_OLED_BRITE2     "OBR2"     // OLED brightness level

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
  (void) buffer;

  if (bufsize < 5) return;

  if (report_type == 2) {
    // Big Reform style
    if (report_id == 'x') {
      const char* cmd = (const char*)buffer;

      // uncomment for debugging
      /*char repinfo[64];
      sprintf(repinfo, "cm: %c%c%c%c 4: %d sz: %d", cmd[0],cmd[1],cmd[2],cmd[3], cmd[4], bufsize);
      gfx_poke_str(0, 0, repinfo);
      gfx_flush();*/

      if (cmd == strnstr(cmd, CMD_TEXT_FRAME, 4)) {
        gfx_clear();
        gfx_on();

        int c = 4;
        for (uint8_t y=0; y<4; y++) {
          for (uint8_t x=0; x<21; x++) {
            if (buffer[c] == '\n') {
              c++;
              x = 0;
              y++;
            } else if (x < 21 && y < 4) {
              gfx_poke(x, y, buffer[c++]);
            }
            if (c>=bufsize) break;
          }
        }
        gfx_flush();
      }
      else if (cmd == strnstr(cmd, CMD_POWER_OFF, 4)) {
        reset_menu();
        remote_turn_off_som();
        reset_keyboard_state();
      }
      else if (cmd == strnstr(cmd, CMD_OLED_CLEAR, 4)) {
        gfx_clear();
        gfx_flush();
      }
      else if (cmd == strnstr(cmd, CMD_OLED_BITMAP, 4)) {
        matrix_render_direct(&buffer[4]);
      }
      else if (cmd == strnstr(cmd, CMD_RGB_BITMAP, 4)) {
        // row, data (12 * 3 rgb bytes)
        led_bitmap(buffer[4], &buffer[5]);
      }
      else if (cmd == strnstr(cmd, CMD_RGB_BACKLIGHT, 4)) {
        uint32_t pixel_grb = (buffer[5]<<16u) | (buffer[6]<<8u) | buffer[4];
        led_task(pixel_grb);
      }
      else if (cmd == strnstr(cmd, CMD_LOGO, 4)) {
        anim_hello();
      }
      else if (cmd == strnstr(cmd, CMD_OLED_BRITE, 4)) {
        uint8_t val = (uint8_t)atoi((const char*)&buffer[4]);
        gfx_poke(0,0,'0'+(val/100));
        gfx_poke(1,0,'0'+((val%100)/10));
        gfx_poke(2,0,'0'+(val%10));
        gfx_flush();
        gfx_contrast(val);
      }
      else if (cmd == strnstr(cmd, CMD_OLED_BRITE2, 4)) {
        uint8_t val = (uint8_t)atoi((const char*)&buffer[4]);
        gfx_poke(0,0,'0'+(val/100));
        gfx_poke(1,0,'0'+((val%100)/10));
        gfx_poke(2,0,'0'+(val%10));
        gfx_flush();
        gfx_precharge(val);
      }
    }
  }

  /*if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      //uint8_t const kbd_leds = buffer[0];
    }
  }*/
}

// TODO
// from https://github.com/raspberrypi/pico-sdk/issues/1118
// Support for default BOOTSEL reset by changing baud rate
/*void tud_cdc_line_coding_cb(__unused uint8_t itf, cdc_line_coding_t const* p_line_coding) {
    if (p_line_coding->bit_rate == 110) {
        reset_usb_boot(gpio_mask, 0);
    }
}*/
