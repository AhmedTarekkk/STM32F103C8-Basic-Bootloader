/******************************************************************************
*  File name:		bootloader.c
*  Date:				May 12, 2023
*  Author:			Ahmed Tarek
*  Version:         1.0
*******************************************************************************/

/*******************************************************************************
*                        		Inclusions                                   		   *
*******************************************************************************/
#include <stdint.h>
#include "bootloader.h"
#include "bootloader_private.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"
#include "crc.h"

/*******************************************************************************
*                           Global Variables                                  *
*******************************************************************************/
static uint8_t BL_HOST_Buffer[BL_HOST_BUFFER_SIZE];

uint8_t BL_Supported_Commands[12] =
{
	CBL_GET_VER_CMD,
	CBL_GET_HELP_CMD,
	CBL_GET_CID_CMD,
	CBL_GET_RDP_STATUS_CMD,
	CBL_GO_TO_ADDR_CMD,
	CBL_FLASH_ERASE_CMD,
	CBL_MEM_WRITE_CMD,
	CBL_EN_R_W_PROTECT_CMD,
	CBL_MEM_READ_CMD,
	CBL_READ_SECTOR_STATUS_CMD,
	CBL_OTP_READ_CMD,
	CBL_CHANGE_ROP_LEVEL_CMD
};
/*******************************************************************************
*                      Functions Definitions                                   *
*******************************************************************************/

