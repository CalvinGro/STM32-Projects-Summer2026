/*
Author - Calvin Gross
Date - 5/17/26
Title - 6050_logic.c
Project - Modeling Data from a MPU6050; Project #1
Description - This declares 2 functions. One to initalize the MPU6050 which is the IMU I am using.
			  The other to take
*/

#include "6050_logic.h"
#include "i2c.h"
#include <string.h>





// pwr on and check who_am_i,
HAL_StatusTypeDef MPU6050_init(void){

	uint8_t start = 0x01;
	uint8_t who_am_i = 0x00;

	HAL_StatusTypeDef status1, status2, status3;
	status1 = HAL_I2C_IsDeviceReady(&hi2c1, MPU6050_ADR, 3, 100);
	if (status1 != HAL_OK) return status1;

	status2 = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADR, MPU6050_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &start, 1, 100);
	if (status2 != HAL_OK) return status2;

	status3 = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADR, MPU6050_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &who_am_i, 1, 100);
	if (status3 != HAL_OK) return status3;
	if (who_am_i == 0x0) {
		return HAL_ERROR;
	}

	// setting config bit for accel enabling a -4g to 4g range.
	uint8_t config_8g_range;
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADR, MPU6050_ACCEL_CONFIG, I2C_MEMADD_SIZE_8BIT, &config_8g_range, 1, HAL_MAX_DELAY);
	config_8g_range = (config_8g_range & 0xEF) | 0x08;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADR, MPU6050_ACCEL_CONFIG, I2C_MEMADD_SIZE_8BIT, &config_8g_range, 1, HAL_MAX_DELAY);

	return HAL_OK;
}


// returns a single sample from the MPU6050 and the status of the i2c read
HAL_StatusTypeDef MPU6050_get_sample(MPU6050Sample *data) {
	  uint8_t buf[14];

	  HAL_StatusTypeDef status;
	  status = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADR, MPU6050_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, buf, 14, 100);

	  if (status != HAL_OK) {
		  memset(data, 0, sizeof(*data));
		  return status;
	  }
	  data->ax = (int16_t)(buf[0] << 8 | buf[1]);
	  data->ay = (int16_t)(buf[2] << 8 | buf[3]);
	  data->az = (int16_t)(buf[4] << 8 | buf[5]);

	  data->temp = (int16_t)(buf[6] << 8 | buf[7]);

	  data->gx = (int16_t)(buf[8] << 8 | buf[9]);
	  data->gy = (int16_t)(buf[10] << 8 | buf[11]);
	  data->gz = (int16_t)(buf[12] << 8 | buf[13]);

	  return HAL_OK;
}







