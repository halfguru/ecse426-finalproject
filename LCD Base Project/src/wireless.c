#include "wireless.h"
#include "cc2500_settings.h"

uint8_t status_state(uint8_t status) {
	return ((status & 0x70) >> 4);
}

__IO uint32_t Timeout = FLAG_TIMEOUT;

int TIMEOUT_UserCallback(void) {
	while (1);
}

uint8_t CC2500_state;

void wireless_delay(long num_ticks){
	while(num_ticks-- > 0);
}

void CC2500_Change_State(uint8_t Strobe) {
	if (Strobe == SIDLE){
		CC2500_state = (status_state(CC2500_Strobe(SIDLE)));
		while (CC2500_state != IDLE_STATE){
			CC2500_state = (status_state(CC2500_Strobe(SNOP)));
		}	
	}
	else if (Strobe == SRX){
		CC2500_state = (status_state(CC2500_Strobe(SRX)));
		while (CC2500_state != RX_STATE){
			CC2500_state = (status_state(CC2500_Strobe(SNOP)));
		}	
	}
	else if (Strobe == STX){
		CC2500_state = (status_state(CC2500_Strobe(STX)));
		while (CC2500_state != TX_STATE){
			CC2500_state = (status_state(CC2500_Strobe(SNOP)));
		}	
	}	
}

uint8_t CC2500_Strobe(uint8_t Strobe){
	CC2500_CS_LOW();

	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE)== RESET);	// check flag for transmission to be RESET
	SPI_I2S_SendData(CC2500_SPI, Strobe);												// condition satisfied --> send command strobe

	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET);		// check flag for being busy to be SET
	CC2500_state = (SPI_I2S_ReceiveData(CC2500_SPI));											// set status to most recent received data on SPI1

	// Set chip select High at the end of the transmission
	CC2500_CS_HIGH();   
	return CC2500_state;
}

// Initializes the low level interface used to drive the wireless components
void LowLevel_Init(void){
  
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  // Enable the SPI periph
  RCC_APB2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);
  
	// Enable SCK, MOSI and MISO GPIO clocks
	RCC_AHB1PeriphClockCmd(CC2500_SPI_GPIO_CLK, ENABLE);
	
	// Enable CSn clock
	RCC_AHB1PeriphClockCmd(CC2500_SPI_CS_GPIO_CLK, ENABLE);
	
	// Alternate Functions
  GPIO_PinAFConfig(CC2500_SPI_GPIO_PORT, CC2500_SPI_SCK_SOURCE, CC2500_SPI_GPIO_AF); // SCK
  GPIO_PinAFConfig(CC2500_SPI_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_GPIO_AF); // MISO
	GPIO_PinAFConfig(CC2500_SPI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_GPIO_AF); // MOSI

  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  // SPI SCK pin configuration
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCK_PIN;
  GPIO_Init(CC2500_SPI_GPIO_PORT, &GPIO_InitStructure);

  // SPI  MOSI pin configuration
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MOSI_PIN;
  GPIO_Init(CC2500_SPI_GPIO_PORT, &GPIO_InitStructure);

  //SPI MISO pin configuration
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
  GPIO_Init(CC2500_SPI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
	// make sure you know why these are this way
  SPI_I2S_DeInit(CC2500_SPI);
  SPI_InitStructure.SPI_Direction 				= SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize 					= SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL 							= SPI_CPOL_Low; // polarity as per section 3.1 of configuring cc2500
  SPI_InitStructure.SPI_CPHA 							= SPI_CPHA_1Edge; // phase as per section 3.1 of configuring cc2500
  SPI_InitStructure.SPI_NSS 							= SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit 					= SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial 		= 7;
  SPI_InitStructure.SPI_Mode 							= SPI_Mode_Master; // master mode as per section 3.1
  SPI_Init(CC2500_SPI, &SPI_InitStructure);

  // Enable SPI1
  SPI_Cmd(CC2500_SPI, ENABLE);

  // Configure GPIO PIN for Lis Chip select
  GPIO_InitStructure.GPIO_Pin 	= CC2500_SPI_NSS_PIN;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CC2500_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure GPIO PINs to detect Interrupts */ //-------------------------------------------------------------------------------------------------------------
	GPIO_InitTypeDef GPIO_InitStructure_Int;
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN; // The pin to read interrupt data from; the accelerometer in this case
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; // input from GPIO
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // floating
  GPIO_Init(CC2500_SPI_GPIO_PORT, &GPIO_InitStructure_Int);
  
	SYSCFG_EXTILineConfig(CC2500_INT_PORT_SOURCE, CC2500_INT_PIN_SOURCE); // the interrupt port is GPIOE
	
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = EXTI_Line5; // line 0
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt; // configuring an interrupt
	exti_init.EXTI_Trigger = EXTI_Trigger_Rising; // rising for active high
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);
	
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = EXTI9_5_IRQn; // exti line 5 matching exti
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0x03; // high priority
	nvic_init.NVIC_IRQChannelSubPriority = 0x03; // high priority
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);

  // Deselect : Chip Select high
  GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_NSS_PIN);
	
	CC2500_CS_LOW();
	wireless_delay(100);
	CC2500_CS_HIGH();
	wireless_delay(100);
	CC2500_CS_LOW();
	wireless_delay(150);
	
	// Send reset command
	CC2500_Strobe(SRES); 
	
	while ((GPIO_ReadInputDataBit(CC2500_SPI_GPIO_PORT, CC2500_SPI_MISO_PIN) & 0x80) == 0x80);
	
	// Set to IDLE state
	uint8_t state = status_state(CC2500_Strobe(SIDLE));
	
	while (state != 0x00) {
		state = status_state(CC2500_Strobe(SNOP));
	}
	
	CC2500_CS_HIGH();
}


