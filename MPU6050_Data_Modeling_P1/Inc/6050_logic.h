/*
Author - Calvin Gross
Date - 5/17/26
Title - 6050_logic.h
Project - Modeling Data from a MPU6050; Project #1
Description - Header file for 6050_logic.c.
*/

#ifndef MPU6050_LOGIC_H
#define MPU6050_LOGIC_H

#include "main.h"


#define MPU6050_ADR           (0x68 << 1)

#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_WHO_AM_I      0x75
#define MPU6050_ACCEL_XOUT_H  0x3B
#define MPU6050_GYRO_XOUT_H   0x43
#define MPU6050_ACCEL_CONFIG  0x1C

typedef struct {
	int16_t ax, ay, az;
	int16_t gx, gy, gz;
	int16_t temp;
} MPU6050Sample;

HAL_StatusTypeDef MPU6050_init(void);
HAL_StatusTypeDef MPU6050_get_sample(MPU6050Sample *data);

#endif
