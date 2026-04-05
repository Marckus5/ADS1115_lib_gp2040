#include <cstdint>
#include <ratio>
#include <stdio.h>
#include "peripheral_i2c.h"
#include "pico/stdlib.h"
#include <ADS1115.h>
#include <sys/unistd.h>
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_BLOCK 0
#define I2C_SDA 0
#define I2C_SCL 1
#define BAUD_RATE 400000



int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    PeripheralI2C i2c;
    i2c.setConfig(I2C_BLOCK, I2C_SDA, I2C_SCL, BAUD_RATE);

    ADS1115 ads(&i2c);
    
    while (true) {
    printf("Try:\n");
    
    ads.resetConfig();
    uint16_t config = ads.readRegister16(ads1115AddressRegister_t::CONFIG);
    printf("Config: %04x\n", config);

    ads.setChannel(2);                         // Start on Channel 2
    ads.setConversionMode(ads1115Mode_t::CONTINUOUS);         // Read analog continuously
    ads.setGain(ads1115Gain_t::ONE);                          // Set gain to 1
    ads.setDataRate(32);                     // 1mhz (1.1ms delay)
                                              
    sleep_ms(2000);
    gpio_put(25, 1);

    // reread config
    config = ads.readRegister16(ads1115AddressRegister_t::CONFIG);
    printf("Config: %04x\n", config);
    
    printf("Data: %d \n", ads.readConversionResult());
    
    sleep_ms(2000);
    gpio_put(25, 0);
    }

    return 0;
    


}
