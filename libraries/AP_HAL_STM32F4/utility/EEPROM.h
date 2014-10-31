#ifndef __EEPROM_H
#define __EEPROM_H

#include "stm32f4xx.h"

/* Exported macro ------------------------------------------------------------*/
/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define EEPROM_MAIN_SECTOR FLASH_Sector_11
#define EEPROM_SWAP_SECTOR FLASH_Sector_10

#define EEPROM_USES_16BIT_WORDS
#define EEPROM_SECTOR_SIZE	((uint32_t)0x20000)
#define EEPROM_PAGE_SIZE	((uint32_t)0x2000) //8kByte

#define EEPROM_START_ADDRESS	ADDR_FLASH_SECTOR_11

/* Pages 0 and x base and end addresses */
#define EEPROM_PAGE0_BASE		((uint32_t)(EEPROM_START_ADDRESS))
#define EEPROM_PAGEx_BASE		((uint32_t)(EEPROM_START_ADDRESS + EEPROM_SECTOR_SIZE - EEPROM_PAGE_SIZE))

#define EEPROM_SWAP_BASE		ADDR_FLASH_SECTOR_10

/* Page status definitions */
#define EEPROM_ERASED			((uint16_t)0xFFFF)	/* PAGE is empty */
#define EEPROM_RECEIVE_DATA		((uint16_t)0xEEEE)	/* PAGE is marked to receive data, temperary state in transfering */
#define EEPROM_VALID_PAGE		((uint16_t)0xCCCC)	/* PAGE containing valid data */
#define EEPROM_IGNORED_PAGE			((uint16_t)0x0000)

/* Page full define */
enum //: uint16
{
	EEPROM_OK            = ((uint16_t)0x0000),
	EEPROM_OUT_SIZE      = ((uint16_t)0x0081),
	EEPROM_BAD_ADDRESS   = ((uint16_t)0x0082),
	EEPROM_BAD_FLASH     = ((uint16_t)0x0083),
	EEPROM_NOT_INIT      = ((uint16_t)0x0084),
	EEPROM_NO_VALID_PAGE = ((uint16_t)0x00AB)
};

#define EEPROM_DEFAULT_DATA		0xFFFF

class EEPROMClass
{
public:
	EEPROMClass(void); //*

	uint16_t init(void); //*

	uint16_t format(void); //*

	uint16_t erases(uint16_t *); //*
	uint16_t read (uint16_t address); //*
	uint16_t read (uint16_t address, uint16_t *data); //*
	uint16_t write(uint16_t address, uint16_t data); //*
	uint16_t count(uint16_t *);
	uint16_t maxcount(void);

	uint32_t CurPageBase;
	uint32_t CurSwapPageBase;
	uint32_t PageSize;
	uint16_t Status;
private:
	FLASH_Status EE_EraseSector(uint32_t pageBase);
	uint32_t EE_FindValidPage(uint32_t sector); //*
	uint16_t EE_GetVariablesCount(uint16_t); //*
	uint16_t EE_PageTransfer(uint32_t, uint32_t, uint16_t); //*
	uint16_t EE_VerifyPageFullWriteVariable(uint16_t, uint16_t); //*
};

#endif	/* __EEPROM_H */
