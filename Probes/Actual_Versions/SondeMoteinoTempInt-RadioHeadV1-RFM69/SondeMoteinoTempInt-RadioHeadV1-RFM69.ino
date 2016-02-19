// rf69_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

#include <SPI.h>
#include <RFM69.h> //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPIFlash.h>  //get it here: https://www.github.com/lowpowerlab/spiflash
#include <OneWire.h> // Inclusion de la librairie OneWire
#include <EEPROM.h>
//#include <avr/wdt.h>
#include <WirelessHEX69.h> //get it here: https://github.com/LowPowerLab/WirelessProgramming/tree/master/WirelessHEX69
 
#define DS18B20 0x28     // Adresse 1-Wire du DS18B20
#define BROCHE_ONEWIRE 3 // Broche utilisée pour le bus 1-Wire

#define DEBUG 1
#define DISTRIBUTED_DEBUG     0
#define VERSION 001
#define SERIAL_BAUD   9600
#define FREQUENCY   RF69_433MHZ
#define ACK_TIME    30  // # of ms to wait for an ack
//#define ENCRYPTKEY "sampleEncryptKey" //(16 bytes of your choice - keep the same on all encrypted nodes)
#ifdef __AVR_ATmega1284P__
  #define FLASH_SS      23 // and FLASH SS on D23
#else
  #define FLASH_SS      8 // and FLASH SS on D8
#endif
#define LED           9 // Moteinos have LEDs on D9

// ADDRID is the address of the ID in the EEPROM, SETPEEROM is a boolean to setup the ID in the EEPROM, IDTOSETUP is the ID to put in the EEPROM and ID is the actual ID read from the EEPROM. Same schema for others
//#define SETUPEEPROM 0
#define ADDRID 0
#define IDTOSETUP 002
#define ADDRNODEID 10
#define NODEIDTOSETUP 002
#define ADDRNETWORKID 20
#define NETWORKIDTOSETUP 100
#define ADDRGATEWAYID 30
#define GATEWAYIDTOSETUP 001

long ID = 2;
long NODEID = 2;
long NETWORKID = 100;
long GATEWAYID = 001;

long TRANSMITPERIOD = 20000; //transmit a packet to gateway so often (in ms)
long lastPeriod = 0;
RFM69 radio;
char input = 0;

SPIFlash flash(FLASH_SS, 0xEF30); //EF30 for windbond 4mbit flash
// Singleton instance of the radio driver

//RH_RF69 rf69(15, 16); // For RF69 on PJRC breakout board with Teensy 3.1

OneWire ds(BROCHE_ONEWIRE); // Création de l'objet OneWire ds
 
