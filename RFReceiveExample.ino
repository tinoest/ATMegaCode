#include <JeeLib.h> // https://github.com/jcw/jeelib


#define MYNODE  30            // Reciever ID
#define FREQ    RF12_433MHZ     // Frequency
#define GROUP   210            // Group

char xml[100];

typedef struct {
  int rxD;              // sensor value
  int tmpC;
  int supplyV;          // tx voltage
} 
Payload;
Payload rx;

int nodeID;    //node ID of tx, extracted from RF datapacket. Not transmitted as part of structure

void setup () {

  Serial.begin(9600);
  rf12_initialize(MYNODE,FREQ,GROUP); // Initialise the RFM12B

  // Node = n , Group = g , Freq = f , SW = Software Version
  memset(xml, 0, sizeof(xml));
  sprintf(xml ,"<xml><n>%i</n><f>%i</f><g>%i</g><sw>0.01</sw></xml>", MYNODE , FREQ , GROUP );
  Serial.println(xml);

}

void loop() {

  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
    nodeID = rf12_hdr & 0x1F;  // get node ID
    rx = *(Payload*) rf12_data;
    int value = rx.rxD;
    int tmpC  = rx.tmpC;
    int mV    = rx.supplyV;

    if (RF12_WANTS_ACK) {                  // Send ACK if requested
      rf12_sendStart(RF12_ACK_REPLY, 0, 0);
      Serial.println("-> ack sent");
    }

    memset(xml, 0, sizeof(xml));
    sprintf(xml ,"<xml><n>%i</n><c>%i</c><tmpr>%i</tmpr><v>%i</v></xml>", nodeID , value , tmpC , mV );
    Serial.println(xml);
    /*
  Serial.print("Received a packet from Node: ");
     Serial.print(nodeID);
     Serial.print(" Value: ");
     Serial.print(value);
     Serial.print(" TX TempC: ");
     Serial.print(tmpC);
     Serial.print(" TX Millivolts: ");
     Serial.println(milliVolts);
     */
    //Serial.println("--------------------");
  }

}


