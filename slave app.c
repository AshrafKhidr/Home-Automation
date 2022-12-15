
#include "application.h"

void ISRs(void);
volatile uint16 required_temperature =24; // the required temperature which sent from Master with initial value 24
volatile uint16 temp_sensor_reading =0; // the temperature of the room 
volatile uint8 counter =0; // the counter which determine the periodic time of implementing ISR
volatile uint8 last_air_conditioning_value =AIR_CONDTIONING_OFF; // last air conditioning value which will help in hysteresis

volatile uint16 session_counter = 0;//ignore this line
uint8 timeout_flag = 0;//ignore this line


led_t room1_led = {.port_name = PORTD_INDEX, .pin = GPIO_PIN4, .led_status = GPIO_LOW};
led_t room2_led = {.port_name = PORTD_INDEX, .pin = GPIO_PIN5, .led_status = GPIO_LOW};
led_t room3_led = {.port_name = PORTD_INDEX, .pin = GPIO_PIN6, .led_status = GPIO_LOW};
led_t room4_led = {.port_name = PORTD_INDEX, .pin = GPIO_PIN7, .led_status = GPIO_LOW};
led_t cond_led = {.port_name = PORTD_INDEX, .pin = GPIO_PIN2, .led_status = GPIO_LOW};
led_t tv_led = {.port_name = PORTD_INDEX, .pin = GPIO_PIN3, .led_status = GPIO_LOW};
SPI_Config spi_slave = {
    .spi_config.ClockPolarity = SPI_TRANSMIT_ACTIVE_TO_IDLE,
    .spi_config.ClockSelect = SPI_IDLE_STATE_HIGH_LEVEL,
    .spi_config.SampleSelect = SPI_DATA_SAMPLE_MIDDLE,
    .spi_mode = SPI_SLAVE_SS_ENABLE,    
    };
timer0_t timer0_obj = {
      .timer0_counter_edge = TIMER0_RISING_EDGE_CFG,
      .timer0_mode = TIMER0_TIMER_MODE,
      .timer0_preload_value = 3036,
      .timer0_register_size = TIMER0_16BIT_MODE,
      .timer0_prescaler_enable = TIMER0_PRESCALER_DISABLE_CFG,
      .timer0_InterruptHandler  = ISRs,
    };
