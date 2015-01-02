#ifndef ESP8266_h
#define ESP8266_h

#include <Arduino.h>
#include <IPAddress.h>

//#define ESP8266_DEBUG

#define ESP8266_MAX_CONNECTIONS 5
#define ESP8266_SINGLE_CLIENT 5

enum ESP8266WifiMode {
    ESP8266_WIFI_STATION = 1,
    ESP8266_WIFI_ACCESSPOINT,
    ESP8266_WIFI_BOTH
};

enum ESP8266Encryption {
    ESP8266_ENCRYPTION_OPEN = 0,
    ESP8266_ENCRYPTION_WEP,
    ESP8266_ENCRYPTION_WPA_PSK,
    ESP8266_ENCRYPTION_WPA2_PSK,
    ESP8266_ENCRYPTION_WPA_WPA2_PSK
};

enum ESP8266ConnectionStatus {
    ESP8266_CONNECTION_GOT_IP = 2,
    ESP8266_CONNECTION_CONNECTED,
    ESP8266_CONNECTION_DISCONNECTED
};

enum ESP8266Protocol {
    ESP8266_PROTOCOL_TCP = 0,
    ESP8266_PROTOCOL_UDP
};

enum ESP8266Role {
    ESP8266_ROLE_CLIENT = 0,
    ESP8266_ROLE_SERVER
};

enum ESP8266CommandStatus {
    ESP8266_COMMAND_INVALID = -2,
    ESP8266_COMMAND_TIMEOUT,
    ESP8266_COMMAND_OK,
    ESP8266_COMMAND_NO_CHANGE,
    ESP8266_COMMAND_ERROR,
    ESP8266_COMMAND_NO_LINK,
    ESP8266_COMMAND_TOO_LONG,
    ESP8266_COMMAND_FAIL,
    ESP8266_COMMAND_ALREADY_CONNECTED
};

struct ESP8266Connection {
    unsigned int id;
    ESP8266Protocol protocol;
    IPAddress ip;
    unsigned int port;
    ESP8266Role role;
};

struct ESP8266Station {
    IPAddress ip;
    byte mac[6];
};

//TODO: ESP8266AccessPoint struct

class ESP8266 : public Stream
{
public:
    ESP8266(Stream& serial) : Stream(), _serial(&serial) {}

    // Prepare the module
    bool begin();

    /****************************************/
    /******       Basic commands       ******/
    /****************************************/
    // Test AT startup
    ESP8266CommandStatus test();

    // Restart module. Returns false if an error occurred
    bool restart();

    // Get version info
    ESP8266CommandStatus getVersion(char* buffer, int length);

    // Enter deep-sleep mode
    ESP8266CommandStatus deepSleep(unsigned long time);

    // Set baudrate to the given value
    void setBaudrate(unsigned long baudrate);

    /****************************************/
    /******        WiFi commands       ******/
    /****************************************/
    // Set mode
    ESP8266CommandStatus setMode(ESP8266WifiMode mode);

    // Get the mode
    ESP8266CommandStatus getMode(ESP8266WifiMode* mode);

    // Join the access point
    ESP8266CommandStatus joinAP(char* ssid, char* password);

    // Get the current access point
    ESP8266CommandStatus getAP(char* ssid);

    // List available APs
    //TODO
    //ESP8266CommandStatus listAP();

    // List available APs matching criteria
    //TODO
    //ESP8266CommandStatus listAP(char* ssid, byte mac[6], unsigned int channel);

    // Quit the current access point
    ESP8266CommandStatus quitAP();

    // Set the configuration of the access point
    ESP8266CommandStatus setAPConfiguration(char* ssid, char* password, uint8_t channel = 6, ESP8266Encryption encryption = ESP8266_ENCRYPTION_WPA_WPA2_PSK);

    // Get access point's configuration
    ESP8266CommandStatus getAPConfiguration(char* ssid, char* password, uint8_t& channel, ESP8266Encryption& encryption);

    // Get IP of stations connected to the access point
    ESP8266CommandStatus getConnectedStations(ESP8266Station* stations, unsigned int& count, const unsigned int max);

    // Set DHCP
    ESP8266CommandStatus setDHCP(ESP8266WifiMode mode, bool enable);

    // Set the mac address
    ESP8266CommandStatus setMAC(ESP8266WifiMode mode, byte mac[6]);

    // Get the mac address
    ESP8266CommandStatus getMAC(ESP8266WifiMode mode, byte mac[6]);

    // Set the ip address
    ESP8266CommandStatus setIP(ESP8266WifiMode mode, IPAddress& ip);

    // Get the ip address
    ESP8266CommandStatus getIP(ESP8266WifiMode mode, IPAddress& ip);

    /****************************************/
    /******       TCP/IP commands      ******/
    /****************************************/
    // Get connection status
    ESP8266CommandStatus getConnectionStatus(ESP8266ConnectionStatus& status, ESP8266Connection* connection, unsigned int& count);

    // Establish connection
    ESP8266CommandStatus connect(ESP8266Protocol protocol, IPAddress ip, unsigned int port);
    ESP8266CommandStatus connect(ESP8266Protocol protocol, const char* host, unsigned int port);
    ESP8266CommandStatus connect(unsigned int id, ESP8266Protocol protocol, IPAddress ip, unsigned int port);
    ESP8266CommandStatus connect(unsigned int id, ESP8266Protocol protocol, const char* host, unsigned int port);

