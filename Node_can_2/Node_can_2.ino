//Ce code permet de faire le lien entre un module XBee et un bus CAN.

#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsgRx; //trame CAN à recevoir
struct can_frame canMsgTx; //trame CAN à envoyer
MCP2515 mcp2515(10);  //SS de la SPI pour le MCP2515
byte frame_ID[2] = {'I', 'D'};  //initialisation de l'ID de la trame à envoyer
byte api_frame[100];  //initialisation de la frame à recevoir XBee. 100 parcequ'on ne connait pas la taille. (à modifier)
byte received_ID[2];  //initialisation de l'ID reçu (du XBee)
byte received_payload[8]; //initialisation des données recues (du XBee)
int error_watch = 0;  //variable qui compte les erreurs XBee
bool isBusy = true; //variable qui définit si le XBee est occupé ou non
char rx_byte = 0; //obsolète

int scan_address_switch(void); //scan du switch d'adresse
int scan_channel_switch(void); //scan du switch de channel (radio)
void openATmode(void);  //ouvrir le mode de configuration du XBee
void setChannel(int); //définir le channel utilisé par le XBee
void closeATmode(void); //fermer le mode de configuration du XBee
void decode_frame(void);  //décoder la trame reçue du XBee
void canTransmit (void);  //Envoie une trame CAN
void XBeeStatusCheck (void);  //vérifie que le module XBee est toujours up
void XBee_rst(void);  //Soft reset le module XBee
void receiveXBee (void); //recoit une trame du module XBee

void setup() {
  Serial.begin(57600);
  delay(50);
  pinMode(2, INPUT_PULLUP); //pullups integrées associées aux switchs
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  int my_address = scan_address_switch(); //on récupère l'adresse en fonctions des switchs
  int my_channel = scan_channel_switch(); //on récupère le channel en fonction des switchs
  setChannel(my_channel+48);  //set channel prend un char en paramètre. +48 (correspondance tabme table ASCII)

  //On génère la structure de la trame CAN
  canMsgTx.can_id  = 0x62;  //L'adresse (de destination) est mise à 0x62 car l'arduino qui va recevoir la trame est bloquée à cette adresse (elle n'a pas de switchs)
  canMsgTx.can_dlc = 8; //payload toujours 8
  canMsgTx.data[0] = 0x00;  
  canMsgTx.data[1] = 0x00;
  canMsgTx.data[2] = 0x00;
  canMsgTx.data[3] = 0x00;
  canMsgTx.data[4] = 0x00;
  canMsgTx.data[5] = 0x00;
  canMsgTx.data[6] = 0x00;
  canMsgTx.data[7] = 0x00;
  
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_16MHZ);  //paramètres les plus rapides testés pour le CAN
  mcp2515.setNormalMode();
  delay(200);
}

void loop() {
  receiveXBee();  
  XBeeStatusCheck();
  canTransmit();
}

void canTransmit (void){  //Transmission CAN (envoi seulement)
  if(received_payload[0]!=0){ //Si on a des données à envoyer
    for(int i=0; i<8; i++){
      canMsgTx.data[i] = received_payload[i]; //On associe la data à la structure CAN
    }
    mcp2515.sendMessage(&canMsgTx); //On envoie le message au mcp2515
    delay(10);
    for(int i=0; i<8; i++){
      received_payload[i]=0;  //On remet la payload à 0 pour ne pas la renvoyer
    }
  }
}

void XBeeStatusCheck (void){
  if(error_watch>10){ //si on a accumulé plus de 10 erreurs, reset.
    //Un crash peut être dû au buffer XBee qui sature (côté ZigBee, pas Serial)
    //Le module peut ne pas répondre si il n'est pas correctement sorti du mode de commande.
    
    XBee_rst();
    delay(1000);
    error_watch = 0;  //réinitialisation des erreurs
    while(Serial.available()){Serial.read();} //On vide les buffers Serial qui ont pu se remplir pendant le reset
    isBusy = false;//on libère le flag busy, au cas où il ait été mis à 0 avant.
  }
}

void XBee_rst(void){
  openATmode();
  while(Serial.available()){Serial.read();} //Le module renvoie "OK" à l'ouvertue
  while(!Serial.available()){//Commande déclenchant le reset
    Serial.write('A');
    Serial.write('T');
    Serial.write('F');
    Serial.write('R');
    Serial.write('\r');
    delay(200);
  }
}

