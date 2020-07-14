#include "TwiFi.h"

#include <EEPROM.h>

#ifdef ESP8266
    #include <ESP8266WiFi.h>
#elif ESP32
    #include <WiFi.h>
#endif

/* Default Settings */
#define DEFAULT_WIFI 0

/* WiFi Attempt Duration */
#define WIFI_ATTEMPT_TIME 500

/* EEPROM Addresses. */
#define EEPROM_SAVED 0
#define EEPROM_WIFI  1



/* Functions */
static String splitData(String data, char separator, int index);
static IPAddress getIPFromString(char* addressString, int id);
static void setupWiFiSettings();
static bool attemptWiFiConnection(char* ssid, char* pass, char* conf, bool isOpen);



/* Variables */
int prevWiFi;
int currWiFi;

TwiFiEntry* vEntries;
int         vEntryCount;
char*       vHostName;
bool        vDebug;
int         vTimeout;

/* Stored Functions */
void (*fConnectAttempt)(int attempt);
void (*fConnectSuccess)(int idEntry);
void (*fConnectFail)(int idEntry);

/* Functions */
static String splitData(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for(int i = 0; i <= maxIndex && found <= index; i++) {
        if(data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
} 

static IPAddress getIPFromString(char* addressString, int id) {
    String address = splitData(String(addressString), '|', id);
    return IPAddress(splitData(address, '.', 0).toInt(), splitData(address, '.', 1).toInt(), splitData(address, '.', 2).toInt(), splitData(address, '.', 3).toInt());
}

static void setupWiFiSettings() {    
    EEPROM.begin(512);
    int saved = EEPROM.read(EEPROM_SAVED);
    EEPROM.end();

    if (saved == 1) {
        if (vDebug)
            Serial.println("\nWiFi settings already exist. Loading...");
        EEPROM.begin(512);
        prevWiFi = currWiFi = (int) EEPROM.read(EEPROM_WIFI);
        EEPROM.end();
        if (vDebug)
            Serial.println("Loaded WiFi settings from EEPROM.");
    }
    else {   
        prevWiFi = currWiFi = DEFAULT_WIFI;
        if (vDebug)
            Serial.println("\nCreated new WiFi settings. Saving...");
        EEPROM.begin(512);
        EEPROM.write(EEPROM_SAVED, 1);
        EEPROM.write(EEPROM_WIFI,  prevWiFi);  
        EEPROM.end();
        if (vDebug)
            Serial.println("Saved WiFi settings to EEPROM.");       
    }
}

static bool attemptWiFiConnection(char* ssid, char* pass, char* conf, bool isOpen) {
    if (vDebug)
        Serial.print("\nConnecting to: '" + (String) ssid + "'");
    if (conf != "DHCP") {
        String address;
        
        IPAddress ipLocal =   getIPFromString(conf, 0);
        IPAddress ipGateway = getIPFromString(conf, 1);
        IPAddress ipSubnet =  getIPFromString(conf, 2);
        IPAddress ipDNS1 =    getIPFromString(conf, 3);
        IPAddress ipDNS2 =    getIPFromString(conf, 4);

        WiFi.config(ipLocal, ipGateway, ipSubnet, ipDNS1, ipDNS2);
    }
    #ifdef ESP32
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    #endif
    
    if (isOpen)
        WiFi.begin(ssid, NULL);
    else    
        WiFi.begin(ssid, pass);

    #ifdef ESP32
        WiFi.setHostname(vHostName);
    #endif

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < (vTimeout * 1000) / WIFI_ATTEMPT_TIME) {        
        delay(WIFI_ATTEMPT_TIME);
        if (vDebug)
            Serial.print(".");
        attempt++;
            
        fConnectAttempt(attempt);
    }
    if (WiFi.status() == WL_CONNECTED) {
        if (vDebug) {
            Serial.print("\nConnected to WiFi: '" + (String) ssid + "' With IP: ");
            Serial.println(WiFi.localIP());
        }
        return true;
    }
    else {
        if (vDebug)
            Serial.println("\nWiFi connection timed out.");
        return false;
    }
}

/* Public Functions */

void twifiInit(
    TwiFiEntry* entries,
    int entryCount,
    char* hostName,
    int timeout,
    void (*connectAttempt)(int attempt),
    void (*connectSuccess)(int idEntry),
    void (*connectFail)(int idEntry),
    bool debug
    ) {

    vEntries = entries;
    vEntryCount = entryCount;
    vHostName = hostName;
    vTimeout = timeout;
    fConnectAttempt = connectAttempt;
    fConnectSuccess = connectSuccess;
    fConnectFail = connectFail;
    vDebug = debug;
}

bool twifiConnect(bool repeatAttempts) {
    setupWiFiSettings();
    
    WiFi.mode(WIFI_STA);
    #ifdef ESP8266
        WiFi.hostname(vHostName);
    #endif

    bool result;
    int counter = 0;

    do {
        result = attemptWiFiConnection(
            vEntries[currWiFi].ssid, 
            vEntries[currWiFi].pass,
            vEntries[currWiFi].conf,
            vEntries[currWiFi].open
            );
            
        if (!result) {
            if (vDebug)
                Serial.println("\nWiFi " + String(currWiFi) + " failed..." );

            fConnectFail(currWiFi);

            if (counter >= vEntryCount && !repeatAttempts)
                return false;

            currWiFi++;
            if (currWiFi >= vEntryCount)
                currWiFi = 0;
            counter++;
        }
    } while (!result);   
    if (vDebug)     
        Serial.println("\nWiFi " + String(currWiFi) + " connected!" );

    fConnectSuccess(currWiFi);
    
    if (currWiFi != prevWiFi) {
        if (vDebug)
            Serial.println("Saved settings to EEPROM.");     
        prevWiFi = currWiFi;
        EEPROM.begin(512);
        EEPROM.write(EEPROM_SAVED, 1);
        EEPROM.write(EEPROM_WIFI,  prevWiFi);  
        EEPROM.end();
    }

    return true;
}

bool twifiIsConnected() {
    return WiFi.status() == WL_CONNECTED;
}
