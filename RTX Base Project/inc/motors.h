#ifndef motors_h
#define motors_h

#include "stm32f4xx_gpio.h"
#include "osObjects.h"
#include "stdio.h"

// GPIO MOTOR CLOCKS
#define MOTOR_CLOCK RCC_AHB1Periph_GPIOB

//// GPIO MOTOR PORT
//#define MOTOR_PORT GPIOB

//// GPIO MOTOR PINS
//#define MOTOR_0 GPIO_Pin_11
//#define MOTOR_1 GPIO_Pin_12
//#define MOTOR_2 GPIO_Pin_13

// Constants
#define arm_a (double) 6.5
#define arm_b (double) 7
#define motor_position (double) 1.9
#define segm_length 1
#define step_size 0.5
	
#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286


// Frame
#define x_max 7
#define x_min -7
#define y_max 12
#define y_min 6


// Path
#define square_side 3
#define rect_x 2
#define rect_y 4
#define triangle_side 3
#define segment_length 2
#define array_length 50
#define up 1
#define down 0

extern double current_x;
extern double current_y;

extern double x_path[array_length];
extern double y_path[array_length];

extern double motor_0_angle;
extern double motor_1_angle;
extern double motor_2_angle;





void motors_init();
void set_angles();
void path_thread(void const *argument);

void upDown();

void goTo(double x, double y);
void draw_LroR(double x, double y);
void drawSquare(double x, double y);
void drawRectangle(double x, double y);
void drawTriangle(double x, double y);
void drawSegment(double x, double y, double angle);
void drawBoard_thread(void const *argument);


void drawO(int direction);
void drawX(int direction);
#endif



