#include "keypad.h"
#include <stdio.h>
#include "stm32f4xx.h"                  
#include "stm32f4xx_conf.h"



int count;

int result=0;
int lastResult=1;
int angleCount=0;
//int angle;


void configInit_GPIO(GPIO_TypeDef* GPIOx,
										 uint32_t periph_GPIOx,
										 uint32_t pins,
										 GPIOMode_TypeDef mode,
										 GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType,
										 GPIOPuPd_TypeDef puPd){
	//Enable GPIO clock
	RCC_AHB1PeriphClockCmd(periph_GPIOx, ENABLE);
	
	//GPIO configuration
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = pins;
	gpio.GPIO_Mode = mode;
	gpio.GPIO_Speed = speed;
	gpio.GPIO_OType = oType;
	gpio.GPIO_PuPd = puPd;
	
	GPIO_Init(GPIOx, &gpio);
										 											 
}

void Keypad_readDigit(){
	
	configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
										GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12,
										GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
										GPIO_PuPd_DOWN);
	// Set the colomns high
	GPIO_WriteBit(GPIOC, GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12, Bit_SET);
	
	// Read the rows
	configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
										GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7,
										GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
										GPIO_PuPd_DOWN);
										
	if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)== Bit_RESET && GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4) == Bit_RESET && GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5) == Bit_RESET &&	GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7) == Bit_RESET){
		// Nothing is pressed
		result = 99;
	} 
	
	else{
	
		// First row
		if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2) == Bit_SET){
			// Set this row to output
			configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
											GPIO_Pin_2,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
											GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_SET){
				result = 1;
			}			
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == Bit_SET){
				result = 2;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_SET){
				result = 3;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == Bit_SET){
				result = 10; // A
			}
		
		} 
		// Second row
		else if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4) == Bit_SET){
			// Set this row to output
			configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
											GPIO_Pin_4,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
											GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_SET){
				result = 4;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == Bit_SET){
				result = 5;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_SET){
				result = 6;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == Bit_SET){
				result = 11; // B
			}
			
		} 
		
		// Third row
		else if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5) == Bit_SET){
			// Set this row to output
			configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
											GPIO_Pin_5,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
								GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12,
								GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
								GPIO_PuPd_DOWN);
			
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_SET){
				result = 7;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == Bit_SET){
				result = 8;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_SET){
				result = 9;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == Bit_SET){
				result = 12; // C
			}
		}
		
		// Fourth row
		else{
			// Set this row to output
			configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
											GPIO_Pin_7,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOD, GPIO_Pin_7, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
								GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12,
								GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
								GPIO_PuPd_DOWN);
			
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_SET){
				result = 21; // Mode *
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == Bit_SET){
				result = 0;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == Bit_SET){
				result = 22; // Mode #
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == Bit_SET){
				result = 13; // Enter
			}
		}
	}
	
}

void Keypad_read(){
	send = 0;
	Keypad_readDigit();
	//printf("result: %i\n", result);
	
	if (result != lastResult && result != 99/*&& count<5*/){
		
		uint8_t pkt;
		makeLCD2MotorPkt(&pkt, result, 0);
		printf("transmitting: 0x%02x\n", pkt);
		CC2500_Transmit(&pkt, 1);
		
		drawn = 0;
		if (result == 21) mode = STAR;
		if (result == 22) {
			mode = TicTacToe;
			shape = BOARD;
			send = 1;
			printf("drawing board\n");
		}
			
		if (mode == STAR){
			
			// Shapes
			if (result == 10) {
				shape = SQUARE;
				printf("square\n");
				//send = 1;
				direction = RESET;
			}
			else if (result == 11){
				shape = RECTANGLE;
				//send = 1;
				direction = RESET;
			}
			else if (result == 12){
				shape = TRIANGLE;
				//send = 1;
				direction = RESET;
			}
			else if (result != 13){
				printf("result: %i\n", result);
				shape = 0;
				// Straight lines
				if (result == 6) userInput = 0;
				if (result == 2) userInput = 90;
				if (result == 4) userInput = 180;
				if (result == 8) userInput = 270;
				
				// 30 to 60 degrees
				if (result == 3) {
					if (angleCount == 0){
						userInput = 30;
						angleCount ++;
					}else if (angleCount == 1){
						userInput = 45;
						angleCount ++;
					}else if (angleCount == 2){
						userInput = 60;
						angleCount = 0;
					}
				}
				// 120 to 150 decrees
				if (result == 1) {
					if (angleCount == 0){
						userInput = 120;
						angleCount ++;
					}else if (angleCount == 1){
						userInput = 135;
						angleCount ++;
					}else if (angleCount == 2){
						userInput = 150;
						angleCount = 0;
					}
				}
				// 210 to 240 decrees
				if (result == 7) {
					if (angleCount == 0){
						userInput = 210;
						angleCount ++;
					}else if (angleCount == 1){
						userInput = 225;
						angleCount ++;
					}else if (angleCount == 2){
						userInput = 240;
						angleCount = 0;
					}
				}
				// 300 to 330 decrees
				if (result == 9) {
					if (angleCount == 0){
						userInput = 300;
						angleCount ++;
					}else if (angleCount == 1){
						userInput = 315;
						angleCount ++;
					}else if (angleCount == 2){
						userInput = 330;
						angleCount = 0;
					}
				}
				direction = userInput;
			}
			
			if (result == 5) {
				direction = RESET;
				shape = 0;
				send = 1;
				angleCount =0;
			}else if (result == 13) { // send result
				//direction = userInput;
				send = 1;
				angleCount =0;
				printf("shape: %i\n", shape);
			}
			
		}else if (mode == TicTacToe){
			
			printf("I am here...\n");
			if (result < 10){ // alternate 
				printf("setting direction...\n");
				if(shape == BOARD || shape == X) shape = O; 
				else shape = X;
				direction = result;
				send = 1;
			}
		}
		
		
		
		
		
		count = 0;
	}else	if (result == 99) {
		count ++;
	}
	
	lastResult = result; 
}
