/*****************************************************************************
*
* arbwave.c
*
* This is a collection of routines to generate Sine, Square, Triangle and
* Sawtooth waveforms as part of a simple STM32 Function Generator
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

#include <math.h>
#include "arbwave.h"
#include "arm_math.h"

uint16_t outPos = 0;
uint16_t lastBinFreq = 0;
uint16_t lastFreq = 0;


inline uint16_t Calc_Triangle_Wave(applicationState* appState, float32_t scaleVal,
								 uint16_t *position, float32_t slope) {
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

inline uint16_t Calc_Square_Wave(uint16_t offset, uint16_t amp,
								float32_t scaleVal, uint16_t *position) {
	uint16_t outVal = 0;
	float32_t plotPos = scaleVal * (*position)++;

	if (plotPos < M_PI) {
		outVal = amp;
	} else if (plotPos < 6.243) {
		outVal = offset;
	} else {
		*position = 0;
		outVal = amp;
	}
	return outVal;
}

inline uint16_t Calc_SawTooth_Wave(applicationState* appState, float32_t scaleVal,
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

	float32_t slope = (2*appState->amp)/M_PI;;

	uint16_t offset = 2*appState->offset;

	uint16_t reset = DAC_OUTPUT_RATE/appState->freq;
	float32_t step = DAC_OUTPUT_RATE*1.0f/appState->freq;

	switch(appState->func) {
	case F_SQUARE_WAVE:
		offset = appState->offset;
		uint16_t amp = offset+2*appState->amp;
		while (i < DAC_SAMPLES_SIZE) {
			*activeSample++ = Calc_Square_Wave(offset, amp, scaleVal, &outPos);
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
			*activeSample++ = (uint16_t)(arm_sin_f32(scaleVal*outPos++)*appState->amp+offset);

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

