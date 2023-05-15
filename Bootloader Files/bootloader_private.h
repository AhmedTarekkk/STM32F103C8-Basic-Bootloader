
#ifndef		_BL_PRIVATE_H_
#define		_BL_PRIVATE_H_

/*******************************************************************************
*                      Private Functions                               		     *
*******************************************************************************/
/*******************************************************************************
* Function Name:		BL_Send_Data_To_Host
* Description:			Function to send data to the host uart
* Parameters (in):  data buffer and the size
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Send_Data_To_Host(uint8_t *Data_Buffer, uint32_t Data_Len);

/*******************************************************************************
* Function Name:		BL_Send_ACK_NACK
* Description:			Function to send ACK or NACK to the user
* Parameters (in):  the reply length in case we want to send ACK
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Send_ACK_NACK(BL_Status BL_Message,uint8_t Reply_Len);


/*******************************************************************************
* Function Name:		BL_Get_Version
* Description:			Read the bootloader version from the MCU
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Get_Version(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Get_Help
* Description:			Read the commands supported by the bootloader
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Get_Help(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Get_Chip_ID
* Description:			Read the MCU Chip identification number
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Get_Chip_ID(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_GET_RDP_LEVEL
* Description:			Get the flash protection level
* Parameters (in):  None
* Parameters (out): The protection level
* Return value:     uint8_t
********************************************************************************/
static uint8_t BL_GET_RDP_LEVEL(void);

/*******************************************************************************
* Function Name:		BL_Get_Read_Protection_Level
* Description:			Read the flash read protection level
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Get_Read_Protection_Level(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Host_Jump_Address_Verify
* Description:			Function to verify the value of the jump address
* Parameters (in):  The jump address
* Parameters (out): OK or NOK
* Return value:     uint8_t
********************************************************************************/
static uint8_t BL_Host_Jump_Address_Verify(uint32_t Jump_Address);

/*******************************************************************************
* Function Name:		BL_Jump_To_User_App
* Description:			Jump bootloader to the main application
* Parameters (in):  None
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Jump_To_User_App(void);

/*******************************************************************************
* Function Name:		BL_Jump_To_Address
* Description:			Jump bootloader to specified address
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Jump_To_Address(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Perform_Flash_Erase
* Description:			Mass erase or sector erase of user flash
* Parameters (in):  Sector Number and number of sectors
* Parameters (out): OK or ERROR
* Return value:     uint8_t
********************************************************************************/
static uint8_t BL_Perform_Flash_Erase(uint8_t Sector_Number, uint8_t Number_Of_Sectors);

/*******************************************************************************
* Function Name:		BL_Erase_Flash
* Description:			Mass erase or sector erase of user flash
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Erase_Flash(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Write_Payload_In_Flash
* Description:			Write the payload in the flash memory
* Parameters (in):  The required payload, the start address and the payload length
* Parameters (out): OK or ERROR
* Return value:     uint8_t
********************************************************************************/
static uint8_t BL_Write_Payload_In_Flash(uint8_t *Host_Payload, uint32_t Start_Address, uint8_t Payload_Len);

/*******************************************************************************
* Function Name:		BL_Memory_Write
* Description:			Write data into different memories of the MCU
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Memory_Write(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Enable_RW_Protection
* Description:			Enable read/write protect on different sectors of the user flash
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Enable_RW_Protection(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Memory_Read
* Description:			Read data from different memories of the MCU
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Memory_Read(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Get_Sector_Protection_Status
* Description:			Read all the sector protection status
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Get_Sector_Protection_Status(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Read_OTP
* Description:			Read the OTP Content
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Read_OTP(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_Change_ROP_Level
* Description:			Change read protection on different sectors of the user flash
* Parameters (in):  The required protection level
* Parameters (out): OK or ERROR
* Return value:     uint8_t
********************************************************************************/
static uint8_t BL_Change_ROP_Level(uint8_t ROP_Level);

/*******************************************************************************
* Function Name:		BL_Change_Read_Protection_Level
* Description:			Change read protection on different sectors of the user flash
* Parameters (in):  The host buffer
* Parameters (out): None
* Return value:     Void
********************************************************************************/
static void BL_Change_Read_Protection_Level(uint8_t *Hostbuffer);

/*******************************************************************************
* Function Name:		BL_CRC_Verify
* Description:			Function to verify the CRC value
* Parameters (in):  Pointer to data that we want to calculate the CRC in it
										Length of that data
										The host CRC
* Parameters (out): OK or NOK
* Return value:     uint8_t
********************************************************************************/
static uint8_t BL_CRC_Verify(uint8_t *pData, uint32_t Data_Len, uint32_t Host_CRC);

#endif
