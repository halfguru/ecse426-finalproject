/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"


#include <stdio.h>
#include <string.h>
#include "keypad.h"
#include "LCD.h"
#include "math.h"

#include "wireless.h"
#include "packet.h"

#define RECVSIG 0x01
#define SENDSIG 0x02

#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286

// For keypad
int direction;
int mode;
int shape;
int send;
int drawn=0; 
int userInput;

// For LCD
double x1=215;
double x2=90;
double y1=30;
double y2=100;
double xBlink=90;
double yBlink=100;
int blink = 1;
int done=0;

static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--)
  {
  }
}

// ID for theads
osThreadId keypad_thread_id;
osThreadId drawSquare_thread_id;
osThreadId blinkSquare_thread_id;
osThreadId drawRectangle_thread_id;
osThreadId blinkRectangle_thread_id;
osThreadId drawTriangle_thread_id;
osThreadId blinkTriangle_thread_id;
osThreadId drawLine_thread_id;
osThreadId blinkLine_thread_id;

osThreadId RecvData_thread;
osThreadId TransmitData_thread;

osThreadId toggle_thread;

void keypad_thread(void const *argument){
	int lastMode;
	int lastDirection;
	int lastShape;
	int lastUserInput;

	double last_x1;
	double last_y1;
	while(1){
			Keypad_read();
//		shape =1;
//		send=1;
//		direction = 45;
//		mode = 1;
		
	if(shape != 500){	
		printf("--------------------");	
		printf("send: %i\n", send);		
		printf("shape: %i\n", shape);
		printf("direction: %i\n", direction);
		printf("mode: %i\n", mode);
			if (send == 0) blink = 1;
			else {
				blink = 0;
				// send mode, direction and shape
			}
			printf("blink: %i\n", blink);

		
		if (direction == RESET){
			LCD_Clear(LCD_COLOR_WHITE);
			x1=215;
			y1=30;
		}
		if (mode == STAR){		//*
			//printf("++++++++++SHAPE: %i\n", shape);
			if (shape == SQUARE) {
				x1=90;
				y1=130;
				if(blink == 0) {
					osSignalSet (drawSquare_thread_id, 0x01);
					lastShape = shape; 
					shape =500;
				}else {
					osSignalSet (blinkSquare_thread_id, 0x01);
					lastShape = shape; 
					osDelay(50);
				}
				
				

			}else if (shape == RECTANGLE) {
				x1=90;
				y1=130;
				if(blink == 0) {
					osSignalSet (drawRectangle_thread_id, 0x01);
					lastShape = shape; 
					shape =500;
				}else{
					osSignalSet (blinkRectangle_thread_id, 0x01);
					lastShape = shape; 
					osDelay(50);
				}
				

			}else if (shape == TRIANGLE) {
				x1=90;
				y1=130;
				if(blink == 0) {
					osSignalSet (drawTriangle_thread_id, 0x01);
					lastShape = shape; 
					shape =500;
				}else {
					osSignalSet (blinkTriangle_thread_id, 0x01);
					lastShape = shape; 
					osDelay(50);
				}

			}else if (shape == 0 && direction != 500) {
				if (lastShape != 0){
					LCD_Clear(LCD_COLOR_WHITE);
					x1 = 215;
					y1 = 30;
				}
				if(send ==1) blink =0;
				else blink =1;
				
				x2 = x1-25*cos(PI*direction/180);
				y2 = y1+25*sin(PI*direction/180);
				if(blink == 0) {
					osSignalSet (drawLine_thread_id, 0x01);
					osDelay(50);
					x1=x2;
					y1=y2;
					lastShape = shape; 
					shape =500;
				}else {
	//				printf("blink the line!!!");
					osSignalSet (blinkLine_thread_id, 0x01);
					lastShape = shape; 
					osDelay(50);
				}
				

			}
		
		}else { //#
		
		}
		
	}
		
		osDelay(100);
	}
	

}

