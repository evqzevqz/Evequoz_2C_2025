/*! @mainpage Actividad 1 - Medidor de distancia por ultrasonido
 *
 * @section genDesc General Description
 *
 * Este programa implementa un medidor de distancia por ultrasonido utilizando el sensor HC-SR04.
 * El sistema muestra la distancia medida mediante LEDs y un display LCD, con funcionalidades
 * de inicio/detención de medición y hold de resultados.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	HC-SR04 Trigger | 	GPIO_3		|
 * | 	HC-SR04 Echo    | 	GPIO_2		|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 27/10/2025 | Entrega del proyecto		                         |
 *
 * @author Ana Clara Evequoz (ana.evequoz@uner.edu.ar)
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
#include <switch.h>

/*==================[variables globales]=========================*/
static bool medicion_activa = false;
static bool hold_activo = false;
static uint16_t distancia_actual = 0;
static uint16_t distancia_mostrada = 0;

/*==================[tarea medición + display]====================*/
static void PrenderLedyMostrarDistancias(void *pvParameter){
    while(true){
        if (medicion_activa) {
            distancia_actual = HcSr04ReadDistanceInCentimeters();
        }

        if (!hold_activo) {
            distancia_mostrada = distancia_actual;
        }

        // Control de LEDs
        if (distancia_mostrada < 10) {
            LedOff(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
        } else if (distancia_mostrada < 20) {
            LedOn(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
        } else if (distancia_mostrada < 30) {
            LedOn(LED_1);
            LedOn(LED_2);
            LedOff(LED_3);
        } else {
            LedOn(LED_1);
            LedOn(LED_2);
            LedOn(LED_3);
        }

        // Mostrar distancia en LCD
        LcdItsE0803Write(distancia_mostrada);  

        vTaskDelay(1000/ portTICK_PERIOD_MS); // refresco 1s
    }
}

/*==================[tarea botones]====================*/
static void PrenderBoton(void *pvParameter){
    while(true){   
        uint8_t teclas = SwitchesRead();

        if (teclas & SWITCH_1) {               // TEC1: iniciar/detener medición
            medicion_activa = !medicion_activa;
            vTaskDelay(pdMS_TO_TICKS(300));    // anti-rebote
        }

        if (teclas & SWITCH_2) {               // TEC2: hold on/off
            hold_activo = !hold_activo;
            vTaskDelay(pdMS_TO_TICKS(300));
        }

        vTaskDelay(1000/ portTICK_PERIOD_MS);         // escaneo cada 50ms
    }
}

/*==================[main]====================*/
void app_main(void){
    LedsInit();
    LcdItsE0803Init();
    HcSr04Init(GPIO_3, GPIO_2);  
    SwitchesInit();

    xTaskCreate(PrenderLedyMostrarDistancias, "medir_y_prender", 1024, NULL, 5, NULL);
    xTaskCreate(PrenderBoton, "leer_botones", 1024, NULL, 5, NULL);
}
