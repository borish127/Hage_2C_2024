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
void setStates(uint8_t bcd_digit, gpioConf_t *b)
{
	for (int i=0;i<4;i++){
		GPIOState(b[i].pin,bcd_digit&1);
		bcd_digit=bcd_digit>>1;
	}
}
void ShowNumbs(uint32_t dato, uint8_t digits, gpioConf_t *b, gpioConf_t *d)
{
	uint8_t bcd_number[digits];
	converToBcdArray(dato,digits,&bcd_number);
	//showDigits(digits,&bcd_number);
	for (int i=0;i<digits;i++){
		setStates(bcd_number[i],b);
		GPIOOn(d[i].pin);
		GPIOOff(d[i].pin);
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){
	uint32_t dato=645;
	uint8_t digits=3;
	gpioConf_t b[4];
	gpioConf_t d[3];
	b[0].pin=GPIO_20;
	b[1].pin=GPIO_21;
	b[2].pin=GPIO_22;
	b[3].pin=GPIO_23;
	for (int i=0;i<4;i++){
		b[i].dir=OUT;
	}
	d[0].pin=GPIO_19;
	d[1].pin=GPIO_18;
	d[2].pin=GPIO_9;
	for (int i=0;i<3;i++){
		d[i].dir=OUT;
	}
	for (int i=0;i<4;i++){
		GPIOInit(b[i].pin,b[i].dir);
		GPIOOff(b[i].pin);
	}
	for (int i=0;i<3;i++){
		GPIOInit(d[i].pin,d[i].dir);
		GPIOOff(d[i].pin);
	}
	ShowNumbs(dato,digits,&b,&d);
	//printf("Hello world!\n");
}
/*==================[end of file]============================================*/