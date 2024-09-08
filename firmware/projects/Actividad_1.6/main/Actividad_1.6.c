/** @mainpage Guia 1 - Actividad 6
 * 	Proyecto que recibe un numero de 3 digitos y lo muestra en una pantalla LCD
 * 
 * @section genDesc General Description
 *
 *	La aplicacion recibe un numero de 3 digitos y separa cada digito en una posicion
 *  de un arreglo. Luego cada digito lo convierte a BCD y por ultimo carga cada digito
 * 	en cada posicion de la pantalla LCD.
 * 
 * @section hardConn Hardware Connection
 *
 * |    Pantalla LCD  |   ESP32   	|
 * |:---------------:|:--------------|
 * | 	PIN_0	 	 | 	 GPIO_20	|
 * | 	PIN_1	 	 | 	 GPIO_21	|
 * | 	PIN_2	 	 | 	 GPIO_22	|
 * | 	PIN_3	 	 | 	 GPIO_23	|
 * | 	PIN_0	 	 | 	 GPIO_19	|
 * | 	PIN_1	 	 | 	GPIO_18		|
 * | 	PIN_2	 	 | 	GPIO_9		|
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 23/08/2024 | Document creation		                         |
 * | 08/09/2024 | Documentation 		                         |
 *
 * @author Boris Hage (boris.hage@ingenieria.uner.edu.ar)
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
/** @def IN
 * @brief Indica que el pin es de lectura
 */
#define IN 0
/** @def OUT
 * @brief Indica que el pin es de escritura
 */
#define OUT 1
/*==================[internal data definition]===============================*/
/** @def gpioConf_t
 * @brief Estructura que almacena la direccion de cada pin y si es lectura o escritura
 */
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/

} gpioConf_t;
/*==================[internal functions declaration]=========================*/

/** @fn void converToBcdArray
 * @brief Recibe el numero y separa cada digito como BCD en un arreglo
 * @param data Es el numero que debe ser separado en digitos
 * @param digits Es la cantidad de digitos que posee el numero
 * @param bcd_number Es un puntero a un arreglo donde se almacenaran los digitos
 */
void converToBcdArray (uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for(int i=digits-1;i>=0;i--){
		bcd_number[i]=data%10;
		data=data/10;
	}
}

/** @fn void showDigits
 * @brief Recibe el arreglo de los digitos y los muestra por consola
 * @param digits La cantidad de digitos a mostrar
 * @param bcd_number Puntero al arreglo de los digitos
 */
void showDigits (uint8_t digits, uint8_t *bcd_number)
{
	for(int i=0;i<digits;i++){
		printf("%d",bcd_number[i]);
	}
}

/** @fn void setStates
 * @brief Recibe un digito y el puntero del arreglo de pines correspondientes a la posicion
 * de la pantalla LCD en la que se quiere colocar el digito y setea cada uno de los 4 pines 
 * del dicha posicion de la pantalla en 1 o 0 para formar el numero en BCD
 * @param bcd_digit numero BCD que se desea escribir en la pantalla
 * @param b puntero al arreglo de pines
 */
void setStates(uint8_t bcd_digit, gpioConf_t *b)
{
	for (int i=0;i<4;i++){
		GPIOState(b[i].pin,bcd_digit&1);
		bcd_digit=bcd_digit>>1;
	}
}

/** @fn void ShowNumbs
 * @brief Recibe un numero y lo muestra en la pantalla LCD
 * @param dato Numero a mostrar en pantalla
 * @param digits Cantidad de digitos del numero
 * @param b Puntero al arreglo de pines corresponedientes a los 4 pines de cada digito en BCD
 * @param d Puntero al arreglo de pines correspondientes a cada posicion de cada digito
 */
void ShowNumbs(uint32_t dato, uint8_t digits, gpioConf_t *b, gpioConf_t *d)
{
	uint8_t bcd_number[digits];
	converToBcdArray(dato,digits,&bcd_number);
	//showDigits(digits,&bcd_number);
	for (int i=0;i<digits;i++){
		setStates(bcd_number[i],b);	//Seteo el numero que sera cargado en la pantalla
		GPIOOn(d[i].pin);		//Genero el pulso para que se cargue el numero en la pantalla
		GPIOOff(d[i].pin);
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){
	uint32_t dato=645;	//Numero que quiero mostrar en pantalla
	uint8_t digits=3;	//Cantidad de digitos
	gpioConf_t b[4];	//Arreglo de pines correspondientes a los 4 pines de cada digito en BCD
	gpioConf_t d[3];	//Arreglo de pines correspondientes a cada poscion de cada digito
	b[0].pin=GPIO_20;	//Seteo el numero de pin a utilizar
	b[1].pin=GPIO_21;
	b[2].pin=GPIO_22;
	b[3].pin=GPIO_23;
	for (int i=0;i<4;i++){	//Seteo los pines para ser de escritura
		b[i].dir=OUT;
	}
	d[0].pin=GPIO_19;	//Repito el paso anterior con el otro arreglo de pines
	d[1].pin=GPIO_18;
	d[2].pin=GPIO_9;
	for (int i=0;i<3;i++){
		d[i].dir=OUT;
	}
	for (int i=0;i<4;i++){	//Inicializo todos los pines y los seteo en off
		GPIOInit(b[i].pin,b[i].dir);
		GPIOOff(b[i].pin);
	}
	for (int i=0;i<3;i++){	//Repito el paso anterior con el otro arreglo de pines
		GPIOInit(d[i].pin,d[i].dir);
		GPIOOff(d[i].pin);
	}
	ShowNumbs(dato,digits,&b,&d);	//Llamo a la funcion de mostrar los numeros en la pantalla
}
/*==================[end of file]============================================*/