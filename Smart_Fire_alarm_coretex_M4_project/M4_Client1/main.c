/******************************************************************************
USART 1 : wifi
USART2 : printf
MCU   ESP8266 Serial WIFI 모듈  핀
  3.3V                 VCC, CH_PD
 GND                  GND

CortexM4            ESP8266 Serial WIFI 모듈  핀
PORTA9(USART1 TX)             RX  
PORTA10(USART1 RX)             TX
GND                         GND
PORTC0~11 : FND
PORTB8~15 : LED
PORTA0(ADC)    : VRES
******************************************************************************/
// stm32f4xx의 각 레지스터들을 정의한 헤더파일
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wifi.h"
#include "dot.h"
#include "dht.h"


// delay 함수


volatile unsigned char data1;
volatile unsigned char Rflag1 ; // 수신 인터럽트가 발생했는지를 알아보는 변수
volatile unsigned char data2;
volatile unsigned char Rflag2 ; // 수신 인터럽트가 발생했는지를 알아보는 변수
volatile unsigned long systick_count;
volatile unsigned long int t_cnt;
extern volatile char uart1_rxdata[5][100];
unsigned long systick_sec;
volatile int adc_flag;
volatile int dht_flag=0;
volatile int gKey=-1;

int dot_st = 0;

volatile  uint16_t adc_data=0;  
struct dht11 dhtInfo; 

#define ARR_CNT 5

void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
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
void TX1_string(char *string)			/* transmit a string by USART2 */
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

void TIM7_IRQHandler(void)              //4ms
{
 
 
  if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    t_cnt++ ;
    
    
    
     if(!(t_cnt%2)) systick_count++;
      
     
    if(t_cnt >= 500)             //1s
    {
      t_cnt = 0;
      systick_sec++;
      if(dht_flag==1 ) { 
        dot_st++;
        if(dot_st>=6) dot_st=0;
                                systick_sec++;

        
      }
    }
  }
}

void TIM1_UP_TIM10_IRQHandler(void)
{
 
 if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    
  }
}


void ADC_IRQHandler(void)
{ 
  if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) 
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    adc_flag = 1;
  }
}


void EXTI15_10_IRQHandler(void)
{
 
  if(EXTI_GetITStatus(EXTI_Line11) != RESET)
  {
     gKey = 1;
    
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
   if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  {
     gKey = 2;
    
    EXTI_ClearITPendingBit(EXTI_Line12);
  }
}


int main()
{
  char senddata[100]={0};
  char recvdata[100]={0};
  char *pToken;
  char *pArray[ARR_CNT]={0};


  
  
  Init_Port();

  LD1071_Reset();

  LD1071_Tx_CMD(0x0034); // Configuration Register Update Command
  LD1071_Tx_Data(0x04FF); // Configuration Register

  dhtInfo.GPIOx = GPIOB;
  dhtInfo.GPIO_Pin = GPIO_Pin_7;
  DHT11_Init(dhtInfo);

 
  GPIO_InitTypeDef   GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;                       //TIM7
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11 | EXTI_PinSource12);
  
    EXTI_InitStructure.EXTI_Line = EXTI_Line11|EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  int i;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);        //TIM7  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);        //ADC1
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
                                         GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|
                                           GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|
                                      GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
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

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  
  //인터럽트 enable 및 Priority 설정.
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
   NVIC_Init(&NVIC_InitStructure);   

   NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;               //TM7
   NVIC_Init(&NVIC_InitStructure);    
   
   NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
   NVIC_Init(&NVIC_InitStructure); 
      
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1;         //(168Mhz/2)/84 = 1MHz(1us)  //1us
 // TIM_TimeBaseStructure.TIM_Period = 10000-1;        //1us * 10000 =  10ms     
  TIM_TimeBaseStructure.TIM_Period = 2000-1;        //1us * 4000 =  4ms     
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  //타이머7을 동작시킨다.
  TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM7, ENABLE);

  TIM_TimeBaseStructure.TIM_Prescaler = 168-1;         //168Mhz/84 = 1MHz(1us)
  TIM_TimeBaseStructure.TIM_Period = 17700-1;          
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);
  TIM_ClearITPendingBit(TIM10, TIM_IT_CC1|TIM_IT_Update);
  TIM_ITConfig(TIM10, TIM_IT_CC1|TIM_IT_Update, ENABLE);
