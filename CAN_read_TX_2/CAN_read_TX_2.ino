#include <SPI.h>
#include <mcp2515.h>

union float_bytes { //conversion des float reçus.
     float val;
     unsigned char bytes[sizeof(float)];
} dataX,dataY,dataRay;

MCP2515 mcp2515(10);  //définition de la pin SS du mcp2515
struct can_frame canMsgRx;  //strcture de la trame CAN reçue
struct can_frame canMsgTx; //trame CAN à envoyer
byte payload[8];  //donnée reçues
int affiche=0;  //activation de l'affichage
int chill_cnt=0;

void affichage (void);
void getCan (void);
void canTransmit (void);

void setup() {  //initialisation
  
  while (!Serial);
  Serial.begin(57600);
  SPI.begin();

  //On génère la structure de la trame CAN
  canMsgTx.can_id  = 0x5A;  //L'adresse (de destination) est mise à 0x62 car l'arduino qui va recevoir la trame est bloquée à cette adresse (elle n'a pas de switchs)
  canMsgTx.can_dlc = 8; //payload toujours 8
  canMsgTx.data[0] = 0x43;  
  canMsgTx.data[1] = 0x42;
  canMsgTx.data[2] = 0x43;
  canMsgTx.data[3] = 0x44;
  canMsgTx.data[4] = 0x45;
  canMsgTx.data[5] = 0x46;
  canMsgTx.data[6] = 0x47;
  canMsgTx.data[7] = 0x48;

  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_16MHZ);  //paramètres les plus rapides testés pour le CAN
  mcp2515.setNormalMode();

  delay(200);

  Serial.println("I'm up, bitch !");
}

void loop() {
  getCan(); //on lit le CAN
  affichage();  //gestion de l'affichage

  if(chill_cnt >= 10000){
    canTransmit();
    chill_cnt = 0;
  }
  else {
    chill_cnt ++;
  }
}

void canTransmit (void){  //Transmission CAN (envoi seulement)
  if (mcp2515.sendMessage(&canMsgTx) == MCP2515::ERROR_OK) { //On envoie le message au mcp2515
    delay(10);
    Serial.println("can data well sent");
  }
  else {
    Serial.println("can data emission failed");
  }
}

void affichage (void){
  if(affiche==1){
  Serial.print(payload[0], HEX);
  Serial.print("  |  ");
  Serial.print(payload[1], HEX);
  Serial.print("  |  ");
  Serial.print(payload[2], HEX);
  Serial.print("  |  ");
  Serial.print(payload[3], HEX);
  Serial.print("  |  ");
  Serial.print(payload[4], HEX);
  Serial.print("  |  ");
  Serial.print(payload[5], HEX);
  Serial.print("  |  ");
  Serial.print(payload[6], HEX);
  Serial.print("  |  ");
  Serial.print(payload[7], HEX);
  Serial.print("   |||   ");
  Serial.println(canMsgRx.can_id);
  affiche = 0;  //une fois la donnée affichée, on empêcha l'affichage
  }
}

void getCan (void){
  if ((mcp2515.readMessage(&canMsgRx) == MCP2515::ERROR_OK)) { //Si le CAN est disponible
    for (int i = 0; i<canMsgRx.can_dlc+1; i++)
    {
        payload[i]=canMsgRx.data[i];
    }
    affiche = 1;  //on autorise l'affichage
    Serial.println("ERROR_OK");
  }
  else if (mcp2515.readMessage(&canMsgRx) == MCP2515::ERROR_FAIL) {
    Serial.println("received stuff but failed");
  }
}

