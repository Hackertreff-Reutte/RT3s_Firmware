#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "usb/mainUSB.h"
#include "utils/serial.h"


#define VTOR_ADDR 0xE000ED08
#define VECTOR_TABLE_ADDR 0x0800C000

/*
 * Handler in case our application overflows the stack
 */
void vApplicationStackOverflowHook(
	TaskHandle_t xTask __attribute__((unused)),
    char *pcTaskName __attribute__((unused))) {
	for (;;);
}


static void task2(void *args __attribute__((unused))) {
    for (;;) {
		gpio_toggle(GPIOE, GPIO1);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


int main(){

    //fixes the vector table address in the VTOR register
    //bootloader sets it to 0x08000000, but need to be 0x0800C000
    *(uint32_t*)VTOR_ADDR = VECTOR_TABLE_ADDR;

    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    //setup the usb
    setupUSB();

    //setup the status LEDs of the radio
    rcc_periph_clock_enable(RCC_GPIOE);
    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);


    xTaskCreate(task2, "LED2", 100, NULL, 2, NULL);

    /* Start the scheduler. */
	vTaskStartScheduler();



    for(;;) {}

    return 0;
}