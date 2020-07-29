// Use with Arduino IDE 1.5.2
//
// Ethernet.localIP()
// Ethernet.subnetMask()
// Ethernet.gatewayIP()
// Ethernet.dnsServerIP()
//
// Ethernet uses Pins D4, D10, D11, D12, D13, A0 and A1
// Piezo on Pin 7
// DMX out: D1 (Serial TX)
//
// ArtPollReply implemented in library ArtNet.cpp :)
// Caution: The ArtNetominator prevents ArtPollReply packets from arriving at QLC!


#include "DIYArtnet.h"    // Use lib file in sketch dir for specific ArtPollReply, Serial.prints commented in DIYArtnet.cpp to avoid compilation error
#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <SPI.h>
#include <DMXSerial.h>

Artnet artnet;
//EthernetUDP Udp;

// Change ip and mac address for your setup
byte ip[] = {192, 168, 1, 5};                          // IP if DHCP not available
byte mac[] = {0xAE, 0x20, 0x72, 0xA8, 0xBA, 0xAA};   // GreeNode (FSMach)

long enableSaveTime = 0;
boolean dhcp_successful = false;



void setup(){
  // DMX is output on Hardware serial pin 1 (TX)
  pinMode(7, OUTPUT);
  
  beepiezo(3000, 200);
  beepiezo(4100, 200);
  delay(1000);
  
  if(Ethernet.begin(mac)){    // DHCP succesful 
    beepiezo(2000, 200);
    delay(100);
    beepiezo(2000, 200);
    for (byte thisByte = 0; thisByte < 4; thisByte++){
      ip[thisByte] = Ethernet.localIP()[thisByte];
    }
    dhcp_successful = true;
    artnet.begin();          // !!! Uses given Subnet (DHCP)
  }
  else{    // If == 0 --> DHCP failed
    beepiezo(1500, 200);
    delay(100);
    beepiezo(1900, 200);
    delay(100);
    beepiezo(1500, 200);
    delay(100);
    beepiezo(1900, 200);
    delay(100);
    beepiezo(1500, 200);
    delay(100);
    beepiezo(1900, 200);
    artnet.begin(mac, ip);   // !!! Assumes Subnet /24 (255.255.255.0)
  }
  
  DMXSerial.init(DMXController);
  
  delay(200);
  beepiezo(4100, 200);
  beepiezo(3000, 200);
  artnet.setArtDmxCallback(onDmxFrame);  // This will be called for each received packet
  enableSaveTime = millis();
}



void loop(){
  //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  //Udp.write(artnet.read());
  //Udp.endPacket();
  
  if(dhcp_successful){
    Ethernet.maintain();
    /*
    0: nothing happened
    1: renew failed
    2: renew success
    3: rebind fail
    4: rebind success
    */
  }
  
  artnet.read();
}



void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data){
  for (int i=0 ; i<length ; i++){
    DMXSerial.write(i+1, byte(data[i]));
  }  
  if(data[0] != 0  &&  millis()-enableSaveTime <= 20000){   // Mirror first DMX Channel to Piezo for 30s
    beepiezo(round(map(data[0], 1, 255, 1733, 3467)), 15);  // Frequency Range above and below 2600Hz (Max Loudness of piezo) with exactly one octave (f2=2*f1)
  }
}



void beepiezo(int _freg, int _soundLength){  // Freq in Hz, length in ms
  if(_freg >= 50  &&  _freg <= 20000){
    int T = round(1000000/_freg);   // T in Microseconds
    long m = millis();
    
    while(millis()-m <= _soundLength){
      digitalWrite(7, HIGH);
      delayMicroseconds(T/2);
      digitalWrite(7, LOW);
      delayMicroseconds(T/2);
    }
  }
}
