#include <SoftwareSerial.h>

#include "ESP8266.h"

SoftwareSerial esp8266Serial = SoftwareSerial(10, 11);
ESP8266 wifi = ESP8266(esp8266Serial);

void setup()
{
    Serial.begin(9600);

    // ESP8266
    esp8266Serial.begin(9600);
    wifi.begin();
    wifi.setTimeout(1000);

    /****************************************/
    /******       Basic commands       ******/
    /****************************************/
    // test
    /*Serial.print("test: ");
    Serial.println(getStatus(wifi.test()));*/

    // restart
    /*Serial.print("restart: ");
    Serial.println(getStatus(wifi.restart()));*/

    // getVersion
    char version[16] = {};
    Serial.print("getVersion: ");
    Serial.print(getStatus(wifi.getVersion(version, 16)));
    Serial.print(" : ");
    Serial.println(version);

    // deepSleep
    /*Serial.print("deepSleep: ");
    Serial.println(wifi.deepSleep(5000));*/

    // setBaudrate
    /*Serial.print("setBaudrate: ");
    Serial.println(wifi.setBaudrate(9600));*/

    /****************************************/
    /******        WiFi commands       ******/
    /****************************************/
    // setWifiMode
    Serial.print("setWifiMode: ");
    Serial.println(getStatus(wifi.setMode(ESP8266_WIFI_ACCESSPOINT)));

    // getWifiMode
    /*ESP8266WifiMode mode;
    Serial.print("getWifiMode: ");
    Serial.println(getStatus(wifi.getMode(&mode)));*/

    // joinAP
    /*Serial.print("joinAP: ");
    Serial.println(getStatus(wifi.joinAP("ssid", "password")));*/

    // getAP
    /*char ap[32] = {};
    Serial.print("getAP: ");
    Serial.print(getStatus(wifi.getAP(ap)));
    Serial.print(" : ");
    Serial.println(ap);*/

    // quitAP
    /*Serial.print("quitAP: ");
    Serial.println(getStatus(wifi.quitAP()));*/

    // setAPConfiguration
    Serial.print("setAPConfiguration: ");
    Serial.println(getStatus(wifi.setAPConfiguration("ESP8266", "awesomelib", 10, ESP8266_ENCRYPTION_WPA_WPA2_PSK)));
    wifi.restart();

    // getAPConfiguration
    /*char ssid[32] = {};
    char password[63] = {};
    uint8_t channel;
    ESP8266Encryption encryption;
    Serial.print("getAPConfiguration: ");
    Serial.print(getStatus(wifi.getAPConfiguration(ssid, password, channel, encryption)));
    Serial.print(" : ");
    Serial.print(ssid);
    Serial.print(" - ");
    Serial.print(password);
    Serial.print(" - ");
    Serial.print(channel);
    Serial.print(" - ");
    Serial.println(encryption);*/

    // setDHCP
    /*Serial.print("setDHCP STA: ");
    Serial.println(getStatus(wifi.setDHCP(ESP8266_WIFI_STATION, false)));
    Serial.print("setDHCP AP: ");
    Serial.println(getStatus(wifi.setDHCP(ESP8266_WIFI_ACCESSPOINT, true)));*/

    // setMAC
    /*byte mac[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    Serial.print("setMAC AP: ");
    Serial.println(getStatus(wifi.setMAC(ESP8266_WIFI_ACCESSPOINT, mac)));*/

    // getMAC
    /*mac[6] = {};
    Serial.print("getMAC STA: ");
    Serial.print(getStatus(wifi.getMAC(ESP8266_WIFI_STATION, mac)));
    Serial.print(" : ");
    for (uint8_t i = 0; i < 6; i++) {
      Serial.print(mac[i], HEX);
      if (i < 5)
        Serial.print(":");
    }
    Serial.println();
    Serial.print("getMAC AP: ");
    Serial.print(getStatus(wifi.getMAC(ESP8266_WIFI_ACCESSPOINT, mac)));
    Serial.print(" : ");
    for (uint8_t i = 0; i < 6; i++) {
      Serial.print(mac[i], HEX);
      if (i < 5)
        Serial.print(":");
    }
    Serial.println();*/

    // setIP
    /*IPAddress ipAP = IPAddress(10, 0, 4, 1);
    Serial.print("setIP AP: ");
    Serial.print(getStatus(wifi.setIP(ESP8266_WIFI_ACCESSPOINT, ipAP)));
    Serial.print(" : ");
    Serial.println(ipAP);*/

    // getIP
    /*IPAddress ip;
    Serial.print("getIP STA: ");
    Serial.print(getStatus(wifi.getIP(ESP8266_WIFI_STATION, ip)));
    Serial.print(" : ");
    Serial.println(ip);
    Serial.print("getIP AP: ");
    Serial.print(getStatus(wifi.getIP(ESP8266_WIFI_ACCESSPOINT, ip)));
    Serial.print(" : ");
    Serial.println(ip);*/

    /****************************************/
    /******       TCP/IP commands      ******/
    /****************************************/
    // connect
    /*Serial.print("connect: ");
    Serial.println(getStatus(wifi.connect(ESP8266_PROTOCOL_TCP, IPAddress(192,168,4,100), 4000)));*/

    // send
    /*Serial.print("send: ");
    Serial.println(getStatus(wifi.send("Hello World\n")));*/

    // close
    /*Serial.print("close: ");
    Serial.println(getStatus(wifi.close()));*/

    // setMultipleConnections
    Serial.print("setMultipleConnections: ");
    Serial.println(getStatus(wifi.setMultipleConnections(true)));

    // getMultipleConnections
    /*bool multipleConnections;
    Serial.print("getMultipleConnections: ");
    Serial.print(getStatus(wifi.getMultipleConnections(multipleConnections)));
    Serial.print(" : ");
    Serial.println(multipleConnections);*/

    // createServer
    Serial.print("createServer: ");
    Serial.println(getStatus(wifi.createServer(4000)));

    // deleteServer
    Serial.print("deleteServer: ");
    Serial.println(getStatus(wifi.deleteServer()));

    // setServerTimeout
    /*Serial.print("setServerTimeout: ");
    Serial.println(getStatus(wifi.setServerTimeout(10)));*/

    // getServerTimeout
    /*unsigned int timeout;
    Serial.print("getServerTimeout: ");
    Serial.print(getStatus(wifi.getServerTimeout(timeout)));
    Serial.print(" : ");
    Serial.println(timeout);*/
}

