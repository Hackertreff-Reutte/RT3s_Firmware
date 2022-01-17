#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>


void delay(){
    //just do something (busy waiting)
    for (int i = 0; i < 10000000; i++) {
			__asm__("nop");
		}
}

int main(){
    //enable clock
    rcc_periph_clock_enable(RCC_GPIOE);

    //set led gpio as output
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);

    //disable read an green led
    gpio_clear(GPIOE, GPIO0);
    gpio_clear(GPIOE, GPIO1);

    while (1) {

        //green led on
        gpio_set(GPIOE, GPIO0);	//green led on
        delay();

        //red led on
        gpio_clear(GPIOE, GPIO0); //green led off
        gpio_set(GPIOE, GPIO1);	//red led on
        delay();

        //both leds on
        gpio_set(GPIOE, GPIO0);	//green led on 
        delay();

        //both leds off
        gpio_clear(GPIOE, GPIO0);	//green led off
        gpio_clear(GPIOE, GPIO1);	//red led off
        delay();
		
    }
}