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

//Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de salida y un puntero a un arreglo donde se almacene los n dígitos. La función deberá convertir el dato recibido a BCD, guardando cada uno de los dígitos de salida en el arreglo pasado como puntero.

int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{  
	for (int i = digits - 1; i >= 0; i--) {
		bcd_number[i] = data % 10; // Obtener el dígito menos significativo
		data /= 10;                // Eliminar el dígito menos significativo
	}
	return 0; // Retornar 0 para indicar éxito
}

#include "gpio_mcu.h"

typedef struct
	{
		gpio_t pin;			/*!< GPIO numero de pin */
		io_t dir;			/*!< GPIO direccion '0' IN;  '1' OUT*/
	 	
}gpioConf_t;

void establecer_GPIO (uint8_t bcd_digit,  gpioConf_t *gpio_vector ){
for (uint8_t i = 0; i < 4; i++) {
        uint8_t bit = (bcd_digit >> i) & 0x01; // Extrae el bit correspondiente
        gpioWrite(gpio_vector[i].pin, bit);    // Cambia el estado del GPIO
    }
}




void app_main(void){
    gpioConf_t gpio_vector[4] = {
        {GPIO_20, 1},
        {GPIO_21, 1},
        {GPIO_22, 1},
        {GPIO_23, 1}
    };
    uint8_t bcd_digits[3];
    convertToBcdArray(123, 3, bcd_digits); // Convierte 123 en [1,2,3]
    for (int i = 0; i < 3; i++) {
        establecer_GPIO(bcd_digits[i], gpio_vector); // Muestra cada dígito en los GPIO
}
}
/*==================[end of file]============================================*/