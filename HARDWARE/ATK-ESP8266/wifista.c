#include "common.h"
#include "stdlib.h"
#include "wifista.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码


void atk_8266_wifista_test(void)
{
	if(atk_8266_send_cmd("AT+CWMODE=1","OK",50))
	{
		printf("设置STA模式失败，准备重启\r\n");
	}
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	while(atk_8266_send_cmd("AT+CWJAP=\"HUAWEI\",\"123456789\"","WIFI GOT IP",600)); //连接WIFI指令
	atk_8266_send_cmd("AT+CIPMUX=0","OK",200);   //0：单连接，1：多连接
	delay_ms(200);
	
	while(atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.43.84\",8086","OK",1000));

	atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
	delay_ms(200);
	printf("%s\r\n","connect success");
	atk_8266_send_cmd("AT+CIPSEND","OK",200); 
	delay_ms(200);	
	}



























