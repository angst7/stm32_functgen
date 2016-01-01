/*
 * arbwave.c
 *
 *  Created on: Dec 24, 2015
 *      Author: Matt
 */

#include <math.h>
#include "arbwave.h"
#include "arm_math.h"

uint32_t outPos = 0;
uint16_t lastBinFreq = 0;
uint16_t lastFreq = 0;


void Fill_DAC_SW_Half_Buffer(applicationState* appState) {
	uint16_t *activeSample = appState->dacData+
								((appState->bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i;
	float32_t scaleVal = M_TWOPI/DAC_OUTPUT_RATE;
	float32_t outVal = 0.0;

	uint16_t offset = appState->offset+appState->amp;
	for (i = 0; i < DAC_SAMPLES_SIZE; i++) {
		outVal = appState->freq*scaleVal*outPos++;
		if (outVal < M_PI) {
			*activeSample++ = offset+appState->amp;
		} else if (outVal < M_TWOPI){
			*activeSample++ = offset-appState->amp;
		} else {
			outPos = 0;
			*activeSample++ = offset+appState->amp;
		}
	}
}

void Fill_DAC_TW_Half_Buffer(applicationState* appState) {
	uint16_t *activeSample = appState->dacData+
								((appState->bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i;
	float32_t scaleVal = M_TWOPI/DAC_OUTPUT_RATE;
	float32_t outVal = 0.0;
	float32_t slope = (2*appState->amp)/M_PI;

	for (i = 0; i < DAC_SAMPLES_SIZE; i++) {
		outVal = appState->freq*scaleVal*outPos++;
		if (outVal < M_PI) {
			*activeSample++ = slope*outVal+appState->offset;
		} else if (outVal < M_TWOPI) {
			*activeSample++ = 2*appState->amp-slope*(outVal-M_PI) +
								appState->offset;
		} else {
			outPos = 0;
			*activeSample++ = appState->offset;
		}
	}
}

void Fill_DAC_ST_Half_Buffer(applicationState* appState) {
	uint16_t *activeSample = appState->dacData+
								((appState->bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i;
	float32_t scaleVal = M_TWOPI/DAC_OUTPUT_RATE;
	float32_t outVal = 0.0;
	float32_t slope = appState->amp/M_PI;

	for (i = 0; i < DAC_SAMPLES_SIZE; i++) {
		outVal = appState->freq*scaleVal*outPos++;
		if (outVal < M_TWOPI) {
			*activeSample++ = slope*outVal+appState->offset;
		} else {
			outPos = 0;
			*activeSample++ = appState->offset;
		}
	}
}

void Fill_DAC_Half_Buffer(applicationState* appState) {
	uint16_t *activeSample = appState->dacData+
								((appState->bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i = 0;
	uint16_t outVal = 0;
	float32_t scaleVal = (appState->freq*M_TWOPI)/DAC_OUTPUT_RATE;

	uint16_t offset = 2*appState->offset;

	uint16_t reset = DAC_OUTPUT_RATE/appState->freq;
	float32_t step = DAC_OUTPUT_RATE*1.0f/appState->freq;

	scaleVal *= (step/reset);	// correct for floating point / integer error

	while (i < DAC_SAMPLES_SIZE) {
		// calculate the sin of 2*pi*f/dac rate
		outVal = (uint16_t)(arm_sin_f32(scaleVal*outPos++)*appState->amp+offset);

		// Scale by amplitude factor, add offset
		*activeSample++ = outVal;

		// check to see if we're near a multiple of 2*pi
		if ((outPos > 10) && (outPos % reset == 0)) {
			outPos = 0;
		}

		i++;
	}

}
