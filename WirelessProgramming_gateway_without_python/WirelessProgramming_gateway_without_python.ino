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
#include <WirelessProg_Gateway.h> //modify from the original source: get it here: https://github.com/LowPowerLab/WirelessProgramming/tree/master/WirelessHEX69
//#include <avr/pgmspace.h> // lib to use the PROGMEM memory (flash memory)
#include <Ethernet.h>

#define NODEID             001 //this node's ID, should be unique among nodes on this NETWORKID
#define NETWORKID          100  //what network this node is on
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MH
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey" //(16 bytes of your choice - keep the same on all encrypted nodes)
//#define IS_RFM69HW             //uncomment only for RFM69HW! Leave out if you have RFM69W!

#define SERIAL_BAUD 9600
#define ACK_TIME    50  // # of ms to wait for an ack
#define TIMEOUT     800

#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
#else
  #define LED           9 // Moteinos hsave LEDs on D9
#endif

RFM69 radio;
char c = 0;
char input[64]; //serial input buffer
byte targetID=0;

// L'adresse MAC du shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xA5, 0x7E };
// L'objet qui nous servira a la communication
EthernetClient client;

void setup(){
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  //radio.encrypt(ENCRYPTKEY); //OPTIONAL
#ifdef IS_RFM69HW
  radio.setHighPower(); //only for RFM69HW!
#endif
  delay(1000);
  
  Ethernet.select(4);

  char erreur = 0;
  // On démarre le shield Ethernet SANS adresse ip (donc donnée via DHCP)
  erreur = Ethernet.begin(mac);


  // Donne une seconde au shield pour s'initialiser
  delay(1000);
}

void loop(){

    // Length of the input: things that will be sent to the target
    byte inputLen;
    // input it what will actually be sent to the targeted moteino

    // For test
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
    // End of test
   
  // Look if the input is FLX? That will initialise the handcheck with the target
  if (inputLen==4 && input[0]=='F' && input[1]=='L' && input[2]=='X' && input[3]=='?') {
    // If no target has been set, the code won,t try anything
    if (targetID==0)
      Serial.println("TO?");
    else
      CheckForSerialHEX((byte*)input, inputLen, radio, targetID, TIMEOUT, ACK_TIME, &client, true);
      /// returns TRUE if a HEX file transmission was detected and it was actually transmitted successfully
  }
  // Look if the input is the initialization of the target (TO: NodeID)
  else if (inputLen>3 && inputLen<=6 && input[0]=='T' && input[1]=='O' && input[2]==':')
  {
    byte newTarget=0;
    for (byte i = 3; i<inputLen; i++){ //up to 3 characters for target ID, remove the TO:, keeping the NodeID
      if (input[i] >=48 && input[i]<=57){
        newTarget = newTarget*10+input[i]-48; // set newTarget = NodeID (10 for the decimal in NodeID; -48 for the int convertion
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
  // If the input is neither FLX? nor TO:NodeID, it should be a HEX line.
  else if (inputLen>0) { //just echo back
    Serial.print("SERIAL IN > ");Serial.println(input);
  }
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
}
