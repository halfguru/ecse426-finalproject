/**
   @file wireless.h
   @brief Driver for the TI MSP430-CC2500 Wireless Chipset
   @date 2015/04/14
*/

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_exti.h"
#include "misc.h"

/** @def CC2500_SPI The SPI used for communication with the CC2500 chipset */
#define CC2500_SPI							SPI4
/** @def CC2500_SPI_CLK The bus used for SPI*/
#define CC2500_SPI_CLK					RCC_APB2Periph_SPI4 // for 4

/**
	@brief CC2500 Pin and Port configurations
*/
#define CC2500_SPI_GPIO_CLK 					RCC_AHB1Periph_GPIOE
#define CC2500_SPI_CS_GPIO_CLK				RCC_AHB1Periph_GPIOC
#define CC2500_SPI_GPIO_PORT					GPIOE
#define CC2500_SPI_CS_GPIO_PORT				GPIOC
#define CC2500_SPI_GPIO_AF						GPIO_AF_SPI4

/** @def CC2500_SPI_NSS_PIN The chip select line for the SPI protocol. It is connected to pin 17 on the CC2500 chipset */
#define CC2500_SPI_NSS_PIN    				GPIO_Pin_13 // p17
/** @def CC2500_SPI_SCK_PIN The clock line for the SPI protocol. It is connected to pin 16 on the CC2500 chipset */
#define CC2500_SPI_SCK_PIN    				GPIO_Pin_2 // p16
/** @def CC2500_SPI_MISO_PIN The Master-In Slave-Out line for the SPI protocol. Connected to pin 15 on the CC2500 chipset */
#define CC2500_SPI_MISO_PIN    				GPIO_Pin_5 // p15
/** @def CC2500_SPI_MOSI_PIN The Master-Out Slave-In line for the SPI protocol. Connected to pin 18 on the CC2500 chipset */
#define CC2500_SPI_MOSI_PIN    				GPIO_Pin_6 // p18
#define CC2500_SPI_NSS_SOURCE         GPIO_PinSource13
#define CC2500_SPI_SCK_SOURCE         GPIO_PinSource2
#define CC2500_SPI_MISO_SOURCE        GPIO_PinSource5
#define CC2500_SPI_MOSI_SOURCE        GPIO_PinSource6

/**
	@brief CC2500 interrupt configurations
	CC2500_INT_PIN_SOURCE is the same as @ref CC2500_SPI_MISO_PIN.
	*/
#define CC2500_INT_PIN_SOURCE					EXTI_PinSource5
#define CC2500_INT_PORT_SOURCE				EXTI_PortSourceGPIOE

/**
	@brief States of wireless transmitter from the status byte
	*/
#define IDLE_STATE 0x00
#define RX_STATE 0x01
#define TX_STATE 0x02

/**
	@brief Strobe commands
	*/
#define SRES 0x30
#define SFSTXON 0x31
#define SXOFF 0x32
#define SCAL 0x33
#define SRX 0x34
#define STX 0x35
#define SIDLE 0x36
#define SWOR 0x38
#define SPWD 0x39
#define SFRX 0x3A
#define SFTX 0x3B
#define SWORRST 0x3C
#define SNOP 0x3D        

/* Define macros for general use */
/**
	@brief Timeout to send byte through SPI
	*/
#define FLAG_TIMEOUT ((uint32_t)0x1000)

/** @def CC2500_CS_LOW() Set the chip select line low */
#define CC2500_CS_LOW()       GPIO_ResetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_NSS_PIN)
/** @def CC2500_CS_HIGH() Set the chip select line high */
#define CC2500_CS_HIGH()      GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_NSS_PIN)


/**
	@brief The registers storing the bytes in the TX and RX FIFOs
	*/
#define CC2500REG_TXBYTES		(0x3A|0xFA)
#define CC2500REG_RXBYTES		(0x3B|0xFB)

/**
	@brief FIFO registers
	*/
#define CC2500REG_TX_FIFO		0x3F
#define CC2500REG_RX_FIFO		0xBF

/** @def READ_STATUS Macro for bust access to the status registers */
#define READ_STATUS 0x02
/** @def PKTSTATUS Packet status register */
#define PKTSTATUS 0x38

/**

*/
static uint8_t CC2500_SendByte(uint8_t byte);

/**
	@brief Change the state of the CC2500 chipset.
	Will block until the chipset has entered the desired state.
	
	@param Strobe The state to enter. Can have values:
								- SRX
								- STX
								- SIDLE
	*/
void CC2500_Change_State(uint8_t Strobe);

/**
	@brief Initialize the SPI and interrupt modules for the CC2500 chipset
	*/
void CC2500_Init(void);

/**
	@brief Send a strobe command to the CC2500 chipset.
	@param Strobe The strobe command. Can have the values:
								- SRES
								- SFSTXON
								- SXOFF
								- SCAL
								- SRX
								- STX
								- SIDLE
								- SWOR
								- SPWD
								- SFRX
								- SFTX
								- SWORRST
								- SNOP
	@retval uint8_t The status byte.
	*/
uint8_t CC2500_Strobe(uint8_t Strobe);

/**
	@brief Read from a register on the CC2500 chipset.
	
	@param pBuffer The buffer the read data will be stored.
	@param address The address of the register to start reading.
	@param bytesToRead The number of bytes to read. If reading from a status register above 0x30, use 0x02.
	*/
void CC2500_SPI_Read(uint8_t* pBuffer, uint8_t address, uint16_t bytesToRead);

/** 
	@brief Write to a register on the CC2500 chipset.
	Do not write multiple bytes if the registers are not consecutive.
	
	@param pBuffer The buffered data to write to the registers.
	@param address The address of the register to start writing.
	@param bytesToWrite The number of bytes to write.
	*/
void CC2500_SPI_Write(uint8_t* pBuffer, uint8_t address, uint16_t bytesToWrite);

/**
	@brief Read from the FIFO receive buffer.
	
	@param buffer The buffered to which the read data will be stored.
	*/
void CC2500_ReadRecvBuffer(uint8_t *buffer);

/**
	@brief Write data to the FIFO transmit buffer and activate transmit mode.
	Will only write one byte at a time.
	
	@param buffer The data to write to the FIFO transmit buffer
	@param num_bytes The number of bytes to write to the buffer.
	*/
uint8_t CC2500_Transmit(uint8_t *buffer, uint16_t num_bytes);

/**
	@brief Applies a bitmask to extract the current state of the CC2500 chipset from a returned status byte.
	
	@param status The status byte
	*/
uint8_t status_state(uint8_t status);
