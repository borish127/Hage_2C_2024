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
#include "hc_sr04.h"
#include "lcditse0803.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
uint8_t read_distance(void)
{
	return HcSr04ReadDistanceInCentimeters();
}

uint8_t create_intervals(uint8_t distance)
{
	uint8_t interval;
	if(distance<10){
		interval=0;
	}
	if((distance>=10)&&(distance<20)){
		interval=1;
	}
	if((distance>=20)&&(distance<30)){
		interval=2;
	}
	if(distance>=30){
		interval=3;
	}
	return interval;
}
void update_leds(uint8_t interval)
{
	switch(interval){
		case 0:
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		break;
		case 1:
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		break;
		case 2:
			LedOn(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);
		break;
		case 3:
			LedOn(LED_1);
			LedOn(LED_2);
			LedOn(LED_3);
		break;
	}
}

uint8_t count_digits(uint8_t number)
{
	uint8_t digits=0;
	while(number!=0){
		number=number>>1;
		digits=digits+1;
	}
	return digits;
}

void distance_task(void *pvParameter)
{	
	uint8_t distance;
	uint8_t digits;
	while(true){
		distance=read_distance();
		digits=2;
		update_leds(create_intervals(distance));
		LcdItsE0803Write(distance);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	HcSr04Init(GPIO_3,GPIO_2);
	LcdItsE0803Init();
	xTaskCreate(&distance_task,"Distance", 512, NULL, 5, NULL);	
}
/*==================[end of file]============================================*/