#ifndef __WIFI_H_
#define __WIFI_H_

#define SSID "emb10"
#define PASS "iptime000"
#define DST_IP "192.168.10.45"
#define DST_PORT 5000

int connectWiFi();
int wifi_send_cmd (char *str,const char *rstr, unsigned long timeout);
int wifi_wait(const char *str, const char *rstr, unsigned long timeout);
int WIFI_init();
int WIFI_send(char * data);
#endif
