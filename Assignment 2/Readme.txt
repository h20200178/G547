
1. Open terminal in the project folder and compile the code using make command
2. Insert the module into the kernel using sudo insmod main.ko command
3. View partition in the block device using sudo fdisk -l /dev/dof and ls -l /dev/dof* command
4. Perform read operation using diskdump utility
    dd if=/etc/sample.txt of=/dev/dof1 count=1
5. Type sudo -s to give permissions and then perform the following read / write operation 
6. Run sudo chmod 777 /dev/dof1 command
7. Perform write operation using the following command
    sudo cat > /dev/dof1
    /* Sample text */
8. Disply the block device content using sudo xxd /dev/dof1 | less command
9. Remove the block device driver using sudo rmmod main command
   
