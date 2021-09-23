#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>


#define MAJOR_NUM 237

#define GYRO_X 0
#define GYRO_Y 1
#define GYRO_Z 2

#define ACC_X 3
#define ACC_Y 4
#define ACC_Z 5

#define COM_X 6
#define COM_Y 7
#define COM_Z 8

#define PRESSURE 9

#define GYRO_X_READ _IOR(MAJOR_NUM,GYRO_X,int)
#define GYRO_Y_READ _IOR(MAJOR_NUM,GYRO_Y,int)
#define GYRO_Z_READ _IOR(MAJOR_NUM,GYRO_Z,int)

#define ACC_X_READ _IOR(MAJOR_NUM,ACC_X,int)
#define ACC_Y_READ _IOR(MAJOR_NUM,ACC_Y,int)
#define ACC_Z_READ _IOR(MAJOR_NUM,ACC_Z,int)

#define COM_X_READ _IOR(MAJOR_NUM,COM_X,int)
#define COM_Y_READ _IOR(MAJOR_NUM,COM_Y,int)
#define COM_Z_READ _IOR(MAJOR_NUM,COM_Z,int)

#define PRESSURE_READ _IOR(MAJOR_NUM,PRESSURE,int)


#define DEVICE_FILE_NAME "/dev/imu_char"

#endif

