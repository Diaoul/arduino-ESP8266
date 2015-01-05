#include <ESP8266.h>
#include <ESP8266Server.h>

ESP8266 wifi = ESP8266(Serial1);
ESP8266Server server(wifi, 80);
IPAddress ip;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  wifi.begin();
  wifi.setMode(ESP8266_WIFI_BOTH);
  wifi.setMultipleConnections(true);
  wifi.joinAP("ssid", "p4ssw0rd");

  wifi.getIP(ESP8266_WIFI_STATION, ip);
  Serial.println(ip);

  server.begin();
}

void loop(){

  ESP8266Client * client = server.available();

  if (client){
    while(client->available()){
      Serial.write(client->read());
    }

    client->print(
      "HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html\r\n\r\n"
      "<html>\r\n"
      "<head>\r\n"
      "<title>ESP8266Server</title>\r\n"
      "</head>\r\n"
      "<body>\r\n"
      "<h1>IP: "
      );
      
    client->print(ip);
    
    client->print(
      "<h1>\r\n"
      "</body>\r\n"
      "</html>"
      );

    client->stop();
  }
}
