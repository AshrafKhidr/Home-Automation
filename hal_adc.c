
#include "hal_adc.h"

#if ADC_INTERRUPT_FEATURE_ENABLE == INTERRUPT_FEATURE_ENABLE
static void (* ADC_InterruptHandlerPtr)(void) = NULL;
#endif


static inline void adc_input_channel_port_config(const adc_channel_select_t channel);
static inline void select_result_format(const adc_config_t *adc_obj);
static inline void config_volt_reference(const adc_config_t *adc_obj);


Std_ReturnType adc_init(const adc_config_t *adc_obj){
    Std_ReturnType ret = E_OK;
    if(NULL == adc_obj){
        ret = E_NOT_OK;
    }
    else{
        /* Disable the ADC */
        ADC_CONVERSION_DISABLE();
        /* Configure the acquisition time */
        ADCON2bits.ACQT = adc_obj->acquisition_time;
        /* Configure the conversion clock */
        ADCON2bits.ADCS = adc_obj->conversion_clock;
        /* Configure the default channel */
        ADCON0bits.CHS = adc_obj->adc_channel;
        adc_input_channel_port_config(adc_obj->adc_channel);
        /* Configure the interrupt */
#if ADC_INTERRUPT_FEATURE_ENABLE == INTERRUPT_FEATURE_ENABLE        
        ADC_InterruptEnable();
        ADC_InterruptHandlerPtr = adc_obj->ADC_InterruptHandler;
        ADC_InterruptFlagClear();
#if INTERRUPT_PRIORITY_LEVELS_ENABLE == INTERRUPT_FEATURE_ENABLE
        if(INTERRUPT_HIGH_PRIORITY == adc_obj->priority){
            INTERRUPT_GlobalInterruptHighEnable();
            ADC_HighPrioritySet();
        }
        else if(INTERRUPT_LOW_PRIORITY == adc_obj->priority){
            INTERRUPT_GlobalInterruptLowEnable();
            ADC_LowPrioritySet();
        }
        else{/*nothing*/}
#else
        INTERRUPT_GlobalInterruptEnable();
        INTERRUPT_PeripheralInterruptEnable();   
#endif
        
#endif
        /* Configure the result format */
        select_result_format(adc_obj);
        /* Configure the voltage reference */
        config_volt_reference(adc_obj);
        /* Enable the ADC */
        ADC_CONVERSION_ENABLE();
    }
    return ret;
}

Std_ReturnType adc_deinit(const adc_config_t *adc_obj){
    Std_ReturnType ret = E_OK;
    if(NULL == adc_obj){
        ret = E_NOT_OK;
    }
    else{
       /* Disable the ADC */
       ADC_CONVERSION_DISABLE();
       /* Configure the interrupt */
       #if ADC_INTERRUPT_FEATURE_ENABLE == INTERRUPT_FEATURE_ENABLE
        ADC_InterruptDisable();
        
        #endif
    }
    return ret;
}

Std_ReturnType adc_select_channel(const adc_config_t *adc_obj , adc_channel_select_t channel){
    Std_ReturnType ret = E_OK;
    if(NULL == adc_obj){
        ret = E_NOT_OK;
    }
    else{
        ADCON0bits.CHS = channel;
        adc_input_channel_port_config(channel);
    }
    return ret;
}

Std_ReturnType adc_start_conversion(const adc_config_t *adc_obj){
    Std_ReturnType ret = E_OK;
    if(NULL == adc_obj){
        ret = E_NOT_OK;
    }
    else{
        ADC_START_CONVERSION();
    }
    return ret;
}

Std_ReturnType adc_is_conversion_done(const adc_config_t *adc_obj , uint8 *conversion_status){
    Std_ReturnType ret = E_OK;
    if( (NULL == adc_obj) || (NULL == conversion_status) ){
        ret = E_NOT_OK;
    }
    else{
        *conversion_status = (uint8)(!(ADCON0bits.GO_nDONE));
    }
    return ret;
}

