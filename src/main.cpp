#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "usb/mainUSB.h"

#include "FreeRTOS.h"
#include "task.h"


/*
 * Handler in case our application overflows the stack
 */
void vApplicationStackOverflowHook(
	TaskHandle_t xTask __attribute__((unused)),
    char *pcTaskName __attribute__((unused))) {

	for (;;);
}


static void task1(void *args __attribute__((unused))) {
    for (;;) {
		gpio_set(GPIOE, GPIO0);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void task2(void *args __attribute__((unused))) {
    for (;;) {
		gpio_set(GPIOE, GPIO1);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

int main(){

    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);



    //LED
    rcc_periph_clock_enable(RCC_GPIOE);
    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);


    xTaskCreate(task1, "LED1", 100, NULL, 2, NULL);
    xTaskCreate(task2, "LED2", 100, NULL, 2, NULL);

    /* Start the scheduler. */
	vTaskStartScheduler();
    /*
    setupUSB();

    while(true){
        pollUSB();
    }
    */
    for(;;) {}

    return 0;
}