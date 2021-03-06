/*
 * stepper.h
 *
 *  Created on: Jul 5, 2021
 *      Author: janpo
 */

#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "main.h"
#include "tim.h"
#include "usart.h"

#define TIM_MTR htim7

typedef struct SM_Param
{
	uint16_t speed;
	uint16_t accel;
	uint16_t decel;
	int16_t steps;

}SM_Param;

/*! \brief Holding data used by timer interrupt for speed ramp calculation.
 *
 *  Contains data used by timer interrupt to calculate speed profile.
 *  Data is written to it by move(), when stepper motor is moving (timer
 *  interrupt running) data is read/updated when calculating a new step_delay
 */
typedef struct speedRampData{
  //! What part of the speed ramp we are in.
  uint8_t run_state;
  //! Direction stepper motor should move.
  uint8_t dir;
  //! Peroid of next timer delay. At start this value set the accelration rate.
  uint16_t step_delay;
  //! What step_pos to start decelaration
  uint16_t decel_start;
  //! Sets deceleration rate.
  int16_t decel_val;
  //! Minimum time delay (max speed)
  int16_t min_delay;
  //! Counter used when accelerateing/decelerateing to calculate step_delay.
  int16_t accel_count;
  int16_t step_count;
  int16_t step_total;
  int8_t step_pos;
  uint8_t state;

  uint16_t last_accel_delay;
  uint16_t last_step_delay;
  // Counting steps when moving.
  // Keep track of remainder from new_step-delay calculation to incrase accurancy
  uint16_t rest;

} speedRampData;


// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

#define STEP_PER_REV 4096
#define CW 0
#define CCW 1

/*
 * Predefined function/ variable
 */
#define T0_FREQ 1000000
#define MTR_PORT GPIOE



// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA 		((2*3.14159)/STEP_PER_REV)                  		// 2*pi/spr
#define A_T_x100 	((uint32_t)(ALPHA*T0_FREQ*100))     	// (ALPHA / T0_FREQ)*100
#define T0_FREQ_COR ((uint16_t)((T0_FREQ*0.676)/100)) 		// divided by 100 and scaled by 0.676
#define A_SQ		((uint32_t)(ALPHA*2*10000000000))       // ALPHA*2*10000000000
#define A_x20000 		((uint32_t)(ALPHA*20000))              	// ALPHA*200


//uint8_t ibuf[10]={0};
//uint16_t c=0;
//void MTR_ISR(void);
void mtr_Output(uint8_t pos);
void f_updateDelay(speedRampData *data);

//static uint32_t sqrt(uint32_t x);
uint16_t min(uint16_t x, uint16_t y);
void updateStepper(speedRampData *data);
void runStepper(speedRampData *data, SM_Param *param);
void mtr_Output(uint8_t pos);

#endif /* INC_STEPPER_H_ */
