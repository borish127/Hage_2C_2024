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
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/
#define IN 0
#define OUT 1
/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/

} gpioConf_t;
/*==================[internal functions declaration]=========================*/
void setStates(uint8_t bcd_digit, gpioConf_t *b)
{
	for (int i=0;i<4;i++){
		GPIOState(b[i].pin,bcd_digit&1);
		bcd_digit=bcd_digit>>1;
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t bcd_digit=5;
	gpioConf_t b[4];
	b[0].pin=GPIO_20;
	b[1].pin=GPIO_21;
	b[2].pin=GPIO_22;
	b[3].pin=GPIO_23;
	for (int i=0;i<4;i++){
		b[i].dir=OUT;
	}
	setStates(bcd_digit,&b);
	//printf("Hello world!\n");
}
/*==================[end of file]============================================*/