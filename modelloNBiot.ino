// Demo per Lab 42
// Basato su https://docs.arduino.cc/hardware/mkr-nb-1500
#include <MKRNB.h>
#include "Arduino_SensorKit.h"

// Define Pin
#define LIGHT A3
	
// variabili globali
int light_value;
int16_t msg_int;
float msg_float;
int idx;

unsigned int serverPort = ;// Aggiungere porta
unsigned int localPort = 5430;              // local port to listen for UDP packets
IPAddress server(51, 159, 156, 42);         // Ip 42iot.staersistemi.com
const int UDP_PACKET_SIZE = 48;             // UDP max packet size
byte packetBuffer[UDP_PACKET_SIZE];         // buffer to hold incoming and outgoing packets

// inizializza le librerie
NBClient client;
GPRS gprs;
NB nbAccess;
NBUDP Udp;

int fillBuffer(int idx, float message){
  float payload = message;  
  byte * b = (byte *) &payload;
  packetBuffer[idx] = (b[0]);
  packetBuffer[idx+1] = (b[1]);
  packetBuffer[idx+2] = (b[2]);
  packetBuffer[idx+3] = (b[3]);

  return (idx+4);
}

int fillBuffer(int idx, int16_t message){
  int16_t payload = message; 
  packetBuffer[idx] = lowByte(payload);
  packetBuffer[idx + 1] = highByte(payload);

  return (idx+2);
}

void sendUDPpacket(IPAddress& address){
  Udp.beginPacket(address, serverPort); //NTP requests are to port 123
  Udp.write(packetBuffer, UDP_PACKET_SIZE);
  Udp.endPacket();
}

void setup(){
  Serial.begin(9600);           // Open serial communications and wait for port to open
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino GPRS NTP client.");
  boolean connected = false;    // connection state
  while (!connected) {
    if ((nbAccess.begin("") == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);

  pinMode(LIGHT , INPUT);
}

void loop(){
    idx=0;
    memset(packetBuffer, 0, UDP_PACKET_SIZE);       // Clean buffer 
    
    msg_int = int16_t(analogRead(LIGHT));           // Lettura luminosità
    idx = fillBuffer(idx, msg_int);

    sendUDPpacket(server);                          // Invio pacchetto UDP
    delay(10000);
}
