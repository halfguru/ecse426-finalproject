#include "MEMS.h"

const double PI = 3.14159265359;

double x_data;
double y_data;
double z_data;

// initialize Kalman filter values
kalman_state x_kstate = {
	0.01,          // q
	1.396274,      // r
	0,             // x
	1,             // p
	0              // k
};

kalman_state y_kstate = {
	0.01,          // q
	0.743019,      // r
	0,             // x
	1,             // p
	0              // k
};

kalman_state z_kstate = {
	0.01,          // q
	1.724278,      // r
	1000,          // x
	1,             // p
	0              // k
};




// configures LIS3DSH settings and initializes it
void MEMS_config (){
	
	LIS3DSH_InitTypeDef LIS3DSH_InitStruct;
	
	// configure the LIS3DSH parameters
	LIS3DSH_InitStruct.AA_Filter_BW 								= LIS3DSH_AA_BW_800;
	LIS3DSH_InitStruct.Axes_Enable 									= LIS3DSH_X_ENABLE | LIS3DSH_Y_ENABLE | LIS3DSH_Z_ENABLE;
	LIS3DSH_InitStruct.Continous_Update 						= LIS3DSH_ContinousUpdate_Disabled;
	LIS3DSH_InitStruct.Full_Scale 									= LIS3DSH_FULLSCALE_2;
	LIS3DSH_InitStruct.Power_Mode_Output_DataRate 	= LIS3DSH_DATARATE_100;
	LIS3DSH_InitStruct.Self_Test 										= LIS3DSH_SELFTEST_NORMAL;

	// initialize LIS3DSH
	LIS3DSH_Init(&LIS3DSH_InitStruct);
}
// configures settings for interrupt
void MEMS_interrupt_config() {
	
	LIS3DSH_DRYInterruptConfigTypeDef LIS3DSH_InterruptConfigStruct;
	
	// configure interrupt signal
	LIS3DSH_InterruptConfigStruct.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED; // enable signal
	LIS3DSH_InterruptConfigStruct.Interrupt_signal 		= LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL; // active-high signal (pull-down)
	LIS3DSH_InterruptConfigStruct.Interrupt_type 			= LIS3DSH_INTERRUPT_REQUEST_PULSED;     // non-latched(pulsed)
	// initialize interrupt signal (output from GPIO to which ACC interrupt is hardwired)
	LIS3DSH_DataReadyInterruptConfig(&LIS3DSH_InterruptConfigStruct);
	
	/* Enable clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	/* Enable clock for SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Set pin as input */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	// connect PE1 pin to EXTI Line 1 (INT1)
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
	
	// configure EXTI line 1 (to be source for external interrupt from ACC)
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	// initialize EXTI
	EXTI_Init(&EXTI_InitStruct);
	
	// configure and set EXTI line 1 interrupt to the lowest priority in NVIC module
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	// initialize and enable interrupt in NVIC module
	NVIC_Init(&NVIC_InitStruct);
}


/* reads X,Y,Z values from sensor
   filters values using Kalman */
void MEMS_read_value () {
	//Stores values obtained from sensor
	float xyz_data[3];
	
	// read values from ACC and store in xyz_data array
	LIS3DSH_ReadACC(xyz_data);
	
	x_data = (double)xyz_data[0];
	y_data = (double)xyz_data[1];
	z_data = (double)xyz_data[2];
	
	// use Least Square method to calibrate values 
	x_data = 0.9645*x_data - 0.0136*y_data + 0.0766*z_data - 14.9427;
	y_data = 0.0630*x_data + 0.9893*y_data + 0.0097*z_data - 16.1191;
	z_data = 0.0104*x_data + 0.0320*y_data + 0.9680*z_data + 10.7952;
	
	//printf ("X: %f, Y: %f, Z: %f\n", x_data, y_data, z_data);
	//printf ("%f, %f, %f\n", x_data, y_data, z_data);
	//printf ("%f\n", x_data);
	
	// filter X, Y, Z using Kalman
	Kalmanfilter_C(x_data, &x_kstate);
	x_data = x_kstate.x;
	Kalmanfilter_C(y_data, &y_kstate);
	y_data = y_kstate.x;
	Kalmanfilter_C(z_data, &z_kstate);
	z_data = z_kstate.x;
}

double get_pitch() {
	MEMS_read_value ();
	//Tri-axis tilt sensing method for calculation pitch and roll
	pitch = (180/PI) * atan2(x_data,z_data);//((180.0/PI)*atan(x_data/(sqrt((y_data*y_data)+(z_data*z_data)))));
	
	//printf ("pitch: %f\n", pitch);
	return pitch;
		
}

double get_roll() {
	
	MEMS_read_value ();
	//Tri-axis tilt sensing method for calculation pitch and roll
	roll = (180/PI) * atan2(y_data,z_data);//((180.0/PI)*atan(y_data/(sqrt((x_data*x_data)+(z_data*z_data)))));
	
	//printf ("roll: %f\n", roll);
	return roll;
		
}

void angle_thread(void const *argument){
	while(1){
		osSignalWait(0x01, osWaitForever);
		pitch = get_pitch();
		roll = get_roll();
	}
}



