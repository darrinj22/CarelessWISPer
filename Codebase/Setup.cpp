#include "Globals.h";
/*
 * ******************************************************************************************************************************
 *  Function: Setup
 *  Author: DJ Hansen
 *  Date: 11/29/2021
 *  About:
 *  Runs once before going into the Always loop.
 *  Sets up WiFi, Server, and Serial Connections
 * ******************************************************************************************************************************
 */

 /****************************************************************************************************************************
  *                     Global Variables and Objects needed for Reader and Database Functionality                            *
  ****************************************************************************************************************************
  */

bool add_image_done = false;
const char* host = "esp32";
const char* ssid = "ncsu";
const char* password = "";

void setup()
{

    Serial.begin(115200);

    // Connect to WiFi network

    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Begin Server Connection Setup for EPaper Display 

    // use mdns for host name resolution
    if (!MDNS.begin(host)) { //http://esp32.local
        Serial.println("Error setting up MDNS responder!");
        while (1) {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");
    // return index page which is stored in serverIndex 
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", loginIndex);
        });
    server.on("/serverIndex", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", serverIndex);
        });
    // handling uploading firmware file 
    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
        }, []() {
            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {                               //start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE) {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) {                                                 //true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else {
                    Update.printError(Serial);
                }
            }
        });
    server.begin();
    // End Server Connection Setup for EPaper Display 

    // Begin Reader Setup 
    init_array();                                                               // initializes storage values of EPC to all 0
    Serial.begin(115200);                                                       // defined elsewhere, can probably remove
    while (!Serial);                                                            //Wait for the serial port to come online

    Serial.println(F("Ready to Begin Program"));                                //If this prints, then no wiring issues found
    if (DEBUG) nano.enableDebugging(Serial);

    // SetupNano establishes connection between ESP32 and M6E Nano Reader 

    if (setupNano(38400) == false)                                              //Configure nano to run at 38400bps
    {
        Serial.println(F("Module failed to respond. Please check wiring."));
        while (1); //Freeze!
    }

    nano.setRegion(NANOREGION);                                                //Set to the right region

    nano.setReadPower(500);                                                   //5.00 dBm. Higher values may caues USB port to brown out
                                                                              //Max Read TX Power is 27.00 dBm and may cause temperature-limit throttling
    // End Reader Setup 


    // Database setup
    connect_to_wifi();
}