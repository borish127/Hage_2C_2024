/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
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
// #define ON 1
// #define OFF 0
// #define TOGGLE 2
enum mode{ON,OFF,TOGGLE};
#define BLINKING_PERIOD 500
#define GREEN LED_1
#define ORANGE LED_2
#define RED LED_3
/*==================[internal data definition]===============================*/
typedef struct
{
    	uint8_t mode;         //ON, OFF, TOGGLE
		uint8_t n_led;        //indica el número de led a controlar
		uint8_t n_ciclos;     //indica la cantidad de ciclos de ncendido/apagado
		uint16_t periodo;     //indica el tiempo de cada ciclo
} my_leds;

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
//void my_functions(my_leds *leds);
void my_function(my_leds *leds){
	switch (leds->mode)
	{
	case ON:
		LedOn(leds->n_led);
		break;
	case OFF:
		LedOff(leds->n_led);
		break;
	case TOGGLE:
		for(int i=0;i<leds->n_ciclos*2;i++){
			LedToggle(leds->n_led);
			vTaskDelay(leds->periodo/2);
		}
	}
}

void app_main(void){
	my_leds leds;
	LedsInit();
	SwitchesInit();
	leds.mode=TOGGLE;
	leds.n_led=GREEN;
	leds.n_ciclos=10;
	leds.periodo=100;
	my_function(&leds);	
}


/*==================[end of file]============================================*/