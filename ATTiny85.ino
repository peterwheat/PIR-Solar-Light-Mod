#include <avr/sleep.h>
#include <avr/interrupt.h>
#define __AVR_ATtiny85__
//Pins
#define SNDMSG 0
#define LIGHT 1
#define PIR 4
#define SOLAR A3

bool dark() {
  int dval = 0;
  bool d = false;
  dval = analogRead(SOLAR);
  if(dval < 100) d = true;
  return d;
}

void flash() {
   digitalWrite(LIGHT, HIGH); 
   delay(15000);
   digitalWrite(LIGHT, LOW); 
}

void sendMsg(){
  digitalWrite(SNDMSG,HIGH);
  delay(1000);
  digitalWrite(SNDMSG,LOW);
  delay(120000);  //keep battery power stable during SNDMSG (Blockade time)
}

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
  return result;
}  //readVcc

void sleep() {
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT4);                   // Use PB4 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep
    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on
    sei();                                  // Enable interrupts
} // sleep

ISR(PCINT0_vect) {
    // This is called when the interrupt occurs, but I don't need to do anything in it
}
    
void setup(){
  pinMode(LIGHT,OUTPUT);
  pinMode(SNDMSG,OUTPUT);
  pinMode(SOLAR,INPUT);
  pinMode(PIR,INPUT);
}

void loop() {
  if(readVcc()<3000){  //Is battery really low?
    sleep();  
  } else {
    sleep();  //Sleep until movement detected.
    if(dark()) flash();
    sendMsg();
  }
}
