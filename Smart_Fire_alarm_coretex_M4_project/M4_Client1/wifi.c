/* ========================================================== */
/*	     대한상공회의소 서울기술교육센터 작성자 : KSH	      */
/* ========================================================== */
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_usart.h"
#include "wifi.h"
//#include "uart.h"
//#include "timer.h"

//#define DEBUG
#define TIMEOUT 140L
extern volatile unsigned long systick_count;
extern volatile unsigned char data1;
extern volatile unsigned char Rflag1 ; // 수신 인터럽트가 발생했는지를 알아보는 변수
extern void   TX1_string(char *);


volatile char uart1_rxdata[5][100];

int WIFI_init()
{
	int i;
        printf("Wifi Module Restart!!\r\n");
//	if (!wifi_send_cmd("AT+RST","Ai-Thinker",1L))   //wifi reset
        if (!wifi_send_cmd("AT+RST","OK",TIMEOUT))   //wifi reset
	{
		printf("Module have no response.\r\n");
//		return 0;
	}
	for(i=0;i<5;i++)
	{
		if(connectWiFi())
		{  
		    printf("OK, Connected to WiFi.\r\n");
			break;
		}
	}	
	if(i == 5)
	{
		printf("Wifi connect fail.\r\n");
		return 0;
	}
	if (!wifi_send_cmd("AT+CIFSR","+CIFSR:STAIP,\"192",TIMEOUT))   		//local ip
		;

	if (!wifi_send_cmd("AT+CIPMUX=0","OK",TIMEOUT))   //single mode
		;
	return 1;
}
int connectWiFi()
{
	char cmd[50]={0};

    if (!wifi_send_cmd("AT+CWMODE=1","OK",TIMEOUT))	// timeout : 250 * 0.004 = 1Sec
			;

	sprintf(cmd,"AT+CWJAP=\"%s\",\"%s\"",SSID,PASS);		//AP name, password 
	if (wifi_send_cmd(cmd,"OK",1000L))  { 					//AP 접속
//	if (wifi_send_cmd(cmd,"WIFI CONNECTED",TIMEOUT*2)){
		return 1;
	}
	return 0;
}
int wifi_send_cmd (char *str,const char *rstr, unsigned long timeout)
{
  int ret=0;
  TX1_string(str);
  TX1_string("\r\n");

  ret = wifi_wait(str,rstr, timeout);
 
  if(ret)
//  		printf("[Recv_OK] %s\r\n",str);
	;
  else 
  {
#ifdef  DEBUG
	printf("\n[Recv_NG] wifi_wait(%s): TIMEOUT\r\n",str);

	for(int i=0;i<5;i++)
	{
		printf("RECV TEST %d : %s\r\n",i,uart1_rxdata[i]);
	}
#endif
  }
  return ret;

}
int wifi_wait(const char *str, const char *rstr, unsigned long timeout)
{

	int ucount_recv=0;
	int i=0;
	unsigned char data=0;
	unsigned long tick = systick_count;

	ucount_recv = 0;
	memset((void *)uart1_rxdata,0x00,sizeof(uart1_rxdata));
	while (systick_count - tick < timeout)
	{
		if (Rflag1 == 0)
			continue;  
		else if(Rflag1 == 1)
		{
                  uart1_rxdata[i][ucount_recv] = data =data1;
//			uart1_rxdata[i][ucount_recv] = data = USART_ReceiveData(USART1);
//                    uart1_rxdata[i][ucount_recv] = data =(uint16_t)(USART1->DR & (uint16_t)0x01FF);
			Rflag1 = 0;
			ucount_recv++;
		}
		if (data == '\n')  //CR:0x0d:'\r', LF:0x0a:'\n'
		{
			uart1_rxdata[i][ucount_recv] = 0;
			if (strncmp(( char *)uart1_rxdata[i], rstr, strlen(rstr)) == 0)  //FIND
			{
				if(!strncmp((char *)uart1_rxdata[i],"+CIFSR:STAIP",3))
					printf("Local IP : %s\n",uart1_rxdata[i]+13);
				return 1;
			}

			ucount_recv = 0;
			i++;
			if(i == 5)
				break;
			continue;
		}

	}//close while
	return 0;
}
int WIFI_send(char *data)
{		
	static int connect_flag = 0;

	char cmd[120]={0};
	if(!connect_flag)
	{
		sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%d",DST_IP,DST_PORT);
		if (wifi_send_cmd(cmd,"CONNECT",TIMEOUT*2))   // timeout : 250 * 0.004 = 1Sec
			connect_flag = 1;
	
		else if (wifi_send_cmd(cmd,"ALREADY",TIMEOUT))   // timeout : 250 * 0.004 = 1Sec
			connect_flag = 1;
	}

	sprintf(cmd,"AT+CIPSEND=%d",strlen(data));
           
	if (!wifi_send_cmd(cmd,"OK",TIMEOUT))   //>  //AT+CIPSEND=10>   응답메시지 
		;

//	if (!wifi_send_cmd(data,"SEND OK",1000UL))   
	if (!wifi_send_cmd(data,">",TIMEOUT))   
			connect_flag = 0;	

   return connect_flag;
}
