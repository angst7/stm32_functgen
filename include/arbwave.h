/*
 * arbwave.h
 *
 *  Created on: Dec 24, 2015
 *      Author: Matt
 */

#ifndef ARBWAVE_H_
#define ARBWAVE_H_

#include "stm32f4xx.h"

#define DAC_OUTPUT_RATE 100000
#define DAC_SAMPLES_SIZE 5000
#define OFFSET_VALUE 2050

void Fill_DAC_SW_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp);
void Fill_DAC_TW_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp);
void Fill_DAC_ST_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp);
void Fill_DAC_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp);

#endif /* ARBWAVE_H_ */
