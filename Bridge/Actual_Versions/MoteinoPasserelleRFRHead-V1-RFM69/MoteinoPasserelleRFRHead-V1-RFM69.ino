// rf69_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_client
// Demonstrates the use of AES encryption, setting the frequency and modem
// configuration.
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

#include <SPI.h>
#include <RFM69.h>

#include <EEPROM.h>
#include <Ethernet.h>
#include <utility/W5100.h>

// Singleton instance of the radio driver

//RH_RF69 rf69(15, 16); // For RF69 on PJRC breakout board with Teensy 3.1
//RH_RF69 rf69(4, 2); // For MoteinoMEGA https://lowpowerlab.com/shop/moteinomega
#define LED 9
#define FREQUENCY     RF69_433MHZ
#define ACK_TIME      300 // max # of ms to wait for an ack
#define SERIAL_BAUD   9600
#define GATEWAY "001"
char writeAPIKeyElec[] = "c33f1ab16fc77cb03b0fdc7ad11a5735";
char writeAPIKeyMeteo[] = "OD8OB7F3CWE25CLE";

// ADDRID is the address of the ID in the EEPROM, SETPEEROM is a boolean to setup the ID in the EEPROM, IDTOSETUP is the ID to put in the EEPROM and ID is the actual ID read from the EEPROM. Same for the others.
//#define SETUPEEPROM 0
#define ADDRID 0
#define IDTOSETUP 001
#define ADDRNODEID 10
#define NODEIDTOSETUP 005
#define ADDRNETWORKID 20
#define NETWORKIDTOSETUP 100

long ID = 1;
long NODEID = 5;
long NETWORKID = 100;

#define DEBUG 1
#define DEBUG2 1
#define REPLY 1
#define ENABLE_IDLE_STBY_RF 0

#define MAX_JSON 100

// Cons to update the modules
#define VERSION_MODULE_TEMP 1 // Version number of the emp module
#define NUMBER_OF_MODULES 1 // Number of different modules existing (1 here, the temperature module)

RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

char thingSpeakAddress[] = "kgb.emn.fr";
int server_port = 8001;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetClient client;

void startEthernet()
{
  Serial.println("Start Ethernet");
  
  client.stop();

  if (DEBUG==1) Serial.println(F("Connecting Arduino to network..."));
  if (DEBUG==1)  Serial.println();  

  delay(4000);
  // Change the PIN used for the ethernet
  Ethernet.select(4);

  
  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    if (DEBUG==1) Serial.println(F("DHCP Failed, reset Arduino to try again"));
    if (DEBUG==1) Serial.println();
  }
  else
  {
    if (DEBUG==1) Serial.println(F("Arduino connected to network using DHCP"));
    if (DEBUG==1) Serial.println();
  }
  
  if (DEBUG==1) {
    Serial.print("My IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print(".");
    }
    Serial.println();
  }
  W5100.setRetransmissionTime(0x07D0); //where each unit is 100us, so 0x07D0 (decimal 2000) means 200ms.
  W5100.setRetransmissionCount(3); //That gives me a 3 second timeout on a bad server connection.

  delay(100);
}


void setup() 
{
  Serial.begin(9600);
  Serial.println("Setup");

  radio.initialize(FREQUENCY,NODEID,NETWORKID);

//  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);
  char buff[50];
  if (DEBUG==1) sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  if (DEBUG==1) Serial.println(buff);

   startEthernet();

}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}


// Parse the message into the response according to the value which gives the value-th element
void parseMessage(char *Message, char * Response, int value) {
  int cpt = 0;
 // Serial.println(Message);

  for (int i=0; (i<100) & (i<(strlen(Message) ) ); ) {
    // We look for the right element to take into the message
    if (cpt==value) {
      for (int j=0; (Message[i]!=';' & (i<100) & (i<(strlen(Message) ) ) ); ) {
     //  Serial.print(cpt);Serial.print('-');Serial.print(i);Serial.print('-');Serial.print(j);

        Response[j] = Message[i];
        j++;
        i++;
      }
      return ;
    } 

    // If we move to the next element, we increment the cpt
    if (Message[i] == ';') {
      cpt++;
    }
    i++;
     
  }
}

