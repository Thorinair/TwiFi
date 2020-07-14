/*
    Library: TwiFi
    Programmed by: Thorinair
    Description: Provides simple WiFi management features.
    Usage:  
        First add this library to the "libraries" folder in your Arduino workspace.
        Then, include the library in your program by calling #include <TwiFi.h>.
        Please refer to this same file as reference for different functions and values.
    Requirements: 
        This library requires you to have an ESP8266 or ESP32 based board.
*/

#ifndef TWIFI_H
#define TWIFI_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/*
 * Structure holding data for a single WiFi network.
 */
typedef struct TwiFiEntry_s {
    char* ssid;
    char* pass;
    char* conf;
    bool  open;
} TwiFiEntry;

/*
 * Initializes the module.
 * Pass the pointers to various structures and functions.
 * @param  entries         Pointer to an array of structures which define various WiFi networks.
 * @param  entryCount      Number of WiFi networks to use.
 * @param  hostName        Name of the device which will appear on the network once connected.
 * @param  timeout         Number of seconds to keep attempting connection to one network.
 * @param  connectAttempt  Pointer to a function which is called on while attempting a single WiFi network.
 * @param  connectSuccess  Pointer to a function which is called once a connection was successful.
 * @param  connectFail     Pointer to a function which is called once a connection fails.
 * @param  debug           Whether debugging should be printed.
 */
void twifiInit(
    TwiFiEntry* entries,
    int entryCount,
    char* hostName,
    int timeout,
    void (*connectAttempt)(int attempt),
    void (*connectSuccess)(int idEntry),
    void (*connectFail)(int idEntry),
    bool debug
    );

/*
 * Begins a connection to a WiFi network.
 * @param  repeatAttempts  Whether the networks should be looped through continuously.
 * @return  bool  WiFi connection result, true if successful.
 */
bool twifiConnect(bool repeatAttempts);

/*
 * Checks if WiFi is still connected
 * @return  bool  True if WiFi is connected.
 */
bool twifiIsConnected();

#endif
