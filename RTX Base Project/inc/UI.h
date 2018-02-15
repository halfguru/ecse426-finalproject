/**
   @file UI.h

*/

#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"

#define STAR 1
#define TicTacToe 2

#define SQUARE 1
#define RECTANGLE 2
#define TRIANGLE 3
#define BOARD 4
#define O 5
#define X 6



#define RESET 500




/** @var
		@brief The value of the user input
	*/
extern int direction;
extern int mode;
extern int shape;
extern int send;
extern uint8_t result;

/**
  * @brief Configure a GPIO port and Initialize it.
  *
	* The first two parameters correspond to selecting the specific GPIO port and bus.
	* The remaining parameters are used to set the values of the GPIO initilization struct (GPIO_InitTypeDef).
	*
  * @param GPIOx Specifies the GPIO port to enable
  * @param periph_GPIOx Specifies the GPIO port to enable for the bus clock.
  * @param pins Specifies the GPIO pins to be configured.
  * @param mode Specifies the operating mode for the selected pins.
  * @param speed Specifies the speed for the selected pins.
  * @param oType Specifies the operating output type for the selected pins.
  * @param puPd Specifies the operating Pull-up/Pull down for the selected pins.
	* @retval none
  */
void configInit_GPIO(GPIO_TypeDef* GPIOx,
										 uint32_t periph_GPIOx,
										 uint32_t pins,
										 GPIOMode_TypeDef mode,
										 GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType,
										 GPIOPuPd_TypeDef puPd);

/**
	*	@brief Read user input from keypad
	*	
	*	- Read user input from keypad	(pins LEFT TO RIGHT)
	*																PE6(brown)	PC13(red)	PE4(orange)		PE5(yellow)	PB4(green)		PB5(blue)		PD1(purple)	PD2(black)
	*	- Special mappings:	
	*				A ------ value = 10 --- Equivalent to HEX representation 
	*				B ------ value = 11 --- Equivalent to HEX representation 
	*				C ------ value = 12 --- Equivalent to HEX representation
	*				D ------ value = 13 --- Equivalent to HEX representation (used as ENTER)
	*				# ------ value = 22 --- Used to select mode #
	*				*	------ value = 21	---	Used to select mode *
	*				idle --- value = 99 --- Result when nothing is pressed
	*	
	*/
void Keypad_readDigit(void);

/**
	*	@brief Assembling user input
	*	
	*	- Looking for user input
	*	-	Update when change detected
	*	-	Handle keypad bounce problem by counting the number if consecutive idle
	*	-	Exit when ENTER is pressed
	*	- Special mappings interpretation:	
	*				A ------ value = 10 --- Equivalent to HEX representation (not used)
	*				B ------ value = 11 --- Equivalent to HEX representation (not used)
	*				C ------ value = 12 --- Equivalent to HEX representation (not used)
	*				D ------ value = 13 --- Equivalent to HEX representation (not used)
	*				# ------ value = 22 --- Used as ENTER --- Exit the function
	*				*	------ value = 21	---	Used for calibration (not used)
	*				idle --- value = 99 --- Result when nothing is pressed --- Continue waiting for input
	*	
	*/
void Keypad_read(void);