TIM_Cmd(TIM10, ENABLE);
  
    
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
    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;             // ADC123_IN3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;    // 단일 컨버전
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);     // ADC1의 채널3 사용
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);             // ADC1 인터럽트 사용
  ADC_Cmd(ADC1, ENABLE);
  ADC_SoftwareStartConv(ADC1);                                  // ADC1 컨버전 시작
  
  WIFI_init();
  sprintf(senddata,"[KSJ_M4:PASSWD]");
  WIFI_send(senddata);
 
  while(1)
  {
    if(dht_flag==1) d_disp(dot_st);
  
    
    if(DHT_Read(&dhtInfo))
    {
      printf("%d %d %d %d\r\n",dhtInfo.data[0],dhtInfo.data[1],dhtInfo.data[2],dhtInfo.data[3]);
      if ((dhtInfo.data[2]>40)&&(dht_flag==0))
      {
         sprintf(senddata,"[%s]%s@%s\n","YJJ_M4","FIRE","ON");
         WIFI_send(senddata);
         printf(senddata);
         printf("\r\n");
         dht_flag=1;
     
      }
      else if ((dhtInfo.data[2] < 39) && (dht_flag==1))
      {
        sprintf(senddata,"[%s]%s@%s\n","YJJ_M4","FIRE","OFF");
        WIFI_send(senddata);
        printf(senddata);
        printf("\r\n");
        dht_flag=0;
      }
    }
    
    
    if(gKey==1)
    {
        sprintf(senddata,"[%s]%s@%s\n","YJJ_M4","FIRE","ON");
        WIFI_send(senddata);
        printf(senddata);
        printf("\r\n");
        gKey=-1;
    }
    
    if(gKey==2)
    {
        sprintf(senddata,"[%s]%s@%s\n","YJJ_M4","FIRE","OFF");
        WIFI_send(senddata);
        printf(senddata);
        printf("\r\n");
        gKey=-1;
    }
    
      recvdata[0] = 0;
      if(wifi_wait("+IPD","+IPD", 10))  //수신포멧 :  +IPD,6:hello  끝문자 0x0a
      {	
              for(i=0;i<5;i++) 
              {
                      if(strncmp((char *)uart1_rxdata[i],"+IPD",4)==0) 
                      {
      //					sprintf(recvdata,"RECV Data(index:%d,len:%d) : %s\r\n",i,strlen((char *)(uart0_rxdata[i]+8)),uart0_rxdata[i]+8);
      //					printf(recvdata);
                              strcpy(recvdata,(char *)(uart1_rxdata[i]+8));
                              recvdata[strlen((char *)(uart1_rxdata[i]+8)) - 1] = 0;
                              printf(recvdata); 
                              printf("\r\n");
                      }
              }
      }
      if(recvdata[0] != 0) {

          pToken = strtok(recvdata,"[@]");
          i = 0;
          while(pToken != NULL)
          {
            pArray[i] =  pToken;
            if(i++ >= ARR_CNT)
                break;
            pToken = strtok(NULL,"[@]");
          }
          if(!strncmp(pArray[1]," New con",7))
         {
              printf("TEST01\r\n");
              continue;
          }
          else  if(!strncmp(pArray[1]," Already",7))
         {
              printf("TEST02\r\n");
              WIFI_init();
              sprintf(senddata,"[KSJ_M4:PASSWD]");
              WIFI_send(senddata);
              continue;
          }
/*          else if(!strncmp(pArray[1],"LIGHT_ON",8))
          {
             GPIO_SetBits(GPIOB, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|
                                      GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); 
          }
          else if(!strncmp(pArray[1],"LIGHT_OFF",9))
          {
              GPIO_ResetBits(GPIOB, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|
                                      GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
          }
          else if(!strncmp(pArray[1],"LED",3))
          {
            temp=strtol(pArray[2],0,16)&0xff;
            sprintf(senddata,"Recv : %d\n",temp);
            printf(senddata);
            printf("\r\n");
           
            GPIO_Write(GPIOC,temp); 
            sprintf(senddata,"[%s]%s#%s\n",pArray[0],pArray[1],pArray[2]);
            WIFI_send(senddata);
            printf(senddata);
            printf("\r\n");
              
          }
          else if(!strncmp(pArray[1],"GETCDS",6))
          {
             lcdFlag = 1;
           }
           else if(!strncmp(pArray[1],"OFFCDS",6))
          {
              lcdFlag = 0;
          }
          */
          else if(!strncmp(pArray[1],"LED",3)) {
                  if(!strncmp(pArray[2],"ON",2)) {
                          GPIO_SetBits(GPIOB,GPIO_Pin_8);
                          sprintf(senddata,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
                  }
                  else if(!strncmp(pArray[2],"OFF",3)) {
                          GPIO_ResetBits(GPIOB,GPIO_Pin_8);
                          sprintf(senddata,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
                  }
          } else if(!strncmp(pArray[1],"LAMP",4)) {
                  if(!strncmp(pArray[2],"ON",2)) {
                         GPIO_SetBits(GPIOB,GPIO_Pin_9);
                          sprintf(senddata,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
                  }
                  else if(!strncmp(pArray[2],"OFF",3))
                  {
                          GPIO_ResetBits(GPIOB,GPIO_Pin_9);
                          sprintf(senddata,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
                  }
          }else if(!strncmp(pArray[1],"GETSTATE",strlen("GETSTATE"))) {
                  if(!strncmp(pArray[2],"DEV",3)) {
                          sprintf(senddata,"[%s]DEV@%s@%s\n",pArray[0], GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_8)?"ON":"OFF",GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9)?"ON":"OFF");
                  }
          }
          else if(!strncmp(pArray[1],"GETSENSOR",9)) {
 //                 if(pArray[2] != NULL) {
        //                  sensorTime = atoi(pArray[2]);
   //                       sprintf(senddata,"[%s]%s@%u\n",pArray[0],pArray[1],adc_data);
     //             } else {
   //                       sensorTime = 0;
 //                         pArray[2][strlen(pArray[2])-1] = '\0';
                          sprintf(senddata,"[%s]%s@%u@%lu@%d\n",pArray[0],pArray[1],adc_data,systick_sec,50);
       //           }
//                  strcpy(getSensorId,pArray[2]);
          }         
          
          
//          else
          {
//            sprintf(senddata,"[%s]%s\n",pArray[0],pArray[1]);
//            WIFI_send(senddata);
//            printf(senddata);
//            printf("\r\n");
          }
      }
//      if( !(systick_sec % 5 ) && (systick_sec_old != systick_sec))	//10sec
//      {
//              ADC_SoftwareStartConv(ADC1);
//              printf("TEST03 %d\r\n",adc_data);
//              systick_sec_old = systick_sec;
//              if(lcdFlag)
//              {
//                    sprintf(senddata,"[%s]%s#%d\n",pArray[0],pArray[1],adc_data);
//                    WIFI_send(senddata);
//                    printf(senddata);
//                    printf("\r\n");
//              }
//                
//      }
//      if(adc_flag)
//      {
//        adc_data = ADC_GetConversionValue(ADC1);
//        adc_flag = 0;
//      }
//      if((adc_data > 2000) && (fcmCdsFlag == 0))
//      {
//              fcmCdsFlag = 1;
//              sprintf(senddata,"[FCMSEND]LAMP_ON@%d\n",adc_data);
//              WIFI_send(senddata);
//              printf(senddata); printf("\r");
//      }
//      else if((adc_data <= 2000) && (fcmCdsFlag == 1))
//      {
//              fcmCdsFlag = 0;
//              sprintf(senddata,"[FCMSEND]LAMP_OFF@%d\n",adc_data);
//              WIFI_send(senddata);
//              printf(senddata); printf("\r");
//      }
      
      //move(Dot_flag);
  }
}



 