// Sends a byte through SPI protocol
uint8_t CC2500_SendByte(uint8_t byte) {
	
	// Loop while DR register is not empty
  Timeout = FLAG_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET){
		if((Timeout--) == 0) return TIMEOUT_UserCallback();
  }
  
  // Send a Byte through the SPI peripheral
  SPI_I2S_SendData(CC2500_SPI, byte);
  
	// Wait to receive a Byte
  Timeout = FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET){
    if((Timeout--) == 0) return TIMEOUT_UserCallback();
  }
  
  // Return the Byte read from the SPI bus
  return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

void CC2500_SPI_Read(uint8_t* pBuffer, uint8_t address, uint16_t bytesToRead) {
	
	// enable Read mode
	uint8_t RW_mode = 0x80;
	
	// if more than 1 byte to read, enable burst mode
	uint8_t burst_mode = 0x00;
	if (bytesToRead > 0x01){
		burst_mode = 0x40;
	}
	
	// concatenate R/W and Burst bits to address
	address = RW_mode | burst_mode | address;
  //printf ("Read Add: %i \n", address);
	// start SPI
	CC2500_CS_LOW();
	
	// send desired source address of read
	CC2500_SendByte(address);
	
	// for each byte to read, send a dummy byte through SPI
	// send byte returned by SPI to buffer
	while (bytesToRead > 0x00){
		*pBuffer = CC2500_SendByte (0x00);
		//printf("Value read: %i\n", *pBuffer);
		pBuffer ++;
		bytesToRead--;
	}
		
	// end SPI
	CC2500_CS_HIGH();
}

void CC2500_SPI_Write(uint8_t* pBuffer, uint8_t address, uint16_t bytesToWrite) {
			
	// if more than 1 byte to write, enable burst mode
	uint8_t burst_mode = 0x00;
	if (bytesToWrite > 0x01){
		burst_mode = 0x40;
	}
	
	//printf("bytes to write: %i\n", bytesToWrite);
	
	// concatenate Burst bit to address
	address = burst_mode | address;
	//printf ("Write Add: %i \n", address);
	// start SPI
	CC2500_CS_LOW();
	
	// send desired destination address of write
	CC2500_SendByte(address);
	
	// send each byte in the buffer to registers through SPI
	while (bytesToWrite > 0x00){
		//printf("Value to write: %i\n", *pBuffer);
		CC2500_SendByte (*pBuffer);
		pBuffer ++;
		bytesToWrite--;
	}
		
	// end SPI
	CC2500_CS_HIGH();
}

uint8_t CC2500_Transmit(uint8_t *buffer, uint16_t num_bytes) {
	CC2500_CS_LOW();
	
	status_state(CC2500_Strobe(SIDLE));								
	while (status_state(CC2500_Strobe(SNOP)) != IDLE_STATE);
	
	uint8_t NumBytesinFIFO;
	CC2500_SPI_Read(&NumBytesinFIFO, CC2500REG_TXBYTES, 0x02);
	if ((64-NumBytesinFIFO) >= num_bytes) {
		CC2500_SPI_Write(buffer, CC2500REG_TX_FIFO, num_bytes);
	} else {
		return 0;
	}

	status_state(CC2500_Strobe(STX));
	while (status_state(CC2500_Strobe(SNOP)) != TX_STATE);
	while (status_state(CC2500_Strobe(SNOP)) != IDLE_STATE);
	
	CC2500_CS_HIGH();
	
	return 1;
}

void CC2500_ReadRecvBuffer(uint8_t *buffer) {
		uint8_t NumBytesinFIFO;
		CC2500_SPI_Read(&NumBytesinFIFO, CC2500REG_RXBYTES, 0x02);
		if (NumBytesinFIFO >= 0x01){
			CC2500_SPI_Read(buffer, CC2500REG_RX_FIFO, 0x01);
		}
}