void receiveXBee (void){
  if(Serial.available()){ //Si le XBee a quelque chose à nous dire
    if(Serial.read()==0x7E){  //Delimiter de début de trame API
      api_frame[0]=0x7E;
      delay(1);
      int i = 1;
      int x = 26;
      while(i<x){ //On vient lire les autres octets de la trame API. X est fixé ici, mais il sera remplacé par l'octet qui définit la longueur de la trame.
        api_frame[i]=Serial.read();
        i++;
        delay(1);
      }
    }
    if(api_frame[3]==0x8B){ //frame type 0x8B : transmit status - équivalent ACK. Permet de ne pas spammer le port série du XBee.
      if(api_frame[8]==0x00){ //transmit status 0x00 : success
        for(int j=0; j<=api_frame[2]+4; j++){ //api_frame[2] contient la longueur de la trame, en exlucant le checksum et les 3 premiers octets
          api_frame[j]=0; //on efface la trame API une fois qu'on a fini
        }
        for(int j=0; j<=8; j++){
          received_payload[j]=0;  //On efface les données reçues (si on est pas en mode DEMO, auquel cas on garde toujours la même payload).
        }
      }
      else{ //Si le transmit status n'est pas un succès
        error_watch++;
        for(int j=0; j<=api_frame[2]+4; j++){
          api_frame[j]=0; //On nettoie la trame API.
        }
      }
    }
    else if (api_frame[3]==0x90){ //0x90 signale une réception de données par le XBee
      decode_frame(); //on décoe la trame reçue
    }
    else{
      for(int j=0; j<=30; j++){
          api_frame[j]=0; //On nettoie la trame API (garbage)
      }
    }
    isBusy = false; //On est sûr que le module est libre maintenant
  }
}

void decode_frame(void){
  for(int i=0; i<2; i++){
    received_ID[i] = api_frame[i+15];
  }
  for(int i=0; i<8; i++){
    received_payload[i] = api_frame[i+17];
  }
  for(int j=0; j<=api_frame[2]+4; j++){
    api_frame[j]=0;
  }
}


int scan_address_switch(){  //Lecture des switchs d'adresse
  int value = 0;
  value = value + !digitalRead(16);
  value = value + !digitalRead(15) * 2;
  value = value + !digitalRead(14) * 4;
  value = value + !digitalRead(9) * 8;
  value = value + !digitalRead(8) * 16;
  value = value + !digitalRead(7) * 32;
  value = value + !digitalRead(6) * 64;
  value = value + !digitalRead(5) * 128;
  value = value + !digitalRead(4) * 256;
  value = value + !digitalRead(3) * 512;
  value = value + !digitalRead(2) * 1024;
  return value;
}

int scan_channel_switch(void){  //Lecture des switchs de channel radio
  int value = 0;
  value = value + !digitalRead(19);
  value = value + !digitalRead(18) * 2;
  value = value + !digitalRead(17) * 4;
  return value;
}

void openATmode(void){  //Ouverture du mode AT
  while(Serial.available()){Serial.read();} //On vide le buffer Serial
  while(!Serial.available()){ //Tant que le module ne répond pas OK, on continue à demander le mode AT
    Serial.write('+');  //Commande pour déclencher le mode AT
    Serial.write('+');
    Serial.write('+');
    delay(1200);
  }
}

void closeATmode(void){ //Fermeture du mode AT
  while(Serial.available()){Serial.read();}
  while(!Serial.available()){
    Serial.write('A');
    Serial.write('T');
    Serial.write('C');
    Serial.write('N');
    Serial.write('\r');
    delay(200);
  }
}

void setChannel(int mode){  //Paramétrage du channel radio
  openATmode(); //Ouverture du mode commande
  Serial.write('A');  //Commande permettant de modifier le channel d'émission
  Serial.write('T');
  Serial.write('C');
  Serial.write('M');
    
  switch (mode){   //Sélection des différents channels (3 channels de 200kHz adjacents) en fonction du mode
    case '0':
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('7');
    break;
    
    case '1':
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('3');
    Serial.write('8');

    break;
    
    case '2':
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('5');
    Serial.write('8');
    Serial.write('0');
    break;
    
    case '3':
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('3');
    Serial.write('8');
    Serial.write('0');
    Serial.write('0');
    break;
    
    case '4':
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('1');
    Serial.write('C');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    break;
    
    case '5':
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('E');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    break;
    
    case '6':
    Serial.write('0');
    Serial.write('0');
    Serial.write('7');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    break;
    
    case '7':
    Serial.write('0');
    Serial.write('6');
    Serial.write('8');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    Serial.write('0');
    break;
    
    default:
    break;
  }
  Serial.write('\r');
  delay(500);
  Serial.write('A');  //Commande permettant de charger les changement en EEPROM
  Serial.write('T');
  Serial.write('W');
  Serial.write('R');
  Serial.write('\r');
  delay(500);
  Serial.write('A');  //Application des changements
  Serial.write('T');
  Serial.write('A');
  Serial.write('C');
  Serial.write('\r');
  delay(500);
  closeATmode();  //Quitter le mode de commande AT
}

