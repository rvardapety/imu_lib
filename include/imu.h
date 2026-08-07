#ifndef IMU_LIB_IMU_H
#define IMU_LIB_IMU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
} accel_value_t ;

typedef struct {
    float gyro_x;
    float gyro_y;
    float gyro_z;
} gyro_value_t;

bool imu_i2c_init(void);
bool imu_i2c_deinit (void);
bool imu_get_gyro_data (gyro_value_t *gyro);
bool imu_get_accel_data (accel_value_t *accel);

#endif //IMU_LIB_IMU_H
