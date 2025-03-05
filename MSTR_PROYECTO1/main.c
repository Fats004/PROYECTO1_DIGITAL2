/// Universidad del Valle de Guatemala
// Electrónica Digital 2
// main.c
// Proyecto: Lab2
// Hardware: ATMEGA328P
// Created: 2/23/2025
// Author : Fatima Urrutia
//********************************************************************************************************************************************

//********************************************************************************************************************************************
// Librerias
//********************************************************************************************************************************************

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "LCD/LCD.h"
#include "I2C/I2C.h"
#include "ADC/ADC.h"

#define slavetemp 0x38
#define slavehumo 0x40
#define slavelluvia 0x30

//********************************************************************************************************************************************
// Variables Globales
//********************************************************************************************************************************************

uint8_t temp;
uint8_t buffer;
uint8_t status;
uint8_t modo = 0;

float CH;
float concent;
float H2O;
float temperatura;

uint8_t valorTYH[6];
uint8_t valorH[1];
uint8_t valorl[1];

volatile char recibido;
char buf[20];
char uartbuf[4];
//uint8_t AWA;

//********************************************************************************************************************************************
// Caracteres especiales
//********************************************************************************************************************************************

char sun1[8] = {
	0b00000,
	0b00000,
	0b00001,
	0b10001,
	0b01000,
	0b00011,
	0b00111,
	0b10111
};

char sun2[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00010,
	0b00100,
	0b10000,
	0b11000,
	0b11010
};

char sun3[8] = {
	0b00111,
	0b00011,
	0b01000,
	0b10001,
	0b00001,
	0b00000,
	0b00000,
	0b00000
};

