#include "mcal_interrupt_manager.h"

static volatile uint8 rb4_flag = 1;
static volatile uint8 rb5_flag = 1;
static volatile uint8 rb6_flag = 1;
static volatile uint8 rb7_flag = 1;

#if INTERRUPT_PRIORITY_LEVELS_ENABLE == INTERRUPT_FEATURE_ENABLE
void __interrupt() InterruptManagerHigh(void){
      
   if((INTERRUPT_ENABLE == INTCON3bits.INT2IE) && (INTERRUPT_OCCUR == INTCON3bits.INT2IF)){
        INT2_ISR();
    }
    else{ /* Nothing */ }
    if((INTERRUPT_ENABLE == INTCONbits.INT0IE) && (INTERRUPT_OCCUR == INTCONbits.INT0IF)){
        INT0_ISR();
    }
    else{ /* Nothing */ }
}

void __interrupt(low_priority) InterruptManagerLow(void){
    if( (INTCON3bits.INT1E == INTERRUPT_ENABLE) && (INTCON3bits.INT1F == INTERRUPT_OCCUR) ){
        INT1_ISR();
    }
    else{/*nothing*/}
}


#else

void __interrupt() InterruptManagerHigh(void){
//   
//    if( (INTCONbits.INT0IE == INTERRUPT_ENABLE) && (INTCONbits.INT0IF == INTERRUPT_OCCUR) ){
//        INT0_ISR();
//    }
//    else{/*nothing*/}
//    if( (INTCON3bits.INT1E == INTERRUPT_ENABLE) && (INTCON3bits.INT1F == INTERRUPT_OCCUR) ){
//        INT1_ISR();
//    }
//    else{/*nothing*/}
//    if( (INTCON3bits.INT2IE == INTERRUPT_ENABLE) && (INTCON3bits.INT2IF == INTERRUPT_OCCUR) ){
//        INT2_ISR();
//    }
//    else{/*nothing*/}
//    
//    
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR)
//            && (PORTBbits.RB4 == GPIO_HIGH) && (rb4_flag == 1)){
//        rb4_flag = 0;
//        RB4_ISR(0);
//    }
//    else{/*nothing*/}
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR)
//            && (PORTBbits.RB4 == GPIO_LOW) && (rb4_flag == 0) ){
//        rb4_flag = 1;
//        RB4_ISR(1);
//    }
//    else{/*nothing*/}
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR) 
//            && (PORTBbits.RB5 == GPIO_HIGH) && (rb5_flag == 1) ){
//        rb5_flag = 0;
//        RB5_ISR(0);
//    }
//    else{/*nothing*/}
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR)
//            && (PORTBbits.RB5 == GPIO_LOW) && (rb5_flag == 0)){
//        rb5_flag = 1;
//        RB5_ISR(1);
//    }
//    else{/*nothing*/}
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR)
//            && (PORTBbits.RB6 == GPIO_HIGH) && (rb6_flag == 1) ){
//        rb6_flag = 0;
//        RB6_ISR(0);
//    }
//    else{/*nothing*/}
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR)
//            && (PORTBbits.RB6 == GPIO_LOW) && (rb6_flag == 0) ){
//        rb6_flag = 1;
//        RB6_ISR(1);
//    }
//    else{/*nothing*/}
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR)
//            && (PORTBbits.RB7 == GPIO_HIGH) && (rb7_flag == 1) ){
//        rb7_flag = 0;
//        RB7_ISR(0);
//    }
//    else{/*nothing*/}
//    if( (INTCONbits.RBIE == INTERRUPT_ENABLE) && (INTCONbits.RBIF == INTERRUPT_OCCUR)
//            && (PORTBbits.RB7 == GPIO_LOW) && (rb7_flag == 0)){
//        rb7_flag = 1;
//        RB7_ISR(1);
//    }
//    else{/*nothing*/}
    
    if( (PIE1bits.ADIE == INTERRUPT_ENABLE) && (PIR1bits.ADIF == INTERRUPT_OCCUR) ){
        ADC_ISR();
    }
    else{/*nothing*/}
    
    if( (INTCONbits.TMR0IE == INTERRUPT_ENABLE) && (INTCONbits.TMR0IF == INTERRUPT_OCCUR) ){
        TIMER0_ISR();
    }
    else{/*nothing*/}
    
    if( (PIE1bits.TMR1IE == INTERRUPT_ENABLE) && (PIR1bits.TMR1IF == INTERRUPT_OCCUR) ){
        TIMER1_ISR();
    }
    else{/*nothing*/}
    
    if( (PIE1bits.TMR2IE == INTERRUPT_ENABLE) && (PIR1bits.TMR2IF == INTERRUPT_OCCUR) ){
        TIMER2_ISR();
    }
    else{/*nothing*/}
    
    if( (PIE2bits.TMR3IE == INTERRUPT_ENABLE) && (PIR2bits.TMR3IF == INTERRUPT_OCCUR) ){
        TIMER3_ISR();
    }
    else{/*nothing*/}
    
    if( (PIE1bits.CCP1IE == INTERRUPT_ENABLE) && (PIR1bits.CCP1IF == INTERRUPT_OCCUR) ){
        CCP1_ISR();
    }
    else{/*nothing*/}
    
    if( (PIE2bits.CCP2IE == INTERRUPT_ENABLE) && (PIR2bits.CCP2IF == INTERRUPT_OCCUR) ){
        CCP2_ISR();
    }
    else{/*nothing*/}
    
    if( (PIE1bits.TXIE == INTERRUPT_ENABLE) && (PIR1bits.TXIF == INTERRUPT_OCCUR) ){
        USART_TX_ISR();
    }
    else{/*nothing*/}
    
    if( (PIE1bits.RCIE == INTERRUPT_ENABLE) && (PIR1bits.RCIF == INTERRUPT_OCCUR) ){
        USART_RX_ISR();
    }
    else{/*nothing*/}
    
    if((INTERRUPT_ENABLE == PIE1bits.SSPIE) && (INTERRUPT_OCCUR == PIR1bits.SSPIF)){
        MSSP_SPI_ISR();
    }
    else{ /* Nothing */ }
    
    if((INTERRUPT_ENABLE == PIE1bits.SSPIE) && (INTERRUPT_OCCUR == PIR1bits.SSPIF)){
        I2C_ISR();
    }
    else{ /* Nothing */ }
    
    if((INTERRUPT_ENABLE == PIE2bits.BCLIE) && (INTERRUPT_OCCUR == PIR2bits.BCLIF)){
        I2C_BUS_COLLISION_ISR();
    }
    else{ /* Nothing */ }
    
}
#endif