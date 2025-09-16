/*! @mainpage Blinking switch: Ejercicio de introduccion a la Electronica Programable, usando los LEDs y los switch de la placa provista por la catedra. 
 *
 * \section genDesc General Description
 *
 * La aplicacion permite que se enciedan los LEDs 1 y 2 si se presiona la tecla 1 o 2 correspondientemente. Tambien si ambas teclas son presionadas al mismo tiempo, se encienda el LED 3.
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 15/09/2025 | Document creation		                         |
 *
 * @author Ana Clara Evequoz 
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
#define CONFIG_BLINK_PERIOD 1000
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t teclas;
	LedsInit();
	SwitchesInit();
    while(1)    {
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
    			LedToggle(LED_1);
    		break;
    		case SWITCH_2:
    			LedToggle(LED_2);
    		break;
    		case (SWITCH_1 | SWITCH_2):  // Ambos a la vez
			LedToggle(LED_3);
			break;
    	}
	    //LedToggle(LED_3);
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}
