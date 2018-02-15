/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "motors.h"
#include "wireless.h"

#include "MEMS.h"
#include "UI.h"
#include "tictactoe.h"
#include "packet.h"

#include <stdio.h>

#define RECVSIG 0x01
#define SENDSIG 0x02
#define NEWDATA_SIG 0x04

uint8_t result;

double pitch;
double roll;

double motor_0_angle;
double motor_1_angle;
double motor_2_angle;
double current_x;
double current_y;
double x_path[array_length] = {0};
double y_path[array_length] = {0};


int board[9];
int direction;
int mode;
int shape;
int send;

// ID for thread
osThreadId path_thread_id;
osThreadId set_xy_thread_id;
osThreadId angle_thread_id;
osThreadId drawBoard_thread_id;

osThreadId RecvData_thread;

osThreadId keypad_thread_id;

void set_xy_thread(void const *argument){
	while(1){
	//	printf("roll: %f\n", roll);
	//	printf("pitch: %f\n", pitch);
	//	printf("x: %f\n", current_x);
	//	printf("y: %f\n", current_y);
		if(mode != TicTacToe){
			double new_x = current_x;
			double new_y = current_y;
			
			if(pitch < - 30) new_x = current_x + 0.2;
			else if(pitch > 30) new_x = current_x - 0.2;
			
			if(roll < - 30) new_y = current_y + 0.2;
			else if(roll > 30) new_y = current_y - 0.2;
			
			goTo (new_x,new_y);
			uint8_t pkt;
			//makeCallbackPkt(&pkt, (int8_t)new_x, (int8_t)new_y, 0);
			//CC2500_Transmit(&pkt, 1);
		}
		osDelay(50);
	}
}

void ReceiveData(void const *argument){
	uint8_t r_buffer;
	uint8_t prev;
	uint8_t not_idle = 1;
	while (1){
		if (not_idle) {
			// start a new receive cycle
			prev = r_buffer;
			CC2500_Change_State (SRX);
			osSignalWait(RECVSIG, osWaitForever);
		}
		uint8_t status = status_state(CC2500_Strobe(SNOP));
		if (status != IDLE_STATE) {
			// status is not idle, have not recieved data
			not_idle = 0;
			osDelay(100);
		} else {
			// status is idle, received data
			not_idle = 1;
		}
		if (not_idle) {
			// received data, read from FIFO
			CC2500_ReadRecvBuffer(&r_buffer);
			if (r_buffer != prev && status != RX_STATE) {
				// processing for non-repeated data
				
				printf("read: 0x%02x\n", r_buffer);
				result=r_buffer & 0x1f;
				printf("result: %d\n", result);
				osSignalSet(keypad_thread_id, NEWDATA_SIG);
			}
		}
	}
}


// After moving keypad to the other board, use this thread the collect data from the wireless!
void keypad_thread(void const *argument){
	while(1){
		// wait for a signal from wireless instead of reading keypad~
		osSignalWait(NEWDATA_SIG, osWaitForever);
		Keypad_read();
		
		if (send == 1){
			if (shape == 0 && direction == RESET){ // move to bottom left corner
				upDown(up);
				osDelay(500);
				goTo(-7, 6);
				osDelay(500);
			}
			//printf("mode: %i \nshape: %i\ndirection: %i\n", mode, shape, direction);
			if(mode == TicTacToe){
				if (shape == BOARD){
					osSignalSet(drawBoard_thread_id, 0x01);
					osDelay(19000);
					
					board_init(board);

					int move = AI(board);
					drawX(move);
					osSignalSet (path_thread_id, 0x01);
					osDelay(2000);
				} else if(checkWin(board) == 9) {//if (shape == O){
					printf("draw O...\n");
					printf("direction: %d\n", direction);
					if(board[direction-1] == 0){
						drawO(direction);
						board[direction-1] = 1;
						osSignalSet (path_thread_id, 0x01);
						osDelay(7000);
						int move = AI(board);
						printf("move: %d\n", move);
						for(int a = 0; a < 9; a++){
							printf("%d ", board[a]);
						}
						printf("\n");
						if(move != 0){
							printf("draw X...\n");
							drawX(move);
							osSignalSet (path_thread_id, 0x01);
							int win = checkWin(board);
							if(win < 3){
								osDelay(5000);
								goTo(-4, 11 - (win * 2));
								osDelay(500);
								upDown(down);
								for(float i=0; i<=4; i=i+0.1){
									goTo(-4+i, 11 - (win * 2));
									//printf("print something...\n");
									osDelay(50);
								}
								upDown(up);
							}
							else if(win < 6){
								osDelay(5000);
								goTo(-4 + ((win - 3)*2), 11);
								osDelay(500);
								upDown(down);
								for(float i=0; i<=4; i=i+0.1){
									goTo(-4 + ((win - 3)*2), 11 - i);
									osDelay(50);
								}
								upDown(up);
							}
							else if(win == 6){
								osDelay(5000);
								goTo(-4, 11);
								osDelay(500);
								upDown(down);
								for(float i=0; i<=5; i=i+0.1){
									goTo(-4 + i , 11 - i);
									osDelay(50);
								}
								upDown(up);
							}
							else if(win == 7){
								osDelay(5000);
								goTo(0, 11);
								osDelay(500);
								upDown(down);
								for(float i=0; i<=5; i=i+0.1){
									goTo(0 - i , 11 - i);
									osDelay(50);
								}
								upDown(up);
							}
						}
					}
				}
				
			
			}else if(direction != RESET){
				
				if (shape==1) drawSquare(current_x,current_y);
				if (shape==2) drawRectangle(current_x,current_y);
				if (shape==3) drawTriangle(current_x,current_y);
				
				if (shape ==0){
					drawSegment(current_x, current_y, direction);
				}
				
			osSignalSet (path_thread_id, 0x01);
			}
			
			
		}
		osDelay(50);
	}
}



osThreadDef(path_thread, osPriorityNormal, 1, 0);
osThreadDef(set_xy_thread, osPriorityNormal, 1, 0);
osThreadDef(angle_thread, osPriorityNormal, 1, 0);
osThreadDef(drawBoard_thread, osPriorityNormal, 1, 0);

osThreadDef(ReceiveData, osPriorityNormal, 1, 0);

osThreadDef(keypad_thread, osPriorityNormal, 1, 0);


/*
 * main: initialize and start the system
 */
int main (void) {
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
  // initialize peripherals here
	motors_init();
	MEMS_config();
	MEMS_interrupt_config();
	upDown(up);
	
	CC2500_Init();
	
//	while(1){
//		u8 ReadValue;
//		ReadValue = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);
//		printf("%d\n", ReadValue);
//	}


	path_thread_id = osThreadCreate(osThread(path_thread), NULL);
	keypad_thread_id = osThreadCreate(osThread(keypad_thread), NULL);
	set_xy_thread_id = osThreadCreate(osThread(set_xy_thread), NULL);
	angle_thread_id = osThreadCreate(osThread(angle_thread), NULL);
	RecvData_thread = osThreadCreate(osThread(ReceiveData), NULL);
	drawBoard_thread_id = osThreadCreate(osThread(drawBoard_thread), NULL);

	osKernelStart ();                         // start thread execution 
}

void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		// clear the flag in the sensor's end
		EXTI_ClearITPendingBit(EXTI_Line0);
		osSignalSet(angle_thread_id, 0x01);
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
		osSignalSet (RecvData_thread, RECVSIG);
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}

