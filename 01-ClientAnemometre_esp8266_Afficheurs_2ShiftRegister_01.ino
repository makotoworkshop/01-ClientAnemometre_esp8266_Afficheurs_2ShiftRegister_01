#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// https://www.instructables.com/id/ESP8266-Parsing-JSON/
// https://arduinojson.org/v5/assistant/
// https://arduinojson.org/v5/faq/the-first-parsing-succeeds-why-do-the-next-ones-fail/
/****************/
/* DÉCLARATIONS */
/****************/
#define ssid      "xxx"      // WiFi SSID
#define password  "xxx"      // WiFi password
#define Data0 D0  // Input LSB Afficheur DLG7137
#define Data1 D1
#define Data2 D2
#define Data3 D3
#define Data4 D4
#define Data5 D5
#define Data6 D6 // Input MSB Afficheur DLG7137
#define datapin D8   // pin  pour les données vers les registres à décallages
#define clockpin D7  // pin  pour l'horloge qui coordonne les registres à décallages
static const uint8_t latchpin1 = 3; // utilise la pin RX/TX donc sérial print impossible sans désassigner
static const uint8_t latchpin2 = 1; // ces pins ni les dessouder
//static const uint8_t latchpin1 = 25;
//static const uint8_t latchpin2 = 25;
long VitesseVent;
long rpmEolienne;
int VitesseVentC = 0;
int VitesseVentD = 0;
int VitesseVentU = 0;
int VitesseEolienneC = 0;
int VitesseEolienneD = 0;
int VitesseEolienneU = 0;
byte dataArray[16];    // Tableau de données

/*********/
/* SETUP */
/*********/
void setup() {
  Serial.begin ( 115200 );  // init du mode débug
  // Connexion au WiFi
  WiFi.begin ( ssid, password );
  // Attente de la connexion au réseau WiFi / Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
//   Connexion WiFi établie
  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );
  
//==== Afficheurs ====//
  pinMode(Data0, OUTPUT);
  pinMode(Data1, OUTPUT);
  pinMode(Data2, OUTPUT);
  pinMode(Data3, OUTPUT);
  pinMode(Data4, OUTPUT);
  pinMode(Data5, OUTPUT);
  pinMode(Data6, OUTPUT);
  pinMode(clockpin, OUTPUT);     // pin correspondant à "clockpin" initialisée en sortie
  pinMode(datapin, OUTPUT);      // pin correspondant à "datakpin" initialisée en sortie
  pinMode(latchpin1, OUTPUT);     // pin correspondant à "latchpin" initialisée en sortie
  pinMode(latchpin2, OUTPUT);     // pin correspondant à "latchpin" initialisée en sortie

// low byte - 8 bits premier registre
  dataArray[0] = B01111111;  // Case du tableau qui contient la valeur binaire pour permettre d'adresser l'afficheur unité
  dataArray[1] = B10111111;
  dataArray[2] = B11011111; 
  dataArray[3] = B11101111; 
  dataArray[4] = B11110111; 
  dataArray[5] = B11111011; 
  dataArray[6] = B11111101;
  dataArray[7] = B11111110;
// high byte - 8 bits second registre
  dataArray[8] = B01111111;
  dataArray[9] = B10111111;
 dataArray[10] = B11011111;  
 dataArray[11] = B11101111; 
 dataArray[12] = B11110111; 
 dataArray[13] = B11111011; 
 dataArray[14] = B11111101;
 dataArray[15] = B11111110;   
}

/*************/
/* PROGRAMME */
/*************/
void loop() {
  reception();
  CalculReception();
  Afficheurs();
      // Delay
  delay(500);
    
//  VitesseVentC = 2;
//  VitesseVentD = 7;
//  VitesseVentU = 4;
//  VitesseEolienneC = 5;
//  VitesseEolienneD = 6;
//  VitesseEolienneU = 2;
// TestAfficheurs();
}

