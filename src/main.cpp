#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "usb/mainUSB.h"



int main(){

    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    //LED
    rcc_periph_clock_enable(RCC_GPIOE);
    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
    gpio_set(GPIOE, GPIO0);


    setupUSB();

    while(true){
        pollUSB();
    }

}