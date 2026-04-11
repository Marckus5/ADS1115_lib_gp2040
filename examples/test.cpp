#include "hardware/i2c.h"
#include "peripheral_i2c.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include <ADS1115.h>
#include <chrono>
#include <cstdint>
#include <pico/runtime_init.h>
#include <pico/time.h>
#include <ratio>
#include <stdio.h>
#include <sys/unistd.h>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for
// information on GPIO assignments
#define I2C_BLOCK 0
#define I2C_SDA 0
#define I2C_SCL 1
#define BAUD_RATE 400000
#define ADS1115_3_3V_REMAP_FACTOR 2.482462122f

int main() {
  stdio_init_all();
  sleep_ms(1000);

  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  gpio_put(25, 1);

  PeripheralI2C i2c;
  i2c.setConfig(I2C_BLOCK, I2C_SDA, I2C_SCL, BAUD_RATE);

  ADS1115 ads(&i2c);

  printf("Try Configuration:\n");
  uint16_t config = ads.readConfigRegister();
  sleep_ms(1000);
  printf("Config: %04x\n\n", config);
  sleep_ms(1000);

  int channel = 0;
  ads.setChannel(channel);                          // Start on Channel 2
  ads.setConversionMode(ads1115Mode_t::CONTINUOUS); // Read analog continuously
  ads.setGain(ads1115Gain_t::ONE);                  // Set gain to 1
  ads.setDataRate(860);

  config = ads.readConfigRegister();
  printf("Config: %04hx\n\n", config);
  int time = 0;
  // Will run for 50 seconds before rebooting to usb

  uint16_t data[4];
  float result[4];
  sleep_ms(100);
  while (time <= 10000) {

    sleep_ms(500);
    // uint16_t config = ads.readRegister16(ads1115AddressRegister_t::CONFIG);
    // printf("Config: %04x\n", config);

    data[channel] = ads.readConversionResult();
    // result[channel] = (int16_t)data[channel] * ADS1115_3_3V_REMAP_FACTOR;
    result[channel] = (int16_t)data[channel] * ADS1115_3_3V_REMAP_FACTOR;
    result[channel] = (result[channel] > 0xFFFF) ? 0xFFFF : result[channel];

    channel++;
    channel %= 4;
    ads.setChannel(channel);
    // sleep_ms(10);

    printf("Time %d\n", time);
    for (int i = 0; i < 4; i++) {
    // ads.setChannel(i);
    // sleep_ms(10);
    // int data = ads.readConversionResult();
    // printf("Data %d: %d \n", i, data);
    // printf("Data %d: %d\n", i, ads.readSingleEnded(i));
    printf("Channel %d: %u | %f | %u\n", i,data[i],result[i],(uint16_t)result[i]);
    }

    time = to_ms_since_boot(get_absolute_time());
  }
  reset_usb_boot(0, 0);

  return 0;
}
