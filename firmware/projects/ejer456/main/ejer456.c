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
#include <led.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <gpio_mcu.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

//establezco la configuracion de gpio con la que estoy trabajand !!

typedef struct
	{
		gpio_t pin;			/*!< GPIO numero de pin */
		io_t dir;			/*!< GPIO direccion '0' IN;  '1' OUT*/
	 	
}gpioConf_t;


//Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de salida y un puntero a un arreglo donde se almacene los n dígitos. La función deberá convertir el dato recibido a BCD, guardando cada uno de los dígitos de salida en el arreglo pasado como puntero.

int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{  
	for (int i = digits - 1; i >= 0; i--) {
		bcd_number[i] = data % 10; // Obtener el dígito menos significativo
		data /= 10;                // Eliminar el dígito menos significativo
	}
	return 0; // Retornar 0 para indicar éxito
}

void establecer_GPIO (uint8_t bcd_digit,  gpioConf_t *gpio_vector ){
for (uint8_t i = 0; i < 4; i++) {
        uint8_t bit = (bcd_digit >> i) & 0x01; // Extrae el bit correspondiente
        gpioWrite(gpio_vector[i].pin, bit);    // Cambia el estado del GPIO
    }
}

/**
 * @brief Muestra un número en los 3 dígitos del display multiplexado
 * @param numero Número decimal a mostrar
 * @param digits Cantidad de dígitos (3)
 * @param gpio_bcd Vector de pines para datos BCD (D1..D4)
 * @param gpio_sel Vector de pines de selección de dígito (SEL1..SEL3)
 */

void mostrarNumero(uint32_t numero, uint8_t digits,
                   gpioConf_t *gpio_bcd, gpioConf_t *gpio_sel) {

    uint8_t bcd_digits[digits];
    convertToBcdArray(numero, digits, bcd_digits);

    for (uint8_t i = 0; i < digits; i++) {
        establecer_GPIO(bcd_digits[i], gpio_bcd);   // carga valor BCD
        gpioWrite(gpio_sel[i].pin, 1);               // habilita dígito i
        vTaskDelay(pdMS_TO_TICKS(5));                // pequeño pulso latch
        gpioWrite(gpio_sel[i].pin, 0);               // deshabilita dígito i
    }
}


void app_main(void){

    // Pines BCD (antes era tu gpio_vector)
    gpioConf_t gpio_bcd[4] = {
        {GPIO_20, 1},
        {GPIO_21, 1},
        {GPIO_22, 1},
        {GPIO_23, 1}
    };

    // Pines de selección
    gpioConf_t gpio_sel[3] = {
        {GPIO_19, 1},
        {GPIO_18, 1},
        {GPIO_9,  1}
    };

    // Inicializo todos los pines como salidas
    for (int i = 0; i < 4; i++) GPIOInit(gpio_bcd[i].pin, gpio_bcd[i].dir);
    for (int i = 0; i < 3; i++) GPIOInit(gpio_sel[i].pin, gpio_sel[i].dir);

    // Bucle principal: refresca continuamente el número
    while (1) {
        mostrarNumero(123, 3, gpio_bcd, gpio_sel); // mostrar "123"
        // (mostrarNumero multiplexa cada dígito 5ms, por eso hay que repetirlo)
    }
}
/*==================[end of file]============================================*/