# TwiFi
TwiFi is a simple library which allows you to easily manage the WiFi connections on your ESP8266 or ESP32. This is a new version of my old [TwiFi Module (RETIRED)](https://github.com/Thorinair/TwiFi-Module-RETIRED), as using the old one has proven to be horrible in the long run.

NOTE: TwiFi saves WiFI status on the EEPROM, specifically on memory positions 0 and 1. Please don't use these in your own code.

# Usage
Include the library in your sketch. Call the `twifiInit` function and pass all the relevant parameters in your `setup()`. Please refer to the TwiFi.h file for details on which parameters should be used. To begin connecting, call the `twifiConnect` function and pass a bool parameter to tell it whether it should continuously loop through all WiFis.

For a real example, please check the examples folder in the repository.