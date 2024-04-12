#include <Arduino.h>  // Include the Arduino library for Serial
#include <WiFi.h>  // Librairie Wifi.h
#include <WebServer.h>  // Librairie WebServer.h
#define LIGHT_SENSOR_PIN 36 // ESP32 pin GIOP36 (ADC0)  

const char* ssid = "OnePlus 10 Pro 5G";
const char* password = "nigaznigaz97";
WebServer server(80);

// const int led = 5;  // Led sur GPIO5
// const int Photoresistor = 36;  // Photoresistor sur GPIO34
bool etatLed = 0;
char texteEtatLed[2][10] = {"ÉTEINTE","ALLUMÉE"};  // Affichage ETEINTE ou ALLUMEE

void handleRoot(){   // Début de la page HTML
  String page = "<!DOCTYPE html>";
    page += "<html lang='fr'>";
    
    page += "<head>";
    page += "    <title>Serveur ESP32</title>";
    page += "    <meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1' charset='UTF-8'/>";
    page += "    <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>";  // Utilisation du css 
    page += "</head>";

    page += "<body>";
    page += "    <div class='w3-card w3-blue w3-padding-small w3-jumbo w3-center'>";
    page += "        <p>ÉTAT LED: "; page += texteEtatLed[etatLed]; page += "</p>";
    page += "    </div>";

    page += "    <div class='w3-bar'>";
    page += "        <a href='/on' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>ON</a>";
    page += "        <a href='/off' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>OFF</a>";
    page += "    </div>";

    page += "    <div class='w3-center w3-padding-16'>";
    page += "        <p>Ce serveur est hébergé sur un ESP32</p>";
    page += "        <i>Créé par Samuel Assani</i>";
    page += "        <p>Luminosité: "; page += analogRead(LIGHT_SENSOR_PIN); page += "</p>";
    page += "    </div>";

    

    page += "</body>";
    page += "</html>";  // Fin de la page HTML

    server.setContentLength(page.length());  // Permet l'affichage plus rapide après chaque clic sur les boutons
    server.send(200, "text/html", page);
}

void handleOn(){  // Code pour allumer la led
    etatLed = 1;
    digitalWrite(LED_BUILTIN, HIGH);
    server.sendHeader("Location","/");
    server.send(303);
}

void handleOff(){   // Code pour éteindre la led
    etatLed = 0;
    digitalWrite(LED_BUILTIN, LOW);
    server.sendHeader("Location","/");
    server.send(303);
}

void handleNotFound(){  // Page Not found
  server.send(404, "text/plain","404: Not found");
}

void photoresistor(){
  // reads the input on analog pin (value between 0 and 4095)
  int analogValue = analogRead(LIGHT_SENSOR_PIN);

  Serial.print("Analog Value = ");
  Serial.print(analogValue);   // the raw analog reading

  // We'll have a few threshholds, qualitatively determined
  if (analogValue < 40) {
    Serial.println(" => Dark");
  } else if (analogValue < 800) {
    Serial.println(" => Dim");
  } else if (analogValue < 2000) {
    Serial.println(" => Light");
  } else if (analogValue < 3200) {
    Serial.println(" => Bright");
  } else {
    Serial.println(" => Very bright");
  }

  delay(500);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n");

  pinMode(LED_BUILTIN, OUTPUT); // Définition de la led en OUTPUT
  digitalWrite(LED_BUILTIN, 0);  // Initialisation de la led à 0 (éteinte)

  WiFi.persistent(false);
  WiFi.begin(ssid, password);
  Serial.print("Attente de connexion ...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\n");
  Serial.println("Connexion etablie !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);  // Chargement de la page accueil
  server.on("/on", handleOn);  // Chargement du handleOn - Allumée la led
  server.on("/off", handleOff);  // Chargement du handleOff - Eteindre la led
  server.onNotFound(handleNotFound);  // Chargement de la page Not found
  server.begin();

  Serial.println("Serveur web actif");

  // find the ip
  
}


void loop() {

    int analogValue = analogRead(LIGHT_SENSOR_PIN);

    server.handleClient();
    Serial.println(WiFi.localIP());
    Serial.println("Serveur web actif");
    delay(10000);
    photoresistor();
}