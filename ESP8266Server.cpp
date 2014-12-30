#include "ESP8266Server.h"

void ESP8266Server::begin()
{
    for (unsigned int i = 0; i < 5; i++){
        _clients[i] = new ESP8266Client(_esp8266, i);
    }

    _esp8266.createServer(_port);
}

ESP8266Client * ESP8266Server::available()
{
    if(_esp8266.available() > 0){
        return _clients[_esp8266.getId()];
    } else {
        return NULL;
    }
}
