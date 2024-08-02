/*! @mainpage Blinking switch
 *
 * \section genDesc General Description
 *
 * This example makes LED_1 and LED_2 blink if SWITCH_1 or SWITCH_2 are pressed.
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 500
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t teclas;
	bool green=0;
	bool orange=0;
	bool red=0;
	LedsInit();
	SwitchesInit();
    while(1)    {
    	teclas  = SwitchesRead();
    	switch(teclas){
			case 0:
				if(green){
					LedToggle(LED_1);
					green=!green;
				}
				if(orange){
					LedToggle(LED_2);
					orange=!orange;
				}
				if(red){
					LedToggle(LED_3);
					red=!red;
				}
			break;
    		case SWITCH_1:
    			LedToggle(LED_1);
				green=!green;
				if(orange){
					LedToggle(LED_2);
					orange=!orange;
				}
				if(red){
					LedToggle(LED_3);
					red=!red;
				}
    		break;
    		case SWITCH_2:
    			LedToggle(LED_2);
				orange=!orange;
				if(green){
					LedToggle(LED_1);
					green=!green;
				}
				if(red){
					LedToggle(LED_3);
					red=!red;
				}
    		break;
			case SWITCH_1|SWITCH_2:
				LedToggle(LED_3);
				red=!red;
				if(orange){
					LedToggle(LED_2);
					orange=!orange;
				}
				if(green){
					LedToggle(LED_1);
					green=!green;
				}
			break;
    	}
	    //LedToggle(LED_3);
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}
