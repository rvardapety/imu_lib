#include "imu.h"
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define AWAKE_MODE 0x00
#define SLEEP_MODE 0x40
#define PWR_MGMT_1 0x6B
#define MPU_I2C_ADDR 0x68
#define GYRO_OUT_X_H 0x43
#define ACCEL_OUT_X_H 0x3B
#define GYRO_CONFIG   0x1B
#define ACCEL_CONFIG  0x1C

#define DEG_TO_RAD 0.017453292519943295f
#define GYRO_SCALE_FACTOR   131.0f    // LSB/(°/s)
#define ACCEL_SCALE_FACTOR  16384.0f  // LSB/g

// Register block starting at ACCEL_OUT_X_H: accel X/Y/Z (0-5), temperature (6-7), gyro X/Y/Z (8-13)
#define SENSOR_BLOCK_LENGTH 14

static int file = -1;
static uint8_t buffer[SENSOR_BLOCK_LENGTH];
static const char *filename = "/dev/i2c-1";

// Big-endian register pair -> signed 16-bit value (independent of whether char is signed)
static int16_t to_int16(const uint8_t *bytes)
{
    return (int16_t)(((uint16_t)bytes[0] << 8) | (uint16_t)bytes[1]);
}

static void parse_accel(const uint8_t *block, accel_value_t *accel)
{
    accel->accel_x = to_int16(&block[0]) / ACCEL_SCALE_FACTOR;
    accel->accel_y = to_int16(&block[2]) / ACCEL_SCALE_FACTOR;
    accel->accel_z = to_int16(&block[4]) / ACCEL_SCALE_FACTOR;
}

static void parse_gyro(const uint8_t *block, gyro_value_t *gyro)
{
    gyro->gyro_x = (to_int16(&block[8]) / GYRO_SCALE_FACTOR) * DEG_TO_RAD;
    gyro->gyro_y = (to_int16(&block[10]) / GYRO_SCALE_FACTOR) * DEG_TO_RAD;
    gyro->gyro_z = (to_int16(&block[12]) / GYRO_SCALE_FACTOR) * DEG_TO_RAD;
}

// Reads 'length' bytes starting at ACCEL_OUT_X_H into buffer
static bool read_sensor_block(int length)
{
    uint8_t reg[1] = {ACCEL_OUT_X_H};

    if (write(file, reg, 1) < 0) {
        return false;
    }

    return read(file, buffer, (size_t)length) == length;
}

bool imu_i2c_init(void) {

    if ((file = open(filename, O_RDWR)) < 0) {
        return false;
    }

    if (ioctl(file, I2C_SLAVE, MPU_I2C_ADDR) < 0) {
        return false;
    }

    uint8_t config[2] = {PWR_MGMT_1, AWAKE_MODE};
    if (write(file, config, 2) < 0) {
        return false;
    }

    // Accelerometer: ±2 g
    uint8_t accel_config[2] = {ACCEL_CONFIG, 0x00};

    if (write(file, accel_config, 2) < 0) {
        return false;
    }

    // Gyroscope: ±250 degrees/second
    uint8_t gyro_config[2] = {GYRO_CONFIG, 0x00};

    if (write(file, gyro_config, 2) < 0) {
        return false;
    }

    return true;
}

bool imu_get_accel_data (accel_value_t *accel) {
    if (!read_sensor_block(6)) {
        return false;
    }

    parse_accel(buffer, accel);
    return true;
}

bool imu_get_gyro_data (gyro_value_t *gyro) {
    if (!read_sensor_block(SENSOR_BLOCK_LENGTH)) {
        return false;
    }

    parse_gyro(buffer, gyro);
    return true;
}

bool imu_get_data (accel_value_t *accel, gyro_value_t *gyro) {
    // One I2C transaction, so accel and gyro come from the same sample
    if (!read_sensor_block(SENSOR_BLOCK_LENGTH)) {
        return false;
    }

    parse_accel(buffer, accel);
    parse_gyro(buffer, gyro);
    return true;
}

bool imu_i2c_deinit (void) {
    if (file < 0) {
        return true;
    }

    uint8_t imu_sleep_mode[2] = {PWR_MGMT_1, SLEEP_MODE};
    bool ok = write(file, imu_sleep_mode, 2) >= 0;

    close(file);
    file = -1;
    return ok;
}
