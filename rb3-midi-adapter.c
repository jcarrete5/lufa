#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void blink(void) {
    DDRC |= _BV(DDC7);
    while (true) {
        PORTC |= _BV(PORTC7);
        _delay_ms(1000);
        PORTC &= ~_BV(PORTC7);
        _delay_ms(1000);
    }
}

void usb_up(void) {
    /* Power on USB Voltage Regulator */
    UHWCON |= _BV(UVREGE);

    /* Configure PLL */
    PLLCSR |= _BV(PINDIV);              /* Divide PLL input by 2 (16MHz -> 8MHz) */
    PLLFRQ |= _BV(PDIV3) | _BV(PDIV1);  /* Output 96MHz from PLL */
    PLLFRQ |= _BV(PLLUSB);              /* Divide PLL output by 2 for USB module */
    PLLFRQ |= _BV(PLLTM1);              /* Divide PLL output by 1.5 for high-speed clock timer */

    /* Enable PLL */
    PLLCSR |= _BV(PLLE);

    /* Wait for PLL to lock to the reference clock */
    while (!(PLLCSR & _BV(PLOCK)));

    /* Enable USB interface */
    USBCON |= _BV(USBE);
    USBCON &= ~_BV(FRZCLK);

    /* Configure USB interface */
    UDCON &= ~_BV(LSM);

    /* Wait for USB VBUS connection */

    /* Attach USB device */
    UDCON &= ~_BV(DETACH);
}

int main(void) {
    // usb_up();

    blink();

    return 0;
}
