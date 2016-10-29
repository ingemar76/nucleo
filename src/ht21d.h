#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"

typedef enum {
  HT21D_OK = 0,
  HT21D_ERR_I2C,
  HT21D_ERR_CRC
} HT21D_STATUS;

#define HTU21D_ADDR                  0x40
#define HTU21D_ADDR_READ             ((HTU21D_ADDR << 1) | 0x01)
#define HTU21D_ADDR_WRITE            (HTU21D_ADDR << 1)
#define TRIGGER_TEMP_MEASURE_HOLD    0xE3
#define TRIGGER_HUMD_MEASURE_HOLD    0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD  0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD  0xF5
#define WRITE_USER_REG               0xE6
#define READ_USER_REG                0xE7
#define SOFT_RESET                   0xFE

HT21D_STATUS ht21d_read_temp(I2C_HandleTypeDef *hi2c, float *temperature);
