/*
 * Tritium pedal Interface
 * Copyright (c) 2015, Tritium Pty Ltd.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
 *	  in the documentation and/or other materials provided with the distribution.
 *	- Neither the name of Tritium Pty Ltd nor the names of its contributors may be used to endorse or promote products
 *	  derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * - Generates target motor rpm and current setpoints
 * - Inputs:
 *		Pedal A & B (redundant dual outputs from hall sensor pedals)
 *		Regen slider C
 *		Vehicle velocity (motor rpm at present, don't know km/h)
 *		Selected operating mode (neutral, drive, etc)
 * - Outputs:
 *		Motor current setpoint
 *		Motor rpm setpoint
 *		Errors
 *
 */

// Include files
//#include "../mspgcc/msp430/include/msp430x21x2.h"
#include "../include/tri86.h"
#include "../include/pedal.h"

// Public variables
command_variables	command;

extern float motor_rpm;
extern float cruise_setpoint;
extern float battery_voltage;
extern float phase_voltage;
extern float phase_current;


/**************************************************************************************************
 * PUBLIC FUNCTIONS
 *************************************************************************************************/

/*
 * Process analog pedal inputs
 * Basic stuff only at this point: map channel A to 0-100% current, no regen or redundancy
 *
 */
void process_pedal( unsigned int analog_a, unsigned int analog_b, unsigned int analog_c, unsigned char request_regen )
{
	float pedal, regen, rpm;
	static float regen_prev = 0;

	// Error Flag updates
	// Pedal too low
	if (analog_a < PEDAL_ERROR_MIN) command.flags |= FAULT_ACCEL_LOW;
	else command.flags &= ~FAULT_ACCEL_LOW;
	// Pedal too high
	if (analog_a > PEDAL_ERROR_MAX) command.flags |= FAULT_ACCEL_HIGH;
	else command.flags &= ~FAULT_ACCEL_HIGH;
	// Pedal A & B mismatch
	// not implemented...
	// Regen pot too low
	if (analog_c < REGEN_ERROR_MIN) command.flags |= FAULT_REGEN_LOW;
	else command.flags &= ~FAULT_REGEN_LOW;
	// Pedal too high
	if (analog_c > REGEN_ERROR_MAX) command.flags |= FAULT_REGEN_HIGH;
	else command.flags &= ~FAULT_REGEN_HIGH;


	// Run command calculations only if there are no pedal faults detected
	if (command.flags == 0x00)
	{
		float max_regen = 1.0;
		// Scale pedal input to a 0.0 to CURRENT_MAX range
		// Clip lower travel region of pedal input
		if (analog_a > PEDAL_TRAVEL_MIN) pedal = (analog_a - PEDAL_TRAVEL_MIN);
		else pedal = 0.0;
		// Scale pedal input to produce target motor current
		pedal = pedal * CURRENT_MAX / PEDAL_TRAVEL;
		// Check limits and clip upper travel region
		if (pedal > CURRENT_MAX) pedal = CURRENT_MAX;

#ifndef REGEN_SETS_RPM
		// Scale regen input to a 0.0 to REGEN_MAX range
		// Clip lower travel region of regen input
		if (analog_c > REGEN_TRAVEL_MIN) regen = (analog_c - REGEN_TRAVEL_MIN);
		else regen = 0.0;
		// Scale regen input
		regen = regen * REGEN_MAX / REGEN_TRAVEL;
		// Check limits and clip upper travel region
		//if (regen > REGEN_MAX) regen = REGEN_MAX;
		max_regen = battery_voltage * MAX_REGEN_CURRENT / (MAX_PHASE_CURRENT * phase_voltage * 3 * MOTOR_EFFICIENCY);
		//max_regen = 0.5; //388;//125.0 * MAX_REGEN_CURRENT / (MAX_PHASE_CURRENT * 27.0);
		if(regen > max_regen)
		{
			regen = max_regen;
		}
		if(regen > 1.0) {regen = 1.0;}
		if(regen < 0.0) {regen = 0.0;}
		if(regen > regen_prev + 0.005){
			regen = regen_prev + 0.005;
		}
		regen_prev = regen;
		rpm = 0.0;
#else
		// Scale regen input to a 0.0 to RPM_MAX (direction dependent) range
		// Clip lower travel region of regen input
		if (analog_c > REGEN_TRAVEL_MIN) rpm = (analog_c - REGEN_TRAVEL_MIN);
		else rpm = 0.0;
		// Check direction and scale appropriately
		if (command.state == MODE_R)
		{
			rpm = rpm * RPM_REV_MAX / REGEN_TRAVEL;
			if (rpm < RPM_REV_MAX) rpm = RPM_REV_MAX;
		}
		else
		{
			rpm = rpm * RPM_FWD_MAX / REGEN_TRAVEL;
			if (rpm > RPM_FWD_MAX) rpm = RPM_FWD_MAX;
		}
		regen = 0.0;
#endif

#ifndef REGEN_SETS_RPM
		// Choose target motor velocity
		switch(command.state)
		{
			case MODE_R:
				//if ( request_regen == FALSE )
				if ( regen <= 0.0 )
				{
					command.current = pedal;
					command.rpm = RPM_REV_MAX;
				}
				else
				{
					command.current = regen;
					command.rpm = 0.0;
				}
				break;
			case MODE_DL:
				//if ( request_regen == FALSE )
				if ( regen <= 0.0 )
				{
					command.current = pedal;
					command.rpm = RPM_FWD_MAX;
				}
				else
				{
					command.current = regen;
					command.rpm = 0.0;
				}
				break;
			case MODE_DH:
			case MODE_BL: //CRUISE
				if (regen <= 0.0)
				{
					if (pedal <= 0.1)
					{
						if (motor_rpm < cruise_setpoint)
						{
							//the go faster part, also check to make sure you don't accidentally the whole current
							if ((K_P * (cruise_setpoint - motor_rpm) + phase_current) <= CURRENT_MAX)
							{
								command.current = K_P * (cruise_setpoint - motor_rpm) + phase_current;
								command.rpm = motor_rpm + (cruise_setpoint - motor_rpm);
							}
							else
							{
								command.current = CURRENT_MAX;
								command.rpm = RPM_FWD_MAX;
							}
						}
						// Slow down a little
						// motor_rpm > cruise_setpoint
						else
						{
							command.current = K_P * (motor_rpm - cruise_setpoint) + phase_current;
							command.rpm = motor_rpm - (motor_rpm - cruise_setpoint);
						}
					}
					else
					{
						//pedal is down so you don't have a care in the world
						command.current = pedal;
						command.rpm = RPM_FWD_MAX;
					}
				}
				else
				{
					command.current = regen;
				command.rpm = 0.0;
				}
				// K_P needs to be defined as 0.9 somewhere as well
				break;
			case MODE_BH:
			case MODE_CHARGE:
			case MODE_N:
			case MODE_START:
			case MODE_ON:
			case MODE_OFF:
			default:
				command.current = 0.0;
				command.rpm = 0.0;
				break;
		}
	}
#else
		// Choose target motor velocity
		switch(command.state)
		{
			case MODE_R:
			case MODE_DL:
			case MODE_DH:
			case MODE_BL:
			case MODE_BH:
				command.current = pedal;
				command.rpm = rpm;
				break;
			case MODE_CHARGE:
			case MODE_N:
			case MODE_START:
			case MODE_ON:
			case MODE_OFF:
			default:
				command.current = 0.0;
				command.rpm = 0.0;
				break;
		}
	}
#endif
	// There was a pedal fault detected
	else
	{
		command.current = 0.0;
		command.rpm = 0.0;
	}

}
