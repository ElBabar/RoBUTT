//Ce code est celui du programmateur, capable de programmer les modules XBee (certaines valeurs)

/*AT Commands :
 * ATCT = AT Mode timeout (100s = 3E8)
 * ATID = network ID (0x7FFF)
 * ATCM = Channel Mask
 * ATAP = AP mode (0=Transparent, 1=API)
 * ATBD = Baud Rate
 * 0     1     2     3     4     5     6     7      8
 * 1200  2400  4800  9600  19200 38400 57600 115200 230400
 * (penser à reboot le module)
 * 
 * Dans l'ordre :
 * +++  Open AT Mode (NO LINE FEED)
 * ATxx Command xx
 * ATAC Apply Changes
 * ATWR Changes permanent
 * ATCN Close AT Mode
 * 
 * Line Feed applies command.
 */

#include <SoftwareSerial.h>

SoftwareSerial XBeeSerial(11,12); //RX, TX
int data = '9'; //data récupérée sur le Serial. Je ne met pas 0 car on l'envoie souvent, et je peux metre ce caractère car on envoie que de l'Hexa pour les valeurs.
byte inst = 0; //instructions : 1 si les instructions sont présentes sur le Serial. Mis à 0 lorsqu'on affiche autre chose.


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

void setup() {
  Serial.begin(57600);
  XBeeSerial.begin(57600);
  delay(50);
  Serial.println("Ne pas utiliser de fin de ligne");
  instructions(); //Affiche les instructions
}

void loop() {
  Menu(); //Teste un input pour entrer dans le menu
  readSerial(); //On scanne un input de l'utilisateur
  directWrite();  //Mode d'écriture directe vers le XBee
  instructions(); //Affiche les instructions
}

void readSerial(void){
  if(Serial.available()){
    data = Serial.read(); //On enregistre l'input dans data
  }
}

void Menu(void){
  if(data != '9'){  //Si il y a eu un input
    switch (data){  //Le switch case permet de faire les sous menus et de sélectionner les fonctions adéquates
      case '1':
        inst = 0;
        Serial.println("Quelle bande radio choisir ? (0 - 7)");
        while(!Serial.available()){
        }
        setChannel(Serial.read());
      break;

      case '2':
        inst = 0;
        Serial.println();
        readChannel();
      break;
      
      case '3':
        inst = 0;
        Serial.println();
        setATtime100();
      break;
      
      case '4':
        inst = 0;
        Serial.println();
        readATtime();
      break;

      case '5':
        inst = 0;
        Serial.println("Baud Rate ? 3:9600 - 4:19200 - 5:38400 - 6:57600 - 7:115200");
        while(!Serial.available()){
        }
        setBaud(Serial.read());
      break;
      
      case '6':
        inst = 0;
        Serial.println("Mode AP ? 0:Transparent - 1:API ");
        while(!Serial.available()){
        }
        setAP(Serial.read());
      break;

      case '7':
        inst = 0;
        Serial.println();
        readAP();
      break;
      
      case '8':
        inst = 0;
        Serial.println("Nombre de Multi-Transmit en Broadcast ? entre 1 et 5 ");
        while(!Serial.available()){
        }
        setMultiTransmit(Serial.read());
      break;
      
      case '9':
        inst = 0;
        Serial.println();
        readMultiTransmit();
      break;
    }
  }
  data = '9'; //On remet data à son état par défaut
}

void directWrite(void){ //Mode d'écruture directe des craactères vers le module XBee
  if(data != '9'){  //SI il y a une data 
    if(data == '+'){  //Et si la data est "+" (cette fonction est appellée avant Menu une fois que le serial est lu)
      openATmode(); //Ouverture du mode AT
    }
    else if (data == '*'){  //Ce caractère marque une fin de ligne, nécessaire à l'nevoi des commandes vers le module
      XBeeSerial.write('\r'); 
      Serial.println();
    }
    else if (data == '-'){  //Ce caractère déclenche la fermeture du mode AT
      closeATmode();  
    }
    else{
      XBeeSerial.write(data); //On écris ce que l'on veut
    }
  }
  if(XBeeSerial.available()){
    while(XBeeSerial.available()){  //Si le module répond, on affiche sa réponse ici
      Serial.write(XBeeSerial.read());
    }
  }
}


void openATmode(void){  //Ouverture du mode AT
  Serial.print("Engaging AT mode ");
  while(!XBeeSerial.available()){
    XBeeSerial.write('+');
    XBeeSerial.write('+');
    XBeeSerial.write('+');
    delay(1200);
    Serial.print(". ");
  }
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
}

void closeATmode(void){ // Fermeture du mode AT
  Serial.print("Closing AT mode ");
  while(!XBeeSerial.available()){
    XBeeSerial.write('A');
    XBeeSerial.write('T');
    XBeeSerial.write('C');
    XBeeSerial.write('N');
    XBeeSerial.write('\r');
    delay(200);
    Serial.print(". ");
  }
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
}

