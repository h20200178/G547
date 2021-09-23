#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>		
#include "main_ioctl.h"	
#include<fcntl.h>              /* open */
#include<unistd.h>             /* exit */
#include<sys/ioctl.h>          /* ioctl */

int gyro_x_read(int file_desc)	//Reading Gyroscope X-axis
{
	unsigned int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("GYRO X-AXIS:%d\n", *data);
	return 0;
}

int gyro_y_read(int file_desc)	//Reading Gyroscope Y-axis
{
	unsigned int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("GYRO Y-AXIS:%d\n", *data);
	return 0;
}
int gyro_z_read(int file_desc)	//Reading Gyroscope Z-axis
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("GYRO Z-AXIS:%d\n", *data);
	return 0;
}

int acc_x_read(int file_desc)		//Reading Acclerometer X-axis
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("ACCELEROMETER X-AXIS:%d\n", *data);
	return 0;
}

int acc_y_read(int file_desc)		//Reading Acclerometer Y-axis
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("ACCELEROMETER Y-AXIS:%d\n", *data);
	return 0;
}
int acc_z_read(int file_desc)		//Reading Acclerometer Z-axis
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("ACCELEROMETER Z-AXIS:%d\n", *data);
	return 0;
}
int com_x_read(int file_desc)		//Reading Magnetometer X-axis
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("COMPASS X-AXIS:%d\n", *data);
	return 0;
}

int com_y_read(int file_desc)		//Reading Magnetometer Y-axis
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("COMPASS Y-AXIS:%d\n", *data);
	return 0;
}

int com_z_read(int file_desc)		//Reading Magnetometer Z-axis
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, GYRO_X_READ, data1);
	uint16_t *data = (uint16_t *) data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("COMPASS Z-AXIS:%d\n", *data);
	return 0;
}

int pressure_read(int file_desc)	//Reading Barometric pressure data
{
	int ret_val;
	char *data1;
	ret_val = ioctl(file_desc, PRESSURE_READ, data1);
	uint16_t *data = (uint16_t *)data1;
	
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }

	printf("BAROMETRIC PRESSURE:%d\n", *data);
	return 0;
}

int main()
{
	int file_desc;
	
	file_desc = open(DEVICE_FILE_NAME, 0);	//Opening file
	if (file_desc < 0) 
	{
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}
	
	//CAlling user functions to read 10 different paramters
	gyro_x_read(file_desc);
	gyro_y_read(file_desc);
	gyro_z_read(file_desc);
	
	acc_x_read(file_desc);
	acc_y_read(file_desc);
	acc_z_read(file_desc);
	
	com_x_read(file_desc);
	com_y_read(file_desc);
	com_z_read(file_desc);
	
	pressure_read(file_desc);
	
	close(file_desc);	//CLosing file
	return 0;
}