void ReceiveData(void const *argument) {
	uint8_t r_buffer;
	uint8_t prev;
	uint8_t not_idle = 1;
	while (1){
		if (not_idle) {
			prev = r_buffer;
			CC2500_Change_State (SRX);
			osSignalWait(RECVSIG, osWaitForever);
		}
		uint8_t status = status_state(CC2500_Strobe(SNOP));
		if (status != IDLE_STATE) {
			not_idle = 0;
			osDelay(100);
		} else {
			not_idle = 1;
		}
		if (not_idle) {
			CC2500_ReadRecvBuffer(&r_buffer);
			if (r_buffer != prev && status != RX_STATE) {
				// processing for non-repeated data
				printf("read: 0x%02x\n", r_buffer);

			}
		}
	}
}

void TransmitData(void const *argument) {
	while (1) {
		// wait for a signal to send
		osSignalWait(SENDSIG, osWaitForever);
		uint8_t pkt;
		makeLCD2MotorPkt(&pkt, 13, 0);
		printf("transmitting: 0x%02x\n", pkt);
		CC2500_Transmit(&pkt, 1);
	}
}

osThreadDef(keypad_thread, osPriorityNormal, 1, 0);
osThreadDef(drawSquare_thread, osPriorityNormal, 1, 0);
osThreadDef(blinkSquare_thread, osPriorityNormal, 1, 0);
osThreadDef(drawRectangle_thread, osPriorityNormal, 1, 0);
osThreadDef(blinkRectangle_thread, osPriorityNormal, 1, 0);
osThreadDef(drawTriangle_thread, osPriorityNormal, 1, 0);
osThreadDef(blinkTriangle_thread, osPriorityNormal, 1, 0);
osThreadDef(drawLine_thread, osPriorityNormal, 1, 0);
osThreadDef(blinkLine_thread, osPriorityNormal, 1, 0);

osThreadDef(ReceiveData, osPriorityNormal, 1, 0);
osThreadDef(TransmitData, osPriorityNormal, 1, 0);

void toggle(void const *argument) {
	while (1) {
		osDelay(1);
		osSignalSet(TransmitData_thread, SENDSIG);
		osDelay(1000);
	}
}
osThreadDef(toggle, osPriorityNormal, 1, 0);


/*
 * main: initialize and start the system
 */
int main (void) {
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS
 
	// initialize peripherals here
	/* LCD initiatization */
  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC controler */
  LTDC_Cmd(ENABLE);
  /* Set LCD foreground layer as the current layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_Clear(LCD_COLOR_WHITE);
	
	// initialize wireless SPI
	CC2500_Init();

	keypad_thread_id = osThreadCreate(osThread(keypad_thread), NULL);
	drawSquare_thread_id = osThreadCreate(osThread(drawSquare_thread), NULL);
	blinkSquare_thread_id = osThreadCreate(osThread(blinkSquare_thread), NULL);	
	drawRectangle_thread_id = osThreadCreate(osThread(drawRectangle_thread), NULL);
	blinkRectangle_thread_id = osThreadCreate(osThread(blinkRectangle_thread), NULL);	
	drawTriangle_thread_id = osThreadCreate(osThread(drawTriangle_thread), NULL);
	blinkTriangle_thread_id = osThreadCreate(osThread(blinkTriangle_thread), NULL);	
	drawLine_thread_id = osThreadCreate(osThread(drawLine_thread), NULL);
	blinkLine_thread_id = osThreadCreate(osThread(blinkLine_thread), NULL);		

	//RecvData_thread = osThreadCreate(osThread(ReceiveData), NULL);
//	TransmitData_thread = osThreadCreate(osThread(TransmitData), NULL);
//	toggle_thread = osThreadCreate(osThread(toggle), NULL);
	
	osKernelStart ();                         // start thread execution 
}
				

void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
		osSignalSet (RecvData_thread, RECVSIG);
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}