void CC2500_Init(void) {
	LowLevel_Init();
	
	// Settings
	uint8_t w_buffer[1];

	w_buffer[0] = VAL_CC2500_IOCFG2;
	CC2500_SPI_Write(&w_buffer[0], 0x00, 1);

	w_buffer[0] = VAL_CC2500_IOCFG0;
	CC2500_SPI_Write(&w_buffer[0], 0x02, 1);
	
	w_buffer[0] = VAL_CC2500_FIFOTHR;
	CC2500_SPI_Write(&w_buffer[0], 0x03, 1);

	w_buffer[0] = VAL_CC2500_PKTLEN;
	CC2500_SPI_Write(&w_buffer[0], 0x06, 1);
	w_buffer[0] = VAL_CC2500_PKTCTRL1;
	CC2500_SPI_Write(&w_buffer[0], 0x07, 1);
	w_buffer[0] = VAL_CC2500_PKTCTRL0;
	CC2500_SPI_Write(&w_buffer[0], 0x08, 1);
	w_buffer[0] = VAL_CC2500_ADDR;
	CC2500_SPI_Write(&w_buffer[0], 0x09, 1);
	
	w_buffer[0] = VAL_CC2500_CHANNR;
	CC2500_SPI_Write(&w_buffer[0], 0x0A, 1);
	w_buffer[0] = VAL_CC2500_FSCTRL1;
	CC2500_SPI_Write(&w_buffer[0], 0x0B, 1);
	w_buffer[0] = VAL_CC2500_FSCTRL0;
	CC2500_SPI_Write(&w_buffer[0], 0x0C, 1);
	w_buffer[0] = VAL_CC2500_FREQ2;
	CC2500_SPI_Write(&w_buffer[0], 0x0D, 1);
	w_buffer[0] = VAL_CC2500_FREQ1;
	CC2500_SPI_Write(&w_buffer[0], 0x0E, 1);
	
	w_buffer[0] = VAL_CC2500_FREQ0;
	CC2500_SPI_Write(&w_buffer[0], 0x0F, 1);
	w_buffer[0] = VAL_CC2500_MDMCFG4;
	CC2500_SPI_Write(&w_buffer[0], 0x10, 1);
	w_buffer[0] = VAL_CC2500_MDMCFG3;
	CC2500_SPI_Write(&w_buffer[0], 0x11, 1);
	w_buffer[0] = VAL_CC2500_MDMCFG2;
	CC2500_SPI_Write(&w_buffer[0], 0x12, 1);
	w_buffer[0] = VAL_CC2500_MDMCFG1;
	CC2500_SPI_Write(&w_buffer[0], 0x13, 1);
	
	w_buffer[0] = VAL_CC2500_MDMCFG0;
	CC2500_SPI_Write(&w_buffer[0], 0x14, 1);
	w_buffer[0] = VAL_CC2500_DEVIATN;
	CC2500_SPI_Write(&w_buffer[0], 0x15, 1);
	w_buffer[0] = 0x00;
	w_buffer[0] = VAL_CC2500_MCSM1;
	CC2500_SPI_Write(&w_buffer[0], 0x17, 1);
	w_buffer[0] = VAL_CC2500_MCSM0;
	CC2500_SPI_Write(&w_buffer[0], 0x18, 1);
	
	w_buffer[0] = VAL_CC2500_FOCCFG;
	CC2500_SPI_Write(&w_buffer[0], 0x19, 1);
	w_buffer[0] = VAL_CC2500_BSCFG;
	CC2500_SPI_Write(&w_buffer[0], 0x1A, 1);
	w_buffer[0] = VAL_CC2500_AGCTRL2;
	CC2500_SPI_Write(&w_buffer[0], 0x1B, 1);
	w_buffer[0] = VAL_CC2500_AGCTRL1;
	CC2500_SPI_Write(&w_buffer[0], 0x1C, 1);
	w_buffer[0] = VAL_CC2500_AGCTRL0;
	CC2500_SPI_Write(&w_buffer[0], 0x1D, 1);

	w_buffer[0] = VAL_CC2500_FREND1;
	CC2500_SPI_Write(&w_buffer[0], 0x21, 1);
	w_buffer[0] = VAL_CC2500_FREND0;
	CC2500_SPI_Write(&w_buffer[0], 0x22, 1);
	
	w_buffer[0] = VAL_CC2500_FSCAL3;
	CC2500_SPI_Write(&w_buffer[0], 0x23, 1);
	w_buffer[0] = VAL_CC2500_FSCAL2;
	CC2500_SPI_Write(&w_buffer[0], 0x24, 1);
	w_buffer[0] = VAL_CC2500_FSCAL1;
	CC2500_SPI_Write(&w_buffer[0], 0x25, 1);
	w_buffer[0] = VAL_CC2500_FSCAL0;
	CC2500_SPI_Write(&w_buffer[0], 0x26, 1);

	w_buffer[0] = VAL_CC2500_FSTEST;
	CC2500_SPI_Write(&w_buffer[0], 0x29, 1);

	w_buffer[0] = VAL_CC2500_TEST2;
	CC2500_SPI_Write(&w_buffer[0], 0x2C, 1);
	
	w_buffer[0] = VAL_CC2500_TEST1;
	CC2500_SPI_Write(&w_buffer[0], 0x2D, 1);
	w_buffer[0] = VAL_CC2500_TEST0;
	CC2500_SPI_Write(&w_buffer[0], 0x2E, 1);
}


