#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "fusb302b.h"
#include "pd.h"

#define FW_REV "PREF1SYS D1 20220824"

#define PIN_SDA 0
#define PIN_SCL 1

#define PIN_DISP_RESET 2
#define PIN_FLIGHTMODE 3
#define PIN_KBD_UART_TX 4
#define PIN_KBD_UART_RX 5
#define PIN_WOWWAN 6
#define PIN_DISP_EN 7
#define PIN_SOM_MOSI 8
#define PIN_SOM_SS0 9
#define PIN_SOM_SCK 10
#define PIN_SOM_MISO 11
#define PIN_SOM_UART_TX 12
#define PIN_SOM_UART_RX 13
#define PIN_FUSB_INT 14
#define PIN_LED_B 15
#define PIN_LED_R 16
#define PIN_LED_G 17
#define PIN_SOM_WAKE 19
#define PIN_1V1_ENABLE 23
#define PIN_3V3_ENABLE 24
#define PIN_5V_ENABLE 25
#define PIN_USB_SRC_ENABLE 28

// FUSB302B USB-PD controller
#define FUSB_ADDR 0x22
// MAX17320 protector/balancer
// https://datasheets.maximintegrated.com/en/ds/MAX17320.pdf
#define MAX_ADDR1 0x36
#define MAX_ADDR2 0x0b
// MP2762A charger
// https://www.monolithicpower.com/en/documentview/productdocument/index/version/2/document_type/Datasheet/lang/en/sku/MP2762AGV/document_id/9073/
#define MPS_ADDR  0x5c

void i2c_scan() {
  printf("\nI2C Scan\n");
  printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

  for (int addr = 0; addr < (1 << 7); ++addr) {
    if (addr % 16 == 0) {
      printf("%02x ", addr);
    }

    int ret;
    uint8_t rxdata;
    ret = i2c_read_blocking(i2c0, addr, &rxdata, 1, false);

    printf(ret < 0 ? "." : "@");
    printf(addr % 16 == 15 ? "\n" : "  ");
  }
}

uint8_t max_read_byte(uint8_t addr)
{
  uint8_t buf;
  i2c_write_blocking(i2c0, MAX_ADDR1, &addr, 1, true);
  i2c_read_blocking(i2c0, MAX_ADDR1, &buf, 1, false);
  return buf;
}

uint16_t max_read_word(uint8_t addr)
{
  uint8_t buf[2];
  i2c_write_blocking(i2c0, MAX_ADDR1, &addr, 1, true);
  i2c_read_blocking(i2c0, MAX_ADDR1, buf, 2, false);
  uint16_t result = ((uint16_t)buf[1]<<8) | (uint16_t)buf[0];
  return result;
}

uint16_t max_read_word_100(uint8_t addr)
{
  uint8_t buf[2];
  i2c_write_blocking(i2c0, MAX_ADDR2, &addr, 1, true);
  i2c_read_blocking(i2c0, MAX_ADDR2, buf, 2, false);
  uint16_t result = ((uint16_t)buf[1]<<8) | (uint16_t)buf[0];
  return result;
}

void max_read_buf(uint8_t addr, uint8_t size, uint8_t *buf)
{
  i2c_write_blocking(i2c0, MAX_ADDR1, &addr, 1, true);
  i2c_read_blocking(i2c0, MAX_ADDR1, buf, size, false);
}

void max_write_byte(uint8_t addr, uint8_t byte)
{
  uint8_t buf[2] = {addr, byte};
  i2c_write_blocking(i2c0, MAX_ADDR1, buf, 2, false);
}

void max_write_word(uint8_t addr, uint16_t word)
{
  uint8_t buf[3] = {addr, word&0xff, word>>8};
  int res = i2c_write_blocking(i2c0, MAX_ADDR1, buf, 3, false);
}

void max_write_word_100(uint8_t addr, uint16_t word)
{
  uint8_t buf[3] = {addr, word&0xff, word>>8};
  int res = i2c_write_blocking(i2c0, MAX_ADDR2, buf, 3, false);
}

void max_write_buf(uint8_t addr, uint8_t size, const uint8_t *buf)
{
  uint8_t txbuf[size + 1];
  txbuf[0] = addr;
  for (int i = 0; i < size; i++) {
    txbuf[i + 1] = buf[i];
  }
  i2c_write_blocking(i2c0, MAX_ADDR1, txbuf, size + 1, false);
}

float max_word_to_mv(uint16_t w)
{
  float result = ((float)w)*0.078125;
  return result;
}

