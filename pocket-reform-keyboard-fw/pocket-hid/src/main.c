/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"

#include "usb_descriptors.h"
#include "oled.h"

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

void hid_task(void);
void led_task(uint32_t rgb, int bar_sz);
void led_core(void);
int process_keyboard(uint8_t* resulting_scancodes);

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

static char uart_rx_line[128];
static int uart_rx_i=0;

void on_uart_rx() {
  while (uart_is_readable(UART_ID)) {
    char c = uart_getc(UART_ID);
    if (uart_rx_i<127) {
      uart_rx_line[uart_rx_i++] = c;
      uart_rx_line[uart_rx_i] = 0;
    }
    if (c == '\n') {
      // TODO hack
      if (uart_rx_i>4) {
        // cut off after 4 digits
        uart_rx_line[4] = 0;
        float percentage = ((float)atoi(uart_rx_line))/(float)10.0;
        float bar_sz = (percentage/(float)100.0)*12;
        led_task(0x008800, (int)bar_sz);
      }
      uart_rx_i = 0;
    }
  }
}

/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();

  uart_init(UART_ID, BAUD_RATE);
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
  uart_set_hw_flow(UART_ID, false, false);
  uart_set_fifo_enabled(UART_ID, true);
  gpio_set_function(PIN_UART_TX, GPIO_FUNC_UART);
  gpio_set_function(PIN_UART_RX, GPIO_FUNC_UART);
  unsigned int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);
  uart_set_irq_enables(UART_ID, true, false); // bool rx_has_data, bool tx_needs_data

  gpio_pull_up(PIN_COL1);
  gpio_init(PIN_LEDS);
  gpio_set_dir(PIN_LEDS, true); // output

  gpio_init(PIN_COL1);
  gpio_set_dir(PIN_COL1, false);
  gpio_pull_up(PIN_COL1);
  gpio_init(PIN_COL2);
  gpio_set_dir(PIN_COL2, false);
  gpio_pull_up(PIN_COL2);
  gpio_init(PIN_COL3);
  gpio_set_dir(PIN_COL3, false);
  gpio_pull_up(PIN_COL3);
  gpio_init(PIN_COL4);
  gpio_set_dir(PIN_COL4, false);
  gpio_pull_up(PIN_COL4);
  gpio_init(PIN_COL5);
  gpio_set_dir(PIN_COL5, false);
  gpio_pull_up(PIN_COL5);
  gpio_init(PIN_COL6);
  gpio_set_dir(PIN_COL6, false);
  gpio_pull_up(PIN_COL6);
  gpio_init(PIN_COL7);
  gpio_set_dir(PIN_COL7, false);
  gpio_pull_up(PIN_COL7);
  gpio_init(PIN_COL8);
  gpio_set_dir(PIN_COL8, false);
  gpio_pull_up(PIN_COL8);
  gpio_init(PIN_COL9);
  gpio_set_dir(PIN_COL9, false);
  gpio_pull_up(PIN_COL9);
  gpio_init(PIN_COL10);
  gpio_set_dir(PIN_COL10, false);
  gpio_pull_up(PIN_COL10);
  gpio_init(PIN_COL11);
  gpio_set_dir(PIN_COL11, false);
  gpio_pull_up(PIN_COL11);
  gpio_init(PIN_COL12);
  gpio_set_dir(PIN_COL12, false);
  gpio_pull_up(PIN_COL12);

  gpio_init(PIN_ROW1);
  gpio_set_dir(PIN_ROW1, true);
  gpio_init(PIN_ROW2);
  gpio_set_dir(PIN_ROW2, true);
  gpio_init(PIN_ROW3);
  gpio_set_dir(PIN_ROW3, true);
  gpio_init(PIN_ROW4);
  gpio_set_dir(PIN_ROW4, true);
  gpio_init(PIN_ROW5);
  gpio_set_dir(PIN_ROW5, true);
  gpio_init(PIN_ROW6);
  gpio_set_dir(PIN_ROW6, true);

  i2c_init(i2c0, 100 * 1000);
  gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);

  bi_decl(bi_2pins_with_func(PIN_SDA, PIN_SCL, GPIO_FUNC_I2C));

  unsigned char buf[] = {0x7f, 0x00, 0x00, 0x00};
  i2c_write_blocking(i2c0, ADDR_SENSOR, buf, 2, false);

  buf[0] = 0x05;
  buf[1] = 0x01;
  i2c_write_blocking(i2c0, ADDR_SENSOR, buf, 2, false);

  //multicore_launch_core1(led_core);
  /*for (int i=0; i<10; i++) {
    led_task(0x000000);
    }*/

  gfx_init(false);
  gfx_poke_str(0, 0, "Hello Pocket!");
  gfx_flush();

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
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

