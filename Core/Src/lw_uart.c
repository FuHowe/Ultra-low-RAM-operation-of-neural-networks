#include "main.h"

UART_HandleTypeDef UART1_Handler; 


void lw_uart1_init(void)
{	
	UART1_Handler.Instance=  USART1;
	UART1_Handler.Init.BaudRate = 115200;
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;
	UART1_Handler.Init.Parity = UART_PARITY_NONE;
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&UART1_Handler);
}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Initure;

	if(huart->Instance == USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();
	
		GPIO_Initure.Pin = GPIO_PIN_9;
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;
		GPIO_Initure.Pull = GPIO_PULLUP;
		GPIO_Initure.Speed = GPIO_SPEED_FAST;
		GPIO_Initure.Alternate = GPIO_AF7_USART1;	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);

		GPIO_Initure.Pin=GPIO_PIN_10;
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);
		
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn,3,3);
	}
}


int fputc(int indata, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);
	USART1->DR = (uint8_t) indata;      
	return indata;
}