void instructions(void){
  if(inst == 0){
    Serial.println();
    Serial.println("Commandes auto :");
    Serial.println("1 : Choisir Canal Radio");
    Serial.println("2 : Lire Canal Radio");
    Serial.println("3 : Set AT timeout to 100s");
    Serial.println("4 : Lire AT timeout value");
    Serial.println("5 : Changer Baud Rate");
    Serial.println("6 : Changer Mode AP");
    Serial.println("7 : Lire Mode AP");
    Serial.println("8 : Changer Multi Transmit Mode");
    Serial.println("9 : Lire Multi Transmit Mode");
    Serial.println();
    Serial.println("Commandes manuelles :");
    Serial.println("* : Valider une commande");
    Serial.println("+ : Ouvrir mode AT");
    Serial.println("- : Fermer mode AT");
    inst = 1;
    Serial.println();
  }
}

void readChannel(void){
  openATmode();
  Serial.print("Channel Value : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('C');
  XBeeSerial.write('M');
  XBeeSerial.write('\r');
  delay(1000);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
}

void readATtime(void){
  openATmode();
  Serial.print("AT Mode Timeout Value : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('C');
  XBeeSerial.write('T');
  XBeeSerial.write('\r');
  delay(1000);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
}

void setATtime100(void){
  openATmode();
  Serial.print("Set AT Timeout to 100s : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('C');
  XBeeSerial.write('T');
  XBeeSerial.write('3');
  XBeeSerial.write('E');
  XBeeSerial.write('8');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changed made permanent : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('W');
  XBeeSerial.write('R');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changes applied : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('A');
  XBeeSerial.write('C');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
}

void setChannel(int mode){
  openATmode();
  Serial.print("Channel configuration : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('C');
  XBeeSerial.write('M');
    
  switch (mode){
    case '0':
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('7');
    break;
    
    case '1':
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('3');
    XBeeSerial.write('8');

    break;
    
    case '2':
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('5');
    XBeeSerial.write('8');
    XBeeSerial.write('0');
    break;
    
    case '3':
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('3');
    XBeeSerial.write('8');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    break;
    
    case '4':
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('1');
    XBeeSerial.write('C');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    break;
    
    case '5':
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('E');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    break;
    
    case '6':
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('7');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    break;
    
    case '7':
    XBeeSerial.write('0');
    XBeeSerial.write('6');
    XBeeSerial.write('8');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    XBeeSerial.write('0');
    break;
    
    default:
    Serial.println();
    Serial.println("Unknown mode");
    break;
  }
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changed made permanent : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('W');
  XBeeSerial.write('R');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changes applied : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('A');
  XBeeSerial.write('C');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
  inst = 0;
}

void setBaud(int mode){
  openATmode();
  Serial.print("Baud Rate configuration : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('B');
  XBeeSerial.write('D');
    
  switch (mode){
    case '3':
    XBeeSerial.write('3');
    break;
    
    case '4':
    XBeeSerial.write('4');

    break;
    
    case '5':
    XBeeSerial.write('5');
    break;
    
    case '6':
    XBeeSerial.write('6');
    break;
    
    case '7':
    XBeeSerial.write('7');
    break;
    
    default:
    Serial.println();
    Serial.println("Unknown mode");
    break;
  }
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changed made permanent : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('W');
  XBeeSerial.write('R');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changes applied, please reboot");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('A');
  XBeeSerial.write('C');
  XBeeSerial.write('\r');
  delay(500);
  while(1){}
}

void setAP(int mode){
  openATmode();
  Serial.print("AP Mode configuration : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('A');
  XBeeSerial.write('P');
    
  switch (mode){
    case '0':
    XBeeSerial.write('0');
    break;
    
    case '1':
    XBeeSerial.write('1');
    break;
    
    default:
    Serial.println();
    Serial.println("Unknown mode");
    break;
  }
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changed made permanent : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('W');
  XBeeSerial.write('R');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changes applied : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('A');
  XBeeSerial.write('C');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
  inst = 0;
}

void readMultiTransmit(void){
  openATmode();
  Serial.print("Multi Transmit attempts : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('M');
  XBeeSerial.write('T');
  XBeeSerial.write('\r');
  delay(1000);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
}

void setMultiTransmit(int mode){
  openATmode();
  Serial.print("Multi Transmit configuration : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('M');
  XBeeSerial.write('T');
    
  switch (mode){
    case '1':
    XBeeSerial.write('1');
    break;
    
    case '2':
    XBeeSerial.write('2');
    break;
    
    case '3':
    XBeeSerial.write('3');
    break;
    
    case '4':
    XBeeSerial.write('4');
    break;
   
    case '5':
    XBeeSerial.write('5');
    break;
    
    default:
    Serial.println();
    Serial.println("Unknown mode");
    break;
  }
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changed made permanent : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('W');
  XBeeSerial.write('R');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  Serial.print("Changes applied : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('A');
  XBeeSerial.write('C');
  XBeeSerial.write('\r');
  delay(500);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
  inst = 0;
}

void readAP(void){
  openATmode();
  Serial.print("AP Mode : ");
  XBeeSerial.write('A');
  XBeeSerial.write('T');
  XBeeSerial.write('A');
  XBeeSerial.write('P');
  XBeeSerial.write('\r');
  delay(1000);
  while(XBeeSerial.available()){
    Serial.write(XBeeSerial.read());
  }
  Serial.println();
  closeATmode();
  Serial.println("Done");
}

