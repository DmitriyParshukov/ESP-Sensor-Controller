#include <DHT.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


/*-------------CONFIG--------------------*/
const char*  WIFI_SSID      = "WIFINAME";
const char*  WIFI_Password  = "****";
const String ESP_Password   = "****";
const int    Relay_PIN      = 16;
const int    DHT_PIN        = 5;
/*-------------CONFIG--------------------*/

DHT dht(DHT_PIN, DHT11);
String relayState = "OFF";

ESP8266WebServer server(80);

void setup(void) {
  pinMode(Relay_PIN, OUTPUT);
  digitalWrite(Relay_PIN, LOW);
  relayState = "OFF";

  dht.begin();


  
  
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_Password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/relay/", handleRelay);
  server.on("/dht/", handleDHT);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

void handleRelay() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    Serial.println(server.arg(0));
    Serial.println(server.arg(1));

    if(server.arg(0) == ESP_Password) {
       if(server.arg(1) == "ON") {
           server.send(200, "text/plain", "ON");
           digitalWrite(Relay_PIN, HIGH);
           relayState = "ON";
       }
       else if(server.arg(1) == "OFF") {
           server.send(200, "text/plain", "OFF");
           digitalWrite(Relay_PIN, LOW);
           relayState = "OFF";
       }
       else if(server.arg(1) == "STATE") {
           server.send(200, "text/plain", relayState);
       }
       else {
           server.send(200, "text/plain", "UNKNOWN_COMMAND");
       }
    }
    else {
       server.send(401, "text/plain", "AUTH_FAIL");
    }
  }
}

void handleDHT() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    Serial.println(server.arg(0));

    if(server.arg(0) == ESP_Password) {
       float h = dht.readHumidity(); //Измеряем влажность
       float t = dht.readTemperature(); //Измеряем температуру
       Serial.println(h);
       Serial.println(t);

       String hstr = String( round( h * 10 ) / 10 );
       String tstr = String( round( t * 10 ) / 10 );
       
       server.send(200, "text/plain", "[" + hstr + ", " + tstr + "]");
    }
    else {
       server.send(401, "text/plain", "AUTH_FAIL");
    }
  }
}
