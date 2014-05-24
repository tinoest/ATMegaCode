#include <JeeLib.h> // https://github.com/jcw/jeelib
#include "DHT.h"

#define MYNODE  30            // Reciever ID
#define FREQ    RF12_433MHZ   // Frequency
#define GROUP   210           // Group
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

long readVcc();
void usart_init();
int usart_putc( char data, FILE *t);

typedef struct {
  uint16_t rxD;              // sensor value
  uint16_t tmpC;
  uint16_t supplyV;          // tx voltage
} 
Payload;

Payload rx;

uint8_t nodeID;    //node ID of tx, extracted from RF datapacket. Not transmitted as part of structure

#define DHTPIN 3 
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime;
unsigned long counter;

void setup () {

  usart_init();
  fdevopen( &usart_putc, 0);
  
  rf12_initialize(MYNODE,FREQ,GROUP); // Initialise the RFM12B
  dht.begin();
  lastTime = millis();

  // Baud Rate = b , Node = n , Group = g , Freq = f , SW = Software Version
  printf_P(PSTR("<xml><b>%i</b><n>%i</n><f>%i</f><g>%i</g><sw>0.02</sw></xml>\n"), BAUDRATE , MYNODE , FREQ , GROUP );

}

void loop() {

  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
    nodeID = rf12_hdr & 0x1F;  // get node ID
    rx = *(Payload*) rf12_data;
    uint16_t value = rx.rxD;
    uint16_t tmpC  = rx.tmpC;
    uint16_t mV    = rx.supplyV;

    if (RF12_WANTS_ACK) {                  // Send ACK if requested
      rf12_sendStart(RF12_ACK_REPLY, 0, 0);

    }

    printf_P(PSTR("<xml><n>%i</n><c>%i</c><tmpr>%i</tmpr><v>%i</v></xml>\n"), nodeID , value , tmpC , mV );

  }

  if(((millis() - lastTime) > 30000) || ((millis() - lastTime) < 0)) { 
    counter++;
    lastTime        = millis();
    int16_t humidty = (int16_t)dht.readHumidity();
    int16_t tmpC    = (int16_t)dht.readTemperature();
    uint16_t mV     = readVcc();
    printf_P(PSTR("<xml><n>%i</n><c>%lu</c><tmpr>%i</tmpr><h>%i</h><v>%i</v></xml>\n"), MYNODE , counter , tmpC , humidty , mV );  
  }

}

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void usart_init() {

  UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
  UBRR0L = (uint8_t)(BAUD_PRESCALLER);
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));

}

int usart_putc( char data, FILE *t) 
{
  while(!(UCSR0A & (1<<UDRE0)));
  UDR0 = data;

  return 0;
}




