/*
 * I2C.h
 *
 * Created: 13/02/2025 18:32:23
 *  Author: Luis
 */ 
#include <stdlib.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif

#ifndef I2C_H_
#define I2C_H_

void I2C_MASTER_INIT(unsigned long SCL_CLOCK, uint8_t Prescaler);
void I2C_MASTER_START(void);
void I2C_MASTER_STOP(void);
uint8_t I2C_MASTER_WRITE(uint8_t dato);
uint8_t I2C_MASTER_READ(uint8_t *buffer, uint8_t ack);
void I2C_SLAVE_INIT(uint8_t address);



#endif /* I2C_H_ */