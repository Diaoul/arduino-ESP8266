#include "ESP8266.h"

bool ESP8266::begin(Stream& serial)
{
    _serial = &serial;

    return initialize();
}

void ESP8266::setTimeout(unsigned long timeout)
{
    _timeout = timeout;
}

/****************************************/
/******       Basic commands       ******/
/****************************************/
ESP8266CommandStatus ESP8266::test()
{
    clear();
    _serial->println(F("AT"));

    return readStatus(20);
}

bool ESP8266::restart()
{
    clear();
    _serial->println(F("AT+RST"));

    if (readStatus(20) != ESP8266_COMMAND_OK)
        return false;

    return initialize();
}

ESP8266CommandStatus ESP8266::getVersion(char* buffer, int length)
{
    clear();
    _serial->println(F("AT+GMR"));

    if (!readUntil(buffer, length, F("\r"), 20))
        return ESP8266_COMMAND_TIMEOUT;

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::deepSleep(unsigned long time)
{
    clear();
    _serial->print(F("AT+GSLP="));
    _serial->println(time);

    return readStatus(20);
}

void ESP8266::setBaudrate(unsigned long baudrate)
{
    clear();
    _serial->print(F("AT+IPR="));
    _serial->println(baudrate);
}

/****************************************/
/******        WiFi commands       ******/
/****************************************/
ESP8266CommandStatus ESP8266::setMode(ESP8266WifiMode mode)
{
    clear();
    _serial->print(F("AT+CWMODE="));
    _serial->println(mode);

    return readStatus(200);
}

ESP8266CommandStatus ESP8266::getMode(ESP8266WifiMode* mode)
{
    int c;

    clear();
    _serial->println(F("AT+CWMODE?"));

    if (!find(F("+CWMODE:"), 20))
        return ESP8266_COMMAND_TIMEOUT;

    c = timedRead(20);

    if (c == -1)
        return ESP8266_COMMAND_TIMEOUT;

    c -= '0';

    if (c < 1 || c > 3)
        return ESP8266_COMMAND_INVALID;

    *mode = (ESP8266WifiMode)c;

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::joinAP(char* ssid, char* password)
{
    clear();
    _serial->print(F("AT+CWJAP=\""));
    _serial->print(ssid);
    _serial->print(F("\",\""));
    _serial->print(password);
    _serial->println(F("\""));

    return readStatus(16000);
}

ESP8266CommandStatus ESP8266::getAP(char* ssid)
{
    clear();
    _serial->println(F("AT+CWJAP?"));

    if (!find(F("+CWJAP:\""), 20))
        return ESP8266_COMMAND_TIMEOUT;

    if (!readUntil(ssid, 32, F("\""), 20))
        return ESP8266_COMMAND_TIMEOUT;

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::quitAP()
{
    clear();
    _serial->println(F("AT+CWQAP"));

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::setAPConfiguration(char* ssid, char* password, uint8_t channel, ESP8266Encryption encryption)
{
    clear();
    _serial->print(F("AT+CWSAP=\""));
    _serial->print(ssid);
    _serial->print(F("\",\""));
    _serial->print(password);
    _serial->print(F("\","));
    _serial->print(channel);
    _serial->print(F(","));
    _serial->print(encryption);
    _serial->println();

    return readStatus(1000);
}

ESP8266CommandStatus ESP8266::getAPConfiguration(char* ssid, char* password, uint8_t& channel, ESP8266Encryption& encryption)
{
    clear();
    _serial->println(F("AT+CWSAP?"));

    if (!find(F("+CWSAP:\""), 2000))
        return ESP8266_COMMAND_TIMEOUT;

    if (!readUntil(ssid, 32, F("\""), 2000))
        return ESP8266_COMMAND_TIMEOUT;

    if (!find(F(",\""), 2000))
        return ESP8266_COMMAND_TIMEOUT;

    if (!readUntil(password, 63, F("\""), 2000))
        return ESP8266_COMMAND_TIMEOUT;

    if (!find(F(","), 2000))
        return ESP8266_COMMAND_TIMEOUT;

    channel = (uint8_t)parseInt(2000);

    if (!find(F(","), 2000))
        return ESP8266_COMMAND_TIMEOUT;

    encryption = (ESP8266Encryption)parseInt(2000);

    return readStatus(2000);
}

ESP8266CommandStatus ESP8266::getConnectedStations(ESP8266Station* stations, unsigned int& count, const unsigned int max)
{
    int c;

    clear();
    _serial->println(F("AT+CWLIF"));

    count = 0;

    while (count < max) {
        if (count > 0) {
            if (!find(F("\r\n"), 20))
                return ESP8266_COMMAND_TIMEOUT;
        }

        c = timedPeek(20);

        if (c < '0' || c > '9')
            return readStatus(20);

        stations[count].ip = parseIPAddress(20);

        if (!find(F(","), 20))
            return ESP8266_COMMAND_TIMEOUT;

        parseMACAddress(stations[count].mac, 20);

        count++;
    }
}

ESP8266CommandStatus ESP8266::setDHCP(ESP8266WifiMode mode, bool enable)
{
    clear();
    _serial->print(F("AT+CWDHCP="));

    switch (mode) {
    case ESP8266_WIFI_STATION:
        _serial->print(1);
        break;

    case ESP8266_WIFI_ACCESSPOINT:
        _serial->print(0);
        break;

    case ESP8266_WIFI_BOTH:
        _serial->print(2);
        break;

    default:
        break;
    }

    _serial->print(F(","));
    _serial->println((int)enable);

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::setMAC(ESP8266WifiMode mode, byte mac[6])
{
    clear();
    _serial->print(F("AT+CIP"));

    switch (mode) {
    case ESP8266_WIFI_STATION:
        _serial->print(F("STA"));
        break;

    case ESP8266_WIFI_ACCESSPOINT:
        _serial->print(F("AP"));

    default:
        break;
    }

    _serial->print(F("MAC=\""));

    for (uint8_t i = 0; i < 6; i++) {
        _serial->print(mac[i], HEX);

        if (i < 5)
            _serial->print(F(":"));
    }

    _serial->println(F("\""));

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::getMAC(ESP8266WifiMode mode, byte mac[6])
{
    clear();

    switch (mode) {
    case ESP8266_WIFI_STATION:
        _serial->println(F("AT+CIPSTAMAC?"));

        if (!find(F("+CIPSTAMAC:\""), 20))
            return ESP8266_COMMAND_TIMEOUT;

        break;

    case ESP8266_WIFI_ACCESSPOINT:
        _serial->println(F("AT+CIPAPMAC?"));

        if (!find(F("+CIPAPMAC:\""), 20))
            return ESP8266_COMMAND_TIMEOUT;

        break;

    default:
        break;
    }

    parseMACAddress(mac, 20);

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::setIP(ESP8266WifiMode mode, IPAddress& ip)
{
    clear();
    _serial->print(F("AT+CIP"));

    switch (mode) {
    case ESP8266_WIFI_STATION:
        _serial->print(F("STA"));
        break;

    case ESP8266_WIFI_ACCESSPOINT:
        _serial->print(F("AP"));

    default:
        break;
    }

    _serial->print(F("=\""));
    _serial->print(ip);
    _serial->println(F("\""));

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::getIP(ESP8266WifiMode mode, IPAddress& ip)
{
    clear();

    switch (mode) {
    case ESP8266_WIFI_STATION:
        _serial->println(F("AT+CIPSTA?"));

        if (!find(F("+CIPSTA:\""), 20))
            return ESP8266_COMMAND_TIMEOUT;

        break;

    case ESP8266_WIFI_ACCESSPOINT:
        _serial->println(F("AT+CIPAP?"));

        if (!find(F("+CIPAP:\""), 20))
            return ESP8266_COMMAND_TIMEOUT;

        break;

    default:
        break;
    }

    ip = parseIPAddress(20);

    return readStatus(20);
}

/****************************************/
/******       TCP/IP commands      ******/
/****************************************/
ESP8266CommandStatus ESP8266::getConnectionStatus(ESP8266ConnectionStatus& status, ESP8266Connection* connection, unsigned int& count)
{
    int c;

    clear();
    _serial->println(F("AT+CIPSTATUS"));

    if (!find(F("STATUS:"), 20))
        return ESP8266_COMMAND_TIMEOUT;

    status = (ESP8266ConnectionStatus)parseInt(20);

    count = 0;

    while (true) {
        if (!find(F("\r\n"), 20))
            return ESP8266_COMMAND_TIMEOUT;

        c = timedPeek(20);

        if (c == -1)
            return ESP8266_COMMAND_TIMEOUT;

        if (c != '+')
            return readStatus(20);

        if (!find(F("+CIPSTATUS:"), 20))
            return ESP8266_COMMAND_TIMEOUT;

        //TODO: check in case of single connection mode
        connection[count].id = parseInt(20);

        if (!find(F(",\""), 20))
            return ESP8266_COMMAND_TIMEOUT;

        connection[count].protocol = parseProtocol(20);

        if (!find(F("\","), 20))
            return ESP8266_COMMAND_TIMEOUT;

        connection[count].ip = parseIPAddress(20);

        if (!find(F(","), 20))
            return ESP8266_COMMAND_TIMEOUT;

        connection[count].port = parseInt(20);

        if (!find(F(","), 20))
            return ESP8266_COMMAND_TIMEOUT;

        connection[count].role = (ESP8266Role)parseInt(20);

        count++;
    }
}

ESP8266CommandStatus ESP8266::connect(ESP8266Protocol protocol, IPAddress ip, unsigned int port)
{
    return connect(ESP8266_SINGLE_CLIENT, protocol, ip, port);
}

ESP8266CommandStatus ESP8266::connect(ESP8266Protocol protocol, const char* host, unsigned int port)
{
    return connect(ESP8266_SINGLE_CLIENT, protocol, host, port);
}

ESP8266CommandStatus ESP8266::connect(unsigned int id, ESP8266Protocol protocol, IPAddress ip, unsigned int port)
{
    pre_connect(id, protocol);
    _serial->print(ip);
    return post_connect(port);
}

ESP8266CommandStatus ESP8266::connect(unsigned int id, ESP8266Protocol protocol, const char* host, unsigned int port)
{
    pre_connect(id, protocol);
    _serial->print(host);
    return post_connect(port);
}

ESP8266CommandStatus ESP8266::send(const char* data)
{
    return send(ESP8266_SINGLE_CLIENT, (uint8_t*)data, strlen(data));
}

ESP8266CommandStatus ESP8266::send(const uint8_t* buffer, size_t size)
{
    return send(ESP8266_SINGLE_CLIENT, buffer, size);
}

ESP8266CommandStatus ESP8266::send(unsigned int id, const char* data)
{
    return send(id, (uint8_t*)data, strlen(data));
}

ESP8266CommandStatus ESP8266::send(unsigned int id, const uint8_t* buffer, size_t size)
{
    int c;

    clear();
    _serial->print(F("AT+CIPSEND="));

    if (id != ESP8266_SINGLE_CLIENT) {
        _serial->print(id);
        _serial->print(F(","));
    }

    _serial->println(size);

    c = timedPeek(20);

    if (c == -1)
        return ESP8266_COMMAND_TIMEOUT;

    if (c != '>')
        return readStatus(_timeout);

    _serial->write(buffer, size);

    return readStatus(_timeout);
}

ESP8266CommandStatus ESP8266::close(unsigned int id)
{
    clear();
    _serial->print(F("AT+CIPCLOSE"));
    _serial->print(F("="));
    _serial->println(id);

    return readStatus(500);
}

ESP8266CommandStatus ESP8266::setMultipleConnections(bool enable)
{
    clear();
    _serial->print(F("AT+CIPMUX="));
    _serial->println((int)enable);

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::getMultipleConnections(bool& enable)
{
    int c;

    clear();
    _serial->println(F("AT+CIPMUX?"));

    if (!find(F("+CIPMUX:"), 20))
        return ESP8266_COMMAND_TIMEOUT;

    c = timedRead(20);

    if (c == -1)
        return ESP8266_COMMAND_TIMEOUT;

    c -= '0';

    enable = (bool)c;

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::createServer()
{
    return configureServer(1, 0);
}

ESP8266CommandStatus ESP8266::createServer(unsigned int port)
{
    return configureServer(1, port);
}

ESP8266CommandStatus ESP8266::deleteServer()
{
    return configureServer(0, 0);
}

ESP8266CommandStatus ESP8266::setServerTimeout(unsigned int timeout)
{
    clear();
    _serial->print(F("AT+CIPSTO="));
    _serial->println(timeout);

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::getServerTimeout(unsigned int& timeout)
{
    clear();
    _serial->println(F("AT+CIPSTO?"));

    if (!find(F("+CIPSTO:"), 20))
        return ESP8266_COMMAND_TIMEOUT;

    timeout = parseInt(20);

    return readStatus(20);
}

int ESP8266::available()
{
    int tmp;
    int c;

    // check if already receiving
    if (_available > 0)
        return _available;

    // incoming sequence is +IPD,id,length:data or +IPD,length:data
    // non-blocking search of '+'
    do {
        c = _serial->read();

        if (c == -1)
            return _available = 0;
    } while (c != '+');

    // find the rest slower
    if (!find(F("IPD,"), 20))
        return _available = 0;

    // read first int, id or length
    tmp = parseInt(20);

    // read next char, ',' or ':'
    c = timedRead(20);

    // check for timeouts and store _available and _id
    if (c == -1)
        _available = -1;
    else if (c == ',') {
        _id = tmp;
        _available = parseInt(20);

        // skip ':'
        if (timedRead(20) == -1)
            _available = -1;
    } else if (c ==  ':') {
        _id = ESP8266_SINGLE_CLIENT;
        _available = tmp;
    } else
        _available = -2;

    return _available;
}

unsigned int ESP8266::getId()
{
    return _id;
}

int ESP8266::read()
{
    if (available() > 0) {
        _available--;

        while (!_serial->available()) {}

        return _serial->read();
    }

    return -1;
}

int ESP8266::read(char* buffer, size_t size)
{
    return read((uint8_t*)buffer, size);
}

int ESP8266::read(uint8_t* buffer, size_t size)
{
    if (available() > 0) {
        int readable = min(size, _available);
        size_t count = 0;

        while (count < readable) {
            int c = timedRead(_timeout);

            if (c == -1)
                break;

            *buffer++ = (uint8_t)c;
            count++;
        }

        _available -= count;
        return count;
    }

    return -1;
}

int ESP8266::peek()
{
    if (available() > 0) {

        while (!_serial->available()) {}

        return _serial->peek();
    }

    return -1;
}

void ESP8266::flush()
{
    _serial->flush();
}

size_t ESP8266::write(uint8_t b)
{
    if (send(_id, &b, 1) != ESP8266_COMMAND_OK)
        return 0;

    return 1;
}

/****************************************/
/******           Private          ******/
/****************************************/
void ESP8266::clear()
{
    while (_serial->read() != -1) {}
}

bool ESP8266::initialize()
{
    unsigned long startMillis = millis();

    while (millis() - startMillis < 3000) {
        if (test() == ESP8266_COMMAND_OK && setEcho(false) == ESP8266_COMMAND_OK && setUnvarnishedMode(false) == ESP8266_COMMAND_OK)
            return true;

        delay(100);
    }

    return false;
}

ESP8266CommandStatus ESP8266::setEcho(bool enable)
{
    clear();
    _serial->print(F("ATE"));
    _serial->println((int)enable);

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::setUnvarnishedMode(bool enable)
{
    clear();
    _serial->print(F("AT+CIPMODE="));
    _serial->println((int)enable);

    return readStatus(20);
}

ESP8266CommandStatus ESP8266::configureServer(int mode, unsigned int port)
{
    clear();
    _serial->print(F("AT+CIPSERVER="));
    _serial->print(mode);

    if (port == 0)
        _serial->println();
    else
        _serial->print(F(","));

    _serial->println(port);

    return readStatus(20);
}

int ESP8266::timedRead(unsigned int timeout)
{
    unsigned long startMillis = millis();
    int c;

    do {
        c = _serial->read();

        // return the byte if valid
        if (c >= 0) {
#ifdef ESP8266_DEBUG

            if (millis() - startMillis > 20) {
                Serial.print(F("==> Read: "));
                Serial.print(millis() - startMillis);
                Serial.println(F("ms"));
            }

#endif
            return c;
        }
    } while(millis() - startMillis < timeout);

    return -1;
}

void ESP8266::pre_connect(unsigned int id, ESP8266Protocol protocol)
{
    int c;

    clear();
    _serial->print(F("AT+CIPSTART="));

    if (id != ESP8266_SINGLE_CLIENT) {
        _serial->print(id);
        _serial->print(F(","));
    }

    _serial->print(F("\""));

    switch (protocol) {
    case ESP8266_PROTOCOL_TCP:
        _serial->print(F("TCP"));
        break;

    case ESP8266_PROTOCOL_UDP:
        _serial->print(F("UDP"));
        break;
    }

    _serial->print(F("\",\""));
}

ESP8266CommandStatus ESP8266::post_connect(unsigned int port)
{
    _serial->print(F("\","));
    _serial->println(port);

    return readStatus(10000);
}

int ESP8266::timedPeek(unsigned int timeout)
{
    unsigned long startMillis = millis();
    int c;

    do {
        c = _serial->peek();

        // return the byte if valid
        if (c >= 0) {
#ifdef ESP8266_DEBUG

            if (millis() - startMillis > 20) {
                Serial.print(F("==> Peek: "));
                Serial.print(millis() - startMillis);
                Serial.println(F("ms"));
            }

#endif
            return c;
        }
    } while(millis() - startMillis < timeout);

    // timeout
    return -1;
}

int ESP8266::parseInt(unsigned int timeout)
{
    _serial->setTimeout(timeout);

    return _serial->parseInt();
}

byte ESP8266::parseHex(unsigned int timeout)
{
    int c;
    byte b = 0;

    for (uint8_t i = 0; i < 2; i++) {
        c = timedRead(timeout);

        if (c == -1)
            return 0;

        if (c >= '0' && c <= '9')
            b += c - '0';

        if (c >= 'a' && c <= 'z')
            b += 10 + c - 'a';

        if (c >= 'A' && c <= 'Z')
            b += 10 + c - 'A';

        if (i == 0)
            b = b << 4;
    }

    return b;
}

ESP8266Protocol ESP8266::parseProtocol(unsigned int timeout)
{
    const char* protocols[] = {"TCP", "UDP"};

    return (ESP8266Protocol)findStrings(protocols, 2, true, timeout);
}

IPAddress ESP8266::parseIPAddress(unsigned int timeout)
{
    IPAddress ip;

    for (uint8_t i = 0; i < 4; i++)
        ip[i] = (uint8_t)parseInt(timeout);

    return ip;
}

void ESP8266::parseMACAddress(byte* mac, unsigned int timeout)
{
    for (uint8_t i = 0; i < 6; i++) {
        mac[i] = (uint8_t)parseHex(timeout);

        if (i < 5)
            timedRead(timeout);
    }
}

ESP8266CommandStatus ESP8266::readStatus(unsigned int timeout)
{
    const char* statuses[] = {"OK\r\n", "no change\r\n", "ERROR\r\n", "link is not\r\n", "too long\r\n", "FAIL\r\n", "ALREAY CONNECT\r\n"};

    return (ESP8266CommandStatus)findStrings(statuses, 7, false, timeout);
}

bool ESP8266::find(const __FlashStringHelper* target)
{
    return find(target, 0);
}

bool ESP8266::find(const __FlashStringHelper* target, unsigned int timeout)
{
    PGM_P p = reinterpret_cast<PGM_P>(target);
    size_t index = 0;
    int c;

    // read until timeout
    while ((c = timedRead(timeout)) >= 0) {

#ifdef ESP8266_DEBUG
        Serial.write(c);
#endif

        // match magic
        if(c == pgm_read_byte(p + index)) {
            if (pgm_read_byte(p + ++index) == 0)
                return true;
        } else
            index = 0;
    }

    return false;
}

size_t ESP8266::readUntil(char* buffer, size_t length, const __FlashStringHelper* target, unsigned int timeout)
{
    PGM_P p;
    size_t index = 0;
    size_t bufferIndex = 0;
    int c;

    if (target != NULL)  // target is specified
        p = reinterpret_cast<PGM_P>(target);

    // read until timeout
    while ((c = timedRead(timeout)) >= 0) {
        // skip first carriage return and new line characters
        if (bufferIndex == 0 && (c == 13 || c == 10))
            continue;

        if (target != NULL) {  // target is specified
            // match magic
            if(c == pgm_read_byte(p + index)) {
                if (pgm_read_byte(p + ++index) == 0)
                    return bufferIndex;
            } else
                index = 0;
        }

        // store in buffer if char does not match any of the target
        if (index == 0) {
            bufferIndex++;
            *buffer++ = (char)c;

            // return if length is reached
            if (bufferIndex >= length)
                return bufferIndex;
        }
    }

    return bufferIndex;
}

int ESP8266::findStrings(const char** strings, unsigned int count, bool strict, unsigned int timeout)
{
    unsigned int indexes[10] = {};
    bool match;
    int c;

    // read until match or timeout
    while ((c = timedRead(timeout)) >= 0) {
        match = false;

#ifdef ESP8266_DEBUG
        Serial.write(c);
#endif

        // loop over possible strings
        for (int i = 0; i < count; i++) {
            // match magic
            if(c == strings[i][indexes[i]]) {
                match = true;

                if (strings[i][++indexes[i]] == 0)
                    return i;
            } else
                indexes[i] = 0;
        }

        // strict mode
        if (strict && !match)
            return -2;
    }

    return -1;
}
