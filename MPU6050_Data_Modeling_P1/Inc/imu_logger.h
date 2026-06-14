/*
Author - Calvin Gross
Date - 5/17/26
Modified - 5/18/26
Title - imu_logger.h
Project - Modeling Data from a MPU6050; Project #1
Description - Header file for imu__logger.c.
*/


#ifndef IMU_LOGGER_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "usart.h"

void uart_print(char *msg);

typedef struct {
	uint32_t time;
	int16_t ax, ay, az;
	int16_t gx, gy, gz;
	int16_t temp;
} FullIMUSample;

HAL_StatusTypeDef logger_init(void);
HAL_StatusTypeDef write_to_buf(FullIMUSample *sample);
bool buf_full(void);
HAL_StatusTypeDef clear_buf(void);
bool flash_empty(void);
HAL_StatusTypeDef clear_flash_sector_7(void);
HAL_StatusTypeDef write_bytes_to_flash(const uint8_t *data, uint32_t length, uint32_t flash_addr);
HAL_StatusTypeDef write_buf_to_flash(void);
HAL_StatusTypeDef dump_flash_uart(void);

#endif
