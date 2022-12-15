/* 
 * File:   hal_eeprom.h
 * Author: ashra
 *
 * Created on 30 October 2022, 23:33
 */

#ifndef HAL_EEPROM_H
#define	HAL_EEPROM_H
/*------------------------Section : Includes------------------------*/

#include <pic18f4620.h>
#include "../../mcal/mcal_std_types.h"
#include "../INTERRUPT/mcal_internal_interrupt.h"
/*------------------------Section : Macro Declarations------------------------*/
#define ACCESS_FLASH_MEMORY                   1
#define ACCESS_EEPROM                         0
#define ACCESS_CONFIG_REGISTERS               1
#define ACCESS_FLASH_EEPROM                   0
#define ALLOW_WRITE_CYCLES_FLASH_EEPROM       1
#define INHIBIT_WRITE_CYCLES_FLASH_EEPROM     0

#define INITIATE_EEPROM_WRITE_ERASE           1
#define EEPROM_WRITE_ERASE_COMPLETE           0

#define INITIATE_EEPROM_READ                  1
/*------------------------Section : Macro Functions Declarations------------------------*/


/*------------------------Section : Data Types Declaration------------------------*/


/*------------------------Section : Function Declaration------------------------*/
Std_ReturnType Data_EEPROM_WriteByte(uint16 bAdrr, uint8 bData);
Std_ReturnType Data_EEPROM_ReadByte(uint16 bAdrr, uint8 *bData);


#ifdef	__cplusplus
}
#endif

#endif	/* HAL_EEPROM_H */

