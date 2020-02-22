/*
 * utilities.h
 *
 *  Created on: Jan 26, 2020
 *      Author: mikep
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#define USE_CRLF_TRANSLATION

// This maps normal print output to the serial tx/rx (in this case, uart2)
int __io_putchar(int ch);

int __io_getchar(void);


void Task256ms(void);

void Task1024ms(void);


#endif /* UTILITIES_H_ */
