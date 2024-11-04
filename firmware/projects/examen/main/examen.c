/*! @mainpage Examen
 *
 * @section genDesc General Description
 *
 * El programa utiliza un sensor ultrasonido para medir la distancia a la que se encuentra
 * un veículo detrás del ciclista, si el vehículo se encuentra a mas de 5 metros se enciende 
 * el led verde, si se encuentra entre 3 y 5 metros se encienden los leds verde y amarillo,
 * y si se encuentra a menos de 3 metros se encienden los leds verde, amarillo y rojo.
 * A su vez hay un buzzer que suena cada 1,0 segundo si el vehículo se encuenra entre 3 y 5 metros,
 * y suena cada 0,5 segundos si el vehículo se encuentra a menos de 3 metros.
 * El programa envía un mensaje por bluetooth indicando precación o peligro si el vehículo se 
 * encuentra entre 3 y 5 metros, y si se encuentra a menos de 3 metros respectivamente.
 * El programa sensa la aceleración del ciclista y envía un mensaje indicando si el ciclista se cae.
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   			|
 * |:--------------:|:----------------------|
 * | 	hc_sr04		| 	GPIO_3 - GPIO_2		|
 * |	Buzzer		|	GPIO_18	- GND		|
 * |	bt			|	UART usb			|
 * |	analog_io	|	ESP32 analog port	|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 04/11/2024 | Document creation		                         |
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
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
//#include "buzzer.h"
#include "hc_sr04.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/
#define READ_DISTANCE_DELAY 500
#define BUZZER_BEEP_DURATION 50
#define CAUTION_BEEP_DELAY 1000
#define DANGER_BEEP_DELAY 500
#define ACCELEROMETER_READ_DELAY 10
/*==================[internal data definition]===============================*/
uint8_t security_state=0;
/*==================[internal functions declaration]=========================*/

/** @fn green_light
 * @brief Enciende el led verde y apaga los demás
 */
void green_light()
{
	LedOn(LED_1);
	LedOff(LED_2);
	LedOff(LED_3);
	
}

/** @fn yellow_light
 * @brief Enciende los leds verde y amarillo y apaga el rojo
 */
void yellow_light()
{
	LedOn(LED_1);
	LedOn(LED_2);
	LedOff(LED_3);
	
}

/** @fn red_light
 * @brief Enciende los leds verde, amarillo y rojo
 */
void red_light()
{
	LedOn(LED_1);
	LedOn(LED_2);
	LedOn(LED_3);
}

/** @fn set_secure
 * @brief setea el estado de seguridad en estado seguro
 */
void set_secure()
{
	security_state=0;
}

/** @fn set_caution
 * @brief setea el estado de seguridad en precacuión
 */
void set_caution()
{
	security_state=1;
}

/** @fn set_danger
 * @brief setea el estado de seguridad en peligro
 */
void set_danger()
{
	security_state=2;
}

/** @fn send_caution
 * @brief Envía por bluetooth el mensaje de precaución
 */
void send_caution()
{
	UartSendString(UART_PC,"Precaución, vehículo cerca\r\n");
}

/** @fn send_danger
 * @brief Envía por bluetooth el mensaje de peligro
 */
void send_danger()
{
	UartSendString(UART_PC,"Peligro, vehículo cerca\r\n");
}

/** @fn send_fall
 * @brief Envía por bluetooth el mensaje de caída detectada
 */
void send_fall()
{
	UartSendString(UART_PC,"Caída detectada\r\n");
}

/** @fn distance_cases
 * @brief determina en cual de los 3 casos de proximidad se encuentra el vehículo
 * @param distance la distancia medida para determinar en que caso se encuentra
 */
 uint8_t distance_cases(uint16_t distance)
 {
	uint8_t distance_case;
	if(distance>500){
		distance_case=0;
	}
	if(distance<=500&&distance>300){
		distance_case=1;
	}
	if(distance<=300){
		distance_case=2;
	}
	return distance_case;
 }

/** @fn distance_task
 * @brief se encarga de medir la distancia, encender los leds correspondientes y enviar los mensajes correspondientes
 * @param pvParameter puntero utilizado por el sistema operativo para las distintas tareas
 */
