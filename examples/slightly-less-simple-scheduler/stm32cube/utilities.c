/*
 * utilities.c
 *
 *  Created on: Jan 26, 2020
 *      Author: mikep
 */

#include "main.h"
#include "stdio.h"
#include "stdarg.h"

#include "utilities.h"

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;



// This maps normal print output to the serial tx/rx (in this case, uart2)
int __io_putchar(int ch) {
    // Code to write character 'ch' on the UART

#ifdef USE_CRLF_TRANSLATION
	switch (ch)
	{
		case '\n' : { HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2 , 1000); break;	};			// cr/lf translation
		default:  {HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1 , 1000); break; 	};
	}
#else
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1 , 1000);
#endif
	return 1;
}


int __io_getchar(void) {
    // Code to read a character from the UART

	uint8_t ch8;

	HAL_UART_Receive(&huart2,&ch8,1,HAL_MAX_DELAY);

	return ch8;
}


void Task256ms(void) 		// Called every 256ms
{

	HAL_GPIO_TogglePin(LEDHB_GPIO_Port, LEDHB_Pin);
//	printf("256ms\n");

}

int count = 0;

void Task1024ms(void) 		// Called every 1.024s
{
	printf("1s Task Ran\n");
};



