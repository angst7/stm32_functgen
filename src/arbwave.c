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


void Fill_DAC_SW_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp) {
	uint16_t *activeSample = dacData+((bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i;
	float32_t scaleVal = M_TWOPI/DAC_OUTPUT_RATE;
	float32_t outVal = 0.0;

	for (i = 0; i < DAC_SAMPLES_SIZE; i++) {
		outVal = freq*scaleVal*outPos++;
		if (outVal < M_PI) {
			*activeSample++ = OFFSET_VALUE+amp;
		} else if (outVal < M_TWOPI){
			*activeSample++ = OFFSET_VALUE-amp;
		} else {
			outPos = 0;
			*activeSample++ = OFFSET_VALUE+amp;
		}
	}
}

void Fill_DAC_TW_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp) {
	uint16_t *activeSample = dacData+((bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i;
	float32_t scaleVal = M_TWOPI/DAC_OUTPUT_RATE;
	float32_t outVal = 0.0;
	float32_t slope = (2*amp)/M_PI;

	for (i = 0; i < DAC_SAMPLES_SIZE; i++) {
		outVal = freq*scaleVal*outPos++;
		if (outVal < M_PI) {
			*activeSample++ = slope*outVal;
		} else if (outVal < M_TWOPI) {
			*activeSample++ = 2*amp-slope*(outVal-M_PI);
		} else {
			outPos = 0;
			*activeSample++ = 0;
		}
	}
}

void Fill_DAC_ST_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp) {
	uint16_t *activeSample = dacData+((bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i;
	float32_t scaleVal = M_TWOPI/DAC_OUTPUT_RATE;
	float32_t outVal = 0.0;
	float32_t slope = amp/M_PI;

	for (i = 0; i < DAC_SAMPLES_SIZE; i++) {
		outVal = freq*scaleVal*outPos++;
		if (outVal < M_TWOPI) {
			*activeSample++ = slope*outVal;
		} else {
			outPos = 0;
			*activeSample++ = 0;
		}
	}
}

void Fill_DAC_Half_Buffer(uint16_t *dacData, uint8_t bin, uint16_t freq, uint16_t amp) {
	uint16_t *activeSample = dacData+((bin-1)*DAC_SAMPLES_SIZE);
	uint32_t i = 0;
	uint16_t outVal = 0;
	float32_t scaleVal = freq*(M_TWOPI/DAC_OUTPUT_RATE);


	while (i < DAC_SAMPLES_SIZE) {
		// calculate the sin of 2*pi*f/dacrate
		outVal = (uint16_t)(arm_sin_f32(scaleVal*outPos++)*amp+OFFSET_VALUE);

		// Scale by amplitude factor, add offset
		*activeSample++ = outVal;

		// check to see if we're near a multiple of 2*pi

		if (outVal == OFFSET_VALUE) {
			if (outPos > 400) {
				if (arm_cos_f32(scaleVal*(outPos-1)) > 0) {
					// reset the counter here to avoid glitches
					outPos = 0;
				}
			}
		}
		i++;
	}

}
