#include "Globals.h";
#include "ePaperDisplay.h";
/*
 * **********************************************************************************************************************************************
 *  Function: loop
 *  Author: DJ Hansen and Joe Harrison
 *  Date: 11/29/2021
 *  From: North Carolina State University
 *  About:
 *  This loop runs continuously, checking first to make sure ePaper display is properly setup,
 *  Then goes into loop of scanning for time_pd_scan and then waiting for time_pd_wait
 *  Any Use of Serial.println is used for debugging purposes only and does not affect the outcome of the program
 *
 *
 * **********************************************************************************************************************************************
 */

 /****************************************************************************************************************************
  *                     Global Variables and Objects needed for Reader and Database Functionality                            *
  ****************************************************************************************************************************
  */

uint8_t EPC_recv[EPC_COUNT][12];                 //stores unique tag values in a session 
const unsigned long time_pd_wait = 60000;        // how long we should wait in between scan sessions (ms) 
const unsigned long time_pd_scan = 30000;        // scan duration (ms) 
unsigned long prev_time = 0;                     // timer wait update
unsigned long prev_scan_time = 0;                // timer scan update 
bool onetime = true;                             // one time flag 
bool firstTime = true;                           // one time flag 
unsigned long scan_time = 0;                     // used to store millis() 

// Object Instances 
RFID nano;                                      //Create instance for reader 
HTTPClient http;                               //Create instance for http connection


void loop()
{
    unsigned long curr_time = millis();       // gets current run time 
  // Begin Epaper Section of Loop

    server.handleClient();
    delay(5);
    if (!add_image_done) {
        add_image();
        add_image_done = true;
    }

    // End Epaper Section of loop  
    /*************************************************************************************************************************************************/
    // Begin Reader and Database Loop 

    if (((curr_time - prev_time) >= time_pd_wait) || onetime) {      // This checks for two things: either we are going into this loop for the first time,
        onetime = false;                                             // or we have waited the designated amount of time. 
        clear_arrays();
        nano.startReading();                                         //Begin scanning for tags
        Serial.println(F("Done waiting"));
        prev_scan_time = millis();
        scan_time = millis();
        scan_time += 1;

        while (scan_time - prev_scan_time < time_pd_scan) {           // Stay in this loop until we have been in it for time_pd_scan ms 
            scan_time = millis();

            if (nano.check() == true)                                  //Check to see if any new data has come in from module
            {
                byte responseType = nano.parseResponse();                //Break response into tag ID, RSSI, frequency, and timestamp

                if (responseType == RESPONSE_IS_KEEPALIVE)              // Haven't found anything yet......
                {
                    Serial.println(F("Scanning"));
                }
                else if (responseType == RESPONSE_IS_TAGFOUND)          // Great! We found a Tag! 
                {
                    //If we have a full record we can pull out the fun bits
                    int rssi = nano.getTagRSSI();                         //Get the RSSI for this tag read
                    long freq = nano.getTagFreq();                        //Get the frequency this tag was detected at
                    long timeStamp = nano.getTagTimestamp();              //Get the time this was read, (ms) since last keep-alive message
                    byte tagEPCBytes = nano.getTagEPCBytes();             //Get the number of bytes of EPC from response

                    if (check_array(nano.msg)) {                                // Check to see if we have already found this tag in this session, don't do anything if we have  
                    }
                    else {
                        add_array(nano.msg);                                 // If we haven't then store it 
                                                                             //Print EPC bytes, this is a subsection of bytes from the response/msg array

                        Serial.print(F(" epc["));                            // This code is just used to print it to serial output 
                        for (byte x = 0; x < tagEPCBytes; x++)
                        {
                            if (nano.msg[31 + x] < 0x10) Serial.print(F("0"));
                            Serial.print(nano.msg[31 + x], HEX);
                            Serial.print(F(" "));
                        }
                        Serial.print(F("]"));

                        Serial.println();
                    }
                }
                else if (responseType == ERROR_CORRUPT_RESPONSE)      // Bad CRC 
                {
                    Serial.println("Bad CRC");
                }
                else
                {
                    //Unknown response
                    Serial.print("Unknown error");
                }
            }
        }
        firstTime = false;                                      // Shut Everything down and begin storing found tags in database 
        nano.stopReading();
        Serial.println(F("Done scanning"));
        delay(500);
        database();
        prev_time = curr_time;
    }
    // End Reader and Database Loop