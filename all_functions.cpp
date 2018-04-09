CAN_read_TX :

void affichage (void);
void decodeCan (void);
void getCan (void);

Node_can :

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

Node_programmer :

void openATmode(void);  //Ouverture du mode de commande AT
void closeATmode(void); //Fermeture du mode de commande AT
void instructions(void); //permet d'écrire les instructions 
void directWrite(void); //Transmet directement un caractère du Serial vers le XBee (pour les commandes manuelles)
void Menu(void);  //Affiche le menu de sélection (et attend un input de l'utilisateur)
void readSerial(void);  //Lis le port Série
void readChannel(void); //Récupère la valeur du channel mode (radio)
void readATtime(void);  //Lis la valeur pendant laquelle le mode AT reste actif
void readAP(void);   //Lis le mode d'utilisation (transparent ou API)
void setATtime100(void);  //Règle le temps pendant lequel le mode AT reste actif
void setChannel(int); //Règle les channels radio utilisés pour la transmission
void setBaud(int); //Règle le BaudRate du port série du XBee
void setAP(int); //Règle le mode AP (Transparent ou API)
void setMultiTransmit(int); //Règle le nombre de'émissions en mode broadcast
void readMultiTransmit(void);

Node_serial :

void checkSerial (void);  //lit le port série
void XBeeStatusCheck (void);  //vérifie que le module XBee est toujours up
void XBee_rst(void);  //Soft reset le module XBee
void receiveXBee (void);  //recoit une trame du module XBee
void decode_frame(void);  //décode une trame du module XBee
void sendXBee (void); //envoie une trame au module XBee
int scan_address_switch(void);  //scan des switchs d'adresse
int scan_channel_switch(void);  //scan des switchs de channel
void setChannel(int mode);  //redéfinit le channel radio du module XBee
void openATmode(void);  //Ouvre le mode de commande du mosule XBee
void closeATmode(void); //Ferme le mode de commaned du module XBee