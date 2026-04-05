#include "hardware/i2c.h"
#include "peripheral_i2c.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
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

int main() {
  stdio_init_all();

  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  gpio_put(25, 1);

  PeripheralI2C i2c;
  i2c.setConfig(I2C_BLOCK, I2C_SDA, I2C_SCL, BAUD_RATE);

  ADS1115 ads(&i2c);

  printf("Try:\n");
  // reread config
  ads.setChannel(2);                                // Start on Channel 2
  ads.setConversionMode(ads1115Mode_t::CONTINUOUS); // Read analog continuously
  ads.setGain(ads1115Gain_t::ONE);                  // Set gain to 1
  ads.setDataRate(860);
  uint16_t config = ads.readRegister16(ads1115AddressRegister_t::CONFIG);
  config = ads.readRegister16(ads1115AddressRegister_t::CONFIG);
  printf("Config: %04x\n", config);
  int time = 0;
  // Will run for 50 seconds before rebooting to usb
  while (time <= 50000) {

    // uint16_t config = ads.readRegister16(ads1115AddressRegister_t::CONFIG);
    // printf("Config: %04x\n", config);

    gpio_put(25, 1);
    for (int i = 0; i < 4; i++) {
      printf("Data %d: %d \n", i, (int16_t)ads.readSingleEnded(i));
      sleep_ms(10);
    }
    printf("Time %d\n",time);
    sleep_ms(200);
    gpio_put(25, 0);
    time = to_ms_since_boot(get_absolute_time());
  }
  reset_usb_boot(0,0);

  return 0;
}
