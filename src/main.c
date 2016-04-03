//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "arbwave.h"

#include "arm_math.h"

// ----- Timing definitions -------------------------------------------------

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

uint16_t sampleData[SAMPLES_SIZE * 2] = {0};
volatile uint16_t outData[SAMPLES_SIZE * 2] = {0};

uint8_t sampleBucket = 0;
uint8_t dacBucket = 1;
uint8_t txComplete;
uint8_t rxComplete;
uint8_t activeBucket;
uint16_t lastOut;



static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_DMA_Init();


int main(int argc, char* argv[])
{

  uint16_t i;
  uint16_t freqValues[5] = {0};
  uint16_t nextValue = 0;
  uint16_t sum = 0;
  uint16_t dacData[DAC_SAMPLES_SIZE * 2];
  applicationState appState = {.dacData=(uint16_t*)&dacData, .freq=370,
		  .func=F_SINE_WAVE, .amp=500, .offset=2050};

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("System clock: %u Hz\n", SystemCoreClock);
  
  txComplete = 0;
  rxComplete = 0;
  activeBucket = 0;
  lastOut = 0;

  //Fill_DAC_Half_Buffer(dacData, dacBucket, freq, amp, offset);
  dacBucket = 0;

  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
	  while (1)
		  ;

  if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
	  while (1)
		  ;

  if (HAL_ADC_Start(&hadc1) != HAL_OK)
	  while (1)
		  ;

  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&sampleData,
		  SAMPLES_SIZE*2) != HAL_OK)
	  while (1)
		  ;

  if (HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)dacData,
		  DAC_SAMPLES_SIZE*2, DAC_ALIGN_12B_R) != HAL_OK)
	  while (1)
		  ;

  while (1)
    {

	  if (dacBucket) {
		  appState.bin = dacBucket;
		  if (pinset(SQUARE)) {
			  appState.func = F_SQUARE_WAVE;
		  } else if (pinset(TRIANGLE)) {
			  appState.func = F_TRIANGLE_WAVE;
		  } else if (pinset(SAWTOOTH)) {
			  appState.func = F_SAWTOOTH_WAVE;
		  } else if (pinset(SINE)) {
			  appState.func = F_SINE_WAVE;
		  }

		  Fill_DAC_Half_Buffer(&appState);
		  dacBucket = 0;
	  }

	  if (sampleBucket) {

		  	toggle(RED_LED);
		  	freqValues[nextValue++] = sampleData[(sampleBucket-1)*SAMPLES_SIZE]*2.8;

		  	appState.amp = (sampleData[(sampleBucket-1)*SAMPLES_SIZE+1]/2);

		  	if (appState.amp > 2000)
		  		appState.amp = 2000;

		  	appState.offset = (sampleData[(sampleBucket-1)*SAMPLES_SIZE+2]/2);

		  	if (appState.offset > 4000)
		  		appState.offset = 4000;

		  	if (nextValue == 5)
		  		nextValue = 0;

		  	sum = 0;
		  	for (i=0; i<5; i++) {
		  		sum += freqValues[i];
		  	}
		    appState.freq = (sum/5)+100;

		  	sampleBucket = 0;
	  }

    }
  // Infinite loop, never return.
}

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION12b;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = SAMPLES_SIZE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = EOC_SEQ_CONV;
  HAL_ADC_Init(&hadc1);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;

  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 3;

  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/* DAC init function */
void MX_DAC_Init(void)
{

  DAC_ChannelConfTypeDef sConfig;

    /**DAC Initialization
    */
  hdac.Instance = DAC;
  HAL_DAC_Init(&hdac);

    /**DAC channel OUT1 config
    */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

}


void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 5249;					// Prescaler is 16-bit
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0x0;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}

void MX_TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = (uint32_t)(SystemCoreClock/(2*DAC_OUTPUT_RATE) - 1);
  //htim6.Init.Period = 419;							// 200kHz Update Clock
  HAL_TIM_Base_Init(&htim6);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);

}

void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __DMA1_CLK_ENABLE();
  __DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = RED_LED_PIN|GREEN_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SQUARE_PIN|TRIANGLE_PIN|SAWTOOTH_PIN|SINE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SELECT_PORT, &GPIO_InitStruct);

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