void loop()
{
    /****************************************/
    /******        WiFi commands       ******/
    /****************************************/
    // getConnectedStations
    /*ESP8266Station stations[5];
    unsigned int stationCount;
    Serial.print("getConnectedStations: ");
    Serial.print(getStatus(wifi.getConnectedStations(stations, stationCount, 5)));
    Serial.print(" : ");
    Serial.println(stationCount);
    for (uint8_t i = 0; i < stationCount; i++) {
      Serial.print(" - ");
      Serial.print(stations[i].ip);
      Serial.print(" - ");
      for (uint8_t j = 0; j < 6; j++) {
        Serial.print(stations[i].mac[j], HEX);
        if (j < 5)
          Serial.print(":");
      }
      Serial.println();
    }
    delay(5000);*/

    /****************************************/
    /******       TCP/IP commands      ******/
    /****************************************/
    // getConnectionStatus
    /*ESP8266ConnectionStatus connectionStatus;
    ESP8266Connection connections[5];
    unsigned int connectionCount;
    Serial.print("getConnectionStatus: ");
    Serial.print(getStatus(wifi.getConnectionStatus(connectionStatus, connections, connectionCount)));
    Serial.print(" : ");
    Serial.println(connectionCount);
    for (int i = 0; i < connectionCount; i++) {
      Serial.print(" - Connection: ");
      Serial.print(connections[i].id);
      Serial.print(" - ");
      Serial.print(getProtocol(connections[i].protocol));
      Serial.print(" - ");
      Serial.print(connections[i].ip);
      Serial.print(":");
      Serial.print(connections[i].port);
      Serial.print(" - ");
      Serial.println(getRole(connections[i].role));
    }
    delay(5000);*/


    // read data
    /*unsigned int id;
    int length;
    int totalRead;
    char buffer[11] = {};

    if ((length = wifi.available()) > 0) {
      id = wifi.getId();
      totalRead = wifi.read(buffer, 10);

      if (length > 0) {
        Serial.print("Received ");
        Serial.print(totalRead);
        Serial.print("/");
        Serial.print(length);
        Serial.print(" bytes from client ");
        Serial.print(id);
        Serial.print(": ");
        Serial.println((char*)buffer);
      }
    }*/
}

String getStatus(bool status)
{
    if (status)
        return "OK";

    return "KO";
}

String getStatus(ESP8266CommandStatus status)
{
    switch (status) {
    case ESP8266_COMMAND_INVALID:
        return "INVALID";
        break;

    case ESP8266_COMMAND_TIMEOUT:
        return "TIMEOUT";
        break;

    case ESP8266_COMMAND_OK:
        return "OK";
        break;

    case ESP8266_COMMAND_NO_CHANGE:
        return "NO CHANGE";
        break;

    case ESP8266_COMMAND_ERROR:
        return "ERROR";
        break;

    case ESP8266_COMMAND_NO_LINK:
        return "NO LINK";
        break;

    case ESP8266_COMMAND_TOO_LONG:
        return "TOO LONG";
        break;

    case ESP8266_COMMAND_FAIL:
        return "FAIL";
        break;

    default:
        return "UNKNOWN COMMAND STATUS";
        break;
    }
}

String getRole(ESP8266Role role)
{
    switch (role) {
    case ESP8266_ROLE_CLIENT:
        return "CLIENT";
        break;

    case ESP8266_ROLE_SERVER:
        return "SERVER";
        break;

    default:
        return "UNKNOWN ROLE";
        break;
    }
}

String getProtocol(ESP8266Protocol protocol)
{
    switch (protocol) {
    case ESP8266_PROTOCOL_TCP:
        return "TCP";
        break;

    case ESP8266_PROTOCOL_UDP:
        return "UDP";
        break;

    default:
        return "UNKNOWN PROTOCOL";
        break;
    }
}


