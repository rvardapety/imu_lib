#ifndef IMU_LIB_IMU_H
#define IMU_LIB_IMU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
} accel_value_t ;

typedef struct {
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} gyro_value_t;

bool imu_i2c_init(void);
bool imu_i2c_deinit (void);
bool imu_get_gyro_data (gyro_value_t *gyro);
bool imu_get_accel_data (accel_value_t *accel);

#endif //IMU_LIB_IMU_H
