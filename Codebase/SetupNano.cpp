#include "Globals.h";
/************************************************************************************************************************************************
 * Function: setupNano
 * Author:   Nathan Seidle @ SparkFun Electronics
 * Date: October 3rd, 2016
 * From:  https://github.com/sparkfun/Simultaneous_RFID_Tag_Reader
 * About: Gracefully handles a reader that is already configured and already reading continuously
 * Because Stream does not have a .begin() we have to do this outside the library
 *
 **********************************************************************************************************************************************/

bool setupNano(long baudRate)
{
    nano.begin(NanoSerial);                              //Tell the library to communicate over serial port

                                                        //Test to see if we are already connected to a module
                                                        //This would be the case if the Arduino has been reprogrammed and the module has stayed powered
    NanoSerial.begin(baudRate);                        //For this test, assume module is already at our desired baud rate
    while (!NanoSerial);                                 //Wait for port to open

                                                      //About 200ms from power on the module will send its firmware version at 115200. We need to ignore this.
    while (NanoSerial.available()) NanoSerial.read();

    nano.getVersion();

    if (nano.msg[0] == ERROR_WRONG_OPCODE_RESPONSE)
    {                                                //This happens if the baud rate is correct but the module is doing a ccontinuous read
        nano.stopReading();
        Serial.println(F("Module cont. reading. Asking it to stop..."));
        delay(1500);
    }

    else if (nano.msg[0] != ALL_GOOD)
    {                                                 //The module did not respond so assume it's just been powered on and communicating at 115200bps
        NanoSerial.begin(115200);                       //Start software serial at 115200
        nano.setBaud(baudRate);                         //Tell the module to go to the chosen baud rate. Ignore the response msg

        NanoSerial.begin(baudRate);                     //Start the serial port, this time at user's chosen baud rate

        nano.getVersion();                              //Test the connection
        if (nano.msg[0] != ALL_GOOD) return (false);    //Something is not right
    }

    //The M6E has these settings no matter what
    nano.setTagProtocol();                            //Set protocol to GEN2

    nano.setAntennaPort();                          //Set TX/RX antenna ports to 1

    return (true);                                  //We are ready to rock
}

/************************************************************************************************************************************************
 * Function: init_array
 * Author:   DJ Hansen @ North Carolina State University
 * Date: 11/29/21
 * From:
 * About: Initializes all storage arrays to zero
 *
 **********************************************************************************************************************************************/
void init_array() {
    int i;

    for (i = 0; i < EPC_COUNT; i++) {
        EPC_recv[i][0] = 0;
    }
}
/************************************************************************************************************************************************
 * Function: check_array
 * Author:   DJ Hansen @ North Carolina State University
 * Date: 11/29/21
 * From:
 * About: Checks to see if the scanned tag has already been scanned in this section
 *
 **********************************************************************************************************************************************/
bool check_array(uint8_t* msg) {
    int i, j;
    int found;
    i = 0;

    // as long as not end of list
    while (i < EPC_COUNT && EPC_recv[i][0] != 0) {
        found = 1;
        for (j = 0; j < 12; j++) {
            if (EPC_recv[i][j] != msg[31 + j]) {
                found = 0;
                j = 12;
                i++;
            }
        }

        // if found
        if (found == 1) return 1;
    }
    return 0;
}
/************************************************************************************************************************************************
 * Function: add_array
 * Author:   DJ Hansen @ North Carolina State University
 * Date: 11/29/21
 * From:
 * About: If this tag has not been scanned this section, then store it
 *
 **********************************************************************************************************************************************/
void add_array(uint8_t* msg) {
    int i, j;
    int found;
    i = 0;

    // as long as not end of list
    while (i < EPC_COUNT && EPC_recv[i][0] != 0) {
        found = 1;
        for (j = 0; j < 12; j++) {
            if (EPC_recv[i][j] != msg[31 + j]) {
                found = 0;
                j = 12;
                i++;
            }
        }
        // if found
        if (found == 1) return;
    }

    if (i == EPC_COUNT) {
        Serial.print(F("Can not add more to array"));
        Serial.println();
        return;
    }
    // add to array
    for (j = 0; j < 12; j++) {
        EPC_recv[i][j] = msg[31 + j];
    }
    Serial.print(F("Entry added"));
    Serial.println();
}
/************************************************************************************************************************************************
 * Function: count_entries
 * Author:   DJ Hansen @ North Carolina State University
 * Date: 11/29/21
 * From:
 * About: Counts the number of tags scanned in a session
 *
 **********************************************************************************************************************************************/
int count_entries(void) {
    int i = 0;

    while (i < EPC_COUNT) {
        if (EPC_recv[i][0] == 0)  break;
        i++;
    }
    return i;
}
/************************************************************************************************************************************************
 * Function: clear_arrays
 * Author:   DJ Hansen @ North Carolina State University
 * Date: 11/29/21
 * From:
 * About: same as init_array only this also sets the tag data sent to the database to a null string
 *
 **********************************************************************************************************************************************/
void clear_arrays(void) {
    for (int i = 0; i < EPC_COUNT; i++) {
        for (int j = 0; j < 12; j++) {
            EPC_recv[i][j] = 0;
            final_epc[i] = "";
        }
    }
}