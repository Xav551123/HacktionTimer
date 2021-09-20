#include "LittleFS.h"
 #include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <DS1307.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
//#include <ArduinoOTA.h>
#include <AsyncElegantOTA.h>;


//const char *ssid = "Bbox-34E9EF0E";
//const char *password = "ED1993992F2EAAAA93C4CD62A6649D";
const char *ssid = "NameOfNetwork";
const char *password = "AardvarkBadgerHedgehog";

const int led = LED_BUILTIN;
const int capteurLuminosite = A0;

AsyncWebServer server(80);

//......................................................LED


#define NP_LED 50

DS1307 horloge;
Adafruit_NeoPixel Neopixel(NP_LED, D6, NEO_GRB + NEO_KHZ800);

int i;
int temps_precedent=0;
int temps=0;
int heure=0;
bool change=0;
int changereset=0;
int trancheprec=0;
int tranche = 0;
int ptime = 0;

//var for time update
const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
String inputMessage= "none";
String inputMessage2= "none";

//var for color update
const char* PARAM_INPUT_GAUCHE = "gauche";
const char* PARAM_INPUT_CENTRE = "center";
const char* PARAM_INPUT_DROITE = "droite";



// var for range update
long Plageset[] = {485, 545, 613, 665,725,785,845,905,965,1025,1085};
char *Plagetest[]={"M1","M2", "M3","M4","M5","M6","S1","S2","S3","S4","S5"};

//------variable couleurs [1]r=red g=green b=blue and [2]g=gauche c=centre d=droite
//gauche
int rg=150;
int gg=0;
int bg=0;
//centre
int rc=83;
int gc=53;
int bc=167;
//droite
int rd=0;
int gd=255;
int bd=0;
//---------------------HTMLCOLORS
String Colorgauche="#ff0000";
String Colorcentre="#5335a7";
String Colordroite="#00ff00";

//-----var for range update
String HTMLPlageselection="M22";
String HTMLPlagevalue="12";
const char* PARAM_INPUT_PLAGESET = "plageset";
const char* PARAM_INPUT_PLAGEVALUE = "timeset";



//---uint16-t généré par vittascience
uint16_t clockRTC_getTime(uint8_t select) {
  horloge.getTime();
  delay(10);
  switch (select) {
    case 0: return horloge.dayOfMonth;
    case 1: return horloge.month;
    case 2: return horloge.year + 2000;
    case 3: return horloge.hour;
    case 4: return horloge.minute;
    case 5: return horloge.second;
  }
}


   

//--void généré par vittascience
void neopixel_showAllLed(Adafruit_NeoPixel *neoPx, uint8_t ledCount, uint8_t r, uint8_t g, uint8_t b) {
  for (int i=0; i<ledCount; i++) {
    neoPx->setPixelColor(i, neoPx->Color(r, g, b));
  }
  neoPx->show();
}

void setup()
{
  //----------------------------------------------------Serial
  Serial.begin(9600);
  Serial.println("\n");

  //----------------------------------------------------GPIO
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
 
  //----------------------------------------------------LittleFS setup
  if(!LittleFS.begin())
  {
    Serial.println("Erreur LittleFS...");
    return;
  }

  File root = LittleFS.open("/","r");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }
//------------------------------------load range stored with littlefs on startup
for(int i=0;i<10;i++){
String chemin = (String("/")+String(Plagetest[i]));
      Serial.println();
      Serial.println(chemin);
     File file = LittleFS.open(chemin, "r");
     
    if (!file) {
    // File not found | le fichier de test n'existe pas
    Serial.println("Failed to open test file");
  } else {
    // Display file content on serial port | Envoie le contenu du fichier test sur le port série
    
    Serial.print("Read var updated from file content: ");
    String result="";
    while (file.available()) {
      //Serial.write(file.read());
      result += (char)file.read();
    }
    Plageset[i]=strtol(result.c_str(),NULL,10);
      
      Serial.println(Plageset[i]);
  }

  file.close();

}
  //----------------------------------------------------WIFI
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");
  
 /*while(WiFi.status() != WL_CONNECTED  )
  {
    Serial.print(".");
    
    delay(100);
  }
  
  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
*/
//----------------------------------------------------SERVER setup
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/w3.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/script.js", "text/javascript");
  });

