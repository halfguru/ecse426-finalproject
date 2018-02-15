/**
   @file packet.h
   @brief Configure packets to be sent between the LCD and motor boards.
   @date 2015/04/14
*/

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/**
	@brief General packet information
	*/
#define PKT_LENGTH 1 // byte
#define CONFIRMATION_PKT 0x99

/**
	@brief Create a packet to send from the LCD board to the motor board.
	
	Packet format:
	 ---------------------------------------------------
	| Toggle Bit (7) | State (6-4) | Keypad Value (3-0) |
	 ---------------------------------------------------
	
	@param pkt The packet to store the values
	@param keypad_val The keypad value read.
	@param state The current state of the CC2500 chipset. Currently not used. Set to 0
	*/
void makeLCD2MotorPkt(uint8_t *pkt, uint8_t keypad_val, uint8_t state);

/**
	@brief Create a packet to send from the motor board to the LCD board.
	Not used.
	
	Packet format:
	 -----------------------------
	| X data (7-4) | Y Data (3-0) |
	 -----------------------------
	
	@param pkt the packet to store the values
	@param x The x position of the pen. Must satisfy -7 <= x <= 7
	@param y The y position of the pen. Must satisfy 6 <= y <= 12
	@param fail Indicate that the received packet contained corrupted information
	*/
uint8_t makeCallbackPkt(uint8_t *pkt, int8_t x, int8_t y, uint8_t fail);
