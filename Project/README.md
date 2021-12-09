# Project Title - Device Driver to read temperature data from the LM75 sensor by the host
## **Summary :-**
This project aims to develop a driver for a temperature sensor to interface it with any device running on linux which has i2c functionality. For testing we are using a Raspberry pi as the host running the Raspbian OS on top. It is based on Debian which is a popular version of Linux. The temperature sensor we used is LM-75.


## **Hardware Design :-**
- Raspberry Pi B+ v1.2
- LM75 sensor

Datasheet : <https://www.nxp.com/docs/en/data-sheet/LM75B.pdf>

![](Aspose.Words.d4901098-260f-4fb0-8dae-3e4c93d7a426.001.png)


|![](Aspose.Words.d4901098-260f-4fb0-8dae-3e4c93d7a426.002.jpeg)|<p><h3>**Pin Configuration:-**</h3></p><p>Sensor - Raspberry Pi</p><p>VCC  - Pin 1 (3.3V)</p><p>GND - Pin 6</p><p>SDA - Pin 3</p><p>SCL - Pin 5</p><p></p>|
| :- | :- |



LM-75 Address Mapping :- 


|**Address**|**Function**|
| :-: | :-: |
|0x00h|Temperature register|
|0x01h|Configuration register|
|0x02h|Hysteresis register|
|0x03h|Overtemperature shutdown threshold register|




**Kernel Space Driver:-**

The driver is meant to be used with the Linux kernel. It creates a simple interface for the host to communicate with the sensor. The sensor and the host use I2C communication protocol to transfer information. 

IOCTL system calls have been used for device specific I/O operations. This includes reading the temperature values and other configuration registers as mentioned above.

Commands to be executed for driver insertion into the kernel:

(Note : To be executed from the directory where the make file and kernel are present.)

- make
- sudo insmod temp.ko 

(Temp is the name of kernel file)

- sudo chmod 777 /dev/LM75 

(LM75 is the name of device file)

chmod - Command used to change the access permission of file

To remove the driver the following command can be executed:

- sudo rmmod temp.ko
### **User Space Application:-**
The user space application has separate functions for different I/O operations that have been implemented using IOCTL in the kernel driver. Depending upon the input received from the user, separate functionality is implemented which has been clearly mentioned through output.

Commands to be executed after driver insertion:-

- ./user : To run the userapp
- Input Value : 
  - 1 - Current Temperature
  - 2 - Configuration
  - 3 - Hysteresis Temperature
  - 4 - OverTemperature
  - 5 - Shutdown Mode
  - 6 - Normal Mode

