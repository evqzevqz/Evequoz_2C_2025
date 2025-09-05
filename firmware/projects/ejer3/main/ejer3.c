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
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

struct leds
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;        //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
}; 

enum {ON , OFF , TOGGLE};

void recibir_leds( struct leds *myleds) {
	switch ( myleds -> mode) {
		case OFF :
			if (myleds -> n_led == 1){
				LedOff(LED_1);
			} else if (myleds -> n_led == 2){
				LedOff (LED_2);
			} else if (myleds -> n_led == 3){
				LedOff (LED_3);}
		break;
		case ON :
			if (myleds -> n_led == 1){
				LedOn(LED_1);
			} else if (myleds -> n_led == 2){
				LedOn (LED_2);
			} else if (myleds -> n_led == 3){
				LedOn (LED_3);}
		break;
		case TOGGLE: 
			for (int i = 0 ; i<myleds->n_ciclos*2 ; i++){
				if (myleds -> n_led == 1){
				LedToggle(LED_1);
				}
				else if (myleds -> n_led == 2)
				{
					LedToggle(LED_3);
				}
				else if (myleds -> n_led == 3)
				{
					LedToggle(LED_3);
				}
					vTaskDelay(myleds -> periodo / portTICK_PERIOD_MS);}
		break;
	}
}



void app_main(void){

	LedsInit();
	
	struct leds mled;
	mled.mode = TOGGLE;       
	mled.n_led = 2;
	mled.n_ciclos = 10;        
	mled.periodo = 1000;

	recibir_leds(&mled);
	while (1)
	{
		/* code */
	}
	
}

/*==================[end of file]============================================*/