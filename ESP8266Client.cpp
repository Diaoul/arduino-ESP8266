#include "ESP8266Client.h"

bool ESP8266Client::begin(ESP8266& esp8266)
{
    _esp8266 = &esp8266;

    if (_esp8266->getMultipleConnections(_multipleConnections) != ESP8266_COMMAND_OK)
        return false;

    return true;
}

int ESP8266Client::connect(const char* host, uint16_t port) {
    //TODO
    return 0;
}

int ESP8266Client::connect(IPAddress ip, uint16_t port)
{
    if (_multipleConnections) {
        for (uint8_t i = 0; i < ESP8266_MAX_CONNECTIONS; i++) {
            if (_esp8266->connect(ESP8266_PROTOCOL_TCP, ip, port) == ESP8266_COMMAND_OK) {
                _id = i;
                break;
            }

            if (i == ESP8266_MAX_CONNECTIONS - 1)
                return 0;
        }
    } else {
        if (_esp8266->connect(ESP8266_PROTOCOL_TCP, ip, port) != ESP8266_COMMAND_OK)
            return 0;

        _id = ESP8266_SINGLE_CLIENT;
    }

    _ip = ip;
    _port = port;

    return 1;
}

int ESP8266Client::available() {
    return _esp8266->available();
}


int ESP8266Client::read() {
    return _esp8266->read();
}

int ESP8266Client::read(uint8_t* buffer, size_t size) {
    return _esp8266->read(buffer, size);
}

size_t ESP8266Client::write(uint8_t b)
{
    if (_esp8266->send(_id, &b, 1) != ESP8266_COMMAND_OK)
        return 0;

    return 1;
}

size_t ESP8266Client::write(const uint8_t* buffer, size_t size)
{
    if (_esp8266->send(_id, buffer, size) != ESP8266_COMMAND_OK)
        return 0;

    return size;
}

int ESP8266Client::peek()
{
    return _esp8266->peek();
}

void ESP8266Client::flush()
{
    return _esp8266->flush();
}

void ESP8266Client::stop()
{
    _esp8266->close(_id);
}

uint8_t ESP8266Client::connected()
{
    ESP8266Connection connections[ESP8266_MAX_CONNECTIONS - 1];
    ESP8266ConnectionStatus status;
    unsigned int count;

    if (_esp8266->getConnectionStatus(status, connections, count) != ESP8266_COMMAND_OK)
        return 0;

    for (unsigned int i = 0; i < count; i++) {
        if (connections[i].protocol == ESP8266_PROTOCOL_TCP && connections[i].ip == _ip && connections[i].port == _port && connections[i].role == ESP8266_ROLE_CLIENT)
            return 1;
    }

    return 0;
}

ESP8266Client::operator bool()
{
    return connected();
}