// Update the data for TeleIC
void UpdateTeleIC(char *Data){
  
  char MessageServeur[100]="";
  char Inst[20]="";
  char hchc[20]="";
  char hchp[20]="";

  memset(MessageServeur,'\0',100);
    
   //if (DEBUG==1) { Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]"); }
   if (DEBUG==1) Serial.println();
  
    memset(Inst,'\0',20);
    parseMessage(Data,Inst,5);
    if (DEBUG==1) Serial.print("IINST :");   
    if (DEBUG==1) Serial.println(Inst);
    
    memset(hchp,'\0',20);
    parseMessage(Data,hchp,6);
    if (DEBUG==1) Serial.print("HCHP :");   
    if (DEBUG==1) Serial.println(hchp);
    
    memset(hchc,'\0',20);
    parseMessage(Data,hchc,7);
    if (DEBUG==1) Serial.print("HCHC :");    
    if (DEBUG==1) Serial.println(hchc);
   
    sprintf(MessageServeur,"field4=%s&field5=%s&field6=%s",hchp,hchc,Inst);
    
    updateThingSpeak(MessageServeur,writeAPIKeyElec);

}

// Update the data for the temperature
void UpdateTempInt(char *Data){
  
  char MessageServeur[100]="";
  char TempInt[20]="";

  memset(MessageServeur,'\0',100);
    
   if (DEBUG==1) { Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]"); }
   if (DEBUG==1) Serial.println();
  
    memset(TempInt,'\0',20);
    parseMessage(Data,TempInt,5);
    if (DEBUG==1) Serial.print("TempInt :");   
    if (DEBUG==1) Serial.println(TempInt);
    
   
    sprintf(MessageServeur,"field1=%s",TempInt);
    
    updateThingSpeak(MessageServeur,writeAPIKeyElec);

}

// Update the data for the meteo
void UpdateMeteo(char *Data){
  
  char MessageServeur[100]="";
   if (DEBUG==1) Serial.print("Meteo :");
   if (DEBUG==1) Serial.println(Data);
   char Light[20];
   char BPV[20];
   char TV[20];
   char HV[20];
   char WD[20];
   char WS[20];
   char RV[20];
   
   memset(Light,'\0',20);
   memset(BPV,'\0',20);
   memset(TV,'\0',20);
   memset(HV,'\0',20);
   memset(WD,'\0',20);
   memset(WS,'\0',20);
   memset(RV,'\0',20);

  memset(MessageServeur,'\0',100);
  
    parseMessage(Data,Light,5);
    if (DEBUG==1) Serial.print("Light :");   
    if (DEBUG==1) Serial.println(Light);
    
    parseMessage(Data,BPV,6);
    if (DEBUG==1) Serial.print("BPV :");   
    if (DEBUG==1) Serial.println(BPV);
    
    parseMessage(Data,TV,7);
    if (DEBUG==1) Serial.print("TV :");    
    if (DEBUG==1) Serial.println(TV);

    parseMessage(Data,HV,8);
    if (DEBUG==1) Serial.print("HV :");    
    if (DEBUG==1) Serial.println(HV);
 
     parseMessage(Data,WD,9);
    if (DEBUG==1) Serial.print("WD :");    
    if (DEBUG==1) Serial.println(WD);
 
     parseMessage(Data,WS,10);
    if (DEBUG==1) Serial.print("WS :");    
    if (DEBUG==1) Serial.println(WS);
 
    parseMessage(Data,RV,11);
    if (DEBUG==1) Serial.print("RV :");    
    if (DEBUG==1) Serial.println(RV);
    
    if (atoi(WD)>-1)
      sprintf(MessageServeur,"field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field7=%s&field8=%s",BPV,TV,HV,RV,Light,WD,WS);
    else   
      sprintf(MessageServeur,"field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field8=%s",BPV,TV,HV,RV,Light,WS);

    if (DEBUG==1) Serial.println(MessageServeur);
    
//   if (DEBUG==1) { Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]"); }
//   if (DEBUG==1) Serial.println();
//  
//    memset(TempInt,'\0',20);
//    parseMessage(Data,TempInt,2);
//    Serial.print("TempInt :");   
//    Serial.println(TempInt);
//    
//   
//    sprintf(MessageServeur,"field1=%s",TempInt);
//    
    updateThingSpeak(MessageServeur,writeAPIKeyMeteo);

}

