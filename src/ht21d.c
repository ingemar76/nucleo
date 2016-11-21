#include "ht21d.h"

#define SHIFTED_DIVISOR              0x988000

float conv_temp(uint16_t rawTemp) {
  //Zero out the status bits but keep them in place
  rawTemp &= 0xFFFC;
  //Given the raw temperature data, calculate the actual temperature
  float tempTemperature = rawTemp / (float) 65536;
  //2^16 = 65536float realTemperature = -46.85 + (175.72 * tempTemperature);
  //From page 14*temp = realTemperature;
  float realTemperature = -46.85 + (175.72 * tempTemperature);

  return realTemperature;
}

float conv_humid(uint16_t rawHumidity) {
    //sensorStatus = rawHumidity & 0x0003; //Grab only the right two bits
    rawHumidity &= 0xFFFC; //Zero out the status bits but keep them in place
    //Given the raw humidity data, calculate the actual relative humidity
    float tempRH = rawHumidity / (float) 65536;
    //2^16 = 65536float rh = -6 + (125 * tempRH);
    float rh = -6 + (125 * tempRH); //From page 14
    return rh;
}


uint8_t check_crc(uint16_t value, uint8_t crc) {
  uint32_t remainder = value; //Pad with 8 bits because we have to add in the check value
  remainder = remainder << 8;
  remainder |= crc; //Add on the check value

  uint32_t divsor = (unsigned long) SHIFTED_DIVISOR;

  for (int i = 0; i < 16; i++) { //Operate on only 16 positions of max 24. The remaining 8 are our remainder and should be zero when we're done.
    if (remainder & (uint32_t) 1 << (23 - i)) //Check if there is a one in the left position
      remainder ^= divsor;

    //Rotate the divsor max 16 times so that we have 8 bits left of a remainder
    divsor >>= 1;
  }

  if (remainder != 0) {
    return 0;
  } else {
    return 1;
  }
}

HT21D_STATUS ht21d_read_data(I2C_HandleTypeDef *hi2c, uint8_t command, uint16_t *raw_data) {
  uint8_t i2c_buf[3];
  i2c_buf[0] = command;

  if (HAL_I2C_Master_Transmit(hi2c, HTU21D_ADDR_WRITE, i2c_buf, 1, 2000)
      != HAL_OK) {
    return HT21D_STATUS_ERR_I2C;
  }

  HAL_Delay(55);

  if (HAL_I2C_Master_Receive(hi2c, HTU21D_ADDR_READ, i2c_buf, 3, 2000)
      != HAL_OK) {
    return HT21D_STATUS_ERR_I2C;
  }

  *raw_data = i2c_buf[0] << 8 | i2c_buf[1];

  if (!check_crc(*raw_data, i2c_buf[2])) {
    return HT21D_STATUS_ERR_CRC;
  }

  return HT21D4_STATUS_OK;
}



HT21D_STATUS ht21d_read_temp(I2C_HandleTypeDef *hi2c, float *temperature) {
  uint16_t raw;

  HT21D_STATUS res = ht21d_read_data(hi2c, TRIGGER_TEMP_MEASURE_NOHOLD,&raw);

  if (res != HT21D4_STATUS_OK) {
    return res;
  }

  *temperature = conv_temp(raw);

  return res;
}

HT21D_STATUS ht21d_read_humidity(I2C_HandleTypeDef *hi2c, float *humidity) {
  uint16_t raw;

  HT21D_STATUS res = ht21d_read_data(hi2c, TRIGGER_HUMD_MEASURE_NOHOLD,&raw);

  if (res != HT21D4_STATUS_OK) {
    return res;
  }

  *humidity = conv_humid(raw);

  return res;
}
