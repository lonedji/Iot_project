/*
 * wifi.c
 *
 * Created: 2018-12-07 오후 12:31:16
 *  Author:  서울기술교육센터 KSH
 */ 
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "wifi.h"
#include "uart.h"
int connect_flag = 0;
extern int rx0flag;
extern int rx1flag;
extern char rx0data[BUFSIZE];
extern char rx1data[BUFSIZE];

extern volatile unsigned long systick_count;

char uart1_rxdata[5][BUFSIZE];
int WIFI_init(void)
{
	int i;
	int ret = 0;
//	if (!wifi_send_cmd("AT+RST","Ai-Thinker",100UL))   //wifi reset
	if (!wifi_send_cmd("AT+RST","OK",250UL))   //250 * 0.004 = 1(1000ms)
	{
		printf("Module have no response.\r\n");
		return 0;
	}

	for(i=0;i<5;i++)
	{
		if(connectWiFi())
		{
//			printf("OK, Connected to WiFi.\r\n");
			ret = 1;
			break;
		}
	}
	if(i == 5)
	{
		printf("Wifi connect fail.\r\n");
		return 0;
	}
	for(i=0;i<5;i++)
	{	
		if(wifi_send_cmd("AT+CIFSR","+CIFSR",250UL))	//local ip
		{
			printf("          Local IP : %s\r",uart1_rxdata[1]+13);
			break;
		}
	}
	if (!wifi_send_cmd("AT+CIPMUX=0","OK",250UL)) //Single TCP connection
		;
	WIFI_send("["ID":"PASSWD"]");
	return ret;
}
int wifi_send_cmd (char *str,const char *rstr, unsigned long timeout)
{
	int ret=0;
	UART1_string(str);
	UART1_string("\r\n");
	ret = wifi_wait(str,rstr, timeout);
	if(ret)
		printf("[Recv_OK] %s\r\n",str);
/*	else
	{
		printf("[Recv_NG] %s: TIMEOUT\r\n",str);

		for(i=0;i<5;i++)
		{
			printf("RECV TEST %d : %s\r\n",i,uart1_rxdata[i]);
		}
	}
*/
	return ret;

}
int wifi_wait(const char *str, const char *rstr, unsigned long timeout)
{
	int i=0;
	int ret=0;
	unsigned long tick = systick_count;
	memset((void *)uart1_rxdata,0x00,sizeof(uart1_rxdata));
//	printf("TEST20 :%lu, %lu, %lu\r\n",systick_count,tick,timeout);
	while (systick_count - tick < timeout)
	{
		if (rx1flag == 0)
			continue;
		else if(rx1flag == 1)
		{
			rx1flag = 0;
			strcpy(uart1_rxdata[i],rx1data);
			if(strncmp(( char *)uart1_rxdata[i], rstr, strlen(rstr)) == 0)  //FIND
			{
				ret = 1;
				break;
			}
			if(++i == 5)
				break;
		}
	}//close while
	return ret;
}
int connectWiFi(void)
{
	int ret = 0;
	//와이파이 잘안되어서 막음
	if (!wifi_send_cmd("AT+CWMODE=1","OK",500UL))//Station Mode	// timeout : 500 * 0.004 = 2sec
	  ;
//	sprintf(cmd,"AT+CWJAP=\"%s\",\"%s\"",SSID,PASS);//AP name, password
	if (wifi_send_cmd("AT+CWJAP=\""APSSID"\",\""APPASSWD"\"","WIFI CONNECTED",750UL)) {
		ret = 1;
	}
	return ret;
}
void WIFI_send(char * data)
{

	char cmd[BUFSIZE]={0};
	if(!connect_flag)
	{
		sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%d",SERVER_IP,SERVER_PORT);

		if (wifi_send_cmd(cmd,"CONNECT",500UL))   // timeout : 250 * 0.004 = 1Sec
			connect_flag = 1;
		
		else if (wifi_send_cmd(cmd,"ALREADY",250UL))   // timeout : 250 * 0.004 = 1Sec
			connect_flag = 1;
		else
		{
			printf(SERVER_IP" connect fail.\r\n");
			connect_flag = 0;
			WIFI_init();
		}
	}
	sprintf(cmd,"AT+CIPSEND=%d",strlen(data));
	if (!wifi_send_cmd(cmd,"OK",250UL))   //>  //AT+CIPSEND=10>   응답메시지
		connect_flag = 0;

	if (!wifi_send_cmd(data,"SEND OK",250UL))
		;
//		connect_flag = 0;

//		connect_flag = 0;;
//	if (!wifi_send_cmd(data,">",250UL))
//		connect_flag = 0;
	
}