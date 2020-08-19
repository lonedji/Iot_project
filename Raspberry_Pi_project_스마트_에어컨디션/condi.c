


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <mysql/mysql.h>

static char * host = "localhost";
static char * user = "emb";
static char * pass = "kcci";
static char * dbname = "test";

char device[] = "/dev/ttyACM0";

int fd;
int str_len;
unsigned long baud = 9600;

int main()
{
        MYSQL * conn;
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;


        int res, res2;
        char in_sql[200] = {0};

        char *pArray[3] = {0};
        char ser_buff[100]={0};
        int buffindex = 0;
        char *pToken;
        int temp, humi, state;

        int index, sql_index;
        int flag = 0;
	int cnt=0;

        conn = mysql_init(NULL);

        printf("%s \n", "Raspberry startup");
        fflush(stdout);

        if((fd = serialOpen(device,baud)) <  0)
                exit(1);
        if(wiringPiSetup() == -1)
                return -1;
        if(!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
        {
                fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
                exit(1);
        }
        else
                printf("Connection Successful!\n\n");
        while(1)
        {
                if(serialDataAvail(fd))
                {
                   ser_buff[buffindex++] = serialGetchar(fd);

                   if(ser_buff[buffindex-1] == 'L')
                   {
                      flag = 1;
                      ser_buff[buffindex-1] = '\0';
                      str_len = strlen(ser_buff);
                      printf("test ser_buff = %s\n", ser_buff);
                      pToken = strtok(ser_buff, ":");
                      int i = 0;
                      while(pToken != NULL)
                      {
                        pArray[i]=pToken;
                        if(++i > 4)
                           break;
                        pToken = strtok(NULL, ":");
                 //       printf("ser_buff[%d] = %s \n", i-1, pArray[i-1]);
                      }
                      temp = atoi(pArray[0]);
                      humi = atoi(pArray[1]);
		      state = atoi(pArray[2]);


                      printf("temp=%d,state=%d\n",temp, state);
                      for(int i = 0; i <=str_len; i++)
                        ser_buff[i] = 0;
                      buffindex = 0;
                   }

                if(temp <100 && humi <100)
                {
                        if(flag == 1&&state==1)
                        {
                        sprintf(in_sql, "insert into condi3(id, DATE, START_TIME, TEMP, STATE, END_TIME) values (null, curdate(),curtime(),%d, %d,curtime())",temp, state);
			cnt++;
                       res = mysql_query(conn, in_sql);
                    //    sprintf(in_sql, "update  condi set state = '1', end_time = now(), temp=%d;",temp);
                    //    res = mysql_query(conn, in_sql);
                        printf("res= %d\n", res);
                                if(!res)
                                printf("inserted %lu rows\n", (unsigned long) mysql_affected_rows(conn));
                                else
                                {
                                fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
                                exit(1);
                                }
                        }


                        if(flag == 1&&state==0)
                        {
			res2==mysql_query(conn, "SELECT * FROM `condi3` ORDER BY id DESC LIMIT 1");
			res_ptr=mysql_store_result(conn);
			sqlrow=mysql_fetch_row(res_ptr);
//			cnt=sqlrow[0];
//			printf("cnt=%s", cnt);
                        sprintf(in_sql, "update condi3 set state='0', end_time=now() where id = %s", sqlrow[0]);
                        res = mysql_query(conn, in_sql);
                        printf("res= %d\n", res);
			       if(!res)
                                printf("inserted %lu rows\n", (unsigned long) mysql_affected_rows(conn));
                                else
                                {
                                fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
                                exit(1);
                                }
                        }


                flag = 0;
                }
             }
       }
        mysql_close(conn);
        return EXIT_SUCCESS;
}





