// Update ThingSpeak with the date on the right chanel
void updateThingSpeak(char* tsData, char *chanel)
{
  

  if (DEBUG2 == 1 ) {
    Serial.println(F("+++++++++++++++++  updateThingSpeak +++++++++++++++++++++++"));
    Serial.print("Le message: ");
    Serial.print(tsData);
    Serial.print(" Sa taille: ");
    Serial.println(strlen(tsData));
    delay(100);
  }

  
  //if (ENABLE_IDLE_STBY_RF==1)  
  noInterrupts();
  radio.setMode(RF69_MODE_SLEEP);

  // Connecting to the server
  byte server[] = { 193, 54, 76, 34 }; // kgb.emn.fr
  delay(1000);
  //Serial.println(client.connect("kgb.emn.fr", 8001));
  int connectInt = client.connect("kgb.emn.fr", 8001);

  if (connectInt)
  {         
    if (DEBUG==1) Serial.print("Conex");
    // Sending the data with a POST method
    client.print(F("POST /update HTTP/1.1\n"));
    client.print(F("Host: api.thingspeak.com\n"));
//    client.print(F("Host: kgb.emn.fr\n"));
    client.print(F("Connection: close\n"));
    client.print(F("X-THINGSPEAKAPIKEY: "));
    client.print(chanel);
    client.print(F("\n"));
    client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    client.print(F("Content-Length: "));
    client.print(strlen(tsData));
    client.print("\n\n");
    client.print(tsData);
    client.print("\n\n");
    
   // lastConnectionTime = millis();

   client.flush();

    //  while (client.connected()) {
    //     if (client.available()) {
    //       char c = client.read();
    //       if (DEBUG==1) Serial.print(c);
    //     }
    //  }
    if (DEBUG==1) Serial.println("Fin Conex");  
  }
  else Serial.println("PB!!! Conex");

 
 client.stop();
 if (DEBUG2==1) Serial.println(F("Client Stopped"));   

  Blink(LED,500);

  radio.setMode(RF69_MODE_STANDBY);
   
  interrupts();

  digitalWrite(4, HIGH);


  if (DEBUG==1) Serial.println(F("+++++++++++++++++  End updateThingSpeak +++++++++++++++++++++++"));


}

/*void loop_1(){
   char MessageNode[100];
   int sendSize=0;
   char Temp[20];
   uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
 
    memset(Temp,'\0',20);
    memset(buf,'\0',RH_RF69_MAX_MESSAGE_LEN);
    uint8_t len = sizeof(buf);
    memset(MessageNode,'\0',100);

    delay(300);

   if (rf69.recv(buf, &len)) {
         Blink(LED,3);
         MessageNode[99]='\0';
         parseMessage((char*)buf,Temp,3);
         /////////// BEGIN DEBUG 
         if (DEBUG==1) {
            Serial.print("got request: ");
            for (int t=0;((t<RH_RF69_MAX_MESSAGE_LEN)&(t<len));t++) {
              Serial.print((char) buf[t]);
            }
            Serial.print("From");
            Serial.print(Temp);
            Serial.println();
          }
         /////////// END DEBUG 
         if (strcmp(Temp,"002")==0)    UpdateTeleIC((char*)buf);
         if (strcmp(Temp,"003")==0)    UpdateTempInt((char*)buf);
         if (strcmp(Temp,"004")==0)    UpdateMeteo((char*)buf);
   }
}*/

