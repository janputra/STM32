/*
 * stepper.c
 *
 *  Created on: Jul 5, 2021
 *      Author: janpo
 */
#include "stepper.h"
#include <math.h>


static uint8_t MTR_OUT_TAB[8] = { 0b1000,
							      0b1100,
								  0b0100,
								  0b0110,
								  0b0010,
								  0b0011,
								  0b0001,
								  0b1001};




void step_Counter(speedRampData *data)
{
	data->step_count++;


	if (data->dir==CW)
	{
		data->step_pos++;


		if (data->step_pos==8){
				data->step_pos=0;
			}
	}
	else if (data->dir==CCW)
	{
		data->step_pos--;


		if (data->step_pos==-1){
				data->step_pos=8;
			}
	}



	mtr_Output(data->step_pos);
}

void mtr_Output(uint8_t pos)
{
	uint16_t mask = SM_IN1_Pin|SM_IN2_Pin|SM_IN3_Pin|SM_IN4_Pin;
	uint32_t temp = (MTR_OUT_TAB[pos]<<2) & mask;
	temp |= (((~MTR_OUT_TAB[pos])<<2) & mask)<<16;
	MTR_PORT->BSRR=temp;


}


void runStepper(speedRampData *data, SM_Param *param)
{
	//! Number of steps before we hit max speed.
	  uint16_t max_s_lim;
	  //! Number of steps before we must start deceleration (if accel does not hit max speed).
	  uint16_t accel_lim;
	  uint16_t speed = param->speed;
	  uint16_t accel = param->accel;
	  uint16_t decel = param->decel;
	  int16_t step = param->steps;
	  // Set direction from sign on step value.
	  if(step < 0){
	    data->dir=CCW;
		step = -step;
	  }
	  else{
		data->dir=CW;

	  }

	  // If moving only 1 step.
	  if(step == 1){
	    // Move one step...
	    data->accel_count=-1;
		// ...in DECEL state.
	    data->state=DECEL;
	    // Just a short delay so main() can act on 'running'.
	    data->step_delay=1000;

	    // Run Timer/Counter 1 with prescaler = 8.

	  }
	  // Only move if number of steps to move is not zero.
	  else if(step != 0)
	  {
	    // Refer to documentation for detailed information about these calculations.

	    // Set max speed limit, by calc min_delay to use in timer.
	    data->min_delay = A_T_x100 / speed;

	    // Set accelration by calc the first (c0) step delay .
	     data->step_delay = (T0_FREQ_COR * sqrt(A_SQ / accel))/100;

	    // Find out after how many steps does the speed hit the max speed limit.
	    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
	    // If we hit max speed limit before 0,5 step it will round to 0.
	    // But in practice we need to move atleast 1 step to get any speed at all.
	    if(max_s_lim == 0){
	      max_s_lim = 1;
	    }

	    // Find out after how many steps we must start deceleration.
	    accel_lim = ((long)step*decel) / (accel+decel);
	    // We must accelrate at least 1 step before we can start deceleration.
	    if(accel_lim == 0){
	      accel_lim = 1;
	    }

	    // Use the limit we hit first to calc decel.
	    if(accel_lim <= max_s_lim){
	      data->decel_val = accel_lim - step;
	    }
	    else{
	      data->decel_val = -((long)max_s_lim*accel)/decel;
	    }
	    // We must decelrate at least 1 step to stop.
	    if(data->decel_val == 0){
	      data->decel_val = -1;
	    }

	    // Find step to start decleration.
	    data->decel_start = step + data->decel_val;

	    // If the maximum speed is so low that we dont need to go via accelration state.
	    if(data->step_delay <= data->min_delay){
	    	data->step_delay = data->min_delay;
	    	data->state = RUN;
	    }
	    else{
	    	data->state = ACCEL;
	    }

	    // Reset counter.
	    data->accel_count = 0;


	  }

	  TIM_MTR.Instance->ARR= data->step_delay-1;
	  HAL_TIM_Base_Start_IT(&TIM_MTR);


}

void updateStepper(speedRampData *data)
{

	// Holds next delay period.
	// Remember the last step delay used when accelrating.


		//printf("ok!\n");
	  //OCR1A = srd.step_delay;

		if (data->last_step_delay!=data->step_delay){
			TIM_MTR.Instance->ARR= data->step_delay-1;
			data->last_step_delay=data->step_delay;
		}



	  switch(data->state) {
	    case STOP:
	      data->step_count=0;
	      data->rest = 0;
	      data->step_delay =0;

	      HAL_TIM_Base_Stop_IT(&TIM_MTR);

	      break;

	    case ACCEL:
	      step_Counter(data);
	      data->accel_count++;

	      f_updateDelay(data);
	      // Chech if we should start decelration.
	      if(data->step_count >= data->decel_start) {
	    	  data->accel_count = data->decel_val;
	        data->state = DECEL;
	      }
	      // Chech if we hitted max speed.
	      else if(data->step_delay <= data->min_delay) {
	        data->last_accel_delay = data->step_delay;
	        //data->step_delay = data->step_delay;
	        data->rest = 0;
	        data->state = RUN;
	      }
	      break;

	    case RUN:
	      step_Counter(data);
	      //new_step_delay = srd.min_delay;
	      // Chech if we should start decelration.
	      if(data->step_count >= data->decel_start) {
	        data->accel_count = data->decel_val;
	        // Start decelration with same delay as accel ended with.
	        data->step_delay= data->last_accel_delay;
	        data->state = DECEL;
	      }
	      break;

	    case DECEL:
	      step_Counter(data);
	      data->accel_count++;
	      f_updateDelay(data);
	      // Check if we at last step
	      if(data->accel_count >= 0){
	        data->state = STOP;
	      }
	      break;
	  }


}


void f_updateDelay(speedRampData *data)
{
	uint16_t new_step_delay;
	uint16_t new_rest;

	new_step_delay = (((2 * (long)data->step_delay) + data->rest)/(4 * data->accel_count + 1));
	new_step_delay = data->step_delay- new_step_delay;
	new_rest = ((2 * (long)data->step_delay)+data->rest)%(4 * data->accel_count + 1);

	data->step_delay= new_step_delay;
	data->rest = new_rest;

}



//}

/*! \brief Square root routine.
 *
 * sqrt routine 'grupe', from comp.sys.ibm.pc.programmer
 * Subject: Summary: SQRT(int) algorithm (with profiling)
 *    From: warwick@cs.uq.oz.au (Warwick Allison)
 *    Date: Tue Oct 8 09:16:35 1991
 *
 *  \param x  Value to find square root of.
 *  \return  Square root of x.
 */
//static uint32_t sqrt(uint32_t x)
//{
//  register uint32_t xr;  // result register
//  register uint32_t q2;  // scan-bit register
//  register uint8_t  f;   // flag (one bit)
//
//  xr = 0;                     // clear result
//  q2 = 0x40000000L;           // higest possible result bit
//  do
//  {
//    if((xr + q2) <= x)
//    {
//      x -= xr + q2;
//      f = 1;                  // set flag
//    }
//    else{
//      f = 0;                  // clear flag
//    }
//    xr >>= 1;
//    if(f){
//      xr += q2;               // test flag
//    }
//  } while(q2 >>= 2);          // shift twice
//  if(xr < x){
//    return xr +1;             // add for rounding
//  }
//  else{
//    return xr;
//  }
//}

/*! \brief Find minimum value.
 *
 *  Returns the smallest value.
 *
 *  \return  Min(x,y).
 */
//uint16_t min(uint16_t x, uint16_t y)
//{
//  if(x < y){
//    return x;
//  }
//  else{
//    return y;
//  }
//}
