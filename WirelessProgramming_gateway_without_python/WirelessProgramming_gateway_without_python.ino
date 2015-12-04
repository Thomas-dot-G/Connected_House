// **********************************************************************************
// This sketch is an example of how wireless programming can be achieved with a Moteino
// that was loaded with a custom 1k bootloader (DualOptiboot) that is capable of loading
// a new sketch from an external SPI flash chip
// This is the GATEWAY node, it does not need a custom Optiboot nor any external FLASH memory chip
// (ONLY the target node will need those)
// The sketch includes logic to receive the new sketch from the serial port (from a host computer) and 
// transmit it wirelessly to the target node
// The handshake protocol that receives the sketch from the serial port 
// is handled by the SPIFLash/WirelessHEX69 library, which also relies on the RFM69 library
// These libraries and custom 1k Optiboot bootloader for the target node are at: http://github.com/lowpowerlab
// **********************************************************************************
// Copyright Felix Rusu, LowPowerLab.com
// Library and code by Felix Rusu - felix@lowpowerlab.com
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// You should have received a copy of the GNU General    
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************
#include <RFM69.h>          //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <SPIFlash.h>      //get it here: https://www.github.com/lowpowerlab/spiflash
#include <WirelessHEX69.h> //get it here: https://github.com/LowPowerLab/WirelessProgramming/tree/master/WirelessHEX69
#include <SD.h>
#include <avr/pgmspace.h>

#define NODEID             254  //this node's ID, should be unique among nodes on this NETWORKID
#define NETWORKID          249  //what network this node is on
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MH
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey" //(16 bytes of your choice - keep the same on all encrypted nodes)
//#define IS_RFM69HW             //uncomment only for RFM69HW! Leave out if you have RFM69W!

#define SERIAL_BAUD 115200
#define ACK_TIME    50  // # of ms to wait for an ack
#define TIMEOUT     300

#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
#else
  #define LED           9 // Moteinos hsave LEDs on D9
#endif

//String HEX PROGMEM = "FLX:100000000C9463000C94B8070C94E5070C948B00D7;FLX:100010000C948B000C948B000C948B000C948B0034;FLX:100020000C948B000C948B000C948B000C948B0024;FLX:100030000C948B000C948B000C948B000C948B0014;FLX:100040000C9412080C948B000C94F40A0C94260B5C;FLX:100050000C948B000C948B000C948B000C948B00F4;FLX:10055000FACF40E065E2CE010E94DE011E3308F0D2;FLX:100560001DE3E11004C0002319F000E401C000E81D;FLX:10057000E881F981008CF18DE02DCE01099580E8AC;FLX:100580000E943A0183E0810F0E943A018F2D0E9460;FLX:100590003A018D810E943A01802F0E943A010D2D6F;FLX:1005A000ED2CFC2C8E2D801B811730F4F7018191EE;FLX:1018300069F760957095809590959B01AC01BD010D;FLX:10184000CF010895EE0FFF1F0590F491E02D09944C;FLX:04185000F894FFCF3A;FLX:1018540000009A02800073616D706C65456E63725E;FLX:101864007970744B6579005374617274206E6F647F;FLX:10187400652E2E2E0053504920466C617368204912;FLX:101884006E6974204F4B210053504920466C61739C;FLX:101894006820496E6974204641494C210048656CB2;FLX:1018A4006C6F0000000000A30561011801F0054EF3;FLX:1018B400067502F104E7008F02100306026E0602A9;FLX:1018C40001F300000000002D0AA90BAF09C809BAF2;FLX:1018D400090B0A0D0A006E616E00696E66006F7670;FLX:0218E40066009C;FLX:00000001FF";

RFM69 radio;
char c = 0;
char input[64]; //serial input buffer
byte targetID=0;

void setup(){
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(ENCRYPTKEY); //OPTIONAL
#ifdef IS_RFM69HW
  radio.setHighPower(); //only for RFM69HW!
#endif
  Serial.println("Start wireless gateway...");
}

void loop(){
  byte inputLen = readSerialLine(input, 10, 64, 100); //readSerialLine(char* input, char endOfLineChar=10, byte maxLength=64, uint16_t timeout=1000);
    //char s[64] = "FLX:58:1003A0000E940013F394ECCF84E294E00E94471281";
    if(targetID){
      inputLen = 4;
      char s[64] = "FLX?";
      strncpy(input, s, 64);
    }
    else{
      inputLen = 6;
      char s[64] = "TO:123";
      strncpy(input, s, 64);
    }
    //char s[64] = "FLX:100000000C9463000C94B8070C94E5070C948B00D7";
    
    //char* lines = strtok(HEX, ";");
    //for(int i=0; i<sizeof(s); i=i+1){ Serial.println(s[i]);}
    //input = "FLX:58:1003A0000E940013F394ECCF84E294E00E94471281";
    //strncpy(input, s, 64);
    Serial.println("THE INPUT");
    Serial.println(input);
    Serial.println("END");
  if (inputLen==4 && input[0]=='F' && input[1]=='L' && input[2]=='X' && input[3]=='?') {
    if (targetID==0)
      Serial.println("TO?");
    else
      CheckForSerialHEX((byte*)input, inputLen, radio, targetID, TIMEOUT, ACK_TIME, true);
  }
  else if (inputLen>3 && inputLen<=6 && input[0]=='T' && input[1]=='O' && input[2]==':')
  {
    byte newTarget=0;
    for (byte i = 3; i<inputLen; i++){ //up to 3 characters for target ID
      if (input[i] >=48 && input[i]<=57){
        Serial.println((input[i]));
        newTarget = newTarget*10+input[i]-48;
        Serial.println(newTarget);
      }
      else
      {
        newTarget=0;
        break;
      }
    }
    if (newTarget>0)
    {
      targetID = newTarget;
      Serial.print("TO:");
      Serial.print(newTarget);
      Serial.println(":OK");
    }
    else
    {
      Serial.print(input);
      Serial.print(":INV");
    }

  }
  else if (inputLen>0) { //just echo back
    Serial.print("SERIAL IN > ");Serial.println(input);
  }
  Serial.print("Radio:");
  Serial.print(radio.receiveDone());
  if (radio.receiveDone())
  {
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    
    if (radio.ACK_REQUESTED)
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
    }
    
    Serial.println();
  }
  Blink(LED,5); //heartbeat
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
