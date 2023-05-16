# STM32F103C8 Basic Bootloader

UART based bootloader for STM32F103C8 target MCU that support multiple commands to access and control the flash memory of the MCU
You can find the BL files only in "Bootloader Files" folder, the host script in "Host Script" folder and the whole project using Keil IDE in "Project" folder
## Tools Used
1- StTM32 CubeMX <br>
2- Keil uVision5 IDE <br>
3- Python Host Script <br>
4- STM32 ST-LINK Utility <br>

## Usage and Host Commands

The Python Host (I din't implelmet it, i just edited afew thing to use it with stm32f103 instead of stm32f07 )
when you run it, it will ask for the COM Port that the USB to TTL module connected to, then it will list the supported commands by the host and their numbers.
##### 1- Get Version 
The BL will reply with its version which stored in the flash memory.
##### 2- Get Help
The BL will reply with all the commands that it supports.
##### 3- Get Chip Identification
The BL will reply with the Chip ID which stored in the IDCODE Register in its flash memory which is = 0x410 in STM32F103 MCU
##### 4- Get protectinon level
The BL will reply with the flash read protection level (0 or 1).
##### 5- Go to address
The host will ask the user for the address that he wants to jump to it then sends the command to the BL so he can execute it.
If the address  = 0x08008000 the BL will jump to the user main application and resets the other peripherals like the RCC.
Note : if the given address is invaild the BL will refuse to jump to the address and reply with NACK.
##### 6- Flash erase command
The host will asks the user for the start sector and the number of sectors that he wants to erase then sends the command to the BL.
If the givin inputs is invalid the BL will refues to do the operation and replies with NACK.
Note : we have totoal of 64 pages in stm32f103 MCU so i assumed that there is only 4 sections (from 0 to 3) and the max number of section to erase = 4.
##### 7- Memory write command
You have to put the new Binary file in the same directory with the Host script and rename it to
"Application.bin".
The host will asks the user for the required memory address that we want to write our binary file into it then sends the command and the data to the BL.
The BL will receive the bin file and replies with ACK for each group of byte, if any error occurred while writing the memory the BL will terminate the operation the replies with NACK as the binary file will be corrupted.

##### NOTE
the user have to vaildate the application binary file first and set the offset of the code using the linker script or keil options and the IVT using (SCB->VTOR) register before generating the Application binary file out the Application will always jump the BL IVT not its IVT.
<a href="https://ibb.co/Fzjctb4"><img src="https://i.ibb.co/fHPZ7Yd/1.png" alt="1" border="0"></a>

<a href="https://ibb.co/BCDYYM5"><img src="https://i.ibb.co/DRXjj2H/2.png" alt="2" border="0"></a>

<a href="https://ibb.co/NTGv7dp"><img src="https://i.ibb.co/DrjJLHk/3.png" alt="3" border="0"></a>

Setting the binary file to write it to 0x08008000 address.

##### 8~11 For future updates ISA
­
##### 12- Change the flash read protection level
We have only two levels for stm32f103 MCU:<br>
level 0 -> default. <br>
level 1 -> Disable the read . <br>
Note : For level 1 we can't use the debugger to connect our MCU as it will always trigger the HardFault handler so we need to back to level 0 to be able to debug the code and read the flash.
Changing the protection level from level 1 to 0 will erase the entire chip so we have to upload our codes again.