float max_word_to_pack_mv(uint16_t w)
{
  float result = ((float)w)*0.3125;
  return result;
}

float max_word_to_ma(uint16_t w)
{
  float result = ((float)w)*0.3125;
  return result;
}

uint8_t mps_read_byte(uint8_t addr)
{
  uint8_t buf;
  i2c_write_blocking(i2c0, MPS_ADDR, &addr, 1, true);
  i2c_read_blocking(i2c0, MPS_ADDR, &buf, 1, false);
  return buf;
}

uint16_t mps_read_word(uint8_t addr)
{
  uint8_t buf[2];
  i2c_write_blocking(i2c0, MPS_ADDR, &addr, 1, true);
  i2c_read_blocking(i2c0, MPS_ADDR, buf, 2, false);
  uint16_t result = ((uint16_t)buf[1]<<8) | (uint16_t)buf[0];
  return result;
}

float mps_word_to_ntc(uint16_t w)
{
  float result = (float)(w&0xfff)*1.6/4096.0;
  return result;
}

float mps_word_to_3200(uint16_t w)
{
  float result = (w>>10)*100
    + ((w&(1<<9))>>9)*50
    + ((w&(1<<8))>>8)*25
    + ((w&(1<<7))>>7)*12.5
    + ((w&(1<<6))>>6)*6.25;
  return result;
}

float mps_word_to_6400(uint16_t w)
{
  float result = (w>>9)*100
    + ((w&(1<<8))>>8)*50
    + ((w&(1<<7))>>7)*25
    + ((w&(1<<6))>>6)*12.5;
  return result;
}

float mps_word_to_12800(uint16_t w)
{
  float result = (w>>8)*100
    + ((w&(1<<7))>>7)*50
    + ((w&(1<<6))>>6)*25;
  return result;
}

float mps_word_to_w(uint16_t w)
{
  float result = (w>>9)
    + ((w&(1<<8))>>8)*0.5
    + ((w&(1<<7))>>7)*0.25
    + ((w&(1<<6))>>6)*0.125;
  return result;
}

// tj=903-2.578*t
// tj-903=-2.578*t
// (tj-903)/-2.578=t

float mps_word_to_temp(uint16_t w)
{
  float result = (float)(w>>6);
  result = (result - 903) / -2.578;
  //result = 903-2.578*result;
  return result;
}

void mps_read_buf(uint8_t addr, uint8_t size, uint8_t *buf)
{
  i2c_write_blocking(i2c0, MPS_ADDR, &addr, 1, true);
  i2c_read_blocking(i2c0, MPS_ADDR, buf, size, false);
}

void mps_write_byte(uint8_t addr, uint8_t byte)
{
  uint8_t buf[2] = {addr, byte};
  i2c_write_blocking(i2c0, MPS_ADDR, buf, 2, false);
}

void mps_write_buf(uint8_t addr, uint8_t size, const uint8_t *buf)
{
  uint8_t txbuf[size + 1];
  txbuf[0] = addr;
  for (int i = 0; i < size; i++) {
    txbuf[i + 1] = buf[i];
  }
  i2c_write_blocking(i2c0, MPS_ADDR, txbuf, size + 1, false);
}

// https://git.clarahobbs.com/pd-buddy/pd-buddy-firmware/src/branch/master/lib/src/fusb302b.c

uint8_t fusb_read_byte(uint8_t addr)
{
  uint8_t buf;
  i2c_write_blocking(i2c0, FUSB_ADDR, &addr, 1, true);
  i2c_read_blocking(i2c0, FUSB_ADDR, &buf, 1, false);
  return buf;
}

void fusb_read_buf(uint8_t addr, uint8_t size, uint8_t *buf)
{
  i2c_write_blocking(i2c0, FUSB_ADDR, &addr, 1, true);
  i2c_read_blocking(i2c0, FUSB_ADDR, buf, size, false);
}

void fusb_write_byte(uint8_t addr, uint8_t byte)
{
  uint8_t buf[2] = {addr, byte};
  i2c_write_blocking(i2c0, FUSB_ADDR, buf, 2, false);
}

void fusb_write_buf(uint8_t addr, uint8_t size, const uint8_t *buf)
{
  uint8_t txbuf[size + 1];
  txbuf[0] = addr;
  for (int i = 0; i < size; i++) {
    txbuf[i + 1] = buf[i];
  }
  i2c_write_blocking(i2c0, FUSB_ADDR, txbuf, size + 1, false);
}

