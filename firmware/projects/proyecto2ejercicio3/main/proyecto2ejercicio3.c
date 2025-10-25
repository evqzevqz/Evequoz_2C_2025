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


/*==================[variables globales]=========================*/
static bool medicion_activa = false;
static bool hold_activo = false;
static uint16_t distancia_actual = 0;
static uint16_t distancia_mostrada = 0;

#define CONFIG_PERIOD 1000000

/*==================[internal data definition]===============================*/
TaskHandle_t atender_task_handle = NULL;



/*==================[tarea medición + display]====================*/
static void PrenderLedyMostrarDistancias(void *pvParameter){
    while (true){
        // Espera hasta que el timer dispare la notificación
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (medicion_activa) {
            distancia_actual = HcSr04ReadDistanceInCentimeters();
            UartSendString(UART_PC,(char *)UartItoa(distancia_actual,10));
            UartSendString(UART_PC, "cm\r\n");
        } else {
            distancia_actual = 0;
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

    }
}
/*==================[Funciones recibir UART]====================*/
static void FuncUart(void *param){
    uint8_t dato;

    if (UartReadByte(UART_PC, &dato)) {   // si recibimos un byte
        if (dato == 'O' || dato == 'o') {
            medicion_activa = !medicion_activa;
        }
        if (dato == 'H' || dato == 'h') {
            hold_activo = !hold_activo;
        }
    }
}

/*==================[Interrupcion botones]====================*/
static void InterrumpirMedicionyReiniciarla(void *pvParameter){  
        medicion_activa = !medicion_activa;        
    }

static void InterrumpirMedicionyCongelarla(void *pvParameter){    
        hold_activo = !hold_activo;        
    }

/*==================[Timer]====================*/    
static void AtenderTimer(void){
    vTaskNotifyGiveFromISR(atender_task_handle, NULL);
}



//aca evito consultar todo el tiempo que es lo que hace la tarea, para eso hago interrupciones, es decir el programa corre por si mismo
// hasta que atiende a la tecla, es entrar a la funcion atender tecla y ver lo que te pide 
// ver en el driver de tecla y ver como habilitar la interrumpcion 
//ahora con las teclas hago 2 interrupciones en vez de tareas, en el servicio de interruptor hay que tener funciones chiquitas como cambiar una variable, no medir, leer, etc(cosas q no se cuanto tiempo me lleve)
//con switchactivint activo la interrupcion, hacemos una funcion que cambie el estado de variable hold,a su vez hay 2 funciones ya que hay 2 teclas

//VER NVIC DE TEORIA
// el reset es una expecion no una interrunpcion que son distintas
/*==================[main]====================*/
void app_main(void){
    LedsInit();
    LcdItsE0803Init();
    HcSr04Init(GPIO_3, GPIO_2);  // poner los pines reales
    SwitchesInit();
    timer_config_t timer = {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD,
        .func_p = AtenderTimer,
        .param_p = NULL
    };
    serial_config_t my_uart = {
    .port = UART_PC,
    .baud_rate = 9600,
    .func_p = FuncUart,
    .param_p = NULL,
    };
    TimerInit(&timer);
    UartInit(&my_uart);
    /* Creación de tareas */
    xTaskCreate(PrenderLedyMostrarDistancias, "medir_y_prender", 1024, NULL, 5, &atender_task_handle);
    /* Creación de Interrupciones */
    SwitchActivInt(SWITCH_1, InterrumpirMedicionyReiniciarla,NULL);
    SwitchActivInt(SWITCH_2, InterrumpirMedicionyCongelarla,NULL);
    /* Inicialización del conteo de timers */
    TimerStart(timer.timer);

}


//timer: para cuando necesitamos hacer temporazicaciones precisas en el tiempo usamos timer, osea precision temporal. 
//Para lo que hay que hacer hay que usar la tecnica de notificacion de tareas, vtasknotifytakeygive, hacemos una funcion que sea atender timer, vtaskgiven(handle), que handle es la tarea que le voy a avisar y en la tarea es vtasktake notify

//UART definimos la estructura, usamos velocidad de transmision 960 caracteres por segundo, 9600(baud rate)/10, la inicializamos. Y mandakos por UartSendString() hay que tener cuidado que hay que convertir a valores y se usa funciones de uartintoa. Primera parte

//Segunda Parte. Ver que pasa cuando paso caracteres desde la pc a la placa, uso interruptores, ahora hay que usar la tercer renglon del struct, donde ponemos la funcioon q atiende la interrumpcion y desp un uartreadbyte