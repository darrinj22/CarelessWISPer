#pragma once
#include <cstdint>
#define HIGH 1
#define LOW 0
#define EPD_154 0 //1.54 inch PDi EPD (iTC)
#define EPD_213 1 //2.13 inch PDi EPD (iTC)
#define EPD_266 2 //2.66 inch PDi EPD (iTC)
#define EPD_271 3 //2.71 inch PDi EPD (iTC)
#define EPD_287 4 //2.87 inch PDi EPD (iTC)
#define EPD_370 5 //3.70 inch PDi EPD (iTC)
#define EPD_420 6 //4.20 inch PDi EPD (iTC)
#define EPD_437 7 //4.37 inch PDi EPD (iTC)

#define PDI_EPD_Size EPD_266
#if (PDI_EPD_Size >= 5)                                                         //3.70", 4.20", 4.37"
uint8_t register_data[] = { 0x00, 0x0e, 0x19, 0x02, 0x0f, 0x89 };           //0x00, soft-reset, temperature, active temp., PSR0, PSR1
#else                                                                           //other small sizes
uint8_t register_data[] = { 0x00, 0x0e, 0x19, 0x02, 0xcf, 0x8d };
#endif


#if(PDI_EPD_Size==2)//2.66"
	#include "image_266_296x152_BW.c"
	#define BW_monoBuffer        (uint8_t *) & image_266_296x152_BW_monoTest
	#define BW_monoBuffer1        (uint8_t *) & image_266_296x152_BW_mono1
	#define BW_monoBuffer2        (uint8_t *) & image_266_296x152_BW_mono2
	#define BW_monoBuffer3        (uint8_t *) & image_266_296x152_BW_mono3
	#define BW_monoBuffer4        (uint8_t *) & image_266_296x152_BW_mono4
	#define BW_0x00Buffer        (uint8_t *) & image_266_296x152_BW_0x00
#elif(PDI_EPD_Size==3) //2.71"
	#include "image_data\2.71\image_271_264x176_BW.c"
	#include "image_data\2.71\image_271_264x176_BWR.c"
	#define BW_monoBuffer        (uint8_t *) & image_271_264x176_BW_mono
	#define BW_0x00Buffer        (uint8_t *) & image_271_264x176_BW_0x00
	#define BWR_blackBuffer      (uint8_t *) & image_271_264x176_BWR_blackBuffer
	#define BWR_redBuffer        (uint8_t *) & image_271_264x176_BWR_redBuffer
#endif

#if defined(ENERGIA)                                                            // Valid pins for LaunchPad on Energia
	#define SCL_PIN 7                                                               // EXT3 board J4 pin 2 SCK
	#define BUSY_PIN 11                                                             // EXT3 board J4 pin 3 BUSY
	#define DC_PIN 12                                                               // EXT3 board J4 pin 4 D/C
	#define RESET_PIN 13                                                            // EXT3 board J4 pin 5 RST (RESET)
	#define SDA_PIN 15                                                             // EXT3 board J4 pin 7 MOSI
	#define CS_PIN 19                                                               // EXT3 board J4 pin 9 ECSM (EPD CS Master)
	void sendIndexData(uint8_t index, const uint8_t* data, uint32_t len);

// SPI protocl setup

#else // Valid pins for Arduino board, like M0 Pro
	#define SCL_PIN 18                                                         // EXT3 board J4 pin 2 SCK
	#define BUSY_PIN 15                                                       // EXT3 board J4 pin 3 BUSY
	#define DC_PIN 2                                                          // EXT3 board J4 pin 4 D/C
	#define RESET_PIN 4                                                       // EXT3 board J4 pin 5 RST (RESET)
	#define SDA_PIN 23                                                        // EXT3 board J4 pin 7 MOSI
	#define CS_PIN 21                                                        // EXT3 board J4 pin 9 ECSM (EPD CS Master)
	void softwareSpi(uint8_t data);
	void sendIndexData(uint8_t index, const uint8_t* data, uint32_t len);
// Software SPI setup

#endif

extern const char* loginIndex;
extern const char* serverIndex;
void addImage(void);
unsigned char const image_266_296x152_BW_mono1[];
unsigned char const image_266_296x152_BW_mono2[];
unsigned char const image_266_296x152_BW_mono3[];
unsigned char const image_266_296x152_BW_mono4[];
unsigned char const image_266_296x152_BW_monoTest[];
unsigned char const image_266_296x152_BW_0x00[];