void fusb_send_message(const union pd_msg *msg)
{
  /* Token sequences for the FUSB302B */
  static uint8_t sop_seq[5] = {
    FUSB_FIFO_TX_SOP1,
    FUSB_FIFO_TX_SOP1,
    FUSB_FIFO_TX_SOP1,
    FUSB_FIFO_TX_SOP2,
    FUSB_FIFO_TX_PACKSYM
  };
  static uint8_t eop_seq[4] = {
    FUSB_FIFO_TX_JAM_CRC,
    FUSB_FIFO_TX_EOP,
    FUSB_FIFO_TX_TXOFF,
    FUSB_FIFO_TX_TXON
  };

  /* Get the length of the message: a two-octet header plus NUMOBJ four-octet
   * data objects */
  uint8_t msg_len = 2 + 4 * PD_NUMOBJ_GET(msg);

  /* Set the number of bytes to be transmitted in the packet */
  sop_seq[4] = FUSB_FIFO_TX_PACKSYM | msg_len;

  /* Write all three parts of the message to the TX FIFO */
  fusb_write_buf(FUSB_FIFOS, 5, sop_seq);
  fusb_write_buf(FUSB_FIFOS, msg_len, msg->bytes);
  fusb_write_buf(FUSB_FIFOS, 4, eop_seq);
}

uint8_t fusb_read_message(union pd_msg *msg)
{
  uint8_t garbage[4];
  uint8_t numobj;

  /* If this isn't an SOP message, return error.
   * Because of our configuration, we should be able to assume this means the
   * buffer is empty, and not try to read past a non-SOP message. */
  if ((fusb_read_byte(FUSB_FIFOS) & FUSB_FIFO_RX_TOKEN_BITS)
      != FUSB_FIFO_RX_SOP) {
    return 1;
  }
  /* Read the message header into msg */
  fusb_read_buf(FUSB_FIFOS, 2, msg->bytes);
  /* Get the number of data objects */
  numobj = PD_NUMOBJ_GET(msg);
  /* If there is at least one data object, read the data objects */
  if (numobj > 0) {
    fusb_read_buf(FUSB_FIFOS, numobj * 4, msg->bytes + 2);
  }
  /* Throw the CRC32 in the garbage, since the PHY already checked it. */
  fusb_read_buf(FUSB_FIFOS, 4, garbage);

  return 0;
}

// returns voltage
int print_src_fixed_pdo(uint32_t pdo) {
  int tmp;

  /* Dual-role power */
  tmp = (pdo & PD_PDO_SRC_FIXED_DUAL_ROLE_PWR) >> PD_PDO_SRC_FIXED_DUAL_ROLE_PWR_SHIFT;
  if (tmp) {
    printf("\tdual_role_pwr: %d\n", tmp);
  }

  /* USB Suspend Supported */
  tmp = (pdo & PD_PDO_SRC_FIXED_USB_SUSPEND) >> PD_PDO_SRC_FIXED_USB_SUSPEND_SHIFT;
  if (tmp) {
    printf("\tusb_suspend: %d\n", tmp);
  }

  /* Unconstrained Power */
  tmp = (pdo & PD_PDO_SRC_FIXED_UNCONSTRAINED) >> PD_PDO_SRC_FIXED_UNCONSTRAINED_SHIFT;
  if (tmp) {
    printf("\tunconstrained_pwr: %d\n", tmp);
  }

  /* USB Communications Capable */
  tmp = (pdo & PD_PDO_SRC_FIXED_USB_COMMS) >> PD_PDO_SRC_FIXED_USB_COMMS_SHIFT;
  if (tmp) {
    printf("\tusb_comms: %d\n", tmp);
  }

  /* Dual-Role Data */
  tmp = (pdo & PD_PDO_SRC_FIXED_DUAL_ROLE_DATA) >> PD_PDO_SRC_FIXED_DUAL_ROLE_DATA_SHIFT;
  if (tmp) {
    printf("\tdual_role_data: %d\n", tmp);
  }

  /* Unchunked Extended Messages Supported */
  tmp = (pdo & PD_PDO_SRC_FIXED_UNCHUNKED_EXT_MSG) >> PD_PDO_SRC_FIXED_UNCHUNKED_EXT_MSG_SHIFT;
  if (tmp) {
    printf("\tunchunked_ext_msg: %d\n", tmp);
  }

  /* Peak Current */
  tmp = (pdo & PD_PDO_SRC_FIXED_PEAK_CURRENT) >> PD_PDO_SRC_FIXED_PEAK_CURRENT_SHIFT;
  if (tmp) {
    printf("\tpeak_i: %d\n", tmp);
  }

  /* Voltage */
  tmp = (pdo & PD_PDO_SRC_FIXED_VOLTAGE) >> PD_PDO_SRC_FIXED_VOLTAGE_SHIFT;
  printf("\tv: %d.%02d V\n", PD_PDV_V(tmp), PD_PDV_CV(tmp));

  int voltage = (int)PD_PDV_V(tmp);

  /* Maximum Current */
  tmp = (pdo & PD_PDO_SRC_FIXED_CURRENT) >> PD_PDO_SRC_FIXED_CURRENT_SHIFT;
  printf("\ti: %d.%02d A\n", PD_PDI_A(tmp), PD_PDI_CA(tmp));

  return voltage;
}

