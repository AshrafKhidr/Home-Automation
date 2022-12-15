#include "hal_timer0.h"

static inline void timer0_prescaler_config(const timer0_t *timer);
static inline void timer0_mode_select(const timer0_t *timer);
static inline void timer0_register_size_config(const timer0_t *timer);

#if TIMER0_INTERRUPT_FEATURE_ENABLE == INTERRUPT_FEATURE_ENABLE
static void (* timer0_InterruptHandlerPtr)(void) = NULL;
#endif

static uint16 timer0_preload = 0;

Std_ReturnType timer0_init(const timer0_t *timer){
    Std_ReturnType ret = E_OK;
    if(NULL == timer){
        ret = E_NOT_OK;
    }
    else{
        TIMER0_MODULE_DISABLE();
        timer0_prescaler_config(timer);
        timer0_mode_select(timer);
        timer0_register_size_config(timer);
        TMR0H = (timer->timer0_preload_value) >> 8;
        TMR0L = (uint8)(timer->timer0_preload_value);
        timer0_preload = timer->timer0_preload_value;
#if TIMER0_INTERRUPT_FEATURE_ENABLE == INTERRUPT_FEATURE_ENABLE
        TIMER0_InterruptEnable();
        TIMER0_InterruptFlagClear();
        timer0_InterruptHandlerPtr = timer->timer0_InterruptHandler;
#if INTERRUPT_PRIORITY_LEVELS_ENABLE == INTERRUPT_FEATURE_ENABLE
        INTERRUPT_PriorityLevelsEnable();
        if(INTERRUPT_HIGH_PRIORITY == timer->priority){
            INTERRUPT_GlobalInterruptHighEnable();
            TIMER0_HighPrioritySet();
        }
        else if(INTERRUPT_LOW_PRIORITY == timer->priority){
            INTERRUPT_GlobalInterruptLowEnable();
            TIMER0_LowPrioritySet();
        }
        else{/*nothing*/}
#else
        INTERRUPT_GlobalInterruptEnable();
        INTERRUPT_PeripheralInterruptEnable();   
#endif
#endif
        TIMER0_MODULE_ENABLE();
    }
    return ret;   
}
Std_ReturnType timer0_deinit(const timer0_t *timer){
    Std_ReturnType ret = E_OK;
    if(NULL == timer){
        ret = E_NOT_OK;
    }
    else{
        TIMER0_MODULE_DISABLE();
        #if TIMER0_INTERRUPT_FEATURE_ENABLE == INTERRUPT_FEATURE_ENABLE
        TIMER0_InterruptDisable();
        #endif
    }
    return ret; 
}
Std_ReturnType timer0_write_value(const timer0_t *timer , uint16 value){
    Std_ReturnType ret = E_OK;
    if(NULL == timer){
        ret = E_NOT_OK;
    }
    else{
        TMR0H = (value) >> 8;
        TMR0L = (uint8)(value);
    }
    return ret; 
}
Std_ReturnType timer0_read_value(const timer0_t *timer , uint16 *value){
    Std_ReturnType ret = E_OK;
    uint8 tmr0low = 0 , tmr0high = 0;
    if( (NULL == timer) || (NULL == value) ){
        ret = E_NOT_OK;
    }
    else{
        tmr0high = TMR0H;
        tmr0low = TMR0L;
        *value = (uint16)(tmr0low + (tmr0high << 8));
    }
    return ret; 
}

void TIMER0_ISR(void){
    TIMER0_InterruptFlagClear();
    TMR0H = (timer0_preload) >> 8;
    TMR0L = (uint8)(timer0_preload);
    if(timer0_InterruptHandlerPtr){
        timer0_InterruptHandlerPtr();
    }
    
}

static inline void timer0_prescaler_config(const timer0_t *timer){
    if( TIMER0_PRESCALER_ENABLE_CFG == timer->timer0_prescaler_enable){
        TIMER0_PRESCALER_ENABLE();
        T0CONbits.T0PS = timer->prescaler_value;
    }
    else if( TIMER0_PRESCALER_DISABLE_CFG == timer->timer0_prescaler_enable){
        TIMER0_PRESCALER_DISABLE();
    }
    else{/**/}
}

static inline void timer0_mode_select(const timer0_t *timer){
    if( TIMER0_TIMER_MODE == timer->timer0_mode){
        TIMER0_TIMER_MODE_ENABLE();
    }
    else if(TIMER0_COUNTER_MODE == timer->timer0_mode){
        TIMER0_COUNTER_MODE_ENABLE();
        if(TIMER0_RISING_EDGE_CFG == timer->timer0_counter_edge){
            TIMER0_RISING_EDGE_ENABLE();
        }
        else if(TIMER0_FALLING_EDGE_CFG == timer->timer0_counter_edge){
            TIMER0_FALLING_EDGE_ENABLE();
        }
        else{/*nothing*/}
    }
    else{/*nothing*/}
}
 

static inline void timer0_register_size_config(const timer0_t *timer){
    if( TIMER0_8BIT_MODE == timer->timer0_register_size){
        TIMER0_8BIT_MODE_ENABLE();
    }
    else if(TIMER0_16BIT_MODE == timer->timer0_register_size){
        TIMER0_16BIT_MODE_ENABLE();
    }
    else{/*nothing*/}
}
    
    
    