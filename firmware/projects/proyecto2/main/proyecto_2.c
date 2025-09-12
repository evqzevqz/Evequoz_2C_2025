/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1, LED_2 and LED_3 blink at different rates, using FreeRTOS tasks.
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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <led.h>
#include <hc_sr04.h>
#include <lcditse0803.h>


/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_LED_1 1500
#define CONFIG_BLINK_PERIOD_LED_2 1500
#define CONFIG_BLINK_PERIOD_LED_3 1500
// Remove this macro definition, as HcSr04ReadDistanceInCentimeters should be a function.


/*==================[internal data definition]===============================*/
TaskHandle_t xHandlePrenderLedyMostrarDistanciasTask = NULL;
/*==================[internal functions declaration]=========================*/

static void PrenderLedyMostrarDistancias(void *pvParameter){
    while(true){
        uint16_t distancia = HcSr04ReadDistanceInCentimeters();
        if (distancia > 10 && distancia < 20) {
            LedOn(LED_1);
        }else if (distancia > 20 && distancia < 40) {
            LedOn(LED_1) && LedOn(LED_2);
        }
        else if (distancia > 40) {
            LedOn(LED_1) && LedOn(LED_2) && LedOn(LED_3);
        } else {
            LedOff(LED_1) && LedOff(LED_2) && LedOff(LED_3);
        }
        LcdItsE0803Write(distancia);  
    }
}

static void PrenderBoton(void *pvParameter){
    while(true){   
    }
}


/*==================[external functions definition]==========================*/
void app_main(void){
    LedsInit();
	LcdItsE0803DeInit();
	HcSr04Deinit();


    xTaskCreate(PrenderLedyMostrarDistancias, "mediryprender", 512, NULL, 5, &xHandlePrenderLedyMostrarDistanciasTask);



}
