// arduino library include by default
#include <Arduino.h>

// Include the ESP32 Wi-Fi library
// helps connect to the internet
#include <WiFi.h>

// Include the ESP32 HTTP client library
// helps send HTTP requests to the server
#include <HTTPClient.h>

// Include the ESP32 Web server library
// helps create a web server on the ESP32
#include <WebServer.h>



// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <ESP8266WebServer.h>

// WIFI information for connection
// Replace with your network credentials
const char* ssid = "ssid";
const char* password = "password";

// Port 80 is used since its default port for HTTP
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds
const long timeoutTime = 2000;

/* Attaches content and title to a POST request to be sent to Ruby server
 * @params 
 * String content : string value which will be written into
 *                  the post on the ruby server
 * String title : Title of the post made on the website
 */
void sendToBlog(String content, String title) {
    
    HTTPClient http;

    HTTPClient
    // Begins HTTP connection to the ruby server
    http.begin("http://192.168.20.23:3000/posts"); 
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Creates POST request string with the custom content attached
    String postData = "post[title]=" + title + "&post[content]=" + content;

    // Sends POST request to RUBY server
    int httpCode = http.POST(postData);
    Serial.println(httpCode);
        
    http.end();
}

void setup() {
  // Init baud rate
  Serial.begin(115200);

  // Init on board LED which is active LOW
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  // Start Connection
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("Connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop(){
  // Checks for incoming clients
  WiFiClient client = server.available();

  if (client) {
    // Reveals new client connection
    Serial.println("New Client.");

    String currentLine = "";               
    currentTime = millis();
    previousTime = currentTime;

    // Loop checks if client is connected and the timout time hasnt expired
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      
      currentTime = millis();        
      
      if (client.available()) {      
        // Reads data sent from client and stores in header variable
        char c = client.read();          
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                

          // Sends client connection info
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/json");
            client.println("Connection: close");
            client.println();

            // Checks which route has been requested by the client
            if (header.indexOf("GET /LED/on") >= 0) {
              
              // Turns on LED and sends JSON data to Server
              Serial.println("Turning LED on");
              client.println("{\"Rec\": \"LEDon\"}");
              digitalWrite(LED_BUILTIN, LOW);
              
              // Creates new blog post saying LED has been turned on
              sendToBlog("Turning LED ON", "LED on");
              
            } else if (header.indexOf("GET /LED/off") >= 0) {
              
              // Turns off LED and sends JSON data to Server
              Serial.println("Turning LED off");
              client.println("{\"Rec\": \"LEDoff\"}");
              digitalWrite(LED_BUILTIN, HIGH);
              
              // Creates new blog post saying LED has been turned off
              sendToBlog("Turning LED OFF","LED off");
              
            } 
            
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;    
        }
        
      }  
    }
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  
}
     