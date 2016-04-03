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

#define BUFFER_SIZE 16
#define SAMPLES_SIZE 3		// Number of ADC1 channels we're scanning

#define FREQUENCY_PIN	GPIO_PIN_0		// The ADC Inputs for setting wave
#define AMPLITUDE_PIN	GPIO_PIN_1		// attributes
#define OFFSET_PIN		GPIO_PIN_5

#define DAC_OUTPUT_PIN	GPIO_PIN_4		// DAC audio output

#define SQUARE_PIN		GPIO_PIN_11		// GPIO Inputs to select waveform type
#define TRIANGLE_PIN	GPIO_PIN_12
#define SAWTOOTH_PIN	GPIO_PIN_13
#define SINE_PIN		GPIO_PIN_14

#define GREEN_LED_PIN	GPIO_PIN_8		// LED GPIO Outputs
#define RED_LED_PIN	GPIO_PIN_9

#define ADC_PORT		GPIOA			// Ports for various peripherals
#define DAC_PORT		GPIOA
#define SELECT_PORT	GPIOB
#define LED_PORT		GPIOC

// Shorthand macros

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