int charger_dump() {
  uint8_t status = mps_read_byte(0x13);
  uint8_t fault = mps_read_byte(0x14);

  float adc_bat_v = mps_word_to_6400(mps_read_word(0x16))/1000.0;
  float adc_sys_v = mps_word_to_6400(mps_read_word(0x18))/1000.0;
  float adc_charge_c = mps_word_to_6400(mps_read_word(0x1a))/1000.0;
  float adc_input_v = mps_word_to_12800(mps_read_word(0x1c))/1000.0;
  float adc_input_c = mps_word_to_3200(mps_read_word(0x1e))/1000.0;
  float adc_temp = mps_word_to_temp(mps_read_word(0x24));
  float adc_sys_pwr = mps_word_to_w(mps_read_word(0x26));
  float adc_discharge_c = mps_word_to_6400(mps_read_word(0x28))/1000.0;
  float adc_ntc_v = mps_word_to_ntc(mps_read_word(0x40))/1000.0;

  uint8_t input_c_limit = mps_read_byte(0x00);
  uint8_t input_v_limit = mps_read_byte(0x01);
  uint8_t charge_c = mps_read_byte(0x02);
  uint8_t precharge_c = mps_read_byte(0x03);
  uint8_t bat_full_v = mps_read_byte(0x04);

  printf("[charger info]\n");
  printf("  status: %x\n", status);
  printf("  fault: %x\n  ------------\n", fault);

  printf("  adc_bat_v: %f\n", adc_bat_v);
  printf("  adc_sys_v: %f\n", adc_sys_v);
  printf("  adc_charge_c: %f\n", adc_charge_c);
  printf("  adc_input_v: %f\n", adc_input_v);
  printf("  adc_input_c: %f\n", adc_input_c);
  printf("  adc_temp: %f\n", adc_temp);
  printf("  adc_sys_pwr: %f\n", adc_sys_pwr);
  printf("  adc_discharge_c: %f\n", adc_discharge_c);
  printf("  adc_ntc_v: %f\n  ------------\n", adc_ntc_v);

  printf("  input_c_limit: %d\n", input_c_limit);
  printf("  input_v_limit: %d\n", input_v_limit);
  printf("  charge_c: %d\n", charge_c);
  printf("  precharge_c: %d\n", precharge_c);
  printf("  bat_full_v: %d\n  ============\n", bat_full_v);

  if (adc_input_v < 11) {
    // renegotiate PD
    return 1;
  }
  return 0;
}

