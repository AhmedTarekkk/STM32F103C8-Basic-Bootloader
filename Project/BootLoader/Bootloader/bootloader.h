/******************************************************************************
*  File name:		bootloader.h
*  Date:				May 12, 2023
*  Author:			Ahmed Tarek
*  Version:         1.0
*******************************************************************************/
#ifndef	_BOOTLOADER_H_
#define _BOOTLOADER_H_

/*******************************************************************************
*                         Types Declaration                                   *
*******************************************************************************/

/*******************************************************************************
* Name: BL_Status
* Type: Enumeration
* Description: Data type to get the response of the BL
********************************************************************************/
typedef enum
{
	BL_NACK,
	BL_OK
}BL_Status;

/*******************************************************************************
* Name: pFunction
* Type: Pointer to function
* Description: Data type to represent functions address
********************************************************************************/
typedef void(*pFunction)(void) ;

/*******************************************************************************
*                        		Definitions                                   		 *
*******************************************************************************/
/*******************************************************************************
*                        		Configuration                                   		 *
*******************************************************************************/
#define BL_DEBUG_UART												&huart2
#define BL_HOST_COMMUNICATION_UART					&huart1
#define BL_ENABLE_UART_DEBUG_MESSAGE

#define BL_HOST_BUFFER_SIZE									200

#define CRC_BYTE_SIZE												4
#define CRC_ENGINE_OBJ											&hcrc

/*******************************************************************************
*                        		BL Commands                                   		 *
*******************************************************************************/
#define CBL_GET_VER_CMD												0x10
#define CBL_GET_HELP_CMD											0x11
#define CBL_GET_CID_CMD												0x12
#define CBL_GET_RDP_STATUS_CMD								0x13
#define CBL_GO_TO_ADDR_CMD										0x14
#define CBL_FLASH_ERASE_CMD										0x15
#define CBL_MEM_WRITE_CMD											0x16
#define CBL_EN_R_W_PROTECT_CMD								0x17
#define CBL_MEM_READ_CMD											0x18
#define CBL_READ_SECTOR_STATUS_CMD						0x19
#define CBL_OTP_READ_CMD											0x20
#define CBL_CHANGE_ROP_LEVEL_CMD							0x21

/*******************************************************************************
*                        		Version	 		                                  		 *
*******************************************************************************/
#define CBL_VERSION_ID											100
#define CBL_SW_MAJOR_VERSION								1
#define CBL_SW_MINORR_VERSION								0
#define CBL_SW_PATCH_VERSION								0

/*******************************************************************************
*                        		CRC	 		                                  		 *
*******************************************************************************/
#define CRC_OK															1
#define CRC_NOK															0

/*******************************************************************************
*                        		ACK VALUES	 		                                 	 *
*******************************************************************************/
#define CBL_SEND_NACK												0xAB
#define CBL_SEND_ACK												0xCD

/*******************************************************************************
*                        		ADDRESS VALIDATION	 		                        	 *
*******************************************************************************/
#define ADDRESS_IS_INVALID									0x00
#define ADDRESS_IS_VALID										0x01
#define STM32F103_SRAM_START								(0x20000000)
#define STM32F103_SRAM_END									(STM32F103_SRAM_START+(20*1024))
#define STM32F103_FLASH_START								(0x08000000)
#define STM32F103_FLASH_END									(STM32F103_FLASH_START+(64*1024))
#define APP_BASE_ADDREESS										0x08008000

/*******************************************************************************
*                        		ERASE FLASH			 		                 	             *
*******************************************************************************/
#define SECTOR_NUMBER_INVALID								0x00
#define SECTOR_NUMBER_VALID									0x01
#define ERASE_UNSUCCESSFUL									0x02
#define ERASE_SUCCESSFUL										0x03
#define PAGE_ERASE_SUCCESS									0xFFFFFFFF /* from FlashEx function */
#define MAX_SECTOR_NUMBER										4
#define PAGES_PER_SECTOR										16
#define PAGE_SIZE														1024
#define ERASE_ALL_COMMAND										0xFF

/*******************************************************************************
*                        		WRITE FLASH			 		                  	           *
*******************************************************************************/
#define FLASH_WRITE_FAILED									0x00
#define FLASH_WRITE_PASSED									0x01

/*******************************************************************************
*                        		FLASH PROROTECTION			 		                  	           *
*******************************************************************************/
#define FLASH_RDP_LEVEL_0										0x00
#define FLASH_RDP_LEVEL_1										0x01
#define ROP_CHANGE_FAILED										0x00
#define ROP_CHANGE_SUCCESSED								0x01

/*******************************************************************************
*                      Functions Prototypes                                    *
*******************************************************************************/

/*******************************************************************************
* Function Name:		BL_UART_Fetch_Host_command
* Description:			Function to process the data received
* Parameters (in):  None
* Parameters (out): NACK or OK
* Return value:     BL_Status
********************************************************************************/
BL_Status BL_UART_Fetch_Host_Command(void);

/*******************************************************************************
* Function Name:		BL_Print_Message
* Description:			Function to take string from and print it to the uart
*										that is dedicated for the debugging inside the bootloader
* Parameters (in):  string to print
* Parameters (out): None
* Return value:     Void
********************************************************************************/
void BL_Print_Message(char*format,...);

#endif /* _BOOTLOADER_H_ */