/*************/
/* FONCTIONS */
/*************/
void reception() {
  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  //Object of class HTTPClient
    http.begin("http://192.168.0.34:8080/mesures.json");
    int httpCode = http.GET();
    //Check the returning code                                                                  
//    if (httpCode > 0) {
      // Parsing
      const size_t bufferSize = JSON_OBJECT_SIZE(2) + 40;
      DynamicJsonBuffer jsonBuffer(bufferSize);

      JsonObject& root = jsonBuffer.parseObject(http.getString());  // parsing sur l'url donnée plus haut

    if (root.success()) {
      VitesseVent = root["VitesseVent"];
      rpmEolienne = root["rpmEolienne"];

      Serial.print("VitesseVent:");
      Serial.println(VitesseVent);
      Serial.print("rpmEolienne:");
      Serial.println(rpmEolienne);
    } 
    else {
    Serial.println("JSON parsing failed!");
    Nodata();
    }
//  }
    http.end();   //Close connection
  }
  else
  {
    Serial.println("No Wifi");
    Nowifi();
  }
}


void CalculReception ()
{
//VitesseVent=359.29;
//rpmEolienne=321.00;
  VitesseVentC = VitesseVent / 100;   // par calcul, extrait la dizaine  
  VitesseVentD = VitesseVent % 100 / 10;   // par calcul, extrait la dizaine
  VitesseVentU = VitesseVent % 10;  // par calcul, extrait l'unités
    Serial.print(" VitesseVentC: ");
    Serial.println(VitesseVentC);
    Serial.print(" VitesseVentD: ");
    Serial.println(VitesseVentD); 
    Serial.print(" VitesseVentU: ");
    Serial.println(VitesseVentU); 

  VitesseEolienneC = rpmEolienne / 100;   // par calcul, extrait la dizaine  
  VitesseEolienneD = rpmEolienne % 100 / 10;   // par calcul, extrait la dizaine
  VitesseEolienneU = rpmEolienne % 10;  // par calcul, extrait l'unités
    Serial.println("  ");
    Serial.print(" VitesseEolienneC: ");
    Serial.println(VitesseEolienneC);
    Serial.print(" VitesseEolienneD: ");
    Serial.println(VitesseEolienneD); 
    Serial.print(" VitesseEolienneU: ");
    Serial.println(VitesseEolienneU); 
}


void Afficheurs() {
 //==== Afficheurs ====//
 
// adresse le chiffre des Dizaines sur l'afficheur du bas
  switch (VitesseEolienneD)
  {
  case 0:
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Zero();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 1: 
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Un();
  digitalWrite(latchpin1, 0); 
  break;

  case 2:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Deux();
  digitalWrite(latchpin1, 0); 
  break;
 
  case 3:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Trois();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 4:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Quatre();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 5:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Cinq();
  digitalWrite(latchpin1, 0); 
  break;

  case 6:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Six();
  digitalWrite(latchpin1, 0); 
  break;

  case 7:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Sept();
  digitalWrite(latchpin1, 0); 
  break;

  case 8:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Huit();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 9:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Neuf();
  digitalWrite(latchpin1, 0); 
  break;
  }
// adresse le chiffre des Centaines sur l'afficheur du Haut
  switch (VitesseVentC) 
  {
  case 0:
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Zero();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 1: 
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Un();
  digitalWrite(latchpin1, 0); 
  break;

  case 2:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Deux();
  digitalWrite(latchpin1, 0); 
  break;
 
  case 3:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Trois();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 4:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Quatre();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 5:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Cinq();
  digitalWrite(latchpin1, 0); 
  break;

  case 6:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Six();
  digitalWrite(latchpin1, 0); 
  break;

  case 7:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Sept();
  digitalWrite(latchpin1, 0); 
  break;

  case 8:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Huit();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 9:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Neuf();
  digitalWrite(latchpin1, 0); 
  break;
  }
// adresse le chiffre des Dizaines sur l'afficheur du Haut 
  switch (VitesseVentD)
  {
  case 0:
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Zero();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 1: 
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Un();
  digitalWrite(latchpin1, 0); 
  break;

  case 2:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Deux();
  digitalWrite(latchpin1, 0); 
  break;
 
  case 3:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Trois();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 4:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Quatre();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 5:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Cinq();
  digitalWrite(latchpin1, 0); 
  break;

  case 6:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Six();
  digitalWrite(latchpin1, 0); 
  break;

  case 7:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Sept();
  digitalWrite(latchpin1, 0); 
  break;

  case 8:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Huit();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 9:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Neuf();
  digitalWrite(latchpin1, 0); 
  break;
  }
// adresse le chiffre des Unité sur l'afficheur du Haut
  switch (VitesseVentU)
  {
  case 0:
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Zero();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 1: 
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Un();
  digitalWrite(latchpin1, 0); 
  break;

  case 2:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Deux();
  digitalWrite(latchpin1, 0); 
  break;
 
  case 3:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Trois();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 4:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Quatre();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 5:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Cinq();
  digitalWrite(latchpin1, 0); 
  break;

  case 6:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Six();
  digitalWrite(latchpin1, 0); 
  break;

  case 7:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Sept();
  digitalWrite(latchpin1, 0); 
  break;

  case 8:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Huit();
  digitalWrite(latchpin1, 0); 
  break;
  
  case 9:   
  digitalWrite(latchpin1, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Neuf();
  digitalWrite(latchpin1, 0); 
  break;
  }
  
// adresse les Unités de mesures  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[4]); 
  K();
  digitalWrite(latchpin1, 0);            
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[5]); 
  m();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[7]); 
  m();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[8]); 
  p();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[9]); 
  h();
  digitalWrite(latchpin2, 0); 
  
