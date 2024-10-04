/** @mainpage Guia 2 - Actividad 4
 *
 * @section genDesc General Description
 *
 * La aplicacion se encarga convertir una señal digital de ecg a analógica, para luego convertirla
 * nuevamente a digital y mostrarla por un graficador del puerto serie
 *
 * @section hardConn Hardware Connection
 *
 * |	EDU_ESP32  	  |	  PERIFERICO 			|
 * |	GPIO0		  |	  Salida analógica		|
 * |	GND			  |	  GND					| 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 20/09/2024 | Document creation		                         |
 * | 04/10/2024 | Documentacion 		                         |
 * @author Boris Hage (boris.hage@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
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
#include "analog_io_mcu.h"

/*==================[macros and definitions]=================================*/
//#define REFRESH_PERIOD 2000					// en micro segundos
/**
 * El tamaño del char que contiene los valores digitales del ecg
 *
 * @define BUFFER_SIZE 231
 */
#define BUFFER_SIZE 231

/*==================[internal data definition]===============================*/
TaskHandle_t read_signal_task_handle = NULL; /**<Variable que utiliza el OS para llamar a la tarea de leer la señal */
uint16_t refresh_period = 2000; /**<Define el período utilizado por el timer para enviar datos por el puerto serie para graficar */
uint16_t ecg_period = 4000; /**<Define el período utilizado por el timer para enviar los datos digitales del ecg al conversor a analógico */
uint16_t ecg_value; /**<Variable utilizada para cargar el valor digital del ecg */

TaskHandle_t ecg_task_handle = NULL; /**<Variable que utiliza el OS para llamar a la terea de enviar el ecg */

const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
}; /**<Arreglo de char que contiene los valores digitales de ecg */

unsigned char otro_ecg[256] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17

} ; /**<Otro arreglo de char que contiene valores digitales de ecg */

/*==================[internal functions declaration]=========================*/

/** @fn timer_1_finished
 * @brief Envia una notidicacion a la tarea de distancia cada vez que termina el timer 1
 * @param param Puntero utilizado por el sistema cuando es llamada la funcion
 */
void timer_1_finished(void* param){
    vTaskNotifyGiveFromISR(read_signal_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a la finalizacion del timer */
}

/** @fn timer_2_finished
 * @brief Envia una notidicacion a la tarea de enviar ecg cada vez que termina el timer 2
 * @param param Puntero utilizado por el sistema cuando es llamada la funcion
 */
void timer_2_finished(void* param){
    vTaskNotifyGiveFromISR(ecg_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a la finalizacion del timer */
}

/** @fn set_ecg_period
 * @brief setea el valor del período del ecg
 * @param value Valor a asignar al período del ecg
 */
void set_ecg_period(uint16_t value)
{
	ecg_period=value;
}

/** @fn send_digital_ecg
 * @brief Envía los datos digitales al conversor digital-analógico
 */
void send_digital_ecg()
{
	uint8_t count=0;
	//set_ecg_period(4000);
	while(true)
	{
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		AnalogOutputWrite(otro_ecg[count]);
		count=count+1;
		if(count==256){
			count=0;
		}
	}
}

/** @fn board_interruption
 * @brief Recibe las interrupciones del teclado por el puerto serie y realiza las acciones correspondientes
 */
void board_interruption()
{
	uint8_t data;
	UartReadByte(UART_PC,&data);
	switch(data)
	{
		case 'H':
			//set_hold();
		break;
		case 'O':
			//set_active();
		break;
	}	
}

/** @fn read_analogic_signal
 * @brief Es la tarea encargada de leer la señal analógica
 * @param pvParameter Puntero utilizado por el sistema operativo para la administracion de tareas
 */
void read_analogic_signal(void *pvParameter)
{	
	while(true)	//Se crea un bucle que siempre corre cuando el sistema operativo lo asigna
	{	
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		AnalogInputReadSingle(CH1,&ecg_value);
		UartSendString(UART_PC,(const char*)UartItoa(ecg_value,10));
		UartSendString(UART_PC,"\r\n");
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
        .period = refresh_period,
        .func_p = timer_1_finished,
        .param_p = NULL
    };
	TimerInit(&timer_1);

	timer_config_t timer_2 = {
        .timer = TIMER_B,
        .period = ecg_period,
        .func_p = timer_2_finished,
        .param_p = NULL
    };
	TimerInit(&timer_2);

	LedsInit();		//Inicializa los leds
	SwitchesInit(); //Inicializa las teclas
	serial_config_t serial_port = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = board_interruption,
		.param_p = NULL
	};
	
	UartInit(&serial_port);
	analog_input_config_t analog_input = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = NULL
	};
	AnalogInputInit(&analog_input);
	AnalogOutputInit();
	xTaskCreate(&read_analogic_signal,"Signal", 512, NULL, 5,&read_signal_task_handle);	//Crea la tarea de leer señal y la empieza a correr
	xTaskCreate(&send_digital_ecg,"ECG", 512, NULL, 5,&ecg_task_handle);	//Crea la tarea de leer señal y la empieza a correr
	//SwitchActivInt(SWITCH_1,&set_active,NULL);	//Crea las interrupciones aceptadas por la tecla 1
	//SwitchActivInt(SWITCH_2,&set_hold,NULL);	//Crea las interrupciones aceptadas por la tecla 2
	TimerStart(timer_1.timer);	//Inicia el contador 1
	TimerStart(timer_2.timer);  //Inicia el contador 2

}
/*==================[end of file]============================================*/