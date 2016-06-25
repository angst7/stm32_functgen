/*****************************************************************************
*
* arbwave.h
*
* Definitions for the configuration and operation of the STM32 Function
* Generator
*
* Copyright (c) 2016 Matt Roche
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef ARBWAVE_H_
#define ARBWAVE_H_

#include "stm32f4xx.h"

#define DAC_OUTPUT_RATE 420000		// The update rate of the DAC in Hz
#define DAC_SAMPLES_SIZE 10000		// The size of the DAC output buffer

#define SAMPLES_SIZE 3				// Number of ADC channels we're scanning

#define FREQUENCY_PIN	GPIO_PIN_0		// The ADC Inputs for setting wave
#define AMPLITUDE_PIN	GPIO_PIN_1		// attributes
#define OFFSET_PIN		GPIO_PIN_5

#define FREQUENCY_CHANNEL	ADC_CHANNEL_0 // ADC Channels corresponding to the
#define AMPLITUDE_CHANNEL	ADC_CHANNEL_1 // IO pins selected above
#define OFFSET_CHANNEL		ADC_CHANNEL_5

#define DAC_OUTPUT_PIN	GPIO_PIN_4		// DAC audio output

#define SQUARE_PIN		GPIO_PIN_11		// GPIO Inputs to select waveform type
#define TRIANGLE_PIN	GPIO_PIN_12
#define SAWTOOTH_PIN	GPIO_PIN_13
#define SINE_PIN		GPIO_PIN_14

#define GREEN_LED_PIN	GPIO_PIN_8		// LED GPIO Outputs
#define RED_LED_PIN		GPIO_PIN_9

#define ADC_PORT		GPIOA			// Ports for various peripherals
#define DAC_PORT		GPIOA
#define SELECT_PORT		GPIOB
#define LED_PORT		GPIOC

// Shorthand macros for GPIO read and toggle

#define pinset(X) HAL_GPIO_ReadPin(X)==GPIO_PIN_SET
#define toggle(X) HAL_GPIO_TogglePin(X)

#define SQUARE		 	SELECT_PORT, SQUARE_PIN
#define TRIANGLE		SELECT_PORT, TRIANGLE_PIN
#define SAWTOOTH		SELECT_PORT, SAWTOOTH_PIN
#define SINE			SELECT_PORT, SINE_PIN

#define GREEN_LED		LED_PORT, GREEN_LED_PIN
#define RED_LED			LED_PORT, RED_LED_PIN


typedef enum {
	F_SQUARE_WAVE,
	F_TRIANGLE_WAVE,
	F_SAWTOOTH_WAVE,
	F_SINE_WAVE
} outputFunctionType;

// applicationState stores the current operating state of the generator

typedef struct {
	uint16_t* dacData;
	outputFunctionType func;
	uint8_t bin;
	uint16_t freq;
	uint16_t amp;
	uint16_t offset;
} applicationState;

void Fill_DAC_Half_Buffer(applicationState* appState);

#endif /* ARBWAVE_H_ */
