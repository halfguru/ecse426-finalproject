#include "motors.h"
#include "math.h"


TIM_OCInitTypeDef TIM_OCStruct;

void motors_init(){
	// Hardware clock init
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//	Desired rate can be found by using the equation: 42Mhz/(Prescaler * period)
	TIM_TimeBaseInitTypeDef Timer_InitStruct; 
	Timer_InitStruct.TIM_Prescaler = 420;
	Timer_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	Timer_InitStruct.TIM_Period = 2000;
	Timer_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	Timer_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &Timer_InitStruct);
	TIM_Cmd(TIM3, ENABLE);
	
		GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Clock for GPIOD */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Alternating functions for pins */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
	
	/* Set pins */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCStruct);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCStruct);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OC3Init(TIM3, &TIM_OCStruct);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

}


void set_angles(){
	int duty_cycle;
	
	duty_cycle = (180-motor_0_angle)*2+150;
	//printf("%d\n", duty_cycle);
	TIM_OCStruct.TIM_Pulse = duty_cycle;
	TIM_OC1Init(TIM3, &TIM_OCStruct);

	duty_cycle = (motor_1_angle*2)+150;
	TIM_OCStruct.TIM_Pulse = duty_cycle;
	TIM_OC2Init(TIM3, &TIM_OCStruct);

	duty_cycle = (motor_2_angle*2)+150;
	TIM_OCStruct.TIM_Pulse = duty_cycle;
	TIM_OC3Init(TIM3, &TIM_OCStruct);

}

void path_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);
		goTo(x_path[0], y_path[0]);
		osDelay(1000);
		upDown(down);
		osDelay(1000);
		for(int i=1; i < 50; i++){
			goTo(x_path[i], y_path[i]);
			//printf("position : {%f,%f}\n", x_path[i], y_path[i]);
			set_angles();
			osDelay(50);
		if(x_path[i]==x_path[i-1]&&y_path[i]==y_path[i-1]) break;
		}
		upDown(up);
	}
}

void upDown(int in){
	
	if (in == up) motor_2_angle=110;
	if (in == down) motor_2_angle=180;
	set_angles();

}

void goTo(double x, double y){
	
	if(x<x_min) x=x_min;
	else if(x>x_max) x=x_max;
		
	if (y<y_min) y=y_min;
	else if(y>y_max) y=y_max;
			
	double c_l = sqrt(pow(x+motor_position, 2)+pow(y,2));
	double c_r = sqrt(pow(x-motor_position, 2)+pow(y,2));
	
	double alpha_l = acos((pow(arm_a,2)+pow(c_l, 2)-pow(arm_b,2))/(2*arm_a*c_l));
	double alpha_r = acos((pow(arm_a,2)+pow(c_r, 2)-pow(arm_b,2))/(2*arm_a*c_r));
	
	double beta_l;
	double beta_r;
	
	if (x > motor_position){
		beta_l = atan(y/(x+motor_position));
		beta_r = PI-atan(y/(x-motor_position));
	}else if(x < motor_position && x > -motor_position){
		beta_l = atan(y/(x+motor_position));
		beta_r = atan(y/(motor_position-x));
	}else if(x < -motor_position){
		beta_l = PI-atan(y/(-x-motor_position));
		beta_r = atan(y/(-x+motor_position));
	}else if(x == motor_position){
		beta_l = atan(y/(x+motor_position));
		beta_r = PI/2;
	}else {
		beta_l = PI/2;
		beta_r = atan(y/(-x+motor_position));
	}
	
	motor_0_angle = ((180/PI)*(alpha_l+beta_l));
	motor_1_angle = (180/PI)*(alpha_r+beta_r);

	current_x = x;
	current_y = y;
	set_angles();
//	printf("position : {%f,%f}\n", x, y);
//	printf("beta l is: %f\n",  beta_l);
//	printf("beta r is: %f\n", beta_r);
//	printf("motor 0 is: %f\n", motor_0_angle);
//	printf("motor 1 is: %f\n", motor_1_angle);
		
}

void drawSquare(double x, double y){
	
	if(x < x_min+square_side) x = x_min+square_side;
	if(y < y_min+square_side) y = y_min+square_side;
	
	x_path[0] = x;
	y_path[0] = y;
	
	int i;

	for (i = 1; i<=4*(square_side/step_size); i++){
		if (i<=(square_side/step_size)){
			x_path[i] = x_path[i-1];
			y_path[i] = y_path[i-1]-step_size;
		} else if(i<=(2*square_side/step_size)){
			x_path[i] = x_path[i-1]-step_size;
			y_path[i] = y_path[i-1];
		} else if(i<=(3*square_side/step_size)){
			x_path[i] = x_path[i-1];
			y_path[i] = y_path[i-1]+step_size;
		}else if(i<=(4*square_side/step_size)){
			x_path[i] = x_path[i-1]+step_size;
			y_path[i] = y_path[i-1];
		}
	}
	
	for(;i<array_length;i++){
		x_path[i] = x_path[i-1];
		y_path[i] = y_path[i-1];
	}
	
}

