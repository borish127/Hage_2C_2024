/** @mainpage Guia 2 - Actividad 3
 *
 * @section genDesc General Description
 *
 * La aplicacion se encarga de realizar la misma funcion que la actividad anterior pero
 * envia la informacion por puerto serie para obervarlo en terminal en la pc
 * Tambien realiza la funcion de leer teclas en el teclado del pc
 *
 * @section hardConn Hardware Connection
 *
 * |	EDU_ESP32  	  |	 PERIFERICO |
 * |    GPIO_3		  |   ECHO   	|
 * |    GPIO_2		  |   TRIGGER  	|
 * |	+5V			  |	  +5V		|
 * |	GND			  |	  GND		| 
 * |	J2	Port	| LCD Display	|
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 20/09/2024 | Document creation		                         |
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
#include "timer_mcu.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/
#define REFRESH_PERIOD 1000000
/*==================[internal data definition]===============================*/
TaskHandle_t distance_task_handle = NULL;
bool active=true;
bool holding=false;
/*==================[internal functions declaration]=========================*/

/** @fn timer_1_finished
 * @brief Envia una notidicacion a la tarea de distancia cada vez que termina el timer 1
 * @param param Puntero utilizado por el sistema cuando es llamada la funcion
 */
void timer_1_finished(void* param){
    vTaskNotifyGiveFromISR(distance_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada al SWITCH_1 */
}

/** @fn read_distance
 * @brief Mide la distancia en centimetros
 * @param[in]
 * @return Devuelve la distancia medida en centimetros
 */
uint8_t read_distance(void)
{
	return HcSr04ReadDistanceInCentimeters();
}

/** @fn set_active
 * @brief Se encarga de leer la tecla 1 y setear el booleano de actividad
 */
void set_active()
{
	active=!active;	
}

/** @fn set_hold
 * @brief Se encarga de leer la tecla 2 y setear el booleano de refrescar la pantalla
 */
void set_hold()
{
	holding=!holding;	
}

/** @fn board_interruption
 * @brief Realiza las acciones al recibir la interrupcion del teclado
 */
void board_interruption()
{
	uint8_t data;
	UartReadByte(UART_PC,&data);
	switch(data)
	{
		case 'H':
			set_hold();
		break;
		case 'O':
			set_active();
		break;
	}	
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
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if(active){
			distance=read_distance();	//Lee la distancia
			update_leds(create_intervals(distance));	//Enciende los leds correspondientes
			if(!holding){
				LcdItsE0803Write(distance);	//Muestra la distancia en la pantalla
				UartSendString(UART_PC,(const char*)UartItoa(distance,10));
				UartSendString(UART_PC," cm\r\n");
			}
		}
	}
}

/** @fn switches_task
 * @brief Es la tarea encargada de leer las teclas y definir si se esta leyendo y/o refrescando la pantalla
 * @param pvParameter Puntero utilizado por el sistema operativo para la administracion de tareas
 */
/*==================[external functions definition]==========================*/
void app_main(void){
	timer_config_t timer_1 = {
        .timer = TIMER_A,
        .period = REFRESH_PERIOD,
        .func_p = timer_1_finished,
        .param_p = NULL
    };
	TimerInit(&timer_1);
	LedsInit();		//Inicializa los leds
	SwitchesInit(); //Inicializa las teclas
	HcSr04Init(GPIO_3,GPIO_2);	//Inicializa el sensor ultrasonido
	LcdItsE0803Init();	//Inicializa la pantalla LCD
	serial_config_t serial_port = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = board_interruption,
		.param_p = NULL
	};
	UartInit(&serial_port);
	xTaskCreate(&distance_task,"Distance", 512, NULL, 5,&distance_task_handle);	//Crea la tarea de distancia y la empieza a correr
	SwitchActivInt(SWITCH_1,&set_active,NULL);	//Crea las interrupciones aceptadas por la tecla 1
	SwitchActivInt(SWITCH_2,&set_hold,NULL);	//Crea las interrupciones aceptadas por la tecla 2
	TimerStart(timer_1.timer);	//Inicia el contador 1
}
/*==================[end of file]============================================*/