void max_dump() {
  // disable write protection (CommStat)
  max_write_word(0x61, 0x0000);
  max_write_word(0x61, 0x0000);
  // fixme set thermistor config
  max_write_word_100(0xca, 0x58ef);
  // set pack cfg: 2 cells (0), 1 thermistor, 6v charge pump (?), no aoldo, btpken off (??)
  max_write_word_100(0xb5, (1<11)|(0<<8)|(0<<2)|0);

  uint16_t comm_stat = max_read_word(0x61);
  uint16_t status = max_read_word(0x00);
  uint16_t prot_status = max_read_word(0xd9);
  uint16_t prot_alert = max_read_word(0xaf);
  uint16_t prot_cfg2 = max_read_word_100(0xf1);
  uint16_t therm_cfg = max_read_word_100(0xca);
  float vcell = max_word_to_mv(max_read_word(0x1a));
  float avg_vcell = max_word_to_mv(max_read_word(0x19));
  float cell1 = max_word_to_mv(max_read_word(0xd8));
  float cell2 = max_word_to_mv(max_read_word(0xd7));
  float cell3 = max_word_to_mv(max_read_word(0xd6));
  float cell4 = max_word_to_mv(max_read_word(0xd5));
  float vpack = max_word_to_pack_mv(max_read_word(0xda));

  float temp = ((float)((int16_t)max_read_word(0x1b)))*(1.0/256.0);
  float die_temp = ((float)((int16_t)max_read_word(0x34)))*(1.0/256.0);
  float temp1 = ((float)((int16_t)max_read_word_100(0x3a)))*(1.0/256.0);
  float temp2 = ((float)((int16_t)max_read_word_100(0x39)))*(1.0/256.0);
  float temp3 = ((float)((int16_t)max_read_word_100(0x38)))*(1.0/256.0);
  float temp4 = ((float)((int16_t)max_read_word_100(0x37)))*(1.0/256.0);

  printf("[pack info]\n");

  printf("  comm_stat: %04x\n", comm_stat);

  printf("  status: %04x\n", status);
  if (status & 0x8000) {
    printf("  `-- prot alert\n");
  }
  if (status & 0x0002) {
    printf("  `-- POR, clearing\n");
    max_write_word(0x61, 0x0000);
    max_write_word(0x61, 0x0000);
    max_write_word(0x00, status & (~0x0002));
  }
  printf("  prot_alert: %04x\n", prot_alert);
  printf("  prot_cfg2: %04x\n", prot_cfg2);
  printf("  therm_cfg: %04x\n", therm_cfg);
  printf("  temp: %f\n", temp);
  printf("  die temp: %f\n", die_temp);
  printf("  temp1: %f\n", temp1);
  printf("  temp2: %f\n", temp2);
  printf("  temp3: %f\n", temp3);
  printf("  temp4: %f\n", temp4);

  printf("  prot_status: %04x\n", prot_status);
  if (prot_status & (1<<14)) {
    printf("  `-- too hot\n");
  }
  if (prot_status & (1<<13)) {
    printf("  `-- full\n");
  }
  if (prot_status & (1<<12)) {
    printf("  `-- too cold for charge\n");
  }
  if (prot_status & (1<<11)) {
    printf("  `-- overvoltage\n");
  }
  if (prot_status & (1<<10)) {
    printf("  `-- overcharge current\n");
  }
  if (prot_status & (1<<9)) {
    printf("  `-- qoverflow\n");
  }
  if (prot_status & (1<<8)) {
    printf("  `-- prequal timeout\n");
  }
  if (prot_status & (1<<7)) {
    printf("  `-- imbalance\n");
  }
  if (prot_status & (1<<6)) {
    printf("  `-- perm fail\n");
  }
  if (prot_status & (1<<5)) {
    printf("  `-- die hot\n");
  }
  if (prot_status & (1<<4)) {
    printf("  `-- too hot for discharge\n");
  }
  if (prot_status & (1<<3)) {
    printf("  `-- undervoltage\n");
  }
  if (prot_status & (1<<2)) {
    printf("  `-- overdischarge current\n");
  }
  if (prot_status & (1<<1)) {
    printf("  `-- resdfault\n");
  }
  if (prot_status & (1<<0)) {
    printf("  `-- ship\n");
  }

  printf("  vcell: %f\n", vcell);
  printf("  avg_vcell: %f\n", avg_vcell);
  printf("  cell1: %f\n", cell1);
  printf("  cell2: %f\n", cell2);
  printf("  cell3: %f\n", cell3);
  printf("  cell4: %f\n", cell4);
  printf("  vpack: %f\n  ============\n", vpack);
}

void turn_som_power_on() {
  gpio_put(PIN_LED_B, 1);

  printf("[turn_som_power_on]\n");
  gpio_put(PIN_1V1_ENABLE, 1);
  sleep_ms(10);
  gpio_put(PIN_3V3_ENABLE, 1);
  sleep_ms(10);
  gpio_put(PIN_5V_ENABLE, 1);

  sleep_ms(10);
  gpio_put(PIN_DISP_EN, 1);
  sleep_ms(10);
  gpio_put(PIN_DISP_RESET, 1); // not connected?
}

void turn_som_power_off() {
  gpio_put(PIN_LED_B, 0);

  printf("[turn_som_power_off]\n");
  gpio_put(PIN_DISP_RESET, 0);
  gpio_put(PIN_DISP_EN, 0);

  gpio_put(PIN_5V_ENABLE, 0);
  sleep_ms(10);
  gpio_put(PIN_3V3_ENABLE, 0);
  sleep_ms(10);
  gpio_put(PIN_1V1_ENABLE, 0);
}

#define I2C_TIMEOUT (1000*500)

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE


