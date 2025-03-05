/*
 * PWM0.c
 *
 * Created: 4/17/2024 11:28:01 PM
 *  Author: Fatima
 */ 

#include "PWM0.h"
#include <avr/io.h>
#include <stdint.h>

void setPWM0fastA(uint8_t invertido, uint8_t modo, uint16_t prescaler){
	DDRD |= (1 << DDD6);
	
	TCCR0A = 0;
	TCCR0B = 0;
	
	if (invertido == 1){ // Setear si es invertido o no
		TCCR0A = (1 << COM0A1) | (1 << COM0A0);
	}
	else {
		TCCR0A = (1 << COM0A1);
	}
	
	if (modo == 1){  //Setear el modo FAST deseado
		TCCR0A |= (1 << WGM00) | (1 << WGM01);
		TCCR0B |= (1 << WGM02);
	}
	else {
		TCCR0A |= (1 << WGM00) | (1 << WGM01);
	}
	
	switch (prescaler){ //Setear el prescaler deseado
		case 0:
		TCCR0B |= (1 << CS00);
		break;
		case 8:
		TCCR0B |= (1 << CS01);
		break;
		case 64:
		TCCR0B |= (1 << CS00) | (1 << CS01);
		break;
		case 256:
		TCCR0B |= (1 << CS02);
		break;
		case 1024:
		TCCR0B |= (1 << CS00) | (1 << CS02);
		break;
		
		default:
		TCCR0B |= (1 << CS00);
	}
	
}

void updateDutyA0(uint8_t dutycycle){
	OCR0A = dutycycle; //Actualizar el dutycycle coon el valor deseado
}
