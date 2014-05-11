#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

void setup() {
  usart_init();
  fdevopen( &ustart_putc, 0);
}

void loop() {

  int sensorValue = analogRead(A0);
  printf("SensorValue %i\n",sensorValue);

  delay(1000); 
}

void usart_init() {

  UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
  UBRR0L = (uint8_t)(BAUD_PRESCALLER);
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));

}

int ustart_putc( char data, FILE *t) 
{
  while(!(UCSR0A & (1<<UDRE0)));
  UDR0 = data;
  
  return 0;
}