//-----------------------------------post current time to html
  server.on("/lireMinute", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    
    String minute = String(temps_precedent);//temps precedente
    request->send(200, "text/plain", minute);
  });

  server.on("/lireHeure", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    
    String Sheure = String(heure);
    //Serial.print("flag:");
   // Serial.println(Sheure);
    request->send(200, "text/plain", Sheure);
  });


//--------------------------------------------post currently defined range to html
  server.on("/PlagesetM1", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int Mheure = Plageset[0]/60;
    int MMinute = Plageset[0]- Mheure*60;
    String PlagesetM1 = String(Mheure)+String(":")+String(MMinute);
    //Serial.println(PlagesetM1);
    request->send(200, "text/plain", PlagesetM1);
  });

    server.on("/PlagesetM2", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    
    String PlagesetM2 = String(Plageset[1]);
    //Serial.println(PlagesetM1);
    request->send(200, "text/plain", PlagesetM2);
  });


//-----------------------update color from html form
server.on("/Couleurgauche", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    //Serial.println(Colorgauche);
    request->send(200, "text/plain", Colorgauche);
  });

  server.on("/Couleurcentre", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    //Serial.println(Colorcentre);
    request->send(200, "text/plain", Colorcentre);
  });

  server.on("/Couleurdroite", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    //Serial.println(Colordroite);
    request->send(200, "text/plain", Colordroite);
  });

//---------------------------------------allumer la led de la carte
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(led, LOW);
    request->send(200);
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(led, HIGH);
    request->send(200);
  });
//---------------------reset carte
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    changereset=1;//activation of void loop update fonction
    request->send(200);
  });

//-----------------------------------------update time range from html
server.on("/time", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    
    HTMLPlageselection= request->getParam(PARAM_INPUT_PLAGESET)->value();
    HTMLPlagevalue = request->getParam(PARAM_INPUT_PLAGEVALUE)->value();
   // Serial.println(Plageselection);
   //Serial.println(Plagevalue);
    //--------------save to spiff
    
    String chemin = (String("/")+String(HTMLPlageselection));
      Serial.println(chemin);
      if(LittleFS.remove(chemin)){
      Serial.println("removed");
    }
     File file = LittleFS.open(chemin, "w");
  if (file) {
    
    // Remplace le contenu du fichier, un nouveau fichier est créé s'il n'existe pas
    Serial.println("Write content to file. Create the file if not exist");
    file.println(HTMLPlagevalue);
    Serial.println("done");
    file.close();
  } else {
    Serial.println("Problem on create file!");
  }
    file = LittleFS.open(chemin, "r");  
    if (!file) {
    // File not found | le fichier de test n'existe pas
    Serial.println("Failed to open test file");
  } else {
    // Display file content on serial port | Envoie le contenu du fichier test sur le port série
    Serial.println();
    Serial.print("Read file content: ");
    while (file.available()) {
      Serial.write(file.read());
    }
  }

  file.close();
//-----------------------------save to variable
for(int i=0;i<10;i++){
  if(String(Plagetest[i]) == HTMLPlageselection){
    Serial.println("updating variable");
    Serial.print(Plagetest[i]);
    Serial.print("->");
    Serial.println(Plageset[i]);
    long conversion=strtol(HTMLPlagevalue.c_str(),NULL,10);
    
    Plageset[i]=conversion;
    
    Serial.print("to:");
    Serial.print(Plagetest[i]);
    Serial.print("->");
    Serial.println(Plageset[i]);
    
  }
    
  }
   request->send(200);
   });    
//-----------------------------------------update time from html form

server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    String inputParam;
    String inputParam2;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      change = 1;
    
   // Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage + "and"
                                     + inputParam2 + ") with value2: " + inputMessage2 +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });


