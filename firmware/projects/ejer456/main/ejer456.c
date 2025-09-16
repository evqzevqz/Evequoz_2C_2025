/*! @mainpage Ejercicio 6 del Proyecto 1 de Electronica Programable.
 *
 * @section genDesc Descripción General
 *
 * 
 * Este programa permite mostrar en el display 7 segmentos, un numero que ha elegido el usuario utilizando una funcion que recibe un dato de 32 bits,  la cantidad de dígitos de salida y dos vectores y devuelve el numero.
 * 
 * 
 *
 * 
 *
 * @section hardConn Conexiones de Hardware
 *
 * | EDU-ESP   | PERIFÉRICO |
 * |:----------|:-------------|
 * | GPIO_20   | D1            |
 * | GPIO_21   | D2            |
 * | GPIO_22   | D3            |
 * | GPIO_23   | D4            |
 * | GPIO_19   | SEL_1          |
 * | GPIO_18   | SEL_2          |
 * | GPIO_9    | SEL_3          |
 * | +5V       | +5V             |
 * | GND       | GND              |
 * 
 * @section changelog Historial de Cambios
 *
 * |   Fecha    | Descripción              |
 * |:-----------|:---------------------------|
 * | 15/09/2025 | Creación del documento    |
 *
 * @author Ana Clara Evequoz 
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <led.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <gpio_mcu.h>

/*==================[macros and definitions]=================================*/

/**
 * @brief Estructura de configuración de un pin GPIO
 */
typedef struct
{
    gpio_t pin;    /*!< Número de pin GPIO */
    io_t dir;      /*!< Dirección: 0=entrada, 1=salida */
} gpioConf_t;

/*==================[external functions definition]==========================*/

/**
 * @brief Convierte un número decimal en un arreglo de dígitos BCD
 *
 * @param data Número decimal a convertir
 * @param digits Cantidad de dígitos de salida
 * @param bcd_number Puntero al arreglo donde se guardarán los dígitos
 * @return int8_t Retorna 0 si la conversión fue exitosa
 *
 * @note El dígito menos significativo quedará en el último índice del arreglo
 */
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t * bcd_number)
{  
    for (int i = digits - 1; i >= 0; i--) {
        bcd_number[i] = data % 10;
        data /= 10;
    }
    return 0;
}

/**
 * @brief Establece el valor de 4 líneas GPIO según un dígito BCD
 *
 * @param bcd_digit Dígito BCD (0 a 9)
 * @param gpio_vector Vector con 4 pines de salida (bit0..bit3)
 */
void establecer_GPIO(uint8_t bcd_digit, gpioConf_t *gpio_vector)
{
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t bit = (bcd_digit >> i) & 0x01;
        if (bit == 1) {
            GPIOOn(gpio_vector[i].pin);
        } else {
            GPIOOff(gpio_vector[i].pin);
        }
    }
}

/**
 * @brief Muestra un número en los dígitos de un display multiplexado
 *
 * @param numero Número decimal a mostrar
 * @param digits Cantidad de dígitos (ej: 3)
 * @param gpio_bcd Vector con pines para datos BCD (D1..D4)
 * @param gpio_sel Vector con pines de selección de dígito (SEL1..SELn)
 *
 * @note Esta función enciende cada dígito durante 5ms, por lo que debe ser llamada de forma repetitiva
 */
void mostrarNumero(uint32_t numero, uint8_t digits,
                   gpioConf_t *gpio_bcd, gpioConf_t *gpio_sel)
{
    uint8_t bcd_digits[digits];
    convertToBcdArray(numero, digits, bcd_digits);

    for (uint8_t i = 0; i < digits; i++) {
        establecer_GPIO(bcd_digits[i], gpio_bcd);
        GPIOOn(gpio_sel[i].pin);
        vTaskDelay(pdMS_TO_TICKS(5));
        GPIOOff(gpio_sel[i].pin);
    }
}

/**
 * @brief Función principal de la aplicación
 *
 * Inicializa los pines GPIO y muestra continuamente un número en el display.
 */
void app_main(void)
{
    gpioConf_t gpio_bcd[4] = {
        {GPIO_20, 1},
        {GPIO_21, 1},
        {GPIO_22, 1},
        {GPIO_23, 1}
    };

    gpioConf_t gpio_sel[3] = {
        {GPIO_19, 1},
        {GPIO_18, 1},
        {GPIO_9,  1}
    };

    for (int i = 0; i < 4; i++) GPIOInit(gpio_bcd[i].pin, gpio_bcd[i].dir);
    for (int i = 0; i < 3; i++) GPIOInit(gpio_sel[i].pin, gpio_sel[i].dir);

    while (1) {
        mostrarNumero(407, 3, gpio_bcd, gpio_sel);
    }
}
/*==================[end of file]============================================*/
