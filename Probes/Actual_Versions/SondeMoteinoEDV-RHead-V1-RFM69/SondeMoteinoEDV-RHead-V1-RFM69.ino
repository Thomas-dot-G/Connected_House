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

#include <EEPROM.h>
//#include <avr/wdt.h>
#include <WirelessHEX69.h> //get it here: https://github.com/LowPowerLab/WirelessProgramming/tree/master/WirelessHEX69

#include <SoftwareSerial.h>

#define DEBUG 1
#define DISTRIBUTED_DEBUG     0

// ADDRID is the address of the ID in the EEPROM, SETPEEROM is a boolean to setup the ID in the EEPROM, IDTOSETUP is the ID to put in the EEPROM and ID is the actual ID read from the EEPROM. Same schema for others
//#define SETUPEEPROM 0
#define ADDRID 0
#define IDTOSETUP 001
#define ADDRNODEID 10
#define NODEIDTOSETUP 005
#define ADDRNETWORKID 20
#define NETWORKIDTOSETUP 100
#define ADDRGATEWAYID 30
#define GATEWAYIDTOSETUP 001

long ID = 1;
long NODEID = 5;
long NETWORKID = 100;
long GATEWAYID = 001;

#define VERSION 001
#define SERIAL_BAUD   9600
long TRANSMITPERIOD = 15000; //transmit a packet to gateway so often (in ms)
long lastPeriod = 0;
#define LED           9 // Moteinos have LEDs on D9

#ifdef __AVR_ATmega1284P__
  #define FLASH_SS      23 // and FLASH SS on D23
#else
  #define FLASH_SS      8 // and FLASH SS on D8
#endif

// Singleton instance of the radio driver
RFM69 radio;
SPIFlash flash(FLASH_SS, 0xEF30); //EF30 for windbond 4mbit flash


SoftwareSerial cptSerial(4, 5);
#define FREQUENCY   RF69_433MHZ
/***************** Teleinfo configuration part *******************/
//#define TAILLETRAME  200
#define TAILLELIGNE  40
#define TAILLETIQUETTE  15
#define TAILLDONNEE 15
#define TAILLECHECK 32
char CaractereRecu ='\0';
char Checksum[TAILLECHECK] = "";
char Ligne[TAILLELIGNE]="";
char Etiquette[TAILLETIQUETTE] = "";
char Donnee[TAILLDONNEE] = "";

//char Trame[TAILLETRAME] ="";
int i = 0;
int j = 0;
int cpt=0;


long HCHC = 0;      // Index option Heures Creuses - Heures Creuses, 8 numériques, Wh
long HCHP = 0;      // Index option Heures Creuses - Heures Pleines, 8 numériques, Wh
int IINST = 0;     // Monophasé - Intensité Instantanée, 3 numériques, A  (intensité efficace instantanée)

long previous_HCHC = 0;      // Index option Heures Creuses - Heures Creuses, 8 numériques, Wh
long previous_HCHP = 0;      // Index option Heures Creuses - Heures Pleines, 8 numériques, Wh
int previous_IINST = 0;     // Monophasé - Intensité Instantanée, 3 numériques, A  (intensité efficace instantanée)

/******************* END OF CONFIGURATION *******************/


/***********************************************
   getTeleinfo
   Decode Teleinfo from serial
   Input : n/a
   Output : n/a
***********************************************/
unsigned char sum = 32 ;		// Somme des codes ASCII du message + un espace

int checksum_ok(char *etiquette, char *valeur, char checksum) 
{
       sum = 32 ;
	for (i=0; i < strlen(etiquette); i++) sum = sum + etiquette[i] ;
	for (i=0; i < strlen(valeur); i++) sum = sum + valeur[i] ;
	sum = (sum & 63) + 32 ;
        if (DEBUG) {
          Serial.print(etiquette);Serial.print(" ");
          Serial.print(valeur);Serial.print(" ");
	  Serial.println(checksum);
	  Serial.print("Sum = "); Serial.println(sum);
	  Serial.print("Cheksum = "); Serial.println(int(checksum));
        }
	if ( sum == checksum) return 1 ;	// Return 1 si checkum ok.
	return 0 ;
}