// adresse le chiffre des Unités sur l'afficheur du Bas
  switch (VitesseEolienneU)
  {
  case 0:
  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Zero();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 1: 
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Un();
  digitalWrite(latchpin2, 0); 
  break;

  case 2:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Deux();
  digitalWrite(latchpin2, 0); 
  break;
 
  case 3:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Trois();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 4:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Quatre();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 5:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Cinq();
  digitalWrite(latchpin2, 0); 
  break;

  case 6:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Six();
  digitalWrite(latchpin2, 0); 
  break;

  case 7:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Sept();
  digitalWrite(latchpin2, 0); 
  break;

  case 8:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Huit();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 9:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Neuf();
  digitalWrite(latchpin2, 0); 
  break;
  }
  
// adresse les Unités de mesures  
  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[13]); 
  r();
  digitalWrite(latchpin2, 0); 
             
// adresse le chiffre des Centaines sur l'afficheur du Bas 
  switch (VitesseEolienneC)
  {
  case 0:
  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Zero();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 1: 
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Un();
  digitalWrite(latchpin2, 0); 
  break;

  case 2:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Deux();
  digitalWrite(latchpin2, 0); 
  break;
 
  case 3:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Trois();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 4:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Quatre();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 5:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Cinq();
  digitalWrite(latchpin2, 0); 
  break;

  case 6:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Six();
  digitalWrite(latchpin2, 0); 
  break;

  case 7:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Sept();
  digitalWrite(latchpin2, 0); 
  break;

  case 8:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Huit();
  digitalWrite(latchpin2, 0); 
  break;
  
  case 9:   
  digitalWrite(latchpin2, 1);
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Neuf();
  digitalWrite(latchpin2, 0); 
  break;
  }
 // delay(300);
}








void Nowifi() {
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  blank();
  digitalWrite(latchpin1, 0);  
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  N();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  o();
  digitalWrite(latchpin1, 0);             
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  W();
  digitalWrite(latchpin1, 0);            
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[4]); 
  i();
  digitalWrite(latchpin1, 0); 
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[5]); 
  LettreF();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[7]); 
  blank();
  digitalWrite(latchpin1, 0); 
  
  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[8]); 
  blank();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[9]); 
  i();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  blank();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[13]); 
  blank();
  digitalWrite(latchpin2, 0);   

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  blank();
  digitalWrite(latchpin2, 0); 

    delay(5000);
}

