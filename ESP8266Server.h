#ifndef ESP8266Server_h
#define ESP8266Server_h

#include "ESP8266.h"
#include "ESP8266Client.h"

class ESP8266Server
{
public:
    ESP8266Server(ESP8266& esp8266, uint16_t port) : _esp8266(esp8266), _port(port) {}
    ESP8266Client * available();
    virtual void begin();

protected:
    uint16_t _port;
    ESP8266Client * _clients[ESP8266_MAX_CONNECTIONS];
    ESP8266 _esp8266;
};

#endif
