/*
 * PWM0.h
 *
 * Created: 4/17/2024 4:50:44 PM
 *  Author: Fatima
 */ 


#ifndef PWM0_H_
#define PWM0_H_


#include <avr/io.h>
#include <stdint.h>

#define _FF0 0
#define _OCR0 1

void setPWM0fastA(uint8_t invertido, uint8_t modo, uint16_t prescaler);

void updateDutyA0(uint8_t dutycycle);


#endif /* PWM0_H_ */