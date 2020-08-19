/*
 * wifi.h
 *
 * Created: 2018-12-07 오후 12:31:37
 *  Author:  서울기술교육센터 KSH
 */ 


#ifndef WIFI_H_
#define WIFI_H_

#define APSSID "embsystem"
#define APPASSWD "embsystem0"
#define SERVER_IP "192.168.1.46"
#define SERVER_PORT 5000
#define ID "KMS_AVR"
#define PASSWD "PASSWD"
int WIFI_init(void);
int connectWiFi(void);
void WIFI_send(char *data);
int wifi_send_cmd (char *str,const char *rstr, unsigned long timeout);
int wifi_wait(const char *str, const char *rstr, unsigned long timeout);
#endif /* WIFI_H_ */