void drawRectangle(double x, double y){
	
	if(x < x_min+rect_y) x = x_min+rect_y + 0.5;
	if(y < y_min+rect_x) y = y_min+rect_x + 0.5;
	
	x_path[0] = x;
	y_path[0] = y;
	
	int i;
	
	for (i = 1; i<=2*((rect_x+rect_y)/step_size); i++){
		if (i<=(rect_x/step_size)){
			x_path[i] = x_path[i-1];
			y_path[i] = y_path[i-1]-step_size;
		} else if(i<=((rect_x+rect_y)/step_size)){
			x_path[i] = x_path[i-1]-step_size;
			y_path[i] = y_path[i-1];
		} else if(i<=((2*rect_x+rect_y)/step_size)){
			x_path[i] = x_path[i-1];
			y_path[i] = y_path[i-1]+step_size;
		}else if(i<=((2*(rect_x+rect_y))/step_size)){
			x_path[i] = x_path[i-1]+step_size;
			y_path[i] = y_path[i-1];
		}

	}
	
	for(;i<array_length;i++){
		x_path[i] = x_path[i-1];
		y_path[i] = y_path[i-1];
	}
	
}

void drawTriangle(double x, double y){
	
	if(x < x_min+triangle_side) x = x_min+triangle_side;
	if(y > y_max-triangle_side) y = y_max-triangle_side;
	
	x_path[0] = x;
	y_path[0] = y;
	
	int i;
	
	for (i = 1; i<3*(triangle_side/step_size); i++){
		if (i<=(triangle_side/step_size)){
			x_path[i] = x_path[i-1]-(step_size/2);
			y_path[i] = y_path[i-1]+(sqrt(3)*step_size/2);
		} else if(i<=((2*triangle_side)/step_size)){
			x_path[i] = x_path[i-1]-(step_size/2);
			y_path[i] = y_path[i-1]-(sqrt(3)*step_size/2);
		} else if(i<=((3*triangle_side)/step_size)){
			x_path[i] = x_path[i-1]+step_size;
			y_path[i] = y_path[i-1];
		}

	}
	for(;i<array_length;i++){
		x_path[i] = x_path[i-1];
		y_path[i] = y_path[i-1];
	}
	
}

void drawSegment(double x, double y, double angle){
	
//	if(x < x_min+triangle_side) x = x_min+triangle_side;
//	if(y > y_max-triangle_side) y = y_max-triangle_side;
	
	x_path[0] = x;
	y_path[0] = y;
	
	int i;
	
	for (i = 1; i<(segment_length/step_size); i++){
		x_path[i] = x_path[i-1]+(step_size*cos(angle * PI/180));
		y_path[i] = y_path[i-1]+(step_size*sin(angle * PI/180));
	}
	for(;i<array_length;i++){
		x_path[i] = x_path[i-1];
		y_path[i] = y_path[i-1];
	}
	
}

void drawBoard_thread(void const *argument){
	while(1){
		osSignalWait (0x01, osWaitForever);
		// draw first line
		upDown(up);
		osDelay(500);
		goTo(-5, 10);
		osDelay(500);
		upDown(down);
		for(float i=0; i<=6; i=i+0.1){
			goTo(-5+i, 10);
			//printf("print something...\n");
			osDelay(50);
		}
		upDown(up);
		osDelay(500);
		goTo(1, 8);
		osDelay(500);
		upDown(down);
		osDelay(500);
		for(float i=0; i<=6; i=i+0.1){
			goTo(1-i, 8);
			osDelay(50);
		}
		upDown(up);
		osDelay(500);
		goTo(-3, 6);
		osDelay(500);
		upDown(down);
		osDelay(500);
		for(float i=0; i<=6; i=i+0.1){
			goTo(-3, 6+i);
			osDelay(50);
		}
		upDown(up);
		osDelay(500);
		goTo(-1, 12);
		osDelay(500);
		upDown(down);
		osDelay(500);
		for(float i=0; i<=6; i=i+0.1){
			goTo(-1, 12-i);
			osDelay(50);
		}
		upDown(up);
	}
}

int x_start[3] = { -4, -2, 0};
int y_start[3] = {11, 9 , 7};

void drawO(int direction){
	int x = x_start[(direction-1)%3];
	int y = y_start[(direction-1)/3];
	
	for(int i = 0; i < 50; i++){
		x_path[i] = x+0.5*cos(2*PI*i/50);
		y_path[i] = y+0.5*sin(2*PI*i/50);
	}
	
}

void drawX(int direction){
	int x = x_start[(direction-1)%3];
	int y = y_start[(direction-1)/3];
	
	x_path[0] = x;
	y_path[0] = y;
	for(int i = 1; i < 5; i++){
		x_path[i] = x_path[i-1] + 0.1;
		y_path[i] = y_path[i-1] + 0.1;
	}
	for(int i = 5; i < 15; i++){
		x_path[i] = x_path[i-1] - 0.1;
		y_path[i] = y_path[i-1] - 0.1;
	}
	
	for(int i = 15; i < 20; i++){
		x_path[i] = x_path[i-1] + 0.1;
		y_path[i] = y_path[i-1] + 0.1;
	}
	
	for(int i = 20; i < 25; i++){
		x_path[i] = x_path[i-1] + 0.1;
		y_path[i] = y_path[i-1] - 0.1;
	}
	
	for(int i = 25; i < 35; i++){
		x_path[i] = x_path[i-1] - 0.1;
		y_path[i] = y_path[i-1] + 0.1;
	}
	
	for(int i = 35; i < 50; i++){
		x_path[i] = x_path[i-1];
		y_path[i] = y_path[i-1];
	}
}




