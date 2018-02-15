/**
   @file LCD.h
*/

#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "stm32f429i_discovery_lcd.h"
#include "osObjects.h"
#include "stdio.h"

extern double x1;
extern double x2;
extern double xBlink;
extern double y1;
extern double y2;
extern double yBlink;

extern int blink;


static void delay(__IO uint32_t nCount);


void drawSquare_thread(void const *argument);
void blinkSquare_thread(void const *argument);
void drawRectangle_thread(void const *argument);
void blinkRectangle_thread(void const *argument);
void drawTriangle_thread(void const *argument);
void blinkTriangle_thread(void const *argument);
void drawLine_thread(void const *argument);
void blinkLine_thread(void const *argument);