Std_ReturnType adc_get_conversion_result(const adc_config_t *adc_obj , adc_result_t *conversion_result){
    Std_ReturnType ret = E_OK;
    if( (NULL == adc_obj) || (NULL == conversion_result) ){
        ret = E_NOT_OK;
    }
    else{
        if(ADC_RESULT_RIGHT == adc_obj->result_format){
            *conversion_result = (adc_result_t)(ADRESL + (ADRESH << 8));
        }
        else if(ADC_RESULT_LEFT == adc_obj->result_format){
            *conversion_result = (adc_result_t)( ( ADRESL + (ADRESH << 8) ) >> 6 );
        }
        else{        }
    }
    return ret;
} 

Std_ReturnType adc_convert_from_channel_blocking(const adc_config_t *adc_obj , adc_channel_select_t channel , adc_result_t *conversion_result){
    Std_ReturnType ret = E_OK;
    if( (NULL == adc_obj) || (NULL == conversion_result)){
        ret = E_NOT_OK;
    }
    else{
        /*select channel*/
        ret = adc_select_channel(adc_obj , channel);
        /*start conversion*/
        ret = adc_start_conversion(adc_obj);
        /*check if conversion is done*/
        while(ADCON0bits.GO_nDONE);  // block code till conversion is done 
        ret = adc_get_conversion_result(adc_obj , conversion_result);

    }
    return ret;
}


Std_ReturnType adc_convert_from_channel_interrupt(const adc_config_t *adc_obj , adc_channel_select_t channel){
    Std_ReturnType ret = E_OK;
    if(NULL == adc_obj){
        ret = E_NOT_OK;
    }
    else{
        /*select channel*/
        ret = adc_select_channel(adc_obj , channel);
        /*start conversion*/
        ret = adc_start_conversion(adc_obj);
    }
    return ret;
}

static inline void adc_input_channel_port_config(const adc_channel_select_t channel){
    switch(channel){
        case ADC_CHANNEL_AN0 : SET_BIT(TRISA , _TRISA_RA0_POSN); break;
        case ADC_CHANNEL_AN1 : SET_BIT(TRISA , _TRISA_RA1_POSN); break;
        case ADC_CHANNEL_AN2 : SET_BIT(TRISA , _TRISA_RA2_POSN); break;
        case ADC_CHANNEL_AN3 : SET_BIT(TRISA , _TRISA_RA3_POSN); break;
        case ADC_CHANNEL_AN4 : SET_BIT(TRISA , _TRISA_RA5_POSN); break;
        case ADC_CHANNEL_AN5 : SET_BIT(TRISE , _TRISE_RE0_POSN); break;
        case ADC_CHANNEL_AN6 : SET_BIT(TRISE , _TRISE_RE1_POSN); break;
        case ADC_CHANNEL_AN7 : SET_BIT(TRISE , _TRISE_RE2_POSN); break;
        case ADC_CHANNEL_AN8 : SET_BIT(TRISB , _TRISB_RB2_POSN); break;
        case ADC_CHANNEL_AN9 : SET_BIT(TRISB , _TRISB_RB3_POSN); break;
        case ADC_CHANNEL_AN10 : SET_BIT(TRISB , _TRISB_RB1_POSN); break;
        case ADC_CHANNEL_AN11 : SET_BIT(TRISB , _TRISB_RB4_POSN); break;
        case ADC_CHANNEL_AN12 : SET_BIT(TRISB , _TRISB_RB0_POSN); break;
        default : ;        
    }
}

static inline void select_result_format(const adc_config_t *adc_obj){
    if(ADC_RESULT_RIGHT == adc_obj->result_format){
        ADC_RESULT_RIGHT_FORMAT();
    }
    else if(ADC_RESULT_LEFT == adc_obj->result_format){
        ADC_RESULT_LEFT_FORMAT();
    }
    else{
        ADC_RESULT_RIGHT_FORMAT();
    }
}

static inline void config_volt_reference(const adc_config_t *adc_obj){
    if(ADC_VOLT_REFERENCE_ENABLED == adc_obj->volt_reference){
        ADC_ENABLE_VOLT_REFERENCE();
    }
    else if(ADC_VOLT_REFERENCE_DISABLED == adc_obj->volt_reference){
        ADC_DISABLE_VOLT_REFERENCE();
    }
    else{
        ADC_DISABLE_VOLT_REFERENCE();
    }
}

void ADC_ISR(void){
    ADC_InterruptFlagClear();
    if(ADC_InterruptHandlerPtr){
        ADC_InterruptHandlerPtr();
    }
    
}