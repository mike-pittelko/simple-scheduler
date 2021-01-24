/*
 *
 *
  task-schedule.c

	Implements a very simple task scheduler, based on input of a count of milliseconds

  Copyright (c) 2020 Mike Pittelko

  This is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "task-schedule.h"

static const int MultiplyDeBruijnBitPosition[32] =
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

uint32_t Scheduler_CountOfZeros = 0;


// Returns number of trailing zeros in 5 operations (that doesn't necessarily mean 5 isntructions).
// I did say it was black magic.
// "Using de Bruijn Sequences to Index 1 in a Computer Word" by Charles E. Leiserson, Harald Prokof, and Keith H. Randall

uint16_t CountZerosFromLSB(uint32_t v)
{
	return  MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];
}


void (*Tasks[]) (void) = {
		0,			       // Never called
		TASK_2MS,          // Bit 1
		TASK_4MS,          // Bit 2
		TASK_8MS, 	       // Bit 3
		TASK_16MS,         // Bit 4
		TASK_32MS,         // Bit 5
		TASK_64MS,         // Bit 6
		TASK_128MS,        // Bit 7
		TASK_2562MS,	   // Bit 8
		TASK_512MS,	       // Bit 9
		TASK_1024MS,	   // Bit 10
		TASK_2048MS,	   // Bit 11
		TASK_4096MS,	   // Bit 12
		TASK_8192MS,	   // Bit 13
		TASK_16384MS,	   // Bit 14
		TASK_32768MS,	   // Bit 15
		TASK_65536MS,	   // Bit 16
		TASK_131072MS,	   // Bit 17
		TASK_262144MS,	   // Bit 18
		TASK_528288MS,	   // Bit 19
		TASK_1056576MS,    // Bit 20
		TASK_2113152MS,    // Bit 21
		TASK_4226304MS,    // Bit 22
		TASK_8452608MS,    // Bit 23
		TASK_16905216MS,   // Bit 24
		TASK_33810432MS,   // Bit 25
		TASK_67620864MS,   // Bit 26
		TASK_135241728MS   // Bit 27.  Can be extended to all 32 bits (or more), but for
		                   //          general purposes, 38 days is probably sufficient.

};


// Used to ensure the top bits we won't use are always 1
#define TASK_MASK 0xF0000000


uint32_t stm32_atomic_swap(uint32_t *dest, uint32_t new_value)
{
  uint32_t old_value;
  do
  {
    old_value = __LDREXW(dest);
  } while (__STREXW(new_value,dest));
  return old_value;
}


// Given time in ms, compute the number of zeros given the current time.
// Happens in interrupt context.
void TaskSignals(uint32_t TimeMs)
{
	Scheduler_CountOfZeros = CountZerosFromLSB((TimeMs|TASK_MASK));  // Note, uint32_t is inherently atomic on arm
}

// call tasks on (potentially sloppy) powers of
// 2 ms (2,4,8,16,32,64,128,256,512,1024,2048,4096...)
// On a given slot, will call the maximum slot, then in descending order,
// For example, at 16ms, the 16ms slot, the 8ms slot, the 4ms slot, and 2ms slot will be called in
// that order.
// THIS IS A COOPERATIVE SCHEDULER, NOT A PREEMTIVE ONE.
// Returns number tasks that were called.

uint32_t MainLoopTaskProcessor(void)
{
	uint32_t CountOfZeros = 0;
	uint32_t TasksThatRan = 0;

	CountOfZeros = stm32_atomic_swap(&Scheduler_CountOfZeros,0);  // By clearing the count, if function is called
															// before we get the next tick, will keep from
															// running tasks again.

	if ((CountOfZeros != 0) & (CountOfZeros <= 27))	// Safety - the task signals TASK_MASK is set for 28 bits., and don't bother going further if count == 0
		{
			while (CountOfZeros != 0)		  // Only process if at least 1 zero exists
			{
				if (Tasks[CountOfZeros] != 0) // If a task is defined
				{
					TasksThatRan++;
					Tasks[CountOfZeros]();	  // Call the task
				}
				CountOfZeros--;				  // Decrease the index
			}
		}
	return TasksThatRan;
}
