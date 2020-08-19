#include "stm32f4xx.h"
#ifndef __DHT_H__
#define __DHT_H__

#define DHT_COUNT 6
#define DHT_MAXTIMINGS 85
struct dht11 {
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Pin;
  uint8_t data[5];    /* data from sensor store here */
};

void DHT11_Init(struct dht11 dh);
uint8_t DHT_Read(struct dht11 *dht);
static void Delay(const uint32_t Count);
static void Delay_us(const uint32_t Count);

#endif