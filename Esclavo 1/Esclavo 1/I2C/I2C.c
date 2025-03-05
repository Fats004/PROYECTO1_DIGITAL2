/*
 * I2C.c
 *
 * Created: 27/02/2025 20:49:56
 *  Author: Luis
 */ 
#include <avr/io.h>
#include <stdint.h>
#include "C:\Users\Juan Luis Monzón\Desktop\UVG\7 semestre\Digital 2\PROYECTO 1\Esclavo 1\Esclavo 1\I2C\I2C.h"

//***************************************************************************************************************
//FUNCION PARA INICIALIZAR I2C MAESTRO
//***************************************************************************************************************

void I2C_MASTER_INIT(unsigned long SCL_CLOCK, uint8_t Prescaler){
	DDRC &= ~((1 << DDC4) | (1 << DDC5));			//PINES I2C COMO ENTRADAS
	
	switch(Prescaler){
		case 1:
			TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
		break;
		case 4:
			TWSR &= ~(1 << TWPS1);
			TWSR |= (1 << TWPS0);
		break;
		case 16:
			TWSR &= ~(1 << TWPS0);
			TWSR |= (1 << TWPS1);
		break;
		case 64:
			TWSR |= (1 << TWPS0) | (1 << TWPS1);
		break;
		default:
			TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
			Prescaler = 1;
		break;
	}
	TWBR = ((F_CPU/SCL_CLOCK)-16)/(2*Prescaler);
	TWCR |= (1 << TWEN);
}
//***************************************************************************************************************
//FUNCION PARA INICIAR COMUNICACION I2C
//***************************************************************************************************************
void I2C_MASTER_START(void){
	uint8_t estado;
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);		//INICIAR CONDICION DE START
	while(!(TWCR & (1 << TWINT)));							//ESPERA A QUE TERMINE LA BANDERA TWINT
}
//***************************************************************************************************************
//FUNCION DE STOP DE COMUNICACION I2C
//***************************************************************************************************************
void I2C_MASTER_STOP(void){
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
}
//***************************************************************************************************************
////FUNCION DE RECEPCION DE DATOS DEL ESCLAVO
//***************************************************************************************************************
uint8_t I2C_MASTER_WRITE(uint8_t dato){
	uint8_t	estado;
	
	TWDR = dato;
	TWCR = (1 << TWEN) | (1 << TWINT);
	
	while(!(TWCR & (1 << TWINT)));
	estado = TWSR & 0xF8;
	
	if(estado == 0x18 || estado == 0x28 || estado == 0x40){
		return 1;
	}else{
		return estado;
	}
}
//***************************************************************************************************************
//FUNCION DE TRANSMISION DE DATOS DEL ESCLAVO AL MAESTRO
//***************************************************************************************************************
uint8_t I2C_MASTER_READ(uint8_t *buffer, uint8_t ack){
	uint8_t estado;
	if(ack){
		TWCR |= (1 << TWEA);					//LECTURA CON ACK
	}else{
		TWCR &= ~(1 << TWEA);					//LECTURA SIN ACK
	}
	
	TWCR |= (1 << TWINT);						//INICIAMOS LA LECTURA
	while(!(TWCR & (1 << TWINT)));				//ESPERAMOS LA BANDERA
	estado = TWSR & 0xF8;						//VERIFICAR ESTADO
	//VERIFICAR DATO LEIDO CON ACK O SIN ACK
	if(estado == 0x58 || estado == 0x50){
		*buffer = TWDR;
		return 1;
	}else{
		return estado;
	}
}
//***************************************************************************************************************
//FUNCION PARA INICIALIZAR ESCLAVO
//***************************************************************************************************************
void I2C_SLAVE_INIT(uint8_t address){
	DDRC &= ~((1 << DDC4) | (1 << DDC5));		//PINES DE I2C COMO ENTRADAS
	
	TWAR = address << 1;						//SE ASIGNA LA DIRECCIÓN CORRESPONDIENTE
	
	//SE HABILITA LA INTERFAZ, ACK AUTOMATICO, SE HABILITA LA ISR
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);	
}