int lireValeur(char *ligne, int offset){
    j=0;
    memset(Donnee,'\0',TAILLDONNEE);
    for (i=offset; i < strlen(ligne); i++){ 
      if (ligne[i] != 0x20) { // Tant qu'on est pas au SP, c'est qu'on est sur l'étiquette
      
      if (j<TAILLDONNEE)
          Donnee[j++]=ligne[i];
      else {
        break;
      }
      
      }
      else { //On vient de finir de lire une etiquette
          if (DEBUG) {
	    Serial.print("Valeur : ");
            Serial.println(Donnee);
          }
          return j+2; // on est sur le dernier caractère de la valeur, il faut passer l'espace aussi (donc +2) pour arriver à la valeur
      }

   }
}


void lireChecksum(char *ligne, int offset){
   j=0;
    memset(Checksum,'\0',TAILLECHECK);
    for (i=offset; i < strlen(ligne); i++){ 
      if (j<TAILLECHECK)
          Checksum[j++]=ligne[i];
      else {
        if (DISTRIBUTED_DEBUG) {
         //datastreams2[0].setFloat(5);
         //xivelyclient.put(feed2, xivelyKey2);
        }
        break;
      }
      if (DEBUG) {
	    Serial.print("Chekcsum : ");
            Serial.println(Checksum);
          }
      }

}




int affecteEtiquette(char *etiquette, char *valeur){

 if(strcmp(etiquette,"ADCO") == 0) { 
//   memset(ADCO,'\0',12); 
//   memcpy(ADCO, valeur,strlen(valeur)); 
//   check[1]=1; 
//   Serial.print("ADCO="); Serial.println(ADCO);
//   Serial.print("valeur="); Serial.println(valeur);
 }
 else
 if(strcmp(etiquette,"HCHC") == 0) { 
   previous_HCHC = HCHC;
   HCHC = atol(valeur); 
   // une valeur ne peut pas ^tre infrieur  la precedente.
   if (HCHC<previous_HCHC) HCHC=previous_HCHC;
   // une valeur ne peut pas etre radicalement differente de la precedente
   // on la lisse.
   if ((HCHC>(previous_HCHC*1.1))&(previous_HCHP>0)) {
       HCHC = previous_HCHC; //(atol(valeur) + 10*previous_HCHC)/11;
   }
   
//   check[2]=1;
   if (DEBUG) {Serial.print(F("-->HCHC=")); Serial.println(HCHC);}
 //  strcpy(S_HCHC,valeur);
 //  Serial.print("valeur="); Serial.println(valeur);
 }
 else
 if(strcmp(etiquette,"HCHP") == 0) { 
   previous_HCHP = HCHP;
   HCHP = atol(valeur); 
   if (HCHP<previous_HCHP) HCHP=previous_HCHP;
   // une valeur ne peut pas etre radicalement differente de la precedente
   // on la lisse.
   if ((HCHP>(previous_HCHP*1.1))&(previous_HCHP>0)) {
       HCHP = previous_HCHP; //(atol(valeur) + 10*previous_HCHP)/11;
   }
   
//   check[3]=1;
   if (DEBUG) { Serial.print(F("-->HCHP=")); Serial.println(HCHP);}
 //  Serial.print("valeur="); Serial.println(valeur);
 }
 else
 if(strcmp(etiquette,"HHPHC") == 0) { 
//   memset(HHPHC,'\0',2); 
//   strcpy(HHPHC, valeur); 
//   check[4]=1;
 //  Serial.print("HHPHC="); Serial.println(HHPHC);
 //  Serial.print("valeur="); Serial.println(valeur);
 }
 else
 if(strcmp(etiquette,"PTEC") == 0) { 
//   memset(PTEC,'\0',4); 
//   memcpy(PTEC, valeur,strlen(valeur)); 
//   check[5]=1;
//   Serial.print("PTEC="); Serial.println(PTEC);
//   Serial.print("valeur="); Serial.println(valeur);
 }
 else
 if(strcmp(Etiquette,"IINST") == 0) { 
   previous_IINST = IINST;
   if ((atoi(valeur)<50) && (atoi(valeur)>0)) {
     IINST = atoi(valeur); 
   }
   else {
     IINST = previous_IINST;
   }
   
//   check[6]=1;
   if (DEBUG) {Serial.print(F("-->IINST=")); Serial.println(IINST);}
 //  Serial.print("valeur="); Serial.println(valeur);
 }
 else
 if(strcmp(Etiquette,"PAPP") == 0) { 
//   PAPP = atol(valeur); 
//   check[7]=1;
 //  Serial.print("PAPP="); Serial.println(PAPP);
 //  Serial.print("valeur="); Serial.println(valeur); 
 }
 else
 if(strcmp(Etiquette,"IMAX") == 0) { 
//   IMAX = atol(valeur); 
//   check[8]=1;
 //  Serial.print("IMAX="); Serial.println(IMAX);
 //  Serial.print("valeur="); Serial.println(valeur); 
 }
 else
 if(strcmp(Etiquette,"OPTARIF") == 0) { 
//   memset(OPTARIF,'\0',4); 
//   memcpy(OPTARIF, valeur,strlen(valeur)); 
//   check[9]=1;
 //  Serial.print("OPTARIF="); Serial.println(OPTARIF);
 //  Serial.print("valeur="); Serial.println(valeur); 
 }
 else
 if(strcmp(Etiquette,"ISOUSC") == 0) { 
 //  ISOUSC = atoi(valeur); 
//   check[10]=1;
 //  Serial.print("ISOUSC="); Serial.println(ISOUSC);
 //  Serial.print("valeur="); Serial.println(valeur);  
 }
 else
 if(strcmp(Etiquette,"MOTDETAT") == 0) { 
//    memset(MOTDETAT,'\0',10); 
//    memcpy(MOTDETAT, valeur, strlen(valeur)); 
  // check[0]=1;
  // Serial.print("MOTDETAT="); Serial.println(MOTDETAT);
  // Serial.print("valeur="); Serial.println(valeur);  
 }
 else
 return 0;

}


