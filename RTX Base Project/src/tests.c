#include "tests.h"
#include "wireless.h"
#include "packet.h"

// To test wireless module and SPI
void wireless_testbench (void){
	// initialize wireless SPI
	CC2500_Init();
	uint8_t r_buffer;
#if 0
	printf("\n\n");

	uint8_t settings_buffer[1];
	uint8_t address = 0x00;
	while (address < 47) {
		CC2500_SPI_Read(settings_buffer, address, 1);
		printf("settings 0x%02x: 0x%02x\n", address, settings_buffer[0]);
		address++;
		if (address == 1 || address == 22) {
			address++;
		} else if (address == 4 || address == 39 || address == 42) {
			address+=2;
		} else if (address == 30) {
			address+=3;
		}
	}

	settings_buffer[0] = 0x06;
	CC2500_SPI_Write(settings_buffer, 0x02, 1);
	
	CC2500_SPI_Read(settings_buffer, 0x02, 1);
	printf("read 0x%02x\n", settings_buffer[0]);
	
	printf("\n\n");
	
	// ------------------ SPI testing -----------------------
	
	uint8_t buffer[2];
	address = 0x31;
	uint16_t bytes = 0x02;
	
	// --- Strobe ----
	uint8_t status;
	status = CC2500_Strobe(SNOP);
	printf("status: 0x%02x\n\n", status);
	
	// ---- Read -----
	buffer[0] = 1;
	printf("buffer before: %i\n", buffer[0]);
	CC2500_SPI_Read (buffer, address, bytes);

	printf ("Value after: %i \n\n", buffer[0]);

	// --- Write ---
	uint8_t w_buffer[] = {8};
	printf("value writing: %i\n", w_buffer[0]);

	address = 0x06;
	bytes = 0x01;
	
	CC2500_SPI_Write (w_buffer, address, bytes);
	CC2500_SPI_Read (r_buffer, address, bytes);
	
	printf ("Value: %i \n", r_buffer[0]);
	
	printf("\n\n\n");
	
	// -- Strobe again ---
	status = CC2500_Strobe(SNOP);
	printf("status: 0x%02x\n", status);
	
	delay(100);
	
	status = CC2500_Strobe(SRX);
	printf("status: 0x%02x\n", status);
	
	delay(100);
	
	while (status != 0x01) {
		status = CC2500_Strobe(SNOP);
		printf("status: 0x%02x\n", status);
	
		delay (100);
	}
	
	status = CC2500_Strobe(SIDLE);
	printf("status: 0x%02x\n", status);
	
	delay(100);
	
	status = CC2500_Strobe(SNOP);
	printf("status: 0x%02x\n", status);
#endif
	// ---------------- Receive testing --------------------
#if 1
	while (1){
		CC2500_ReadRecvBuffer(&r_buffer);
		printf("receive: 0x%02x\n", r_buffer);
		
		delay(100);
		
//		uint8_t packet;
//		int8_t x = -5;
//		int8_t y = 8;
//		uint8_t fail = 0;
//		
//		makeCallbackPkt (&packet, x, y, fail);
//		
//		printf("transmit: 0x%02x\n", packet);
//		CC2500_Transmit (&packet, 1);
//		
//		delay(100);
	}
#endif
	
#if 0	
	uint8_t iteration = 2;
	uint8_t value = 86;
	while (1){
		if (iteration == 2){
			value = value + iteration;
			iteration = 0;
		}
		else {
			iteration = 2;
			value = value - iteration;
		}
		printf("transmit: 0x%02x \n",value);
		CC2500_Transmit(&value, 1);
		delay(200);
	}
#endif
}