#define ST_EXPECT_DIGIT_0 0
#define ST_EXPECT_DIGIT_1 1
#define ST_EXPECT_DIGIT_2 2
#define ST_EXPECT_DIGIT_3 3
#define ST_EXPECT_CMD     4
#define ST_SYNTAX_ERROR   5
#define ST_EXPECT_RETURN  6

char remote_cmd = 0;
uint8_t remote_arg = 0;
unsigned char cmd_state = ST_EXPECT_DIGIT_0;
unsigned int cmd_number = 0;
int cmd_echo = 0;
char uart_buffer[255] = {0};

// chr: input character
void handle_commands(char chr) {
  printf("rx: [%c]\n", chr);

  if (cmd_echo) {
    sprintf(uart_buffer, "%c", chr);
    uart_puts(UART_ID, uart_buffer);
  }

  // states:
  // 0-3 digits of optional command argument
  // 4   command letter expected
  // 5   syntax error (unexpected character)
  // 6   command letter entered

  if (cmd_state>=ST_EXPECT_DIGIT_0 && cmd_state<=ST_EXPECT_DIGIT_3) {
    // read number or command
    if (chr >= '0' && chr <= '9') {
      cmd_number*=10;
      cmd_number+=(chr-'0');
      cmd_state++;
    } else if ((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z')) {
      // command entered instead of digit
      remote_cmd = chr;
      cmd_state = ST_EXPECT_RETURN;
    } else if (chr == '\n' || chr == ' ') {
      // ignore newlines or spaces
    } else if (chr == '\r') {
      sprintf(uart_buffer, "error:syntax\r\n");
      uart_puts(UART_ID, uart_buffer);
      cmd_state = ST_EXPECT_DIGIT_0;
      cmd_number = 0;
    } else {
      // syntax error
      cmd_state = ST_SYNTAX_ERROR;
    }
  }
  else if (cmd_state == ST_EXPECT_CMD) {
    // read command
    if ((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z')) {
      remote_cmd = chr;
      cmd_state = ST_EXPECT_RETURN;
    } else {
      cmd_state = ST_SYNTAX_ERROR;
    }
  }
  else if (cmd_state == ST_SYNTAX_ERROR) {
    // syntax error
    if (chr == '\r') {
      sprintf(uart_buffer, "error:syntax\r\n");
      uart_puts(UART_ID, uart_buffer);
      cmd_state = ST_EXPECT_DIGIT_0;
      cmd_number = 0;
    }
  }
  else if (cmd_state == ST_EXPECT_RETURN) {
    if (chr == '\n' || chr == ' ') {
      // ignore newlines or spaces
    }
    else if (chr == '\r') {
      if (cmd_echo) {
        // FIXME
        sprintf(uart_buffer,"\n");
        uart_puts(UART_ID, uart_buffer);
      }

      // execute
      if (remote_cmd == 'p') {
        // toggle system power and/or reset imx
        if (cmd_number == 0) {
          turn_som_power_off();
          sprintf(uart_buffer,"system: off\r\n");
          uart_puts(UART_ID, uart_buffer);
        } else if (cmd_number == 2) {
          //reset_som();
          sprintf(uart_buffer,"system: reset\r\n");
          uart_puts(UART_ID, uart_buffer);
        } else {
          turn_som_power_on();
          sprintf(uart_buffer,"system: on\r\n");
          uart_puts(UART_ID, uart_buffer);
        }
      }
      else if (remote_cmd == 'a') {
        // TODO
        // get system current (mA)
        sprintf(uart_buffer,"%d\r\n",0);
        uart_puts(UART_ID, uart_buffer);
      }
      else if (remote_cmd == 'v' && cmd_number>=0 && cmd_number<=0) {
        // TODO
        // get cell voltage
        sprintf(uart_buffer,"%d\r\n",0);
        uart_puts(UART_ID, uart_buffer);
      }
      else if (remote_cmd == 'V') {
        // TODO
        // get system voltage
        sprintf(uart_buffer,"%d\r\n",0);
        uart_puts(UART_ID, uart_buffer);
      }
      else if (remote_cmd == 's') {
        // TODO
        sprintf(uart_buffer,FW_REV"normal,%d,%d,%d\r",0,0,0);
        uart_puts(UART_ID, uart_buffer);
      }
      else if (remote_cmd == 'u') {
        // TODO
        // turn reporting to i.MX on or off
      }
      else if (remote_cmd == 'w') {
        // TODO
        // wake SoC
      }
      else if (remote_cmd == 'c') {
        // TODO
        // get status of cells, current, voltage, fuel gauge
        sprintf(uart_buffer, "%d\r\n", 0);
        uart_puts(UART_ID, uart_buffer);
      }
      else if (remote_cmd == 'S') {
        // TODO
        // get charger system cycles in current state
        sprintf(uart_buffer, "%d\r\n", 0);
        uart_puts(UART_ID, uart_buffer);
      }
      else if (remote_cmd == 'C') {
        // TODO
        // get battery capacity (mAh)
        sprintf(uart_buffer,"%d/%d/%d\r\n",0,0,0);
        uart_puts(UART_ID, uart_buffer);
      }
      else if (remote_cmd == 'e') {
        // toggle serial echo
        cmd_echo = cmd_number?1:0;
      }
      else {
        sprintf(uart_buffer, "error:command\r\n");
        uart_puts(UART_ID, uart_buffer);
      }

      cmd_state = ST_EXPECT_DIGIT_0;
      cmd_number = 0;
    } else {
      cmd_state = ST_SYNTAX_ERROR;
    }
  }
}

void on_uart_rx() {
  while (uart_is_readable(UART_ID)) {
    handle_commands(uart_getc(UART_ID));
  }
}

int main() {
  stdio_init_all();

  //sleep_ms(2000);
  // UART to keyboard
  uart_init(UART_ID, BAUD_RATE);
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
  uart_set_hw_flow(UART_ID, false, false);
  uart_set_fifo_enabled(UART_ID, true);
  gpio_set_function(PIN_KBD_UART_TX, GPIO_FUNC_UART);
  gpio_set_function(PIN_KBD_UART_RX, GPIO_FUNC_UART);
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
  //irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  //irq_set_enabled(UART_IRQ, true);
  //uart_set_irq_enables(UART_ID, true, false); // bool rx_has_data, bool tx_needs_data

  printf("uart setup\n");

  gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
  bi_decl(bi_2pins_with_func(PIN_SDA, PIN_SCL, GPIO_FUNC_I2C));
  i2c_init(i2c0, 100 * 1000);

  gpio_init(PIN_LED_R);
  gpio_init(PIN_LED_G);
  gpio_init(PIN_LED_B);
  gpio_set_dir(PIN_LED_R, 1);
  gpio_set_dir(PIN_LED_G, 1);
  gpio_set_dir(PIN_LED_B, 1);

  gpio_init(PIN_1V1_ENABLE);
  gpio_init(PIN_3V3_ENABLE);
  gpio_init(PIN_5V_ENABLE);
  gpio_set_dir(PIN_1V1_ENABLE, 1);
  gpio_set_dir(PIN_3V3_ENABLE, 1);
  gpio_set_dir(PIN_5V_ENABLE, 1);
  gpio_put(PIN_1V1_ENABLE, 0);
  gpio_put(PIN_3V3_ENABLE, 0);
  gpio_put(PIN_5V_ENABLE, 0);

  gpio_init(PIN_DISP_RESET);
  gpio_init(PIN_DISP_EN);
  gpio_set_dir(PIN_DISP_EN, 1);
  gpio_set_dir(PIN_DISP_RESET, 1);
  gpio_put(PIN_DISP_EN, 0);
  gpio_put(PIN_DISP_RESET, 0);

  gpio_put(PIN_LED_R, 0);
  gpio_put(PIN_LED_G, 0);
  gpio_put(PIN_LED_B, 0);

  unsigned int t = 0;

  int state = 0;
  int request_sent = 0;
  uint8_t rxdata[2];

  union pd_msg tx;
  int tx_id_count = 0;
  union pd_msg rx_msg;

  int power_objects = 0;

  // https://www.reclaimerlabs.com/blog/2017/2/1/usb-c-for-engineers-part-3

  printf("main loop\n");

  while (true) {
    sleep_ms(1);

    while (uart_is_readable(UART_ID)) {
      handle_commands(uart_getc(UART_ID));
    }

    if (state == 0) {
      gpio_put(PIN_LED_R, 0);
      power_objects = 0;

      //printf("[pocket-sysctl] state 0\n");
      // probe FUSB302BMPX
      if (i2c_read_timeout_us(i2c0, FUSB_ADDR, rxdata, 1, false, I2C_TIMEOUT)) {
        // 1. set auto GoodCRC
        // AUTO_CRC in Switches1
        // Address: 03h; Reset Value: 0b0010_0000
        // TODO: figure out CC direction?

        printf("[pocket-sysctl] FUSB probed.\n");

        fusb_write_byte(FUSB_RESET, FUSB_RESET_SW_RES);
        // turn on all power
        fusb_write_byte(FUSB_POWER, 0x0F);
        fusb_write_byte(FUSB_CONTROL3, 0x07);
        fusb_write_byte(FUSB_CONTROL1, FUSB_CONTROL1_RX_FLUSH);

        /* Measure CC1 */
        fusb_write_byte(FUSB_SWITCHES0, 0x07);
        sleep_us(250);
        uint8_t cc1 = fusb_read_byte(FUSB_STATUS0) & FUSB_STATUS0_BC_LVL;

        printf("[pocket-sysctl] CC1: %d\n", cc1);

        /* Measure CC2 */
        fusb_write_byte(FUSB_SWITCHES0, 0x0B);
        sleep_us(250);
        uint8_t cc2 = fusb_read_byte(FUSB_STATUS0) & FUSB_STATUS0_BC_LVL;

        printf("[pocket-sysctl] CC2: %d\n", cc2);

        if (cc1 > cc2) {
          fusb_write_byte(FUSB_SWITCHES1, 0x25);
          fusb_write_byte(FUSB_SWITCHES0, 0x07);
        } else {
          fusb_write_byte(FUSB_SWITCHES1, 0x26);
          fusb_write_byte(FUSB_SWITCHES0, 0x0B);
        }

        printf("[pocket-sysctl] switches set\n");

        //fusb_write_byte(FUSB_RESET, FUSB_RESET_PD_RESET);
        t = 0;
        state = 1;
      } else {
        if (t > 1000) {
          printf("[pocket-sysctl] state 0: fusb timeout\n");
          t = 0;
        }
      }

    } else if (state == 1) {
      //printf("[pocket-sysctl] state 2\n");

      if (t>2000) {
        printf("[pocket-sysctl] state 2, timeout\n");

        // issue hard reset
        fusb_write_byte(FUSB_CONTROL3, (1<<6) | 0x07);
        sleep_ms(1);
        fusb_write_byte(FUSB_CONTROL3, 0x07);

        request_sent = 0;
        state = 0;
        t = 0;
      }

      int res = fusb_read_message(&rx_msg);

      if (!res) {
        uint8_t msgtype = PD_MSGTYPE_GET(&rx_msg);
        uint8_t numobj = PD_NUMOBJ_GET(&rx_msg);
        printf("  msg type: %x numobj: %d\n", msgtype, numobj);
        if (msgtype == PD_MSGTYPE_SOURCE_CAPABILITIES) {
          int max_voltage = 0;
          for (int i=0; i<numobj; i++) {
            uint32_t pdo = rx_msg.obj[i];
            if ((pdo & PD_PDO_TYPE) == PD_PDO_TYPE_FIXED) {
              int voltage = print_src_fixed_pdo(pdo);
              if (voltage > max_voltage && voltage <= 20) {
                power_objects = i+1;
                max_voltage = voltage;
              }
            } else {
              printf("not a fixed PDO: %08x\n", pdo);
            }
          }
          if (!request_sent) {
            state = 2;
            t = 0;
          }
        } else if (msgtype == PD_MSGTYPE_PS_RDY) {
          // power supply is ready
          printf("[pocket-sysctl] power supply ready!\n");
          request_sent = 0;
          t = 0;
          state = 3;
        }
      }
    } else if (state == 2) {
      printf("[pocket-sysctl] state 2, requesting PO %d\n", power_objects);

      tx.hdr = PD_MSGTYPE_REQUEST | PD_NUMOBJ(1) | PD_DATAROLE_UFP | PD_POWERROLE_SINK | PD_SPECREV_2_0;

      tx.hdr &= ~PD_HDR_MESSAGEID;
      tx.hdr |= (tx_id_count % 8) << PD_HDR_MESSAGEID_SHIFT;

      int current = 1;

      tx.obj[0] = PD_RDO_FV_MAX_CURRENT_SET(current)
        | PD_RDO_FV_CURRENT_SET(current)
        | PD_RDO_NO_USB_SUSPEND | PD_RDO_OBJPOS_SET(power_objects); // FIXME

      fusb_send_message(&tx);

      printf("[pocket-sysctl] request sent.\n");

      tx_id_count++;

      request_sent = 1;
      state = 1;
    } else if (state == 3) {
      gpio_put(PIN_LED_R, 1);

      // running
      if (t>2000) {
        printf("[pocket-sysctl] state 3\n");
        //i2c_scan();

        int renegotiate = charger_dump();
        max_dump();

        if (renegotiate) {
          state = 0;
        }

        t = 0;
      }
    }

    t++;
  }

  return 0;
}
