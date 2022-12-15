
#include "application.h"
void ISR(void);
volatile uint16 session_counter = 0;//indicate session time
uint8 timeout_flag = FALSE;//stores if the session is still valid or outdated


int main() {
   Std_ReturnType ret = E_NOT_OK;
    uint8 temperature = 0;//The average temperature of the room
	uint8 temp_ones = NOT_SELECTED;//The entered right number of the temperature
	uint8 temp_tens = NOT_SELECTED;//The entered left number of the temperature
	
	uint8 login_mode = NO_MODE; //Store the current user mode admin or guest or not logged in
	uint8 block_mode_flag = FALSE;//is true if the login is blocked or false if not blocked
	uint8 key_pressed = KEY_RELEASED;//
	/*****************  INITIALIZE  ***********************/
    led_t admin_led = {.port_name = PORTC_INDEX, .pin = GPIO_PIN0, .led_status = GPIO_LOW};
    led_t guest_led = {.port_name = PORTC_INDEX, .pin = GPIO_PIN1, .led_status = GPIO_LOW};
    led_t block_led = {.port_name = PORTC_INDEX, .pin = GPIO_PIN2, .led_status = GPIO_LOW};
    chr_lcd_8bit_t lcd_user = {
      .lcd_en.direction = GPIO_DIRECTION_OUTPUT ,
      .lcd_en.logic = GPIO_LOW , 
      .lcd_en.port = PORTB_INDEX , 
      .lcd_en.pin = GPIO_PIN0,
      
      .lcd_rs.direction = GPIO_DIRECTION_OUTPUT , 
      .lcd_rs.logic = GPIO_LOW , 
      .lcd_rs.port = PORTB_INDEX , 
      .lcd_rs.pin = GPIO_PIN1,
      
      .lcd_data[0].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[0].logic = GPIO_LOW , .lcd_data[0].port = PORTA_INDEX , .lcd_data[0].pin = GPIO_PIN0,
      .lcd_data[1].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[1].logic = GPIO_LOW , .lcd_data[1].port = PORTA_INDEX , .lcd_data[1].pin = GPIO_PIN1,
      .lcd_data[2].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[2].logic = GPIO_LOW , .lcd_data[2].port = PORTA_INDEX , .lcd_data[2].pin = GPIO_PIN2,
      .lcd_data[3].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[3].logic = GPIO_LOW , .lcd_data[3].port = PORTA_INDEX , .lcd_data[3].pin = GPIO_PIN3,
      .lcd_data[4].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[4].logic = GPIO_LOW , .lcd_data[4].port = PORTC_INDEX , .lcd_data[4].pin = GPIO_PIN7,
      .lcd_data[5].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[5].logic = GPIO_LOW , .lcd_data[5].port = PORTA_INDEX , .lcd_data[5].pin = GPIO_PIN5,
      .lcd_data[6].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[6].logic = GPIO_LOW , .lcd_data[6].port = PORTB_INDEX , .lcd_data[6].pin = GPIO_PIN2,
      .lcd_data[7].direction = GPIO_DIRECTION_OUTPUT , .lcd_data[7].logic = GPIO_LOW , .lcd_data[7].port = PORTB_INDEX , .lcd_data[7].pin = GPIO_PIN3,
    };
    keypad_t keypad_user = {
    .keypad_row_pins[0].direction=GPIO_DIRECTION_OUTPUT,
    .keypad_row_pins[0].logic=GPIO_LOW,
    .keypad_row_pins[0].port=PORTD_INDEX,
    .keypad_row_pins[0].pin = GPIO_PIN0,
    .keypad_row_pins[1].direction=GPIO_DIRECTION_OUTPUT,
    .keypad_row_pins[1].logic=GPIO_LOW,
    .keypad_row_pins[1].port=PORTD_INDEX,
    .keypad_row_pins[1].pin = GPIO_PIN1,
    .keypad_row_pins[2].direction=GPIO_DIRECTION_OUTPUT,
    .keypad_row_pins[2].logic=GPIO_LOW,
    .keypad_row_pins[2].port=PORTD_INDEX,
    .keypad_row_pins[2].pin = GPIO_PIN2,
    .keypad_row_pins[3].direction=GPIO_DIRECTION_OUTPUT,
    .keypad_row_pins[3].logic=GPIO_LOW,
    .keypad_row_pins[3].port=PORTD_INDEX,
    .keypad_row_pins[3].pin = GPIO_PIN3,

    .keypad_col_pins[0].direction = GPIO_DIRECTION_INPUT,
    .keypad_col_pins[0].logic=GPIO_LOW,
    .keypad_col_pins[0].port=PORTD_INDEX,
    .keypad_col_pins[0].pin = GPIO_PIN4,
    .keypad_col_pins[1].direction = GPIO_DIRECTION_INPUT,
    .keypad_col_pins[1].logic=GPIO_LOW,
    .keypad_col_pins[1].port=PORTD_INDEX,
    .keypad_col_pins[1].pin = GPIO_PIN5,
    .keypad_col_pins[2].direction = GPIO_DIRECTION_INPUT,
    .keypad_col_pins[2].logic=GPIO_LOW,
    .keypad_col_pins[2].port=PORTD_INDEX,
    .keypad_col_pins[2].pin = GPIO_PIN6,
    .keypad_col_pins[3].direction = GPIO_DIRECTION_INPUT,
    .keypad_col_pins[3].logic=GPIO_LOW,
    .keypad_col_pins[3].port=PORTD_INDEX,
    .keypad_col_pins[3].pin = GPIO_PIN7,
    };
    SPI_Config spi_mcu = {
    .spi_config.ClockPolarity = SPI_TRANSMIT_ACTIVE_TO_IDLE,
    .spi_config.ClockSelect = SPI_IDLE_STATE_HIGH_LEVEL,
    .spi_config.SampleSelect = SPI_DATA_SAMPLE_MIDDLE,
    .spi_mode = SPI_MASTER_FOSC_DIV4,
    };
    timer0_t tmr0 = {
      .timer0_counter_edge = TIMER0_RISING_EDGE_CFG,
      .timer0_mode = TIMER0_TIMER_MODE,
      .timer0_preload_value = 3036,
      .timer0_register_size = TIMER0_16BIT_MODE,
      .timer0_prescaler_enable = TIMER0_PRESCALER_DISABLE_CFG,
      .timer0_InterruptHandler  = ISR,
      
      
    };
    led_initialize(&admin_led);//initializes the led of admin
    led_initialize(&guest_led);//initializes the led of guest
    led_initialize(&block_led);//initializes the led of block
    lcd_8bit_initialize(&lcd_user);//initializes the LCD screen
    keypad_initialize(&keypad_user);//initializes the keypad
    SPI_Init(&spi_mcu);//initializes the communication protocol of SPI
	
	/******************************************************/
	/* Printing Welcome screen */
	lcd_8bit_send_string(&lcd_user ,"Welcome to smart");
	lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"home system");
	__delay_ms(1000);//Halt the system for the given time in (ms)
	lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
	/***************************/
	/*Setting Admin and Guest passwords if not set */
	//read the state of the the passwords of the admin and guest if both are set or not set
    uint8 admin_pass_stat , guest_pass_stat;
    Data_EEPROM_ReadByte(ADMIN_PASS_STATUS_ADDRESS, &admin_pass_stat);
    Data_EEPROM_ReadByte(GUEST_PASS_STATUS_ADDRESS, &guest_pass_stat);
	if ( (admin_pass_stat!=PASS_SET) || (guest_pass_stat !=PASS_SET )){
	
		lcd_8bit_send_string(&lcd_user , "Login for");//printing login menu
		lcd_8bit_send_char_str_pos(&lcd_user,2,1,"first time");
		__delay_ms(1000);//Halt the system for the given time in (ms)
		lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
		lcd_8bit_send_string(&lcd_user,"Set Admin pass");//printing the set admin password menu
		lcd_8bit_send_char_str_pos(&lcd_user,2,1,"Admin pass:");

		/********************************* setting Admin password **********************************************/
		uint8 pass_counter=0;//the counter of the characters of the password
		uint8 pass[PASS_SIZE]={NOT_STORED,NOT_STORED,NOT_STORED,NOT_STORED};//the array where it stored the password
		while (pass_counter<PASS_SIZE)//loop till the user finish inserting the pass
		{
			key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
			while (key_pressed == KEY_RELEASED)//repeat till the user press any key
			{
				keypad_get_value(&keypad_user , &key_pressed);//if the user pressed any button in keypad save the value in key_pressed
			}

			pass[pass_counter]=key_pressed;//add the entered character to the pass array
			lcd_8bit_send_char_data(&lcd_user , key_pressed);//print the entered character
			__delay_ms(CHARACTER_PREVIEW_TIME);//Halt the system for the given time in (ms)
			//move the lcd cursor to the previous location to write the password symbol over the character
			lcd_8bit_send_char_data_pos(&lcd_user ,2,12+ pass_counter ,PASSWORD_SYMBOL); // to display (Password sign *)
			__delay_ms(100);//Halt the system for the given time in (ms)
			pass_counter++;//increase the characters count
		}
		Data_EEPROM_WriteByte(EEPROM_ADMIN_ADDRESS, pass[0]);//save the entire password as a block to the EEPROM
        Data_EEPROM_WriteByte(EEPROM_ADMIN_ADDRESS + 1, pass[1]);
        Data_EEPROM_WriteByte(EEPROM_ADMIN_ADDRESS + 2, pass[2]);
        Data_EEPROM_WriteByte(EEPROM_ADMIN_ADDRESS + 3, pass[3]);
		Data_EEPROM_WriteByte(ADMIN_PASS_STATUS_ADDRESS,PASS_SET);//write the status of pass as it is set
		lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
		lcd_8bit_send_string(&lcd_user,"Pass Saved");// show pass saved message
		__delay_ms(500);//Halt the system for the given time in (ms)
		lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row

		/********************************* setting guest password **********************************************/
		pass_counter=0;//reset password counter which count the characters of the pass
		lcd_8bit_send_string(&lcd_user,"Set Guest Pass");//printing the set admin password menu
		lcd_8bit_send_char_str_pos(&lcd_user,2,1,"Guest Pass:");
		while (pass_counter<PASS_SIZE)//loop till the user finish inserting the pass
		{
			key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
			while (key_pressed == KEY_RELEASED)//repeat till the user press any key
			{
				keypad_get_value(&keypad_user , &key_pressed);//if the user pressed any button in keypad save the value in key_pressed
			}

			pass[pass_counter]=key_pressed;//add the entered character to the pass array
			lcd_8bit_send_char_data(&lcd_user , key_pressed);//print the entered character
			__delay_ms(CHARACTER_PREVIEW_TIME);//Halt the system for the given time in (ms)
			//move the lcd cursor to the previous location to write the password symbol over the character
			lcd_8bit_send_char_data_pos(&lcd_user ,2,12+ pass_counter ,PASSWORD_SYMBOL); // to display (Password sign *)
			__delay_ms(100);//Halt the system for the given time in (ms)
			pass_counter++;//increase the characters count
		}
        Data_EEPROM_WriteByte(EEPROM_GUEST_ADDRESS, pass[0]);//save the entire password as a block to the EEPROM
        Data_EEPROM_WriteByte(EEPROM_GUEST_ADDRESS + 1, pass[1]);
        Data_EEPROM_WriteByte(EEPROM_GUEST_ADDRESS + 2, pass[2]);
        Data_EEPROM_WriteByte(EEPROM_GUEST_ADDRESS + 3, pass[3]);
        Data_EEPROM_WriteByte(GUEST_PASS_STATUS_ADDRESS,PASS_SET);//write the status of pass as it is set
		lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
		lcd_8bit_send_string(&lcd_user,"Pass Saved");//move the lcd cursor to the previous location
		__delay_ms(500);//Halt the system for the given time in (ms)
		lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
		Data_EEPROM_WriteByte(LOGIN_BLOCKED_ADDRESS, FALSE);
	}//The end of if admin and guest password is set
	else{//this code of else run only if the system is not running for the first time (ADMIN and GUEST passwords are set)
	
		block_mode_flag = Data_EEPROM_ReadByte(LOGIN_BLOCKED_ADDRESS, &block_mode_flag); //read the blocked location from EEPROM
	}
    
   while(1){
        key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
		uint8 pass_tries_count=0;//stores how many times the user tried to log in to the system and failed
		
		if ( timeout_flag==TRUE )//check for timeout
		{//if timeout flag was raised
			timer0_deinit(&tmr0);//stop the timer that increase the session counter
			session_counter = 0;//clear session counter
			timeout_flag=FALSE;//clear time out flag
			login_mode=NO_MODE;//log the user out
			key_pressed = KEY_RELEASED;//;//clear the key_pressed to avoid unwanted selection in the menu switch
			led_turn_off(&guest_led);//turnoff the led of the guest
			led_turn_off(&admin_led);//turnoff the led of the admin
			lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
			lcd_8bit_send_string(&lcd_user,"Session Timeout");//print session timeout message
			__delay_ms(1000);//Halt the system for the given time in (ms)
		}
		while (login_mode==NO_MODE)//The user can only leave the loop only in case of he was logged in as guest or admin
		{
			if(block_mode_flag==TRUE)//if the login process was blocked wait till the end of the block period
			{
				lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
				lcd_8bit_send_string(&lcd_user,"Login blocked");
				lcd_8bit_send_char_str_pos(&lcd_user,2,1,"wait 20 seconds");
				led_turn_on(&block_led);//Turn on the led of Blocked
				__delay_ms(BLOCK_MODE_TIME);//Halt the system for the given time in (ms)
				pass_tries_count = 0; //Clear the count on number of wrong tries
				block_mode_flag = FALSE;//Disable block of runtime
				led_turn_off(&block_led);//Turn off the led of Blocked
				Data_EEPROM_WriteByte(LOGIN_BLOCKED_ADDRESS,FALSE);//write false at blocked location in EEPROM
			}
			lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
            lcd_8bit_send_string(&lcd_user,"Select mode :");
			lcd_8bit_send_char_str_pos(&lcd_user,2,1,"0:Admin 1:Guest");
			while(key_pressed==KEY_RELEASED)//wait for the selection of the mode
			{
				 keypad_get_value(&keypad_user , &key_pressed);//if the user pressed any button in keypad save the value in key_pressed
			}
			if ( key_pressed!=CHECK_ADMIN_MODE && key_pressed!=CHECK_GUEST_MODE )
			{
				lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
				lcd_8bit_send_string(&lcd_user,"Wrong input.");//Prints error message on the LCD
				key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
				__delay_ms(1000);//Halt the system for the given time in (ms)
				continue;//return to the loop of login #while (login_mode==NO_MODE)# line 128
			}

			uint8 pass_counter=0;//counts the entered key of the password from the keypad
			uint8 pass[PASS_SIZE]={NOT_STORED,NOT_STORED,NOT_STORED,NOT_STORED};//temporarily hold the entire password that will be entered by the user to be check
			uint8 stored_pass[PASS_SIZE]={NOT_STORED,NOT_STORED,NOT_STORED,NOT_STORED};//temporarily hold the entire stored password that is written by the user for the first time
			
			switch(key_pressed)
			{
				/********************************* Admin login **********************************************/
				case CHECK_ADMIN_MODE:
				while(login_mode!=ADMIN)//this loop is to repeat the login for admin in case of wrong password
				{
					key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
					lcd_8bit_send_command(&lcd_user , LCD_CLEAR);;//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					lcd_8bit_send_string(&lcd_user,"Admin mode");
					lcd_8bit_send_char_str_pos(&lcd_user,2,1,"Enter Pass:");
					__delay_ms(200);//Halt the system for the given time in (ms)
					pass_counter=0;//counts the number of entered characters
					while(pass_counter<PASS_SIZE)
					{
						while (key_pressed == KEY_RELEASED)//repeat till the user press any key
						{
							 keypad_get_value(&keypad_user , &key_pressed);//if the user pressed any button in keypad save the value in key_pressed
						}
						pass[pass_counter]=key_pressed;//add the entered character to the pass array
						lcd_8bit_send_char_data(&lcd_user,key_pressed);//print the entered character
						__delay_ms(CHARACTER_PREVIEW_TIME);//Halt the system for the given time in (ms)
						//move the cursor of the lcd screen to the previous location
						lcd_8bit_send_char_data_pos(&lcd_user, 2 ,12+pass_counter , PASSWORD_SYMBOL);// to display (Password sign *)
						__delay_ms(100);//Halt the system for the given time in (ms)
						pass_counter++;//increase the password counter that count the characters of the pass
						key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
					}
					//read the stored pass from the EEPROM
                    Data_EEPROM_ReadByte(EEPROM_ADMIN_ADDRESS , &stored_pass[0]);
                    Data_EEPROM_ReadByte(EEPROM_ADMIN_ADDRESS+1 , &stored_pass[1]);
                    Data_EEPROM_ReadByte(EEPROM_ADMIN_ADDRESS+2 , &stored_pass[2]);
                    Data_EEPROM_ReadByte(EEPROM_ADMIN_ADDRESS+3, &stored_pass[3]);

					/*compare passwords*/
					if ((ui8ComparePass(pass,stored_pass,PASS_SIZE)) == TRUE)//in case of right password
					{
						login_mode = ADMIN;//set the login mode to admin mode
						pass_tries_count=0;//clear the counter of wrong tries
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user,"Right pass");
						lcd_8bit_send_char_str_pos(&lcd_user, 2, 1, "Admin mode");
						__delay_ms(500);//Halt the system for the given time in (ms)
						led_turn_on(&admin_led);//turn on the led of admin
						timer0_init(&tmr0);//start the timer that counts the session time
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					}
					else//in case of wrong password
					{
						pass_tries_count++;//increase the number of wrong tries to block login if it exceeds the allowed tries
						login_mode = NO_MODE;//set the mode as not logged in
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user, "Wrong Pass");
						lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"Tries left:");
						lcd_8bit_send_char_data(&lcd_user , TRIES_ALLOWED-pass_tries_count+ASCII_ZERO);//print the number of tries left before block mode to be activated
						__delay_ms(1000);//Halt the system for the given time in (ms)
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						if (pass_tries_count>=TRIES_ALLOWED)//if the condition of the block mode is true
						{
							Data_EEPROM_WriteByte(LOGIN_BLOCKED_ADDRESS,TRUE);//write to the EEPROM TRUE to the the block mode address
							block_mode_flag = TRUE;//turn on block mode
							break;//break the loop of admin login #while(login_mode!=ADMIN)# at line 169
						}
					}
				}
				break;//bREAK SWITCH case
				/********************************* Guest login **********************************************/
				case  CHECK_GUEST_MODE:
				while(login_mode != GUEST)
				{
					key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
					lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					lcd_8bit_send_string(&lcd_user,"Guest mode");
					lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"Enter pass:");
					__delay_ms(200);//Halt the system for the given time in (ms)
					pass_counter=0;//counts the number of entered characters
					while(pass_counter<PASS_SIZE)
					{
						while (key_pressed == KEY_RELEASED)//repeat till the user press any key
						{
							keypad_get_value(&keypad_user , &key_pressed);//if the user pressed any button in keypad save the value in key_pressed
						}
						pass[pass_counter]=key_pressed;//add the pressed key to the password string
						lcd_8bit_send_char_data(&lcd_user , key_pressed);//print the entered character
						__delay_ms(CHARACTER_PREVIEW_TIME);//Halt the system for the given time in (ms)
						//return the cursor to the location of the previous character to replace it with the password sign
						lcd_8bit_send_char_data_pos(&lcd_user, 2, 12+pass_counter,PASSWORD_SYMBOL); // to display (Password sign *)
						__delay_ms(100);//Halt the system for the given time in (ms)
						pass_counter++;//increase the password counter that count the characters of the pass
						key_pressed = KEY_RELEASED;//return the variable that holds the pressed key from keypad to its initial value
					}
					//Save the entire password in the EEPROM
                    Data_EEPROM_ReadByte(EEPROM_GUEST_ADDRESS , &stored_pass[0]);
                    Data_EEPROM_ReadByte(EEPROM_GUEST_ADDRESS+1 , &stored_pass[1]);
                    Data_EEPROM_ReadByte(EEPROM_GUEST_ADDRESS+2 , &stored_pass[2]);
                    Data_EEPROM_ReadByte(EEPROM_GUEST_ADDRESS+3, &stored_pass[3]);
					/*compare passwords*/
					if (ui8ComparePass(pass,stored_pass,PASS_SIZE)==TRUE)//in case of right password
					{
						login_mode = GUEST;
						pass_tries_count=0;//clear the counter of wrong tries
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					    lcd_8bit_send_string(&lcd_user,"Right pass");
						lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"Guest mode");
						__delay_ms(500);//Halt the system for the given time in (ms)
						led_turn_on(&guest_led);//turn the led of gust mode that is connected to the master micro controller on
						timer0_init(&tmr0);//start the counter of the session
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					}
					else//in case of wrong password
					{
						pass_tries_count++;//increase the number of wrong tries to block login if it exceeds the allowed tries
						login_mode = NO_MODE;//set the mode as not logged in
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user,"Wrong pass");
						lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"Tries left:");
						lcd_8bit_send_char_data(&lcd_user ,TRIES_ALLOWED-pass_tries_count+ASCII_ZERO);//print the number of tries left before block mode to be activated
						__delay_ms(1000);//Halt the system for the given time in (ms)
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						if (pass_tries_count>=TRIES_ALLOWED)//if the condition of the block mode is true
						{
							Data_EEPROM_WriteByte(LOGIN_BLOCKED_ADDRESS,TRUE);//write to the EEPROM TRUE to the the block mode address
							block_mode_flag = TRUE;//turn on block mode
							break;//breaks the loop of insert guest password #while(login_mode != GUEST)# line 228
						}//end of if that check if the number of tries exceeds the maximum tries allowed
					}//end of the case of wrong password
				}//end of loop of guest login
				break;//break for CHECK_GUEST_MODE switch case
			}//end of switch
			
		}
		
		/*************************************************************************************************/
		uint8 show_menu = MAIN_MENU;
		
		
		while(timeout_flag!=TRUE)//Show the menu in case of the time is not out
		{
			key_pressed = KEY_RELEASED;//Set the key pressed by the user to its default value
			switch (show_menu)
			{
				case MAIN_MENU:
				do
				{
					/******************** print main Menu ******************/
					lcd_8bit_send_command(&lcd_user , LCD_CLEAR);
					lcd_8bit_send_string(&lcd_user,"1:Room1 2:Room2");
					if(login_mode==ADMIN)//check login mode
					{
						lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"3:Room3 4:More ");//this menu options only printed if the logged in user is an admin
					}
					else if(login_mode==GUEST)//check login mode
					{
						lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"3:Room3 4:Room4");//this menu options only printed if the logged in user is a guest
					}
					/*******************************************************/
					
					key_pressed = u8GetKeyPressed(login_mode, &keypad_user);//wait for the user till key is pressed or the time is out
					__delay_ms(100);//to avoid the duplication of the pressed key
					
					if (key_pressed == SELECT_ROOM1)//If key pressed is 1
					{
						show_menu = ROOM1_MENU;//Set the next menu to be shown to room1 menu
					}
					else if (key_pressed == SELECT_ROOM2)//If key pressed is 2
					{
						show_menu = ROOM2_MENU;//Set the next menu to be shown to room2 menu
					}
					else if (key_pressed == SELECT_ROOM3)//If key pressed is 3
					{
						show_menu = ROOM3_MENU;//Set the next menu to be shown to room3 menu
					}
					else if (key_pressed == SELECT_ROOM4 && login_mode == GUEST)//If key pressed is 4 and the logged in user is guest
					{
						show_menu = ROOM4_MENU;//Set the next menu to be shown to room4 menu
					}
					else if (key_pressed == ADMIN_MORE_OPTION && login_mode == ADMIN)//If key pressed is 4 and the logged in user is admin
					{
						show_menu = MORE_MENU;//Set the next menu to be shown to more menu
					}
					else if(key_pressed != KEY_RELEASED)//show wrong input message if the user pressed wrong key
					{
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user,"Wrong input");//print error message
						__delay_ms(500);//Halt the system for the given time in (ms)
					}
				} while ( ((key_pressed < '1') || (key_pressed > '4') ) && (timeout_flag == FALSE) );//break the loop in case of valid key or time is out
				
				break;//End of main menu case
				
				case MORE_MENU:
				do
				{
					/******************** print more Menu ******************/
					lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					lcd_8bit_send_string(&lcd_user , "1:Room4    2:TV   ");
					lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"3:Air Cond.4:RET");
					/*******************************************************/
					key_pressed = u8GetKeyPressed(login_mode, &keypad_user);//wait for the user till key is pressed or the time is out
					__delay_ms(100);//to avoid the duplication of the pressed key
					
					if (key_pressed == SELECT_ROOM4_ADMIN)//If key pressed is 1
					{
						show_menu = ROOM4_MENU;//Set the next menu to be shown to room4 menu
					}
					else if (key_pressed == SELECT_TV)//If key pressed is 2
					{
						show_menu = TV_MENU;//Set the next menu to be shown to TV menu
					}
					else if (key_pressed == SELECT_AIR_CONDITIONING)//If key pressed is 3
					{ 
						show_menu = AIRCONDITIONING_MENU;//Set the next menu to be shown to Air conditioning menu
					}
					else if (key_pressed == ADMIN_RET_OPTION)//If key pressed is 4 (RET)
					{
						show_menu = MAIN_MENU;//Set the next menu to be shown to main menu
					}
					else if(key_pressed != KEY_RELEASED)//show wrong input message if the user pressed wrong key
					{
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user ,"Wrong input");//print error message
						__delay_ms(500);//Halt the system for the given time in (ms)
					}
				} while (( (key_pressed < '1') || (key_pressed > '4') ) && (timeout_flag == FALSE));//break the loop in case of valid key or time is out
				
				break;//End of more menu case
				
				case AIRCONDITIONING_MENU:
				do
				{
					/******************** print more Menu ******************/
					lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					lcd_8bit_send_string(&lcd_user ,"1:Set temperature ");
					lcd_8bit_send_char_str_pos(&lcd_user, 2, 1,"2:Control  0:RET");
					/*******************************************************/
					key_pressed = u8GetKeyPressed(login_mode, &keypad_user);//wait for the user till key is pressed or the time is out
					__delay_ms(100);//to avoid the duplication of the pressed key
					
					if (key_pressed == SELECT_SET_TEMPERATURE)//If key pressed is 1
					{
						show_menu = TEMPERATURE_MENU;//Set the next menu to be shown to set temperature menu
					}
					else if (key_pressed == SELECT_AIR_COND_CTRL)//If key pressed is 2
					{
						show_menu = AIRCOND_CTRL_MENU;//Set the next menu to be shown to air conditioning control menu
					}
					else if (key_pressed == SELECT_AIR_COND_RET)//If key pressed is 0
					{
						show_menu = MORE_MENU;//Set the next menu to be shown to more menu
					}
					else if(key_pressed != KEY_RELEASED)//show wrong input message if the user pressed wrong key
					{
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user ,"Wrong input");//print error message
						__delay_ms(500);//Halt the system for the given time in (ms)
					}
				} while (( (key_pressed < '0') || (key_pressed > '2') ) && (timeout_flag == FALSE));//break the loop in case of valid key or time is out
				break;//End of air conditioning menu case
				
				case ROOM1_MENU:
				vMenuOption(ROOM1_MENU,login_mode,&lcd_user , &spi_mcu , &keypad_user);//call the function that show the menu of room 1
				show_menu = MAIN_MENU;//Set the next menu to be shown to main menu
				break;//End of room1 menu case
				
				case ROOM2_MENU:
				vMenuOption(ROOM2_MENU,login_mode,&lcd_user , &spi_mcu , &keypad_user);//call the function that show the menu of room 2
				show_menu = MAIN_MENU;//Set the next menu to be shown to main menu
				break;//End of room2 menu case
				
				case ROOM3_MENU:
				vMenuOption(ROOM3_MENU,login_mode,&lcd_user , &spi_mcu , &keypad_user);//call the function that show the menu of room 3
				show_menu = MAIN_MENU;//Set the next menu to be shown to main menu
				break;//End of room3 menu case
				
				case ROOM4_MENU:
				vMenuOption(ROOM4_MENU,login_mode,&lcd_user , &spi_mcu , &keypad_user);//call the function that show the menu of room 4
				if (login_mode == GUEST)//in case of guest is logged in
				{
					show_menu = MAIN_MENU;//Set the next menu to be shown to main menu
				}
				else//in case of admin is logged in
				{
					show_menu = MORE_MENU;//Set the next menu to be shown to more menu
				}
				break;//End of room4 menu case
				
				case TV_MENU:
				vMenuOption(TV_MENU,login_mode ,&lcd_user , &spi_mcu , &keypad_user);//call the function that show the menu of tv
				show_menu = MORE_MENU;//Set the next menu to be shown to more menu
				break;//End of TV menu case
				
				case AIRCOND_CTRL_MENU:
				vMenuOption(AIRCOND_CTRL_MENU,login_mode ,&lcd_user , &spi_mcu , &keypad_user);//call the function that show the menu of Air conditioning control
				show_menu = AIRCONDITIONING_MENU;//Set the next menu to be shown to air conditioning menu
				break;//End of air conditioning control menu case
				
				case TEMPERATURE_MENU:
				temperature = 0;//clear the value of temperature
				while (temperature==0 && timeout_flag == FALSE)//start the loop that asks for the temperature from the user in case of the time is not out
				{
					key_pressed = KEY_RELEASED;//set the key pressed to the default value
					lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					lcd_8bit_send_string(&lcd_user ,"Set temp.:__ ");//print the format of inserting temperature
					lcd_8bit_send_char_data(&lcd_user ,DEGREES_SYMBOL); // print the symbol of degree
					lcd_8bit_send_char_data(&lcd_user ,'C'); // print the C character
					lcd_8bit_send_char_str_pos(&lcd_user, 1, 1,"Set temp.: ");//move the cursor to the place to write the entered temperature
					__delay_ms(200);//Halt the system for the given time in (ms)
					/*******************************************************************************/
					key_pressed = u8GetKeyPressed(login_mode, &keypad_user);//wait for the user till key is pressed or the time is out
					__delay_ms(250);//to avoid the duplication of the pressed key

					if (timeout_flag == TRUE) //in case of the time is out before the user press a key
					{
						break;//break the loop that ask for temperature
					}
					if (key_pressed <'0' || key_pressed >'9')//show wrong input message if the user entered non numeric value
					{
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user ,"Wrong input");//print error message
						__delay_ms(500);//Halt the system for the given time in (ms)
						continue;//return to #while (temperature==0)# line 672
					}
					else//if the value is valid
					{
						lcd_8bit_send_char_data(&lcd_user , key_pressed);//print the value on the lcd
						temp_tens = key_pressed-ASCII_ZERO;//save the entered value
						key_pressed = KEY_RELEASED;//set the key pressed to the default value
					}
					/*******************************************************************************/
					key_pressed = u8GetKeyPressed(login_mode , &keypad_user);//wait for the user till key is pressed or the time is out
					__delay_ms(250);//to avoid the duplication of the pressed key
					
					if (timeout_flag == TRUE)//if the user session is timeout
					{
						break;//break the loop that ask for temperature
					}
					if ((key_pressed <'0' || key_pressed >'9'))//show wrong input message if the user entered non numeric value
					{
						lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
						lcd_8bit_send_string(&lcd_user ,"Wrong input");//print error message
						__delay_ms(500);//Halt the system for the given time in (ms)
						continue;//repeat the loop that ask for the temperature
					}
					else//if the value is valid
					{
						lcd_8bit_send_char_data(&lcd_user , key_pressed);//print the value on the lcd
						temp_ones = key_pressed-ASCII_ZERO;//save the entered value
						key_pressed = KEY_RELEASED;//set the key pressed to the default value
					}
					temperature = temp_tens*10 + temp_ones;//set the value of the temperature from the given separated values
					SPI_Send_Byte(&spi_mcu , SET_TEMPERATURE);//Send the code of set temperature
					__delay_ms(200);//Halt the system to prevent write collision
					SPI_Send_Byte(&spi_mcu ,temperature);//send the entered temperature
					lcd_8bit_send_command(&lcd_user , LCD_CLEAR);//remove all previously printed characters on the LCD and move the cursor to the first column of the first row
					lcd_8bit_send_string(&lcd_user ,"Temperature Sent");//show the message
					__delay_ms(500);//Halt the system for the given time in (ms)
				}
				show_menu = AIRCONDITIONING_MENU;//Set the next menu to be shown to air conditioning menu
				break;//break from switch
			}//End of the switch
		}//End of while that repeats the menu after each successful action till session timeout
   }

    return (EXIT_SUCCESS);
}



void ISR(void)
{
	session_counter++;//increase the indicator of session time for every tick
}