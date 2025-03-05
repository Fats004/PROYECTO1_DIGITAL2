/*
 * ADC.h
 *
 * Created: 2/27/2025 7:34:06 PM
 *  Author: Fatima
 */ 

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <stdint.h>

#define voltajeoff 0
#define externo 1
#define interno 2

#define izquierda 1
#define derecha 0

void setADC(uint8_t voltaje, uint8_t justificacion, uint8_t canal);

void controlADC(uint8_t enable, uint8_t trigger, uint8_t interrupcion, uint8_t prescaler);

#endif /* ADC_H_ */