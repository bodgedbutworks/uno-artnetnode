/*The MIT License (MIT)

Copyright (c) 2014 Nathanaël Lécaudé
https://github.com/natcl/Artnet, http://forum.pjrc.com/threads/24688-Artnet-to-OctoWS2811

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "DIYArtnet.h"

Artnet::Artnet() {}

void Artnet::begin(byte mac[], byte ip[])
{
  #if !defined(ARDUINO_SAMD_ZERO)
    Ethernet.begin(mac,ip);
  #endif

  Udp.begin(ART_NET_PORT);
}

void Artnet::begin()
{
  Udp.begin(ART_NET_PORT);
}

uint16_t Artnet::read()
{
  packetSize = Udp.parsePacket();

  if (packetSize <= MAX_BUFFER_ARTNET && packetSize > 0)
  {
      Udp.read(artnetPacket, MAX_BUFFER_ARTNET);

      // Check that packetID is "Art-Net" else ignore
      for (byte i = 0 ; i < 8 ; i++)
      {
        if (artnetPacket[i] != ART_NET_ID[i])
          return 0;
      }

      opcode = artnetPacket[8] | artnetPacket[9] << 8;

      if (opcode == ART_DMX)
      {
        sequence = artnetPacket[12];
        incomingUniverse = artnetPacket[14] | artnetPacket[15] << 8;
        dmxDataLength = artnetPacket[17] | artnetPacket[16] << 8;

        if (artDmxCallback) (*artDmxCallback)(incomingUniverse, dmxDataLength, sequence, artnetPacket + ART_DMX_START);
        return ART_DMX;
      }
      if (opcode == ART_POLL)
      {
        sendArtPollreply();
        return ART_POLL;
      }
      return 0;
  }
  else
  {
    return 0;
  }
}

void Artnet::sendArtPollreply()
{
  byte replyPack[239] = {'A',
						   'r',
						   't',
						   '-',
						   'N',
						   'e',
						   't',
						   '\0',
						   0,		//OpCodeLo (Hex: 00)
						   33,		//OpCodeHi (Hex: 21)	--> Full OpCode (Hex): 2100 (ArtPollReply)
						   Ethernet.localIP()[0],	//IP Address of this Node
						   Ethernet.localIP()[1],	//IP Address of this Node
						   Ethernet.localIP()[2],	//IP Address of this Node
						   Ethernet.localIP()[3],	//IP Address of this Node
						   54,		//Ethernet Port Lo Byte (Hex: 36)
						   25,		//Ethernet Port Hi Byte (Hex: 19) --> Hex 1936 = Dez 6454
						   1,		//Firmware Revision Hi
						   0,		//Firmware Revision Lo
						   0,		//NetSwitch (??)
						   0,		//SubSwitch (??)
						   1,		//OEM Hi (Manufacturer Code) (Hex:  1)
						   208,		//OEM Lo (Manufacturer Code) (Hex: D0) --> OpCode (Hex): 01D0 ("Hippotizer", "Green Hippo") XD
						   0,		//UBEA Firmware Number (None --> 0)
						   192,		//Status 1 (Various Status bits)
						   119,		//ESTA Manufacturer Code Hi
						   100,		//ESTA Manufacturer Code Lo (--> Used ID of "WET" for now
					int('A'),	//ShortName
					int('e'),	//ShortName
					int('r'),	//ShortName
					int('o'),	//ShortName
					int('T'),	//ShortName
					int('r'),	//ShortName
					int('a'),	//ShortName
					int('x'),	//ShortName
					int(' '),	//ShortName
					int('G'),	//ShortName
					int('r'),	//ShortName
					int('e'),	//ShortName
					int('e'),	//ShortName
					int('N'),	//ShortName
					int('o'),	//ShortName
					int('d'),	//ShortName
					int('e'),	//ShortName
					0,			//ShortName ('\0' to terminate String!)
							int('A'),	//LongName
							int('e'),	//LongName
							int('r'),	//LongName
							int('o'),	//LongName
							int('T'),	//LongName
							int('r'),	//LongName
							int('a'),	//LongName
							int('x'),	//LongName
							int(' '),	//LongName
							int('A'),	//LongName
							int('r'),	//LongName
							int('t'),	//LongName
							int('-'),	//LongName
							int('N'),	//LongName
							int('e'),	//LongName
							int('t'),	//LongName
							int(' '),	//LongName
							int('N'),	//LongName
							int('o'),	//LongName
							int('d'),	//LongName
							int('e'),	//LongName
							int(' '),	//LongName
							int('i'),	//LongName
							int('n'),	//LongName
							int(' '),	//LongName
							int('f'),	//LongName
							int('a'),	//LongName
							int('n'),	//LongName
							int('c'),	//LongName
							int('y'),	//LongName
							int(' '),	//LongName
							int('g'),	//LongName
							int('r'),	//LongName
							int('e'),	//LongName
							int('e'),	//LongName
							int('n'),	//LongName
							int(' '),	//LongName
							int('e'),	//LongName
							int('n'),	//LongName
							int('c'),	//LongName
							int('l'),	//LongName
							int('o'),	//LongName
							int('s'),	//LongName
							int('u'),	//LongName
							int('r'),	//LongName
							int('e'),	//LongName
							int(' '),	//LongName
							int(':'),	//LongName
							int(')'),	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName
							0,	//LongName ('\0' to terminate String!)
					int('S'),	//NodeReport
					int('z'),	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport
					0,	//NodeReport ('\0' to terminate String!)
							0,		//NumPortsHi (Number of physical in/output ports)
							1,		//NumPortsLo (Number of physical in/output ports)
							128,	//PortTypes (Here: DMX, Output) Port 0
							0,		//PortTypes (Here: none) Port 1
							0,		//PortTypes (Here: none) Port 2
							0,		//PortTypes (Here: none) Port 3
							128,	//GoodInput
							128,	//GoodInput
							128,	//GoodInput
							128,	//GoodInput
							128,	//GooOutput
							128,	//GooOutput
							128,	//GooOutput
							128,	//GooOutput
							0,		//SwIn (??)
							0,		//SwIn (??)
							0,		//SwIn (??)
							0,		//SwIn (??)
							0,		//SwOut (??)
							0,		//SwOut (??)
							0,		//SwOut (??)
							0,		//SwOut (??)
							0,		//SwVideo (Deprecated)
							0,		//SwMacro (Deprecated)
							0,		//SwRemote (Deprecated)
							0,		//Spare1
							0,		//Spare2
							0,		//Spare3
							0,		//Style (??)
							int(0xAE),		//MAC Adress (MSB to LSB)
							int(0x20),		//MAC Adress (MSB to LSB)
							int(0x72),		//MAC Adress (MSB to LSB)
							int(0xA8),		//MAC Adress (MSB to LSB)
							int(0xBA),		//MAC Adress (MSB to LSB)
							int(0x55),		//MAC Adress (MSB to LSB)
							Ethernet.localIP()[0],	//IP Address of this Node
						    Ethernet.localIP()[1],	//IP Address of this Node
						    Ethernet.localIP()[2],	//IP Address of this Node
						    Ethernet.localIP()[3],	//IP Address of this Node
							1,		//BindIndex
							14,		//Status2
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0,	//Filler
					0  //Filler
					};

	Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPack, 239);
    Udp.endPacket();
}

void Artnet::printPacketHeader()
{/*
  Serial.print("packet size = ");
  Serial.print(packetSize);
  Serial.print("\topcode = ");
  Serial.print(opcode, HEX);
  Serial.print("\tuniverse number = ");
  Serial.print(incomingUniverse);
  Serial.print("\tdata length = ");
  Serial.print(dmxDataLength);
  Serial.print("\tsequence n0. = ");
  Serial.println(sequence);*/
}

void Artnet::printPacketContent()
{
  for (uint16_t i = ART_DMX_START ; i < dmxDataLength ; i++){
    /*Serial.print(artnetPacket[i], DEC);
    Serial.print("  ");*/
  }
  /*Serial.println('\n');*/
}