// RGB LEDS

int __attribute__((optimize("Os"))) delay300ns() {
  // ~300ns
  asm volatile(
               "mov  r0, #6\n"    		// 1 cycle (was 10)
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
int active_meta_mode = 0;
uint8_t last_meta_key = 0;
uint8_t* active_matrix = matrix;
bool media_toggle = 0;
bool fn_key = 0; // Am I holding FN?
bool circle = 0; // Am I holding circle?

int command_sent = 0;

int process_keyboard(uint8_t* resulting_scancodes) {
  // how many keys are pressed this round
  uint8_t total_pressed = 0;
  uint8_t used_key_codes = 0;

  for (int i=0; i<MAX_SCANCODES; i++) {
    pressed_scancodes[i] = 0;
  }

  // pull ROWs low one after the other
  for (int y = 0; y < KBD_ROWS; y++) {
    switch (y) {
    case 0: gpio_put(PIN_ROW1, 0); break;
    case 1: gpio_put(PIN_ROW2, 0); break;
    case 2: gpio_put(PIN_ROW3, 0); break;
    case 3: gpio_put(PIN_ROW4, 0); break;
    case 4: gpio_put(PIN_ROW5, 0); break;
    case 5: gpio_put(PIN_ROW6, 0); break;
    }

    // wait for signal to stabilize
    // TODO maybe not necessary
    //_delay_us(10);

    // check input COLs
    for (int x = 0; x < KBD_COLS; x++) {
      uint8_t keycode;
      int loc = y*KBD_COLS+x;
      keycode = active_matrix[loc];
      uint8_t  pressed = 0;
      uint8_t  debounced_pressed = 0;

      switch (x) {
      case 0:  pressed = !gpio_get(PIN_COL1); break;
      case 1:  pressed = !gpio_get(PIN_COL2); break;
      case 2:  pressed = !gpio_get(PIN_COL3); break;
      case 3:  pressed = !gpio_get(PIN_COL4); break;
      case 4:  pressed = !gpio_get(PIN_COL5); break;
      case 5:  pressed = !gpio_get(PIN_COL6); break;
      case 6:  pressed = !gpio_get(PIN_COL7); break;
      case 7:  pressed = !gpio_get(PIN_COL8); break;
      case 8:  pressed = !gpio_get(PIN_COL9); break;
      case 9:  pressed = !gpio_get(PIN_COL10); break;
      case 10: pressed = !gpio_get(PIN_COL11); break;
      case 11: pressed = !gpio_get(PIN_COL12); break;
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

        // Is circle key pressed?
        // FIXME HACK

        if (keycode == KEY_COMPOSE && (matrix_state[0] || matrix_state[11]) && !command_sent) {
          if (matrix_state[0]) {
            // ESC
            //remote_turn_som_power_on();
            uart_puts(UART_ID, "1p\r\n");
            command_sent = 1;
          } else if (matrix_state[11]) {
            //remote_turn_som_power_off();
            led_task(0x000000, 1);
            uart_puts(UART_ID, "0p\r\n");
            command_sent = 1;
          }
        } else if (keycode == HID_KEY_EXECUTE) {
          fn_key = 1;
          //active_matrix = matrix_fn;
        } else {
          if (active_meta_mode) {
            // not holding the same key?
            if (last_meta_key != keycode) {
              // hyper/circle/menu functions
              int stay_meta = 0; //execute_meta_function(keycode);
              // don't repeat action while key is held down
              last_meta_key = keycode;

              // exit meta mode
              if (!stay_meta) {
                active_meta_mode = 0;
              }

              // after wake-up from sleep mode, skip further keymap processing
              if (stay_meta == 2) {
                //reset_keyboard_state();
                //enter_meta_mode();
                return 0;
              }
            }
          } else if (!last_meta_key) {
            // not meta mode, regular key: report keypress via USB
            // 6 keys is the limit in the HID descriptor
            if (used_key_codes < MAX_SCANCODES && resulting_scancodes && y < 5) {
              resulting_scancodes[used_key_codes++] = keycode;
            }
          }
        }
      } else {
        // key not pressed
        if (keycode == KEY_COMPOSE) {
          command_sent = 0;
        }
        else if (keycode == HID_KEY_EXECUTE) {
          fn_key = 0;
          if (media_toggle) {
            //active_matrix = matrix_fn_toggled;
          } else {
            //active_matrix = matrix;
          }
        } else if (keycode == KEY_SYSRQ) {
          if (fn_key && circle) {
            if (!media_toggle) {
              media_toggle = 1;
              //active_matrix = matrix_fn_toggled;
            } else {
              media_toggle = 0;
              //active_matrix = matrix_fn;
            }
          }
          circle = 0;
        }
      }
    }

    switch (y) {
    case 0: gpio_put(PIN_ROW1, 1); break;
    case 1: gpio_put(PIN_ROW2, 1); break;
    case 2: gpio_put(PIN_ROW3, 1); break;
    case 3: gpio_put(PIN_ROW4, 1); break;
    case 4: gpio_put(PIN_ROW5, 1); break;
    case 5: gpio_put(PIN_ROW6, 1); break;
    }
  }

  // if no more keys are held down, allow a new meta command
  if (total_pressed<1) last_meta_key = 0;

  return used_key_codes;
}


int prev_buttons = 0;
int scroll_toggle = 0;
int prev_num_keys = 0;

static void send_hid_report(uint8_t report_id)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  switch (report_id) {
    case REPORT_ID_KEYBOARD:
    {
      //int num_keys = process_keyboard(pressed_scancodes);

      //if (num_keys > 0) {
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, pressed_scancodes);
        //} else {
        // send empty key report if previously has key pressed
        //if (prev_num_keys) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        //}
      prev_num_keys = pressed_keys;
    }
    break;

    case REPORT_ID_MOUSE:
    {
      uint8_t buf[] = {0x7f, 0x00, 0x00, 0x00};

      buf[0] = 0x02;
      i2c_write_blocking(i2c0, ADDR_SENSOR, buf, 1, true);
      i2c_read_blocking(i2c0, ADDR_SENSOR, buf, 1, false);

      int btn3 = matrix_state[KBD_COLS*5+3]>0;
      int btn1 = matrix_state[KBD_COLS*5+4]>0;
      int btn2 = matrix_state[KBD_COLS*5+7]>0;
      int btn4 = matrix_state[KBD_COLS*5+8]>0;

      int buttons = btn1 | (btn2<<1) | (btn4<<2);

      if (buf[0] & 0xf0) {
        buf[0] = 0x03;
        i2c_write_blocking(i2c0, ADDR_SENSOR, buf, 1, true);
        i2c_read_blocking(i2c0, ADDR_SENSOR, buf, 2, false);

        int8_t nx = (int8_t)buf[0];
        int8_t ny = (int8_t)buf[1];

        // no button, right + down, no scroll pan
        if (btn3 || scroll_toggle) {
          tud_hid_mouse_report(REPORT_ID_MOUSE, (uint8_t)buttons, 0, 0, 2*ny, -2*nx);
        } else {
          tud_hid_mouse_report(REPORT_ID_MOUSE, (uint8_t)buttons, -2*nx, -2*ny, 0, 0);
        }
      } else {
        //if (buttons != prev_buttons) {
          tud_hid_mouse_report(REPORT_ID_MOUSE, (uint8_t)buttons, 0, 0, 0, 0);
          //}
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
      // use to avoid send multiple consecutive zero report for keyboard
      /*static bool has_gamepad_key = false;

      hid_gamepad_report_t report =
      {
        .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
        .hat = 0, .buttons = 0
      };

      if ( btn )
      {
        report.hat = GAMEPAD_HAT_UP;
        report.buttons = GAMEPAD_BUTTON_A;
        tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

        has_gamepad_key = true;
      }else
      {
        report.hat = GAMEPAD_HAT_CENTERED;
        report.buttons = 0;
        if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
        has_gamepad_key = false;
        }*/
    }
    break;

    default: break;
  }
}

int led_counter = 0;
int rgb_channel = 0;
int rgb_val = 0;

void led_task(uint32_t rgb, int bar_sz) {
  uint32_t color = rgb;

  for (int y=0; y<6; y++) {
    int w = 12;
    if (y==5) w = 4;
    for (int x=0; x<w; x++) {
      uint32_t bits = color;

      if (x>bar_sz || y>0) bits = 0;

      for (int i=23; i>=0; i--) {
        if (bits & (1<<23)) {
          // one
          gpio_put(PIN_LEDS, 1);
          delay300ns();
          delay300ns();
          delay300ns();
          gpio_put(PIN_LEDS, 0);
          // ~600ms delay
          delay300ns();
          delay300ns();
        } else {
          // zero
          gpio_put(PIN_LEDS, 1);
          delay300ns();
          gpio_put(PIN_LEDS, 0);
          // ~1.2ms delay
          delay300ns();
          delay300ns();
          delay300ns();
          delay300ns();
          //delay300ns();
        }
        bits <<= 1;
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

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  uint32_t const btn = 0; //board_button_read();

  // Remote wakeup
  if ( tud_suspended() && btn )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  } else {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_KEYBOARD);
  }
}

void led_core() {
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
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

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
  (void) buffer;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      //uint8_t const kbd_leds = buffer[0];
    }
  }
}
