
#ifndef __DOT_H__
#define __DOT_H__

#include "stm32f4xx.h"

static void Delay(const uint32_t Count);
void LD1071_Tx_CMD(unsigned int cmd);
void LD1071_Tx_Data(unsigned int data);
void LD1071_Global_Latch(void);
void LD1071_Tx_Display( unsigned int data);
void LD1071_Reset(void);
void Init_Port(void);
void d_disp(int state);

#endif 