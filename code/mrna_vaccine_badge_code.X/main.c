#include "config.h"

#include <stdbool.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "button.h"
#include "sequence.h"
#include "moderna_sequence.h"
#include "pfizer_sequence.h"


FUSES = 
{
	.APPEND = 0,
	.BODCFG = ACTIVE_DIS_gc | LVL_BODLEVEL0_gc | SAMPFREQ_1KHZ_gc | SLEEP_DIS_gc,
	.BOOTEND = 0,
	.OSCCFG = FREQSEL_16MHZ_gc,
	.SYSCFG0 = CRCSRC_NOCRC_gc | RSTPINCFG_UPDI_gc,
	.SYSCFG1 = SUT_64MS_gc,
	.WDTCFG = PERIOD_OFF_gc | WINDOW_OFF_gc,
};

void switch_off_leds() {
    VPORTA.OUT = 255;
    VPORTB.OUT = 255;
}

void go_to_sleep(){
    while (~PORTB.IN & PIN2_bm) {}
    _delay_ms(50);
    switch_off_leds();
    /* disable periodic interrupt, so it doesn't wake up MCU */
    RTC.PITCTRLA &= ~RTC_PITEN_bm;
    _delay_ms(100);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    while (~PORTB.IN & PIN2_bm) {}
    _delay_ms(50);
    button_released = 0;
    /* enable periodic interrupt again */
    RTC.PITCTRLA |= RTC_PITEN_bm;
}


void show_bases(Base b1, Base b2) {
    VPORTA.OUT = 255;
    VPORTB.OUT = 255;
    switch (b1) {
        case A: // green
            // cast to prevent warning caused by integer promotion to signed...
            VPORTB.OUT &= ~PIN6_bm;
            break;
        case C: // blue
            VPORTB.OUT &= ~PIN4_bm;
            break;
        case G: // yellow
            VPORTA.OUT &= ~PIN4_bm;
            break;
        case T: // red
            VPORTA.OUT &= ~PIN6_bm;
            break;
    }
    
    switch (b2) {
        case A: // green
            // cast to prevent warning caused by integer promotion to signed...
            VPORTB.OUT &= ~PIN7_bm;
            break;
        case C: // blue
            VPORTB.OUT &= ~PIN5_bm;
            break;
        case G: // yellow
            VPORTA.OUT &= ~PIN5_bm;
            break;
        case T: // red
            VPORTA.OUT &= ~PIN7_bm;
            break;
    }
};


const struct sequence *select_sequence(struct config cfg) {
    if (cfg.sequence == MODERNA) {
        return &moderna_sequence;
    } else if (cfg.sequence == PFIZER) {
        return &pfizer_sequence;
    }
    /* this should never happen */ 
    while(1){}
}


void sequence_select_menu(struct config *cfg) {
    while (1) {
        if (button_released > LONG_PRESS_LENGTH) {
            button_released = 0;
            save_config(*cfg); // store config in EEPROM
            break; // start displaying sequence
        } else if (button_released) {
            button_released = 0;
            if (cfg->sequence == MODERNA) {
                cfg->sequence = PFIZER;
            } else if (cfg->sequence == PFIZER) {
                cfg->sequence = MODERNA;
            }
        }
        if (cfg->sequence == MODERNA) {
            VPORTB.OUT = 255;
            VPORTA.OUT = (uint8_t) ~(PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
        } else if (cfg->sequence == PFIZER) {
            VPORTA.OUT = 255;
            VPORTB.OUT = (uint8_t) ~(PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
        }
    }
}


int main(void) {
    
    /* Configure clock prescaler for 1MHz  */
    _PROTECTED_WRITE(
    CLKCTRL.MCLKCTRLB,
    CLKCTRL_PDIV_16X_gc /* Prescaler division: 16X */
    | CLKCTRL_PEN_bm /* Prescaler enable: enabled */
    );
    
    
    /* Enable pullups for low power consumption (20uA -> 0.1uA (afair))*/
    PORTA.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL |= PORT_PULLUPEN_bm;
    
    PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN4CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN5CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN6CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN7CTRL |= PORT_PULLUPEN_bm;
    
    PORTC.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN4CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN5CTRL |= PORT_PULLUPEN_bm;
    
    
    VPORTB.DIR = (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
    VPORTA.DIR = (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
    
    
    /* Enable interrupt on power button
     * (this is only for wakeup)*/
    PORTB.PIN2CTRL = (PORTB.PIN2CTRL & ~PORT_ISC_gm) | PORT_ISC_BOTHEDGES_gc;
    
    init_button();
    sei();
    
    struct config cfg = load_config();
    
    go_to_sleep();
    
    while (1) {
        const struct sequence *current_sequence = select_sequence(cfg);
        for (uint32_t i=0; i< current_sequence->n_bases - 1; i++) {
            show_bases(
                    read_base(current_sequence, i),
                    read_base(current_sequence, i + 1)
                    );
            _delay_ms(LIGHT_DURATION_MS);
            switch_off_leds();
            if (button_released) {
                break;
            }
            _delay_ms(INTERVAL_DURATION_MS);
        }
        
        if (button_released > LONG_PRESS_LENGTH) {
            button_released = 0;
            // long press, switch between sequences
            sequence_select_menu(&cfg);
        } else {
            button_released = 0;
            go_to_sleep();
        }
    }
}
