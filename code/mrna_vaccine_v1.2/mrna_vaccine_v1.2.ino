#include "moderna_sequence.h"
#include "pfizer_sequence.h"
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <PinChangeInterrupt.h>

#define green1_led_pin 0 //PA0
#define green2_led_pin 1//PA1
#define blue1_led_pin 2//PA2
#define blue2_led_pin 3//PA3
#define yellow1_led_pin 4//PA4
#define yellow2_led_pin 5//PA5
#define red1_led_pin  6 //PA6
#define red2_led_pin 7//PA7
#define button_pin 2 //is 8 or PCINT10 or INT0
#define button_pin_arduino 8 //is PB2 or PCINT10 or INT0

uint8_t sequence_option;

//for setting register bits with AVR code
//cbi and sbi are standard (AVR) methods for setting, or clearing, bits in PORT (and other) variables. 
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif 

void setup() {
  //Serial.begin(9600);
  sequence_option = EEPROM.read(10); //read if we have enabled MODERNA or PFIZER; 0==moderna, everything else=pfizer
  
  DDRA = 0b11111111; //LEDs are on PA0-7, so data direction on portA should be OUTPUT for all 
}



void loop() {
  if(sequence_option){
    PORTA = 0b01010101; //show that we've enabled PFIZER
    delay(1000);
  }
  else{
    PORTA = 0b10101010; //show that we've enabled MODERNA
    delay(1000);
  }
  
  int i=0;
  while( i< (4080/4) -1 ){//4080
    uint8_t base_byte = 0;
    if(sequence_option){
      base_byte = pfizer_sequence_data[i];
    }
    else{
      base_byte = moderna_sequence_data[i];
    }
        
    //Serial.println(3 - i % 4);
    uint8_t base_A = 0;
    uint8_t base_B = 0;
    for(int j=1; j>=0; j--){
      base_A =  (base_byte >> (4 * j )) & 0b00000011;
      base_B =  (base_byte >> (2 + 4 * j )) & 0b00000011;

      PORTA = 0b11111111; //all LEDS LOW
      for(int del1=0; del1<10000; del1++){ //10.000@1MHz ^ 10ms
        PORTA = 0b11111111; //delay
      }
      
      /* 2 bits per base, 00:A, 01:C, 10:G, 11:T */
      if(base_A == 0){ //A=green
        //Serial.print("A");
        PORTA &= ~(1<<green1_led_pin); //clear bit
      }
      if(base_A == 1){ //C=Blue
        //Serial.print("C");
        PORTA &= ~(1<<blue1_led_pin); //clear bit
      }
      if(base_A == 2){ //G=Yellow
        //Serial.print("G");yellow1_led_pin
        PORTA &= ~(1<<blue1_led_pin); //clear bit
      }
      if(base_A == 3){ //U/T=Red
        //Serial.print("T");
        PORTA &= ~(1<<red1_led_pin); //clear bit
      }
      
      if(base_B == 0){ //A=green
        //Serial.print("A");
        PORTA &= ~(1<<green2_led_pin); //clear bit
      }
      if(base_B == 1){ //C=Blue
        //Serial.print("C");
        PORTA &= ~(1<<blue2_led_pin); //clear bit
      }
      if(base_B == 2){ //G=Yellow
        //Serial.print("G");
        PORTA &= ~(1<<yellow2_led_pin); //clear bit
      }
      if(base_B == 3){ //U/T=Red
        //Serial.print("T");
        PORTA &= ~(1<<red2_led_pin); //clear bit
      }

      for(int del=0; del<5000; del++){ //10.000@1MHz ^ 10ms
        //do nothing
        uint32_t counter = 0;
        j=0;//restart the sequence
        while(!digitalRead(button_pin_arduino)){ //check if the button was pressed
          counter++;
          PORTA = 0b11000011; //show that we've recognized the button press
          if(counter>10000){
            PORTA = 0b00000000; //all LEDS HIGH
            delay(1000);
            PORTA = 0b11111111; //all LEDS LOW
            while(!digitalRead(button_pin_arduino)){//wait for the button to be repeased
              }
            delay(100);
            goToSleep();
          }  
        }
        if(counter){ //if the button was pressed only for a short time, change the chosen sequence
          if(sequence_option){
            sequence_option=0;
            EEPROM.write(10, sequence_option); 
            for(int blinker=0; blinker<10; blinker++ ){
              PORTA = 0b10101010; //show that we've enabled MODERNA
              delay(50);
              PORTA = 0b11111111; //all off
              delay(50);
              i=0;
            }
          }
          else{         
            sequence_option=1;
            EEPROM.write(10, sequence_option) ;
            for(int blinker=0; blinker<10; blinker++ ){
              PORTA = 0b01010101; //show that we've enabled PFIZER
              delay(50);
              PORTA = 0b11111111; //all off
              delay(50);
              i=0;
            }
          }
        }
        //should be a delay
      }
    i++;
    }
  }
  goToSleep();
}

//external interrupt ISR (for INT0 pin)
ISR(INT0_vect)
{
  DDRA = 0b11111111; //LEDs are on PA0-7, so data direction on portA should be OUTPUT for all 
  PORTA = 0b00000000; //8 LEDs HIGH
  cbi(GIMSK,INT0); //disable external interrupt request 0
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void goToSleep(){
  DDRA = 0b0000000; //all pins to INPUT to save power
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  //ADCSRA = 0; // Turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_bod_disable(); 

  sleep_enable();
  
  cli(); //disable global interrupts  
  // Set ISC01= 1 and ISC00 = 0 to generate an external interrupt request if INT0 is LOW
  cbi(MCUCR,ISC01);
  cbi(MCUCR,ISC00); 
  //enable external interrupt request 0
  sbi(GIMSK,INT0);
  sei(); //enable global interrupts
  
  sleep_cpu();  // go to sleep...

  sleep_disable();  // wake up !
  power_all_enable();    // power everything back on
  delay(1000);
  int sequence_option_old = sequence_option;
  while(!digitalRead(button_pin_arduino)){
    sequence_option=0;
    PORTA = 0b10101010; //show that we've enabled MODERNA
    delay(1000);
    if(digitalRead(button_pin_arduino)){
      break;
    }
    sequence_option=1;
    PORTA = 0b01010101; //show that we've enabled PFIZER
    delay(1000);
    if(digitalRead(button_pin_arduino)){
      break;
    }
  }
  if(sequence_option_old != sequence_option){ //only write to eeprom if option has been changed
    EEPROM.write(10, sequence_option); 
    if(sequence_option){
      for(int blinker=0; blinker<10; blinker++ ){
        PORTA = 0b01010101; //show that we've enabled PFIZER
        delay(50);
        PORTA = 0b11111111; //all off
        delay(50);
      }
    }
    else{
      for(int blinker=0; blinker<10; blinker++ ){
        PORTA = 0b10101010; //show that we've enabled MODERNA
        delay(50);
        PORTA = 0b11111111; //all off
        delay(50);
      }
    }
  }
  resetFunc(); //start from the beginning after waking up 
  //detachInterrupt(0);
}
