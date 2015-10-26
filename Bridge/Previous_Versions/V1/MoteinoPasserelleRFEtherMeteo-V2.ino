// Copyright Benoit Blanchon 2014
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson

#include <RFM69.h>
#include <SPI.h>
#include <Ethernet.h>
#include <utility/W5100.h>

#define DEBUG 1
#define DEBUG2 1

#define NODEID        1    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      300 // max # of ms to wait for an ack
#define SERIAL_BAUD   9600

#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
  #define FLASH_SS      23 // and FLASH SS on D23
#else
  #define LED           9 // Moteinos have LEDs on D9
  #define FLASH_SS      8 // and FLASH SS on D8
#endif

byte ackCount=0;

 #define MAX_JSON 100
 
RFM69 radio;
//SPIFlash flash(FLASH_SS, 0xEF30); //EF30 for 4mbit  Windbond chip (W25X40CL)
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

int cpt=0;

int lightValue = 0;
float windSpeedValue = 0; // mph
int windDirectionValue = 0; // degrees
long int barometricPressureValue = 0; // pascals
float dhtTemperatureValue = 0; // degrees C
float dhtHumidityValue = 0; // percent
float rainValue = 0; // inches


/////////////////////////////////////////////// BEGIN NETWORK ////////////////////////////////////////
// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

//// EMN Settings
char thingSpeakAddress[] = "kgb.emn.fr";
int server_port = 8001;
char writeAPIKeyElec[] = "L57JXD7X3XF5CV87";
char writeAPIKeyMeteo[] = "EBDWFJVVAGFX8I9M";


// TS Settings
//char thingSpeakAddress[] = "api.thingspeak.com";
//int server_port = 80;
//String writeAPIKey = "YGB6F7SFQAV0LSDR";

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
/////////////////////////////////////////////// END NETWORK ////////////////////////////////////////



int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}



void startEthernet()
{
  
  client.stop();

  if (DEBUG==1) Serial.println(F("Connecting Arduino to network..."));
  if (DEBUG==1)  Serial.println();  

  delay(4000);
  
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

  delay(1000);
}

//const int BUFFER_SIZE = JSON_OBJECT_SIZE(5) + JSON_ARRAY_SIZE(7);


void setup() {
  Serial.println("INIT");
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);

  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);
  char buff[50];
  if (DEBUG==1) sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  if (DEBUG==1) Serial.println(buff);

  startEthernet();
}


void updateThingSpeak2(char* tsData, char *chanel)
{
  

  if (DEBUG2 == 1 ) {
    Serial.println(F("+++++++++++++++++  updateThingSpeak +++++++++++++++++++++++"));
    Serial.print("Le message: ");
    Serial.print(tsData);
    Serial.print(" Sa taille: ");
    Serial.println(strlen(tsData));

//    Serial.print("Adress:");Serial.print(thingSpeakAddress);Serial.print("port : ");Serial.println(server_port);
    delay(100);
  }

  
  noInterrupts();
  radio.setMode(RF69_MODE_SLEEP);
  
//    for (int i=0;i<10000;i++)
//      for (int j=0;j<10000;j++);
    
  if (client.connect(thingSpeakAddress, server_port))
  {         
    if (DEBUG==1) Serial.print("Conex");
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

    if (DEBUG2==1) Serial.println();
    if (DEBUG2==1) Serial.print(F("RAM libre :"));
    if (DEBUG2==1) Serial.println(freeRam());

}




void UpdateTeleIC3(char *Data){
  
  char MessageServeur[100]="";
  char Inst[20]="";
  char hchc[20]="";
  char hchp[20]="";

  memset(MessageServeur,'\0',100);
    
   if (DEBUG==1) { Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]"); }
   if (DEBUG==1) Serial.println();
  
    memset(Inst,'\0',20);
    parseMessage(Data,Inst,2);
    if (DEBUG==1) Serial.print("IINST :");   
    if (DEBUG==1) Serial.println(Inst);
    
    memset(hchp,'\0',20);
    parseMessage(Data,hchp,3);
    if (DEBUG==1) Serial.print("HCHP :");   
    if (DEBUG==1) Serial.println(hchp);
    
    memset(hchc,'\0',20);
    parseMessage(Data,hchc,4);
    if (DEBUG==1) Serial.print("HCHC :");    
    if (DEBUG==1) Serial.println(hchc);
   
    sprintf(MessageServeur,"field4=%s&field5=%s&field6=%s",hchp,hchc,Inst);
    
    updateThingSpeak2(MessageServeur,writeAPIKeyElec);

}

void UpdateTempInt(char *Data){
  
  char MessageServeur[100]="";
  char TempInt[20]="";

  memset(MessageServeur,'\0',100);
    
   if (DEBUG==1) { Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]"); }
   if (DEBUG==1) Serial.println();
  
    memset(TempInt,'\0',20);
    parseMessage(Data,TempInt,2);
    if (DEBUG==1) Serial.print("TempInt :");   
    if (DEBUG==1) Serial.println(TempInt);
    
   
    sprintf(MessageServeur,"field1=%s",TempInt);
    
    updateThingSpeak2(MessageServeur,writeAPIKeyElec);

}

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
  
    parseMessage(Data,Light,2);
    if (DEBUG==1) Serial.print("Light :");   
    if (DEBUG==1) Serial.println(Light);
    
    parseMessage(Data,BPV,3);
    if (DEBUG==1) Serial.print("BPV :");   
    if (DEBUG==1) Serial.println(BPV);
    
    parseMessage(Data,TV,4);
    if (DEBUG==1) Serial.print("TV :");    
    if (DEBUG==1) Serial.println(TV);

    parseMessage(Data,HV,5);
    if (DEBUG==1) Serial.print("HV :");    
    if (DEBUG==1) Serial.println(HV);
 
     parseMessage(Data,WD,6);
    if (DEBUG==1) Serial.print("WD :");    
    if (DEBUG==1) Serial.println(WD);
 
     parseMessage(Data,WS,7);
    if (DEBUG==1) Serial.print("WS :");    
    if (DEBUG==1) Serial.println(WS);
 
    parseMessage(Data,RV,8);
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
    updateThingSpeak2(MessageServeur,writeAPIKeyMeteo);

}

