/*
Author - Calvin Gross
Date - 5/18/26
Title - imu_logger.c
Project - Modeling Data from a MPU6050; Project #1
Description - Header file for imu__logger.c.
*/


#include "imu_logger.h"
#include <stdint.h>
#include <string.h>

#define MAX_SAMPLES    4500
#define LOG_START_ADDR 0x08060000
#define LOG_SIZE       (128U * 1024U)
#define LOG_HEADER_KEY 0x1AE23FEDU

typedef struct {
	uint32_t key;
	uint16_t sample_count;
	uint16_t sample_size;
	uint16_t sample_rate_hz;
} LogHeader;

static uint16_t sample_count = 0;
static FullIMUSample samples_buf[MAX_SAMPLES];


HAL_StatusTypeDef logger_init(void) {
	sample_count = 0;
	return HAL_OK;
}


HAL_StatusTypeDef write_to_buf(FullIMUSample *sample) {
	if (sample_count >= MAX_SAMPLES || sample == NULL) {
		return HAL_ERROR;
	}
	samples_buf[sample_count] = *sample;
	sample_count++;
	return HAL_OK;
}


bool buf_full(void) {
	if (MAX_SAMPLES <= sample_count) return false;
	return true;
}

HAL_StatusTypeDef clear_buf(void) {
	sample_count = 0;
	memset((uint8_t *)samples_buf, 0, MAX_SAMPLES);
}

bool flash_empty(void) {
	uint32_t key_buf;

	memcpy(&key_buf, (const void *)LOG_START_ADDR, sizeof(key_buf));

	return key_buf == 0xFFFFFFFFU;
}


HAL_StatusTypeDef clear_flash_sector_7(void) {

	FLASH_EraseInitTypeDef erase_init;
	uint32_t erase_error = 0;
	HAL_StatusTypeDef erase_status;

	HAL_FLASH_Unlock();

	erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
	erase_init.Sector = FLASH_SECTOR_7;
	erase_init.NbSectors = 1;
	erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase_status = HAL_FLASHEx_Erase(&erase_init, &erase_error);

	HAL_FLASH_Lock();

	if (erase_error != 0xFFFFFFFFU) return HAL_ERROR;
	return erase_status;
}


HAL_StatusTypeDef write_bytes_to_flash(const uint8_t *data, uint32_t length, uint32_t flash_addr) {

	HAL_StatusTypeDef status;

	HAL_FLASH_Unlock();

	for (uint32_t i = 0; i < length; i += 4) {

		uint32_t word = 0xFFFFFFFFU;
		memcpy(&word, &data[i], (length - i >= 4U ? 4U : length-i));

		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_addr+i, word);
		if (status != HAL_OK) {
			HAL_FLASH_Lock();
			return status;
		}
	}

	HAL_FLASH_Lock();

	return HAL_OK;
}


HAL_StatusTypeDef write_buf_to_flash(void) {

	HAL_StatusTypeDef status;
	LogHeader header;

	header.key = LOG_HEADER_KEY;
	header.sample_count = sample_count;
	header.sample_rate_hz = 50;
	header.sample_size = sizeof(FullIMUSample);

	uint32_t header_bytes = sizeof(header);
	uint32_t sample_bytes = header.sample_size * header.sample_count;
	uint32_t total_bytes  = header_bytes + sample_bytes;
	uint32_t flash_addr   = LOG_START_ADDR;

	if (total_bytes > LOG_SIZE) return HAL_ERROR;
	status = clear_flash_sector_7();
	if (status !=  HAL_OK) return status;


	status = write_bytes_to_flash((uint8_t *)&header, header_bytes, flash_addr);
	if (status != HAL_OK) return status;

	status = write_bytes_to_flash((uint8_t *)samples_buf, sample_bytes, flash_addr+header_bytes);
	return status;

}


HAL_StatusTypeDef dump_flash_uart(void) {
	LogHeader header;
	FullIMUSample sample;
	HAL_StatusTypeDef status;
	char sample_str[120];

	memcpy(&header, (const void *)LOG_START_ADDR, sizeof(header));
	if (header.key != LOG_HEADER_KEY) return HAL_ERROR;

	const uint32_t log_data_addr = LOG_START_ADDR + sizeof(header);
	const uint32_t total_data_bytes = header.sample_size * header.sample_count;

	for (uint32_t i = log_data_addr; i < total_data_bytes+log_data_addr; i += sizeof(FullIMUSample)) {

		memcpy(&sample, (const void *)i, sizeof(FullIMUSample));

		snprintf(sample_str, sizeof(sample_str), "%d, %d, %d, %d, %d, %d, %d, %lu\r\n",
													sample.ax, sample.ay, sample.az, sample.gx,
													sample.gy, sample.gz, sample.temp, sample.time);
		uart_print(sample_str);
	}

	status = clear_flash_sector_7();
	if (status != HAL_OK) return status;

	return status;
}