adc_config_t adc_temp = {
   .acquisition_time = ADC_12_TAD, 
   .conversion_clock = ADC_CONVERSION_CLOCK_FOSC_DIV_16,
   .adc_channel = ADC_CHANNEL_AN0,
   .volt_reference = ADC_VOLT_REFERENCE_DISABLED,
   .result_format  = ADC_RESULT_RIGHT,   
};
int main() {
   Std_ReturnType ret = E_NOT_OK;
    

    adc_init(&adc_temp);//initialize the ADC of the micro controller
    timer0_init(&timer0_obj);//Initialize the timer zero of the micro controller
    SPI_Init(&spi_slave);//initialize the SPI as a slave
    /* initialization of output pins of connected leds or devices */
    led_initialize(&cond_led);
    led_initialize(&tv_led);
    led_initialize(&room1_led);
    led_initialize(&room2_led);
    led_initialize(&room3_led);
    led_initialize(&room4_led);
    
	uint8 request = DEFAULT_ACK;//the value that is received from the master
	uint8 response = DEFAULT_ACK;//the values that is sent back to the master
    
   while(1){
        request = SPI_Read_Byte(&spi_slave , &request);//wait for the master to start the transmitting
		//ALL Messages are defined in STD_messages.h
		switch (request)
		{
			/*********************************   STATUS COMMANDS ********************************/
			//commands related to send the current status back to the master
			case ROOM1_STATUS:
			if (room1_led.led_status==LED_OFF)//if the led is turned off
			{
				response = OFF_STATUS;//set the response as off status
			}
			else if (room1_led.led_status==LED_ON)//if the led is turned on
			{
				response = ON_STATUS;//set the response as on status
			}
			else
			{
			}
			SPI_Send_Byte(&spi_slave ,response);//response to the transmitter with the status
			break;//break the switch case
			case ROOM2_STATUS:
			if (room2_led.led_status==LED_OFF)//if the led is turned off
			{
				response = OFF_STATUS;//set the response as off status
			}
			else if (room2_led.led_status==LED_ON)//if the led is turned on
			{
				response = ON_STATUS;//set the response as on status
			}
			else
			{
			}
			SPI_Send_Byte(&spi_slave ,response);//response to the transmitter with the status
			break;//break the switch case
			case ROOM3_STATUS:
			if (room3_led.led_status==LED_OFF)//if the led is turned off
			{
				response = OFF_STATUS;//set the response as off status
			}
			else if (room3_led.led_status==LED_ON)//if the led is turned on
			{
				response = ON_STATUS;//set the response as on status
			}
			else
			{
			}
			SPI_Send_Byte(&spi_slave ,response);//response to the transmitter with the status
			break;//break the switch case
			case ROOM4_STATUS:
			if (room4_led.led_status==LED_OFF)//if the led is turned off
			{
				response = OFF_STATUS;//set the response as off status
			}
			else if (room4_led.led_status==LED_ON)//if the led is turned on
			{
				response = ON_STATUS;//set the response as on status
			}
			else
			{
			}
			SPI_Send_Byte(&spi_slave ,response);//response to the transmitter with the status
			break;//break the switch case
			case AIR_COND_STATUS:
			if (cond_led.led_status==LED_OFF)//if the led is turned off
			{
				response = OFF_STATUS;//set the response as off status
			}
			else if (cond_led.led_status==LED_ON)//if the led is turned on
			{
				response = ON_STATUS;//set the response as on status
			}
			else
			{
			}
			SPI_Send_Byte(&spi_slave ,response);//response to the transmitter with the status
			break;
			case TV_STATUS:
			if (tv_led.led_status==LED_OFF)//if the led is turned off
			{
				response = OFF_STATUS;//set the response as off status
			}
			else if (tv_led.led_status==LED_ON)//if the led is turned on
			{
				response = ON_STATUS;//set the response as on status
			}
			else
			{
			}
			SPI_Send_Byte(&spi_slave ,response);//response to the transmitter with the status
			break;//break the switch case
			
			/*********************************   TURN ON COMMANDS ********************************/
			case ROOM1_TURN_ON:
			led_turn_on(&room1_led);//turn on the led of room 1
			break;//break the switch case
			case ROOM2_TURN_ON:
			led_turn_on(&room2_led);//turn on the led of room 2
			break;//break the switch case
			case ROOM3_TURN_ON:
			led_turn_on(&room3_led);//turn on the led of room 3
			break;//break the switch case
			case ROOM4_TURN_ON:
			led_turn_on(&room4_led);//turn on the led of room 4
			break;//break the switch case
			case AIR_COND_TURN_ON:
			timer0_init(&timer0_obj);
			led_turn_on(&cond_led);//turn on the led of air conditioning
			break;//break the switch case
			case TV_TURN_ON:
			led_turn_on(&tv_led);//turn on the led of the TV
			break;//break the switch case
			
			/*********************************   TURN OFF COMMANDS ********************************/
			case ROOM1_TURN_OFF:
			led_turn_off(&room1_led);//turn off the led of room 1
			break;//break the switch case
			case ROOM2_TURN_OFF:
			led_turn_off(&room2_led);//turn off the led of room 2
			break;//break the switch case
			case ROOM3_TURN_OFF:
			led_turn_off(&room3_led);//turn off the led of room 3
			break;//break the switch case
			case ROOM4_TURN_OFF:
			led_turn_off(&room4_led);//turn off the led of room 4
			break;//break the switch case
			case AIR_COND_TURN_OFF:
            timer0_deinit(&timer0_obj);
			led_turn_off(&cond_led);//turn off the led of air conditioning
			break;//break the switch case
			case TV_TURN_OFF:
			led_turn_off(&tv_led);//turn off the led of the TV
			break;//break the switch case
			
			/*********************************   Set temperature   ********************************/
			case SET_TEMPERATURE:
			SPI_Read_Byte(&spi_slave,&required_temperature);//get the temperature from the master and store the temperature in required temperature 
			break;//break the switch case
		}
	}
   

    return (EXIT_SUCCESS);
}

void ISRs(void)
{
	counter++;//count the ticks of the timer zero
	if(counter>=10)//do that code every 10 ticks 
	{
		counter=0;//clear the counter of ticks
        adc_get_conversion_result(&adc_temp , &temp_sensor_reading);
		temp_sensor_reading=0.25*temp_sensor_reading;//read the temperature from the temperature sensor connected to the ADC of the micro controller
		if (temp_sensor_reading>=(required_temperature+1))//do that code if the read temperature if greater than required temperature by one or more
		{
			led_turn_on(&cond_led);//turn on the led of the air conditioning
			last_air_conditioning_value=AIR_CONDTIONING_ON;//save the value of the state of the air conditioning
		}
		else if (temp_sensor_reading<=(required_temperature-1))//do that code if the read temperature if lesser than required temperature by one or more
		{
			led_turn_off(&cond_led);//turn off the led of the air conditioning
			last_air_conditioning_value=AIR_CONDTIONING_OFF;//save the value of the state of the air conditioning
		}
		else if (required_temperature==temp_sensor_reading)//do that code if the read temperature is equal to the required temperature
		{
			if (last_air_conditioning_value==AIR_CONDTIONING_ON)//in the case of the last saved status of the air conditioning was on 
			{
				led_turn_on(&cond_led);//turn on the led of the air conditioning
			}
			else if (last_air_conditioning_value==AIR_CONDTIONING_OFF)//in the case of the last saved status of the air conditioning was off 
			{
				led_turn_off(&cond_led);//turn off the led of the air conditioning
			}
		}
	}
}
