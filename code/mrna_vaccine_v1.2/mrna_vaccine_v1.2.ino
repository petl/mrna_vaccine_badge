#include "moderna_sequence.h"
#include "pfizer_sequence.h"
#include <EEPROM.h>
#include <avr/sleep.h>

#define green1_led_pin PA0
#define green2_led_pin PA1
#define blue1_led_pin PA2
#define blue2_led_pin PA3
#define yellow1_led_pin PA4
#define yellow2_led_pin PA5
#define red1_led_pin  PA6 
#define red2_led_pin PA7
#define button_pin PB2 //is 8 or PCINT10 or INT0
#define button_pin_arduino 8 //is PB2 or PCINT10 or INT0

uint8_t sequence_option;

void setup() {
  //Serial.begin(9600);
  sequence_option = EEPROM.read(10); //read if we have enabled MODERNA or PFIZER; 0==moderna, everything else=pfizer
  
  DDRB = 0b11111111; //LEDs are on PA0-7, so data direction on portA should be OUTPUT for all 

}



void loop() {
  int i=0;
  while( i< (4080/4) -1 ){
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

      PORTA = 0b00000000; //all LEDS LOW
      
      /* 2 bits per base, 00:A, 01:C, 10:G, 11:T */
      if(base_A == 0){ //A=green
        //Serial.print("A");
        PORTB |= (1 << green1_led_pin);
      }
      if(base_A == 1){ //C=Blue
        //Serial.print("C");
        PORTB |= (1 << blue1_led_pin);
      }
      if(base_A == 2){ //G=Yellow
        //Serial.print("G");
        PORTB |= (1 << yellow1_led_pin);
      }
      if(base_A == 3){ //U/T=Red
        //Serial.print("T");
        PORTB |= (1 << red1_led_pin);
      }
      
      if(base_B == 0){ //A=green
        //Serial.print("A");
        PORTB |= (1 << green2_led_pin);
      }
      if(base_B == 1){ //C=Blue
        //Serial.print("C");
        PORTB |= (1 << blue2_led_pin);
      }
      if(base_B == 2){ //G=Yellow
        //Serial.print("G");
        PORTB |= (1 << yellow2_led_pin);
      }
      if(base_B == 3){ //U/T=Red
        //Serial.print("T");
        PORTB |= (1 << red1_led_pin);
      }

      for(int del=0; del<10000; del++){ //10.000@1MHz ^ 10ms
        //do nothing
        int counter = 0;
        while(!digitalRead(button_pin_arduino)){
          if(sequence_option){
            sequence_option=0;
            EEPROM.write(10, sequence_option);
            PORTA = 0b00001111; //show that we've enabled MODERNA
            for(int del2=0; del2<10000; del2++){ //10.000@1MHz ^ 10ms
              //
            }
          }
          else{         
            sequence_option=1;
            EEPROM.write(10, sequence_option) ;
            PORTA = 0b11110000; //show that we've enabled PFIZER
            for(int del2=0; del2<10000; del2++){ //10.000@1MHz ^ 10ms
              //
            }
          }

          if(counter>100000){
            PORTA = 0b00000000; //all LEDS LOW
            goToSleep();
          }
          counter++;
        }
        //should be a delay
      }
    i++;
    }
  }
  goToSleep();
}


void wakeUpNow(){
  
}

void goToSleep(){
  while(!digitalRead(button_pin_arduino)){
    
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0; // Turn off ADC
  attachInterrupt(0 ,wakeUpNow, FALLING); // interrupt : INT0
  sleep_enable();
  sleep_cpu();  // go to sleep...

  sleep_disable();  // wake up !
  detachInterrupt(0);
}
