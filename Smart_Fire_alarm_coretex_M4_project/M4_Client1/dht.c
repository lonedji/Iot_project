#include "stm32f4xx.h"
#include "dht.h"
#include <stdio.h>


extern char rx1Data[50];
extern volatile unsigned char Rflag ; // 수신 인터럽트가 발생했는지를 알아보는 변수
static void Delay_us(const uint32_t Count);
GPIO_InitTypeDef   GPIO_InitStructure;


void DHT11_Init(struct dht11 dh)
{
  struct dht11 dhtInfo; 
  dhtInfo.GPIOx = GPIOB;
  dhtInfo.GPIO_Pin = GPIO_Pin_7;
  
  if( dh.GPIOx == GPIOA)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  else if(dh.GPIOx == GPIOB)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  else if(dh.GPIOx == GPIOC)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = dh.GPIO_Pin;  
  GPIO_Init(dh.GPIOx, &GPIO_InitStructure);  
}

uint8_t DHT_Read(struct dht11 *dht)
{
    uint8_t tmp;
    uint8_t sum = 0;
    uint8_t j = 0, i;
    uint8_t last_state = 1;
    uint16_t counter = 0;
     /*
     * Pull the pin 1 and wait 250 milliseconds
     */

   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Pin = dht->GPIO_Pin;
   GPIO_Init(dht->GPIOx, &GPIO_InitStructure);
    
   GPIO_SetBits(dht->GPIOx, dht->GPIO_Pin); 
   Delay(250);

   dht->data[0] = dht->data[1] = dht->data[2] = dht->data[3] = dht->data[4] = 0;  
	//data배열 0으로 초기화 

   GPIO_ResetBits(dht->GPIOx, dht->GPIO_Pin);   
   Delay(20);
   GPIO_SetBits(dht->GPIOx, dht->GPIO_Pin); 
   Delay_us(30);                   					//40 나노 세컨드 딜레이
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Pin = dht->GPIO_Pin;
   GPIO_Init(dht->GPIOx, &GPIO_InitStructure);

    /* Read the timings */
    for (i = 0; i < DHT_MAXTIMINGS; i++) {             //DHT_MAXTIMINGS = 85
        counter = 0;
        while (1) {
            tmp = GPIO_ReadInputDataBit(dht->GPIOx,dht->GPIO_Pin);
            Delay_us(3);

            if (tmp != last_state)    //last state = 1
                break;

            counter++;
            Delay_us(1);

            if (counter == 255)  //85 times
                break;
        }

        last_state = GPIO_ReadInputDataBit(dht->GPIOx,dht->GPIO_Pin);  //0000 0x00

        if (counter == 255)  //85 times
            break;

        /* Ignore first 3 transitions */
        if ((i >= 4) && (i % 2 == 0)) {
            /* Shove each bit into the storage bytes */
            dht->data[j/8] <<= 1;
            if (counter > DHT_COUNT)  //DHT_COUNT 6
                dht->data[j/8] |= 1;
            j++;
        }
    }

    sum = dht->data[0] + dht->data[1] + dht->data[2] + dht->data[3];
	//sum = check sum

    if ((j >= 40) && (dht->data[4] == (sum & 0xFF)))  //data[4]은 check sum
        return 1;
    return 0;
}
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}

static void Delay_us(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16 * Count); index != 0; index--);
}