int lireEtiquette(char *ligne){
    j=0;
  
    memset(Etiquette,'\0',TAILLETIQUETTE);
    for (i=1; i < strlen(ligne); i++){ 
      if (ligne[i] != 0x20) { // Tant qu'on est pas au SP, c'est qu'on est sur l'étiquette
      if (j<TAILLETIQUETTE)
          Etiquette[j++]=ligne[i];
      else {
        break;
      }
      
      }
      else { //On vient de finir de lire une etiquette
          if (DEBUG) {
	    Serial.print("Etiquette : ");
            Serial.println(Etiquette);
          }
          return j+2; // on est sur le dernier caractère de l'etiquette, il faut passer l'espace aussi (donc +2) pour arriver à la valeur
      }

   }
}

  
  int debutValeur; 
  int debutChecksum;
  
int decodeLigne(char *ligne){ 
    
  
 if (DEBUG) Serial.println(F("--> Decodage ligne"));
  //Checksum='\0';

  // Décomposer en fonction pour lire l'étiquette etc ...  
  debutValeur=lireEtiquette(ligne);
  debutChecksum=lireValeur(ligne, debutValeur);
  lireChecksum(ligne,debutValeur + debutChecksum -1);

//affecteEtiquette(Etiquette,Donnee);

  if (checksum_ok(Etiquette, Donnee, Checksum[0])){ // si la ligne est correcte (checksum ok) on affecte la valeur à l'étiquette
    return affecteEtiquette(Etiquette,Donnee);
  } 
  else return 0;

}


