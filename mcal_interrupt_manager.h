/* 
 * File:   mcal_interrupt_manager.h
 * Author: ashra
 *
 * Created on 27 October 2022, 22:36
 */

#ifndef MCAL_INTERRUPT_MANAGER_H
#define	MCAL_INTERRUPT_MANAGER_H

/* Section : Includes */
#include "mcal_interrupt_config.h"
#include "../GPIO/hal_gpio.h"
/* Section : Macro Declarations */


/* Section : Macro Functions Declarations */


/* Section : Data Types Declarations */


/* Section : Function Declarations */
void INT0_ISR(void);
void INT1_ISR(void);
void INT2_ISR(void);
void RB4_ISR(uint8 RB4_source);
void RB5_ISR(uint8 RB5_source);
void RB6_ISR(uint8 RB6_source);
void RB7_ISR(uint8 RB7_source);
void ADC_ISR(void);
void TIMER0_ISR(void);
void TIMER1_ISR(void);
void TIMER2_ISR(void);
void TIMER3_ISR(void);
void CCP1_ISR(void);
void CCP2_ISR(void);
void USART_TX_ISR(void);
void USART_RX_ISR(void);
void MSSP_SPI_ISR(void);
void I2C_ISR(void);
void I2C_BUS_COLLISION_ISR(void);
#endif	/* MCAL_INTERRUPT_MANAGER_H */