void Nodata() {
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  blank();
  digitalWrite(latchpin1, 0);  
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  N();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  o();
  digitalWrite(latchpin1, 0);             
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  D();
  digitalWrite(latchpin1, 0);            
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[4]); 
  a();
  digitalWrite(latchpin1, 0); 
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[5]); 
  t();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[7]); 
  blank();
  digitalWrite(latchpin1, 0); 
    digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[8]); 
  blank();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[9]); 
  a();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  blank();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[13]); 
  blank();
  digitalWrite(latchpin2, 0);   

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  blank();
  digitalWrite(latchpin2, 0); 

    delay(5000);
}


//==== Fonctions Afficheurs ====//

void Zero ()
{
  digitalWrite(Data6, LOW);
  digitalWrite(Data5, HIGH);
  digitalWrite(Data4, HIGH);
  digitalWrite(Data3, LOW);
  digitalWrite(Data2, LOW);
  digitalWrite(Data1, LOW);
  digitalWrite(Data0, LOW);
}
void Un ()
{
  digitalWrite(Data6, LOW);
  digitalWrite(Data5, HIGH);
  digitalWrite(Data4, HIGH);
  digitalWrite(Data3, LOW);
  digitalWrite(Data2, LOW);
  digitalWrite(Data1, LOW);
  digitalWrite(Data0, HIGH);
}
void Deux ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 0);
}
void Trois ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 1);
}
void Quatre ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}
void Cinq ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 1);
}
void Six ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 0);
}
void Sept ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 1);
}
void Huit ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}
void Neuf ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 1);
}
void r ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 0);
}
void p ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}
void m ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 1);
}
void K ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 0);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 1);
}
void h ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}
void slash ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 1);
}
void x ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}
void N ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 0);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 0);
}
void o ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 1);
}
void W ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 0);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 1);
}
void i ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 1);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 1);
}
void LettreF ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 0);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 1);
  digitalWrite(Data0, 0);
}
void a ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 1);
}
void D ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 0);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}
void t ()
{
  digitalWrite(Data6, 1);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 1);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 1);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}
void blank ()
{
  digitalWrite(Data6, 0);
  digitalWrite(Data5, 1);
  digitalWrite(Data4, 0);
  digitalWrite(Data3, 0);
  digitalWrite(Data2, 0);
  digitalWrite(Data1, 0);
  digitalWrite(Data0, 0);
}


void TestAfficheurs() {
          
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[0]); 
  Sept();
  digitalWrite(latchpin1, 0);  
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[1]); 
  Zero();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[2]); 
  Un();
  digitalWrite(latchpin1, 0);             
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[3]); 
  Deux();
  digitalWrite(latchpin1, 0);            
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[4]); 
  Trois();
  digitalWrite(latchpin1, 0); 
  
  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[5]); 
  Quatre();
  digitalWrite(latchpin1, 0); 

  digitalWrite(latchpin1, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[7]); 
  p();
  digitalWrite(latchpin1, 0); 



  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[8]); 
  r();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[9]); 
  Cinq();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
  Huit();
  digitalWrite(latchpin2, 0); 

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[13]); 
  Neuf();
  digitalWrite(latchpin2, 0);   

  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
  Six();
  digitalWrite(latchpin2, 0); 
  
//  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
//  shiftOut(datapin, clockpin, LSBFIRST, dataArray[12]); 
//  x();
//  digitalWrite(latchpin2, 0); 
//
//  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
//  shiftOut(datapin, clockpin, LSBFIRST, dataArray[13]); 
//  x();
//  digitalWrite(latchpin2, 0); 
//
//  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
//  shiftOut(datapin, clockpin, LSBFIRST, dataArray[14]); 
//  x();
//  digitalWrite(latchpin2, 0); 
//
//  digitalWrite(latchpin2, 1);                              // latch à l'état HAUT pour autoriser le transfert des données série   
//  shiftOut(datapin, clockpin, LSBFIRST, dataArray[15]); 
//  x();
//  digitalWrite(latchpin2, 0); 
  delay(300);
}