char sun4[8] = {
	0b11000,
	0b10000,
	0b00100,
	0b00010,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

char rain1[8] = {
	0b00000,
	0b00000,
	0b00100,
	0b01110,
	0b11111,
	0b11111,
	0b01110,
	0b00000
};

char rain2[8] = {
	0b00100,
	0b01110,
	0b11111,
	0b11111,
	0b01110,
	0b00000,
	0b00000,
	0b00000
};

char rain3[8] = {
	0b11111,
	0b01111,
	0b00011,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

char rain4[8] = {
	0b11110,
	0b11100,
	0b10000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

//********************************************************************************************************************************************
// Declarar Funciones
//********************************************************************************************************************************************

void setup(void);
void floatToString(float num, char *str, int precision);
void initUART9600(void);
void textUART(char *texto);

void sun(void);
void rain(void);

void I2Ctemp(void);
void I2Chumo(void);
void I2Clluvia(void);

//********************************************************************************************************************************************
// Main
//********************************************************************************************************************************************

int main(void)
{
    setup();
    cursor(1,1);
    WriteS("T:");
    
    cursor(1,2);
    WriteS("H:");
	
	cursor(12,1);
	WriteC('|');
	cursor(12,2);
	WriteC('|');
	
	sun();
	
    while (1) 
    {
		//ADCSRA |= (1 << ADSC);
		
		/*floatToString(AWA, buf, 2);
		cursor(8,1);
		WriteS(buf);*/
		
		I2Ctemp();
		
		uint32_t rawTemp = ((uint32_t)(valorTYH[3] & 0x0F) << 16) | ((uint32_t)valorTYH[4] << 8) | valorTYH[5]; // Aislar solo el valor de la temperatura
		temperatura = (rawTemp * 200.0 / 1048576.0) - 50.0;  // Fórmula de conversión según datasheet

		floatToString(temperatura, buf, 1);
		cursor(3,1);
		WriteS(buf);
		cursor(7,1);
		WriteS("C");
		
		textUART("T"); // Enviar identificador de temperatura
		textUART(buf);
		textUART("\r\n");
		
		I2Chumo();
		uint8_t humo = valorH[0]; //humo es mi Rs
		// Ro es 5k
		CH = (pow(50.0 / (5.0 / humo), 2))/1000; //Calculo de concentracion de humo
		
		floatToString(CH, buf, 2);
		cursor(3,2);
		WriteS(buf);
		cursor(7,2);
		WriteS("kppm");
		
		textUART("B"); // Enviar identificador de humo
		textUART(buf);
		textUART("\r\n");
		
		I2Clluvia();
		H2O = valorl[0];
		textUART("S");
		if (H2O < 200){
			rain();
			textUART("w:rain");
			textUART("\r\n");
		}
		else if (H2O > 240){
			sun();
			textUART("w:day-sunny");
			textUART("\r\n");
		}
		
    }
}

//********************************************************************************************************************************************
// Setup
//********************************************************************************************************************************************

void setup(void){
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);  //Setear las salidas en puerto d
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) |(1 << DDB5);
	
	//----------------------- LCD ------------------------------
	
	initLCD_8bit();
	
	//----------------------- I2C ------------------------------
	
	I2C_MASTER_INIT(100000, 1); //100KHz y prescaler de 1
	
	// *** INICIALIZAR SENSOR AHT10 ***
	I2C_MASTER_START();
	I2C_MASTER_WRITE(0x38 << 1); // Dirección del AHT10 en modo escritura
	I2C_MASTER_WRITE(0xE1); // Comando de inicialización
	I2C_MASTER_STOP();
	_delay_ms(10); // Esperar a que el sensor se estabilice
	
	//---------------------- UART ------------------------------
	
	initUART9600();
	
	_delay_ms(1000);
	
	//------------------------ ADC -----------------------------
	
	/*setADC(externo, izquierda, 7);
	controlADC(1, 0, 1, 128);*/
	
	sei(); 
}

//********************************************************************************************************************************************
// Sub-Funciones
//********************************************************************************************************************************************

void floatToString(float num, char *str, int precision) {
	int intPart = (int)num;  // Parte entera
	float decimalPart = num - intPart;  // Parte decimal

	// Convertir parte entera con itoa()
	itoa(intPart, str, 10);

	// Agregar punto decimal	
	int i = 0;
	while (str[i] != '\0') i++;  // Buscar el final de la cadena
	str[i++] = '.';

	// Convertir parte decimal manualmente
	for (int j = 0; j < precision; j++) {
		decimalPart *= 10;
		int decDigit = (int)decimalPart;
		str[i++] = decDigit + '0';
		decimalPart -= decDigit;
	}

	str[i] = '\0';  // Terminar la cadena
}

void initUART9600(void){
	DDRD &= ~(1<<DDD0); //RX y TX como entrada y salida
	DDRD |= (1<<DDD1);
	
	UCSR0A = 0;
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
	
	UCSR0C = 0;
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	
	UBRR0 = 103; //valor baud y velocidad
}

void I2Ctemp(void) {
	
	I2C_MASTER_START();
	status = I2C_MASTER_WRITE((slavetemp << 1) | 0); //Modo de escritura
	if (status != 1) {
		I2C_MASTER_STOP();
		return;
	}

	status = I2C_MASTER_WRITE(0xAC); //Comando de medicion
	if (status != 1) {
		I2C_MASTER_STOP();
		return;
	}

	I2C_MASTER_STOP();
	_delay_ms(80);

	I2C_MASTER_START();
	status = I2C_MASTER_WRITE((slavetemp << 1) | 1);  //Modo de lectura
	if (status != 1) {
		I2C_MASTER_STOP();
		return;
	}

	for (uint8_t i = 0; i < 6; i++) { //Lee 6 bytes de datos del sensor.
		status = I2C_MASTER_READ(&valorTYH[i], i < 5); //Se almacena los bytes en el array.
		//Asegura que se envíe un ACK después de los primeros 5 bytes y un NACK después del sexto byte.
		if (status != 1) {
			I2C_MASTER_STOP();
			return;
		}
	}

	I2C_MASTER_STOP();
}

void I2Chumo(void){
	
	if (temperatura >= 29){ //verificar temperatura
		I2C_MASTER_START();
		status = I2C_MASTER_WRITE((slavehumo << 1) | 0); //Modo de escritura
		status = I2C_MASTER_WRITE('V'); //Comando de ventilador
		
		textUART("D1"); // Enviar identificador de ventilador
		textUART("\r\n");
		
		if (status != 1) {
			I2C_MASTER_STOP();
			return;
		}
		I2C_MASTER_STOP();
		_delay_ms(80);
	}else if (temperatura < 29){
		I2C_MASTER_START();
		status = I2C_MASTER_WRITE((slavehumo << 1) | 0); //Modo de escritura
		status = I2C_MASTER_WRITE('v'); //Comando de ventilador
		
		textUART("D0"); // Enviar identificador de ventilador
		textUART("\r\n");
		
		if (status != 1) {
			I2C_MASTER_STOP();
			return;
		}
		I2C_MASTER_STOP();
		_delay_ms(80);
	}
		
		
	if (CH > 100){ //verificar concentracion de humo arriba de 400 ppm
		I2C_MASTER_START();
		status = I2C_MASTER_WRITE((slavehumo << 1) | 0); //Modo de escritura
		
		status = I2C_MASTER_WRITE('H'); //Comando de ventilador
		
		if (status != 1) {
			I2C_MASTER_STOP();
			return;
		}
		I2C_MASTER_STOP();
		_delay_ms(80);
	} else if (CH < 100){
		I2C_MASTER_START();
		status = I2C_MASTER_WRITE((slavehumo << 1) | 0); //Modo de escritura
		
		status = I2C_MASTER_WRITE('h'); //Comando de ventilador
		
		if (status != 1) {
			I2C_MASTER_STOP();
			return;
		}
		I2C_MASTER_STOP();
		_delay_ms(80);
	}
	
	I2C_MASTER_START();
	status = I2C_MASTER_WRITE((slavehumo << 1) | 1); //Modo de lectura
	
	if (status != 1) {
		I2C_MASTER_STOP();
		return;
	}
	status = I2C_MASTER_READ(&valorH[0], 0);
	I2C_MASTER_STOP();
	
}

//************************************** LLUVIA ************************************
void I2Clluvia(void){
	
	if (modo == 0){
		if (H2O < 200){ //verificar cuanta agua
			I2C_MASTER_START();
			status = I2C_MASTER_WRITE((slavelluvia << 1) | 0); //Modo de escritura
			status = I2C_MASTER_WRITE('S'); //Comando de ventana
			
			textUART("V1"); // Enviar identificador de ventana
			textUART("\r\n");
			
			if (status != 1) {
				I2C_MASTER_STOP();
				return;
			}
			I2C_MASTER_STOP();
			_delay_ms(80);
			} else if (H2O > 240){ //verificar concentracion de humo arriba de 400 ppm
			I2C_MASTER_START();
			status = I2C_MASTER_WRITE((slavelluvia << 1) | 0); //Modo de escritura
			status = I2C_MASTER_WRITE('s'); //Comando de ventilador
			
			textUART("V0"); // Enviar identificador de ventana
			textUART("\r\n");
			
			if (status != 1) {
				I2C_MASTER_STOP();
				return;
			}
			I2C_MASTER_STOP();
			_delay_ms(80);
		}
		
		I2C_MASTER_START();
		status = I2C_MASTER_WRITE((slavelluvia << 1) | 1); //Modo de lectura
		
		if (status != 1) {
			I2C_MASTER_STOP();
			return;
		}
		status = I2C_MASTER_READ(&valorl[0], 0);
		I2C_MASTER_STOP();
	}
	
}

void textUART(char *texto){
	uint8_t i;
	
	for(i = 0; texto[i] != '\0'; i++){
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = texto[i];
	}
}

//********************************************************************************************************************************************
// Interrupciones
//********************************************************************************************************************************************

ISR(USART_RX_vect){
	while (!(UCSR0B & (1 << RXC0)));
	recibido = UDR0;
	char xd[20];
	
	/*itoa(recibido, xd, 10);
	cursor(10, 1);
	WriteS(xd);*/
	
	if (recibido == 'M'){
		while (!(UCSR0B & (1 << RXC0)));
		recibido = UDR0;
		/*("MODOOOOO");
		uartbuf[0] = recibido;
		textUART(uartbuf);*/
		if (recibido == 1){
			modo = 1;
		}
		else{
			modo = 0;
		}
	}else if (recibido == 'D'){
		while (!(UCSR0B & (1 << RXC0)));
		recibido = UDR0;
		
		if (recibido == 1){
			I2C_MASTER_START();
			status = I2C_MASTER_WRITE((slavehumo << 1) | 0); //Modo de escritura
			status = I2C_MASTER_WRITE('D'); //Comando de ventilador
	
			
			if (status != 1) {
				I2C_MASTER_STOP();
				return;
			}
			I2C_MASTER_STOP();
			_delay_ms(80);
		} else if(recibido == 0){
			I2C_MASTER_START();
			status = I2C_MASTER_WRITE((slavehumo << 1) | 0); //Modo de escritura
			status = I2C_MASTER_WRITE('d'); //Comando de ventilador
			
			if (status != 1) {
				I2C_MASTER_STOP();
				return;
			}
			I2C_MASTER_STOP();
			_delay_ms(80);
		}
	}else if (recibido == 'L'){
		while (!(UCSR0B & (1 << RXC0)));
		recibido = UDR0;
	
		if (recibido == 1){
			I2C_MASTER_START();
			status = I2C_MASTER_WRITE((slavelluvia << 1) | 0); //Modo de escritura
			status = I2C_MASTER_WRITE('L'); //Comando de ventana
		
		
			if (status != 1) {
				I2C_MASTER_STOP();
				return;
			}
			I2C_MASTER_STOP();
			_delay_ms(80);
		} else if(recibido == 0){
		I2C_MASTER_START();
		status = I2C_MASTER_WRITE((slavelluvia << 1) | 0); //Modo de escritura
		status = I2C_MASTER_WRITE('l'); //Comando de ventana
		
		if (status != 1) {
			I2C_MASTER_STOP();
			return;
		}
		I2C_MASTER_STOP();
		_delay_ms(80);
	}
}

}

/*ISR(ADC_vect){
	AWA = ADCH; //Leer el potenciometro activado
	
	ADCSRA |= (1 << ADIF); //Apagar la bandera de ADC
}*/


//********************************************************************************************************************************************
// Caracteres especiales
//********************************************************************************************************************************************

void sun(void){
	CreateChar(0, sun1);
	cursor(14,1);
	WriteC(0x00);
	
	CreateChar(1, sun2);
	cursor(15,1);
	WriteC(0x01);
	
	CreateChar(3, sun3);
	cursor(14,2);
	WriteC(0x03);
	
	CreateChar(4, sun4);
	cursor(15,2);
	WriteC(0x04);
}

void rain(void){
	CreateChar(5, rain1);
	cursor(14,1);
	WriteC(0x05);
	
	CreateChar(6, rain2);
	cursor(15,1);
	WriteC(0x06);
	
	CreateChar(7, rain1);
	cursor(14,2);
	WriteC(0x07);
	
	CreateChar(2, rain2);
	cursor(15,2);
	WriteC(0x02);
}