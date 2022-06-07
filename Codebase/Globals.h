#pragma once

/****************************************************************************************************************************
 *													  MACROS															   *
 ****************************************************************************************************************************
 */
// Many of these header files are from ESP32 libraries that must be downloaded 
// processor predirectives and MACROS 
#define use_soft_serial 0                 // predirective to control whether using software or hardware serial port, if set to 1, then switch on M6E needs to be set to SW
#define DEBUG 0                           // 0 : no messages  1 : request sending and receiving
#define EPC_COUNT 20                      // how many tags are expected in the area, can reasonably go up to around 100 
#define NANOREGION REGION_NORTHAMERICA    //which region the reader operates in, check https://github.com/sparkfun/Simultaneous_RFID_Tag_Reader for other regions
#define ALWAYS 1

// Reader header files 
#include "SparkFun_UHF_RFID_Reader.h"    //Library for controlling the M6E Nano module
#include <string>

#if use_soft_serial
#include <SoftwareSerial.h>             //Used for transmitting to the device
SoftwareSerial softSerial(2, 3);        //RX, TX
#else 
#define NanoSerial Serial               // define the serial port to use (E.g. softSerial, Serial1 etc)
#endif

// Database header files 
#include <WiFi.h>
#include <HTTPClient.h>

// ePaper header files 
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <SPI.h>

/****************************************************************************************************************************
 *														   Function and Global Variable Declarations                        *
 ****************************************************************************************************************************
 */

void setup(void);
void loop(void);
void init_array();
bool check_array(uint8_t* msg);
void add_array(uint8_t* msg);
int count_entries(void);
void clear_arrays(void);
bool setupNano(long baudRate);
void connect_to_wifi(void);
void database(void);



uint8_t EPC_recv[EPC_COUNT][12];          //stores unique tag values in a session 
const unsigned long time_pd_wait;        // how long we should wait in between scan sessions (ms) 
const unsigned long time_pd_scan;        // scan duration (ms) 
unsigned long prev_time;                   // timer wait update
unsigned long prev_scan_time;             // timer scan update 
bool onetime;                            // one time flag 
bool firstTime;                          // one time flag 
unsigned long scan_time;				// used to store millis() 
const char WIFI_SSID[];					 // wifi network name 
const char WIFI_PASSWORD[];				 // wifi password 
const char * HOST_NAME ;				// PC's IP address, use port 80 
const char * PATH_NAME;					// php file placed in C:\xampp\htdocs
const char * queryString;               // query to send to database
const char * final_epc[EPC_COUNT];      // string epc values 