    // Send data char array/string overloads
    ESP8266CommandStatus send(char *value)
    {
        return send(ESP8266_SINGLE_CLIENT, value, strlen(value));
    }
    ESP8266CommandStatus send(const char *value)
    {
        return send(ESP8266_SINGLE_CLIENT, value, strlen(value));
    }
    ESP8266CommandStatus send(unsigned id, char *value)
    {
        return send(id, value, strlen(value));
    }
    ESP8266CommandStatus send(unsigned int id, const char *value)
    {
        return send(id, value, strlen(value));
    }
    ESP8266CommandStatus send(String &value)
    {
        return send(ESP8266_SINGLE_CLIENT, value.c_str(), value.length());
    }
    ESP8266CommandStatus send(unsigned int id, const String &value)
    {
        return send(id, value.c_str(), value.length());
    }

    // Send data generic types
    template <class T>
    ESP8266CommandStatus send(const T& value)
    {
        return send(ESP8266_SINGLE_CLIENT, value);
    }
    template <class T>
    ESP8266CommandStatus send(const T value[], size_t size)
    {
        return send(ESP8266_SINGLE_CLIENT, value, size);
    }
    template <class T>
    ESP8266CommandStatus send(unsigned int id, const T value[], size_t size)
    {
        int c;

        clear();
        _serial->print(F("AT+CIPSEND="));

        if (id != ESP8266_SINGLE_CLIENT) {
            _serial->print(id);
            _serial->print(F(","));
        }

        _serial->println((sizeof(*value)*size));

        c = timedPeek(20);

        if (c == -1)
            return ESP8266_COMMAND_TIMEOUT;

        if (c != '>')
            return readStatus(_timeout);

        _serial->write((byte*)value, (sizeof(*value)*size));

        return readStatus(_timeout);
    }

    template <class T>
    ESP8266CommandStatus send(unsigned int id, const T& value)
    {
        int c;

        clear();
        _serial->print(F("AT+CIPSEND="));

        if (id != ESP8266_SINGLE_CLIENT) {
            _serial->print(id);
            _serial->print(F(","));
        }

        _serial->println(sizeof(value));

        c = timedPeek(20);

        if (c == -1)
            return ESP8266_COMMAND_TIMEOUT;

        if (c != '>')
            return readStatus(_timeout);

        _serial->write((byte*)&value, sizeof(value));

        return readStatus(_timeout);
    }

    // Close connection
    ESP8266CommandStatus close(unsigned int id);

    // Set multiple connections
    ESP8266CommandStatus setMultipleConnections(bool enable);

    // Get multiple connections
    ESP8266CommandStatus getMultipleConnections(bool& enable);

    // Create server
    ESP8266CommandStatus createServer();
    ESP8266CommandStatus createServer(unsigned int port);

    // Delete server
    ESP8266CommandStatus deleteServer();

    // Set server timeout
    ESP8266CommandStatus setServerTimeout(unsigned int timeout);

    // Set server timeout
    ESP8266CommandStatus getServerTimeout(unsigned int& timeout);

    // Available
    int available();

    // Connection id
    unsigned int getId();

    // Read
    int read();
    int read(char* buffer, size_t size);
    int read(uint8_t* buffer, size_t size);

    // Peek
    int peek();

    // Flush
    void flush();

    // Write
    size_t write(uint8_t b);

protected:
    Stream* _serial;

    int _available;

    unsigned int _id;

    // Clear the incomming data
    void clear();

    // Initialize the module and wait for it to be ready
    bool initialize();

    // Set the echo
    ESP8266CommandStatus setEcho(bool enable);

    // Set transmission mode
    ESP8266CommandStatus setUnvarnishedMode(bool enable);

    // Configure server
    ESP8266CommandStatus configureServer(int mode);
    ESP8266CommandStatus configureServer(int mode, unsigned int port);

    // Connect
    void pre_connect(unsigned int id, ESP8266Protocol protocol);
    ESP8266CommandStatus post_connect(unsigned int port);

    // Read the underlying serial, waiting for timeout milliseconds. Returns the read char or -1 if timeout
    int timedRead(unsigned int timeout);

    // Peek the underlying serial, waiting for timeout milliseconds. Returns the read char or -1 if timeout
    int timedPeek(unsigned int timeout);

    // Parse an int from serial
    int parseInt(unsigned int timeout);

    // Parse an hex byte (2 char) from serial
    byte parseHex(unsigned int timeout);

    // Parse a protocol from serial
    ESP8266Protocol parseProtocol(unsigned int timeout);

    // Parse an IP address from serial
    IPAddress parseIPAddress(unsigned int timeout);

    // Parse a MAC address from serial
    void parseMACAddress(byte mac[6], unsigned int timeout);

    // Read status
    ESP8266CommandStatus readStatus(unsigned int timeout);

    // Read bytes until target is matched or timeout occurs. Returns true if matched, false otherwise
    bool find(const __FlashStringHelper* target);
    bool find(const __FlashStringHelper* target, unsigned int timeout);

    // Read bytes into buffer until length is reached or target is matched or timeout occurs, whichever first. Returns the number of bytes placed in the buffer
    size_t readUntil(char* buffer, size_t length, const __FlashStringHelper* target, unsigned int timeout);

    // Read until a string is found. If strict is true, stop on first non-matching character
    int findStrings(const char** strings, unsigned int count, bool strict, unsigned int timeout);
};

#endif
