#include "uart.h"
#include "stm32f4xx_usart.h"

volatile unsigned char data1;
volatile unsigned char Rflag1 ; // 수신 인터럽트가 발생했는지를 알아보는 변수
volatile unsigned char data2;
volatile unsigned char Rflag2 ; // 수신 인터럽트가 발생했는지를 알아보는 변수

void uart_init()
{
        GPIO_InitTypeDef   GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef   NVIC_InitStructure;
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);     //USART2_TX
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);    //USART2_RX
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);     //USART1_TX
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);    //USART1_RX
        
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_Init(&NVIC_InitStructure);   
        
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART2, &USART_InitStructure);
        
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // USART2 Interrupt enable
        USART_Cmd(USART2, ENABLE);
        
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &USART_InitStructure);  
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // USART1 Interrupt enable
        USART_Cmd(USART1, ENABLE);
}

void Serial_Send1(unsigned char t)
{
        USART_SendData(USART1, t);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_Send2(unsigned char t)
{
        USART_SendData(USART2, t);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}
void TX1_string(char *string)			//transmit a string by USART2 
{
	while(*string != '\0')
	{ 
		USART_SendData(USART1,*string);
                while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		string++;
	}
}


void USART1_IRQHandler(void)
{
        if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
                data1 = USART_ReceiveData(USART1);
                Rflag1 = 1; // 수신 인터럽트 수행시 ‘1’로 만들어 준다.
        }
}
void USART2_IRQHandler(void)
{
        if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
        {
                data2 = USART_ReceiveData(USART2);
                Rflag2 = 1; // 수신 인터럽트 수행시 ‘1’로 만들어 준다.
        }
}

int putchar(int ch)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,ch);
	return ch;
}
