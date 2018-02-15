#include "LCD.h"


static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--)
  {
  }
}


void drawSquare_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);
		printf("don't blink!!!!\n");
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DrawRect(x1,y1,60,60);
	}
}
void blinkSquare_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);

		//	for (int i=0; i<10; i++){
				LCD_SetTextColor(LCD_COLOR_BLACK);	
				LCD_DrawRect(x1,y1,60,60);
				osDelay(50);
				LCD_SetTextColor(LCD_COLOR_WHITE);	
				LCD_DrawRect(x1,y1,60,60);
				
		//	}
	}
}

void drawRectangle_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);
		printf("don't blink!!!!\n");
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DrawRect(x1,y1,60,110);	
	}
}
void blinkRectangle_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);

		//	for (int i=0; i<10; i++){
				LCD_SetTextColor(LCD_COLOR_BLACK);	
				LCD_DrawRect(x1,y1,60,110);
				osDelay(50);
				LCD_SetTextColor(LCD_COLOR_WHITE);	
				LCD_DrawRect(x1,y1,60,110);

		//	}
	}
}

void drawTriangle_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);
		printf("don't blink!!!!\n");
		LCD_SetTextColor(LCD_COLOR_RED);	
		LCD_DrawUniLine(x1,y1,x1+30,y1-45);
		LCD_DrawUniLine(x1+30,y1-45,x1+60,y1);
		LCD_DrawUniLine(x1+60,y1,x1,y1);
	}
}
void blinkTriangle_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);
		printf("Triangle blink!!!!\n");
		//	for (int i=0; i<10; i++){
				LCD_SetTextColor(LCD_COLOR_BLACK);	
				LCD_DrawUniLine(x1,y1,x1+30,y1-45);
				LCD_DrawUniLine(x1+30,y1-45,x1+60,y1);
				LCD_DrawUniLine(x1+60,y1,x1,y1);
				osDelay(50);
				LCD_SetTextColor(LCD_COLOR_WHITE);
				LCD_DrawUniLine(x1,y1,x1+30,y1-45);
				LCD_DrawUniLine(x1+30,y1-45,x1+60,y1);
				LCD_DrawUniLine(x1+60,y1,x1,y1);	

		//	}
	}
}

void drawLine_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);
		printf("solid line!!!!\n");
		LCD_SetTextColor(LCD_COLOR_RED);	
		//LCD_DrawRect(x1,y1,60,60);
		
		LCD_DrawUniLine(x1,y1,x2,y2);
	//	osDelay(250);
	}
}
void blinkLine_thread(void const *argument){
	
	while(1){
		osSignalWait (0x01, osWaitForever);
		//	for (int i=0; i<10; i++){
				LCD_SetTextColor(LCD_COLOR_BLACK);	
				LCD_DrawUniLine(x1,y1,x2,y2);
				osDelay(50);
				LCD_SetTextColor(LCD_COLOR_WHITE);
				LCD_DrawUniLine(x1,y1,x2,y2);

		//	}
	}
}