void loop()
{
  // Setup of the ID
  #ifdef SETUPEEPROM
    EEPROM.write(ADDRID, IDTOSETUP);
    EEPROM.write(ADDRNODEID, NODEIDTOSETUP);
    EEPROM.write(ADDRNETWORKID, NETWORKIDTOSETUP);
    ID = EEPROM.read(ADDRID);
    NODEID = EEPROM.read(ADDRNODEID);
    NETWORKID = EEPROM.read(ADDRNETWORKID);
  #else
    ID = EEPROM.read(ADDRID);
    NODEID = EEPROM.read(ADDRNODEID);
    NETWORKID = EEPROM.read(ADDRNETWORKID);
  #endif
  
   /*char MessageNode[100];
   int sendSize=0;
   char Temp[20];
   uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];*/

  char json[MAX_JSON];
  char conf[35] = "{\"type\":\"Base\",\"V\":\"1\",\"id\":15}";
  long int temp = 2000*30;
  char buf[20];

//   while (radio.receiveDone()) {
//  Serial.println(radio.receiveDone());
//  }
  if (radio.receiveDone())
  {
    if (DEBUG==1) {  
      Serial.println("Message Receive");
      Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    }

    
    
    memset(json,'\0',MAX_JSON);

    for (byte i = 0; ((i < radio.DATALEN) & (i < MAX_JSON)); i++)
      json[i] = (char)radio.DATA[i];
    
   // radio.recv( json, len );
   
   if (DEBUG==1) {
     Serial.print('[');Serial.print( json );Serial.print(']');Serial.println(radio.DATALEN);
   }
   
    
    
    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
     if (DEBUG==1) Serial.print(" - ACK sent.");

      // When a node requests an ACK, respond to the ACK
      // and also send a packet requesting an ACK (every 3rd one only)
      // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
     
    }
    Serial.println();
    
  /*delay(3000);
  // Serial.println("Loop");
  if (rf69.available())
  {
    // Should be a message for us now   
    memset(buf,'\0',RH_RF69_MAX_MESSAGE_LEN);
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len))
    {
      RH_RF69::printBuffer("request: ", buf, len);
  if (DEBUG==1) {
      Serial.print("got request: ");
      for (int t=0;((t<RH_RF69_MAX_MESSAGE_LEN)&(t<len));t++) {
          Serial.print((char) buf[t]);
      }
      Serial.println();
  }
      //Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf69.lastRssi(), DEC);

    memset(MessageNode,'\0',100);
    memset(Temp,'\0',20);
    if (REPLY==1) {
      parseMessage((char*)buf,Temp,2);
      sprintf(MessageNode,"%s;%s;%s;ACK;",NETWORKID,Temp,GATEWAY);
      sendSize = strlen(MessageNode);
      if (DEBUG==1) Serial.println(MessageNode);
      if (DEBUG==1) Serial.println(sendSize);
    
      // Send a reply
        if (REPLY==1) rf69.send((uint8_t*)MessageNode, sendSize);
       // if (REPLY==1) rf69.waitPacketSent();
        if ((DEBUG==1)&&(REPLY==1)) Serial.println("Sent a Ack");
    }
    Blink(LED,3);
      */
    memset(buf,'\0',20);
    parseMessage(json,buf,2);
    if (DEBUG==1) Serial.print("PROBE :");    
    if (DEBUG==1) Serial.println(buf);
    
    if (strcmp(buf,"001")==0)    UpdateTeleIC(json);
    if (strcmp(buf,"2")==0)    UpdateTempInt(json);
    if (strcmp(buf,"003")==0)    UpdateMeteo(json);
    
    }
    else
    {
      //Serial.println("recv failed");
    }
  }
  




