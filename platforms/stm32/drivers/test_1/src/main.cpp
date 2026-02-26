#include "stm32h7xx_hal.h"

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	// Configure power supply
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	// Use HSI as system clock source (64MHz)
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	// Configure CPU, AHB and APB clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
								| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
								| RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

int main() {
	HAL_Init();
	SystemClock_Config();

	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	while(1) {
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
		HAL_Delay(500);
	}
	return 0;
}

extern "C" void SysTick_Handler(void) {
	HAL_IncTick();
}
