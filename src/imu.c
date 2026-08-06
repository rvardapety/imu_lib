#include "imu.h"
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

int file;
char buffer[14];
char *filename = "/dev/i2c-1";

bool imu_i2c_init(void) {

    if ((file = open(filename, O_RDWR)) < 0) {
        return false;
    }

    if (ioctl(file, I2C_SLAVE, MPU_I2C_ADDR) < 0) {
        return false;
    }

    char config[2] = {PWR_MGMT_1, AWAKE_MODE};
    if (write(file, config, 2) < 0) {
        return false;
    }

    return true;
}

bool imu_get_accel_data (accel_value_t *accel) {

        char reg[1] = {ACCEL_OUT_X_H};

        if (write(file, reg, 1) < 0) {
            return false;
        }

        if (read(file, buffer, 6) == 6) {
            accel->accel_x = (buffer[0] << 8) | buffer[1];
            accel->accel_y = (buffer[2] << 8) | buffer[3];
            accel->accel_z = (buffer[4] << 8) | buffer[5];

            return true;
        }
    return false;
}

bool imu_get_gyro_data (gyro_value_t *gyro) {
        char reg[1] = {ACCEL_OUT_X_H};

        if (write(file, reg, 1) < 0) {
            return false;
        }

        if (read(file, buffer, 14) == 14) {
            gyro->gyro_x = (buffer[8] << 8) | buffer[9];
            gyro->gyro_y = (buffer[10] << 8) | buffer[11];
            gyro->gyro_z = (buffer[12] << 8) | buffer[13];

            return true;
        }
    return false;
}

bool imu_i2c_deinit (void) {
    if (file >= 0) {
        char imu_sleep_mode[2] = {PWR_MGMT_1, SLEEP_MODE};
        if (write(file, imu_sleep_mode, 2) < 0) {
            return false;
        }
    }
    close(file);
    return true;
}