void getTeleinfo2() {
  
  /* vider les infos de la dernière trame lue */
  memset(Ligne,'\0',TAILLELIGNE); 

 // IINST = 0;
  cpt=0;

    while(CaractereRecu != 0x02) // boucle jusqu'a "Start Text 002" début de la trame
    {
       if (cptSerial.available()) {
         CaractereRecu = cptSerial.read() & 0x7F;
       }

    }


    j=0;
    cpt=0;
    
    while(CaractereRecu != 0x03) // || !trame_ok ) // Tant qu'on est pas arrivé à "EndText 003" Fin de trame ou que la trame est incomplète
    { 
      if (cptSerial.available()) {
          CaractereRecu = cptSerial.read() & 0x7F;
          if (CaractereRecu != 0x0D) { // Tant qu'on est pas au CR, c'est qu'on est sur une ligne du groupe
            if (j<TAILLELIGNE)
              Ligne[j++]=CaractereRecu;
             else {
               break;
             }
          }
          else { //On vient de finir de lire une ligne, on la décode (récupération de l'etiquette + valeur + controle checksum
            decodeLigne(Ligne);
            memset(Ligne,'\0',32); // on vide la ligne pour la lecture suivante
          j=0;
          }
      }	
            
    }
    
}

////////////////////////////////////////////



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
  Serial.begin(9600);
  cptSerial.begin(1200);

  
  // No encryption
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

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  //radio.setEncryptionKey(key);
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

void sendTeleIC() {
   char temp1[20];
  uint8_t buf[20];
  uint8_t len = sizeof(buf);
  
  
  float temp;
  char MessageServeur[100];
  int   sendSize = 0;
  char buf2[20];

  // For now we send random data
  // getTeleinfo2();

  int currPeriod = millis()/TRANSMITPERIOD;
  if (currPeriod != lastPeriod)
  {
    lastPeriod=currPeriod;

  IINST = random(100,200);
  HCHP = random(100,200);
  HCHC = random(100,200);
  
  memset(MessageServeur,'\0',100);
  sprintf(MessageServeur,"%i;%i;%i;%i;%i;%i;%i;%i;",NETWORKID,GATEWAYID,NODEID,ID,VERSION,IINST,HCHP,HCHC);
  sendSize = strlen(MessageServeur);
    
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
 
   if (DEBUG) Serial.print(" Send --- ");

  radio.sendWithRetry(GATEWAYID, data, sendSize);
  
     //if (DEBUG) Serial.print(" Wait --- ");

  //rf69.waitPacketSent();

    // if (DEBUG) Serial.print(" End Wait --- ");
  
  // Now wait for a reply


//  if (rf69.waitAvailableTimeout(500))
//  { 
//    // Should be a reply message for us now   
//    if (rf69.recv(buf, &len))
//    {
//     if (DEBUG==1) {
//        Serial.print("got reply: ");
//        Serial.println((char*)buf);
//      }
//      memset(temp1,'\0',20);
//      parseMessage((char*)buf,temp1,0);
//      Serial.println(temp1);
//      if (strncmp((char*)temp1,NETWORKID,3)==0) {
//        memset(temp1,'\0',20);
//        parseMessage((char*)buf,temp1,1);  
//        Serial.println(temp1);
//        if (strncmp((char*)temp1,NODEID,3)==0) {
//            memset(temp1,'\0',20);
//            parseMessage((char*)buf,temp1,3); 
//            Serial.println(temp1);
//            if (strncmp((char*)temp1,"ACK",3)==0) {
//              Serial.println("--> ACK");
//            }
//        }
//      }
//        
//      
//    }
//    else
//    {
//      Serial.println("NO ACK");
//    }
//  }
//  else
//  {
//    Serial.println("No reply, is rf69_server running?");
 // }
 // delay(400);
}
}

// [SousReseau,To,From,Type,Version,...]
// [NETWORKID;GATEWAY;NODEID;TYPE;VERSION...]

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
   sendTeleIC();
  ////////////////////////////////////////////////////////////////////////////////////////////
  }
   
}

