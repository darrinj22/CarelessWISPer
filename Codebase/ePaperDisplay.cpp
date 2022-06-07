#include "ePaperDisplay.h";
#include "Globals.h";
/****************************************************************************************************************************
 *                     Global Variables and Objects needed for EPaper Display                                               *
 ****************************************************************************************************************************
 */

bool add_image_done = false;
const char* host = "esp32";
const char* ssid = "ncsu";
const char* password = "";

long image_data_size[] = { 2888, 2756, 5624, 5808, 4736, 12480, 15000, 10560 };  //followed by the index above

//variabls for blinking an LED with Millis
const int led = 2;                                                        // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0;                                         // will store last time LED was updated
const long interval = 1000;                                               // interval at which to blink (milliseconds)
int ledState = LOW;                                                       // ledState used to set the LED

WebServer server(80);

/****************************************************************************************************************************
 *                     Functions needed for EPaper Display																	 *
 ****************************************************************************************************************************
 */

#if defined (ENERGIA)
	void sendIndexData(uint8_t index, const uint8_t* data, uint32_t len) {
		SPI.begin();
		SPI.setDataMode(SPI_MODE3);
		SPI.setClockDivider(SPI_CLOCK_DIV32);
		SPI.setBitOrder(MSBFIRST);

		digitalWrite(DC_PIN, LOW);                                                  //DC Low
		digitalWrite(CS_PIN, LOW);                                                  //CS Low
		delayMicroseconds(500);
		SPI.transfer(index);
		delayMicroseconds(500);
		digitalWrite(CS_PIN, HIGH);                                                  //CS High
		digitalWrite(DC_PIN, HIGH);                                                  //DC High
		digitalWrite(CS_PIN, LOW);                                                  //CS Low
		delayMicroseconds(500);

		for (int i = 0; i < len; i++) {
			SPI.transfer(data[i]);
		}

		delayMicroseconds(500);
		digitalWrite(CS_PIN, HIGH);                                                 //CS High
	}
#else
	void softwareSpi(uint8_t data) {
		for (int i = 0; i < 8; i++) {
			if (((data >> (7 - i)) & 0x01) == 1) {
				digitalWrite(SDA_PIN, HIGH);
			}else {
				digitalWrite(SDA_PIN, LOW);
			}

			digitalWrite(SCL_PIN, HIGH);
			digitalWrite(SCL_PIN, LOW);
		}
	}

	// Software SPI protocl setup
	void sendIndexData(uint8_t index, const uint8_t* data, uint32_t len) {
		digitalWrite(DC_PIN, LOW);                                              //DC Low
		digitalWrite(CS_PIN, LOW);                                              //CS Low
		softwareSpi(index);
		digitalWrite(CS_PIN, HIGH);                                             //CS High
		digitalWrite(DC_PIN, HIGH);                                             //DC High
		digitalWrite(CS_PIN, LOW);                                              //CS High
		for (int i = 0; i < len; i++) {
			softwareSpi(data[i]);
		}
		digitalWrite(CS_PIN, HIGH);                                             //CS High
	}
#endif

/****************************************************************************************************************************
 *                      Main Function needed for EPaper Display                                               *
 ****************************************************************************************************************************
 */

	void addImage(void) {
		pinMode(SCL_PIN, OUTPUT);
		pinMode(SDA_PIN, OUTPUT);
		pinMode(CS_PIN, OUTPUT);
		pinMode(DC_PIN, OUTPUT);
		pinMode(RESET_PIN, OUTPUT);
		pinMode(BUSY_PIN, INPUT);                                          //All Pins 0
		delay(5);
		digitalWrite(RESET_PIN, HIGH);                                      //RES# = 1
		delay(5);
		digitalWrite(RESET_PIN, LOW);
		delay(10);
		digitalWrite(RESET_PIN, HIGH);
		delay(5);
		digitalWrite(CS_PIN, HIGH);                                         //CS# = 1

		sendIndexData(0x00, &register_data[1], 1);                          //Soft-reset
		while (digitalRead(BUSY_PIN) != HIGH);

		sendIndexData(0xe5, &register_data[2], 1);                          //Input Temperature: 25C
		sendIndexData(0xe0, &register_data[3], 1);                          //Active Temperature
		sendIndexData(0x00, &register_data[4], 2);                           //PSR

		// Send 1st image data for black and white colors
		sendIndexData(0x10, BW_monoBuffer1, image_data_size[PDI_EPD_Size]); //First frame: black frame where 1=black and 0=white pixel
		sendIndexData(0x13, BW_0x00Buffer, image_data_size[PDI_EPD_Size]);  //Second frame: all 0x00

		sendIndexData(0x04, &register_data[0], 1);                          //Power on
		while (digitalRead(BUSY_PIN) != HIGH);
		sendIndexData(0x12, &register_data[0], 1);                          //Display Refresh
		while (digitalRead(BUSY_PIN) != HIGH);

		delay(5000);

		// Send 2nd image data for black and white colors
		sendIndexData(0x10, BW_monoBuffer2, image_data_size[PDI_EPD_Size]);  //First frame: black frame where 1=black and 0=white pixel
		sendIndexData(0x13, BW_0x00Buffer, image_data_size[PDI_EPD_Size]);  //Second frame: all 0x00

		sendIndexData(0x04, &register_data[0], 1);                          //Power on
		while (digitalRead(BUSY_PIN) != HIGH);
		sendIndexData(0x12, &register_data[0], 1);                         //Display Refresh
		while (digitalRead(BUSY_PIN) != HIGH);

		delay(5000);

		// Send 3rd image data for black and white colors
		sendIndexData(0x10, BW_monoBuffer3, image_data_size[PDI_EPD_Size]);  //First frame: black frame where 1=black and 0=white pixel
		sendIndexData(0x13, BW_0x00Buffer, image_data_size[PDI_EPD_Size]);  //Second frame: all 0x00

		sendIndexData(0x04, &register_data[0], 1);                          //Power on
		while (digitalRead(BUSY_PIN) != HIGH);
		sendIndexData(0x12, &register_data[0], 1);                          //Display Refresh
		while (digitalRead(BUSY_PIN) != HIGH);

		delay(5000);

		//Turn-off DC/DC
		sendIndexData(0x02, &register_data[0], 1);                          //Turn off DC/DC
		while (digitalRead(BUSY_PIN) != HIGH);
		digitalWrite(DC_PIN, LOW);
		digitalWrite(CS_PIN, LOW);
		digitalWrite(SDA_PIN, LOW);
		digitalWrite(SCL_PIN, LOW);
		digitalWrite(BUSY_PIN, LOW);
		delay(150);
		digitalWrite(RESET_PIN, LOW);
	}