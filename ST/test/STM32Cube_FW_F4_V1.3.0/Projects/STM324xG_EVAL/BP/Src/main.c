/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef TimHandle; // Timer handler declaration
TIM_OC_InitTypeDef sConfig; // Timer Output Compare Configuration Structure declaration
__IO uint32_t uhCCR1_Val = 1000;
__IO uint32_t uhCCR2_Val = 1100;

/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);
static void SystemClock_Config(void);
static void BSP_Config(void);
static void ETH_Config(void);

/* Private functions ---------------------------------------------------------*/

int main(void) {

  /* STM32F4xx HAL library initialization:
		 - Configure the Flash prefetch, instruction and Data caches
		 - Configure the Systick to generate an interrupt each 1 msec
		 - Set NVIC Group Priority to 4
		 - Global MSP (MCU Support Package) initialization
   */
  HAL_Init();

  /* Configure the System clock to have a frequency of 168 MHz */
  SystemClock_Config();
	
	/* Configure the BSP (Board Support Package) */
  BSP_Config();
	
	/* Configurates the network interface */
	ETH_Config();

	/*##-1- Configure the TIM peripheral #######################################*/
	TimHandle.Instance = TIMx;
	TimHandle.Init.Period = 8000;
	TimHandle.Init.Prescaler = (uint32_t)(((SystemCoreClock /2) / 1000) - 1); //1kHz
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_OC_Init(&TimHandle) != HAL_OK) {
    Error_Handler();
  }

	/*##-2- Configure the Output Compare channels #########################################*/
	sConfig.OCMode = TIM_OCMODE_TOGGLE;
	sConfig.Pulse = uhCCR1_Val;
  sConfig.OCPolarity = TIM_OCPOLARITY_LOW;
	if(HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
	sConfig.Pulse = uhCCR2_Val;
	if(HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
	
	/*##-3- Start signals generation #######################################*/
	/* Start channel 1 in Output compare mode */
  if(HAL_TIM_OC_Start_IT(&TimHandle, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
	if(HAL_TIM_OC_Start_IT(&TimHandle, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }

  while (1) {
		/* EMPTY LOOP */
	}

}

/**
  * @brief  Output Compare callback in non blocking mode 
  * @param  htim : TIM OC handle
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		//__HAL_TIM_SetCompare(&TimHandle, TIM_CHANNEL_1, (uhCCR1_Val));
		BSP_LED_On(LED4);
  }
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		//__HAL_TIM_SetCompare(&TimHandle, TIM_CHANNEL_2, (uhCCR2_Val));
		BSP_LED_Off(LED4);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void) {
	BSP_LED_On(LED4);
  while(1) {}
}

static void BSP_Config(void) {
	BSP_LED_Init(LED1);
	BSP_LED_Init(LED2);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
}

static void ETH_Config(void) {
	/*
	struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
	
	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	*/
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  while (1) {}
}
#endif