// Fonction récupérant la température depuis le DS18B20
// Retourne true si tout va bien, ou false en cas d'erreur
boolean getTemperature(float *temp){
byte data[9], addr[8];
  // data : Données lues depuis le scratchpad
  // addr : adresse du module 1-Wire détecté
 
  if (!ds.search(addr)) { // Recherche un module 1-Wire
    ds.reset_search();    // Réinitialise la recherche de module
    return false;         // Retourne une erreur
  }
   
  if (OneWire::crc8(addr, 7) != addr[7]) // Vérifie que l'adresse a été correctement reçue
    return false;                        // Si le message est corrompu on retourne une erreur
 
  if (addr[0] != DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
    return false;         // Si ce n'est pas le cas on retourne une erreur
 
  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
   
  ds.write(0x44, 1);      // On lance une prise de mesure de température
  delay(800);             // Et on attend la fin de la mesure
   
  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
  ds.write(0xBE);         // On envoie une demande de lecture du scratchpad
 
  for (byte i = 0; i < 9; i++) // On lit le scratchpad
    data[i] = ds.read();       // Et on stock les octets reçus
   
  // Calcul de la température en degré Celsius
  *temp = ((data[1] << 8) | data[0]) * 0.0625; 
   
  // Pas d'erreur
  return true;
}

char *ftoa(char *a, double f, int precision)
{
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
 
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long desimal = abs((long)((f - heiltal) * p[precision]));
 itoa(desimal, a, 10);
 return ret;
}

void setup() 
{
  Serial.begin(SERIAL_BAUD);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  
  Serial.println("init");
  //delay(10000);

  // Wireless programming setup
  pinMode(LED, OUTPUT);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  //radio.encrypt(ENCRYPTKEY); //OPTIONAL
  #ifdef IS_RFM69HW
    radio.setHighPower(); //only for RFM69HW!
  #endif
  Serial.print("Start node...");

  if (flash.initialize())
    Serial.println("SPI Flash Init OK!");
  else
    Serial.println("SPI Flash Init FAIL!");
}

void parseMessage(char *Message, char * Response, int value) {
  int cpt = 0;
 // Serial.println(Message);

  for (int i=0; (i<100) & (i<(strlen(Message) ) ); ) {
    if (cpt==value) {
      for (int j=0; (Message[i]!=';' & (i<100) & (i<(strlen(Message) ) ) ); ) {
     //  Serial.print(cpt);Serial.print('-');Serial.print(i);Serial.print('-');Serial.print(j);

        Response[j] = Message[i];
        j++;
        i++;
      }
      return ;
    } 
    
    if (Message[i] == ';') {
      cpt++;
    }
    i++;
     
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

// [SousReseau,To,From,Type,Version,...]
// [NETWORKID;GATEWAY;NODEID;TYPE;VERSION...]

void sendTemp(){
  char temp1[20];
  
  
  int currPeriod = millis()/TRANSMITPERIOD;
  if (currPeriod != lastPeriod)
  {
    lastPeriod=currPeriod;
  if (DEBUG==1) Serial.println("Loop");
  
  float temp;
  char MessageServeur[100];
  int   sendSize = 0;
  char buf2[20];
  
  // Lit la température ambiante à ~1Hz
 //if(getTemperature(&temp)) {
 temp = random(-30,0);
     if (DEBUG ==1 ) {
      // Affiche la température
      Serial.print("Temperature : ");
      Serial.print(temp);
      Serial.write(' '); // caractère °
      Serial.write('C');
      Serial.println();
     }
  //}
  //delay(500);



  Serial.println("Data Serveur:");  
  memset(MessageServeur,'\0',100);
  sprintf(MessageServeur,"%i;%i;%i;%i;%i;%s;",NETWORKID,GATEWAYID,NODEID,ID,VERSION,ftoa(buf2,temp,2));
  sendSize = strlen(MessageServeur);
  Serial.println(MessageServeur);
    
    if (DEBUG==1) {
      Serial.println();

      Serial.print(" Sending[");
      Serial.print(sendSize);
      Serial.print(" octets]: to : ");
      Serial.print("server");
      Serial.print(" Message[");
      Serial.print( MessageServeur );
      Serial.print("]");
    }
    
  // Send a message to rf69_server
  uint8_t data[100];
  for (int i=0;i<100;i++)
    data[i]=MessageServeur[i];

     Blink(LED,3);

  radio.sendWithRetry(GATEWAYID, data, sendSize);
  //rf69.send(data, sendSize);
  
  //rf69.waitPacketSent();
  // Now wait for a reply


  if (radio.ACKReceived(GATEWAYID)) // ACKREQUESTED ?
  { 
    // Should be a reply message for us now   
/*    if (rf69.recv(buf, &len))
    {
     if (DEBUG==1) {
        Serial.print("got reply: ");
        Serial.println((char*)buf);
      }
      memset(temp1,'\0',20);
      parseMessage((char*)buf,temp1,0);
      Serial.println(temp1);
      if (strncmp((char*)temp1,(char*)NETWORKID,3)==0) {
        memset(temp1,'\0',20);
        parseMessage((char*)buf,temp1,1);  
        Serial.println(temp1);
        if (strncmp((char*)temp1,(char*)NODEID,3)==0) {
            memset(temp1,'\0',20);
            parseMessage((char*)buf,temp1,3); 
            Serial.println(temp1);
            if (strncmp((char*)temp1,"ACK",3)==0) {
              Serial.println("--> ACK");
            }
        }
      }*/
      radio.sendACK();
        
      
    }
    else
    {
      Serial.println("NO ACK");
    }
  }
  else
  {
    //Serial.println("No reply, is rf69_server running?");
  }
  delay(400);
}



void loop()
{
  // Setup of the ID
  #ifdef SETUPEEPROM
    EEPROM.write(ADDRID, IDTOSETUP);
    EEPROM.write(ADDRNODEID, NODEIDTOSETUP);
    EEPROM.write(ADDRNETWORKID, NETWORKIDTOSETUP);
    EEPROM.write(ADDRGATEWAYID, GATEWAYIDTOSETUP);
    ID = EEPROM.read(ADDRID);
    NODEID = EEPROM.read(ADDRNODEID);
    NETWORKID = EEPROM.read(ADDRNETWORKID);
    GATEWAYID = EEPROM.read(ADDRGATEWAYID);
  #else
    ID = EEPROM.read(ADDRID);
    NODEID = EEPROM.read(ADDRNODEID);
    NETWORKID = EEPROM.read(ADDRNETWORKID);
    GATEWAYID = EEPROM.read(ADDRGATEWAYID);
  #endif
  
  // Check for existing RF data, potentially for a new sketch wireless upload
  // For this to work this check has to be done often enough to be
  // picked up when a GATEWAY is trying hard to reach this node for a new sketch wireless upload
  
  if (radio.receiveDone())
  {
    Serial.print("Got [");
    Serial.print(radio.SENDERID);
    Serial.print(':');
    Serial.print(radio.DATALEN);
    Serial.print("] > ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i], HEX);
    Serial.println();
    CheckForWirelessHEX(radio, flash, true);
    Serial.println();
  } else {
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  // Normal sketch code here
   sendTemp();
  ////////////////////////////////////////////////////////////////////////////////////////////
  }
}

