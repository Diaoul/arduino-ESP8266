#include "ESP8266Client.h"

bool ESP8266Client::begin()
{
    if (_esp8266->setMultipleConnections(true) != ESP8266_COMMAND_OK)
        return false;

    return true;
}

int ESP8266Client::connect(const char* host, uint16_t port)
{
    ESP8266CommandStatus status = _esp8266->connect(_id, ESP8266_PROTOCOL_TCP, host, port);

    if (status == ESP8266_COMMAND_OK || status == ESP8266_COMMAND_ALREADY_CONNECTED) {
        _connected = true;
        return 1;
    }

    _connected = false;
    return 0;
}

int ESP8266Client::connect(IPAddress ip, uint16_t port)
{
    ESP8266CommandStatus status = _esp8266->connect(_id, ESP8266_PROTOCOL_TCP, ip, port);

    if (status == ESP8266_COMMAND_OK || status == ESP8266_COMMAND_ALREADY_CONNECTED) {
        _connected = true;
        return 1;
    }

    _connected = false;
    return 0;
}

int ESP8266Client::available()
{
    return _esp8266->available();
}


int ESP8266Client::read()
{
    return _esp8266->read();
}

int ESP8266Client::read(uint8_t* buffer, size_t size)
{
    return _esp8266->read(buffer, size);
}

size_t ESP8266Client::write(uint8_t b)
{
    ESP8266CommandStatus status = _esp8266->send(_id, &b, 1);

    if (status == ESP8266_COMMAND_NO_LINK)
        _connected = false;

    if (status != ESP8266_COMMAND_OK)
        return 0;

    return 1;
}

size_t ESP8266Client::write(const char* data)
{
    ESP8266CommandStatus status = _esp8266->send(_id, data);

    if (status == ESP8266_COMMAND_NO_LINK)
        _connected = false;

    if (status != ESP8266_COMMAND_OK)
        return 0;

    return strlen(data);
}

size_t ESP8266Client::write(const uint8_t* buffer, size_t size)
{
    ESP8266CommandStatus status = _esp8266->send(_id, buffer, size);

    if (status == ESP8266_COMMAND_NO_LINK)
        _connected = false;

    if (status != ESP8266_COMMAND_OK)
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
    _connected = false;
}

uint8_t ESP8266Client::connected()
{
    ESP8266Connection connections[ESP8266_MAX_CONNECTIONS - 1];
    ESP8266ConnectionStatus status;
    unsigned int count;

    // check _connected flag
    if (_connected)
        return 1;

    // assume connected if data is available
    if (available() > 0)
        return 1;

    // get connection status
    if (_esp8266->getConnectionStatus(status, connections, count) != ESP8266_COMMAND_OK)
        return 0;

    for (unsigned int i = 0; i < count; i++) {
        if (connections[i].id == _id) {
            _connected = true;
            return 1;
        }
    }

    return 0;
}

ESP8266Client::operator bool()
{
    return connected();
}
