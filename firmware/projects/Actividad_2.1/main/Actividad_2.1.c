/** @mainpage Guia 2 - Actividad 1
 *
 * @section genDesc General Description
 *
 * La aplicacion se encarga de medir la una distancia utilizando un sensor ultrasonido
 * Luego segun la distancia a la que se encuentra enciende los leds, y por otro lado 
 * muestra en la pantalla LCD la distancia medida
 *
 * @section hardConn Hardware Connection
 *
 * |	EDU-ESP32     |	 PERIFERICO |
 * |    GPIO_3		  |   ECHO   	|
 * |    GPIO_2		  |   TRIGGER  	|
 * |	+5V			  |	  +5V		|
 * |	GND			  |	  GND		| 
 * |	J2	Port	| LCD Display	|
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 06/09/2024 | Document creation		                         |
 * | 06/09/2024 | Documentation 		                         |
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
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
bool active=true; /**<Variable de si está activo*/
bool holding=false; /**<Variable de si refresca la pantalla*/
/*==================[internal functions declaration]=========================*/

/** @fn read_distance
 * @brief Mide la distancia en centimetros
 * @param[in]
 * @return Devuelve la distancia medida en centimetros
 */
uint8_t read_distance(void)
{
	return HcSr04ReadDistanceInCentimeters();
}

/** @fn create_intervals
 * @brief Crea los intervalos de distancia en las que se enciende cada led
 * @param distance Distancia medida en centrimetros
 * @return Devuelve el numero de intervalo correspondiente
 */
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

/** @fn update_leds
 * @brief Enciende los leds correspondientes al intervalo en el que se encuentra la medicion
 * @param interval El numero de intervalo en el que se encuentra la medicion
 */
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

/** @fn distance_task
 * @brief Es la tarea encargada de medir la distancia, mostrarla en pantalla y encender los leds
 * @param pvParameter Puntero utilizado por el sistema operativo para la administracion de tareas
 */
void distance_task(void *pvParameter)
{	
	uint8_t distance;
	while(true){	//Se crea un bucle que siempre corre cuando el sistema operativo lo asigna
		if(active){
			distance=read_distance();	//Lee la distancia
			update_leds(create_intervals(distance));	//Enciende los leds correspondientes
			if(!holding){
				LcdItsE0803Write(distance);	//Muestra la distancia en la pantalla
			}
		}
		vTaskDelay(1000/portTICK_PERIOD_MS);	//Le entrega el control al sistema operativo por 1 segundo
	}
}

/** @fn set_active
 * @brief Se encarga de leer la tecla 1 y setear el booleano de actividad
 */
void set_active()
{
	if(SwitchesRead()==SWITCH_1){
		active=!active;
		vTaskDelay(150/portTICK_PERIOD_MS);
	}
}

/** @fn set_hold
 * @brief Se encarga de leer la tecla 2 y setear el booleano de refrescar la pantalla
 */
void set_hold()
{
	if(SwitchesRead()==SWITCH_2){
		holding=!holding;
		vTaskDelay(150/portTICK_PERIOD_MS);
	}
}

/** @fn switches_task
 * @brief Es la tarea encargada de leer las teclas y definir si se esta leyendo y/o refrescando la pantalla
 * @param pvParameter Puntero utilizado por el sistema operativo para la administracion de tareas
 */
void shwitches_task(void *pvParameter)
{
	while(true){
		set_active();
		set_hold();
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();		//Inicializa los leds
	SwitchesInit(); //Inicializa las teclas
	HcSr04Init(GPIO_3,GPIO_2);	//Inicializa el sensor ultrasonido
	LcdItsE0803Init();	//Inicializa la pantalla LCD
	xTaskCreate(&distance_task,"Distance", 512, NULL, 5, NULL);	//Crea la tarea de distancia y la empieza a correr
	xTaskCreate(&shwitches_task,"Swhitches",512,NULL,5,NULL);//Crea la tarea de teclas y la empieza a correr
}
/*==================[end of file]============================================*/