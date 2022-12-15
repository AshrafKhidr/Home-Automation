#include "hal_eeprom.h"

Std_ReturnType Data_EEPROM_WriteByte(uint16 bAdrr, uint8 bData){
    Std_ReturnType ret = E_OK;
    /* Read the Interrupt Status "Enabled or Disabled" */
    uint8 global_interrupt_status = INTCONbits.GIE;        
    /* Update the Address Registers */
    EEADRH = (uint8)((bAdrr >> 8) & 0x03); 
    EEADR = (uint8)(bAdrr & 0xFF);
    /* Update the Data Register */
    EEDATA = bData;
    /* Select Access data EEPROM memory */
    EECON1bits.EEPGD = ACCESS_EEPROM; 
    EECON1bits.CFGS = ACCESS_FLASH_EEPROM;
    /* Allows write cycles to Flash program/data EEPROM */
    EECON1bits.WREN = ALLOW_WRITE_CYCLES_FLASH_EEPROM;
    /* Disable all interrupts "General Interrupt" */
    INTERRUPT_GlobalInterruptDisable();
    /* Write the required sequence : 0x55 -> 0xAA */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    /* Initiates a data EEPROM erase/write cycle */
    EECON1bits.WR = INITIATE_EEPROM_WRITE_ERASE;
    /* Wait for write to complete */
    while(EECON1bits.WR);
    /* Inhibits write cycles to Flash program/data EEPROM */
    EECON1bits.WREN = INHIBIT_WRITE_CYCLES_FLASH_EEPROM;
    /* Restore the Interrupt Status "Enabled or Disabled" */
    INTCONbits.GIE = global_interrupt_status;
    return ret;
}
Std_ReturnType Data_EEPROM_ReadByte(uint16 bAdrr, uint8 *bData){
    Std_ReturnType ret = E_OK;
    if(NULL == bData){
        ret = E_NOT_OK;
    }
    else{
        /* Update the Address Registers */
        EEADRH = (uint8)((bAdrr >> 8) & 0x03); 
        EEADR = (uint8)(bAdrr & 0xFF);
        /* Select Access data EEPROM memory */
        EECON1bits.EEPGD = ACCESS_EEPROM; 
        EECON1bits.CFGS = ACCESS_FLASH_EEPROM;
        /* Initiates a data EEPROM read cycle */
        EECON1bits.RD = INITIATE_EEPROM_READ;
        /* NOPs may be required for latency at high frequencies */
        NOP();
        /* NOPs may be required for latency at high frequencies */
        NOP();
        /* Return data */
        *bData = EEDATA;

    }
    return ret;
}
