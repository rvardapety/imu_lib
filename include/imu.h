#ifndef IMU_LIB_IMU_H
#define IMU_LIB_IMU_H

bool imu_i2c_init(void);
bool imu_i2c_deinit (void);
bool imu_get_gyro_data (gyro_value_t *gyro);
bool imu_get_accel_data (accel_value_t *accel);

#endif //IMU_LIB_IMU_H
