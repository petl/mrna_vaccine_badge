#include "moderna_sequence.h"
#include "pfizer_sequence.h"
#include <avr/sleep.h>

#define green1_led_pin 0
#define green2_led_pin 1
#define blue1_led_pin 2
#define blue2_led_pin 3
#define yellow1_led_pin 4 
#define yellow2_led_pin 5
#define red1_led_pin  4
#define red2_led_pin 7
#define button_pin 8 //is PB2 or PCINT10 or INT0

void setup() {
  //Serial.begin(9600);

}



void loop() {
  int i=0;
  while( i< (4080/4) -1 ){
    uint8_t base_byte = 0;
    base_byte = moderna_sequence_data[i];
        
    //Serial.println(3 - i % 4);
    uint8_t base = 0;
    for(int j=3; j>=0; j--){
      base =  (base_byte >> (2 * (j%4) )) & 0b00000011;
      
      /* 2 bits per base, 00:A, 01:C, 10:G, 11:T */
      if(base == 0){ //A=green
        //Serial.print("A");
        digitalWrite(2, LOW);
        digitalWrite(3, LOW);
      }
      if(base == 1){ //C=Blue
        //Serial.print("C");
        digitalWrite(2, LOW);
        digitalWrite(3, HIGH);
      }
      if(base == 2){ //G=Yellow
        //Serial.print("G");
        digitalWrite(2, HIGH);
        digitalWrite(3, LOW);
      }
      if(base == 3){ //U/T=Red
        //Serial.print("T");
        digitalWrite(2, HIGH);
        digitalWrite(3, HIGH);
      }

      if(digitalRead(button_pin) == LOW){
        while(digitalRead(button_pin) == LOW){
          digitalWrite(2, HIGH);
          digitalWrite(3, HIGH);
        }
        goToSleep();
      }
    }
    i++;
  }


  goToSleep();
}


void wakeUpNow(){
  
}

void goToSleep(){
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0; // Turn off ADC
  attachInterrupt(0 ,wakeUpNow, FALLING); // interrupt : INT0
  sleep_enable();
  sleep_cpu();  // go to sleep...

  sleep_disable();  // wake up !
  detachInterrupt(0);
}
