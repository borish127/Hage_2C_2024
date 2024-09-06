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

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void converToBcdArray (uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for(int i=digits-1;i>=0;i--){
		bcd_number[i]=data%10;
		data=data/10;
	}
}
void showDigits (uint8_t digits, uint8_t *bcd_number)
{
	for(int i=0;i<digits;i++){
		printf("%d",bcd_number[i]);
	}
}

/*==================[external functions definition]==========================*/

void app_main(void){
	uint32_t data=527;
	uint8_t digits=3;
	uint8_t bcd_number[digits];
	converToBcdArray(data,digits,&bcd_number);
	showDigits(digits,&bcd_number);
	
	// printf("Hello world!\n");
}
/*==================[end of file]============================================*/