//-------------------------changement couleur
server.on("/color", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Colorgauche= request->getParam(PARAM_INPUT_GAUCHE)->value();;
    Colorcentre= request->getParam(PARAM_INPUT_CENTRE)->value();;
    Colordroite= request->getParam(PARAM_INPUT_DROITE)->value();;

  //-------------//-----html couleur gauche to rgb
  long y1 = strtol(Colorgauche.c_str()+1, NULL, 16); // <--
  //Serial.println(y1); 
  rg = ((y1 >> 16) & 0xFF);
  gg = ((y1 >> 8) & 0xFF);
  bg = (y1 & 0xFF);
  //-------------//-----html couleur centre to rgb
  y1 = strtol(Colorcentre.c_str()+1, NULL, 16); // <--
  //Serial.println(y1); 
  rc = ((y1 >> 16) & 0xFF);
  gc = ((y1 >> 8) & 0xFF);
  bc = (y1 & 0xFF);

  //-------------//-----html couleur gauche to rgb
  y1 = strtol(Colordroite.c_str()+1, NULL, 16); // <--
 // Serial.println(y1); 
  rd = ((y1 >> 16) & 0xFF);
  gd = ((y1 >> 8) & 0xFF);
  bd = (y1 & 0xFF);

  Serial.println(String(rg)+(":")+String(gg)+(":")+String(bg)+(":")+String(rc)+(":")+String(gc)+(":")+String(bc)+(":")+String(rd)+(":")+String(gd)+(":")+String(bd));
  
    //Serial.println(Colorgauche);
    //Serial.println(Colorgauche);
    Serial.println(String(Colorgauche)+String(":")+String(Colorcentre)+String(":")+String(Colordroite));
    request->send(200,"text/html","HTTP GET request sent to your ESP on input field ("+Colorgauche +", "+Colordroite+", "+Colorcentre+")"+"<br><a href=\"/\">Return to Home Page</a>");
  });





//----------------------server start
AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("Serveur actif!");
  
  

//----------------------------------------------config led
horloge.begin();
Neopixel.begin();
  temps = 0;
  temps_precedent = clockRTC_getTime(4);
  heure = clockRTC_getTime(3);
  neopixel_showAllLed(&Neopixel, NP_LED, 0,0, 0);
  neopixel_showAllLed(&Neopixel, NP_LED, gd,rd,bd);//GRB color droite
}


void loop()
{
  AsyncElegantOTA.loop();
  
  //---------------------------update time from html form fonction
  if (change==1)
  {
    
    int im= strtol(inputMessage.c_str(),NULL,10);
    int im2= strtol(inputMessage2.c_str(),NULL,10);
Serial.println(String("mise a jour to->")+String(im)+String(":")+String( im2));

    horloge.fillByHMS(im,im2,00);
    horloge.setTime();
    change=0;
  }
  if (changereset==1){
    
    Serial.println(String("reset to")+String(rd)+String(gd)+String(bd));
    uint32_t actual = Neopixel.Color(gd, rd, bd);
     Neopixel.fill(actual, 0);
     Neopixel.show();
    changereset=0;
  }
  if (trancheprec != tranche){
    neopixel_showAllLed(&Neopixel, NP_LED, gd,rd,bd);
    trancheprec = tranche;

  }
  //------------------------------changement de minute
  ptime = clockRTC_getTime(3) * 60 + clockRTC_getTime(4);// minute converted time
  if (clockRTC_getTime(4) != temps_precedent) {
    
    int ttime = ptime - tranche;
    if(tranche != 0){
      for(i=0;i < ttime;i++){
      Neopixel.setPixelColor(i, Neopixel.Color(gc, rc, bc));//grb centre
      Neopixel.show();
      Neopixel.setPixelColor(i - 1, Neopixel.Color(gg, rg, bg));//grb gauche
      Neopixel.show();
      temps_precedent = clockRTC_getTime(4);
      heure = clockRTC_getTime(3);
      delay(10);
    }
    }
    else{
      neopixel_showAllLed(&Neopixel, NP_LED,0,0, 0);
    
    }
    
    
for(int i=0;i<10;i++){
  if(ptime >=Plageset[i] && ptime < Plageset[i+1]){
    tranche = Plageset[i];
    return;
  }
  else{
    tranche = 0;
  }
}

}
}