void distance_task(void *pvParameter)
{
	uint16_t distance;
	uint8_t distance_case;
	while(true){
		distance=HcSr04ReadDistanceInCentimeters();
		distance_case=distance_cases(distance);
		switch (distance_case)
		{
		case 0:
			green_light();
			set_secure();
			break;
		case 1:
			yellow_light();
			set_caution();
			send_caution();
			break;
		case 2:
			red_light();
			set_danger();
			send_danger();
			break;
		}
	}
	vTaskDelay(READ_DISTANCE_DELAY/portTICK_PERIOD_MS);
}

/** @fn buzzer_beep
 * @brief envía el pulso que enciende y apaga el buzzer
 */
void buzzer_beep()
{
	GPIOOn(GPIO_18);
	vTaskDelay(BUZZER_BEEP_DURATION/portTICK_PERIOD_MS);
	GPIOOff(GPIO_18);
}

/** @fn buzzer_task
 * @brief se encarga hacer sonar el buzzer con la frecuencia correspondiente a cada estado
 * @param pvParameter puntero utilizado por el sistema operativo para las distintas tareas
 */
void buzzer_task(void *pvParameter)
{
	while(true){
		switch (security_state)
		{
		case 1:
			buzzer_beep();
			vTaskDelay(CAUTION_BEEP_DELAY/portTICK_PERIOD_MS);
			break;
		case 2:
			buzzer_beep();
			vTaskDelay(DANGER_BEEP_DELAY/portTICK_PERIOD_MS);
			break;
		}
	}
}

/** @fn accelerometer_task
 * @brief se encarga de tomar las medidas del acelerómetro, realizar la suma y enviar la señal de caída
 * @param pvParameter puntero utilizado por el sistema operativo para las distintas tareas
 */
void accelerometer_task(void *pvParameter)
{
	uint16_t acceleration_x;
	uint16_t acceleration_y;
	uint16_t acceleration_z;
	uint16_t sum;
	while(true){
		AnalogInputReadSingle(CH0,&acceleration_x);
		AnalogInputReadSingle(CH1,&acceleration_y);
		AnalogInputReadSingle(CH2,&acceleration_z);
		sum=1.65*(3+0.3*(acceleration_x+acceleration_y+acceleration_z)); // Convertido de voltaje a G
		if(sum>4){
			send_fall();
		}
		vTaskDelay(ACCELEROMETER_READ_DELAY/portTICK_PERIOD_MS);
	}
}

/** @fn initialize_uart
 * @brief realiza la configuración y la inicialización del puerto UART
 */
void initialize_uart()
{
	serial_config_t serial_port = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL
	};
	UartInit(&serial_port);
}

/** @fn initialize_accelerometer
 * @brief realiza la configuración y la inicialización del acelerómetro
 */
void initialize_accelerometer()
{
	analog_input_config_t accelerometer_x = {
		.input = CH0,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = NULL
	};
	analog_input_config_t accelerometer_y = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = NULL
	};
	analog_input_config_t accelerometer_z = {
		.input = CH2,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = NULL
	};
	AnalogInputInit(&accelerometer_x);
	AnalogInputInit(&accelerometer_y);
	AnalogInputInit(&accelerometer_z);
}

/**	@fn initialize_all
 * @brief inicializa todos los pefiféricos utilizados
 */
void initialize_all()
{
	GPIOInit(GPIO_18,1);	//El GPIO del buzzer (elegí uno arbitrariamente)
	LedsInit();
	HcSr04Init(GPIO_3,GPIO_2);
	initialize_uart();
	initialize_accelerometer();
}
/*==================[external functions definition]==========================*/
void app_main(void){
	//printf("Hello world!\n");
	initialize_all();
	xTaskCreate(&distance_task,"Distance",512,NULL,5,NULL);
	xTaskCreate(&buzzer_task,"Buzzer",512,NULL,5,NULL);
	xTaskCreate(&accelerometer_task,"Accelerometer",512,NULL,5,NULL);
}
/*==================[end of file]============================================*/