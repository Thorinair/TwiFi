#ifdef ESP8266
    #include <ESP8266WiFi.h>
#elif ESP32
    #include <WiFi.h>
#endif
#include <TwiFi.h>

/* Prepare WiFis */
#define WIFI_COUNT 2
TwiFiEntry wifis[WIFI_COUNT] = {
    {"ExampleWiFiName", "password", "DHCP", false},
    {"AnotherWiFi",     "password", "192.168.0.201|192.168.0.1|255.255.255.0|8.8.8.8|8.8.4.4", false}
};


void connectAttempt(int attempt) {
    /* Blink LED on and off while trying to connect. */
    if (attempt % 2)
        digitalWrite(13, LOW);
    else
        digitalWrite(13, HIGH);
}

void connectSuccess(int idEntry) {
    Serial.print("Success fired! SSID: " + String(wifis[idEntry].ssid));

    /* Quickly flash 4 times on success. */
    for (int i = 0; i < 4; i++) {
        delay(20);
        digitalWrite(13, LOW);
        delay(20);
        digitalWrite(13, HIGH);
    }

}

void connectFail(int idEntry) {
    Serial.print("Fail fired! SSID: " + String(wifis[idEntry].ssid));

    /* Slowly flash 2 times on fail. */
    for (int i = 0; i < 2; i++) {
        delay(200);
        digitalWrite(13, LOW);
        delay(200);
        digitalWrite(13, HIGH);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(13, OUTPUT);

    /* Initialize the TwiFi library. */
    twifiInit(
        wifis,           // WiFi array
        WIFI_COUNT,      // Number of WiFis
        "Test_Device",   // Host name
        10,              // Seconds to keep trying one WiFi
        &connectAttempt, // Function called on individual attempts
        &connectSuccess, // Function called on successful connection
        &connectFail,    // Function called on failed connection
        true             // Perform debugging
        );

    /* Connect to first available WiFi, repeat attempts. */
    twifiConnect(true);
}

void loop() {

    /* Check if WiFi is still connected, and if not, reconnect. */
    if (!twifiIsConnected())
        twifiConnect(true);
}
