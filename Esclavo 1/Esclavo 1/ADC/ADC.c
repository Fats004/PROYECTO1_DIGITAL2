/*
 * ADC.c
 *
 * Created: 27/02/2025 20:54:30
 *  Author: Luis
 */ 
#include "C:\Users\Juan Luis Monzón\Desktop\UVG\7 semestre\Digital 2\PROYECTO 1\Esclavo 1\Esclavo 1\ADC\ADC.h"
#include <avr/io.h>
#include <stdint.h>

void setADC(uint8_t voltaje, uint8_t justificacion, uint8_t canal){
	if(voltaje == 0){ // sETEAR EL TIPO DE VOLTAJE DE REFERENCIA PARA EL ADC
		ADMUX &= ~((1 << REFS0) | (1 << REFS1));
	}
	else if(voltaje == 1){
		ADMUX &= ~(1 << REFS1);
		ADMUX |= (1 << REFS0);
	}
	else{
		ADMUX |= (1 << REFS0) | (1 << REFS1);
	}
	
	
	if(justificacion == 1){ //Setear la justificacion deseada
		ADMUX |= (1 << ADLAR);
	}
	else{
		ADMUX &= ~(1 << ADLAR);
	}
	
	ADMUX = (ADMUX & 0xF0) | canal; //Encender el canal deseado
}

void controlADC(uint8_t enable, uint8_t trigger, uint8_t interrupcion, uint8_t prescaler){
	if(enable == 1){
		ADCSRA |= (1 << ADEN);  // Enceder o apagar el ADC
	}
	else{
		ADCSRA &= ~(1 << ADEN);
	}
	
	if(trigger == 1){
		ADCSRA |= (1 << ADATE);  // Seleccionar si se quiere utilizsar el autotrigger o no
	}
	else{
		ADCSRA &= ~(1 << ADATE);
	}
	
	if(interrupcion == 1){ // Seteara la interrupcion
		ADCSRA |= (1 << ADIE);
	}
	else {
		ADCSRA &= ~(1 << ADIE);
	}
	
	switch (prescaler){ // Setear el prescaler deseado
		case 2:
		ADCSRA &= ~((1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2));
		break;
		case 4:
		ADCSRA |= (1 << ADPS1);
		ADCSRA &= ~((1 << ADPS0) | (1 << ADPS2));
		break;
		case 8:
		ADCSRA |= (1 << ADPS0 ) | (1 << ADPS1);
		ADCSRA &= ~(1 << ADPS2);
		break;
		case 16:
		ADCSRA |= (1 << ADPS2);
		ADCSRA &= ~((1 << ADPS0) | (1 << ADPS1));
		break;
		case 32:
		ADCSRA |= (1 << ADPS0 ) | (1 << ADPS2);
		ADCSRA &= ~(1 << ADPS1);
		break;
		case 64:
		ADCSRA |= (1 << ADPS1) | (1 << ADPS2);
		ADCSRA &= ~(1 << ADPS0);
		break;
		case 128:
		ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
		break;
		default:
		ADCSRA &= ~((1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2));
	}
}