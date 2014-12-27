#ifndef ESP8266Client_h
#define ESP8266Client_h

#include <Client.h>
#include "ESP8266.h"

class ESP8266Client : public Client
{
public:
    ESP8266Client()
    {
        _id = 0;
        _connected = false;
    }

    // Set the underlying ESP8266 and prepare the client
    bool begin(ESP8266& esp8266);
    bool begin(ESP8266& esp8266, unsigned int id);

    // Connect
    int connect(const char* host, uint16_t port);
    int connect(IPAddress ip, uint16_t port);

    // Available
    int available();

    // Read
    int read();
    int read(uint8_t* buffer, size_t size);

    // Write
    size_t write(uint8_t b);
    size_t write(const char* data);
    size_t write(const uint8_t* buffer, size_t size);

    // Peek
    int peek();

    // Flush
    void flush();

    // Stop
    void stop();

    // Connected
    uint8_t connected();

    // Connected
    operator bool();

protected:
    ESP8266* _esp8266;

    unsigned int _id;

    bool _connected;
};

#endif
