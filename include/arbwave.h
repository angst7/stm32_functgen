/*
 * arbwave.h
 *
 *  Created on: Dec 24, 2015
 *      Author: Matt
 */

#ifndef ARBWAVE_H_
#define ARBWAVE_H_

#include "stm32f4xx.h"

#define DAC_OUTPUT_RATE 120000
#define DAC_SAMPLES_SIZE 5000

typedef enum {
	F_SQUARE_WAVE,
	F_TRIANGLE_WAVE,
	F_SAWTOOTH_WAVE,
	F_SINE_WAVE
} outputFunctionType;

typedef struct {
	uint16_t* dacData;
	outputFunctionType func;
	uint8_t bin;
	uint16_t freq;
	uint16_t amp;
	uint16_t offset;
} applicationState;

void Fill_DAC_SW_Half_Buffer(applicationState* appState);
void Fill_DAC_TW_Half_Buffer(applicationState* appState);
void Fill_DAC_ST_Half_Buffer(applicationState* appState);
void Fill_DAC_Half_Buffer(applicationState* appState);

#endif /* ARBWAVE_H_ */
