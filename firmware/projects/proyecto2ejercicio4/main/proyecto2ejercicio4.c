#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <led.h>
#include <hc_sr04.h>
#include <lcditse0803.h>
#include <switch.h>
#include <timer_mcu.h>
#include <uart_mcu.h>
#include <analog_io_mcu.h>

/*==================[variables globales]=========================*/

static uint16_t medicion = 0;
#define CONFIG_PERIOD 20000

/*==================[internal data definition]===============================*/

TaskHandle_t atender_task_handle = NULL;

/*==================[Funciones recibir UART y mandarlo ]====================*/
static void FuncLeeryMandar(){
    uint16_t dato;
    while (true){
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    AnalogInputReadSingle(CH1,&dato); 
    UartSendString(UART_PC, ">volts: ");
    UartSendString(UART_PC,(char *)UartItoa(dato,10));
    UartSendString(UART_PC, "\r\n");
    }
}

/*==================[Timer]====================*/    

static void AtenderTimer(void){
    vTaskNotifyGiveFromISR(atender_task_handle, NULL);
    
}

/*==================[main]====================*/
void app_main(void){
    analog_input_config_t aic = {
        .input = CH1,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .param_p = NULL,
        .sample_frec = 0,
    };
    
    timer_config_t timer = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD,
        .func_p = AtenderTimer,
        .param_p = NULL
    };
    serial_config_t my_uart = {
    .port = UART_PC,
    .baud_rate = 115200,
    .func_p = NULL,
    .param_p = NULL,
    };
    TimerInit(&timer);
    UartInit(&my_uart);
    AnalogInputInit(&aic);
    /* Creación de tareas */
    
    xTaskCreate(FuncLeeryMandar, "leer_y_mandar", 4096, NULL, 5, &atender_task_handle);
    /* Creación de Interrupciones */

    /* Inicialización del conteo de timers */
    TimerStart(timer.timer);

}


