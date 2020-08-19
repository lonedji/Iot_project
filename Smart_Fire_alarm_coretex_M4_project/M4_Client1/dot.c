#include "stm32f4xx.h"
#include "dot.h"


#define LD1071_DIN      GPIO_Pin_0
#define LD1071_CLK     GPIO_Pin_1
#define LD1071_STB     GPIO_Pin_2
#define LD1071_PWM    GPIO_Pin_5

char CLOS0[]= { 0x7E, 0x7E, 0x02, 0x7E, 0x7E, 0x02, 0x7E, 0x7E};

char CLOS1[]= { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81};
char CLOS2[]={0x7E,0x7E,0x18,0x18,0x18,0x18,0x7E,0x7E};
char CLOS3[]={0xFF,0xFF,0x18,0x18,0x18,0x18,0x18,0x18};
char CLOS4[]={0x0C,0x2C,0x28,0x3F,0x09,0x0F,0x2A,0x3A};
char CLOS5[]={0x0C,0x0D,0x09,0x7F,0x48,0x4E,0x2A,0x3A,};

static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}

void LD1071_Tx_CMD(unsigned int cmd)
{
  unsigned int i = 0;

  GPIO_SetBits(GPIOB, LD1071_STB);           // Strobe 신호를 'high'=> PORTB.2 = 1;

  // 16비트 데이터인 cmd를 D0부터 1비트씩 직렬로 전송한다.
  for( i = 0 ; i< 16 ; i++ )
  {
    if(cmd & 0x0001) GPIO_SetBits(GPIOB, LD1071_DIN);  // PORTB.0 = 1;
    else GPIO_ResetBits(GPIOB, LD1071_DIN);               // PORTB.0 = 0;

    GPIO_ResetBits(GPIOB, LD1071_CLK);                      // PORTB.1 = 0;
    GPIO_SetBits(GPIOB, LD1071_CLK);                          // PORTB.1 = 1;

    cmd = cmd >> 1 ;                                                   // LSB부터 송신
  }

  GPIO_ResetBits(GPIOB, LD1071_CLK);                        // PORTB.1 = 0;
  GPIO_ResetBits(GPIOB, LD1071_STB);                        // PORTB.2 = 0;
}



void LD1071_Tx_Data(unsigned int data)
{
  unsigned int i;

  GPIO_ResetBits(GPIOB, LD1071_STB);                        // PORTB.2 = 0;

  for( i =0 ; i < 16; i++ )
  {
    if( data & 0x0001) GPIO_SetBits(GPIOB, LD1071_DIN); // PORTB.0 = 1;
    else GPIO_ResetBits(GPIOB, LD1071_DIN);               // PORTB.0 = 0;

    GPIO_ResetBits(GPIOB, LD1071_CLK);                      // PORTB.1 = 0;
    GPIO_SetBits(GPIOB, LD1071_CLK);                          // PORTB.1 = 1;

    data = data >> 1;                                                    // LSB부터 송신
  }

  GPIO_ResetBits(GPIOB, LD1071_CLK);                        // PORTB.1 = 0;
  GPIO_SetBits(GPIOB, LD1071_STB);                            // PORTB.2 = 1;
  GPIO_ResetBits(GPIOB, LD1071_STB);                        // PORTB.2 = 0;
}


void LD1071_Global_Latch(void)
{
  unsigned int i = 0;

  GPIO_SetBits(GPIOB, LD1071_STB);                            // PORTB.2 = 1;
  GPIO_ResetBits(GPIOB, LD1071_DIN);                         // PORTB.0 = 0;

  for( i = 0 ; i < 9 ; i++ )
  {
    GPIO_ResetBits(GPIOB, LD1071_CLK);                      // PORTB.1 = 0;
    GPIO_SetBits(GPIOB, LD1071_CLK);                          // PORTB.1 = 1;
  }

  GPIO_ResetBits(GPIOB, LD1071_STB);                        // PORTB.2 = 0;
  GPIO_SetBits(GPIOB, LD1071_STB);                           // PORTB.2 = 1;
  GPIO_ResetBits(GPIOB, LD1071_STB);                        // PORTB.2 = 0;
  GPIO_SetBits(GPIOB, LD1071_STB);                           // PORTB.2 = 1;
  GPIO_ResetBits(GPIOB, LD1071_STB);                        // PORTB.2 = 0;
  GPIO_ResetBits(GPIOB, LD1071_CLK);                        // PORTB.1 = 0;
 }

void LD1071_Tx_Display( unsigned int data)
{
  unsigned int i , mask = 0x0001;

  LD1071_Tx_CMD( 0x0084);
  for(i = 0 ; i <16 ; i++)
  {
    if(data & mask ) LD1071_Tx_Data(0xFFFF);
    else LD1071_Tx_Data(0x0000);
    mask = mask << 1;
  }
  LD1071_Global_Latch();
}

void LD1071_Reset(void)
{
  LD1071_Tx_CMD(0x0001); // Software Reset Command를 전송해 준다
  Delay(100);
}

void Init_Port(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  // COM0 ~ COM7
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
                                             GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // LD1071_DIN, LD1071_CLK, LD1071_STB
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // LD1071_PWM(PB5, TIM3_CH2)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);       //TIM3_CH2

  // 타이머3 PWM 설정
  TIM_TimeBaseStructure.TIM_Prescaler = 0;           //(168Mhz/2)/1 = 84MHz
  TIM_TimeBaseStructure.TIM_Period = 42-1;           //(42(41+1) /84MHz = 0.5us), 2MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 21;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  //타이머3을 동작시킨다.
  TIM_Cmd(TIM3, ENABLE);
}


   
  void d_disp(int state)
{
  
  int i = 0;
  
  if(state == 0)
  {
    for(i = 0; i<8; i++)
    {
      LD1071_Tx_Display(CLOS0[i]);
      GPIO_Write(GPIOC, 0x01 << i);
    }
  }
  else if(state == 1)
  {
    for(i = 0; i<8; i++)
    {
      LD1071_Tx_Display(CLOS1[i]);
      GPIO_Write(GPIOC, 0x01 << i);
    }
  }
  else if(state == 2)
  {
    for(i = 0; i<8; i++)
    {
      LD1071_Tx_Display(CLOS2[i]);
      GPIO_Write(GPIOC, 0x01 << i);
    }
  }
  else if(state == 3)
  {
    for(i = 0; i<8; i++)
    {
      LD1071_Tx_Display(CLOS3[i]);
      GPIO_Write(GPIOC, 0x01 << i);
    }
  }
  else if(state == 4)
  {
    for(i = 0; i<8; i++)
    {
      LD1071_Tx_Display(CLOS4[i]);
      GPIO_Write(GPIOC, 0x01 << i);
    }
  }
  else if(state == 5)
  {
    for(i = 0; i<8; i++)
    {
      LD1071_Tx_Display(CLOS5[i]);
      GPIO_Write(GPIOC, 0x01 << i);
    }
  }
  
}
    
