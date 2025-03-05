/*
 * LCD.h
 *
 * Created: 2/23/2025 10:12:26 AM
 *  Author: Fatima
 */ 


#ifndef LCD_H_
#define LCD_H_

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

void CreateChar(char location, char charmap[]);

void initLCD_8bit(void); //Iniciar LCD en modo 8 bits

void Port(char a); //Colocar valor en el puerto

void CMD(char a); // Colocar un comando

void WriteC(char a);

void WriteS(char *a);

void cursor(char c, char f);

#endif /* LCD_H_ */