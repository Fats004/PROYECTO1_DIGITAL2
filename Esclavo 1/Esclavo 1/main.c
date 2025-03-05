/*
 * Esclavo 1.c
 *
 * Created: 27/02/2025 20:47:48
 * Author : Luis
 */ 
//ESCLAVO DETECTOR DE HUMO, BUZZER Y MOTOR DC
//*************************************************************************************************
//LIBRERIAS
//*************************************************************************************************
#define F_CPU 16000000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "C:\Users\Juan Luis Monzón\Desktop\UVG\7 semestre\Digital 2\PROYECTO 1\Esclavo 1\Esclavo 1\ADC\ADC.h"
#include "C:\Users\Juan Luis Monzón\Desktop\UVG\7 semestre\Digital 2\PROYECTO 1\Esclavo 1\Esclavo 1\I2C\I2C.h"

//*************************************************************************************************
//VARIABLES GLOBALES
//*************************************************************************************************
#define SlaveAddress 0x40
uint8_t HUMO = 0;
uint8_t buffer = 0;
uint8_t ventilador = 0;

//*************************************************************************************************
//MAIN
//*************************************************************************************************
int main(void){
//*************************************************ADC*********************************************
	setADC(externo, izquierda, 7);
	controlADC(1, 0, 1, 128);
//************************************************SETUP********************************************
	setup_pines();
//*************************************************I2C*********************************************
	I2C_SLAVE_INIT(SlaveAddress);
	
//********************************************INTERRUPCIONES***************************************
	TWCR |= (1 << TWIE);
	sei();

    while (1) 
    {
		ADCSRA |= (1 << ADSC);
//**********************************************BUZZER*********************************************		
		if(buffer == 'H'){
			PORTD |= 0b00001000;
			buffer = 0;
		}
		if(buffer == 'h'){
			PORTD &= 0b11110111;
			buffer = 0;
		}
//*******************************************VENTILADOR********************************************
		if(buffer == 'V'){
			PORTD |= 0b00010000;
			buffer = 0;
		}
		if(buffer == 'v'){
			PORTD &= 0b11101111;
			buffer = 0;
		}
				
    }
}
//*************************************************************************************************
//INTERRUPCIONES
//*************************************************************************************************
ISR(ADC_vect){
	HUMO = ADCH;
	
	ADCSRA |= (1 << ADIF); //Apagar la bandera de ADC
}
ISR(TWI_vect){
	uint8_t estado;
	estado = TWSR & 0xFC;
	switch(estado){
		case 0x60:
		case 0x70:
		TWCR |= (1 << TWINT);
		break;
		case 0x80:
		case 0x90:
		buffer = TWDR;
		TWCR |= (1 << TWINT);
		break;
		case 0xA8:
		case 0xB8:
		TWDR = HUMO;
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		break;
		default:
		TWCR |= (1 << TWINT) | (1 << TWSTO);
		break;
	}
}
//*************************************************************************************************
//FUNCIONES
//*************************************************************************************************
void setup_pines(){
	//CONFIGURAR PINES DE SALIDA
	DDRD |= (1 << DDD3) | (1 << DDD4);			//PD3-PD4 COMO SALIDA (BUZZER Y DC)
}