/*******************************************************************************
* Function Name:		BL_UART_Fetch_Host_command
********************************************************************************/
BL_Status BL_UART_Fetch_Host_Command(void)
{
	BL_Status Status = BL_NACK;
	uint8_t Data_Length = 0;
	HAL_StatusTypeDef UART_Status = HAL_ERROR ;
	
	/* Clearing the host buffer so we can receive */
	memset(BL_HOST_Buffer,0,BL_HOST_BUFFER_SIZE);
	/* Receive the command size from the host */
	UART_Status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART,BL_HOST_Buffer,1,HAL_MAX_DELAY);
	if(UART_Status == HAL_OK)
	{
		Data_Length = BL_HOST_Buffer[0];
		/* Receive the whole command from the host */
		UART_Status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART,BL_HOST_Buffer+1,Data_Length,HAL_MAX_DELAY);
		Data_Length = BL_HOST_Buffer[0];
		if(UART_Status == HAL_OK)
		{
			switch(BL_HOST_Buffer[1])
			{
				case CBL_GET_VER_CMD:
					BL_Get_Version(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_GET_HELP_CMD:
					BL_Get_Help(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_GET_CID_CMD:
					BL_Get_Chip_ID(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_GET_RDP_STATUS_CMD:
					BL_Get_Read_Protection_Level(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_GO_TO_ADDR_CMD:
					BL_Jump_To_Address(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_FLASH_ERASE_CMD:
					BL_Erase_Flash(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_MEM_WRITE_CMD:
					BL_Memory_Write(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_EN_R_W_PROTECT_CMD:
					BL_Print_Message("Enable read/write protect on different sectors of the user flash \r\n");
					BL_Enable_RW_Protection(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_MEM_READ_CMD:
					BL_Print_Message("Read data from different memories of the MCU \r\n");
					BL_Memory_Read(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_READ_SECTOR_STATUS_CMD:
					BL_Print_Message("Read all the sector protection status \r\n");
					BL_Get_Sector_Protection_Status(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_OTP_READ_CMD:
					BL_Print_Message("Read the OTP Content \r\n");
					BL_Read_OTP(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				case CBL_CHANGE_ROP_LEVEL_CMD:
					BL_Change_Read_Protection_Level(BL_HOST_Buffer);
					Status = BL_OK;
					break;
				
				default:
					BL_Print_Message("Invalid command code received from the host !!\r\n");
				
					Status = BL_NACK;
					break;
			}
		}
		else
		{
			Status = BL_NACK;
		}
	}
	else
	{
		Status = BL_NACK;
	}
	
	return Status;
}

/*******************************************************************************
*                      Private Functions Definitions                           *
*******************************************************************************/

/*******************************************************************************
* Function Name:		BL_Print_Message
********************************************************************************/
void BL_Print_Message(char*format,...)
{
	char Message[100] = {0};
	va_list args;
	va_start(args,format);
	vsprintf(Message,format,args);
	#ifdef BL_ENABLE_UART_DEBUG_MESSAGE
	HAL_UART_Transmit(BL_DEBUG_UART, (uint8_t*)Message, sizeof(Message), HAL_MAX_DELAY);
	#endif
	va_end(args);
}

/*******************************************************************************
* Function Name:		BL_Send_Data_To_Host
********************************************************************************/
static void BL_Send_Data_To_Host(uint8_t *Data_Buffer, uint32_t Data_Len)
{
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART,Data_Buffer,Data_Len,HAL_MAX_DELAY);
}

/*******************************************************************************
* Function Name:		BL_Send_ACK_NACK
********************************************************************************/
static void BL_Send_ACK_NACK(BL_Status BL_Message,uint8_t Reply_Len)
{
	uint8_t ACK_Value[2] = {0,Reply_Len};
	switch(BL_Message)
	{
		case BL_OK:
			ACK_Value[0] = CBL_SEND_ACK ;
			BL_Send_Data_To_Host(ACK_Value,2);
			break;
		
		case BL_NACK:
			ACK_Value[0] = CBL_SEND_NACK ;
			BL_Send_Data_To_Host(ACK_Value,1);
			break;
		
		default:
			break;
	}
	
}

/*******************************************************************************
* Function Name:		BL_Get_Version
********************************************************************************/
static void BL_Get_Version(uint8_t *Hostbuffer)
{
	BL_Print_Message("Read the bootloader version from the MCU \r\n");
	
	/* Get the CRC value and the length sent by the user */
	uint8_t BL_Version[4] = {CBL_VERSION_ID,CBL_SW_MAJOR_VERSION,CBL_SW_MINORR_VERSION,CBL_SW_PATCH_VERSION};
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	
	/* CRC Verification */
	if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
	{
		BL_Print_Message("CRC Verification Passed \r\n");
		BL_Send_ACK_NACK(BL_OK,4);
		BL_Send_Data_To_Host(BL_Version,4);
	}
	else
	{
		BL_Print_Message("CRC Verification Failed \r\n");
		BL_Send_ACK_NACK(BL_NACK,0);
	}
}

/*******************************************************************************
* Function Name:		BL_Get_Help
********************************************************************************/
static void BL_Get_Help(uint8_t *Hostbuffer)
{
	BL_Print_Message("Read the commands supported by the bootloader \r\n");
	
	/* Get the CRC value and the length sent by the user */
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	
	/* CRC Verification */
	if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
	{
		BL_Print_Message("CRC Verification Passed \r\n");
		BL_Send_ACK_NACK(BL_OK,12);
		BL_Send_Data_To_Host(BL_Supported_Commands,12);
	}
	else
	{
		BL_Print_Message("CRC Verification Failed \r\n");
		BL_Send_ACK_NACK(BL_NACK,0);
	}
}

/*******************************************************************************
* Function Name:		BL_Get_Chip_ID
********************************************************************************/
static void BL_Get_Chip_ID(uint8_t *Hostbuffer)
{
	BL_Print_Message("Read the MCU Chip identification number \r\n");
	
	/* Get the CRC value and the length sent by the user */
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	
	uint16_t MCU_ID = 0;
	/* CRC Verification */
	if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
	{
		BL_Print_Message("CRC Verification Passed \r\n");
		BL_Send_ACK_NACK(BL_OK,2);
		MCU_ID = (uint16_t)(DBGMCU->IDCODE & 0xFFF); /*To get the id only which is first 11 bits */
		BL_Send_Data_To_Host((uint8_t *)&MCU_ID,2);
	}
	else
	{
		BL_Print_Message("CRC Verification Failed \r\n");
		BL_Send_ACK_NACK(BL_NACK,0);
	}
}

/*******************************************************************************
* Function Name:		BL_GET_RDP_LEVEL
********************************************************************************/
static uint8_t BL_GET_RDP_LEVEL(void)
{
	uint8_t RDP_Value;
	FLASH_OBProgramInitTypeDef OBConfiguration;
	HAL_FLASHEx_OBGetConfig(&OBConfiguration);
	if( OB_RDP_LEVEL_0 == OBConfiguration.RDPLevel )
	{
		RDP_Value = FLASH_RDP_LEVEL_0 ;
	}
	else
	{
		RDP_Value = FLASH_RDP_LEVEL_1 ;
	}
	return (RDP_Value) ;
}

/*******************************************************************************
* Function Name:		BL_Get_Read_Protection_Level
********************************************************************************/
static void BL_Get_Read_Protection_Level(uint8_t *Hostbuffer)
{
	BL_Print_Message("Read the flash read protection level \r\n");

	/* Get the CRC value and the length sent by the user */
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	
	/* CRC Verification */
	if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
	{
		BL_Print_Message("CRC Verification Passed \r\n");
		BL_Send_ACK_NACK(BL_OK,1);
		uint8_t RDP_Level = BL_GET_RDP_LEVEL();
		
		BL_Send_Data_To_Host((uint8_t *)&RDP_Level,1);
	}
	else
	{
		BL_Print_Message("CRC Verification Failed \r\n");
		BL_Send_ACK_NACK(BL_NACK,0);
	}
}

/*******************************************************************************
* Function Name:		BL_Host_Jump_Address_Verify
********************************************************************************/
static uint8_t BL_Host_Jump_Address_Verify(uint32_t Jump_Address)
{
	uint8_t Address_Verification = ADDRESS_IS_INVALID;
	
	if( ((Jump_Address >= STM32F103_SRAM_START) && (Jump_Address <= STM32F103_SRAM_END)) \
		|| ((Jump_Address >= STM32F103_FLASH_START) && (Jump_Address <= STM32F103_FLASH_END)))
	{
		Address_Verification = ADDRESS_IS_VALID;
	}
	else
	{
		Address_Verification = ADDRESS_IS_INVALID;
	}
	return Address_Verification;
}

/*******************************************************************************
* Function Name:		BL_Jump_To_User_App
********************************************************************************/
static void BL_Jump_To_User_App(void)
{
	/* Value if the main stack pointer of our main application */
	uint32_t MSP_Value = *((volatile uint32_t *)APP_BASE_ADDREESS);
	
	/* Reset handler definition function of our main application */
	uint32_t MainAppAddr = *((volatile uint32_t *)(APP_BASE_ADDREESS+4));
	pFunction APP_ResetHandler_Address = (pFunction)MainAppAddr;
	
	/* Set the main stack pointer to its value */
	__set_MSP(MSP_Value);
	
	/* Deintialization of module */
	HAL_RCC_DeInit(); /* Reset the RCC clock configuration to the deafult reset state */
	
	/* Jump to application reset handler */
	APP_ResetHandler_Address();
}

/*******************************************************************************
* Function Name:		BL_Jump_To_Address
********************************************************************************/
static void BL_Jump_To_Address(uint8_t *Hostbuffer)
{
	BL_Print_Message("Jump bootloader to specified address \r\n");
	
	/* Get the CRC value and the length sent by the user */
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	
	/* Extract the jump address and verify it */
	uint32_t Host_Jump_Address = *((uint32_t *)(Hostbuffer+2));
	
		/* CRC Verification */
		if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
		{
			
			BL_Print_Message("CRC Verification Passed \r\n");
			BL_Send_ACK_NACK(BL_OK,1);
			
			uint8_t Address_Verification = BL_Host_Jump_Address_Verify(Host_Jump_Address);
			if(ADDRESS_IS_VALID == Address_Verification)
			{
				BL_Print_Message("Address Verification Passed \r\n");
				BL_Send_Data_To_Host(&Address_Verification,1);
				/* If we received this specific address we will assume that the user want
				 * to end the bootloader and go to the app */
				if( Host_Jump_Address == APP_BASE_ADDREESS )
				{
					BL_Jump_To_User_App();
				}
				if((Host_Jump_Address & 0x01) == 0)
				{
					/* To ensure that the function address LSB = one which stands for Thumb state  */
					Host_Jump_Address++;
				} 
				pFunction Jump_Address = (pFunction)Host_Jump_Address ;
				Jump_Address();
			}
			else
			{
				BL_Print_Message("Address Verification Failed \r\n");
				BL_Send_Data_To_Host(&Address_Verification,1);
			}
		}
		else
		{
			BL_Print_Message("CRC Verification Failed \r\n");
			BL_Send_ACK_NACK(BL_NACK,0);
		}	
}

/*******************************************************************************
* Function Name:		BL_Perform_Flash_Erase
********************************************************************************/
static uint8_t BL_Perform_Flash_Erase(uint8_t Sector_Number, uint8_t Number_Of_Sectors)
{
	uint8_t Erase_Status = ERASE_UNSUCCESSFUL;
	if(( (Number_Of_Sectors+Sector_Number) <= MAX_SECTOR_NUMBER ) || (ERASE_ALL_COMMAND == Sector_Number))
	{
		FLASH_EraseInitTypeDef pEraseInit;
		uint32_t PageError;
		if((Number_Of_Sectors+Sector_Number) <= MAX_SECTOR_NUMBER)
		{
			pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
		}
		else
		{
			pEraseInit.TypeErase = FLASH_TYPEERASE_MASSERASE;
		}
		pEraseInit.Banks = FLASH_BANK_1;
		pEraseInit.PageAddress = STM32F103_FLASH_START + (PAGE_SIZE*PAGES_PER_SECTOR*Sector_Number);
		pEraseInit.NbPages = Number_Of_Sectors * PAGES_PER_SECTOR;
		
		/* Start Erasing */
		HAL_FLASH_Unlock();
		HAL_FLASHEx_Erase(&pEraseInit,&PageError);
		HAL_FLASH_Lock();
		
		if(PAGE_ERASE_SUCCESS == PageError)
		{
			Erase_Status = ERASE_SUCCESSFUL;
		}
		else
		{
			Erase_Status = ERASE_UNSUCCESSFUL;
		}
	}
	else
	{
		Erase_Status = SECTOR_NUMBER_INVALID;
	}
	return Erase_Status;
}

/*******************************************************************************
* Function Name:		BL_Erase_Flash
********************************************************************************/
static void BL_Erase_Flash(uint8_t *Hostbuffer)
{
	BL_Print_Message("Mass erase or sector erase of user flash \r\n");
	
	/* Get the CRC value and the length sent by the user */
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	uint8_t Erase_Status = 0;
	
	/* CRC Verification */
	if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
	{
		BL_Print_Message("CRC Verification Passed \r\n");
		BL_Send_ACK_NACK(BL_OK,1);
		
		/* Erase the required secotrs */
		Erase_Status = BL_Perform_Flash_Erase(Hostbuffer[2],Hostbuffer[3]);
		if(ERASE_SUCCESSFUL == Erase_Status)
		{
			BL_Print_Message("Erase Is Done \r\n");
			BL_Send_Data_To_Host(&Erase_Status,1);
		}
		else
		{
			BL_Print_Message("Erase Failed \r\n");
			BL_Send_Data_To_Host(&Erase_Status,1);
		}
		
	}
	else
	{
		BL_Print_Message("CRC Verification Failed \r\n");
		BL_Send_ACK_NACK(BL_NACK,0);
	}
}

/*******************************************************************************
* Function Name:		BL_Write_Payload_In_Flash
********************************************************************************/
static uint8_t BL_Write_Payload_In_Flash(uint8_t *Host_Payload, uint32_t Start_Address, uint8_t Payload_Len)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint16_t Payload_Counter = 0;
	uint8_t Write_Status = FLASH_WRITE_FAILED;
	
	/* Unlock the flash memory */
	HAL_Status = HAL_FLASH_Unlock();
	
	if(HAL_OK != HAL_Status)
	{
		Write_Status = FLASH_WRITE_FAILED;
	}
	else
	{
		/* If unlock passed then write the flash */
		for(Payload_Counter = 0 ; Payload_Counter < Payload_Len ; Payload_Counter+= 2)
		{
			HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Start_Address+Payload_Counter,*((uint16_t*)(&Host_Payload[Payload_Counter])));
			if(HAL_OK != HAL_Status)
			{
				break;
			}
		}
		//HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,0x08008000,0xBBAA);
		
	}
	
	if(HAL_OK != HAL_Status)
	{
		Write_Status = FLASH_WRITE_FAILED;
	}
	else
	{
		Write_Status = FLASH_WRITE_PASSED;
	}
	
	HAL_Status = HAL_FLASH_Lock();
	
	return Write_Status;
}

/*******************************************************************************
* Function Name:		BL_Memory_Write
********************************************************************************/
static void BL_Memory_Write(uint8_t *Hostbuffer)
{
	BL_Print_Message("Write data into different memories of the MCU \r\n");
	
	/* Get the CRC value and the length sent by the user */
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	
	/* CRC Verification */
	if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
	{
		BL_Print_Message("CRC Verification Passed \r\n");
		BL_Send_ACK_NACK(BL_OK,1);
		
		/* Extract the start address and the payload length */
		uint32_t Start_Address = *((uint32_t *)(Hostbuffer+2)) ;
		uint8_t Payload_Len = Hostbuffer[6];
		uint8_t Address_Verification = BL_Host_Jump_Address_Verify(Start_Address);
		if(ADDRESS_IS_VALID == Address_Verification)
		{
			BL_Print_Message("Address Verification Passed \r\n");
			uint8_t Write_Status = BL_Write_Payload_In_Flash(Hostbuffer+7,Start_Address,Payload_Len);
			if(FLASH_WRITE_PASSED == Write_Status)
			{
				BL_Print_Message("Wite Successed \r\n");
				BL_Send_Data_To_Host(&Write_Status,1);
			}
			else
			{
				BL_Print_Message("Wite Failed \r\n");
				BL_Send_Data_To_Host(&Write_Status,1);
			}
		}
		else
		{
			BL_Print_Message("Address Verification Failed \r\n");
			BL_Send_Data_To_Host(&Address_Verification,1);
		}
	}
	else
	{
		BL_Print_Message("CRC Verification Failed \r\n");
		BL_Send_ACK_NACK(BL_NACK,0);
	}
}

/*******************************************************************************
* Function Name:		BL_Enable_RW_Protection
********************************************************************************/
static void BL_Enable_RW_Protection(uint8_t *Hostbuffer)
{
	
}

/*******************************************************************************
* Function Name:		BL_Memory_Read
********************************************************************************/
static void BL_Memory_Read(uint8_t *Hostbuffer)
{
	
}

/*******************************************************************************
* Function Name:		BL_Get_Sector_Protection_Status
********************************************************************************/
static void BL_Get_Sector_Protection_Status(uint8_t *Hostbuffer)
{
	
}

/*******************************************************************************
* Function Name:		BL_Read_OTP
********************************************************************************/
static void BL_Read_OTP(uint8_t *Hostbuffer)
{
	
}

/*******************************************************************************
* Function Name:		BL_Change_ROP_Level
********************************************************************************/
static uint8_t BL_Change_ROP_Level(uint8_t ROP_Level)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR , HAL_Status1 = HAL_ERROR;
	uint8_t ROP_Status = ROP_CHANGE_FAILED;
	if(ROP_Level == 0x00)
	{
		ROP_Level = OB_RDP_LEVEL_0;
	}
	else if(ROP_Level == 0x01)
	{
		ROP_Level = OB_RDP_LEVEL_1;
	}
	else
	{
		return 0;
	}
	/* Unlock the flash */
	HAL_Status1 = HAL_FLASH_Unlock();
	HAL_Status = HAL_FLASH_OB_Unlock();
	if(HAL_OK == HAL_Status && HAL_OK == HAL_Status1)
	{
		/* Proragm the option bytes */
		FLASH_OBProgramInitTypeDef OBProgram;
		OBProgram.OptionType = OPTIONBYTE_RDP; /* Read protection only */
		OBProgram.Banks = FLASH_BANK_1; /* We have only one bank */
		OBProgram.RDPLevel = ROP_Level; /* The required protection level */
		HAL_Status = HAL_FLASHEx_OBProgram(&OBProgram);
		
		if(HAL_OK == HAL_Status)
		{
			ROP_Status = ROP_CHANGE_SUCCESSED;
		}
		else
		{
			ROP_Status = ROP_CHANGE_FAILED;
		}
		
		/* Lock the flash and the option bytes again */
		HAL_Status = HAL_FLASH_OB_Lock();
		HAL_Status = HAL_FLASH_Lock();
	}
	else
	{
		ROP_Status = ROP_CHANGE_FAILED;
	}
	
	return ROP_Status;
}

/*******************************************************************************
* Function Name:		BL_Change_Read_Protection_Level
********************************************************************************/
static void BL_Change_Read_Protection_Level(uint8_t *Hostbuffer)
{
	BL_Print_Message("Change read protection on different sectors of the user flash \r\n");

	/* Get the CRC value and the length sent by the user */
	uint16_t Host_CMD_Packet_Len = Hostbuffer[0]+1;
	uint32_t Host_CRC32 = *((uint32_t *)(Hostbuffer+Host_CMD_Packet_Len-CRC_BYTE_SIZE));
	
	/* CRC Verification */
	if(CRC_OK == BL_CRC_Verify(Hostbuffer, Host_CMD_Packet_Len - CRC_BYTE_SIZE, Host_CRC32))
	{
		BL_Print_Message("CRC Verification Passed \r\n");
		BL_Send_ACK_NACK(BL_OK,1);
		uint8_t ROP_Status = BL_Change_ROP_Level(Hostbuffer[2]);
		BL_Send_Data_To_Host(&ROP_Status,1);
		
		/* Lauch the option byte to apply the changes */
		HAL_FLASH_OB_Launch();
	}
	else
	{
		BL_Print_Message("CRC Verification Failed \r\n");
		BL_Send_ACK_NACK(BL_NACK,0);
	}
}

/*******************************************************************************
* Function Name:		BL_CRC_Verify
********************************************************************************/
static uint8_t BL_CRC_Verify(uint8_t *pData, uint32_t Data_Len, uint32_t Host_CRC)
{
	uint8_t CRC_Status = CRC_NOK;
	uint32_t CRC_RECEIVED_DATA = 0;
	/* Calculate CRC */
	for(uint16_t i = 0 ; i < Data_Len ; i++)
	{
		uint32_t Data_Buffer = (uint32_t)pData[i];
		CRC_RECEIVED_DATA = HAL_CRC_Accumulate(CRC_ENGINE_OBJ,&Data_Buffer,1);
	}
	/* Reset the CRC Engine to use it again as we use CRC accumlation */
	__HAL_CRC_DR_RESET(CRC_ENGINE_OBJ);
	/* Compare the calculated CRC with the host CRC*/
	if(CRC_RECEIVED_DATA == Host_CRC )
	{
		CRC_Status = CRC_OK;
	}
	else
	{
		CRC_Status = CRC_NOK;
	}
	return CRC_Status;
}