void(* resetFunc) (void) = 0;

long int reset=0;

void parseMessage(char *Message, char * Response, int value) {
  int cpt = 0;
 // Serial.println(Message);

  for (int i=0; (i<MAX_JSON) & (i<(strlen(Message) ) ); ) {
    if (cpt==value) {
      for (int j=0; (Message[i]!=';' & (i<MAX_JSON) & (i<(strlen(Message) ) ) ); ) {
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

void loop() {
   // Serial.println(BUFFER_SIZE);

  char json[MAX_JSON];
  char conf[35] = "{\"type\":\"Base\",\"V\":\"1\",\"id\":15}";
  long int temp = 2000*30;
  char buf[20];

  if (cpt++>1000) {cpt=0; reset++; if (DEBUG==1) Serial.print("-");}
  // if (reset > 120000)   {resetFunc();reset=0;}  //call reset
//   if (reset > 10000)   {
//         updateThingSpeak2("field6=-1",writeAPIKeyMeteo);
//         resetFunc();reset=0;
//         }  //call reset

   
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
    memset(buf,'\0',20);
    parseMessage(json,buf,0);
    if (DEBUG==1) Serial.print("PROBE :");    
    if (DEBUG==1) Serial.println(buf);
//    memset(buf,'\0',20);
//    parseMessage(json,buf,1);
//    Serial.print("FIRMWARE :");    Serial.println(buf);
//    memset(buf,'\0',20);
//    parseMessage(json,buf,2);
//    Serial.print("IINST :");    Serial.println(buf);
//    memset(buf,'\0',20);
//    parseMessage(json,buf,3);
//    Serial.print("HCHP :");    Serial.println(buf);
//    memset(buf,'\0',20);
//    parseMessage(json,buf,4);
//    Serial.print("HCHC :");    Serial.println(buf);

    if (strcmp(buf,"001")==0)    UpdateTeleIC3(json);
    if (strcmp(buf,"002")==0)    UpdateTempInt(json);
    if (strcmp(buf,"003")==0)    UpdateMeteo(json);

    
    if (DEBUG==1) Serial.println();
    Blink(LED,3);
    
   if (DEBUG==1) Serial.println(F("+++++++++++++++++  End Waitting for Message +++++++++++++++++++++++"));

//    const char* type = root["type"];
//      if (strcmp(type,"ElecTI")==0)    UpdateTeleIC2(root);
//      if (strcmp(type,"Meteo1")==0)    UpdateMeteo1(root);
//      if (strcmp(type,"Meteo2")==0)    UpdateMeteo2(root);
 
  }
}



