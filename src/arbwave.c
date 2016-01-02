/*
 * arbwave.c
 *
 *  Created on: Dec 24, 2015
 *      Author: Matt
 */

#include <math.h>
#include "arbwave.h"
#include "arm_math.h"

uint16_t outPos = 0;
uint16_t lastBinFreq = 0;
uint16_t lastFreq = 0;


uint16_t Calc_Triangle_Wave(applicationState* appState, float32_t scaleVal,
								 uint16_t* position, float32_t slope) {
	uint16_t outVal = 0;
	float32_t plotPos = scaleVal * (*position)++;

	if (plotPos < M_PI) {
		outVal = slope*plotPos+appState->offset;
	} else if (plotPos < M_TWOPI) {
		outVal = 2*appState->amp-slope*(plotPos-M_PI) +
					appState->offset;
	} else {
		*position = 0;
		outVal = appState->offset;
	}

	return outVal;
}

uint16_t Calc_Square_Wave(applicationState* appState,
								float32_t scaleVal, uint16_t* position) {
	uint16_t outVal = 0;
	float32_t plotPos = scaleVal * (*position)++;
	if (plotPos < M_PI) {
		outVal = appState->offset+2*appState->amp;
	} else if (plotPos < M_TWOPI) {
		outVal = appState->offset;
	} else {
		*position = 0;
		outVal = appState->offset+2*appState->amp;
	}
	return outVal;
}

uint16_t Calc_SawTooth_Wave(applicationState* appState, float32_t scaleVal,
								uint16_t *position, float32_t slope) {
	uint16_t outVal = 0;
	float32_t plotPos = scaleVal * (*position)++;
	if (plotPos < M_TWOPI) {
		outVal = slope*plotPos+appState->offset;
	} else {
		*position = 0;
		outVal = appState->offset;
	}

	return outVal;
}

void Fill_DAC_Half_Buffer(applicationState* appState) {
	uint16_t *activeSample = appState->dacData+
								((appState->bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i = 0;
	float32_t scaleVal=(appState->freq*M_TWOPI)/DAC_OUTPUT_RATE;
	float32_t outVal = 0.0;
	float32_t slope = (2*appState->amp)/M_PI;;

	uint16_t offset = 2*appState->offset;

	uint16_t reset = DAC_OUTPUT_RATE/appState->freq;
	float32_t step = DAC_OUTPUT_RATE*1.0f/appState->freq;

	switch(appState->func) {
	case F_SQUARE_WAVE:
		while (i < DAC_SAMPLES_SIZE) {
			*activeSample++ = Calc_Square_Wave(appState, scaleVal, &outPos);
			i++;
		}
		break;
	case F_TRIANGLE_WAVE:
		while (i < DAC_SAMPLES_SIZE) {
			*activeSample++ = Calc_Triangle_Wave(appState, scaleVal, &outPos, slope);
			i++;
		}
		break;
	case F_SAWTOOTH_WAVE:
		slope = appState->amp/M_PI;
		while (i < DAC_SAMPLES_SIZE) {
			*activeSample++ = Calc_SawTooth_Wave(appState, scaleVal, &outPos, slope);
			i++;
		}
		break;
	case F_SINE_WAVE:
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
		break;
	default:
		break;
	}
}

