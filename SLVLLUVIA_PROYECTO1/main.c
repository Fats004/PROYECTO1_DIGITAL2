//*******************************************************************************************************************************************
// Universidad del Valle de Guatemala
// IE2023 Programación de Microcontroladores
// main.c
// Proyecto: Prelab4
// Hardware: ATMEGA328P
// Created: 2/14/2024
// Author : Fatima Urrutia
//********************************************************************************************************************************************

//********************************************************************************************************************************************
// Librerias
//********************************************************************************************************************************************

# define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C/I2C.h"
#include "ADC/ADC.h"
#include "PWM1/PWM1.h"

//*************************************************************************************************
// VARIABLES GLOBALES
//*************************************************************************************************
#define SlaveAddress 0x30
uint8_t LLUVIA;
char buffer = 0;

// UART
void initUART9600(void);
void textUART(char *texto);

//*************************************************************************************************
// MAIN
//*************************************************************************************************
int main(void) {
    // Inicializar UART para depuración
	
	CLKPR |= (1 << CLKPCE);
	CLKPR |= (1 << CLKPS0);	
    initUART9600();
    textUART("Iniciando SLAVE 1 LLUVIA...\r\n");

    // Inicializar ADC
    setADC(externo, izquierda, 7);
    controlADC(1, 0, 1, 128);

    // Inicializar I2C
    I2C_SLAVE_INIT(SlaveAddress);

    // Inicializar PWM para servo
    setPWM0fastA(0, _FF0, 1024); // setear el PWM0 Fast

    // Habilitar interrupciones
    TWCR |= (1 << TWIE);
    sei();

    while (1) {
        ADCSRA |= (1 << ADSC);

        if (buffer == 'S') {
           textUART("Comando recibido: S -> Moviendo servo a 90°\r\n");
           updateDutyA0(127/6);  // 90° (1.5 ms)
            _delay_ms(10);
            buffer = 0;
        }
        else if (buffer == 's') { 
            textUART("Comando recibido: s -> Moviendo servo a 0°\r\n");
            updateDutyA0(50/6);  // 0° (1 ms)
            _delay_ms(10);
            buffer = 0;
        }
    }
}

//*************************************************************************************************
// INTERRUPCIONES
//*************************************************************************************************
ISR(ADC_vect) {
    LLUVIA = ADCH;
    ADCSRA |= (1 << ADIF); // Apagar la bandera de ADC
}

ISR(TWI_vect) {
    uint8_t estado = TWSR & 0xFC;
    switch (estado) {
        case 0x60:
        case 0x70:
            textUART("I2C: Dirección propia recibida\r\n");
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
            break;
        case 0x80:
        case 0x90:
            buffer = TWDR;
            textUART("I2C: Dato recibido -> \r\n");
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
            break;
        case 0xA8:
        case 0xB8:
            TWDR = LLUVIA;
            textUART("I2C: Enviando dato\r\n");
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
            break;
        default:
            textUART("I2C: Estado inesperado\r\n");
            TWCR |= (1 << TWINT) | (1 << TWSTO);
            break;
    }
}

//*************************************************************************************************
// FUNCIONES UART
//*************************************************************************************************
void initUART9600(void) {
    DDRD &= ~(1 << DDD0); // RX como entrada
    DDRD |= (1 << DDD1);  // TX como salida

    UCSR0A = 0;
    UCSR0B = 0;
    UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    UBRR0 = 103; // 9600 baudios
}

void textUART(char *texto) {
    uint8_t i;
    for (i = 0; texto[i] != '\0'; i++) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = texto[i];
    }
}


