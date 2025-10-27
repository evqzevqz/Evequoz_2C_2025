/*! @mainpage Actividad 4 -Modulador de voltaje y visualizacion de ECG
 *
 * @section genDesc General Description
 *
 * Modulacion con un potenciometro de una señal y generacion de señal ECG y visualización por 
 * osciloscopio virtual
 * 
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	Potenciometro    | 	CH1		|
 * | 	UART_PC         | 	USB		|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 27/10/2025 | Entrega proyecto 2		                         |
 *
 * @author Ana Clara Evequoz (ana.evequoz@ingenieria.uner.edu.ar)
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lcditse0803.h>
#include <timer_mcu.h>
#include <uart_mcu.h>
#include <analog_io_mcu.h>



/*==================[variables globales]=========================*/

/** @brief Variable para almacenar la lectura del ADC */

static uint16_t medicion_adc = 0;

/** @brief Índice actual para recorrer el array ECG */

static uint16_t ecg_index = 0;

/** @brief Periodo de muestreo de la señal de voltaje a modular  */

#define CONFIG_PERIOD_1 20000

/** @brief Periodo del timer de ECG para 250Hz ya que es la frecuencia con la cual se suele muestrear un ECG */

#define CONFIG_PERIOD_ECG 4000

/** @brief Señal de ECG proporcionada - Array de muestras */
const unsigned char ECG[] = {
    17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
    18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
    10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
    38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
    133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
    24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
    17,17,17
};

/** @brief Calcula automáticamente la longitud del array ECG */
#define ECG_LENGTH (sizeof(ECG) / sizeof(ECG[0]))

/*==================[internal data definition]===============================*/

/** @brief Handle de la tarea de atender osciloscopio */
TaskHandle_t atender_task_handle = NULL;

/** @brief Handle de la tarea de generación de ECG */
TaskHandle_t ecg_task_handle = NULL;

/*==================[Funciones recibir UART y mandarlo ]====================*/
/** @brief Tarea principal de Osciloscopio, aca lee los datos y los manda por UART a la PC donde son mostrados, esta tarea se ejecuta cuando recibe la notificacion del Timer */
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
/**
 * @brief Tarea principal para generar señal ECG y transmitir datos. Esta tarea tambien se ejecuta mediante notificaciones del Timer pero lo que hace es escribir la muestra del ECG en DAC, leer la senal ADC en el CH1 y transmitir el valor por UART para su visualizacion.*/
static void GenerarECG(void *param){
    while (true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        AnalogOutputWrite(ECG[ecg_index]);
        ecg_index++;
        if (ecg_index >= ECG_LENGTH) {
            ecg_index = 0;
        }
    }
}

/*==================[Timer]====================*/    
/**
 * @brief Interrupción del Timer
 * 
 * Esta función se ejecuta cada vez que el timer alcanza el período configurado.
 * Notifica a la tarea principal.
 */
static void AtenderTimerModulador(void){
    vTaskNotifyGiveFromISR(atender_task_handle, NULL);
    
}
static void AtenderTimerECG(void){
    vTaskNotifyGiveFromISR(ecg_task_handle, NULL);
}

/*==================[main]====================*/
void app_main(void){
    // Configuración de los ADC para leer en CH1 
     analog_input_config_t adc_ecg = {
        .input = CH1,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .param_p = NULL,
        .sample_frec = 0,
    };
    // Configuración Timers
    timer_config_t timer_modulador = {
        .timer = TIMER_B,
        .period = CONFIG_PERIOD_1,
        .func_p = AtenderTimerModulador,
        .param_p = NULL
    };
    timer_config_t timer_ecg = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD_ECG,
        .func_p = AtenderTimerECG,
        .param_p = NULL
    };
    // Configuración UARTs
    serial_config_t my_uart_modulador = {
    .port = UART_PC,
    .baud_rate = 115200,
    .func_p = NULL,
    .param_p = NULL,
    };

    // Inicializaciones
    TimerInit(&timer_modulador);
    TimerInit(&timer_ecg);
    UartInit(&my_uart_modulador);
    AnalogInputInit(&adc_ecg);
    AnalogOutputInit();  // DAC en GPIO 0

    /* Creación de tareas */
    
    xTaskCreate(FuncLeeryMandar, "leer_y_mandar", 4096, NULL, 5, &atender_task_handle);
    xTaskCreate(GenerarECG, "ecg_task", 4096, NULL, 5, &ecg_task_handle);

    /* Inicialización del conteo de timers */
    TimerStart(timer_ecg.timer);
    TimerStart(timer_modulador.timer);
}


/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */